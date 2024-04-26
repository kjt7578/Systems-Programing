#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 2

// Structure to hold client information
typedef struct {
    int sockfd;
    struct sockaddr_in addr;
} ClientInfo;

// Function to handle game session
void *handle_game_session(void *arg) {
    ClientInfo *clients = (ClientInfo *)arg;
    int client1_fd = clients[0].sockfd;
    int client2_fd = clients[1].sockfd;

    // Game logic goes here...
    // For example, handle player interactions, game state updates, etc.

    // Close client sockets after game session ends
    close(client1_fd);
    close(client2_fd);

    // Free memory allocated for client info
    free(clients);

    pthread_exit(NULL);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t tid;
    ClientInfo *clients = NULL;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to the specified address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept incoming connections
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("New connection accepted from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Allocate memory for client information
        if (clients == NULL) {
            clients = (ClientInfo *)malloc(MAX_CLIENTS * sizeof(ClientInfo));
            if (clients == NULL) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }
        }

        // Store client information
        clients[MAX_CLIENTS - 1].sockfd = client_fd;
        clients[MAX_CLIENTS - 1].addr = client_addr;

        // Start game session if two clients are connected
        if (clients[0].sockfd != 0 && clients[1].sockfd != 0) {
            // Create thread to handle game session
            if (pthread_create(&tid, NULL, handle_game_session, (void *)clients) != 0) {
                perror("Thread creation failed");
                exit(EXIT_FAILURE);
            }

            // Reset clients array for next game session
            clients = NULL;
        }
    }

    // Close server socket (unreachable in this example)
    close(server_fd);

    return 0;
}
