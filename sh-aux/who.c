/* who.c -- display who is logged on */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990210/990313 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-q] [file]\n", argv[0]), exit(2)

#define UTMP "/etc/utmp"

struct options {
	int qf; 
	char *wfile; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.wfile = UTMP; 
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
		case 'q':
			opts.qf = 1; 
			break; 
		case '?': 
		default: 
			USAGE();
			break;
		}
	}
	return do_args(i, argc, argv); 
}

char *month[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
}; 

char *
cvt_time(time_t t) {
	struct tm *tm; 
	static char buf[99]; 

	tm = gmtime(&t); 
	sprintf(buf, "%s %2d %02d:%02d", 
		month[tm->tm_mon], 
		tm->tm_mday,
		tm->tm_hour, 
		tm->tm_min); 
	return buf; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	int fd; 
	int count=0; 
	int nargs = argc-ix; 
	if (nargs == 1)
		opts.wfile = argv[ix]; 
	else if (nargs != 0)
		USAGE(); 

	if ((fd = open(opts.wfile, O_RDONLY)) == -1) {
		perror(opts.wfile); 
		exit(1); 
	}
	for (;;) {
		struct utmp u; 
		int n = read(fd, &u, sizeof(u)); 

		if (n<=0)
			break; 
		if (opts.qf) {
			printf("%s ", u.ut_user); 
			count++; 
		} else 
			printf("%-9s%-9s%s\n", u.ut_user, u.ut_line, 
				cvt_time(u.ut_time)); 
	}
	close(fd); 
	if (opts.qf) {
		printf("\n# users=%d\n", count); 
	}
	return 0; 
}
