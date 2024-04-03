// Reading through a file by line, non-copying enumerator.
//
// read_lines iterates through the contents of a file, passing
// each line it encounters to the specified use_lines function.
// The strings passed to the use_lines function are stored in
// the buffer itself, meaning they do not persist between calls.
// This approach makes fewer memory allocations, and can be
// advantageous when the data segments do not need to be stored.
//
// Note that this code divides the enumeration algorithm itself
// (the double loop that calls read and scans through the buffer)
// from the code that uses the lines. For simple programs, you
// may not need this degree of generality.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFLENGTH
#define BUFLENGTH 16
#endif

//read_lines 함수:텍스트 파일에서 한 줄씩 읽어서 use_line 함수에 전달합니다. 이 함수는 파일 디스크립터 fd, use_line 함수 포인터, 그리고 use_line 함수에 전달할 arg를 인자로 받습니다. 이 함수의 핵심은 버퍼의 내용을 라인 단위로 처리하고, 라인이 너무 길어 버퍼를 초과하면 버퍼 크기를 늘리는 것입니다.
void read_lines(int fd, void (*use_line)(void *, char *), void *arg)
{//
    int buflength = BUFLENGTH;
    char *buf = malloc(BUFLENGTH);
  //먼저 버퍼의 길이를 설정하고, 이 길이만큼의 메모리를 동적으로 할당합니다.
    int pos = 0;    // 현재 위치
    int bytes;      // 읽어들인 바이트 수
    int line_start; // 라인 시작 위치
  //버퍼 내에서 현재 위치(pos), 읽어들인 바이트 수(bytes), 그리고 라인의 시작 위치(line_start)를 표시하는 변수를 초기화합니다.

  //이 루프는 read 함수를 사용해 파일에서 데이터를 읽어 버퍼에 저장합니다. read 함수는 읽어들인 바이트 수를 반환하며, 이 값이 0이하가 되면 파일의 끝에 도달했음을 의미하므로 루프를 종료합니다.
    while ((bytes = read(fd, buf + pos, buflength - pos)) > 0) {
	if (DEBUG) printf("read %d bytes; pos=%d\n", bytes, pos);

	line_start = 0;
	int bufend = pos + bytes;

//버퍼 내에서 라인을 찾아내고, 각 라인에 대해 use_line 함수를 호출합니다. 
	while (pos < bufend) {//버퍼가 다 차지 않았는데 줄 바꿈을 하였을 경우
	    if (DEBUG) printf("start %d, pos %d, char '%c'\n", line_start, pos, buf[pos]);

        if(buf[pos] == '\n' || buf[pos] == ' ' ){//여기서 buf[pos] == '\n'은 라인의 끝을 나타냅니다.
		// we found a line, starting at line_start ending before pos
		buf[pos] = '\0';       // 줄바꿈 \n 을 \0 (null) 로 바꿔줌
		use_line(arg, buf + line_start);  //print_line 호출
		line_start = pos + 1;  //다음 라인의 시작 위치를 설정
	    }  //50 while end
	    pos++;
	}  //47 while ernd

	// no partial line
	if (line_start == pos) {
	    pos = 0;
    }
	// partial line
	// move segment to start of buffer and refill remaining buffer
   else if (line_start > 0) {
	    int segment_length = pos - line_start;
	    memmove(buf, buf + line_start, segment_length); 
     //buf + line_start 위치에서 시작하는 segment_length 길이의 메모리 영역을 buf 위치로 이동
     //copies a specified number of bytes from one location to another (locations may overlap)
	    pos = segment_length;

	    if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
	
	// partial line filling entire buffer
	} else if (bufend == buflength) {//분할라인이 버퍼를 꽉 채울경우
	    buflength *= 2;
	    buf = realloc(buf, buflength); // 버퍼 크기를 키움

	    if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
	}
    }  //41 while end
    
    // partial line in buffer after EOF (End of File)
    if (pos > 0) {
	if (pos == buflength) {
	    buf = realloc(buf, buflength + 1);
	}
	buf[pos] = '\0';
	use_line(arg, buf + line_start);  //print_line 호출
    }

    free(buf);
    
}

//*********************************************************************************
//print_line 함수: read_lines 함수에서 호출되며, 읽은 라인을 화면에 출력합니다. 라인 번호와 함께 출력합니다.
void print_line(void *st, char *line)
//st는 read_lines 함수에서 전달되는 arg(n)를 가리키며, line은 읽은 줄의 내용을 가리킵니다.
{
    int *p = st;
  //st를 int * 타입의 포인터 p로 변환합니다. st는 main 함수에서 &n으로 전달되었으므로, p는 n의 주소를 가리킵니다.
    printf("%d: |%s|\n", *p, line);
  //라인 번호와 라인의 내용을 출력합니다. 라인 번호는 p가 가리키는 메모리 위치의 값, 즉 n의 값이며, 라인의 내용은 line에 저장된 문자열입니다.
    (*p)++;
  //p가 가리키는 메모리 위치의 값을 1 증가시킵니다. 이는 n의 값을 1 증가시키는 것과 같습니다.
}

//*********************************************************************************
//rev_line 함수: read_lines 함수에서 호출될 수 있으며, 읽은 라인을 역순으로 화면에 출력합니다.
void rev_line(void *st, char *line)
//st는 사용되지 않으며, line은 읽은 줄의 내용을 가리킵니다.
{
    int len = strlen(line);
  //line의 길이를 계산하여 len에 저장합니다.
  //line의 마지막 문자부터 처음 문자까지 역순으로 출력합니다.
    for (int i = len-1; i >= 0; i--) {
	putchar(line[i]);
    }
    putchar('\n');
}

//*********************************************************************************
//main 함수: 프로그램의 시작점입니다. 첫 번째 명령줄 인자로 지정된 파일을 열고, read_lines 함수를 호출하여 print_line 함수를 사용하여 파일의 내용을 출력합니다.
int main(int argc, char **argv)
{
    char *fname = argc > 1 ? argv[1] : "test.txt";
  /*argc (argument count)는 프로그램 실행 시 전달된 명령 줄 인자의 개수를 나타냅니다. 프로그램의 이름 자체도 인자로 간주되므로, 인자가 전달되지 않았을 때에도 argc의 값은 최소한 1입니다.
argv (argument vector)는 실제 명령 줄 인자들의 배열을 가리킵니다. 
각 인자는 문자열로 처리되며 argv[0]은 프로그램의 이름, argv[1]부터 argv[argc-1]까지가 실제 전달된 인자들입니다. argv[argc]는 NULL입니다.
예를 들어, 터미널에서 myprogram arg1 arg2라고 입력하여 프로그램을 실행하면, argc는 3이 되고 argv는 {"myprogram", "arg1", "arg2", NULL}이 됩니다.
항 연산자를 사용하여 명령 줄 인자가 1개 이상 주어졌을 경우 -> fname에 첫 번째 인자를 할당하고, 그렇지 않을 경우 fname에 "test.txt"를 할당합니다.
*/
    int fd = open(fname, O_RDONLY);
//  int xx = open(char *pathname, int flags); 
//open 함수를 사용하여 fname 파일을 읽기 전용(O_RDONLY)으로 엽니다. 파일 디스크립터를 fd에 저장합니다.
    if (fd < 0) {
//if open cannot open the file, it returns -1 and sets errno
	perror(fname);
	exit(EXIT_FAILURE);
    }

    int n = 0;
//라인 번호를 카운트하는 변수 n
  
    read_lines(fd, print_line, &n);
  //read_lines 함수를 호출하여 fd 파일 디스크립터에서 읽은 각 라인을 print_line 함수에 전달합니다. print_line 함수는 라인을 출력하고 n의 값을 1씩 증가시킵니다.
  //print_line 함수는 read_lines 함수에 의해 호출되며, 주어진 라인을 화면에 출력하는데, 이 때 라인 번호를 함께 출력합니다. 이 라인 번호는 n 변수에 저장되어 있습니다.

  
    //read_lines(fd, rev_line, NULL); // 역순읽기
    
    return EXIT_SUCCESS;
}