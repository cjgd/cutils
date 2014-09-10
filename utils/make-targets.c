/* make-targets.c -- find targets on a makefile */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970220/990321 */

/* 
 * very ugly.  very bad.  quick hack.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef NULL
#define NULL 0
#endif

#ifndef EOF
#define EOF (-1)
#endif

char *program_name; 

/* 
 * if `status' != 0, exit; else, just warn; 
 * if `error_no' != 0, print associated errno message
 */
static void
error(int status, int error_no, char *fmt, ... )
{
	va_list ap; 
	int did_write=0; 

	if (program_name) {
		fprintf(stderr, "%s: ", program_name); 
		did_write++; 
	}
	if (fmt) {
		va_start(ap, fmt); 
		vfprintf(stderr, fmt, ap); 
		va_end(ap); 
		did_write++; 
	}
	if (error_no) {
		fprintf(stderr, "%s", sys_errlist[error_no]); 
		did_write++; 
	}
	if (did_write) {
		fprintf(stderr, "\n"); 
		fflush(stderr); 
	}
	if (status) 
		exit(status); 
}

static char *
mk_progname(char *s)
{
	char *t; 

	if (s == NULL)
		return "????"; 

	if ((t = strrchr(s, '/')) != NULL)
		return t+1; 
	return s; 
}

/* forw decls, of functions needed by main */
static void usage(void); 
static void do_args(int,char *[]);
static void do_file(FILE *, char *); 
static void do_exit(void); 

int
main(int argc, char *argv[])
{
	program_name = mk_progname(argv[0]); 

	do_args(argc, argv); 
	if (optind >= argc)
		do_file(stdin, "stdin");
	else for (; optind < argc; optind++) {
		char *fn = argv[optind]; 
		FILE *f; 
		struct stat statbuf;

		if (strcmp(fn, "-") == 0) {
			do_file(stdin, "-");
		} else if ((f = fopen(fn, "r")) == NULL) {
			error(0, errno, "%s: ", fn); 
		} else if (fstat(fileno(f), &statbuf) < 0) {
			error(0, errno, "%s: ", fn);
			fclose(f); 
		} else if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
			error(0, 0, "`%s' is a directory!", fn);
			fclose(f); 
		} else {
			do_file(f, fn);
			fclose(f);
		}
	}
	do_exit(); 
	exit(0);
}

/* get line facility */

static void *
xcalloc(size_t n, size_t sz) {
	void *x = calloc(n, sz); 
	if (x==0) 
		error(1, errno, "calloc");
	return x; 
}

static void *
xmalloc(size_t n) 
{
	void *x = malloc(n); 

	if (x==0)
		error(1, errno, "malloc"); 
	return x; 
}

static void *
xrealloc(void *x, size_t n) 
{
	void *y = realloc(x,n); 

	if (y==0) {
		free(x); 
		error(1, errno, "realloc"); 
	}
	return y;
}

/* if `delim'<0, eat -delim out; else, include it on buffer */
static int
get_delim(FILE *f, char **bufp, size_t *dimp, int delim)
{
	const int chunk = 512; 
        char *top, *s;
        int c;
        int inc_delim;

	if (ferror(f) || feof(f))
		return -1; 

        if (*bufp == 0 || *dimp == 0) {
                *dimp = chunk;
                *bufp = xmalloc(*dimp);
        }

        inc_delim = delim>=0;
        if (delim < 0) 
                delim = -delim;
        else if (delim == 0)
                delim = '\n';

        s = *bufp;
        top = s + (*dimp - 2); /* leave two extra, for delim and \0 */
append: 
	do {
                if ((c = getc(f)) == -1) {
			if (feof(f) && s > *bufp)
				goto skip_delim; 
			return -1;
		}
                if (s >= top) {
                        char *t = xrealloc(*bufp, *dimp+chunk);
                        s = t + (s - *bufp);
                        *bufp = t;
                        *dimp += chunk;
                        top = *bufp + (*dimp - 2);
                }
                *s++ = c;
        } while (c != delim); 
        if (!inc_delim)
		s--; 
	if (s> *bufp && s[-1] == '\\' && s-1> *bufp && s[-2] != '\\') {
		s--; 
		goto append; 
	}
skip_delim: 
        *s = '\0';
	/* returns length of the useful part in buffer */
        return s - *bufp;
}

/****************************************************************/
/*                         USER CODE                            */
/****************************************************************/

#include <ctype.h>

/* message to print on invalid argument passing */
static void
usage(void) {
	fprintf(stderr, "\
usage: %s [files...]\n\
", program_name); 
	exit(2); 
}

/*
struct options {
} opts; 
*/

/* process cmd line args */
static void
do_args(int argc, char *argv[]) {

	for (;;) switch (getopt(argc, argv, "")) {
		case EOF: 
			goto out; 
		case '?': 
		default: 
			usage(); 
	}
out:
	; 
}

char *bline = 0; 
size_t bline_size = 0; 

/* run just before program ends */
static void
do_exit(void) {
	if (bline) 
		free(bline); 
}

void
print_target(char *beg, char *end) {
	char *s = beg; 
	while (s<end && isspace(*s))
		s++; 

	while (s<end && !isspace(*s)) {
		putchar(*s); 
		s++;
	}
	putchar('\n'); 
}

struct var {
	struct {
		char *var; 
		char *val;
	} *d; 
	int n; 
	int all; 
}; 

void
var_open(struct var *v) {
	v->n = 0; 
	v->all = 1024; 
	v->d = xcalloc(v->all, sizeof(*v->d)); 
}

void
var_close(struct var *v) {
	int i = v->n; 
	while (--i >= 0) {
		free(v->d[i].var); 
		free(v->d[i].val); 
	}
	free(v->d); 
}

void *
xstrdup(char *s) {
	char *t = xmalloc(strlen(s)+1); 
	strcpy(t, s); 
	return t; 
}

void
var_enlarge(struct var *v) {
	v->all += 1024; 
	v->d = xrealloc(v->d, v->all*sizeof(*v->d)); 
}

void
var_put(struct var *v, char *var, char *def) {
	int i; 
	for (i=0; i< v->n; i++) {
		if (strcmp(v->d[i].var, var) == 0) {
			free(v->d[i].val); 
			v->d[i].val = xstrdup(def); 
			return; 
		}
	}
	if (v->n >= v->all)
		var_enlarge(v); 
	v->d[v->n].var = xstrdup(var); 
	v->d[v->n].val = xstrdup(def); 
	v->n++; 
}

void
var_def(struct var *v, char *var_beg, char *def_beg) {
	char *s; 
	char *var, *def; 

	s = var_beg; 
	while (s < def_beg && isspace(*s))
		s++; 
	if (s>=def_beg)
		return; 
	var = s; 
	while (s<def_beg && !isspace(*s))
		s++; 
	*s = 0; 
	s = def_beg+1; 
	while (isspace(*s))
		s++;
	if (*s) {
		def = s; 
	} else {
		def = ""; 
	}

	var_put(v, var, def); 
}

#define L_PAREN '('
#define L_BRACE '{'
#define R_PAREN ')'
#define R_BRACE '}'

void
var_repl(struct var *v, char **bufp, size_t *lenp) {

#define add(from, to) do {                                                   \
	int len = to - from;                                                 \
	if (len == 0)                                                        \
		break;                                                       \
                                                                             \
	if (new_n + len >= new_size) {                                       \
		new_size += 1024 + len;                                      \
		new_buf = xrealloc(new_buf, new_size);                       \
	}                                                                    \
	strncpy(new_buf+new_n, from, len);                                   \
	new_n += len;                                                        \
} while (0); 

	char *s, *p, *pp; 
	char *new_buf; 
	int new_size; 
	int new_n = 0; 
	int i; 
	char *myvar; 

	s = *bufp;
	if ((p = strchr(s, '$')) == 0)
		return; 

	new_size = 1024; 
	new_buf= xmalloc(new_size); 

	do {
		add(s,p); 
		if (p[1] == '$') {
			s = p; 
			p += 2; 
			continue; 
		}
		if (p[1] == L_PAREN)
			pp = strchr(p+2, R_PAREN); 
		else if (p[1] == L_BRACE)
			pp = strchr(p+2, R_BRACE); 
		else {
			pp = p+1; 
			while (isalpha(*pp) || *pp == '_')
				pp++; 
		}
		if (!pp) {
			s = p; 
			p = 0; 
			break; 
		}
		p++; 
		if (*p == L_PAREN || *p == L_BRACE)
			p++; 
		if (pp-p<1) {
			/* got $... : a invalid var reference, skip */
			p = s = pp; 
			continue; 
		}
		myvar = xmalloc(pp-p+1); 
		strncpy(myvar, p, pp-p); 
		myvar[pp-p] = 0; 
		for (i=0; i<v->n; i++) {
			if (strcmp(myvar, v->d[i].var) == 0) {
				char *new = v->d[i].val; 
				char *end = new+strlen(new); 
				add(new, end); 
				break; 
			}
		}
		free(myvar); 
		if (*pp == R_PAREN || *pp == R_BRACE)
			pp++; 
		s = p = pp; 
	} while ((p = strchr(p, '$')) != NULL); 

	if (*s) {
		char *end = s+strlen(s)+1; 
		add(s, end); 
	}
	free(*bufp); 
	*bufp = new_buf; 
	*lenp = new_size; 
}

/* process file F, named FN */
static void
do_file(FILE *f, char *fn) {
	int len; 

	struct var v; 

	var_open(&v); 
	while ((len = get_delim(f, &bline, &bline_size, -'\n')) != -1) {
		/* simple parser: do vars subst first, 
		 * define a new var on =, get a target on : 
		 */

		char *p; 
		p = bline; 
		while (isspace(*p))
			p++; 
		if (*p == '#')
			continue; 

		var_repl(&v, &bline, &bline_size); 
		if ((p = strchr(bline, ':')) != NULL) {
			print_target(bline, p);
			continue; 
		}
		if ((p = strchr(bline, '=')) != NULL) {
			var_def(&v, bline, p); 
			continue; 
		}
		/* skip */
	}
	var_close(&v); 
}
