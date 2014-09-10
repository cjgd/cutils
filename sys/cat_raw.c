/* cat_raw.c -- cat utility (no options, use read/write) */

/* $Id$ */
/* Carlos Duarte, 981004 */

#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include <stdarg.h>

#ifndef NULL
#define NULL 0
#endif

char buffer[4096]; 

void
error(char *s, ...)
{
	va_list ap; 
	va_start(ap, s); 
	while (s) {
		write(2, s, strlen(s)); 
		s = va_arg(ap, char *); 
	}
	write(2, "\n", 1); 
	va_end(ap); 
}

static int
copy(char *fn) 
{
	int fd;
	int nr; 

	if (fn == NULL || strcmp(fn, "-") == 0)
		fd = 0; 
	else if ((fd = open(fn, 0)) == -1) {
		error("can not open ", fn, 0); 
		return -1; 
	}
	while ((nr = read(fd, buffer, 4096)) > 0) {
		if (write(1, buffer, nr) != nr) {
			error("incomplete write on stdout", 0); 
			return -1; 
		}
	}

	if (nr < 0) {
		error("bad read from ", fn, 0); 
		return -1; 
	}

	if (fd != 0) {
		if (close(fd) != 0) {
			error("bad close of ", fn, 0); 
			return -1; 
		}
	}
	return 0; 
}

int
main(int argc, char *argv[])
{
	int i = 1; 
	int err = 0; 

	if (i<argc) {
		for (; i<argc; i++) {
			if (copy(argv[i]) == -1) 
				err=1; 
		}
	} else {
		if (copy(NULL) == -1) 
			err=1; 
	}
	return err; 
}
