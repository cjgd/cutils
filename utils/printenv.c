/* printenv.c -- print environ */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 971015/990314 */

#include <stdio.h>
#include <string.h>

extern char **environ;

void
pr_all(void) {
	char **env = environ; 
	while (*env) 
		printf("%s\n", *env++); 
}

void
pr_name(char *name) {
	char **env = environ; 
	while (*env) {
		char *pos = strchr(*env, '='); 
		int len;
		if (!pos)
			goto next; 

		len = pos - *env; 
		if (strncmp(name, *env, len) == 0 && name[len] == '\0')
			printf("%s\n", pos+1); 
	next: 
		env++; 
	}
}

int
main(int argc, char *argv[])
{
	int i; 
	if (argc == 1) 
		pr_all(); 
	else for (i=1; i<argc; i++) 
		pr_name(argv[i]); 
	return 0; 
}
