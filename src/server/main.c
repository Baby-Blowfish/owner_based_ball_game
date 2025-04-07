#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include "server.h"

#define MAX_EVENTS 64

extern TaskQueue* global_task_queue;
extern volatile sig_atomic_t keep_running;

void handle_sigint(int sig) {
    if(sig ==  SIGINT)
    {
        keep_running = 0;
        pthread_cond_broadcast(&global_task_queue->cond); 
        printf(COLOR_YELLOW "\n[Signal] SIGINT received. Shutting down server..." COLOR_RESET);
    }
}

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(void)
{
    int ssock, csock;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clen = sizeof(cliaddr);
    pthread_t workers[NUM_WORKERS];  // woker Pool 생성
    pthread_t cycle_broadcast_id;
    

    signal(SIGINT, handle_sigint); // graceful shutdown 지원

    SharedContext* arg = manager_init();
    if(arg == NULL)
    {
        perror("manager_init()");
        return -1;
    }

       ssock = socket(AF_INET, SOCK_STREAM, 0);
    if (ssock < 0) {
        perror("socket()");
        exit(1);
    }

    set_nonblocking(ssock);

    // 2. 포트 재사용 설정
    int optval = 1;
    setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // 2. 서버 주소 구조체 초기화 및 설정
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;                 // IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // 모든 IP에서의 접속 허용
    servaddr.sin_port = htons(SERVER_PORT);           // 포트 번호

    // 3. IP/포트 바인딩
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        perror("bind() :");
        return -1;
    }

    // 4. 클라이언트 연결 대기 상태 진입
    if((listen(ssock, 8) < 0))
    {
        perror("listen() : ");
        return -1;
    }

    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        return -1;
    }

    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = ssock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, ssock, &ev);
    arg->epoll_fd = epfd;

    pthread_create(&cycle_broadcast_id, NULL, cycle_broadcast_ball_state, (void*)arg);

    for (int i = 0; i < NUM_WORKERS; ++i) {
        if (pthread_create(&workers[i], NULL, worker_thread, (void*)arg) != 0) {
            perror("pthread_create");
            return -1;
        }
    }


    printf(COLOR_BLUE "[Server] Listening on port %d..."COLOR_RESET, SERVER_PORT);

    while (keep_running) {
        int nready = epoll_wait(epfd, events, MAX_EVENTS, 1000);
        if (nready == -1) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nready; i++) {
            int fd = events[i].data.fd;

            if (fd == ssock) {
                csock = accept(ssock, (struct sockaddr*)&cliaddr, &clen);
                if (csock >= 0) {
                    set_nonblocking(csock);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = csock;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, csock, &ev);

                    add_client(arg->client_list_manager, csock, cliaddr);
                    pthread_mutex_lock(&arg->client_list_manager->mutex_client);
                    arg->client_list_manager->client_count++;
                    pthread_mutex_unlock(&arg->client_list_manager->mutex_client);
                    log_client_connect(csock, &cliaddr);    
                }
            } else if (events[i].events & EPOLLIN) {
                char buf[BUFSIZ];
                memset(buf, 0, sizeof(buf));
                int len = recv(fd, buf, sizeof(buf), 0);

                if (len <= 0) {
                    printf("[Server] Client disconnected (fd=%d)\n", fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);

                    ClientNode* removed = remove_client_by_socket(fd, &arg->client_list_manager->head, &arg->client_list_manager->tail);
                    if (removed) {
                        shutdown(removed->ctx.csock, SHUT_RDWR);
                        close(removed->ctx.csock);
                        free(removed);
                    }
                } else {
                    printf("[Server] Received from fd %d: %s\n", fd, buf);
                    // enqueue to task queue
                    Task task;
                    task.fd = fd;
                    memcpy(task.data, buf, len);
                    task.length = len;
                    task_queue_push(arg->task_queue, task);  
                    printf("[Server] Enqueued task for fd %d\n", fd);
                }
            }
        }
    }

    printf("[Server] Main Thread Shutting down...\n");
    close(ssock);
    close(epfd);

    // 남은 클라이언트 소켓 제거
    pthread_mutex_lock(&arg->client_list_manager->mutex_client);
    ClientNode* curr = arg->client_list_manager->head;
    while (curr) {
        close(curr->ctx.csock);
        curr = curr->next;
    }
    pthread_mutex_unlock(&arg->client_list_manager->mutex_client);

    // 워커 스레드 종료 대기
    for (int i = 0; i < NUM_WORKERS; ++i) {
        pthread_join(workers[i], NULL);
    }
    pthread_join(cycle_broadcast_id, NULL);
    manager_destroy(arg);

    return 0;
}

