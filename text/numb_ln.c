/* numb_ln.c -- number lines */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 981006/990314 */

#include <stdio.h>
#include <string.h>

int nl = 0; 
int oc='\n';
int c; 

void
ln(char *fn)
{
	FILE *f = stdin; 

	if (fn && strcmp(fn, "-") != 0 && (f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		return; 
	}
	while ((c = getc(f)) != EOF) {
		if (oc == '\n')
			printf("%6d\t", ++nl); 
		putchar(c); 
		oc = c; 
	}
	if (f != stdin)
		fclose(f);
}

int
main(int argc, char *argv[])
{
	int i; 
	if (argc <= 1) 
		ln(0); 
	else for (i=1; i<argc; i++) 
		ln(argv[i]); 
	return 0; 
}
