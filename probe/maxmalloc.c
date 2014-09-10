/* maxmalloc.c -- find max mallocable size */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 960610/990323 */

#include <stdio.h>
#include <stdlib.h>

static void *
v_malloc(size_t n) {
	/* 
	n *= 10241024; 
	*/
	printf("Trying to allocate %8u bytes... ", n); 
	return malloc(n); 
}

static void
yep(void) {
	printf("yep.\n"); 
}

static void
nope(void) {
	printf("nope!\n"); 
}

int 
main(int argc, char *argv[]) 
{
	size_t start, diff;
	void *mem;

	start = 1;
	while ((mem = v_malloc(start)) != NULL) {
		free(mem); 
		start *= 2; 
		yep(); 
	}
	nope(); 

	start /= 2; 
fix: 
	diff = (start+1)/2;  /* round up */
	for (;; diff = (diff+1)/2) {
		if ((mem = v_malloc(start+diff)) != NULL) {
			free(mem); 
			start += diff; 
			yep(); 
			/* some systems permit more core, that phys core! fix */
			if (diff == 1)
				goto fix; 
			continue; 
		}
		nope(); 
		if (diff == 1)
			break; 
	}
	printf("MAX mem available: %u (=%uKB=%uMB)\n", start, start/1024,
	       start/1024/1024);
	return(0);
}
