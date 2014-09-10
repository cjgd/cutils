/* 970111 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int
main(int argc, char *argv[])
{
	char buf[BUFSIZ]; 
	int nc, nw, nl; 
	char *s, *end; 
	int n; 
	FILE *f; 

	f = stdin; 
	if(argc==2) 
		f = fopen(argv[1], "r"); 

	if(!f) {
		perror("fopen"); exit(1); 
	}

#define RELOAD() 				\
	if((n=fread(buf, 1, BUFSIZ, f))>0) {\
		nc += n;			\
		s = buf; 			\
		end = s+n;			\
	} else 					\
		goto err

	nl = nc = nw = n = 0; 
	s = end = 0; 
	for(;;) {
		/* skip white */
		for(;;) {
			while(s<end) {
				nl += *s == '\n'; 
				if(!isspace(*s++)) goto a; 
			}
			RELOAD();
		}
	a:	nw++; 
		/* skip non white */
		for(;;) {
			while(s<end) {
				if(isspace(*s)) goto b; 
				s++;
			}
			RELOAD();
		}
	b: ; 
	}
err:	if(n == -1) {perror("fread"); exit(1); }
	printf("%7d %7d %7d\n", nl, nw, nc);
	return(0);
}
