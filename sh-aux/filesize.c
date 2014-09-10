/* filesize.c -- print file size */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970720 */

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>

int 
main(int argc, char **argv)
{
	int i; 

	if (argc <= 1) {
		printf("usage: %s files... \n", argv[0]); 
		exit(1);
	}
	for (i=1; i<argc; i++) {
		char *path = argv[i]; 
		struct stat file_stats;

		if (stat(path, &file_stats))
			perror(path); 
		else
			printf("%s: %lu\n", path, file_stats.st_size); 
	}
	return 0; 
}
