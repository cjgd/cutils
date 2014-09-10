/* rle2.c -- 
 * 	compress files, using run length encoding algorithm 
 * 	use big (variable length) counts
 */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990319 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-d] [-o outfile] [infile]\n", argv[0]), exit(2)

struct options {
	int comp; 
	char *ofile; 
	char *ifile; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.comp = 1;
	opts.ofile = 0; 
	opts.ifile = 0; 
	for (i=1; i<argc; i++) {
		char *t, *s = argv[i]; 
		t=0; /* shut up warnings */
		if (s[0] != '-' || s[1] == 0) {
			if (opts.ifile)
				USAGE(); 
			opts.ifile = argv[i]; 
			continue; 
		}
		while (*++s) switch (*s) {
		case 'd': 
			opts.comp = 0; 
			break; 
		case 'o': 
			if (opts.ofile)
				USAGE(); 
			opts.ofile = ARG(); 
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

/*
 * format of compressed data: 
 * 
 * literal [repeat [literal [repeat...]]]
 * 
 * literal: count <count bytes>
 * 	the next count bytes, are output as they are
 * 
 * repeat: count <repeat byte>
 * 	the next byte, is repeated count times
 * 
 * count: c1 [remainder_count]
 * 	if c1 in 0 .. 251, c1 is the count
 * 	c1 == 252	, remainder_count has 4 bytes (256-c1)
 * 	c1 == 253	, remainder_count has 3 bytes (256-c1)
 * 	c1 == 254	, remainder_count has 2 bytes (256-c1)
 * 	c1 == 255	, remainder_count has 1 bytes (256-c1)
 * 	and count will be 252+remainder_count
 * 
 * remainder_count: c1 c2 ...
 * 	an integer, in big endian format (c1, least significative)
 */

int
put_count(FILE *wf, unsigned int count) {
	/* max count: 4 bytes len */
	unsigned char c[4]; 
	unsigned int pos; 

	if (count < 252) {
		putc(count, wf); 
		return 0; 
	}
	count -= 252; 
	pos = 0; 
	do {
		c[pos++] = count&255; 
		count >>= 8; 
	} while (count); 
	putc(256-pos, wf); 
	while (pos>0) {
		--pos; 
		putc(c[pos], wf); 
	}
	return 0; 
}

/* ret count, or -1 on error */
int
get_count(FILE *rf) {
	int count = getc(rf); 
	int n, cnt; 

	if (count == EOF)
		return -1; 

	if (count < 252)
		return count; 

	n = 256-count; 
	cnt = 0; 
	while (n--) {
		int c = getc(rf); 
		if (c == EOF)
			return -1; 

		cnt = cnt * 256 + c; 
	}
	return cnt+252; 
}

static int 
decomp(FILE *rf, FILE *wf) {
	int n, c; 

	for (;;) {
		/* first lit, then repeat */
		if ((n = get_count(rf)) == EOF)
			break; 
		while (n--) {
			c = getc(rf); 
			if (c == EOF)
				return -1; 
			putc(c, wf);
		}
		/* repeat */
		if ((n = get_count(rf)) == EOF)
			break; 
		c = getc(rf); 
		if (c == EOF)
			return -1; 
		while (n--) 
			putc(c, wf); 
	}
	return 0; 
}

struct buf {
	char *b; 
	int len;
	int size; 
};

typedef struct buf BUF;

int
b_open(BUF *b) {
	b->size = 1024; 
	b->b = malloc(b->size);
	if (b->b == 0)
		return -1; 
	b->len = 0; 
	return 0; 
}

void
b_close(BUF *b) {
	if (b->b)
		free(b->b); 
}

int
b_add(BUF *b, int c) {
	if (b->len+1 >= b->size) {
		void *x; 
		b->size *= 2; 
		x = realloc(b->b, b->size); 
		if (!x) {
			free(b->b); 
			return -1; 
		}
		b->b = x; 
	}
	b->b[b->len++] = c; 
	return 0; 
}

static int
comp(FILE *rf, FILE *wf) {
	BUF b; 
	int w0, w1, w2; 
	int count; 

	if (b_open(&b) == -1)
		return -1; 
	w0 = getc(rf); 
	for (;;) {
		w1 = getc(rf); 
		while ((w2 = getc(rf)) != EOF && (w2 != w1 || w2 != w0)) {
			if (b_add(&b, w0) == -1) 
				return -1; 
			w0 = w1; w1 = w2; 
		}
		if (w2 == EOF) {
			if (w0 != EOF && b_add(&b, w0) == -1) return -1; 
			if (w1 != EOF && b_add(&b, w1) == -1) return -1; 
		}
		put_count(wf, b.len); 
		fwrite(b.b, sizeof(*(b.b)), b.len, wf); 
		if (w2 == EOF)
			break; 
		b.len = 0; 
		count = 3; 
		while ((w2 = getc(rf)) == w1)
			count++; 
		put_count(wf, count); 
		putc(w1, wf); 
		if (w2 == EOF)
			break; 
		w0 = w2; 
	}
	b_close(&b); 
	return 0; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	FILE *outf = stdout; 
	FILE *inf = stdin; 
	int errcode = 0; 

	if (ix != argc) 
		USAGE(); 

	if (opts.ifile) {
		inf = fopen(opts.ifile, "r"); 
		if (!inf) {
			perror(opts.ifile);
			exit(1); 
		}
	}
	if (opts.ofile) {
		outf = fopen(opts.ofile, "w"); 
		if (!outf) {
			fclose(inf); 
			perror(opts.ofile); 
			exit(1); 
		}
	}
	if (opts.comp)
		errcode = comp(inf, outf); 
	else
		errcode = decomp(inf, outf); 

	if (errcode != 0) {
		fprintf(stderr, "Error on file %s\n", opts.ifile ? 
			opts.ifile : "(stdin)"); 
	}
	if (inf != stdin)
		fclose(inf); 
	if (outf != stdout)
		fclose(outf); 
	/* ret 0: ok, 1: error */
	return -errcode; 
}
