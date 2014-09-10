/* dumdo -- execute commands as a dum user */
/* Carlos Duarte, 970619 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

char *prog;

void
usage(void) {
	char *s = prog; 
	while (*s && *s != '/')
		s++; 
	if (*s == '/')
		s++; 
	else
		s = prog; 
	printf("usage: %s user prog args...\n", s);
	exit(2); 
}

int
main(int argc, char *argv[])
{
	struct passwd *p; 

	prog = argv[0];
	if (argc < 3) 
		usage(); 

	p = getpwnam(argv[1]);
	if (!p) {
		fprintf(stderr, "%s: no such user\n", argv[1]); 
		exit(1); 
	}
	if (setuid(p->pw_uid) == -1) {
		perror("setuid"); exit(1); 
	}

	setgid(p->pw_gid); 
	argv += 2; 
	execvp(argv[0], argv); 
	perror("execvp"); 
	return 1; 
}
