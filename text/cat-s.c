/* cat-s.c -- 
 * 	output all input lines, without sequencial blanks 
 * 	acts like cat -s 
 */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970109/990321 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [files]\n", argv[0]), exit(2)

/*
struct options {
} opts; 
*/

int
main(int argc, char *argv[]) 
{
	int i; 

	for (i=1; i<argc; i++) {
		char *t, *s = argv[i]; 
		t=0; /* shut up warnings */
		if (s[0] != '-' || s[1] == 0) 
			break; 
		if (s[1] == '-' && s[2] == 0) {
			++i;
			break; 
		}
		while (*++s) switch (*s) {
		case '?': 
		default: 
			USAGE();
			break;
		}
	}
	return do_args(i, argc, argv); 
}

/* USER CODE */

#define PUTCHAR(c) switch (putchar(c)) case EOF: \
fprintf(stderr, "putchar error: can't write\n"), exit(1)

static void
cats(char *fn) {
	FILE *f = stdin; 
	int c0, c1, c; 

	if (fn && strcmp(fn, "-") != 0 && (f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		return; 
	}
	c0 = getc(f); 
	c1 = getc(f); 
	for (;(c = getc(f)) != EOF; c0 = c1, c1 = c) {
		if (c != '\n' || c1 != '\n' || c0 != '\n') {
			PUTCHAR(c0); 
			continue;
		}
	}
	if (c0 != EOF) PUTCHAR(c0); 
	if (c1 != EOF) PUTCHAR(c1); 

	if (ferror(f)) 
		fprintf(stderr, "warning: %s: could not read entirely\n", fn); 

	fclose(f); 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	if (ix == argc) {
		cats(0); 
	} else {
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			cats(s);
		}
	}
	return 0; 
}
