/* zap.c -- kill processes interactively ... */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

char *ps = "ps ax"; 

int main(int argc, char *argv[]) 
{
	FILE *p; 
	int pid, lastpid; 
	char buf[BUFSIZ];

	if (argc != 2) {
		fprintf(stderr, "usage: %s name\n", argv[0]); 
		exit(2);
	}
	if ((p = popen(ps, "r")) == NULL) {
		fprintf(stderr, "can't run %s\n", ps); 
		exit(1); 
	}
	pid = lastpid = -1; 
	while (fgets(buf, sizeof buf, p)) {
		char *pp; 
		int yes, c; 

		if (!strstr(buf, argv[1]))
			continue; 
		pp = &buf[*buf ? strlen(buf)-1 : 0];
		if (*pp == '\n')
			*pp = '\0'; 
		printf("%s? ", buf); 
		yes= -1; 
		while ((c = getchar()) != EOF && c != '\n')
			if (yes == -1)
				yes = (c == 'y' || c == 'Y'); 
		if (yes==1) {
			sscanf(buf, "%d", &pid); 

			kill(pid, SIGTERM); 
			if (lastpid > 0)
				kill(lastpid, SIGKILL); 
			lastpid = pid; 
		}
	}
	if (lastpid > 0)
		kill(lastpid, SIGKILL);
	pclose(p); 
	return 0; 
}
