/* endian.c -- find endianess of machine */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990323 */

#include <stdio.h>

int
main() {
	int ival;
#define sz sizeof(ival)
	char s[sz]; 
	char t[sz]; 

	int i, lit, big; 

	for (i=0; i<sz; i++)
		s[i] = i; 

	ival = *(int *)s; 
	big = lit = 0; 
	for (i=0; i<sz; i++) {
		char c = ival&0xff; 
		ival >>= 8; 
		if (s[i] == c)
			lit++; 
		if (s[sz-i-1] == c)
			big++; 
		t[i] = c; 
	}
	if (lit == sz && big == 0)
		printf("little endian\n"); 
	else if (big == sz && lit == 0)
		printf("big endian\n"); 
	else
		printf("unknown\n"); 
	printf("ENDIANESS ORDER: "); 
	for (i=0; i<sz; i++) 
		printf("%c", '1'+t[i]); 
	printf("\n"); 
	return 0; 
}
