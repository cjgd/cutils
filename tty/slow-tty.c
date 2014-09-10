/* slow-tty.c -- retards tty output speed */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990326 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-n#] [files...]\n\
\n\
  eg: banner -w80 Foo bar bar bar foo... | ./a.out -n 8\n\
", argv[0]), exit(2)

struct options {
	int nr; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.nr = 10; 
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
		case 'n': 	
			opts.nr = atoi(ARG()); 
			if (opts.nr <= 0)
				USAGE(); 
			break; 
		case '?': 
		default: 
			USAGE();
			break;
		}
	}
	return do_args(i, argc, argv); 
}

/* USER CODE */

static void 
retard(char *fn) {
	int c; 
	FILE *f = stdin; 

	if (fn && strcmp(fn, "-") != 0 && (f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		return; 
	}
	setbuf(f, 0); 
	while ((c = getc(f)) != EOF) {
		int n = opts.nr; 

		putchar(c); 
		while (n--)
			putchar('\0'); 
	}
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	setbuf(stdout, 0); /** unbuffers */
	if (ix == argc) {
		retard(0); 
	} else {
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			retard(s); 
		}
	}
	return 0; 
}
