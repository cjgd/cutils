/* align.c -- finds out compiler alignments for several types */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 000223 */

#include <stdio.h>

#define S(x) struct { x yy; char c; x y; } x##_var 
#define A(x) ((char *)(&x##_var.y) - (char *)(&x##_var.c))
#define P(x) printf("%-6s sizeof=%2d: alignment=%d\n", \
                    #x ":", sizeof(x##_var.y), A(x))

S(char); 
S(short); 
S(int); 
S(long); 
S(float); 
S(double); 
typedef char *charp; 
S(charp); 
typedef void *voidp; 
S(voidp); 

int main()
{
	P(char); 
	P(short); 
	P(int); 
	P(long); 
	P(float); 
	P(double); 
	P(charp); 
	P(voidp); 

	return 0;
}
