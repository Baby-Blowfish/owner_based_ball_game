#include "localball.h"

LogicalBall create_logical_ball(int id, int radius, int owner_id) {
    LogicalBall b;
    b.id = id;
    b.owner_id = owner_id;
    b.radius = radius;

    // 논리 좌표계 범위 (0~1000) 내에서 랜덤 위치 생성
    b.x = rand() % (1000 - 2 * radius) + radius;
    b.y = rand() % (1000 - 2 * radius) + radius;

    // 이동 속도 설정 (정수 대신 실수값)
    b.dx = (rand() % 2 == 0) ? -2.5f : 2.5f;
    b.dy = (rand() % 2 == 0) ? -2.5f : 2.5f;

    b.color = get_color_by_owner(owner_id);

    return b;
}


RGBColor get_color_by_owner(int owner_id) {
    switch (owner_id % 5) {
        case 0: return (RGBColor){255, 0, 0};   // 빨강
        case 1: return (RGBColor){0, 255, 0};   // 초록
        case 2: return (RGBColor){0, 0, 255};   // 파랑
        case 3: return (RGBColor){255, 255, 0}; // 노랑
        case 4: return (RGBColor){255, 0, 255}; // 자홍
        default: return (RGBColor){128, 128, 128}; // 회색 (fallback)
    }
}


void move_logical_ball(LogicalBall* b) {
    b->x += b->dx;
    b->y += b->dy;

    // 좌우 경계 반사 처리
    if (b->x <= b->radius || b->x >= (1000.0f - b->radius)) {
        b->dx *= -1;
        b->x += b->dx;  // 반사 후 한 칸 이동
    }

    // 상하 경계 반사 처리
    if (b->y <= b->radius || b->y >= (1000.0f - b->radius)) {
        b->dy *= -1;
        b->y += b->dy;
    }
}
