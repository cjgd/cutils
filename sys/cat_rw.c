/* cat_rw.c -- concatenate file, no options, use block sized read/write */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 981006 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static char *buffer; 
#define bsize (16384*2)

static int
cat(char *fn)
{
	struct stat sb; 
	int sb_ok=0;
	int fd, nr, bs; 

	if (!fn || strcmp(fn, "-") == 0) {
		fd = 0; 
	} else {
		if (stat(fn, &sb) != -1)
			sb_ok++;

		if ((fd = open(fn, O_RDONLY)) == -1) {
			perror(fn);
			return -1; 
		}
	}
	/*
	if (sb_ok) fprintf(stderr, "%s block size: %d\n", fn, sb.st_blksize); 
	*/

	bs = bsize; 
	if (sb_ok && sb.st_blksize <= bsize)
		bs = sb.st_blksize; 

	while ((nr = read(fd, buffer, bs)) > 0) {
		if (write(1, buffer, nr) != nr) {
			perror("bad write"); 
			return -1;
		}
	}
	if (nr < 0) {
		perror("bad read"); return -1; 
	}
	if (fd) {
		if (close(fd) != 0) {
			perror("bad close");
			return -1; 
		}
	}
	return 0; 
}

int
main(int argc, char *argv[])
{
	int err = 0; 
	int i; 
	if ((buffer = malloc(bsize)) == NULL) {
		perror("malloc"); 
		exit(1); 
	}
		
	if (argc <= 1) {
		if (cat(0) != 0)
			err=1;
	} else for (i=1; i<argc; i++) {
		if (cat(argv[i]) != 0)
			err=1; 
	}
	free(buffer);
	return err; 
}
