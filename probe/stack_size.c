/* stack_size.c -- finds out stack size, and where does it grows to */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 000312 */

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

jmp_buf env; 
unsigned long stack_size=0; 
char *a1=0,*a2=0; 
int i; 

#define ff(n) void f_##n(void);
#define f(n) \
void f_##n (void) { \
	char a[n]; \
	for (i=0; i<n; i += n/77?n/77:n/7?n/7:1) \
		a[i]++; \
	stack_size += n; \
	if (a1 && !a2) a2 = a; \
	if (!a1) a1 = a; \
	if (a1 && a2 && a1!=a2) { \
		printf("%s\n", a2<a1 ? "stack grows down..." : \
				       "stack grows up..."); \
		a1=a2; \
	} \
	printf("used stack: %lu (%d incs)\n", stack_size, n) ; \
	fflush(stdout); \
	if (setjmp(env)==0) { \
		f_##n(); \
	} else { \
		switch(n) { \
		case 1048576: f_262144(); break;  \
		case 262144: f_32768(); break;  \
		case 32768: f_1024(); break;  \
		case 1024: f_1(); break;  \
		} \
	} \
}

ff(1048576)
ff(262144) 
ff(32768)
ff(1024) 
ff(1)

f(1048576)
f(262144) 
f(32768)
f(1024) 
f(1)

void catcher() { longjmp(env,1); }

int main() { 
	signal(SIGABRT, catcher);
	signal(SIGBUS, catcher);
	signal(SIGSEGV, catcher);
	signal(SIGSTKFLT, catcher); 
	f_1048576(); 
	return 0; 
}
