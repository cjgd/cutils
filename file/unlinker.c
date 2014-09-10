/* unlinker.c -- unlink either a file or a directory */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 971113/990317 */

#include <stdio.h>
#include <stdarg.h>

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

void
warn(char *fmt, ...) {
	va_list av; 
	char b[999]; 

	va_start(av, fmt); 
	vsnprintf(b, 999, fmt, av); 
	va_end(av), 

	perror(b); 
}

int
main(int argc, char *argv[])
{
	int i; 
	for (i=1; i<argc; i++) {
		struct stat buf; 
		DIR *d; 
		char *s = argv[i]; 

		if (stat(s, &buf) == -1) {
			fprintf(stderr, "error stating %s, skipping...\n", s); 
			continue; 
		}
		if (!S_ISDIR(buf.st_mode)) {
			if (unlink(argv[i]) == -1)
				warn("could not remove %s", s); 
			continue; 
		}
		/* dir */
		if ((d = opendir(s)) == NULL) {
			warn("can not open directory %s", s); 
			continue; 
		}
		readdir(d); readdir(d);  /* "." and ".." */
		/* this must be null */
		if (readdir(d) != NULL) {
			fprintf(stderr, "%s it not empty\n", s); 
			continue; 
		}
		closedir(d); 
		rmdir(s); 
	}
	return 0; 	
}
