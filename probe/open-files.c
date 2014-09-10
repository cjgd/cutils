/* max-fds.c --
 *	find max number of open files, and those that keep open
 *	atfer a program exists
 */

 /* $Id$ */
 /* Carlos Duarte <cgd@teleweb.pt>, 990328 */

#define HAVE_GETDTABLESIZE

#include <stdio.h>
#include <stdlib.h>

#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>

#if defined(RLIMIT_OFILE) && !defined(RLIMIT_NOFILE)
#define RLIMIT_NOFILE RLIMIT_OFILE
#endif

static int maxfds(void) {
#if !defined(done) && defined(HAVE_GETDTABLESIZE)
	return getdtablesize(); 
#define done
#endif

#if !defined(done) && defined(RLIMIT_NOFILE)
	struct rlimit rr = {-1, -1}; 
	getrlimit(RLIMIT_NOFILE, &rr); 
	return rr.rlim_cur; 
#define done
#endif
	return -1; 
}

int
main()
{
	int md, d; 

	md = maxfds(); 
	printf("max open files: %d\n", md); 
	for (d=0; d<md; d++) {
		if (fcntl(d, F_GETFD, 0) != -1)
			printf("fd %d will be kept open on program exit\n", d);
	}
	exit(0); 
	return(0); 
}
