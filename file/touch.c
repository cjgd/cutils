/* touch.c -- touch access and modification times of given files */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 971011/990315 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <utime.h>

int
main(int argc, char *argv[])
{
	struct utimbuf buf, *bufp; 
	char *ref_file, *stamp; 
	char *pg = argv[0]; 
	int i; 

	ref_file = stamp = 0; 

#define _x()	++argv; --argc; if (argc <= 0) goto x
	_x(); if (strcmp(*argv, "-r") == 0) {
		_x(); 
		ref_file = *argv; 
		_x(); 
	}

	if (strcmp(*argv, "-t") == 0) {
		_x(); 
		stamp = *argv; 
		_x(); 
	}

	bufp = &buf; 
	if (ref_file) {
		struct stat xx; 
		if (lstat(ref_file, &xx) == -1) goto y; 
		buf.modtime = xx.st_mtime; 
		buf.actime = xx.st_atime; 
	} else if (stamp) {
		/* parse MMDDhhmm[[CC]YY][.ss] */
		struct tm xx; 
		char *s; char *end; 
		char temp[3]; 
		end = stamp + strlen(stamp); 
		s=stamp;if(s>=end)goto o;strncpy(temp,s,2);xx.tm_mon=atoi(s);
		s+=2;if(s>=end)goto o;strncpy(temp,s,2);xx.tm_mday=atoi(s);
		s+=2;if(s>=end)goto o;strncpy(temp,s,2);xx.tm_hour=atoi(s);
		s+=2;if(s>=end)goto o;strncpy(temp,s,2);xx.tm_min=atoi(s);
		s+=7;if(s>=end)goto o;strncpy(temp,s,2);xx.tm_sec=atoi(s);
	o: 
		buf.actime = buf.modtime = mktime(&xx); 
	} else 
		bufp = 0; 

	for (i=0; i<argc; i++) {
		utime(argv[i], bufp); 
	}

	exit(0); 

x:
	printf("usage: %s [-r file] [-t MMDDhhmm[.ss]] files...\n", pg); 
	exit(1); 

y:
	perror("foo"); 
	exit(1); 
}
