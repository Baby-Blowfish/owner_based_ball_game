#ifndef CLIENT_BALL_MANAGER_H
#define CLIENT_BALL_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>

#include "localballmanager.h"
#include "client_list_manager.h"
#include "task.h"
#include "log.h"

#define SERVER_PORT 5100
#define NUM_WORKERS 4

// sig_atomic_t guarantees atomic read/write operations
extern volatile sig_atomic_t keep_running;
extern TaskQueue* global_task_queue;

/**
 * @brief Global state context structure shared across the game
 * @details Contains pointers to resources (ball list, client list, etc.) that are
 *          accessed by multiple threads. Each thread receives this structure as an
 *          argument to access the resources it needs.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    BallListManager* ball_list_manager;     ///< Ball list manager
    ClientListManager* client_list_manager; ///< Client list manager
    TaskQueue *task_queue;                  ///< Task queue for processing commands
    int epoll_fd;                          ///< Epoll file descriptor for event handling
} SharedContext;

/**
 * @brief Initializes the server manager
 * @return Pointer to the newly created SharedContext
 * @details Creates and initializes the shared context with ball list manager,
 *          client list manager, task queue, and epoll file descriptor.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
SharedContext* manager_init();

/**
 * @brief Destroys the server manager
 * @param arg Pointer to the SharedContext to be destroyed
 * @details Frees all resources used by the shared context, including
 *          the ball list manager, client list manager, and task queue.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void manager_destroy(SharedContext* arg);

/**
 * @brief Parses a command string
 * @param cmdStr Command string to parse
 * @param ball_count Pointer to store the number of balls
 * @param radius Pointer to store the radius of balls
 * @return Command character (a, d, w, s, x)
 * @details Extracts the command character and parameters from the command string.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
char parseCommand(const char* cmdStr, int* ball_count, int* radius);

/**
 * @brief Broadcasts the ball state to all clients
 * @param client_mgr Pointer to the client list manager
 * @param ball_mgr Pointer to the ball list manager
 * @details Serializes the ball list into a string and sends it to all connected clients.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void broadcast_ball_state(ClientListManager* client_mgr, BallListManager* ball_mgr);

/**
 * @brief Logs a client connection event
 * @param fd Client file descriptor
 * @param cliaddr Client address information
 * @details Records information about a new client connection.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void log_client_connect(int fd, struct sockaddr_in* cliaddr);

/**
 * @brief Logs a client disconnection event
 * @param fd Client file descriptor
 * @param reason Reason for disconnection
 * @details Records information about a client disconnection.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void log_client_disconnect(int fd, const char* reason);

/**
 * @brief Logs ball memory usage
 * @param action Action performed (ADD or DEL)
 * @param fd Client file descriptor
 * @param count Number of balls
 * @details Records information about ball creation or deletion, including
 *          memory usage calculations.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void log_ball_memory_usage(const char* action, int fd, int count);

/**
 * @brief Worker thread function
 * @param arg Pointer to the SharedContext
 * @return NULL
 * @details Processes tasks from the task queue, handling commands and
 *          updating ball states.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void* worker_thread(void* arg);

/**
 * @brief Ball state broadcast thread function
 * @param arg Pointer to the SharedContext
 * @return NULL
 * @details Periodically broadcasts the current ball state to all clients.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void* cycle_broadcast_ball_state(void* arg);

#endif