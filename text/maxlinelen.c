/* maxlinelen.c -- find maximum line length of given input files */
/* Carlos Duarte, 010927 */

#include <stdio.h> 

int main(int ac, char *av[]) 
{
	int i; 
	for (i=1; i<ac; i++) {
		char *file; 
		FILE *f;
		int max, len, c; 
		file = av[i]; 
		if ((f = fopen(file, "r")) == NULL) {
			perror(file); 
			continue;
		}
		max = 0; 
		len = 0; 
		while ((c = getc(f)) != EOF) {
			if (c == '\n') {
				if (len>max)
					max = len; 
				len = 0; 
			} else 
				len++;
		}
		printf("%d\n", max); 
		fclose(f); 
	}
	return 0; 
}
