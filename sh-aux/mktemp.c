/* mktemp.c -- hack to output a temp file name */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970512 */

#include <stdio.h>
#include <unistd.h>

int
main()
{
	char *res; 
	res = tmpnam(0); 
	puts(res ? res : "/tmp/._.imp.!.file"); 
	return 0; 
}
