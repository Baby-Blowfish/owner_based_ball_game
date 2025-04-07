#ifndef SCREEN_BALL_MANAGER_H
#define SCREEN_BALL_MANAGER_H

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

#include "fbDraw.h"
#include "screenball_list.h"

/**
 * @brief Command definitions for ball operations
 */
#define CMD_ADD 'a'         ///< Add balls command
#define CMD_DEL 'd'         ///< Delete balls command
#define CMD_SPEED_UP 'w'    ///< Speed up balls command
#define CMD_SPEED_DOWN 's'  ///< Slow down balls command
#define CMD_EXIT 'x'        ///< Exit command

/**
 * @brief Type definition for unsigned char
 */
typedef unsigned char uchar;

/**
 * @brief Structure for managing a collection of balls
 * @details Contains a linked list of balls with synchronization mechanisms
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    BallListNode* head;      ///< Head node of the ball list
    BallListNode* tail;      ///< Tail node of the ball list
    pthread_mutex_t mutex_ball; ///< Mutex for synchronizing access to the ball list
    int total_count;         ///< Total number of balls currently in the list
} BallListManager;



/**
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void draw_ball(dev_fb* fb, ScreenBall* b);

/**
 * @brief Draws all balls in the list on the framebuffer
 * @param fb Pointer to the framebuffer device
 * @param head Pointer to the head of the ball list
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void draw_ball_list(dev_fb* fb, BallListNode* head);

/**
 * @brief Draws the command guide on the framebuffer
 * @param fb Pointer to the framebuffer device
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void draw_command_guide(dev_fb* fb);

/**
 * @brief Initializes a new ball manager
 * @param manager Pointer to the ball manager to initialize
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void ball_manager_init(BallListManager* manager);

/**
 * @brief Destroys a ball manager and frees all resources
 * @param manager Pointer to the ball manager to destroy
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void ball_manager_destroy(BallListManager* manager);

/**
 * @brief Converts a logical coordinate to a pixel coordinate
 * @param logic Logical coordinate value (0.0 ~ 1000.0)
 * @param screen_size Screen size in pixels
 * @return Pixel coordinate
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
int logic_to_pixel(float logic, int screen_size);

/**
 * @brief Converts a logical ball to a screen ball
 * @param l Logical ball to convert
 * @param width Screen width in pixels
 * @param height Screen height in pixels
 * @return Screen ball with converted coordinates
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
ScreenBall logical_to_screen_ball(LogicalBall l, int width, int height);

/**
 * @brief Updates the ball list from a serialized string
 * @param manager Pointer to the ball manager
 * @param str Serialized string containing ball data
 * @param width Screen width in pixels
 * @param height Screen height in pixels
 * @details Parses the serialized string and updates the ball list accordingly
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void updateBallListFromSerialized(BallListManager* manager, const char* str, int width, int height);

/**
 * @brief Adds balls to the manager
 * @param manager Pointer to the ball manager
 * @param count Number of balls to add
 * @param width Screen width in pixels
 * @param height Screen height in pixels
 * @param radius Radius of the balls to add
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void add_ball(BallListManager* manager, int count, int width, int height, int radius);

/**
 * @brief Deletes balls from the manager
 * @param manager Pointer to the ball manager
 * @param count Number of balls to delete
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void delete_ball(BallListManager* manager, int count);

#endif // SCREEN_BALL_MANAGER_H
