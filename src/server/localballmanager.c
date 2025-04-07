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

void add_ball(BallListManager* manager, int count, int radius, int owner_id) {
    for (int i = 0; i < count; i++) {
        LogicalBall b = create_logical_ball(manager->total_count++,radius, owner_id);
        manager->head = appendBall(manager->head, &manager->tail, b);
    }
    printInfoBall(manager->head);
}

void delete_ball(BallListManager* manager, int count, int owner_id) {
    BallListNode *cur = manager->head;
    BallListNode *prev = NULL;

    // 삭제 대상 후보 저장용 (역순으로)
    BallListNode* candidates[count];
    BallListNode* candidates_prev[count];
    int found = 0;

    while (cur) {
        if (cur->data.owner_id == owner_id) {
            if (found < count) {
                candidates[found] = cur;
                candidates_prev[found] = prev;
                found++;
            } else {
                // 가장 오래된 후보는 버리고 새로 push
                for (int i = 0; i < count - 1; ++i) {
                    candidates[i] = candidates[i+1];
                    candidates_prev[i] = candidates_prev[i+1];
                }
                candidates[count-1] = cur;
                candidates_prev[count-1] = prev;
            }
        }
        prev = cur;
        cur = cur->next;
    }

    if (found == 0) {
        printf(COLOR_BLUE "No balls found for owner_id %d\n" COLOR_RESET, owner_id);
        return;
    }

    for (int i = found - 1; i >= 0; --i) {
        BallListNode* target = candidates[i];
        BallListNode* target_prev = candidates_prev[i];

        if (target_prev == NULL) {
            manager->head = target->next;
        } else {
            target_prev->next = target->next;
        }

        if (manager->tail == target)
            manager->tail = target_prev;

        printf(COLOR_GREEN "[Success] '%d' Deleted (owner: %d)\n" COLOR_RESET, target->data.id, owner_id);
        free(target);
        manager->total_count--;
    }

    printInfoBall(manager->head);
}


void move_all_ball(BallListManager* manager) {
    moveBallList(manager->head);
}

char* serialize_ball_list(BallListManager* manager, int owner_id) {
    BallListNode* cur = manager->head;
    char* buffer = (char*)malloc(8192);
    buffer[0] = '\0';

    while (cur) {
        if(cur->data.owner_id == owner_id) {
            char temp[256];
            snprintf(temp, sizeof(temp), "%d,%.2f,%.2f,%d,%d,%d,%hhu,%hhu,%hhu|",
                 cur->data.id,
                 cur->data.x, cur->data.y,
                 cur->data.dx, cur->data.dy,
                 cur->data.radius,
                 cur->data.color.r, cur->data.color.g, cur->data.color.b);

            strcat(buffer, temp);
        }
        cur = cur->next;
    }

    return buffer; // 호출자가 free 해야 함
}


// 핸들러 함수 정의
void handle_add(BallListManager* m, int count, int radius, int owner_id) {
    if (count <= 0) count = 1;
    add_ball(m,count,radius,owner_id);
}

void handle_delete(BallListManager* m, int count, int radius,int owner_id) {
    (void)radius;
    if (count <= 0) count = 1;
    delete_ball(m, count, owner_id);
}

void handle_speed_up(BallListManager* m, int count, int radius, int owner_id) {
    (void)count;
    (void)radius;
    speedUpBalls(m->head, owner_id);
}

void handle_speed_down(BallListManager* m, int count, int radius, int owner_id) {
    (void)count;
    (void)radius;
    speedDownBalls(m->head, owner_id);
}

CommandEntry command_table[] = {
    {CMD_ADD, handle_add},
    {CMD_DEL, handle_delete},
    {CMD_SPEED_UP, handle_speed_up},
    {CMD_SPEED_DOWN, handle_speed_down}
};

void dispatch_command(BallListManager* m, char cmd, int count, int radius, int owner_id) {
    for (size_t i = 0; i < sizeof(command_table)/sizeof(CommandEntry); ++i) {
        if (command_table[i].cmd == cmd) {
            command_table[i].handler(m, count, radius, owner_id);
            return;
        }
    }
    printf("[Server] Unknown command: %c\n", cmd);
}



