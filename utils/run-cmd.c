/* run-cmd.c -- spawn processes with some options */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990613 */

#define HAVE_GETDTABLESIZE
#define HAVE_SETSID

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int do_args(int first, int ac, char *av[]); 
static int get_user(char *user); 
static int add_env(char **var); 

extern char **environ; 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-u user:group] [-E] [-e var=value] [-p] [-n] [-s] cmd args...\n\
\n\
  -u user:group   run command as user:group (may specify only user: -u user,\n\
		  or only group: -u :group)\n\
  -E              do not pass any parent environ to child\n\
  -e var=value    add VAR=VALUE to child environ (may have several of this)\n\
  -p              print childs pid to stdout\n\
  -n              run child, with null stdin, stdout, stderr\n\
  -s              set child as a session leader\n\
  -F              do not fork\n\
", argv[0]), exit(2)

#define ENV_S_MAX 128
struct options {
	int group; 
	int user; 
	struct {
		char *s_buf[ENV_S_MAX]; 
		char **env; 
		int max;
		int cur;
	} env; 
	int printpid; 
	int null; 
	int setsid; 
	int no_parent_env; 
	int no_fork; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 
	char *arg; 

	opts.user = -1; 
	opts.group = -1; 
	opts.env.env = opts.env.s_buf; 
	opts.env.max = ENV_S_MAX; 
	opts.env.cur = 0; 
	for (i=1; i<argc; i++) {
		char *t, *s = argv[i]; 
		t=0; /* shut up warnings */
		if (s[0] != '-' || s[1] == 0) 
			break; 
		if (s[1] == '-' && s[2] == 0) {
			++i;
			break; 
		}
		while (*++s) switch (*s) {
		case 'u': 
			arg = ARG(); 
			if (!get_user(arg)) 
				USAGE(); 
			break; 
		case 'E':
			opts.no_parent_env++; break; 
		case 'e': 
			arg = ARG(); 
			if (!add_env(&arg)) 
				USAGE();
			break; 
		case 'p': 
			opts.printpid++; break; 
		case 'n': 
			opts.null++; break; 
		case 's': 
			opts.setsid++; break; 
		case 'F': 
			opts.no_fork++; break; 
		case '?': 
		default: 
			USAGE();
			break;
		}
	}
	return do_args(i, argc, argv); 
}

/* USER CODE */

#include <errno.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>

#include <pwd.h>
#include <grp.h>
#include <sys/types.h>

#include <unistd.h>

#if defined(RLIMIT_OFILE) && !defined(RLIMIT_NOFILE)
#define RLIMIT_NOFILE RLIMIT_OFILE
#endif

static int 
maxfds(void)
{
#if !defined(done) && defined(HAVE_GETDTABLESIZE)
	return getdtablesize();
#define done
#endif

#if !defined(done) && defined(RLIMIT_NOFILE)
	struct rlimit rr = {-1, -1};
	getrlimit(RLIMIT_NOFILE, &rr);
	return rr.rlim_cur;
#define done
#endif
	return -1;
#undef done
}

static int 
my_setsid() 
{
#if !defined done && defined HAVE_SETSID
	return setsid(); 
#define done
#endif

#if !defined done && defined HAVE_SETPGRP
	int fd; 
	int r; 

	r = setpgrp();
	if ((fd = open("/dev/tty")) != -1) {
		ioctl(fd, TIOCNOTTY, 0); 
		close(fd); 
	}
	return r; 
#endif
	return -1; 
#undef done
}

static int 
add_env(char **var) {
	if (strchr(*var, '=') == NULL)
		return 0; 
	if (opts.env.cur >= (opts.env.max-1)) { /* one for extra NULL */
		opts.env.max *= 2; 
		if (opts.env.env == opts.env.s_buf) {
			opts.env.env = malloc(sizeof(char*) * opts.env.max); 
			if (opts.env.env)
				memcpy(opts.env.env, opts.env.s_buf, 
						sizeof opts.env.s_buf); 
		} else
			opts.env.env =
			  realloc(opts.env.env, sizeof(char*) * opts.env.max);
		if (opts.env.env == NULL) {
			perror("alloc"); 
			exit(1); 
		}
	}
	opts.env.env[opts.env.cur++] = *var; 
	opts.env.env[opts.env.cur] = 0; 
	return 1; 
}

static int 
get_user(char *user) {
#define sz 99
	char u[sz]; 
	char g[sz]; 
	char *p; 

	memset(u, 0, sizeof u); 
	memset(g, 0, sizeof g); 
	if ((p = strchr(user, ':')) == NULL) {
		strncpy(u, user, sz); 
	} else {
		char *s = user; 
		char *t = u; 
		while (s<p && s-user<sz-1) {
			*t++ = *s++; 
		}
		*t = 0; 
		strncpy(g, p+1, sz); 
	}
	if (u[0]) {
		struct passwd *pwd; 
		if (isdigit(u[0]))
			pwd = getpwuid(atoi(u)); 
		else
			pwd = getpwnam(u);

		if (!pwd) {
			fprintf(stderr, "%s: invalid user\n", u); 
			exit(1); 
		}
		opts.user = pwd->pw_uid; 
	}
	if (g[0]) {
		struct group *pwd; 
		if (isdigit(g[0]))
			pwd = getgrgid(atoi(g)); 
		else
			pwd = getgrnam(g);

		if (!pwd) {
			fprintf(stderr, "%s: invalid group\n", g); 
			exit(1); 
		}
		opts.group = pwd->gr_gid; 
	}
	return 1; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	int i, d, pid; 

	if (ix == argc) {
		USAGE(); 
	}

	if (!opts.no_fork) {
		pid = fork(); 
		if (pid == -1) {
			perror("fork"); 
			exit(1);
		}
		if (pid) {
			if (opts.printpid)
				printf("%d\n", pid); 
			return 0; 
		}
	} else {
		if (opts.printpid)
			fprintf(stderr, "%d\n", getpid()); 
	}
	
	d = maxfds(); 
	for (i=3; i<d; i++)
		close(i); 
		
	if (opts.null) {
		if ((d = open("/dev/null", O_RDWR)) == -1) {
			perror("open /dev/null"); 
			exit(1);
		}
		dup2(d,0); 
		dup2(d,1); 
		dup2(d,2); 
		if (d>2) 
			close(d); 
	}

	/* keep parent environ */
	if (!opts.no_parent_env) {
		char **env = environ; 
		while (*env)
			add_env(env++); 
	}

	if (opts.setsid && my_setsid() == -1) {
		perror("my_setsid"); 
		exit(1);
	}

	if (opts.group != -1 && setgid(opts.group) == -1) {
		perror("setgid"); 
		exit(1);
	}
	if (opts.user != -1 && setuid(opts.user) == -1) {
		perror("setuid"); 
		exit(1);
	}
	execve(argv[ix], &argv[ix], opts.env.env); 
	fprintf(stderr, "execve: %s: %s\n", argv[ix], strerror(errno)); 
	return 1; 
}
