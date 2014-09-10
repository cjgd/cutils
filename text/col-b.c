/* col-b.c -- perform a `col -b' like, operation */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990405 */

#include <stdio.h>

#define CTRL(a) ((a)&037)
int
main()
{
	int c1, c2, c3; 

	c1 = getchar(); 
	c2 = getchar(); 
	while ((c3 = getchar()) != EOF) {
		if (c2 != CTRL('H')) {
			putchar(c1); 
			c1 = c2; 
			c2 = c3; 
			continue; 
		}
		if (c1 == '_')
			putchar(c3); 
		else
			putchar(c1); 
		c1 = getchar(); 
		c2 = getchar(); 
	}
	if (c1 != EOF) putchar(c1); 
	if (c2 != EOF) putchar(c2); 
	return 0; 
}
