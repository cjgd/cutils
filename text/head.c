/* head.c -- display first lines of file */

/* $Id$ */
/* Carlos Duarte, 960612 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
usage(void)
{
	fprintf(stderr, 
"usage: head [-c chars] [-n lines] [-lines] [-qv] [files...]\n"); 
	exit(1); 
}

void
head(char *fn, int lines) 
{
	int c; 
	FILE *f; 

	if (fn == 0 || strcmp(fn, "-") == 0)
		f = stdin; 
	else if ((f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		return; 
	}

	if (lines < 0) {
		int n = -lines; 
		while (n-- && (c = getc(f)) != EOF)
			putchar(c); 
	} else {
		int n = lines; 
		if (n) do {
			if ((c = getc(f)) == EOF)
				break; 
			putchar(c);
		} while (c != '\n' || --n > 0);
	}
	if (f != stdin)
		fclose(f); 
}

void
print_header(char *fn) 
{
	if (fn == NULL || strcmp(fn, "-") == 0)
		printf("==> standard input <==\n"); 
	else
		printf("==> %s <==\n", fn); 
}

#define V_AUTO	0
#define V_ALL	1
#define V_NEVER	2

int 
main(int ac, char *av[])
{
	int i; 
	char *arg; 
	int headers; 
	int lines; 

	headers = V_AUTO; 
	lines = 10; 

	for (i=1; i<ac; i++) {
		char *s = av[i]; 
		if (*s != '-')
			break; 
		if (s[1] == '-' && s[2] == '\0') {
			++i; 
			break;
		}
		while (*++s) switch (*s) {
		case 'c': 
			if (s[1])
				arg = s+1; 
			else if (++i<ac)
				arg = av[i]; 
			else 
				usage(); 
			lines = -atoi(arg); 
			goto next; 
		case 'n': 
			if (s[1])
				arg = s+1; 
			else if (++i<ac)
				arg = av[i]; 
			else 
				usage(); 
			lines = atoi(arg); 
			goto next; 
		case 'q': 
			headers = V_NEVER; 
			break; 
		case 'v': 
			headers = V_ALL; 
			break;
		case '0': case '1': case '2': case '3': 
		case '4': case '5': case '6': case '7': 
		case '8': case '9': 
			arg = s; 
			lines = atoi(arg); 
			goto next; 
		default: 
			usage(); 
			break; 
		}
	next: ; 
	}
	if (i<ac) {
		for (; i<ac; i++) {
			if (headers == V_ALL)
				print_header(av[i]); 
			head(av[i], lines); 
			if (headers == V_AUTO)
				headers = V_ALL; 
		}
	} else {
		if (headers == V_ALL)
			print_header(0); 
		head(0, lines); 
	}
	return 0; 
}
