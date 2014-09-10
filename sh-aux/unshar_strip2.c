/* unshar_strip2.c -- 
 * 	strips headers from sh archives (shar)
 * 	works as a filter (read from stdin, write to stdout)
 * 
 */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970222 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int
main()
{
	int c; 

	while ((c = getc(stdin)) != EOF) {
		if (isspace(c))
			continue;
		if (c != '#')
			continue; 
		if ((c = getc(stdin)) == EOF)
			break; 
		if (c == '!')
			break; 
	}
	if (c == EOF)
		exit(1);	/* did not found archive */
	fputs("#!", stdout); 
	while ((c = getc(stdin)) != EOF)
		putc(c, stdout); 
	exit(0); 
	return 0; 
}
