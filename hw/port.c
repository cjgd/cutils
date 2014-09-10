/* port.c -- perform port in/out operations */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 961227/990321 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s in  addr\n\
       %s out addr value\n\
\n\
addr may be a range: a1-a2, like 0x00-0xff\n\
a1, a2 may be: decimal (1**->9**), octal (0**), or hex (0x**)\n\
value may be: hex (0x**), oct (0ooo), dec (ddd), or bin (bbbbbbbb)\n\
(bin requires 8 digits 0 or 1)\n\
", argv[0], argv[0]), exit(2)

/*
struct options {
} opts; 
*/

int
main(int argc, char *argv[]) 
{
	int i; 

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
#include <unistd.h>
#include <asm/io.h>

#if defined(__GLIBC__) && __GLIBC__ >= 2
#include <sys/io.h> /* for glibc */
#endif

int 
stricmp(char *s1, char *s2) {
	while (*s1 && *s2) {
		if (tolower(*s1) != tolower(*s2))
			return 1; 
		s1++; s2++; 
	}
	if (*s1 || *s2)
		return 1; 
	return 0; 
}

static char *
tobin(unsigned int v)
{
	static char buf[16];
	char *ptr;
	unsigned int mask;

	mask = 0x80;
	ptr = buf;

	do {
		*ptr++ = '0' + !!(v & mask);
	} while ((mask >>= 1) != 0);

	return buf;
}

void
say(char *what, int port, int value) {
	printf("0x%04x: 0x%02x [%s ** %d]\n", 
		port, 
		value, 
		tobin(value), 
		value); 
}

int 
bin(char *s) {
	int b = 0; 
	while (*s) {
		b <<= 1; 
		if (*s++ == '1')
			b++; 
	}
	return b;
}

int 
parse_value(char *s) {
	char *errc; 
	int r; 

	if (strlen(s) == 8 && *(s+strspn(s, "01")) == 0)
		return bin(s); 
	r = strtoul(s, &errc, 0);
	if (*errc) {
		fprintf(stderr, "%s: not a valid representation\n", s); 
		exit(2); 
	}
	return r; 
}

int *
parse_addr(char *s) {
	static int a[256]; 
	int pos, r1, r2; 
	char *t; 

	t = strchr(s, '-'); 
	if (!t) {
		a[0] = parse_value(s); 
		a[1] = -1; 
		return a; 
	}
	*t = 0; 
	r1 = parse_value(s); 
	r2 = parse_value(t+1); 
	pos = 0; 
	while (r1<=r2 && pos<sizeof(a)/sizeof(*a)-1)
		a[pos++] = r1++; 
	a[pos] = -1; 
	return a; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	char *value, *addr; 
	int do_in; 
	int *aa, v = 0; 
	int i; 

	do_in = 0; 
	addr = value = 0; 

	if (ix == argc) USAGE(); 
	if (strcmp(argv[ix], "in") == 0)
		do_in = 1; 
	else if (strcmp(argv[ix], "out") == 0)
		do_in = 0; 
	else 
		USAGE(); 
	++ix; 
	if (ix == argc) USAGE(); 
	addr = argv[ix]; 

	++ix; 
	if (do_in && ix != argc) USAGE();	/* force: in addr */
	if (!do_in) {
		if (argc-ix != 1)
			USAGE(); 		/* force: out addr val */
		value = argv[ix]; 
	}

	if (!do_in) 
		v = parse_value(value); 
	aa = parse_addr(addr); 
	for (i=0; aa[i] != -1; i++) {
		/* check for permission to access (any) port */
		if(ioperm(aa[i], 1, 1) == -1) {
			perror("ioperm"); 
			exit(1); 
		}
		if (do_in) {
			v = inb(aa[i]); 
			say(" read: ", aa[i], v); 
		} else {
			outb(v, aa[i]); 
			say("wrote: ", aa[i], v); 
		}
	}
	return 0; 
}
