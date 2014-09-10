/* pipe-size.c -- find pipe buffer size */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990325 */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#ifndef chunk_size
#define chunk_size 4	/* 1 to be sure */
#endif

void err(char *s) {
	perror(s); exit(1); 
}

int
main() {
	char s[chunk_size*1024]; 
	int p[2]; 
	int sz = 0; 

	if (pipe(p) == -1) err("pipe"); 
	if (fcntl(p[1], F_SETFL, O_NONBLOCK) == -1) err("fcntl"); 
	if (fcntl(p[0], F_SETFL, O_NONBLOCK) == -1) err("fcntl"); 

	*s = 0; 
	while (write(p[1], s, chunk_size) != -1)
		sz += chunk_size; 

	printf("%d\n", sz); 

	while (read(p[0], s, sizeof s) != -1)
		; 

	close(p[1]); 
	close(p[0]); 

	return 0; 
}

