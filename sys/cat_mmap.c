/* cat_mmap.c -- cat utility, no options, use mmap */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 981006/990609 */

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>

void
cat(char *fn)
{
	struct stat sb; 
	char *s, *t; 
	int m,n;
	int fd; 
	int nw; 

	if (!fn || strcmp(fn, "-") == 0) {
		int c;
		while ((c = getc(stdin)) != EOF)
			putchar(c);
		return; 
	}

	if (stat(fn, &sb) == -1) {
		perror(fn); 
		return ;
	}
	if ((fd = open(fn, O_RDONLY)) == -1) {
		perror(fn);
		return ; 
	}
	if ((s=t = mmap(0, m=n = sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) 
			== (char *)-1) {
		fprintf(stderr, "error: mmap on %s: %s\n", fn, strerror(errno));
		return ; 
	}

	while ((nw = write(1, s, n)) >= 0) {
		if (nw == 0 || (n -= nw) == 0)
			break; 
		s += nw; 
	}
	/* should check if nw < 0, then error */
	if (close(fd) != 0)
		fprintf(stderr, "warn: on closing %s: %s\n", 
			fn, strerror(errno)); 
	if (munmap(t, m) != 0) 
		fprintf(stderr, "warn: on munmap: %s\n", strerror(errno));
}

int
main(int argc, char *argv[])
{
	int i; 
	if (argc <= 1) 
		cat(0);
	else for (i=1; i<argc; i++) {
		cat(argv[i]); 
	}
	return 0; 
}
