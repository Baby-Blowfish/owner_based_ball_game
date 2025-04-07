#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <sys/select.h>
#include "screenballmanager.h"

/**
 * @brief Server port number for client-server communication
 */
#define SERVER_PORT 5100

/**
 * @brief Maximum number of clients that can connect to the server
 */
#define MAX_CLIENTS 10

/**
 * @brief Maximum length of input buffer
 */
#define MAX_INPUT 100

/**
 * @brief Default radius for balls
 */
#define RADIUS 20

/**
 * @brief Initial radius for balls when starting the game
 */
#define START_BALL_RADIUS 20

/**
 * @brief Global flag to control program execution
 * @details Used by signal handlers to gracefully terminate the program
 */
extern volatile sig_atomic_t keep_running;

/**
 * @brief Global state context structure shared across the game
 * @details Contains pointers to resources (framebuffer, ball list, server socket, etc.) 
 *          accessed by multiple threads. Each thread receives this structure as an argument
 *          to access the resources it needs.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    int socket_fd;                   ///< Socket file descriptor for server communication
    dev_fb* framebuffer;             ///< Framebuffer device information structure (for display control)
    BallListManager* ball_list_manager; ///< Ball list manager
    pthread_mutex_t mutex_ball;      ///< Mutex for synchronizing access to ball resources
} SharedContext;

/**
 * @brief Initializes the shared context manager
 * @return Pointer to the initialized SharedContext structure
 * @details Allocates and initializes all resources needed for the game
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
SharedContext* manager_init();

/**
 * @brief Destroys the shared context manager and frees all resources
 * @param arg Pointer to the SharedContext structure to destroy
 * @details Properly cleans up all allocated resources
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void manager_destroy(SharedContext* arg);

/**
 * @brief Thread function for rendering the game state
 * @param arg Pointer to the SharedContext structure
 * @return NULL when the thread terminates
 * @details Continuously renders the current game state to the framebuffer
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void* render_thread(void* arg);

/**
 * @brief Thread function for receiving data from the server
 * @param arg Pointer to the SharedContext structure
 * @return NULL when the thread terminates
 * @details Continuously listens for incoming data from the server and updates the game state
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void* socket_recv_thread(void* arg);

/**
 * @brief Thread function for sending data to the server
 * @param arg Pointer to the SharedContext structure
 * @return NULL when the thread terminates
 * @details Handles sending data to the server when needed
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void* socket_send_thread(void* arg);

#endif