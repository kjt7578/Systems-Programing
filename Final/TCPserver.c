//0417
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_PENDING_CONNECTIONS 5
#define BUFFER_SIZE 256

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

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
    if (listen(server_fd, MAX_PENDING_CONNECTIONS) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connection
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Read client message
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received data
    printf("Received message from client: %s\n", buffer);

    // Echo back to client
    if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    // Close socket
    close(client_fd);
    close(server_fd);

    return 0;
}

/*socket(): TCP 서버 소켓을 생성합니다.
bind(): 서버 소켓을 지정된 포트에 바인딩합니다.
listen(): 클라이언트의 연결 요청을 수신할 수 있도록 소켓을 대기 상태로 설정합니다.
accept(): 클라이언트의 연결 요청을 수락하고, 클라이언트와 통신할 새로운 소켓을 생성합니다.
recv(): 클라이언트로부터 데이터를 수신합니다.
send(): 클라이언트에게 데이터를 다시 전송합니다.
연결이 끝나면 close()를 사용하여 소켓을 닫습니다.

이 예시 코드를 실행하면 클라이언트가 서버에 "Hello"라는 메시지를 보내고, 서버는 해당 메시지를 다시 클라이언트에게 응답하는 간단한 TCP 통신이 이루어집니다. TCP는 신뢰성 있는 연결을 제공하므로 데이터가 순서대로 전송되고 오류 복구 기능을 갖추고 있습니다.
*/