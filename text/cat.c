/* cat.c -- cat utility: display file contents */

/* $Id$ */
/* Carlos Duarte, 981004 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_ALL 1
#define NUMBER_NON_BLANKS 2

/* flags */
static int visualize = 0; 
static int mark_end = 0; 
static int show_tabs = 0; 
static int number_lines = 0; 
static int squeeze_blanks = 0; 

static void 
warn(char *s) {
	fprintf(stderr, "%s -- could not read\n", s); 
}

static void 
die(char *s) {
	fprintf(stderr, "%s -- can not write\n", s); 
	exit(1); 
}

#define PUTCHAR(c) switch (putchar(c)) case EOF: die("putchar")

static int
cat(char *filename)
{
	FILE *f; 
	int c; 
	static int line_counter = 0; 	/* count lines */
	static int oc = '\n'; 		/* old character */
	static int ooc = -1; 		/* old old character */

	if (filename == NULL || strcmp(filename, "-") == 0)
		f = stdin; 
	else if ((f = fopen(filename, "r")) == NULL) {
			perror(filename); 
			return -1; 
	}
	while ((c = getc(f)) != EOF) {

		if (squeeze_blanks && ooc == '\n' && oc == '\n' && c == '\n')
			continue; 

		if (oc == '\n') {
			if (number_lines == NUMBER_ALL) {
				if (printf("%6d\t", ++line_counter) == EOF)
					die("printf"); 
			} else if (number_lines == NUMBER_NON_BLANKS 
				   && c != '\n') {
				if (printf("%6d\t", ++line_counter) == EOF) 
					die("printf"); 
			}
		}
		if (c == '\n') {
			if (mark_end) 
				PUTCHAR('$');
			PUTCHAR('\n');
		} else {
			if (c == '\t') {
				if (show_tabs) {
					PUTCHAR('^'); 
					PUTCHAR('I'); 
				} else
					PUTCHAR('\t'); 
			} else if (visualize) {
				if (c > 127) {
					PUTCHAR('M'); 
					PUTCHAR('-'); 
					c -= 128; 
				}
				if (c < ' ') {
					PUTCHAR('^'); 
					PUTCHAR(c+'@'); 
				} else if (c == 127) {
					PUTCHAR('^'); 
					PUTCHAR('?'); 
				} else
					PUTCHAR(c); 
			} else
				PUTCHAR(c); 
		}

		ooc = oc; 
		oc = c; 
	}

	if (ferror(f)) 
		warn(filename); 

	if (f != stdin)
		fclose(f); 

	return 0; 
}

static int
copy(char *fn) 
{
	int c; 
	FILE *f; 

	if (fn == NULL || strcmp(fn, "-") == 0)
		f = stdin; 
	else if ((f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		return -1; 
	}
	while ((c = getc(f)) != EOF)
		PUTCHAR(c); 

	if (ferror(f))
		warn(fn); 

	if (f != stdin)
		fclose(f); 
	return 0; 
}

int
main(int argc, char *argv[])
{
	int i; 
	int (*fn_cater)(char *); 

	for (i=1; i<argc && argv[i][0] == '-'; i++) {
		char *s = &argv[i][1]; 

		while (*s) switch (*s++) {
		case 'A': 
			visualize = mark_end = show_tabs = 1;
			break; 
			
		case 'b': 
			number_lines = NUMBER_NON_BLANKS; 
			break;
		case 'e': 
			visualize = mark_end = 1;
			break; 
		case 'E': 
			mark_end=1; 
			break; 
		case 'n': 
			number_lines= NUMBER_ALL; 
			break;
		case 's': 
			squeeze_blanks=1; 
			break;
		case 't': 
			show_tabs=visualize=1; 
			break;
		case 'T': 
			show_tabs=1; 
			break; 
		case 'u': 
			setbuf(stdout, NULL);
			break;
		case 'v': 
			visualize=1; 
			break; 
		default: 
			fprintf(stderr, 
			  "usage: cat [-AbeEnstTuv] [files ...]\n"); 
			exit(1); 
		}
	}

	fn_cater = cat; 
	if (visualize+mark_end+show_tabs+number_lines+squeeze_blanks == 0)
		fn_cater = copy; 

	if (i<argc)
		for (; i<argc; i++)
			fn_cater(argv[i]); 
	else
		fn_cater(NULL); 
	return 0; 
}
