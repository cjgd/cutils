/* count-lines.c -- count lines */

/* $Id$ */
/* Carlos Duarte, 981003 */

#include <stdio.h>

int
wl(char *fn)
{
	FILE *f = stdin;
	int lines, c; 
	char *name = fn; 

	if (!name)
		name = "(standard input)"; 

	if (fn && (f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		return 0; 
	}
	lines=0; 
	while ((c = getc(f)) != EOF) {
		lines += c == '\n'; 
	}
	fclose(f); 
	printf("%7d %s\n", lines, name); 
	return lines; 
}

int
main(int ac, char *av[])
{
	int i; 
	int tot; 
	tot = 0; 
	i=1;
	if (i>=ac) {
		wl(0);
	} else for (; i<ac; i++) {
		tot += wl(av[i]); 
	}
	if (ac>2)
		printf("%7d total\n", tot); 
	return 0; 
}
