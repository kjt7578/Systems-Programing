#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// 신호 핸들러 함수
void signal_handler(int signum) {
    printf("Received signal: %d\n", signum);

    // SIGINT(2)가 발생했을 때 메시지 출력 후 프로그램 종료
    if (signum == SIGINT) {
        printf("SIGINT received. Exiting.\n");
        exit(EXIT_SUCCESS);
    }
}

int main(){
  // SIGINT 신호에 대한 처리 방식 설정
  signal(SIGINT, signal_handler);

  printf("Signal handling example. Press Ctrl+C to send SIGINT.\n");
  while (1) {
      // 프로그램이 종료되지 않고 계속 실행되도록 무한 루프 유지
      sleep(1);
    printf("BYE");
  }


  return 0;
}

/*
위의 예시 코드는 SIGINT (Ctrl+C) 신호를 처리하기 위해 signal() 함수를 사용하여 signal_handler 함수를 등록합니다. 프로그램은 무한 루프를 실행하며, SIGINT가 수신되면 signal_handler 함수가 호출되어 메시지를 출력하고 프로그램을 종료합니다.

주의할 점은 신호 핸들러 내에서는 시스템 호출이나 다른 비동기적인 동작을 수행하는 것이 안전하지 않을 수 있습니다. 따라서 신호 핸들러에서는 가능한 간단한 작업만 수행하는 것이 좋습니다.

프로그램을 실행한 후에는 Ctrl+C를 눌러 SIGINT를 보내면 프로그램이 메시지를 출력하고 종료됩니다.
  */