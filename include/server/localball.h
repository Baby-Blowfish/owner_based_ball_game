#ifndef LOCAL_BALL_H
#define LOCAL_BALL_H

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
 * @details This structure stores the red, green, and blue channel values
 *          to represent a color in the RGB color space.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    unsigned char r;  ///< Red channel value (0 ~ 255)
    unsigned char g;  ///< Green channel value (0 ~ 255)
    unsigned char b;  ///< Blue channel value (0 ~ 255)
} RGBColor;


/**
 * @brief Structure representing a ball for screen display
 * @details This structure represents a ball object that has been converted
 *          from a logical coordinate system to pixel coordinates for display
 *          on the client's screen.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    int id;            ///< Unique identifier for the ball
    int x, y;          ///< Screen coordinates in pixels
    int dx, dy;        ///< Velocity components in pixels per frame
    int radius;        ///< Radius in pixels
    int owner_id;      ///< Owner ID of the ball
    RGBColor color;    ///< Color information for the ball
} ScreenBall;


/**
 * @brief Structure representing a ball object (server-side)
 * @details This structure represents a ball object in a logical coordinate system
 *          (0.0 ~ 1000.0) used by the server for calculations and physics.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    int id;            ///< Unique identifier for the ball
    float x, y;        ///< Logical center position (0.0 ~ 1000.0)
    int dx, dy;        ///< Velocity components
    int radius;        ///< Logical radius (in logical coordinate units)
    int owner_id;      ///< Owner ID of the ball
    RGBColor color;    ///< Color information for the ball
} LogicalBall;

/**
 * @brief Creates a ball in the logical coordinate system
 * @param id Unique identifier for the ball
 * @param radius Radius of the ball
 * @return A newly created LogicalBall object
 * @details Creates a new ball with the specified ID and radius, initializes
 *          its position to a random location within the logical coordinate space,
 *          sets a random velocity, and assigns a random color.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
LogicalBall create_logical_ball(int id, int radius, int owner_id);

/**
 * @brief Gets the color of a ball based on its owner ID
 * @param owner_id The ID of the ball's owner
 * @return A RGBColor structure representing the ball's color
 * @details Returns a color based on the owner ID, with a default fallback color
 *          if the ID is not recognized.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
RGBColor get_color_by_owner(int owner_id);  

/**
 * @brief Moves a ball in the logical coordinate system
 * @param b Pointer to the ball object to be moved
 * @details Updates the ball's position by adding its velocity components to
 *          its current position. If the ball reaches the boundaries of the
 *          logical coordinate space, its velocity is reversed to create a
 *          bouncing effect.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void move_logical_ball(LogicalBall *b);

#endif // LOCAL_BALL_H
