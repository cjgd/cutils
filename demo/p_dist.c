/* p_dist.c  -- demo: text graph of some probability distributions */

/* $Id$ */
/* Carlos Duarte, 980126/990314 */

/* cc p_dist.c -lm */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include <math.h>

int N = 58; 		/* number of lines graph will take (x axis) */
int K = 222222; 	/* number of iters */
int *A; 

/* uniform distributed value, in [0,1) */
double 
u(void)
{
	double t; 

	t = rand(); 
	t /= RAND_MAX+1.0; 
	return t; 
}

int
uniform(void)
{
	return (int)floor(u()*N); 
}

int
normal(void)
{
	double y; 
	int res; 

	do {
		double y1 = -log(1-u()); 
		double y2 = -log(1-u()); 
		y = y1-1.0; 
		y *= y; 
		y /= 2; 
		y = y2 - y; 

	} while (y <= 0); 

	y *= 3; 

	res = (int)floor(y); 
	if (u() > .5)
		res = -res; 

	if (res < -N/2)
		res = -N/2; 
	else if (res >= N/2)
		res = N/2-1; 

	return res+N/2; 
}

int 
expon(void)
{
	int res; 
	double y = -log(1-u()); 
	y *= 9; 
	res = floor(y); 
	if (res >= N)
		res = N-1; 
	return res; 
}

void
p(int n, int c)
{
	while (n--)
		putchar(c); 
	putchar('\n'); 
}

void
t(int (*f)(void))
{
	int max, k; 

	for (k=0; k<N; k++)
		A[k] = 0; 

	for (k=K; k--; ) {
		A[f()]++; 
	}


	max = A[0]; 
	for (k=1; k<N; k++) {
		if (A[k] > max)
			max = A[k]; 
	}

	for (k=0; k<N; k++) {
		p(A[k]*72/max, '|'); 
	}
}

void
dump(int (*f)(void))
{
	for (;;) {
		printf("%d\n", f()); 
	}
}
		
void
usge(void) {
	printf("usage: prog k=NR_ITERS n=NR_LINES\n"); 
	exit(1);
}

void
err(char *s) {
	perror(s); 
	exit(2);
}

int
main(int argc, char *argv[])
{
	int i=1;
	char *s, *cmd; 
	int siz; 

	siz = 1; 
	cmd = malloc(siz); if (!cmd) err("malloc"); 
	*cmd = 0; 
	while (i<argc) {
		siz += strlen(argv[i])+1;
		cmd = realloc(cmd, siz); if (!cmd) err("realloc"); 
		strcat(cmd, argv[i]); 
		strcat(cmd, " "); 
		++i; 
	}
	s = cmd; 
	for (;;) {
		int x,y;
		char c; 
		i = sscanf(s, " %[nNkK] = %d%n ", &c, &x, &y);
		if (i<=0)
			break; 
		s += y; 
		switch(tolower(c)) {
		case 'k' : K = x; break; 
		case 'n' : N = x; break; 
		}
	}
	free(cmd); 
	/*
	printf("n %d, k %d\n", N, K);
	*/

#define z(x) x=abs(x); if (x==0) usge() 
	z(N); z(K); 
	A = malloc(sizeof(*A)*N);  if (!A) err("malloc"); 

	srand(time(0)); 

	/*
	dump(expon);
	*/
	printf("@@ uniform \n");    	t(uniform); 
	printf("@@ normal \n");     	t(normal); 
	printf("@@ exponencial \n");	t(expon); 

	free(A); 
	return 0; 
}
