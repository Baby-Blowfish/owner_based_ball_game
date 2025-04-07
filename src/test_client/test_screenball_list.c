#include "test_screenball_list.h"


BallListNode* createNode(ScreenBall ball) {

  BallListNode *newnode = (BallListNode*)malloc(sizeof(BallListNode));
  if (!newnode) {
      return NULL;
  }

  memset(newnode, 0, sizeof(BallListNode));

  newnode->data = ball;
  newnode->next = NULL;

    return newnode;
}

BallListNode* appendBall(BallListNode* head, BallListNode** tail, ScreenBall ball)
{
  BallListNode* newnode = NULL;
  if((newnode =createNode(ball)) == NULL)
    perror(COLOR_RED "[Error] Memory allocation failed" COLOR_RESET);

  if (head == NULL) {
    head = newnode;
    *tail = newnode;
  } else {
    (*tail)->next = newnode;
    *tail = newnode;
  }

  return head;

}


BallListNode* deleteLastBall(BallListNode** head, BallListNode** tail) {
    if (*head == NULL) return NULL;

    BallListNode* removed = NULL;

    // 한 개만 있을 때
    if (*head == *tail) {
        removed = *head;
        *head = NULL;
        *tail = NULL;
    } else {
        BallListNode* cur = *head;
        while (cur->next != *tail) {
            cur = cur->next;
        }
        removed = *tail;
        cur->next = NULL;
        *tail = cur;
    }

    printf(COLOR_GREEN "[Success] '%d' Deleted successfully." COLOR_RESET, removed->data.id);

    free(removed);
    return NULL;
}

/**
 * @brief 공 리스트 전체 삭제 (모든 공 제거)
 *
 * @param manager BallListManager 포인터
 */
void delete_all_ball(BallListNode** head, BallListNode** tail, int* count) {
    BallListNode* cur = *head;
    while (cur != NULL) {
        BallListNode* temp = cur;
        cur = cur->next;
        free(temp);
        (*count)--;
    }

    *head = NULL;
    *tail = NULL;
}



BallListNode* deep_copy_ball_list(BallListNode* head) {
    BallListNode* new_head = NULL;
    BallListNode* new_tail = NULL;

    BallListNode* current = head;
    while (current) {
        new_head = appendBall(new_head, &new_tail, current->data);
        current = current->next;
    }
    return new_head;
}


void freeBallList(BallListNode** head) {
    BallListNode* cur = *head;
    while (cur != NULL) {
        BallListNode* tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    *head = NULL;
    printf(COLOR_GREEN "Freed memory of the ball linked list." COLOR_RESET);
}


void printInfoBall(BallListNode *head)
{
    int i = 0;

    if (!head)
    {
        printf(COLOR_YELLOW "\nNo data available.\n\n" COLOR_RESET);
        return;
    }

    BallListNode *cur = head;

    printf("\n................................... \n");
    while (cur)
    {
        printf("ID: %d,  x : %d,  y : %d, dx : %d, dy : %d, RGB : (%d, %d, %d)\n",
               cur->data.id, cur->data.x, cur->data.y,
               cur->data.dx, cur->data.dy,
               cur->data.color.r, cur->data.color.g, cur->data.color.b);

        cur = cur->next;
        i++;
    }
    printf("\ntotal : %d\n",i);
    printf("................................... \n\n");
}