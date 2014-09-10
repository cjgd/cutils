/* 970111 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int 
main()
{
	char buf[BUFSIZ];
	FILE *f = stdin; 
	int nw = 0 ; 
	int nc = 0 ; 
	int nl = 0 ; 
	

	for(;;) {
		int n; char *s, *end; 

#define RELOAD() 			\
	n=fread(buf, 1, BUFSIZ, f); 	\
	if(n <= 0) 			\
		goto err; 		\
	else {				\
		nc += n; 		\
		s = buf; 		\
		end = s+n; 		\
	}

	again1:
		RELOAD(); 

	check_new:
		while(s < end) {
			nl += *s == '\n'; 

			if(isspace(*s)) 
				s++; 
			else 
				goto one_more;
		}

		goto again1; 


	one_more:
		nw++; 
		s++; 
	again2: 
		while(s < end)
			if(isspace(*s))
				goto check_new; 
			else
				s++; 

		RELOAD(); goto again2; 


	err:
		if(n == 0) {
			break;
		}
		if(n == -1) {
			perror("stdin"); exit(1); 
		}
	}

	printf( "%7d %7d %7d\n", nl, nw, nc );
	exit(0);
	return 0;
}
