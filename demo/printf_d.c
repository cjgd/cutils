/* printf_d.c -- emulates printf("%6d") */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 981005 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* emulate, printf("%6d", d), knowing first d=1, then =2, =3, ... */

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)
char buf[10] = { '0','0','0','0','0','0','0','0','0','0' }; 
int buf_i = 9; 

void
pf(void)
{
	char *s, *t; 

	s = t = &buf[10-1]; 

	if (*s == '9') {
		int bi = 9; 

		do {
			*s = '0'; 
			s--; 
			bi--; 
		} while (*s == '9'); 
		buf_i = min(buf_i, bi);
	}
	*s = *s + 1; 

	s = &buf[buf_i]; 
	while (s <= t) {
		putchar(*s); 
		s++; 
	}
	putchar('\n');
}

int current; 
void
pf_a(void)
{
	printf("%d\n", ++current); 
}

int
main(int argc, char *argv[])
{
	int limit = 9999; 
	int i=1; 
	if (i<argc && isdigit(argv[i][0])) {
		limit = atoi(argv[i]); 
		i++; 
	}
	if (limit<=0 || i>=argc) {
		printf("usage: %s [nr_tries] pf | emul\n", argv[0]); 
		exit(1);
	}
	if (strncmp(argv[i], "pf", 2) == 0)
		while (limit--) 
			pf_a(); 
	else
		while (limit--)
			pf(); 
	return 0; 
}
