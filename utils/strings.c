/* strings.c -- find ascii sequences on binary data */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 980722/990314 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-n minlen] [-minlen] [-f] [files...]\n", argv[0]), exit(2)

struct options {
	int minlen; 
	int pr_fn; 
} opts; 

char *seq_hold; 

#define VALID(c) ((c)>=32  && (c)<127)

int
strings(char *fn) {
	FILE *f; 
	int state=1; 
	int c; 
	char *seq = 0; 

	f = stdin; 
	if (fn && (f = fopen(fn, "r")) == NULL)
		return -1; 

	if (!fn)
		fn = "standard input"; 

	while ((c = getc(f)) != EOF) {
	redo:
		switch (state) {
		case 0: 	/* searching for ascii */
			if (VALID(c)) {
				state=1; 
				seq = seq_hold; 
				goto redo; 
			}
			break; 
		case 1: 	/* holding sequence */
			if (VALID(c)) {
				*seq++ = c; 

				if (seq-seq_hold >= opts.minlen) {
					*seq++ = 0; 
					if (opts.pr_fn)
						printf("%s: ", fn);
					printf("%s", seq_hold);
					state = 2; 
				}
			} else 
				state = 0; 
			break; 

		case 2: 	/* write sequences */
			if (VALID(c)) {
				putchar(c); 
			} else {
				putchar('\n'); 
				state = 0; 
			}
			break; 
		}
	}
	if (state == 2) 
		putchar('\n'); 
	if (f != stdin) 
		fclose(f); 
	return 0; 
}

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.minlen = 4; 

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
		          case '1': case '2': case '3': case '4': 
		case '5': case '6': case '7': case '8': case '9':
			s--; 
			/* FALLTHROUGH */ 
		case 'n': 
			opts.minlen = atoi(ARG()); 
			if (opts.minlen == 0) 
				USAGE(); 
			break;
		case 'f':
			opts.pr_fn++; 
			break; 
		case '?': 
		default: 
			USAGE();
			break;
		}
	}

	if ((seq_hold = malloc(opts.minlen*sizeof(char))) == NULL) {
		perror("malloc"); exit(2);
	}
	if (i == argc) {
		strings(0); 
	} else {
		for (; i<argc; i++) {
			char *s = argv[i]; 
			strings(s); 
		}
	}
	free(seq_hold); 
	return 0; 
}
