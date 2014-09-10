/* which.c -- which program will be executed */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990705 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "die_warn.c"

#ifndef R_OK
#define R_OK    4               /* Test for read permission.  */
#define W_OK    2               /* Test for write permission.  */
#define X_OK    1               /* Test for execute permission.  */
#define F_OK    0               /* Test for existence.  */
#endif

#ifndef HAVE_ACCESS
int access(const char *command, int acc) 
{
	struct stat sb; 
	int mode; 

	if (stat(command, &sb) != 0)
		return -1; 
	if (acc == F_OK)
		return 0; 	/* ok, file exists */

	mode = sb.st_mode; 
	if (!S_ISREG(mode))
		return -1;

#define match(m) (((m) & acc) == acc)
	if (sb.st_uid == getuid() && match((mode&0700)>>6))
		return 0; 
	if (sb.st_gid == getgid() && match((mode&0070)>>3))
		return 0; 
	if (match(mode&07))
		return 0;
#undef match
	return -1; 
}
#endif /* HAVE_ACCESS */

static char *get_path(void)
{
	char *p, *path, *pa, *t, *s; 

	p = getenv("PATH"); 
	if (!p)
		die("can't find PATH on environ"); 

	path = malloc(strlen(p)+2); 
	if (!path)
		die("malloc: $!"); 
	pa = path; 
	
	t = p; 
	while ((s = strchr(t, ':')) != NULL) {
		if (s-t>0) {
			while (t<s)
				*pa++ = *t++; 
			*pa++ = '\0'; 
		}
		t = s+1; 
	}
	while (*t)
		*pa++ = *t++; 
	*pa++ = '\0'; 
	*pa   = '\0'; 
	return path; 
}

int main(int argc, char *argv[])
{
	int found, i, max, tot; 
	char *path, *p, *buf; 

	set_progname(argv[0]);
	if (argc < 2)
		die("usage: $0 command ... "); 

	path = get_path();

	tot=0; 
	max=0; 
	for (p = path; *p; p += strlen(p)+1) {
		int n = strlen(p); 
		if (n>max)
			max=n; 
	}
	tot += max; 
	max = 0; 
	for (i=1; i<argc; i++) {
		int n = strlen(argv[i]); 
		if (n>max)
			max=n; 
	}
	tot += max; 
	buf = malloc(tot+2); 

	if (!buf)
		die("alloc: $!"); 
		
	found = 1; 
	for (p = path; *p; p += strlen(p)+1) {
		char *pp; 
		strcpy(buf, p); 
		pp = buf+strlen(buf); 
		*pp++ = '/'; 

		for (i=1; i<argc; i++) {
			strcpy(pp, argv[i]); 
			if (access(buf, X_OK)==0) {
				puts(buf); 
				found=0; 
			}
		}
	}
	free(path); 
	free(buf); 
	return found; 
}
