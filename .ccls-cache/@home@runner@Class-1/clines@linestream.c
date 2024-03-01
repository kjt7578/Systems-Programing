// Reading through a file by line, iterator-style.
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
#define DEBUG 0
#endif

#ifndef BUFSIZE
#define BUFSIZE 16
#endif

typedef struct {
    int fd;
    int pos;
    int len;
    char buf[BUFSIZE];
} lines_t;

void ldinit(lines_t *L, int fd)
{
    L->fd = fd;
    L->pos = 0;
    L->len = 0;
}

char *next_line(lines_t *L)
{
    if (L->fd < 0) return NULL;

    char *line = NULL;
    int line_length = 0;
    int segment_start = L->pos;

    if (DEBUG) printf("[%d]: pos %d/len %d\n", L->fd, L->pos, L->len);

    while (1) {
	// refresh buffer once all chars are read
	if (L->pos == L->len) {
	    // preserve partial line segment
	    if (segment_start < L->pos) {
		int segment_length = L->pos - segment_start;
		line = realloc(line, line_length + segment_length + 1);
		memcpy(line + line_length, L->buf + segment_start, segment_length);
		line_length = line_length + segment_length;
		line[line_length] = '\0';
		// add terminator in case we are at the end of the file
		// and might return this line as-is

		if (DEBUG) printf("[%d]: extend line by %d to %d\n", L->fd, segment_length, line_length);
	    }

	    L->len = read(L->fd, L->buf, BUFSIZE);
	    if (DEBUG) printf("[%d]: read %d bytes\n", L->fd, L->len);

	    // at EOF, return the line in progress and mark the
	    // stream as closed
	    // NOTE: currently treats errors as EOF; could be more robust
	    if (L->len < 1) {
		close(L->fd);
		L->fd = -1;
		return line;
	    }

	    // reset state for the next segment
	    L->pos = 0;
	    segment_start = 0;
	}

	// search buffer for newline
	while (L->pos < L->len) {
	    if (L->buf[L->pos] == '\n') {
		if (DEBUG) printf("[%d]: newline at %d; start %d length %d\n", L->fd, L->pos, segment_start, line_length);
		int segment_length = L->pos - segment_start;
		line = realloc(line, line_length + segment_length + 1);
		memcpy(line + line_length, L->buf + segment_start, segment_length);
		line[line_length + segment_length] = '\0';
		L->pos++;

		return line;
	    }
	    L->pos++;
	}


    }

    //unreachable
    return NULL;
}


int main(int argc, char **argv)
{
    char *fname = argc > 1 ? argv[1] : "test.txt";

    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
	perror(fname);
	exit(EXIT_FAILURE);
    }

    lines_t L;
    ldinit(&L, fd);

    char *line;
    int n = 0;
    while ((line = next_line(&L)) && n < 10) {
	printf("%d: |%s|\n", n, line);
	n++;
	free(line);
    }

    return EXIT_SUCCESS;

}

