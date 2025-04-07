#include "localballmanager.h"

// 로깅 함수 선언
extern void log_ball_memory_usage(const char* action, int fd, int count);

void ball_manager_init(BallListManager* manager) {

    memset(manager, 0,sizeof(BallListManager));
    manager->head = NULL;
    manager->tail = NULL;
    manager->total_count = 0;
    pthread_mutex_init(&manager->mutex_ball, NULL);

    add_ball(manager,START_BALL_COUNT,START_BALL_RADIUS);
}

void ball_manager_destroy(BallListManager* manager) {
    pthread_mutex_destroy(&manager->mutex_ball);
    printf( COLOR_GREEN "Mutex 'mutex_ball' has been destroyed." COLOR_RESET);
    freeBallList(&manager->head);
}

void add_ball(BallListManager* manager, int count, int radius) {
    for (int i = 0; i < count; i++) {
        LogicalBall b = create_logical_ball(manager->total_count++,radius);
        manager->head = appendBall(manager->head, &manager->tail, b);
    }
    printInfoBall(manager->head);
}

void delete_ball(BallListManager* manager, int count) {
    for (int i = 0; i < count; i++) {
        if (deleteLastBall(&manager->head, &manager->tail) != NULL)
            manager->total_count--;
        else printf(COLOR_BLUE  "There are no balls." COLOR_RESET);
    }
    printInfoBall(manager->head);
}

void move_all_ball(BallListManager* manager) {
    moveBallList(manager->head);
}

char* serialize_ball_list(BallListManager* manager) {
    BallListNode* cur = manager->head;
    char* buffer = (char*)malloc(8192);
    buffer[0] = '\0';

    while (cur) {
        char temp[256];
        snprintf(temp, sizeof(temp), "%d,%.2f,%.2f,%d,%d,%d,%hhu,%hhu,%hhu|",
                 cur->data.id,
                 cur->data.x, cur->data.y,
                 cur->data.dx, cur->data.dy,
                 cur->data.radius,
                 cur->data.color.r, cur->data.color.g, cur->data.color.b);

        strcat(buffer, temp);
        cur = cur->next;
    }

    return buffer; // 호출자가 free 해야 함
}


// 핸들러 함수 정의
void handle_add(BallListManager* m, int count, int radius) {
    if (count <= 0) count = 1;
    add_ball(m,count,radius);
}

void handle_delete(BallListManager* m, int count, int radius) {
    (void)radius;
    if (count <= 0) count = 1;
    delete_ball(m, count);
}

void handle_speed_up(BallListManager* m, int count, int radius) {
    (void)count;
    (void)radius;
    speedUpBalls(m->head);
}

void handle_speed_down(BallListManager* m, int count, int radius) {
    (void)count;
    (void)radius;
    slowDownBalls(m->head);
}

CommandEntry command_table[] = {
    {CMD_ADD, handle_add},
    {CMD_DEL, handle_delete},
    {CMD_SPEED_UP, handle_speed_up},
    {CMD_SPEED_DOWN, handle_speed_down}
};

void dispatch_command(char cmd, int count, int radius, BallListManager* m) {
    for (size_t i = 0; i < sizeof(command_table)/sizeof(CommandEntry); ++i) {
        if (command_table[i].cmd == cmd) {
            command_table[i].handler(m, count, radius);
            return;
        }
    }
    printf("[Server] Unknown command: %c\n", cmd);
}



