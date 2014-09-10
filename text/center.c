/* center.c -- centers input lines */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970527/990314 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
usage: %s [-w width] [files...]\
\n", program_name); 
	exit(2); 
}

struct options {
	int wid; 
} opts; 

/* process cmd line args */
static void
do_args(int argc, char *argv[]) {

	opts.wid = 72; 
	for (;;) switch (getopt(argc, argv, "w:")) {
		case 'w': 	
			opts.wid = atoi(optarg); 
			if (opts.wid <= 0)
				usage();
			break; 
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

static void 
p_spaces(FILE *f, int n) {
	while(n--) 
		putc(' ', f); 
}

/* process file F, named FN */
static void
do_file(FILE *f, char *fn) {

	while (get_delim(f, &bline, &bline_size, -'\n') != -1) {
		char *s = bline; 
		int nspaces; 
		char *lhs, *rhs; 

		while(isspace(*s)) s++; 
		if(*s) {
			lhs = s; 
			for(;;) {
				while(*s && !isspace(*s)) s++; 
				rhs = s; 
				if(!*s) break; 
				while(*s && isspace(*s)) s++; 
			}
			*rhs = '\0'; 

			nspaces = (opts.wid-(rhs-lhs+1))/2; 
			if (nspaces > 0)
				p_spaces(stdout, nspaces); 
			fputs(lhs, stdout); 
		} 
		putc('\n', stdout);
	}
	fflush(stdout); 
}
