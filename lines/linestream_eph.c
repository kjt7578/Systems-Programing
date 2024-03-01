// Reading through a file by line, non-allocating iterator-style.
//
// This code puts all the file-reading logic into next_line,
// which returns a freshly allocated string, or NULL at EOF.
//
// The lines_t structure holds the state of the iterator,
// which is a buffer and the current position in the buffer.
// next_line automatically refreshes the buffer as needed while
// constructing the next line.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifndef DEBUG
#define DEBUG 1
#endif

#ifndef BUFSIZE
#define BUFSIZE 128
#endif

typedef struct {  //새 타입 (lines_t) 정의
    int fd;
    int pos;
    int len;
    int size;
    char *buf;
} lines_t;

void ldinit(lines_t *L, int fd){  //lines_t L 초기화
  L -> fd = fd;                //파일 디스크립터
  L -> pos = 0;                //현재 버퍼내 위치
  L -> len = 0;                //버퍼에 저장된 길이
  L -> size = BUFSIZE;         //버퍼의 전체 길이
  L -> buf = malloc(BUFSIZE);  //버퍼
}

void lddestroy(lines_t *L){
  if (L->fd >= 0){
    close(L->fd);
    L->fd = -1;
  }
  free(L->buf);
}


char *next_line(lines_t *L){
  if (L->fd < 0) return NULL;  // 파일 열리지 않음
  int line_start = L -> pos;   //현재 버퍼 내에서 읽기 시작할 위치
  if (DEBUG) printf("[%d]: pos %d/len %d\n", L->fd, L->pos, L->len);

  while(1){  // 무한루프 (새 줄을 찾거나 파일의 끝에 도달할 때까지)
    if(L->pos == L->len){  //현재 위치가 버퍼의 끝에 도달했는지 확인. 버퍼를 늘려서 더 받아들임.
      if (line_start == 0 && L->len == L->size) { //현재 줄이 버퍼 전체를 차지하고 있고, 버퍼가 모두 차 있는지 확인합니다. 이 경우, 버퍼 크기를 두 배로 확장합니다.
      L->size *= 2;
      L->buf = realloc(L->buf, L->size);
      if (DEBUG) printf("[%d]: expand buffer to %d\n", L->fd, L->size);
      }
      else if (line_start < L->pos){  //아직 읽지않은 데이터가 버퍼에 남아있는지 확인.
      int segment_length = L-> pos - line_start;  // 남아있는 데이터의 길이 계산
      memmove(L->buf, L->buf + line_start, segment_length); //처리되지 않은 데이터를 버퍼의 시작 부분으로 이동
      L->pos = segment_length;  //현재 위치를 이동한 데이터의 끝으로 설정
      }
      else{
      L->pos = 0;  //버퍼 비우기
      }  

    int bytes = read(L->fd, L->buf + L->pos, L->size - L->pos);
    //파일에서 버퍼로 데이터를 읽어들입니다
    if(bytes < 1) 
      break;    //파일의 끝 도달 / 더 읽을 데이터 없음

    L-> len = L-> pos + bytes;  //버퍼의 길이 업데이트
    line_start = 0;             //새 줄의 시작 위치를 0으로 설정
    if (DEBUG) printf("[%d]: Read %d bytes\n", L->fd, bytes);
  }
    if (DEBUG) printf("[%d]: %d/%d/%d/%d '%c'\n",
      L->fd, line_start, L->pos, L->len, L->size, L->buf[L->pos]);

    if (L->buf[L->pos] == '\n') {
      L->buf[L->pos] = '\0';
      L->pos++;
      return L->buf + line_start;
    }
    else{
      L-> pos++;
    }
  }

  //EOF
  close(L->fd);  //close file
  L->fd = -1;
  
  if(line_start < L->pos) {  //남아있는 데이터 처리
    if (L->pos == L->size){  //버퍼에 자리가 없다면
      L->buf = realloc(L->buf, L->size+1);  //버퍼를 키움
    }
    L-> buf[L->pos] = '\0';

    return L->buf + line_start;
  }
  
  return NULL;      
}



int main(int argc, char**argv){
  char *fname = argc > 1 ? argv[1] : "test.txt";

  int fd = open(fname, O_RDONLY);
  if (fd < 0){
    perror(fname);
    exit(EXIT_FAILURE);
  }

  lines_t L;
  ldinit(&L, fd);

  char *line;
  int n = 0;
  while ((line = next_line(&L)) && n < 10){  // line에 next_line 값 할당. 첫 10줄만 읽음
    printf("%d: |%s|\n", n, line);
    n++;
      }

  lddestroy(&L);

  return EXIT_SUCCESS;
  
}