/* recho.c -- 
 *	randomized version of echo, echoes its arguments in random order 
 */

/* $Id$ */
/* Carlos Duarte, 970901 */
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int
main(int argc, char *argv[])
{
	int i; 

	srand(time(0)); 
	for (i=1; i < argc; i++) {
		int j = (rand()/89891)%(argc-1)+1; 	/* >=1  && < argc */
		printf("%s%s", argv[j], i==argc-1 ? "\n" : " "); 
		argv[j] = argv[i]; 
	}
	exit(0); 
}
