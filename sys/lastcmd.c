/* lastcmd.c -- show who did execute the last commands */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990328 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <pwd.h>
#include <dirent.h>
#include <sys/acct.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-u user1 [-u user2]...] [cmds]\n", argv[0]), exit(2)

struct options {
	int *uids; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 
	int nusers = 0; 

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
		case 'u': {
			struct passwd *p; 
			char *user = ARG(); 
			if (!user)
				USAGE(); 
			p = getpwnam(user); 
			if (!p) {
				fprintf(stderr, "\
warning: %s is an invalid username\n", user); 
				break; 
			}
			nusers++;
			if (nusers == 1) /* first allocations */
				opts.uids = 
					malloc((nusers+1)*sizeof(*opts.uids)); 
			else
				opts.uids = realloc(opts.uids, 
						(nusers+1)*sizeof(*opts.uids)); 
			if (!opts.uids) {
				perror("alloc");
				exit(1);
			}
			opts.uids[nusers-1] = p->pw_uid; 
			break; 
		}
		case '?': 
		default: 
			USAGE();
			break;
		}
	}
	if (opts.uids)
		opts.uids[nusers] = -1;  /* no such user -1 ? */
	return do_args(i, argc, argv); 
}

/* USER CODE */

#define ADM_DIR "/var/adm"

#ifndef MAXPATHLEN 
#define MAXPATHLEN 4096
#endif

void
die(char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	if (fmt[*fmt ? strlen(fmt)-1 : 0] != '\n')
		fprintf(stderr, "\n");
	exit(1); 
}

void
warn(char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	if (fmt[*fmt ? strlen(fmt)-1 : 0] != '\n')
		fprintf(stderr, "\n");
}

static void
pr_one(struct acct *ac) {
	struct tm *tm; 
	time_t t; 

	printf("%16s ", ac->ac_comm); 
	printf("%s", (ac->ac_flag & AFORK) ? "F" : " "); 
	printf("%s", (ac->ac_flag & ASU  ) ? "S" : " "); 
	printf("%s", (ac->ac_flag & ACORE) ? "C" : " "); 
	printf("%s", (ac->ac_flag & AXSIG) ? "K" : " "); 
	printf("  "); 
	t = ac->ac_btime; 
	tm = gmtime(&t); 
	printf("%2d-%2d %2d:%2d",
			tm->tm_mday, tm->tm_mon, tm->tm_hour, tm->tm_min);
	printf(" -> "); 
	t = ac->ac_btime+ac->ac_etime; 
	tm = gmtime(&t); 
	printf("%2d-%2d %2d:%2d",
			tm->tm_mday, tm->tm_mon, tm->tm_hour, tm->tm_min);
	printf(" (%ld)\n", (long)ac->ac_etime-0); 
}

static void
do_pracct(char *fn, char **cmds) {
	struct acct ac; 
	char ac_file[MAXPATHLEN+1]; 
	FILE *f; 

	sprintf(ac_file, "%s/%s", ADM_DIR, fn); 
	f = fopen(ac_file, "r"); 
	if (!f) {
		perror(ac_file); 
		return; 
	}
	for (;;) {
		int n; 
		int *u; 
		char **cmd; 

		n = fread(&ac, sizeof(ac), 1, f); 
		if (n < sizeof(ac)) {
			if (ferror(f) || (feof(f) && n != 0)) {
				warn("%s: file truncated\n", fn); 
			}
			break; 
		}

		/* uid filters */
		u = opts.uids; 
		if (u) while (*u != -1) {
			if (*u == ac.ac_uid)
				break; 
			u++; 
		}
		if (u && *u == -1)
			continue; 
		/* cmd test */
		cmd = cmds; 
		if (cmd) while (*cmd) {
			if (strstr(ac.ac_comm, *cmd))
				break; 
			cmd++; 
		}
		if (cmd && *cmd == NULL)
			continue; 
		/* ok, print this entry */
		pr_one(&ac); 
	}
	fclose(f); 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	DIR *dir; 
	struct dirent *dent; 
	char **av; 

	int count = 0; 

	if (argc == ix)
		av = NULL; 
	else
		av = &argv[ix]; 

	if ((dir = opendir(ADM_DIR)) == NULL)
		die("can not open directory %s", ADM_DIR); 

	while ((dent = readdir(dir)) != NULL) {
		if (strncmp("pacct", dent->d_name, 5) == 0) {
			do_pracct(dent->d_name, av); 
			count++; 
		}
	}
	closedir(dir); 
	if (!count)
		warn("could not found any account file\n"); 

	return 0; 
}
