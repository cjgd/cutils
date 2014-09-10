/* rd_passwd.c -- read passwd information for each named user */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 980628/990315 */

/*
 * usage: ./a.out user1 user2 ...
 * 
 * lines are outputs in /etc/passwd order, and not in cmd line user order.
 * (i.e. if user2 appears first on /etc/passwd, it will be output first)
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
output(FILE *f, char *line)
{
	char *names[] = { 
		"login",
		"passwd",
		"uid",
		"gid",
		"name",
		"dir",
		"shell" 
	}; 
	char **n, *s, *t; 

	n = names; 
	s = line; 
	for (;;) {
		t = strchr(s, ':'); 
		if (t) *t = 0; 
		fprintf(f, "%10s: %s\n", *n, s); 
		if (!t)
			break; 
		s = t+1; 
		n++; 
	}
	fflush(f); 
}

int
main(int argc, char *argv[])
{
	FILE *f; 
#define line_len 99
	char line[line_len]; 
	int i; 

	if (argc <= 1) {
		printf("usage: %s users...\n", argv[0]); 
		exit(1);
	}
	f = fopen("/etc/passwd", "r"); 
	if (f == 0) {
		perror("/etc/passwd");
		exit(1); 
	}
	while (fgets(line, line_len, f)) {
		for (i=1; i<argc; i++) {
			char user[20];
			int len; 

			if (!argv[i])
				continue; 

			if (strlen(argv[i]) > 16) {
				fprintf(stderr, 
				  "warning: %s name too big\n", argv[i]); 
				continue;
			}
			len = sprintf(user, "%s:", argv[i]); 

			if (strncmp(user, line, len) == 0) {
				output(stdout, line); 
				argv[i] = 0; 
				break; 
			}
		}
	}
	exit(0); 
	return 0; 
}
