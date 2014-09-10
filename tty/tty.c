/* tty.c -- print used terminal */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990402 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#ifndef HAVE_ISATTY

#define isatty _isatty 
int _isatty(int fd) {
	struct stat sb; 

	if (fstat(fd, &sb) != 0)
		return 0; 

	return S_ISCHR(sb.st_mode); 
}

#endif


#ifndef HAVE_TTYNAME

#define ttyname _ttyname
char *_ttyname(int fd) {
	struct stat sb; 
	int inum; 
	struct dirent *de; 
	DIR *dir; 
	static char tty[99] = "/dev/";

	if (fstat(fd, &sb) != 0)
		return 0; 

	if (!S_ISCHR(sb.st_mode))
		return 0; 

	inum = sb.st_ino; 
	if ((dir = opendir("/dev")) == NULL)
		return 0; 
	
	while ((de = readdir(dir)) != NULL) {
		char *name; 

		name = de->d_name; 
		if (name[0] == '.' && (name[1] == '\0' || 
		    (name[1] == '.' && name[2] == '\0'))) 
			continue; 

		strcpy(&tty[5], name); 

		if (lstat(tty, &sb) == -1)
			continue; 

		if (sb.st_ino == inum) 
			break; 
	}
	closedir(dir); 
	return tty; 
}

#endif 

void
usage() {
	printf("\
Usage: tty [-s] \n\
\n\
exit code: 0, stdin is a tty\n\
           1, stdin is not a tty\n\
	   2, bad args\n\
	   3, sys error \n\
"); 
	exit(2); 
}

int
main(int argc, char *argv[]) 
{
	int quiet = 0; 
	char *name; 

	if (argc > 2)
		usage(); 
	if (argc == 2) {
		if (strcmp(argv[1], "-s") != 0)
			usage(); 
		quiet++; 
	}
	if (!isatty(0)) {
		if (!quiet)
			puts("not a tty"); 
		exit(1); 
	}

	if ((name = ttyname(0)) == NULL)
		exit(3); 

	puts(name); 
	return(0); 
}

