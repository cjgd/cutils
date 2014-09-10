/* caesar-add.c -- sums each byte of input to a given increment */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990324/990507 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-n incr] [files]\n", argv[0]), exit(2)

struct options {
	int inc; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.inc = 12; 
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
			opts.inc = atoi(ARG()); 
			if (opts.inc <= 0)
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
die(char *s) {
	fprintf(stderr, "%s\n", s);
	exit(1);
}

static void
add(char *fn) {
	FILE *f; 
	int c; 

	f = stdin; 
	if (fn && strcmp(fn, "-") != 0 && (f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		exit(1);
	}
	while ((c = getc(f)) != EOF) {
		register int d = (c+opts.inc)&0xff; 
		if (putchar(d) == EOF) 
			die("putchar"); 
	}
	if (ferror(f)) 
		fprintf(stderr, "\
warning: %s: could not be read entirely\n", fn); 

	if (f != stdin)
		fclose(f); 
}


static int
do_args(int ix, int argc, char *argv[]) 
{
	if (ix == argc) {
		add(0); 
	} else {
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			add(s); 
		}
	}
	return 0; 
}
