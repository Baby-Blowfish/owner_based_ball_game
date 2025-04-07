#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <pthread.h>
#include "test_client.h"
#include <signal.h>
#define THREAD_NUM 2

// 전역 변수 정의
extern volatile sig_atomic_t keep_running;

void handle_sigint(int sig) {
    if(sig ==  SIGINT)
    {
        keep_running = 0;
        printf(COLOR_YELLOW "\n[Signal] SIGINT received. Shutting down server..." COLOR_RESET);
    }
}


int main(int argc, char **argv)
{
  struct sockaddr_in servaddr;

  pthread_t tid_render, tid_socket_send, tid_socket_recv;

  SharedContext* arg = manager_init();
  if(arg == NULL)
  {
    perror("malloc() : arg ");
    return -1;
  }

  signal(SIGINT, handle_sigint);

  // 서버 주소
  if (argc < 2) {
    printf("Usage : %s <SERVER_IP>\n", argv[0]);
    return -1;
  }

  // 소켓 초기화
  if ((arg->socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket()");
    return -1;
  }

  // rand() 초기화
  srand(time(NULL)); 

  // 서버 주소 설정
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr);

  // 서버에 연결
  if (connect(arg->socket_fd , (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
      perror("connect()");
      return -1;
  }


  // 키보드 입력 스레드 생성
  if(pthread_create(&tid_render, NULL, render_thread, arg) != 0)
  {
    manager_destroy(arg);
    perror("pthread_create()");
    return -1;
  }

  // 서버 -> 클라이언트 수신용 스래드
  if(pthread_create(&tid_socket_recv, NULL, socket_recv_thread, arg) != 0)
  {
    manager_destroy(arg);
    shutdown(arg->socket_fd, SHUT_RDWR);
    close(arg->socket_fd);
    perror("pthread_create()");
    return -1;
  }

  if(pthread_create(&tid_socket_send, NULL, socket_send_thread, arg) != 0)
  {
    manager_destroy(arg);
    shutdown(arg->socket_fd, SHUT_RDWR);
    close(arg->socket_fd);
    perror("pthread_create()");
    return -1;
  }

  pthread_join(tid_render, NULL);
  pthread_join(tid_socket_recv, NULL);
  pthread_join(tid_socket_send, NULL);
  
  shutdown(arg->socket_fd, SHUT_RDWR); //  소켓 읽기 쓰기 종료 신호
  close(arg->socket_fd);
  manager_destroy(arg);

  printf(COLOR_GREEN "[Client] Shutting down..." COLOR_RESET);
	return 0;
}
