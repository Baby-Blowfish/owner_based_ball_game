
#include "client.h"

volatile sig_atomic_t keep_running = 1;


SharedContext* manager_init() {
    SharedContext* arg = malloc(sizeof(SharedContext));
    if (!arg) {
        perror("malloc() : SharedContext");
        return NULL;
    }

    arg->framebuffer = malloc(sizeof(dev_fb));
    arg->ball_list_manager = malloc(sizeof(BallListManager));

    if (!arg->framebuffer || !arg->ball_list_manager) {
        perror("malloc() : internal");
        free(arg->framebuffer); 
        free(arg->ball_list_manager); 
        free(arg);
        return NULL;
    }

    ball_manager_init(arg->ball_list_manager);

    // framebuffer 초기화
    if(fb_init(arg->framebuffer) > 0)
    {
        perror("framebuffer init failed");
        return NULL;
    }
    pthread_mutex_init(&arg->mutex_ball, NULL);

    return arg;
}

void manager_destroy(SharedContext* arg) {
    if (!arg) return;

    ball_manager_destroy(arg->ball_list_manager);

    pthread_mutex_destroy(&arg->mutex_ball);

    if (arg->framebuffer) {
        fb_close(arg->framebuffer);
        free(arg->framebuffer);
    }
    free(arg);
    printf(COLOR_GREEN "Freed memory of the SharedContext." COLOR_RESET);
}



// 렌더링 스레드
void* render_thread(void* arg) {
    SharedContext* ctx = (SharedContext*)arg;

    while (keep_running) {
        usleep(1000000 / 30); // 30 FPS
        
        pthread_mutex_lock(&ctx->ball_list_manager->mutex_ball);
        
        BallListNode* local_copy = deep_copy_ball_list(ctx->ball_list_manager->head);
        fb_fillScr(ctx->framebuffer, 0, 0, 0);
        draw_ball_list(ctx->framebuffer, local_copy); // 복사된 리스트 출력
        draw_command_guide(ctx->framebuffer);
        pthread_mutex_unlock(&ctx->ball_list_manager->mutex_ball);

        // 복사한 리스트 해제
        BallListNode* tmp;
        while (local_copy) {
            tmp = local_copy;
            local_copy = local_copy->next;
            free(tmp);
        }
    }
    printf(COLOR_GREEN "[Client] Render Thread Shutting down..." COLOR_RESET);
    return NULL;
}


void* socket_recv_thread(void* arg) {
    
    SharedContext* ctx = (SharedContext*)arg;

    char recv_buf[BUFSIZ];

    while(keep_running)
    {
        memset(recv_buf, 0, sizeof(recv_buf));
        int len = recv(ctx->socket_fd, recv_buf, sizeof(recv_buf)-1, 0);
        recv_buf[len] = '\0';
        if (len <= 0) {
            perror("recv()");
            printf("[Client] Server disconnected (fd=%d)\n", ctx->socket_fd);
            keep_running = 0;
            break;
        }

        if (len >= BUFSIZ - 1) {
            printf(COLOR_RED "[Warning] Received buffer is full. Data may be truncated!\n" COLOR_RESET);
        }

        pthread_mutex_lock(&ctx->ball_list_manager->mutex_ball);
        updateBallListFromSerialized(ctx->ball_list_manager, recv_buf, ctx->framebuffer->vinfo.xres, ctx->framebuffer->vinfo.yres);
        pthread_mutex_unlock(&ctx->ball_list_manager->mutex_ball);
    }
    printf(COLOR_GREEN "[Client] Socket Recv Thread Shutting down..." COLOR_RESET);
    pthread_exit(NULL);
}

// 클라이언트 ->  서버 송신 스레드 : 입력  명령 받기 및 전송
void* socket_send_thread(void* arg) {

    SharedContext *ctx = (SharedContext*)arg;

    char input[MAX_INPUT];

    fd_set read_fds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000; // 100ms

    while (keep_running) {

        // printf("\n[ Command Guide ]\n"
        // "- Create a ball           : a        (e.g., a)\n"
        // "- Delete a ball           : d        (e.g., d)\n"
        // "- Increase speed          : w\n"
        // "- Decrease speed          : s\n"
        // "- Create multiple balls   : a:<count> (e.g., a:3)\n"
        // "- Delete multiple balls   : d:<count> (e.g., d:2)\n"
        // "- Exit the program        : x\n"
        // ">> Enter command: ");

        // fflush(stdout);

        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        if (select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &tv) > 0) {
            if (FD_ISSET(STDIN_FILENO, &read_fds)) {
                if (fgets(input, sizeof(input), stdin) == NULL) continue;
                input[strcspn(input, "\n")] = '\0';

                send(ctx->socket_fd, input, strlen(input), 0);

                if(strcmp(input, "x") == 0) {
                    break;
                }
            }
        }
    }
    send(ctx->socket_fd, "x", 1, 0);
    printf(COLOR_GREEN "[Client] Socket Send Thread Shutting down..." COLOR_RESET);
    pthread_exit(NULL);
}





