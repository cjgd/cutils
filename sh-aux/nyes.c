/* nyes.c -- yes(1) with count */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 980825 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int
isnumber(char *s, int *val)
{
	int n; 
	char *err; 

	n = strtol(s, &err, 0); 
	if (err == s || *err != 0)
		return 0; 

	if (val)
		*val = n; 
	return 1; 
}

char *
argv_to_string(char *av[], int ac)
{
	char *fake_av[] = { "y", 0 }; 
	int fake_ac = 1; 

	char *s, *t; 
	int tot, i; 

	if (ac == 0) {
		av = fake_av; 
		ac = fake_ac; 
	}

	tot=0; 
	for (i=0; i<ac; i++)
		tot += strlen(av[i])+1; /* 1 for an extra space */
	if (tot == 0)
		return 0; 
	s = malloc(tot+1); /* 1 for trailling nul */
	if (s == 0)
		return 0; 
	t=s; 
	for (i=0; i<ac; i++) {
		t+= sprintf(t, "%s ", av[i]); 
	}
	t[-1] = 0; 
	return s; 
}

int
main(int argc, char *argv[])
{
	int n, i; 
	char *s; 

	n = 0; 
	for (i=1; i<argc; i++) {
		if (argv[i][0] != '-')
			break; 
		if (argv[i][1] == '-') {
			i++; 
			break;
		}
		switch (argv[i][1]) {
		case 'n': 
			if (isnumber(&argv[i][2], &n))
				break; 
			if (++i >= argc) {
				printf("-n takes a number option\n"); 
				exit(1);
			}
			if (isnumber(argv[i], &n))
				break; 
			printf("%s -- is not a number\n", argv[i]); 
			exit(1); 
		default: 
			printf("%s -- bad option\n", argv[0]); 
			exit(1); 
		}
	}
	s = argv_to_string(&argv[i], argc-i); 
	if (n) {
		do 
			printf("%s\n", s); 
		while (--n != 0); 
	} else 
		for (;;)
			printf("%s\n", s);

	free(s);
	return 0;
}

