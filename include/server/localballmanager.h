#ifndef LOCAL_BALL_MANAGER_H
#define LOCAL_BALL_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#include "console_color.h"
#include "localball_list.h"

#define FILE_NAME "data.bin"

// Command definitions
#define CMD_ADD 'a'
#define CMD_DEL 'd'
#define CMD_SPEED_UP 'w'
#define CMD_SPEED_DOWN 's'
#define CMD_EXIT 'x'

// Ball properties for initialization
#define START_BALL_COUNT 5
#define START_BALL_RADIUS 20

/**
 * @brief Structure representing a ball manager
 * @details This structure manages a collection of balls and provides
 *          functionality for adding, removing, and updating balls.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    BallListNode* head;  ///< Pointer to the first ball in the list
    BallListNode* tail;  ///< Pointer to the last ball in the list
    pthread_mutex_t mutex_ball; ///< Mutex for synchronizing ball list operations
    int total_count;     ///< Total number of balls in the list
} BallListManager;

/**
 * @brief Function pointer type for command handlers
 * @param BallListManager* Pointer to the ball list manager
 * @param int Number of balls
 * @param int Radius of balls
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef void (*CommandHandler)(BallListManager*, int, int);

/**
 * @brief Structure mapping commands to their handler functions
 * @details This structure pairs command characters with their corresponding
 *          handler function pointers.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    char cmd;            ///< Command character
    CommandHandler handler; ///< Function pointer to handle the command
} CommandEntry;

typedef unsigned char uchar;

extern CommandEntry command_table[];

/**
 * @brief Initializes a ball list manager
 * @param manager Pointer to the ball list manager to be initialized
 * @details Initializes the ball list manager's member variables and creates
 *          default balls.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void ball_manager_init(BallListManager* manager);

/**
 * @brief Destroys a ball list manager
 * @param manager Pointer to the ball list manager to be destroyed
 * @details Frees all nodes in the ball list and the manager itself.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void ball_manager_destroy(BallListManager* manager);

/**
 * @brief Adds new balls to the ball list
 * @param manager Pointer to the ball list manager
 * @param count Number of balls to add
 * @param radius Radius of the balls to add
 * @details Creates and adds balls with the specified count and radius to the list.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void add_ball(BallListManager* manager, int count, int radius);

/**
 * @brief Deletes balls from the ball list
 * @param manager Pointer to the ball list manager
 * @param count Number of balls to delete
 * @details Removes the specified number of balls from the end of the list.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void delete_ball(BallListManager* manager, int count);

/**
 * @brief Updates the positions of all balls
 * @param manager Pointer to the ball list manager
 * @details Updates the position and velocity of all balls in the list.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void move_all_ball(BallListManager* manager);

/**
 * @brief Serializes the ball list into a string
 * @param manager Pointer to the ball list manager
 * @return Serialized ball list string (memory must be freed by caller)
 * @details Converts all ball information in the list to a string format.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
char* serialize_ball_list(BallListManager* manager);

/**
 * @brief Handles the add ball command
 * @param m Pointer to the ball list manager
 * @param count Number of balls to add
 * @param radius Radius of the balls to add
 * @details Processes the add ball command and logs the action.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void handle_add(BallListManager* m, int count, int radius);

/**
 * @brief Handles the delete ball command
 * @param m Pointer to the ball list manager
 * @param count Number of balls to delete
 * @param radius Unused parameter (maintained for function signature consistency)
 * @details Processes the delete ball command and logs the action.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void handle_delete(BallListManager* m, int count, int radius);

/**
 * @brief Handles the speed up command
 * @param m Pointer to the ball list manager
 * @param count Number of balls to speed up
 * @param radius Unused parameter (maintained for function signature consistency)
 * @details Processes the speed up command.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void handle_speed_up(BallListManager* m, int count, int radius);

/**
 * @brief Handles the speed down command
 * @param m Pointer to the ball list manager
 * @param count Number of balls to slow down
 * @param radius Unused parameter (maintained for function signature consistency)
 * @details Processes the speed down command.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void handle_speed_down(BallListManager* m, int count, int radius);

/**
 * @brief Dispatches commands to their handler functions
 * @param cmd Command character to process
 * @param count Number of balls for command processing
 * @param radius Radius of balls for command processing
 * @param m Pointer to the ball list manager
 * @details Calls the appropriate handler function based on the command.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void dispatch_command(char cmd, int count, int radius, BallListManager* m);

#endif // LOCAL_BALL_MANAGER_H
