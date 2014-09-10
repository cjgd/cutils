/* hostname.c -- prints hostname */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 980728 */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>

int
main()
{
	char s[100]; 

	if (gethostname(s, 100) == -1) {
		printf("error gethostname\n");
		exit(1);
	}
	printf("local hostname: %s\n", s); 
	if (gethostbyname(s)==0) {
		printf("error gethostbyname\n"); 
		exit(1);
	}

	return(0); 
}
