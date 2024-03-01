// Reading through a file by line, enumerator-style.
//
// read_lines() iterates through the contents of a file, passing
// each line it encounters to the specified use_line function.
// read_lines() uses two buffers: one to hold the data received
// from read(), and one to store the line being constructed.
//
// Because each line is allocated before calling the function,
// care must be taken to free the lines once they are no longer
// needed.
//
// Note that this code divides the enumeration algorithm itself
// (the double loop and line allocation code) from the code that
// uses the lines. For simple programs, you may not need this
// degree of generality.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFSIZE
#define BUFSIZE 64
#endif

void read_lines(int fd, void (*use_line)(void *, char *), void *arg)
{
    char buf[BUFSIZE];
    char *line = NULL;
    int line_length = 0;
    int bytes, pos, line_start;

    while ((bytes = read(fd, buf, BUFSIZE)) > 0) {
	if (DEBUG) printf("read %d bytes\n", bytes);

	line_start = 0;
	for (pos = 0; pos < bytes; pos++) {
	    if (DEBUG) printf("start %d; pos %d; '%c'\n", line_start, pos, buf[pos]);

	    if (buf[pos] == '\n') {
		int line_segment_length = pos - line_start;
		int new_line_length = line_length + line_segment_length;
		line = realloc(line, new_line_length + 1);
		memcpy(line + line_length, buf + line_start, line_segment_length);
		line[new_line_length] = '\0';

		if (DEBUG) printf("Extended line by %d to %d\n", line_segment_length, new_line_length);

		use_line(arg, line);
		line = NULL;
		line_length = 0;

		line_start = pos + 1;
	    }

	}
	if (line_start < bytes) {
	    int line_segment_length = bytes - line_start;
	    int new_line_length = line_length + line_segment_length;
	    line = realloc(line, new_line_length + 1);
	    memcpy(line + line_length, buf + line_start, line_segment_length);
	    line[new_line_length] = '\0';
	    line_length = new_line_length;

	    if (DEBUG) printf("Extended line by %d to %d\n", line_segment_length, line_length);
	}

    }
    // TODO: check whether buffer contains anything
    if (line) {
	use_line(arg, line);
    }

}

void reverse_line(void *arg, char *l)
{
    int len = strlen(l);
    for (int i = len - 1; i >= 0; i--) {
	putchar(l[i]);
    }
    putchar('\n');
    free(l);
}



int main(int argc, char **argv)
{
    char *fname = argc > 1 ? argv[1] : "test.txt";

    int fd;
    if (strcmp(fname, "-") == 0) {
	fd = STDIN_FILENO;
    } else {
	fd = open(fname, O_RDONLY);
    }
    if (fd < 0) {
	perror(fname);
	exit(EXIT_FAILURE);
    }

    read_lines(fd, reverse_line, NULL);

    close(fd);

    return EXIT_SUCCESS;
}
