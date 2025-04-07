#ifndef TEST_SCREEN_BALL_NODE_H
#define TEST_SCREEN_BALL_NODE_H

#include "test_screenball.h"

/**
 * @brief Node structure for storing ball objects in a linked list
 * @details Each node contains a ball object and a pointer to the next node in the list.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct BallListNode {
    ScreenBall data;           ///< Stored ball object
    struct BallListNode* next; ///< Pointer to the next node (singly linked list)
} BallListNode;

/**
 * @brief Creates a new node with the specified ball
 * @param ball The ball object to store in the new node
 * @return Pointer to the newly created node
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
BallListNode* createNode(ScreenBall ball);

/**
 * @brief Appends a ball to the end of the list
 * @param head Pointer to the head of the list
 * @param tail Pointer to the tail of the list (updated if list was empty)
 * @param ball The ball object to append
 * @return Pointer to the head of the list
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
BallListNode* appendBall(BallListNode* head, BallListNode** tail, ScreenBall ball);

/**
 * @brief Prints information about all balls in the list
 * @param head Pointer to the head of the list
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void printInfoBall(BallListNode *head);

/**
 * @brief Deletes the last ball from the list
 * @param head Pointer to the head of the list (updated if list becomes empty)
 * @param tail Pointer to the tail of the list (updated)
 * @return Pointer to the head of the list
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
BallListNode* deleteLastBall(BallListNode** head, BallListNode** tail);

/**
 * @brief Creates a deep copy of the ball list
 * @param head Pointer to the head of the list to copy
 * @return Pointer to the head of the new list
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
BallListNode* deep_copy_ball_list(BallListNode* head);

/**
 * @brief Frees all memory used by the ball list
 * @param head Pointer to the head of the list (set to NULL after freeing)
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void freeBallList(BallListNode** head);

/**
 * @brief Deletes all balls from the list
 * @param head Pointer to the head of the list (set to NULL after deletion)
 * @param tail Pointer to the tail of the list (set to NULL after deletion)
 * @param count Pointer to the ball count (set to 0 after deletion)
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void delete_all_ball(BallListNode** head, BallListNode** tail, int* count);

#endif  //SCREEN_BALL_NODE_H