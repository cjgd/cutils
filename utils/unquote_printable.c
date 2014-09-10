/* unquote_printable.c -- convert =XX to 8 bit chars */

/* $Id$ */
/* Carlos Duarte, 990207/990324 */

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

static char _ishex[] = {
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,
-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1, 
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
}; 

#define ishex(c) (_ishex[c]!=-1)

static void
unquote(char *fn) {
	FILE *f = stdin; 

	if (fn && strcmp(fn, "-") == 0 && (f = fopen(fn, "r")) != NULL) {
		perror(fn); 
		return; 
	}

	/* =XX -> hex(XX)
	 * =\n -> "" (empty)
	 */
	for (;;) {
		int c, c1, c2; 

		c = getc(f); 

	check: 
		if (c == EOF)
			break; 

		if (c != '=') {
			putchar(c); 
			continue; 
		}
		c1 = getc(f); 
		c2 = getc(f); 
		if (c1 == EOF) { 	/* =EOF */
			putchar('='); 
			break; 
		}
		if (c1 == '\n') {
			c = c2; 	/* =\n ? */
			goto check; 
		}
		if (c2 == EOF) {	/* =X EOF */
			putchar('='); 
			putchar(c1); 
			break; 
		}
		if (ishex(c1) && ishex(c2)) {	/* =HH */
			int v = _ishex[c1]*16 + _ishex[c2]; 
			putchar(v); 
			continue; 
		}
		/* =?? */
		if (c1 != EOF)
			putchar(c1); 
		if (c2 != EOF)
			putchar(c2); 
	}

	if (f != stdin) 
		fclose(f); 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	if (ix == argc) {
		unquote(0); 
	} else {
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			unquote(s); 
		}
	}
	return 0; 
}
