//0415
//This code won't work because there is no host rn
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 8080

int main() {
    int sockfd, n;
    struct sockaddr_in serv_addr;
  // 소켓 파일 디스크립터
    struct hostent *server;
  //serv_addr는 서버 주소를 저장하기 위한 구조체이고, server는 호스트 정보를 저장하는 구조체 포인터

    char buffer[256]; //메시지를 저장할 문자열 배열
    printf("Please enter the message: ");
    fgets(buffer, 255, stdin);

  //socket() 함수를 사용하여 소켓을 생성합니다. AF_INET은 IPv4를 사용하고, SOCK_STREAM은 TCP 소켓을 의미합니다. 소켓 생성에 실패하면 오류 메시지를 출력하고 프로그램을 종료합니다.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

  //gethostbyname() 함수를 사용하여 호스트 이름("localhost")을 IP 주소로 변환하여 server 구조체에 저장합니다. 호스트가 존재하지 않으면 오류를 출력하고 프로그램을 종료합니다.
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

  //serv_addr 구조체를 초기화하고, 주소 패밀리를 AF_INET으로 설정합니다. 호스트의 IP 주소를 serv_addr.sin_addr.s_addr에 복사하고, 포트 번호를 설정합니다.
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);

  //connect() 함수를 사용하여 클라이언트 소켓을 서버에 연결합니다. 연결에 실패하면 오류를 출력하고 프로그램을 종료합니다.
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

  //write() 함수를 사용하여 buffer에 저장된 메시지를 서버로 전송합니다. 전송에 실패하면 오류를 출력하고 프로그램을 종료합니다.
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

  //memset() 함수를 사용하여 buffer를 초기화합니다. 그런 다음 read() 함수를 사용하여 서버로부터 응답을 읽어옵니다. 읽기에 실패하면 오류를 출력하고 프로그램을 종료합니다.
    memset(buffer, 0, sizeof(buffer));
    n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    printf("Server replied: %s\n", buffer);

    close(sockfd);
    return 0;
}
