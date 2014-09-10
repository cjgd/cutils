/* shar_strip.c -- 
	strips a sh archive (sh), in order to remove headers
 */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970222 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SKIPWS(s) while(isspace(*s)) s++; 

#define SKIP 0
#define PRINT 1

#define LINE_SZ 1024

int
main(int ac, char *av[])
{
	char *av2[2]; 
	FILE *f; 
	char *line, *s; 
	int state; 

	if(ac == 1) {
		av2[0] = "-"; 
		av2[1] = 0; 
		av = av2;
		ac = 2; 
	}

	line = malloc(LINE_SZ); if(!line) exit(1); 

	while(ac>0) {
		if(strcmp(av[0], "-") == 0)
			f = stdin; 
		else {
			f = fopen(av[0], "r"); 
			if(!f) goto next; 
		}
		state = SKIP; 
		for(;;) {
			if(fgets(line, LINE_SZ, f) == NULL) {
				if(f != stdin) 
					fclose(f); 
				goto next; 
			}
			switch(state) {
			case SKIP: 
				s = line; 
				SKIPWS(s); 
				if(!*s) break; 
				
				if(*s != '#') break; 
				if(*++s != '!') break; 
				state = PRINT; 
			case PRINT: 
				fputs(line, stdout); 
				break; 
			}
		}
	next: 
		ac--; 
		av++; 
	}
	return 0; 
}
