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

typedef struct {
    int fd;
    int pos;
    int len;
    int size;
    char *buf;
} lines_t;

void ldinit(lines_t *L, int fd)
{
    L->fd = fd;
    L->pos = 0;
    L->len = 0;
    L->size = BUFSIZE;
    L->buf = malloc(BUFSIZE);
}

void lddestroy(lines_t *L)
{
    if (L->fd >= 0) {
	close(L->fd);
	L->fd = -1;
    }
    free(L->buf);
}

char *next_line(lines_t *L)
{
    if (L->fd < 0) return NULL;

    int line_start = L->pos;

    if (DEBUG) printf("[%d]: pos %d/len %d\n", L->fd, L->pos, L->len);

    while (1) {
	if (L->pos == L->len) {
	    if (line_start == 0 && L->len == L->size) {
		L->size *= 2;
		L->buf = realloc(L->buf, L->size);
		if (DEBUG) printf("[%d]: expand buffer to %d\n", L->fd, L->size);

	    } else if (line_start < L->pos) {
		int segment_length = L->pos - line_start;
		memmove(L->buf, L->buf + line_start, segment_length);
		L->pos = segment_length;
		
	    } else {
		L->pos = 0;
	    }

	    int bytes = read(L->fd, L->buf + L->pos, L->size - L->pos);
	    if (bytes < 1) break;

	    L->len = L->pos + bytes;
	    line_start = 0;

	    if (DEBUG) printf("[%d]: Read %d bytes\n", L->fd, bytes);

	} 

	if (DEBUG) printf("[%d]: %d/%d/%d/%d '%c'\n",
	    L->fd, line_start, L->pos, L->len, L->size, L->buf[L->pos]);

	if (L->buf[L->pos] == '\n') {
	    L->buf[L->pos] = '\0';
	    L->pos++;
	    return L->buf + line_start;

	} else {
	    L->pos++;
	}
    }

    // EOF
    close(L->fd);
    L->fd = -1;

    if (line_start < L->pos) {
	if (L->pos == L->size) {
	    L->buf = realloc(L->buf, L->size+1);
	}
	L->buf[L->pos] = '\0';

	return L->buf + line_start;
    }

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
    }

    lddestroy(&L);

    return EXIT_SUCCESS;

}

