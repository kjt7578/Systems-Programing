#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFLENGTH
#define BUFLENGTH 20
#endif


void read_lines(int fd, void(*use_line)(void *, char *), void *arg){
        //  파일  , print_line에 들어갈 변수        ,  n 라인카운트
  int buflength = BUFLENGTH;
  char *buf = malloc(BUFLENGTH);
  
  int pos = 0;    // arr의 현재 위치
  int bytes;      // 읽어들인 바이트 수
  int line_start; // 라인 시작 위치

  while((bytes = read(fd, buf + pos, buflength - pos)) > 0){  //read는 읽어들인 바이트 수를 반환
    if(DEBUG) printf("read %d bytes; pos = %d\n", bytes, pos);
    line_start = 0;
    int bufend = pos + bytes;
    
    while(pos < bufend){  //버퍼가 다 차지 않았는데 줄 바꿈을 하였을 경우
      if(DEBUG) printf("start %d, pos %d, char '%c'\n",line_start, pos, buf[pos]);
      if(buf[pos] == '\n'){                        // 줄바꿈을 찾았을때.
        buf[pos] = '\0';                           // 줄바꿈 \n 을 \0 (null) 로 바꿔줌
        use_line(arg, buf + line_start);           // 그 줄을 출력.
        line_start = pos + 1;                      // 다음라인으로 시작위치 설정
      }
      pos++;
    }
    
    if (line_start == pos){  // no partial line
      pos = 0;
    }
    else if (line_start > 0){ // partial line. 분할된 단어를 버퍼의 처음부터 채움
      int segment_length = pos - line_start;
      memmove(buf, buf + line_start, segment_length); //buf + line_start 위치에서 시작하는 segment_length 길이의 메모리 영역을 buf 위치로 이동
      pos = segment_length;
      if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
    }
    else if(bufend == buflength){    //분할라인이 버퍼를 꽉 채울경우
      buflength *= 2;              
      buf = realloc(buf, buflength); // 버퍼 크기를 키움
      if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
    }

    if (pos > 0){          //남은 라인이 파일 끝 이후에 있을경우
      if(pos == buflength){
        buf = realloc(buf, buflength + 1);
      }
      buf[pos] = '\0';
      use_line(arg, buf + line_start);
    }
  } // 25 while end
  free(buf);
}

//*****************************************************************
void print_line(void *st, char *line){
  int *p = st;  //set pointer pointing n (line count)
  printf("%d: |%s|\n", *p+1, line);
  (*p)++;  //다음 라인 포인팅
}

//*****************************************************************
int main(int argc, char **argv){
  char *fname = argc > 1 ? argv[1] : "test.txt";
  //실행시 특정한 지칭txt를 지정했으면 argv[1]으로 그 파일 오픈. 아닐시 test.txt
  int fd = open(fname, O_RDONLY);
  //읽기 전용으로 f_name 열기
  //open이 파일 못열으면 -1리턴
  if(fd < 0){
    perror(fname);
    exit(EXIT_FAILURE);
  }
  int n = 0; //라인 카운트
  read_lines(fd,print_line,&n);

    return EXIT_SUCCESS;
}