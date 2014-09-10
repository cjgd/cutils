/* unexpand.c -- unexpands spaces into tabs */

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
	int all; 
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
		case 'a': 
			opts.all=1; 
			break; 
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
unexpand(char *s)
{
	FILE *f = stdin; 

	if (s && strcmp(s, "-") != 0) {
		if ((f = fopen(s, "r")) == NULL) {
			warn("can't open %s: $!", s); 
			return 1; 
		}
	} else {
		s = "<stdin>";
	}
	if (!opts.all) {
		int count=0; 
		int beg=1;
		int c; 
		while ((c = getc(f)) != EOF) {
			if (beg && c == ' ') {
				if (++count == opts.tabstop) {
					putchar('\t');
					count=0; 
				}
			} else {
				beg=0; 
				if (count) do {
					putchar(' '); 
				} while (--count>0);
				putchar(c); 
				if (c == '\n') {
					count = 0; 
					beg=1; 
				}
			}
		}
	} else {
		int c; 
		int oc = ' '; 
		int col, space; 
		col = space = 0;
		while ((c = getc(f)) != EOF) {
			if (col++ %opts.tabstop == 0) {
				if (oc == ' ' && space>1) {
					putchar('\t');
					space = 0; 
				} 
			}
			if (c == ' ') {
				if (oc == ' ')
					space++; 
				else {
					oc = ' '; 
					space=1;
				}
			} else {
				if (oc == ' ' && space) 
					do putchar(' '); while (--space>0);

				putchar(c);
				oc = c;
				if (c == '\n') {
					space = col = 0; 
					oc = ' ';
				}
			}
		}
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
		ret_code |= unexpand(0); 
	} else {
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			ret_code |= unexpand(s); 
		}
	}
	return ret_code; 
}
