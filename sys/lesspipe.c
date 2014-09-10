/* lesspipe.c */
/* Carlos Duarte, 980923 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int
main(int argc, char *argv[])
{
	int n; 
	char *s; 

	if (argc <= 1)
		exit(1); 
	s = argv[1]; 
	n = strlen(s); 
	if (n>3 && strcmp(s+n-3, ".gz") == 0)
		execlp("gzip", "gzip", "-dc", s, (char *)0); 
#ifdef USE_STDIO
{
	FILE *f; 
	int c; 

	f = fopen(s, "r"); 
	if (f == NULL)
		exit(1); 
	while ((c = getc(f)) != EOF)
		putchar(c); 
}
#else 
{
	int d; 
	const int buf_size = 16384; 
	char *buf = malloc(buf_size); 

	if (!buf) exit(1); 
	d = open(s, 0); 
	if (d == -1) {
		free(buf); 
		exit(1); 
	}
	while ((n = read(d, buf, buf_size)) > 0)
		write(1, buf, n); 
}
#endif

	exit(0); 
	return 0; 
}
