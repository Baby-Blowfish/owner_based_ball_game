#include "screenballmanager.h"

void draw_ball(dev_fb* fb, ScreenBall* b) {
    pixel center = fb_toPixel(b->x, b->y);
    fb_drawFilledCircle(fb, center, b->color.r, b->color.g, b->color.b);
}

void draw_ball_list(dev_fb* fb, BallListNode* head) {
    BallListNode* cur = head;
    while (cur != NULL) {
        draw_ball(fb,&(cur->data));
        cur = cur->next;
    }
}

void draw_command_guide(dev_fb* fb) {
    short text_height = 12;
    int padding = 5;

    pixel bg_start = {0, fb->vinfo.yres - text_height - padding};
    int box_height = text_height + padding * 2;

    // (1) 텍스트 박스 영역 지우기 (흑색)
    fb_fillBox(fb, bg_start, fb->vinfo.xres, box_height, 0, 0, 0);

    // (2) 텍스트 출력
    pixel cursor;
    cursor.x = 10;
    cursor.y = fb->vinfo.yres - text_height - padding;

    const char* cmd_guide =
        "a: Create | d: Delete | w/s: Speed | a:3 d:2 | x: Exit";

    fb_printStr(fb, cmd_guide, &cursor, text_height, 255, 255, 255); // 흰색
}


void ball_manager_init(BallListManager* manager) {

    memset(manager, 0,sizeof(BallListManager));
    manager->head = NULL;
    manager->tail = NULL;
    pthread_mutex_init(&manager->mutex_ball, NULL);
}

void ball_manager_destroy(BallListManager* manager) {
    pthread_mutex_destroy(&manager->mutex_ball);
    printf( COLOR_GREEN "Mutex 'mutex_ball' has been destroyed." COLOR_RESET);
    freeBallList(&manager->head);
}

int logic_to_pixel(float logic, int screen_size) {
    return (int)((logic / 1000.0f) * screen_size);
}

/**
 * @brief 논리 좌표계(LogicalBall)를 화면 좌표계(ScreenBall)로 변환
 * 
 * @param l 서버에서 받은 논리 공 정보
 * @param width 클라이언트 해상도 너비
 * @param height 클라이언트 해상도 높이
 * @return 변환된 화면 좌표계 공 객체
 */
ScreenBall logical_to_screen_ball(LogicalBall l, int width, int height) {
    ScreenBall s;
    s.id = l.id;
    s.x = logic_to_pixel(l.x, width);
    s.y = logic_to_pixel(l.y, height);
    s.dx = l.dx;
    s.dy = l.dy;

    int min_res = (width < height) ? width : height;
    s.radius = logic_to_pixel(l.radius, min_res);
    if(s.radius <= min_res) s.radius = 5;
    
    s.color = l.color; // RGB 복사
    return s;
}


void updateBallListFromSerialized(BallListManager* manager, const char* str, int width, int height) {

    delete_all_ball(&manager->head, &manager->tail, &manager->total_count); // 전체 삭제

    char* input = strdup(str);
    if(input == NULL) return;   // 복사할 공간이 없을때

    char* token = strtok(input, "|");
    while ((token != NULL) && (strlen(token) > 0)) {
        LogicalBall l;
        sscanf(token, "%d,%f,%f,%d,%d,%d,%hhu,%hhu,%hhu",
               &l.id,
               &l.x, &l.y,
               &l.dx, &l.dy,
               &l.radius,
               &l.color.r, &l.color.g, &l.color.b);
        ScreenBall ball = logical_to_screen_ball(l, width, height);
        manager->head = appendBall(manager->head, &manager->tail, ball);
        manager->total_count++;
        token = strtok(NULL, "|");
    }
    // printf("File: %s | Line: %d | Function: %s | Message: %s\033[0m\n", __FILE__, __LINE__, __FUNCTION__,"ohh" );
    free(input);
}

void add_ball(BallListManager* manager, int count, int width, int height, int radius) {
    for (int i = 0; i < count; i++) {
        ScreenBall b = create_screen_ball(manager->total_count++, width, height, radius);
        manager->head = appendBall(manager->head, &manager->tail, b);
    }
}

void delete_ball(BallListManager* manager, int count) {
    for (int i = 0; i < count; i++) {
        if (deleteLastBall(&manager->head, &manager->tail) != NULL)
            manager->total_count--;
        else printf(COLOR_BLUE  "There are no balls." COLOR_RESET);
    }
}
