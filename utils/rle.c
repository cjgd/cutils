/* rle.c -- compress files, using run length encoding algorithm */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 980910/990319 */

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

/* minimum number of repetitions to enter on repetition mode */
#define MIN_NR_REP	3

static int
decomp(FILE *rd_f, FILE *wr_f) {
	int c; 

	while ((c = getc(rd_f)) != EOF) {
		if (c & 0x80) {	/* literal */
			int n = c&0x7f; 
			n++; 
			while (n--) {
				if ((c = getc(rd_f)) == EOF) {
					fprintf(stderr, 
			"Invalid input on literal count detected\n"); 
					return -1; 
				}
				putc(c,wr_f); 
			}
		} else {	/* repeat */
			int n = c; 
			n += MIN_NR_REP; 
			if ((c = getc(rd_f)) == EOF) {
				fprintf(stderr, 
			"Invalid input on repeat character detected\n"); 
				return -1; 
			}
			while (n--)
				putc(c,wr_f);
		}
	}
	return 0; 
}

/* bits 7 6 5 4 3 2 1 0
	1 ---- n ------ n literal
	0 ---- n ------ n repeats of following char
*/
static void
out_lit(FILE *wf, unsigned char *lit, int *len) {
	/* len = 1 .. 128 -> map into 0 .. 127 */
	putc((1<<7)|(*len - 1), wf); 
	while ((*len)--)
		putc(*lit++, wf); 
	*len = 0; 
}

static int
comp(FILE *rd_f, FILE *wr_f) {
	unsigned char lit[128+MIN_NR_REP-1]; 
	int lit_len = 0; 
	unsigned char c[MIN_NR_REP]; 
	int i; 
	int count; 

	count=0; 
	for (;;) {
		int cc; 
		while (count < MIN_NR_REP) {
			cc = getc(rd_f); 
			c[count++] = cc; 
			if (cc == EOF)
				goto out; 
		}
		for (i=1; i<count; i++) 
			if (c[i-1] != c[i])
				goto skip; 

		/* here: c[0]==c[1] && c[1]==c[2] && c[2]==c[3] ... */
		if (lit_len) 
			out_lit(wr_f, lit, &lit_len); 
		while ((cc = getc(rd_f)) == c[0])
			count++; 

		/* count: MIN_NR_REP..127+MIN_NR_REP, map into 0 .. 127 */
		while (count >= 127+MIN_NR_REP) {
			putc(127, wr_f); 
			putc(c[0], wr_f); 
			count -= 127+MIN_NR_REP; 
		}
		if (count >= MIN_NR_REP) {
			putc(count-MIN_NR_REP, wr_f); 
			putc(c[0], wr_f); 
			count = 0; 
		}
		c[count++] = cc; 
		if (cc == EOF)
			goto out; 
	skip: 
		lit[lit_len++] = c[0]; 
		if (lit_len == 128)
			out_lit(wr_f, lit, &lit_len); 
		count--; 
		for (i=0; i<count; i++)
			c[i] = c[i+1]; 
	}
out: 
	count--; 
	for (i=0; i<count; i++)
		lit[lit_len++] = c[i];
	/* len must be <= 128, and currently it may be 1..128+MIN_NR_REP-1 */
	if (lit_len) {
		int n = lit_len;
		if (n>128) {
			n = 128; 
			out_lit(wr_f, lit, &n);
			n = lit_len-128; 
			out_lit(wr_f, lit+128, &n); 
		} else
			out_lit(wr_f, lit, &lit_len); 
	}
	return 0; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	FILE *outf = stdout; 
	FILE *inf = stdin; 

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
		comp(inf, outf); 
	else
		decomp(inf, outf); 
	if (inf != stdin)
		fclose(inf); 
	if (outf != stdout)
		fclose(outf); 
	return 0; 
}
