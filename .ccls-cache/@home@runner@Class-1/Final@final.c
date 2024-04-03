#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>

void fork_Who();
void fork_Wait();
void fork_Wait2();

int main() {
  //fork_Who();
  //fork_Wait();
  fork_Wait2();
  return 0;
}


void fork_Who(){
  pid_t pid = fork();
  if (pid == -1){
    perror("fork");
    exit(1);
  }
  int x = 1;
  printf("PID : %d,  x : %d\n", getpid(), x);

  if (pid > 0){
    printf("Parent PID : %ld,  x : %d , pid : %d\n",(long)getpid(), x, pid);
  }
  else if (pid == 0){
    printf("Child PID  : %ld,  x : %d\n",(long)getpid(), x);
  }

  printf("WHO IS DOING THIS? PID: %d\n", getpid());
}

void fork_Wait(){
  //https://codetravel.tistory.com/30
  pid_t childPid = fork();
  int status, i;

  if(childPid > 0){ //parent process
    pid_t waitPid;
    printf("부모 PID : %ld, pid : %d %d \n",(long)getpid(), childPid, errno);
            for(i=0;i<5;i++) {
                //sleep(1);
              printf(".");
            }

            waitPid = wait(&status);

            if(waitPid == -1) {
                printf("에러 넘버 : %d \n",errno);
                perror("wait");
            }
            else {
                if(WIFEXITED(status)) {
                    printf("wait : 자식 프로세스 정상 종료 %d\n",WEXITSTATUS(status));
                }
                else if(WIFSIGNALED(status)) {
                    printf("wait : 자식 프로세스 비정상 종료 %d\n",WTERMSIG(status));
                }
            }

            printf("부모 종료 %d %d\n",waitPid,WTERMSIG(status));
        }
        else if(childPid == 0){  // 자식 프로세스
            printf("자식 PID : %ld \n",(long)getpid());

            printf("자식 종료\n");
            exit(0);
        }
        else {  // fork 실패
            perror("fork Fail! \n");
            exit(-1);
        }
    }

void fork_Wait2(){
      pid_t childPid;
      int status,i;

      childPid = fork();

      if(childPid > 0) {  // 부모 프로세스
          pid_t waitPid;
          printf("부모 PID : %ld, pid : %d %d \n",(long)getpid(), childPid, errno);

          waitPid = wait(&status);

          if(waitPid == -1) {
              printf("에러 넘버 : %d \n",errno);
              perror("wait 함수 오류 반환");
          }
          else {
              if(WIFEXITED(status)) {
                  printf("wait : 자식 프로세스 정상 종료 %d\n",WEXITSTATUS(status));
              }
              else if(WIFSIGNALED(status)) {
                  printf("wait : 자식 프로세스 비정상 종료 %d\n",WTERMSIG(status));
              }
          }

          printf("부모 종료 %d %d\n",waitPid,WTERMSIG(status));
      }
      else if(childPid == 0){  // 자식 코드
          printf("자식 PID : %ld \n",(long)getpid());

          for(i=0;i<5;i++) {
              sleep(1);
          }

          printf("자식 종료\n");
          exit(0);
      }
      else {  // fork 실패
          perror("fork Fail! \n");
          exit(-1);
      }

  }

