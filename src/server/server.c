#include "server.h"

// 전역 변수 정의
TaskQueue* global_task_queue = NULL;
volatile sig_atomic_t keep_running = 1;

/**
 * @brief 공 생성/삭제 이벤트를 로깅하는 함수
 * 
 * @param action "생성" 또는 "삭제"와 같은 액션 설명
 * @param fd 클라이언트 소켓 파일 디스크립터
 * @param count 생성/삭제된 공의 개수
 */

SharedContext* manager_init() {
    // 로그 파일 초기화 (파일 내용 지우기)
    FILE* log_file = fopen("logs/ball_operations.log", "w");
    if (log_file) {
        fprintf(log_file, "=== Ball Operations Log (Started at %s) ===\n", 
                __DATE__ " " __TIME__);
        fclose(log_file);
        printf(COLOR_GREEN "[Log] Log file initialized." COLOR_RESET);
    }

    SharedContext* arg = malloc(sizeof(SharedContext));
    if (!arg) {
        perror("malloc() : SharedContext");
        return NULL;
    }

    arg->ball_list_manager = malloc(sizeof(BallListManager));
    arg->client_list_manager = malloc(sizeof(ClientListManager));
    arg->task_queue = malloc(sizeof(TaskQueue));


    if (!arg->ball_list_manager || !arg->client_list_manager || !arg->task_queue) {
        perror("malloc() : internal");
        free(arg->ball_list_manager);
        free(arg->client_list_manager);
        free(arg->task_queue); 
        free(arg);
        return NULL;
    }

    ball_manager_init(arg->ball_list_manager);
    client_list_manager_init(arg->client_list_manager);
    task_queue_init(arg->task_queue);
    
    // 전역 변수 초기화
    global_task_queue = arg->task_queue;

    return arg;
}

void manager_destroy(SharedContext* arg) {
    if (!arg) return;
    ball_manager_destroy(arg->ball_list_manager);
    client_list_manager_destroy(arg->client_list_manager);
    task_queue_destroy(arg->task_queue);
    free(arg);
    printf(COLOR_GREEN "Freed memory of the SharedContext." COLOR_RESET);
}

// 명령 파싱 함수: a:3:30, d:2, w, s, x 등 다양한 형태 지원
char parseCommand(const char* cmdStr, int* ball_count, int* radius) {
    if (!cmdStr || !ball_count || !radius) return 0;

    char op;
    int n1 = 0, n2 = 0;

    // 전체 파라미터 있는 경우: a:3:30 (공 3개, 반지름 30)
    if (sscanf(cmdStr, "%c:%d:%d", &op, &n1, &n2) == 3) {
        *ball_count = n1;
        *radius = n2;
        return op;
    }

    // 기본 형태: a:3
    if (sscanf(cmdStr, "%c:%d", &op, &n1) == 2) {
        *ball_count = n1;
        *radius = START_BALL_RADIUS; // 기본값
        return op;
    }

    // 단일 명령어: a, d, w, s, x
    if (strlen(cmdStr) == 1) {
        *ball_count = 0;
        *radius = START_BALL_RADIUS; // 기본값
        return cmdStr[0];
    }

    return 0;
}

// 공 리스트를 문자열로 직렬화하여 모든 클라이언트에 전송
void broadcast_ball_state(ClientListManager* client_mgr, BallListManager* ball_mgr) {
   
    pthread_mutex_lock(&ball_mgr->mutex_ball);
    char* buffer  = serialize_ball_list(ball_mgr);
    pthread_mutex_unlock(&ball_mgr->mutex_ball);

    if(!buffer) return;
    
    pthread_mutex_lock(&client_mgr->mutex_client);
    ClientNode* curr = client_mgr->head;
    while (curr) {
        send(curr->ctx.csock, buffer, strlen(buffer), 0);
        curr = curr->next;
    }
    pthread_mutex_unlock(&client_mgr->mutex_client);

    free(buffer);
}


void log_client_connect(int fd, struct sockaddr_in* cliaddr) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(cliaddr->sin_addr), ip, INET_ADDRSTRLEN);
    int port = ntohs(cliaddr->sin_port);
    char details[100];
    snprintf(details, sizeof(details), "IP: %s, Port: %d", ip, port);
    log_event(LOG_INFO, "New client connected", fd, 0, details);
}

// 클라이언트 연결 종료 로깅 함수
void log_client_disconnect(int fd, const char* reason) {
    char details[100];
    snprintf(details, sizeof(details), "Reason: %s", reason);
    log_event(LOG_INFO, "Client disconnected", fd, 0, details);
}

// 공 생성/삭제 이벤트 로깅 함수
void log_ball_memory_usage(const char* action, int fd, int count) {
    size_t mem = sizeof(BallListNode) * count;
    char details[100];
    sprintf(details, "[Log] Client FD: %d, Action: %s, Count: %d, Memory: %zu bytes\n",
           fd, action, count, mem);
    log_event(LOG_INFO, "Ball memory usage", fd, count, details);
}

// Worker thread 루프
void* worker_thread(void* arg) {

    int count = 0, radius = 0;
    SharedContext* ctx = (SharedContext*)arg;

    while (keep_running) {

        Task task = task_queue_pop(ctx->task_queue);
        task.data[task.length] = '\0'; // 수신한 데이터 null-terminate 보장

        printf(COLOR_BLUE "[Worker] Processing task from fd %d: %s" COLOR_RESET, task.fd, task.data);

        char cmd = parseCommand(task.data, &count, &radius);

        if ( cmd == 0)
        {
            char error_msg[] = "Invalid command format\n";
            send(task.fd, error_msg, strlen(error_msg), 0);
            continue;
        }

        if(cmd == CMD_EXIT)
        {
            // 클라이언트 종료 처리
            printf(COLOR_YELLOW "[Server] Client requested disconnect (fd=%d)" COLOR_RESET, task.fd);
            log_client_disconnect(task.fd, "Client requested disconnect");

            pthread_mutex_lock(&ctx->client_list_manager->mutex_client);
            ClientNode* removed = remove_client_by_socket(task.fd, &ctx->client_list_manager->head, &ctx->client_list_manager->tail);
            if (removed) {
                ctx->client_list_manager->client_count--;
                epoll_ctl(ctx->epoll_fd, EPOLL_CTL_DEL, task.fd, NULL);
                shutdown(removed->ctx.csock, SHUT_RDWR);
                close(removed->ctx.csock);
                free(removed);
            }
            pthread_mutex_unlock(&ctx->client_list_manager->mutex_client);

            continue; // 다음 작업 처리
        }

        // 다른 명령이 들어왔을때 처리 필요함!
        switch (cmd) {
            case CMD_ADD:
            case CMD_DEL:
            case CMD_SPEED_UP:
            case CMD_SPEED_DOWN:
                pthread_mutex_lock(&ctx->ball_list_manager->mutex_ball);
                dispatch_command(cmd, count, radius, ctx->ball_list_manager);
                pthread_mutex_unlock(&ctx->ball_list_manager->mutex_ball);
                
                if(cmd == CMD_ADD)
                {
                    int log_count = (count <= 0) ? 1 : count;
                    log_ball_memory_usage("ADD", task.fd, log_count);
                }
                else if(cmd == CMD_DEL)
                {
                    int log_count = (count <= 0) ? 1 : count;
                    log_ball_memory_usage("DEL", task.fd, log_count);
                }
                break;
            default:
                {
                    char unknown_msg[] = "Unknown command\n";
                    send(task.fd, unknown_msg, strlen(unknown_msg), 0);
                    continue;
                }
        }

        char response[64];
        snprintf(response, sizeof(response), "OK %c : %d\n", cmd, count);
        send(task.fd, response, strlen(response), 0);

        broadcast_ball_state(ctx->client_list_manager, ctx->ball_list_manager);

    }
    printf(COLOR_GREEN "[Worker] Thread Shutting down..." COLOR_RESET);
    return NULL;
}

void* cycle_broadcast_ball_state(void* arg) {
    SharedContext* ctx = (SharedContext*)arg;

    while (keep_running) {
        usleep(30000); // 약 33 FPS
        pthread_mutex_lock(&ctx->ball_list_manager->mutex_ball);
        move_all_ball(ctx->ball_list_manager);
        pthread_mutex_unlock(&ctx->ball_list_manager->mutex_ball);
        broadcast_ball_state(ctx->client_list_manager, ctx->ball_list_manager);
    }
    printf(COLOR_GREEN "[Cycle Broadcast] Thread Shutting down..." COLOR_RESET);
    return NULL;
}
