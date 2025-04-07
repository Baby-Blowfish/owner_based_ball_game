
#include "client_list_manager.h"

SocketContext create_client(int csock, struct sockaddr_in cliaddr)
{
    SocketContext s;
    s.csock = csock;
    s.cliaddr= cliaddr;
    return s;
}

ClientNode* create_client_node(SocketContext ctx) {
    ClientNode* node = (ClientNode*)malloc(sizeof(ClientNode));
    if (!node) return NULL;
    node->ctx = ctx;
    node->next = NULL;
    return node;
}

ClientNode* append_client(ClientNode* head, ClientNode** tail, SocketContext ctx) {

    ClientNode* newnode = NULL;
    if((newnode =create_client_node(ctx)) == NULL)
        perror( COLOR_RED "[Error] Memory allocation failed" COLOR_RESET);

    if (head == NULL) {
        head = newnode;
        *tail = newnode;
    } else {
        (*tail)->next = newnode;
        *tail = newnode;
    }

    printf( COLOR_GREEN "[Success] client(soket : %d) added successfully." COLOR_RESET, ctx.csock);

    return head;
}

void add_client(ClientListManager* manager, int csock, struct sockaddr_in cliaddr) {
    SocketContext s = create_client(csock, cliaddr);
    manager->head = append_client(manager->head, &manager->tail, s);
}

ClientNode* remove_client_by_socket(int socket_fd, ClientNode** head, ClientNode** tail) {
    if (*head == NULL) return NULL;

    ClientNode* cur = *head;
    ClientNode* prev = NULL;

    while (cur) {
        if (cur->ctx.csock == socket_fd) {
            if (prev == NULL) {
                // 첫 번째 노드를 제거
                *head = cur->next;
                if (*tail == cur) *tail = NULL; // 유일한 노드였을 때
            } else {
                // 중간 또는 마지막 노드를 제거
                prev->next = cur->next;
                if (*tail == cur) *tail = prev;
            }

            printf(COLOR_GREEN "[Success] Client (socket: %d) removed by socket.\n" COLOR_RESET, socket_fd);
            return cur; // 호출자가 free() 해야 함
        }

        prev = cur;
        cur = cur->next;
    }

    printf(COLOR_YELLOW "[Warning] Client with socket %d not found.\n" COLOR_RESET, socket_fd);
    return NULL;
}


void client_list_manager_init(ClientListManager* manager) {

    memset(manager, 0,sizeof(ClientListManager));
    manager->head = NULL;
    manager->tail = NULL;
    pthread_mutex_init(&manager->mutex_client, NULL);
    manager->client_count = 0;
}

void client_list_manager_destroy(ClientListManager* manager) {
    pthread_mutex_destroy(&manager->mutex_client);
    printf( COLOR_GREEN "Mutex 'mutex_client' has been destroyed." COLOR_RESET);
    free_client_list(&manager->head);
}

void free_client_list(ClientNode** head) {
    ClientNode* cur = *head;
    while (cur != NULL) {
        ClientNode* tmp = cur;
        cur = cur->next;

        // 소켓 정리
        if (tmp->ctx.csock > 0) {
            shutdown(tmp->ctx.csock,SHUT_RDWR);
            close(tmp->ctx.csock);
            printf(COLOR_BLUE "[Closed] socket fd %d\n" COLOR_RESET, tmp->ctx.csock);
        }

        free(tmp);
    }
    *head = NULL;
    printf(COLOR_GREEN "Freed memory of the client linked list." COLOR_RESET);
}



void print_clients(ClientNode* head) {

    int i = 0;

    if(!head)
    {
        printf(COLOR_YELLOW "\nNo data available.\n\n" COLOR_RESET);
        return;
    }

    ClientNode *cur = head;

    printf("\n................................... \n");
    while (cur)
    {
        printf("client socket fd : %d\n", cur->ctx.csock);
        cur = cur->next;
        i++;
    }
    printf("\ntotal : %d\n",i);
    printf("................................... \n\n");
}
