#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 8080
#define BUFFER_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Resolve server hostname
    if ((server = gethostbyname("localhost")) == NULL) {
        perror("Host not found");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Prompt user to enter message
    printf("Please enter the message: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    // Send message to server
    if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    // Receive response from server
    ssize_t bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received data
    printf("Server replied: %s\n", buffer);

    // Close socket
    close(sockfd);

    return 0;
}

/*
socket(): TCP 클라이언트 소켓을 생성합니다.
gethostbyname(): 호스트 이름을 IP 주소로 해석합니다.
connect(): 서버에 연결합니다.
사용자로부터 메시지를 입력받고, send()를 사용하여 서버에 메시지를 전송합니다.
recv(): 서버로부터 응답을 수신합니다.
응답을 출력한 후에 클라이언트 소켓을 닫습니다*/