/* length_ln.c -- print each line, prefixed with its length */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 000223 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-m] [files]\n\
  -m    reports max and min line length only\n", argv[0]), exit(2)

struct options {
	int only_max; 
} opts; 

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
		case 'm': 
			opts.only_max++; 
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

struct buf {
	int size; 
	int len; 
	char pool[1]; 
}; 

struct buf *
new(int size)
{
	struct buf *b = malloc(sizeof(*b)+size); 
	if (b==0) {
		perror("malloc"); 
		exit(1); 
	}
	b->size = size; 
	b->len = 0; 
	return b; 
}

static void
stuff(struct buf *b, int c) 
{
	if (b->len >= b->size) {
		void *x = realloc(b, b->size*2); 
		if (x==0) {
			perror("realloc"); 
			exit(1); 
		}
		b = x; 
		b->size += 2; 
	}
	b->pool[b->len++] = c; 
	b->pool[b->len] = 0; 
}

static void 
reset(struct buf *b) 
{
	b->len = 0; 
	b->pool[0] = 0; 
}

static char *
buf(struct buf *b)
{
	return b->pool; 
}

static void 
count(char *fn) 
{
	int c, oc; 
	int ln, l, max, min, max_ln, min_ln; 
	FILE *f; 
	struct buf *b=0; 

	f = stdin; 
	if (fn && (f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		return; 
	}

	if (!opts.only_max) b = new(128); 

	max_ln = min_ln = ln = 1; 
	max = min = l = 0; 
	oc = '\n'; 
	for (;;) {
		c = getc(f); 
		if (c == '\n' || (c == EOF && oc != '\n')) {
			if (ln == 1 || l>max) {
				max_ln = ln; 
				max = l; 
			}
			if (ln == 1 || l<min) {
				min_ln = ln; 
				min = l; 
			}
			if (!opts.only_max) {
				printf("%6d %s", l, buf(b)); 
				if (c != EOF)
					putchar(c); 
				reset(b); 
			}
			ln++; 
			l = 0; 
			if (c == EOF) break; 
			continue; 
		} 
		if (c == EOF) break; 

		if (!opts.only_max) stuff(b, c); 
		l++; 
		oc = c; 
	}
	if (!opts.only_max) free(b); 

	if (f != stdin)
		fclose(f); 

	printf("%-18s min=%d (at line %d) max=%d (at line %d)\n", 
		fn ? fn : "stdin", min, min_ln, max, max_ln); 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	if (ix == argc) {
		count(0); 
	} else {
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			count(s); 
		}
	}
	return 0; 
}
