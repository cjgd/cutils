/* open-fifo.c -- opens a file, blocks until get a sig */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 980510/990314 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <fcntl.h>
#include <unistd.h>

int quit =0; 

void 
intr() { 
	quit++; 
}

int
main(int argc, char *argv[])
{
	char *fn; 
	int d; 

	if (argc != 2) {
		printf("usage: %s file\n", argv[0]); 
		exit(1); 
	}

	signal(SIGINT, intr); 
	signal(SIGTERM, intr); 
	fn = argv[1]; 
	if ((d = open(fn, 1)) == -1) {
		perror(fn); 
		exit(1);
	}
	while (!quit) {
		sleep(10);
	}
	close(d); 
	return 0; 
}
