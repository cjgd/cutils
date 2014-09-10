/* ascii_tbl.c -- print all ascii (and more) chars */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 960727 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define NPER_ROW	4

int
main(int argc, char **argv)
{
	int i,j,k;
	int init = 32; 
	int NPER_COL; 

	if(argc == 2)
		init = atoi(argv[1]);

	init -= init%NPER_ROW; 

	NPER_COL=((256-init)/NPER_ROW); 

	for(i=0; i < NPER_COL; i++) {
		for(j=0; j < NPER_ROW; j++) {
			k = i + init + j*NPER_COL; 
			printf("%3d %0#4o %0#4x: %c%s", 
			       k, k, k, 
			       /* isprint(k) ? k : '.' */ k,
			       j == NPER_ROW-1 ? "\n" : "    "); 
		}
	}
	return 0; 
}
