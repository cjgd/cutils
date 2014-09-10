/* grep.c -- search regular expressions on text */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990712/000126 */

/***************************

TODO: 
. implementar [] class match
. implementar acontext, bcontext, fixed
. grep engine buggy -- nao posso procurar pelo ultimo char em .*c

****************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_MATCH 		1
#define FILE_NO_MATCH 		2

static int do_args(int first, int ac, char *av[]); 
static void pat_add(char *); 
static void pat_fadd(char *); 
static void pat_free(void); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-opts] pattern [files]\n\
  -f file  read one pattern per line from file\n\
  -e pat   add PAT to list of patterns to use\n\
  -i       ignore case\n\
  -w       only match words\n\
  -s       silent. no output.\n\
  -v       revert match (i.e. output results that don't match)\n\
  -n       print line numbers also, for each match\n\
  -H       force output of file name\n\
  -h       never outputs file name\n\
  -L       only output file names that contain no match\n\
  -l       only output file names that contain matches\n\
  -c       count occurences\n\
  -B #     output # lines before each match\n\
  -A #     output # lines after each match\n\
  -C #     like -B # -A #\n\
  -d       dashes matched patterns\n\
  -F       fgrep. treat patterns literally\n", argv[0]), exit(2)

struct options {
	char **pat;
	int icase; 
	int words;
	int silent; 
	int negate_match; 
	int pr_line_nr; 
	int pr_fn;
	int pr_fn_only;
	int count; 
	int acontext; 
	int bcontext; 
	int dashed; 
	int fixed; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; char *arg; 

	opts.pr_fn = -1; 
	opts.pr_fn_only = -1; 
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
		case 'e': arg = ARG(); pat_add(arg); break; 
		case 'f': arg = ARG(); pat_fadd(arg); break; 
		case 'i': opts.icase++; break; 
		case 'w': opts.words++; break; 
		case 's': opts.silent++; break; 
		case 'v': opts.negate_match++; break; 
		case 'n': opts.pr_line_nr++; break; 
		case 'h': opts.pr_fn=0; break; 
		case 'H': opts.pr_fn=1; break; 
		case 'l': opts.pr_fn_only=FILE_MATCH; break; 
		case 'L': opts.pr_fn_only=FILE_NO_MATCH; break; 
		case 'c': opts.count++; break; 
		case 'B': arg = ARG(); opts.bcontext = atoi(arg); break; 
		case 'A': arg = ARG(); opts.acontext = atoi(arg); break; 
		case 'C': {
			int n; 
			arg = ARG(); 
			n = atoi(arg); 
			opts.bcontext = opts.acontext = n; 
			break; 
		}
		case 'F': opts.fixed++; break; 
		case 'd': opts.dashed++; break; 
		case '?': 
		default: 
			USAGE();
			break;
		}
	}
	if (opts.pat == 0) {
		if (i >= argc) 
			USAGE(); 

		pat_add(argv[i]); 
		i++; 
	}
	if (opts.pr_fn == -1) {
		if (argc-i >= 2)
			opts.pr_fn = 1; 
		else
			opts.pr_fn = 0; 
	}

	i = do_args(i, argc, argv); 
	pat_free(); 
	return i; 
}

/* USER CODE */

#include <ctype.h>
#include "die_warn.c"

#define EQ(a,b) ((a)==(b) || (a)=='.')

static void
pat_add(char *re) 
{
	char **p = opts.pat; 
	int i; 

	i = 0; 
	if (!p) {
		p = malloc(sizeof(*p)*2); 
	} else {
		while (*p) {
			p++; 
			i++; 
		}
		p = realloc(opts.pat, sizeof(*opts.pat)*(i+1)); 
	}
	if (!p)
		die("alloc"); 
	if ((p[i] = strdup(re)) == 0)
		die("strdup"); 
	p[i+1] = 0; 
	opts.pat = p; 
	if (opts.icase) {
		unsigned char *s; 
		for (s=p[i]; *s; s++)
			*s = tolower(*s); 
	}
}

static void
pat_fadd(char *file) 
{
	FILE *f = stdin; 
	char buf[4096]; 

	if (file && strcmp(file, "-") != 0 && (f = fopen(file, "r")) == NULL) {
		die("can't open %s: $!", file); 
	}
	while (fgets(buf, sizeof buf, f)) {
		int n = strlen(buf);
		if (n>0 && buf[n-1] == '\n')
			buf[n-1] = 0;
		pat_add(buf); 
	}
	if (ferror(f))
		die("error on file %s: $!", file ? "<stdin>" : file); 

	if (f != stdin)
		fclose(f); 
}

static void
pat_free(void) 
{
	char **p = opts.pat; 
	if (!p)
		return; 
	while (*p) {
		free(*p); 
		p++; 
	}
	free(opts.pat); 
}

/* returns pointer to match (on line), and sets *lenp with length of match */
static char * do_match(char *line, char *re, int *lenp); 
static char * 
do_match(char *line, char *re, int *lenp)
{
	int dollar=0, caret=0; 
	char *r=0, *l=0; 

	if (re[0] == '^') {
		caret++; 
		re++; 
	}
	for (;;line++) {
		r = re; 
		l = line; 
	redo: 
		if (r[0] == 0) {
			if (!dollar) {
				/* empty re: matches everything */
				if (r == re) l += strlen(l); 
				break; 
			}
			if (l[0] == 0) break; /* match at end of line */
			if (re[0] && !re[1]) {
				/* empty re: match everything */
				if (caret) return 0; 
				l += strlen(l); 
				break; 
			}
			continue; 
		}
		if (r[0] == '$' && !r[1]) { dollar++; r++; goto redo; }
		if (r[0] == '\\' && r[1]) { r++; goto literal; }
		if (r[1] == '*') {
			int c = r[0]; 
			r += 2; /* in "xpto.*z", c='o', r points to 'z' */
			if (c != '.') {
				/* search for 0 or more c */
				while (*l == c) 
					l++; 
				goto redo; 
			}
			c = *r; /* in "xpto.*z", c='z' */
			if (!c || c == '$') {
				/* ".*" everything up */
				l += strlen(l); 
				break; 
			}
			if (c == '\\' && r[1]) 
				c = *++r; 
			if (c != '.') {
				l = strrchr(l, c);  
				if (!l) return 0; 
				l++; 
				r++; 
				goto redo; 
			}
			/* special case: "xpto.*.", r points to last '.' */
			{ char *best_pos=0; int best_len=0; 
			  char *pos; int len; 
			while (*l) {
				pos = do_match(l, r, &len); 
				if (pos) {
					len += pos-line; 
					if (len > best_len) {
						best_pos = pos; 
						best_len = len; 
					}
				}
				l++; 
			}
			if (!best_pos) return 0; 
			l = line+len; break; }
		}
		if (l[0] == 0) return 0; 
	literal: 
		if (r[0] == '.' || *l == r[0]) {
			l++; 
			r++; 
			goto redo; 
		}
		if (caret) return 0; 
	}
	if (lenp)
		*lenp = l - line; 
	return line; 
}

static int 
match(char *line, char **posp, int *lenp)
{
	char **p; 
	char *pos=0; int len=0; 
	int found=0; 

	for (p=opts.pat; *p; p++) {
		pos = do_match(line, *p, &len); 
		if (pos && !opts.words) {
			found=1; 
			break; 
		}
		if (pos && opts.words && 
		    (pos == line || !isalnum(pos[-1])) && 
		    (pos[len] == '\0' || !isalnum(pos[len]))) {
			found = 1; 
			break;
		}
	}
	if (posp) *posp = pos; 
	if (lenp) *lenp = len; 
	return found; 
}

static void
outtab(char *s, int len, int outc, int *colp) 
{
	int i,k; 
	if (!s) {
		*colp += len; 
		while (len-- > 0) 
			putchar(outc); 
			return ; 
	}
	for (i=0; i<len; i++) {
		if (s[i] != '\t') {
			*colp += 1;
			putchar(outc); 
			continue; 
		}
		k = 8-(*colp)%8; 
		while (k-- > 0) {
			*colp += 1;
			putchar(outc); 
		}
	}
}

static int
grep(char *file) 
{
	FILE *f; 
	char *fn; 
	char buf[4096], buf_i[4096]; 
	char *pos; int len; 
	int line_nr = 0; 
	int count=0; 

	fn = "<stdin>"; f = stdin; 
	if (file && strcmp(fn = file, "-") != 0 && 
				(f = fopen(file, "r")) == NULL) {
		warn("can't open %s: $!", file); 
		return 2; 
	}

	while (fgets(buf, sizeof buf, f)) {
		int found=0, offset=0; 
		int n = strlen(buf); 
		if (n>0 && buf[n-1] == '\n')
			buf[n-1] = 0;
		line_nr++; 

		if (opts.icase) {
			unsigned char *s,*t; 
			t = buf_i; 
			for (s=buf; *s; s++)
				*t++ = tolower(*s); 
			*t = 0; 
			found = match(buf_i, &pos, &len); 
		} else
			found = match(buf, &pos, &len); 

		if (found && !opts.negate_match) {
			count++; 
			goto output; 
		} 
		if (!found && opts.negate_match) {
			count++; 
			goto output; 
		}
		continue; 

	output: 
		if (opts.silent || opts.count || opts.pr_fn_only>=0) continue; 
		if (opts.pr_fn) offset += printf("%s:", fn); 
		if (opts.pr_line_nr) offset += printf("%d:", line_nr); 
		fputs(buf, stdout); 
		fputs("\n", stdout); 
		if (opts.dashed && found && len) {
			int col=0; 
			outtab(0, offset, ' ', &col); 
			if (opts.icase) {
				outtab(buf_i, n=pos-buf_i, ' ', &col); 
				outtab(buf_i+n, len, '-', &col); 
			} else {
				outtab(buf, n=pos-buf, ' ', &col); 
				outtab(buf+n, len, '-', &col); 
			}
			putchar('\n'); 
		}
	}
	if (ferror(f)) {
		warn("error on file %s: $!", fn); 
		return 2; 
	}
	if (f != stdin && fclose(f) != 0) {
		warn("couldn't close %s: $!", fn); 
	}
	if (opts.count) {
		if (opts.pr_fn) printf("%s:", fn); 
		printf("%d\n", count); 
	}
	if ((opts.pr_fn_only == FILE_MATCH && count) ||
	    (opts.pr_fn_only == FILE_NO_MATCH && !count)) printf("%s\n", file); 
	return !count; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	int status = 0; 
	if (ix == argc) {
		status = grep(0); 
	} else {
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			int st = grep(s); 
			if (st > status) status = st; 
		}
	}
	return status; 
}
