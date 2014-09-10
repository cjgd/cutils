/* wc2.c -- */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 960630 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int open(), close(), read();

static int nl, nc, nw; 
static int tl, tc, tw; 

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

char buffer[BUFSIZ*1024]; 

static int
wc(int f) 
{
	char *s; 
	char *ss; 
	int n;

	nl = nc = nw = 0;

floop:	switch (n = read(f, buffer, sizeof(buffer)/sizeof(char))) {
	case -1: 
		perror("read");
		return -1; 
	case 0: 
		tc += nc; 
		tw += nw; 
		tl += nl;
		return 0; 
	default: 
		break;
	}

	nc += n; 
	s = buffer; 
	ss = s+n;
s1loop:	switch (*s++) {
	case '\n':
		nl++; /* FALLTHROUGH */
	case '\r':
	case '\f':
	case '\t':
	case '\v':
	case ' ':
		if(s < ss) goto s1loop;
		goto floop;

	default:
		break;
	}
	nw++;
s2loop:	switch (*s++) {
	case '\n':
		nl++; /* FALLTHROUGH */
	case '\r':
	case '\f':
	case '\t':
	case '\v':
	case ' ':
		if(s < ss) goto s1loop;
		goto floop;

	default:
		if(s < ss) goto s2loop;
		goto floop;
	}
	return 0; 	/* NOT USED */
}

#define info(file) printf("%7d %7d %7d %s\n", nl, nw, nc, file)
#define info_tail() printf("%7d %7d %7d %s\n", tl, tw, tc, "total")

int
main(int argc, char **argv)
{
	int f; 
	int flag; 

	flag = (argc > 2);

	if (argc <= 1) {
		wc(0);
		info("");
	} else {
		do {
			if (**++argv == '-' && argv[0][1] == '\0')
				f = 0; 
			else if ((f = open(*argv, 0)) == -1) {
				perror(*argv);
				continue;
			}
			wc(f);
			info(*argv);
			if (f != 0)
				close(f);
		} while(--argc > 1); 
	}
	if (flag)
		info_tail();

	exit(0);
	return 0; /* NOT USED */
}
