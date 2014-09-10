/* esc.c -- escapes lines (backslash terminated) */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 961201/990314 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-c #col] [-e esc_char] [-t #tabstop] [files]\n", argv[0]), exit(2)

struct options {
	int tabstop;
	char ch; 
	int col; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.tabstop = 8; 
	opts.ch = '\\'; 
	opts.col = 78; 
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
			if (opts.tabstop <= 0)
				USAGE(); 
			break; 
		case 'e': {
			char *arg = ARG(); 
			if (strlen(arg) != 1) 
				USAGE(); 
			opts.ch = *arg; 
			break; 
		}
		case 'c': 
			opts.col = atoi(ARG()); 
			if (opts.col <= 0)
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

static int
col_after_tab(int cur_col, int tabstop) {
	/* optimization for tabstop=8 : i = ((i-1)&(~07))+9; */
	return ((cur_col-1)/tabstop+1)*tabstop+1; 
}

static void
esc(char *fn) {
	FILE *f = stdin; 
	int c=0, oc; 
	int i; 

	if (fn && strcmp(fn, "-") != 0 && (f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		return; 
	}

	while (c != EOF) {
		c = -1; 
		i = 1; 
		while ((oc=c, c = getc(f)) != EOF && c != '\n') {
			if (c == '\t')
				i = col_after_tab(i, opts.tabstop); 
			else
				i++; 
			if (oc >= 0)
				putchar(oc); 
		}
		if (oc>=0)
			putchar(oc); 
		if (oc == opts.ch)
			goto next; 
		while (i<opts.col) {
			putchar(' '); 
			i++; 
		}
		putchar(opts.ch); 
	next: 
		putchar('\n'); 
	}
	if (f != stdin)
		fclose(f); 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	if (ix == argc) {
		esc(0); 
	} else for (; ix<argc; ix++) {
		esc(argv[ix]); 
	}
	return 0; 
}
