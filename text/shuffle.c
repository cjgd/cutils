/* shuffle.c -- mix input lines */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 981225 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

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
skip_delim: 
        *s = '\0';
	/* returns length of the useful part in buffer */
        return s - *bufp;
}

/****************************************************************/
/*                         USER CODE                            */
/****************************************************************/

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

typedef struct {
	char **av; 
	int alloc; 
	int len; 
} SW; 

static SW *sw = NULL; 

static char *bline = 0; 
static size_t bline_size = 0; 

static SW *
sw_open(void) 
{
	SW *x; 
	x = xmalloc(sizeof(*x)); 
	x->av = malloc(512 * sizeof(*(x->av))); 
	x->alloc = 512; 
	x->len = 0; 
	return x; 
}

static void
sw_close(SW *s)
{
	char **av; 
	int i; 
	av = s->av; 
	i = s->len; 
	while (i--) {
		if (*av) free(*av); 
		av++; 
	}
	free(s->av); 
	free(s);
}

static char *
xstrdup(char *s) {
	char *r = xmalloc(strlen(s)+1); 
	return strcpy(r, s); 
}

static void
swallow(SW *s, FILE *f) 
{
	int len; 
	while ((len = get_delim(f, &bline, &bline_size, 0)) != -1) {
		if (s->len >= s->alloc) {
			char **av = xrealloc(s->av, s->alloc*2*sizeof(*av)); 
			s->alloc *= 2; 
			s->av = av; 
		}
		s->av[s->len++] = xstrdup(bline); 
	}
}

/* process file F, named FN */
static void
do_file(FILE *f, char *fn) {
	srand(time(0));
	if (!sw) 
		sw = sw_open(); 
	swallow(sw, f); 
}

static void
out_rand(SW *s, FILE *outf)
{
	while (s->len) {
		int n = rand()%s->len;
		fputs(s->av[n], outf); 
		s->len--;
		s->av[n] = s->av[s->len]; 
	}
}

/* run just before program ends */
static void
do_exit(void) {
	if (bline) 
		free(bline); 

	if (sw != NULL) {
		out_rand(sw, stdout); 
		sw_close(sw); 
	}
}
