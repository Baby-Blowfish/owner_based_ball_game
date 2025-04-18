#include  "localball_list.h"

BallListNode* createNode(LogicalBall ball) {

  BallListNode *newnode = (BallListNode*)malloc(sizeof(BallListNode));
  if (!newnode) {
      return NULL;
  }

  memset(newnode, 0, sizeof(BallListNode));

  newnode->data = ball;
  newnode->next = NULL;

    return newnode;
}

BallListNode* appendBall(BallListNode* head, BallListNode** tail, LogicalBall ball)
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

void moveBallList(BallListNode* head) {
    BallListNode* cur = head;
    while (cur != NULL) {
        move_logical_ball(&cur->data);
        cur = cur->next;
    }
}



void speedUpBalls(BallListNode* head, int owner_id) {
    BallListNode* cur = head;
    while (cur != NULL) {
        if(cur->data.owner_id == owner_id) {

            if (cur->data.dx > 0)
                cur->data.dx = (cur->data.dx > MAX_SPEED) ? MAX_SPEED : cur->data.dx * 2;
            else if (cur->data.dx < 0)
                cur->data.dx = (cur->data.dx < MIN_SPEED) ? MIN_SPEED : cur->data.dx * 2;

            if (cur->data.dy > 0)
                cur->data.dy = (cur->data.dy > MAX_SPEED) ? MAX_SPEED : cur->data.dy * 2;
            else if (cur->data.dy < 0)
                cur->data.dy = (cur->data.dy < MIN_SPEED) ? MIN_SPEED : cur->data.dy * 2;
        }
        cur = cur->next;
    }
    printInfoBall(head);
}

void speedDownBalls(BallListNode* head, int owner_id) {
    BallListNode* cur = head;
    while (cur != NULL) {
        if(cur->data.owner_id == owner_id) {
            if (cur->data.dx > 0)
                cur->data.dx = (cur->data.dx > 1) ? cur->data.dx / 2 : 1;
            else if (cur->data.dx < 0)
            cur->data.dx = (cur->data.dx < -1) ? cur->data.dx / 2 : -1;

            if (cur->data.dy > 0)
                cur->data.dy = (cur->data.dy > 1) ? cur->data.dy / 2 : 1;
            else if (cur->data.dy < 0)
                cur->data.dy = (cur->data.dy < -1) ? cur->data.dy / 2 : -1;
        }
        cur = cur->next;
    }
    printInfoBall(head);
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


void printInfoBall(BallListNode *head) {
    int i = 0;

    if (!head) {
        printf(COLOR_YELLOW "\nNo data available.\n\n" COLOR_RESET);
        return;
    }

    BallListNode *cur = head;

    printf("\n................................... \n");
    while (cur) {
        printf("FD: %d, ID: %d,  x : %.1f,  y : %.1f, dx : %d, dy : %d, RGB : (%d, %d, %d)\n",
               cur->data.owner_id, cur->data.id, cur->data.x, cur->data.y,
               cur->data.dx, cur->data.dy,
               cur->data.color.r, cur->data.color.g, cur->data.color.b);

        cur = cur->next;
        i++;
    }
    printf("\ntotal : %d\n", i);
    printf("................................... \n\n");
}