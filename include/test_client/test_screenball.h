#ifndef TEST_SCREEN_BALL_H
#define TEST_SCREEN_BALL_H

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
#include "console_color.h"

/**
 * @brief Structure representing RGB color information
 * @details Stores red, green, and blue channel values to represent a color.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    unsigned char r;  ///< Red channel (0 ~ 255)
    unsigned char g;  ///< Green channel (0 ~ 255)
    unsigned char b;  ///< Blue channel (0 ~ 255)
} RGBColor;

/**
 * @brief Ball structure for screen display in client resolution
 * @details Stores the result of converting a BallObject received from the server into pixel units.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    int id;            ///< Unique identifier of the ball
    int x, y;          ///< Actual screen coordinates (in pixels)
    int dx, dy;        ///< Screen velocity components
    int radius;        ///< Radius (in pixels)
    RGBColor color;    ///< Ball color information
} ScreenBall;

/**
 * @brief Structure representing a single ball object (server-side)
 * @details Manages the position and properties of a ball based on logical coordinates (0.0 ~ 1000.0).
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    int id;            ///< Unique identifier of the ball
    float x, y;        ///< Logical center position (0.0 ~ 1000.0)
    int dx, dy;        ///< Velocity components
    int radius;        ///< Logical radius (in logical coordinate units)
    RGBColor color;    ///< Ball color information
} LogicalBall;

/**
 * @brief Creates a ball for screen display
 * @param id Unique identifier for the ball to be created
 * @param width Screen width
 * @param height Screen height
 * @param radius Radius of the ball to be created
 * @return Created ScreenBall object
 * @details Creates a new ball with the specified ID and radius, and places it at a random position within the screen.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
ScreenBall create_screen_ball(int id, int width, int height, int radius);

#endif // SCREEN_BALL_H
