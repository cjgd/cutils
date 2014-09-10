/* unesc.c -- delete \\ at end of lines */

/* $Id$ */
/* Carlos Duarte, 970225/990314 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-e esc_ch] [files]\n", argv[0]), exit(2)

struct options {
	char ch; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.ch = '\\'; 
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
		case 'e': {
			char *arg = ARG(); 
			if (strlen(arg) != 1)
				USAGE(); 
			opts.ch = *arg; 
			break; 
		}
		case '?': 
		default: 
			USAGE();
			break;
		}
	}
	return do_args(i, argc, argv); 
}

/* USER CODE */

/* perform s/[ ]*\\$// */
static void
unesc(char *fn) {
	FILE *f = stdin; 
	int spc,oc,c; 

	if (fn && strcmp(fn, "-") != 0 && (f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		return; 
	}
	spc=0; c= -2; 
	while ((oc=c, c = getc(f)) != EOF) {
		if (c == '\n') {
			if (oc>=0 && oc != opts.ch) {
				putchar(oc); 
				while (spc--)
					putchar(' '); 
			}
			putchar('\n'); 
			spc=0; c= -2; 
			continue; 
		}
		if (oc < 0)
			continue; 
		if (oc == ' ') {
			spc++; 
			continue; 
		}
		if (oc>=0) {
			while (spc--)
				putchar(' '); 
			spc = 0; 
			putchar(oc); 
		}
	}
	if (f != stdin)
		fclose(f); 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	if (ix == argc) {
		unesc(0); 
	} else for (; ix<argc; ix++) {
		unesc(argv[ix]); 
	}
	return 0; 
}
