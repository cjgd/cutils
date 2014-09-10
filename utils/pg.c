/* pg.c -- a simple pager */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 980904 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-n] [-f file] [files]\n", argv[0]), exit(2)

struct options {
	int nr_lines; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.nr_lines = 60; 
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
			opts.nr_lines = atoi(ARG()); 
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

FILE *tty; 

int
pager(char *file)
{
	FILE *f = stdin; 
	int c; 
	int lines; 

	if (file && strcmp(file, "-") != 0 && (f = fopen(file, "r")) == NULL)
		return -1; 

	goto init; 
	while ((c = getc(f)) != EOF) {
		putchar(c); 
		if (c == '\n')
			lines++; 
		if (lines == opts.nr_lines) {
			for (;;) switch (getc(tty)) {
			case 'q':
			case 'Q': 
			case EOF: 
				return 1; /* quit now */
			case '\n': 
				goto init; 
			default: 
				break; /* just continue */
			}
		init: 
			printf("\n\n\n\n\n"); 
			lines=0; 
		}
	}
	if (f != stdin) 
		fclose(f); 
	return 0; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	opts.nr_lines -= 2; /* for getchar() */
	tty = fopen("/dev/tty", "r"); 
	if (!tty) {
		perror("/dev/tty"); 
		exit(1); 
	}
	if (argc <= 1) 
		pager(0);
	else for (; ix<argc; ix++) {
		if (pager(argv[ix]) == 1)
			break;
	}
	fclose(tty); 
	return 0; 
}
