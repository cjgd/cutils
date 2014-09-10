/* fromdos.c -- convert CR LF to LF line endings */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970618/990320 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define CR 13
#define LF 10

char *
mytemp(void)
{
	static char fn[20]; 

	if (*fn == 0) {
		srand(time(0)); 
	}
	for (;;) {
		int r; 
		FILE *fd; 

		r = rand(); 
		sprintf(fn, "fd.%d!", r); 
		fd = fopen(fn, "r"); 
		if (!fd) 
			break; 
		fclose(fd); 
	}
	return fn; 
}

int
cvt_file(char *filename)
{
	FILE *f; 
	FILE *fout; 
	char *tmpfn; 
	int c; 

	if ((tmpfn = mytemp()) == 0) {
		perror("tmpnam"); 
		return -1; 
	}
	if ((f = fopen(filename, "r")) == 0) {
		perror(filename); 
		fclose(f); 
		return -1; 
	}
	if ((fout = fopen(tmpfn, "w")) == 0) {
		perror(tmpfn); 
		return -1; 
	}
	while ((c = getc(f)) != EOF) {
		if (c != CR) {
			putc(c, fout); 
			continue; 
		}
		if ((c = getc(f)) == EOF) {
			/* have CR eof... output CR? */
			putc(CR, fout); 
			break; 
		}
		if (c != LF)
			putc(CR, fout); 
		putc(c, fout); 
	}
	fclose(f); 
	fclose(fout); 

	if (unlink(filename) == -1) {
		perror(filename); 
		return -1; 
	}
	if (rename(tmpfn, filename) == -1) {
		perror("rename"); 
		fprintf(stderr, "%s could not be renamed to %s\n", 
			tmpfn, filename); 
		return -1; 
	}

	return 0; 
}

int
main(int argc, char *argv[])
{
	int i; 

	if (argc <= 1) {
		printf("usage: %s files...\n", argv[0]); 
		exit(2);
	}

	i = 0; 
	while (++i <argc) {
		cvt_file(argv[i]); 
	}
	return 0; 
}
