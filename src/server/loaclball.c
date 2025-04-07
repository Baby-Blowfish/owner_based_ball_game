#include "localball.h"

LogicalBall create_logical_ball(int id, int radius) {
    LogicalBall b;
    b.id = id;
    b.radius = radius;

    // 논리 좌표계 범위 (0~1000) 내에서 랜덤 위치 생성
    b.x = rand() % (1000 - 2 * radius) + radius;
    b.y = rand() % (1000 - 2 * radius) + radius;

    // 이동 속도 설정 (정수 대신 실수값)
    b.dx = (rand() % 2 == 0) ? -2.5f : 2.5f;
    b.dy = (rand() % 2 == 0) ? -2.5f : 2.5f;

    // 랜덤 색상 설정
    b.color.r = rand() % 256;
    b.color.g = rand() % 256;
    b.color.b = rand() % 256;

    return b;
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
