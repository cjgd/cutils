/* bsub.c -- binary (or not) substitute */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990324/990507 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-f] [-e] [-g#] [-n#] [-i[.ext]] FROM_EXPR TO_EXPR [files]\n\
\n\
  -f       fold lower case (i.e. ignore case while searching FROM)\n\
  -e       interpret \?? as usual C escape sequences \n\
  -g#      ignore first # occurrences (def 0: replace the very first one)\n\
  -n#      change only # occurrences (def 0: subst all occurrences)\n\
  -i[ext]  edit files in place, creat backup by adding `ext', to each \n\
	     filename, if given.  by default, files are untouched, and\n\
	     results sent to stdout.\n\
", argv[0]), exit(2)

struct options {
	int icase; 
	int esc; 
	int nr_subs; 
	int nr_ign; 
	int do_edit; 
	char *ext; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 
	char *arg; int val; 

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
		case 'f':
			opts.icase = 1; 
			break; 
		case 'e':
			opts.esc = 1; 
			break; 
		case 'g': 
			arg = ARG(); 
			val = atoi(arg); 
			if (val < 0) USAGE(); 
			opts.nr_ign = val; 
			break; 
		case 'n':
			arg = ARG(); 
			val = atoi(arg); 
			if (val < 0) USAGE(); 
			opts.nr_subs = val; 
			break; 
		case 'i': 
			opts.do_edit = 1; 
			if (s[1])
				opts.ext = ARG(); 
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

#include <ctype.h>
#include <stdarg.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif

struct binstr {
	char *buf; 
	int len; 
}; 

static void 
warn(char *msg, ...) {
	va_list va; 
	va_start(va, msg); 
	vfprintf(stderr, msg, va); 
	va_end(va); 
}

static void 
die(char *msg, ...) {
	va_list va; 
	va_start(va, msg); 
	vfprintf(stderr, msg, va); 
	va_end(va); 
	exit(1); 
}

static char *
get_expr(char *arg, struct binstr *b) {
	char *s, *t; 

	char *res = malloc(strlen(arg)+1); 
	if (!res)
		return 0; 
	if (!opts.esc) {
		strcpy(res, arg); 
		b->buf = res; 
		b->len = strlen(arg); 
		return res; 
	}
	s = arg; 
	t = res; 
	for (;;) switch (*s) {
	case 0: goto ret; 
	case '\\': switch ((s++, *s++)) {
		case 0: *t++ = '\\'; goto ret; 
		case 'a': *t++ = '\a'; break; 
		case 'b': *t++ = '\b'; break; 
		case 't': *t++ = '\t'; break; 
		case 'n': *t++ = '\n'; break; 
		case 'v': *t++ = '\v'; break; 
		case 'f': *t++ = '\f'; break; 
		case 'r': *t++ = '\r'; break; 
		case '\\': *t++ = '\\'; break; 
		case '?': *t++ = '?'; break; 
		case '"': *t++ = '"'; break; 
		case '\'': *t++ = '\''; break; 
		case '0': case '1': case '2': case '3': 
		case '4': case '5': case '6': case '7': {
			int n = s[-1] - '0'; 
			if (*s>='0' && *s<='7') n = n*8 + *s++ - '0'; 
			if (*s>='0' && *s<='7') n = n*8 + *s++ - '0'; 
			*t++ = n; 
			break; 
		}
		default: 
			*t++ = s[-1]; break; 
		}
		break; 
	default: *t++ = *s++; break; 
	}
ret: 
	b->buf = res; 
	b->len = t-res; 
	return res; 
}

/* return -> 0: ok, -1: fatal err, -2: warn */

#define MATCH(a,b) (opts.icase ? tolower((a)) == tolower((b)) : (a) == (b))
#define PUTC(c,f) if (putc(c,f) != EOF) {} else return -1

static int
bsub(FILE *rf, FILE *wf, 
     struct binstr *old, struct binstr *new, struct binstr *orig) {
	int cc; 
	int len; 
	char *buf; 
	int i; 
	int nr_ign, nr_subs; 

	nr_ign = nr_subs = 0; 
	cc = old->buf[0]; 

	for (;;) {
		int c = getc(rf); 
	next: 
		if (c == EOF)
			break; 
		if (!MATCH(c, cc)) {
			PUTC(c, wf); 
			continue; 
		}
		/* found oldbuf[0], now try to match buf[1..oldlen] */
		orig->buf[0] = c; 
		len = 1; 
		while (len < old->len) {
			if ((c = getc(rf)) == EOF || !MATCH(c, old->buf[len])) {
				for (i=0; i<len; i++)
					PUTC(orig->buf[i], wf); 
				goto next; 
			}
			orig->buf[len++] = c; 
		}
		len = new->len;
		buf = new->buf;
		if (opts.nr_ign && nr_ign < opts.nr_ign) {
			nr_ign++; 
			len = orig->len;
			buf = orig->buf;
		} else if (opts.nr_subs) {
			if (nr_subs < opts.nr_subs) {
				nr_subs++; 
			} else {
				len = orig->len; 
				buf = orig->buf; 
			}
		}
		for (i=0; i<len; i++)
			PUTC(buf[i], wf); 
	}
	if (ferror(rf))
		return -2; 
	return 0; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	struct binstr old, new, tmp; 
	int nargs = argc-ix; 

	if (nargs < 2) 
		USAGE(); 

	if (get_expr(argv[ix++], &old) == 0) die("can not alloc mem\n"); 
	if (get_expr(argv[ix++], &new) == 0) die("can not alloc mem\n"); 

	tmp.len = old.len; 
	tmp.buf = malloc(tmp.len); 
	if (tmp.buf == NULL) 
		die("can not alloc memory\n"); 

	if (old.len == 0) {
		fprintf(stderr, "FROM_EXPR must have length >= 1\n"); 
		goto ret;
	}

	if (ix == argc) {
		int code = bsub(stdin, stdout, &old, &new, &tmp); 
		switch (code) {
		case -1: 
			die("could not write to stdout\n"); break; 
		case -2: 
			warn("stdin not completely read\n"); break; 
		}
		goto ret; 
	}
	for (; ix<argc; ix++) {
		char *s = argv[ix]; 
		FILE *rf, *wf; 
		char fnren[MAXPATHLEN]; 
		int tmp_used = 0; 

		if (opts.do_edit) {
			if (opts.ext) {
				strcpy(fnren, s); 
				strcat(fnren, opts.ext); 
			} else {
				if (tmpnam(fnren)==0) {
					warn("\
can't get temp filename for %s, skipping...\n", s); 
					continue;
				}
				tmp_used = 1; 
			}
			if (rename(s, fnren) == -1) {
				warn("\
could not rename %s into %s, skipping this file\n", s, fnren);
				continue; 
			}
			rf = fopen(fnren, "r"); 
			wf = fopen(s, "w"); 
		} else {
			rf = fopen(s, "r"); 
			wf = stdout; 
		}
		if (!rf || !wf) {
			warn("\
could not open files for reading or writing, skipping file %s\n", s); 
			if (rf && rf != stdin)
				fclose(rf); 
			if (wf && wf != stdout)
				fclose(wf); 
			continue; 
		}
		switch(bsub(rf, wf, &old, &new, &tmp)) {
		case -1: 
			die("failed to write on `%s'\n", 
				wf == stdout ? "stdout" : s); break; 
		case -2: 
			warn("could not completely read from `%s'\n", 
				wf == stdout ? s : fnren); break;
		}
		if (rf != stdin ) fclose(rf); 
		if (wf != stdout) fclose(wf); 
		if (tmp_used && remove(fnren) == -1) {
			fprintf(stderr, "\
could not clean up tmp files\n"); 
		}
	}
ret: 
	free(old.buf); 
	free(new.buf); 
	free(tmp.buf); 
	return 0; 
}
