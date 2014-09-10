/* expand.c -- expands tabs into spaces */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990704 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-t#] [files]\n", argv[0]), exit(2)

struct options {
	int tabstop; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.tabstop = 8; 
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
		case 't': 	
			opts.tabstop = atoi(ARG()); 
			if (opts.tabstop == 0)
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

#include "die_warn.c"	/* for die and warn */

static int 
expand(char *s) 
{
	FILE *f = stdin; 
	int c, col; 

	if (s && strcmp(s, "-") != 0) {
		if ((f = fopen(s, "r")) == NULL) {
			warn("can't open %s: $!", s); 
			return 1; 
		}
	} else {
		s = "<stdin>";
	}
	col = 0; 
	while ((c = getc(f)) != EOF) {
		if (c == '\t') {
			int n = opts.tabstop - (col % opts.tabstop); 
			col += n; 
			while (n--)
				putchar(' '); 
			continue; 
		}
		putchar(c); 
		col++; 
		if (c == '\n')
			col = 0; 
	}
	if (ferror(f)) {
		warn("error on %s: $!", s); 
		return 1; 
	}
	if (fclose(f) != 0) {
		warn("can't close %s: $!", s); 
		return 1; 
	}
	return 0; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	int ret_code=0; 

	if (ix == argc) {
		ret_code |= expand(0); 
	} else {
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			ret_code |= expand(s); 
		}
	}
	return ret_code; 
}
