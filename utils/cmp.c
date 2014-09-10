/* cmp.c -- compare files */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990326/990327 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s file1 [file2]\n\
\n\
  -c      output c-like character representation, on diff bytes\n\
  -i N    skip first N bytes, before comparing\n\
  -ia N   skip N bytes of first file (file1)\n\
  -ib N   skip N bytes of second file (file2)\n\
  -n N    only compare N bytes\n\
  -l      be verbose, output all different bytes\n\
  -s      be silent, only exit some code, do not print\n\
\n\
if file2 is ommited, compares file1 against stdin.\n\
if one of file[12] is \"-\", use stdin for that file.\n\
only one stdin mention can be made.\n\
\n\
exit code: 0 files match; 1 files diff; 2 opts or sys error\n\
", argv[0]), exit(2)

enum verbosity {
	V_SILENT,
	V_NORMAL,
	V_ALL
}; 

struct options {
	int skip_a, skip_b; 
	int nuse;
	int pr_chars; 
	enum verbosity verb_lvl;
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.verb_lvl = V_NORMAL; 
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
		case 'c': 	
			opts.pr_chars = 1; 
			break; 
		case 'l': 
			opts.verb_lvl = V_ALL; 
			break; 
		case 's': 
			opts.verb_lvl = V_SILENT; 
			break; 
		case 'n': 
			opts.nuse = atoi(ARG()); 
			if (opts.nuse <= 0)
				USAGE(); 
			break; 
		case 'i': 
			if (s[1] == 'a') {
				s++; 
				opts.skip_a = atoi(ARG()); 
			} else if (s[1] == 'b') {
				s++; 
				opts.skip_a = atoi(ARG()); 
			} else
				opts.skip_a = opts.skip_b = atoi(ARG());
			if (opts.skip_a < 0 || opts.skip_b < 0)
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

static char *
escape(int c) {
	static char buf[11]; 
	char *s = buf; 

	if (c > 127) {
		*s++ = 'M'; 
		*s++ = '-'; 
		c -= 128; 
	} 
	if (c == 127) {
		*s++ = '^'; 
		*s++ = '?'; 
		goto ret; 
	}
	if (c >= ' ') {
		*s++ = c; 
		goto ret; 
	}
	*s++ = '\\'; 
        switch (c) {
        case '\a': *s++ = 'a'; break;
        case '\b': *s++ = 'b'; break;
        case '\f': *s++ = 'f'; break;
        case '\n': *s++ = 'n'; break;
        case '\r': *s++ = 'r'; break;
        case '\t': *s++ = 't'; break;
        case '\v': *s++ = 'v'; break;
	default: 
		s = buf; 
		*s++ = '^'; 
		*s++ = c+'@';
		break; 
	}
ret: 
	*s = 0; 
	return buf; 
}

static int
do_cmp(char *afname, char *bfname) {
	FILE *af, *bf; 
	int c1, c2; 
	int a_ch_count, b_ch_count; 
	int a_ln_count, b_ln_count; 
	int ret_code = 0; 
	char *afn, *bfn; 
	int na, nb, n; 
	int c; 

	a_ch_count = b_ch_count = 0; 
	a_ln_count = b_ln_count = 1; 
#define GETC(f) \
	( (c = getc(f)) == EOF ? EOF : \
		f == af ? (++a_ch_count, a_ln_count += c == '\n', c) \
			: (++b_ch_count, b_ln_count += c == '\n', c) )

	af = stdin; 
	bf = stdin; 
	if (afname && strcmp(afname, "-") != 0 && !(af = fopen(afname, "r"))) {
		perror(afname); 
		return 2; 
	}
	if (bfname && strcmp(bfname, "-") != 0 && !(bf = fopen(bfname, "r"))) {
		perror(bfname); 
		fclose(af); 
		return 2; 
	}
	afn = afname; if (af == stdin) afn = "(standard input)"; 
	bfn = bfname; if (bf == stdin) bfn = "(standard input)"; 

	na = opts.skip_a; 
	nb = opts.skip_b; 
	while (na || nb) {
		c1 = c2 = 0; 
		if (na) c1 = GETC(af); 
		if (nb) c2 = GETC(bf); 
		if (c1 == EOF && c2 == EOF)
			goto ret; 
		if (c1 == EOF) {
			ret_code = 1; 
			if (opts.verb_lvl != V_SILENT)
				printf("\
EOF on %s, while skipping %d bytes\n", afn, opts.skip_a); 
			goto ret; 
		}
		if (c2 == EOF) {
			ret_code = 1; 
			if (opts.verb_lvl != V_SILENT)
				printf("\
EOF on %s, while skipping %d bytes\n", bfn, opts.skip_b); 
			goto ret; 
		}
		if (na) na--; 
		if (nb) nb--; 
	}
	n = opts.nuse; 
	for (;;) {
		c1 = GETC(af); 
		c2 = GETC(bf); 

		if (c1 != c2)
			break; 

		if ((n && --n == 0) || c1 == EOF)
			goto ret; 
	}
	ret_code = 1; 
eof_print: 
	if (c1 == EOF) {
		if (opts.verb_lvl != V_SILENT) 
			printf("\
EOF on %s\n", afn); 
		goto ret; 
	}
	if (c2 == EOF) {
		if (opts.verb_lvl != V_SILENT) 
			printf("\
EOF on %s\n", bfn); 
		goto ret; 
	}
	if (opts.verb_lvl == V_NORMAL) {
		printf("%s %s differ: (char %d, line %d) (char %d, line %d)", 
			afn, bfn,
			a_ch_count, a_ln_count, 
			b_ch_count, b_ln_count); 

		if (opts.pr_chars) {
			printf(" %03o %s",  c1, escape(c1));
			printf(" %03o %s",  c2, escape(c2));
		}
		printf("\n"); 
		goto ret; 
	}
	if (opts.verb_lvl == V_ALL) {
		for (;;) {
			if (c1 != c2) {
				printf("%6d %03o ", a_ch_count, c1); 
				printf("%4s", opts.pr_chars ? escape(c1) : ""); 
				printf("    "); 
				printf("%6d %03o ", b_ch_count, c2); 
				printf("%4s", opts.pr_chars ? escape(c2) : ""); 
				printf("\n"); 
			}
			c1 = GETC(af); 
			c2 = GETC(bf); 
			if ((n && --n == 0) || (c1 == EOF && c2 == EOF))
				goto ret; 
			if (c1 == EOF || c2 == EOF)
				goto eof_print; 
		}
	}

ret:
	if (af != stdin) fclose(af); 
	if (bf != stdin) fclose(bf); 
	return ret_code; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	int nargs, a_is_stdin, b_is_stdin; 
	char *afile, *bfile; 

	a_is_stdin = 0; 
	b_is_stdin = 0; 
	nargs = argc-ix; 
	if (nargs == 1) {
		afile = argv[ix]; 
		bfile = 0; 
	} else if (nargs == 2) {
		afile = argv[ix]; 
		bfile = argv[ix+1]; 
	} else
		USAGE(); 
	if (!afile || strcmp(afile, "-") == 0) a_is_stdin = 1; 
	if (!bfile || strcmp(bfile, "-") == 0) b_is_stdin = 1; 

	if (a_is_stdin && b_is_stdin)
		USAGE(); 

	return do_cmp(afile, bfile); 
}
