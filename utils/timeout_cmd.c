/* timeout_cmd.c -- run a program for no more than a given time */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990703 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

void nothing() {} 

int main(int argc, char *argv[]) 
{
	int secs, pid; 

	if (argc < 3) {
		fprintf(stderr, "usage: %s N cmd args...\n", argv[0]); 
		exit(2);
	}
	secs = atoi(argv[1]); 
	if ((pid = fork()) == -1) {
		perror("can't fork");
		exit(1);
	}
	if (pid) 
		signal(SIGCHLD, nothing); 

	if (pid == 0) {
		execvp(argv[2], &argv[2]); 
		fprintf(stderr, "can't execute %s: %s\n", 
				argv[2], strerror(errno)); 
		exit(1);
	}
	if (sleep(secs)) {
		kill(pid, SIGTERM); 
		if (kill(pid, 0))
			kill(pid, SIGKILL); 
	}
	return 0; 
}
