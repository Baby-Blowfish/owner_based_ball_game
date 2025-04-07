// client_list_manager.h
#ifndef CLIENT_LIST_MANAGER_H
#define CLIENT_LIST_MANAGER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "console_color.h"

#define MAX_CLIENTS 10

/**
 * @brief Structure representing a socket context
 * @details This structure contains information about a client socket connection,
 *          including the socket file descriptor and client address.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    int csock;                  // Client socket file descriptor
    struct sockaddr_in cliaddr; // Client address information
} SocketContext;

/**
 * @brief Structure representing a client node in the linked list
 * @details This structure represents a single client in the linked list of clients,
 *          containing the socket context and a pointer to the next client.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct ClientNode {
    SocketContext ctx;          // Socket context for this client
    struct ClientNode* next;    // Pointer to the next client in the list
} ClientNode;

/**
 * @brief Structure representing a client list manager
 * @details This structure manages a linked list of clients with thread safety
 *          mechanisms for concurrent access.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
typedef struct {
    ClientNode* head;           // Pointer to the first client in the list
    ClientNode* tail;           // Pointer to the last client in the list
    pthread_mutex_t mutex_client; // Mutex for synchronizing client list operations
    int client_count;           // Current number of clients in the list
} ClientListManager;

/**
 * @brief Creates a new socket context
 * @param csock Client socket file descriptor
 * @param cliaddr Client address information
 * @return A newly created SocketContext
 * @details Initializes a new socket context with the provided socket and address.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
SocketContext create_client(int csock, struct sockaddr_in cliaddr);

/**
 * @brief Creates a new client node
 * @param ctx Socket context for the new client
 * @return Pointer to the newly created ClientNode, or NULL if allocation fails
 * @details Allocates memory for a new client node and initializes it with the provided context.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
ClientNode* create_client_node(SocketContext ctx);

/**
 * @brief Appends a client to the linked list
 * @param head Pointer to the head of the client list
 * @param tail Pointer to the tail of the client list
 * @param ctx Socket context for the new client
 * @return Updated head of the client list
 * @details Adds a new client to the end of the linked list.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
ClientNode* append_client(ClientNode* head, ClientNode** tail, SocketContext ctx);

/**
 * @brief Adds a client to the client list manager
 * @param manager Pointer to the client list manager
 * @param csock Client socket file descriptor
 * @param cliaddr Client address information
 * @details Creates a new client and adds it to the manager's list.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void add_client(ClientListManager* manager, int csock, struct sockaddr_in cliaddr);

/**
 * @brief Removes a client from the list by socket file descriptor
 * @param socket_fd Socket file descriptor of the client to remove
 * @param head Pointer to the head of the client list
 * @param tail Pointer to the tail of the client list
 * @return Pointer to the removed client node, or NULL if not found
 * @details Removes a client from the list based on its socket file descriptor.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
ClientNode* remove_client_by_socket(int socket_fd, ClientNode** head, ClientNode** tail);

/**
 * @brief Frees all resources used by the client list
 * @param head Pointer to the head of the client list
 * @details Frees all client nodes in the list and closes their sockets.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void free_client_list(ClientNode** head);

/**
 * @brief Prints information about all clients in the list
 * @param head Pointer to the head of the client list
 * @details Displays information about each client in the list.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void print_clients(ClientNode* head);

/**
 * @brief Initializes a client list manager
 * @param manager Pointer to the client list manager to be initialized
 * @details Initializes the client list manager by setting head and tail to NULL,
 *          initializing the mutex, and setting the client count to 0.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void client_list_manager_init(ClientListManager* manager);

/**
 * @brief Destroys a client list manager
 * @param manager Pointer to the client list manager to be destroyed
 * @details Frees all resources used by the client list manager, including
 *          destroying the mutex and freeing all client nodes.
 * @date 2025-04-07
 * @author Kim Hyo Jin
 */
void client_list_manager_destroy(ClientListManager* manager);

#endif // CLIENT_LIST_MANAGER_H
