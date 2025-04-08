#include "localballmanager.h"


void ball_manager_init(BallListManager* manager) {

    memset(manager, 0,sizeof(BallListManager));
    manager->head = NULL;
    manager->tail = NULL;
    manager->total_count = 0;
    pthread_mutex_init(&manager->mutex_ball, NULL);
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
    printf(COLOR_GREEN "[Success] fd[%d]: '%d' added successfully." COLOR_RESET, owner_id, count);
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

        // target이 이미 리스트에서 빠졌는지 확인
        if (target_prev && target_prev->next != target) continue;

        // head 갱신
        if (manager->head == target) {
            manager->head = target->next;
        }

        // tail 갱신
        if (manager->tail == target) {
            manager->tail = target_prev;
        }

        // 중간 연결 제거
        if (target_prev) {
            target_prev->next = target->next;
        }

        free(target);
        manager->total_count--;
        printf(COLOR_GREEN "[Success] '%d' Deleted (owner: %d)\n" COLOR_RESET, target->data.id, owner_id);
    }


    printInfoBall(manager->head);
}

void delete_ball_by_socket(BallListManager* manager, int socket_fd) {
    BallListNode *cur = manager->head;
    BallListNode *prev = NULL;

    while (cur) {
        BallListNode* next = cur->next;

        if (cur->data.owner_id == socket_fd) {
            if (prev == NULL) {
                manager->head = next;
            } else {
                prev->next = next;
            }

            // tail이 삭제 대상이라면 tail 갱신
            if (manager->tail == cur) {
                manager->tail = prev;
            }

            free(cur);
            manager->total_count--;
        } else {
            prev = cur;
        }

        cur = next;
    }
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
                 cur->data.owner_id,
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

char* serialize_ball_list_all(BallListManager* manager) {
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


int count_ball_by_owner(BallListNode* head, int owner_id) {
    int count = 0;
    while (head) {
        if (head->data.owner_id == owner_id)
            count++;
        head = head->next;
    }
    return count;
}


void log_ball_memory_usage(BallListManager* manager, const char* action, int fd, int count) {
    size_t unit_mem = sizeof(BallListNode);
    size_t delta_mem = unit_mem * count;

    // 현재 전체 공 개수 (이후 기준)
    int now_count = count_ball_by_owner(manager->head, fd);
    size_t now_mem = now_count * unit_mem;

    char details[256];
    snprintf(details, sizeof(details),
        "[Log] Client FD: %d | Action: %s | Count: %d | ΔMemory: %zu bytes | Now: %d balls (Total: %zu bytes)\n",
        fd, action, count, delta_mem, now_count, now_mem);

    log_event(LOG_INFO, "Ball memory usage", fd, count, details);
}

// 핸들러 함수 정의
void handle_add(BallListManager* m, int count, int radius, int owner_id) {
    if (count <= 0) count = 1;
    add_ball(m,count,radius,owner_id);
    log_ball_memory_usage(m, "ADD", owner_id, count);
}

void handle_delete(BallListManager* m, int count, int radius,int owner_id) {
    (void)radius;
    if (count <= 0) count = 1;
    delete_ball(m, count, owner_id);
    log_ball_memory_usage(m, "DEL", owner_id, count);
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



