/* beep.c -- produce a beep, hw level */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 961125 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <asm/io.h>

#if defined(__GLIBC__) && __GLIBC__ >= 2
#include <sys/io.h> /* for glibc */
#endif

const unsigned int beep_div = 1300; 
const unsigned int beep_len = 3; 

static void
beep()
{
	unsigned char status; 

	outb(0xb6, 0x43); 
 	outb(beep_div&0xff, 0x42); 	/* low byte if divisor */
	outb(beep_div>>8, 0x42); 	/* high byte of divisor */

	status = inb(0x61); 

	outb(status|3, 0x61); 

	usleep(beep_len*1000000/18); 

	outb(status&0xfc, 0x61); 
}

int
main()
{
	if(ioperm(0x42, 1, 1) == -1) {
		perror("ioperm"); 
		exit(1); 
	}
	ioperm(0x43, 1, 1); 
	ioperm(0x61, 1, 1); 
	
	beep(); 

	return (0); 
}

