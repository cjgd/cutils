/* rd_symlink.c -- read to where symlink points to */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 980418 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	char buf[999]; 
	memset(buf, 0, sizeof(buf)); 
	readlink(argv[1], buf, sizeof(buf)-1); 
	puts(buf); 
	return 0; 
}
