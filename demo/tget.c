/* tget.c -- get terminal entry from termcap data base */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 960727/990321 */

/* compile with: cc tget.c -ltermcap */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <termcap.h>

static char *prog_name;

static void 
die(char *msg,...)
{
	va_list args;

	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);

	if (msg[*msg ? strlen(msg)-1 : 0] != '\n')
		putchar('\n');
	exit(1);
}

static void 
usage(void)
{
	fprintf(stderr, "\
usage: %s [term]\n\
\n\
print the entry for term, or env var TERM if term not given, \n\
to stdout, in termcap data base\n", prog_name);

	exit(1);
}

static void 
tset(char *t_name)
{
	char *term = t_name; 
	char tbuf[2048];
	int rcode;


	if (term == NULL) 
		term = getenv("TERM");

	if (term == NULL) 
		die("TERM must be setted");

	unsetenv("TERMCAP");
	rcode = tgetent(tbuf, term);

	if (rcode < 0)
		die("can not access to termcap data base");

	if (rcode == 0)
		die("terminal `%s' was not found\n", term);

	puts(tbuf);
}

int
main(int argc, char **argv)
{
	prog_name = argv[0];

	if (argc > 2) {
		usage();
	}
	if (argc == 2) {
		tset(argv[1]);
	} else 
		tset(0); 
	exit(0);
	return(0);
}
