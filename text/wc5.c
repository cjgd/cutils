/* 970111 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define BUFSZ (1<<14)

int
main(int argc, char *argv[])
{
	int nc, nw, nl; 
	char *s, *end; 
	int n; 
	int f; 
	char buf[BUFSZ]; 

	f = 0; 
	if(argc==2) 
		f = open(argv[1], 0); 

	if(f==-1) {
		perror("open"); exit(1); 
	}

#define RELOAD() 				\
	if((n=read(f, buf, BUFSZ))>0) {\
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
err:	if(n == -1) {perror("read"); exit(1); }
	printf("%7d %7d %7d\n", nl, nw, nc);
	return(0);
}
