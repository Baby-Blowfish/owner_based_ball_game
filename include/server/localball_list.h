#ifndef LOCAL_BALL_NODE_H
#define LOCAL_BALL_NODE_H

#include "localball.h"

#define MAX_SPEED 2000
#define MIN_SPEED -2000


/**
 * @brief A node structure for storing ball objects in a linked list
 * @details This structure represents a node in a singly linked list that stores ball objects.
 *          Each node contains a ball object and a pointer to the next node in the list.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct BallListNode {
    LogicalBall data;           ///< The ball object stored in this node
    struct BallListNode* next;  ///< Pointer to the next node in the linked list
} BallListNode;

/**
 * @brief Creates a new node with the given ball object
 * @param ball The ball object to be stored in the new node
 * @return A pointer to the newly created node, or NULL if memory allocation fails
 * @details Allocates memory for a new node, initializes it with the given ball object,
 *          and sets the next pointer to NULL.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
BallListNode* createNode(LogicalBall ball);

/**
 * @brief Appends a ball to the end of the linked list
 * @param head Pointer to the head of the linked list
 * @param tail Double pointer to the tail of the linked list
 * @param ball The ball object to be appended
 * @return A pointer to the newly created node
 * @details Creates a new node with the given ball object and appends it to the end of the list.
 *          If the list is empty, the new node becomes both the head and tail.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
BallListNode* appendBall(BallListNode* head, BallListNode** tail, LogicalBall ball);

/**
 * @brief Prints information about all balls in the linked list
 * @param head Pointer to the head of the linked list
 * @details Traverses the linked list and prints the properties of each ball object.
 *          This function is primarily used for debugging purposes.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void printInfoBall(BallListNode *head);

/**
 * @brief Updates the position of all balls in the linked list
 * @param head Pointer to the head of the linked list
 * @details Traverses the linked list and updates the position of each ball object
 *          based on its current velocity. This function is called periodically to animate the balls.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void moveBallList(BallListNode* head);

/**
 * @brief Deletes the last ball from the linked list
 * @param head Double pointer to the head of the linked list
 * @param tail Double pointer to the tail of the linked list
 * @return A pointer to the deleted node, or NULL if the list is empty
 * @details Removes the last node from the linked list and updates the tail pointer.
 *          If the list becomes empty after deletion, both head and tail are set to NULL.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
BallListNode* deleteLastBall(BallListNode** head, BallListNode** tail);

/**
 * @brief Increases the velocity of all balls in the linked list
 * @param head Pointer to the head of the linked list
 * @details Traverses the linked list and increases the velocity (dx, dy) of each ball object.
 *          This function is called when the user requests to speed up the balls.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void speedUpBalls(BallListNode* head);

/**
 * @brief Decreases the velocity of all balls in the linked list
 * @param head Pointer to the head of the linked list
 * @details Traverses the linked list and decreases the velocity (dx, dy) of each ball object.
 *          This function is called when the user requests to slow down the balls.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void slowDownBalls(BallListNode* head);

/**
 * @brief Frees all memory allocated for the linked list
 * @param head Double pointer to the head of the linked list
 * @details Traverses the linked list and frees the memory allocated for each node.
 *          After this function is called, the head pointer is set to NULL.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void freeBallList(BallListNode** head);

#endif //LOCAL_BALL_NODE_H