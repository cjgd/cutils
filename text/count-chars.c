/* count-chars.c -- count chars! */

/* $Id$ */
/* Carlos Duarte, 981006 */

#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>

int
wc(char *fn)
{
	struct stat sb; 
	int size;
	char *name; 
	name = fn;
	if (!name)
		name = "(standard input)"; 

	size = 0; 
	if (fn && stat(fn, &sb) != -1)
		size = sb.st_size; 
	else {
		FILE *f = stdin; 
		if (fn && (f = fopen(fn, "r")) == NULL) {
			perror(fn);
			return 0; 
		}
		while (getc(f) != EOF)
			size++; 
		if (f != stdin)
			fclose(f);
	}
	printf("%7d %s\n", size, name); 
	return size; 
}

int
main(int ac, char *av[])
{
	int i; 
	int tot; 
	tot = 0; 
	i=1; 
	if (i>=ac) {
		wc(0); 
	} else for (; i<ac; i++) {
		tot += wc(av[i]); 
	}
	if (ac>2)
		printf("%7d total\n", tot); 
	return 0; 
}
