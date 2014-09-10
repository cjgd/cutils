/* mail_check.c -- check for new mail */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990328 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-d secs] [-f file] cmd\n\
\n\
  -d secs     delay for SECS seconds, after each check\n\
  -f file     use this file, instead of one pointed by MAIL env var\n\
  -p          pain in the ass: keep telling you have new mail, until\n\
	        you read it \n\
\n\
  cmd         is the command to execute, via system(3), when new mail\n\
	      has been received\n\
", argv[0]), exit(2)

struct options {
	char *mbox; 
	int nr_secs; 
	int p_ass; 

	/* globals */
	int stop; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 
	int err = 0; 

	opts.nr_secs = 10; 
	opts.mbox = getenv("MAIL"); 
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
		case 'd': 
			opts.nr_secs = atoi(ARG()); 
			break; 
		case 'f': 
			opts.mbox = ARG(); 
			break; 
		case 'p': 
			opts.p_ass = 1; 
			break; 
		case '?': 
		default: 
			USAGE();
			break;
		}
	}
	if (opts.nr_secs <= 0)
		err++, fprintf(stderr, "-d secs, must be > 0\n"); 
	if (opts.mbox == NULL) 
		err++, fprintf(stderr, "\
must specifie -f mbox, or setenv MAIL environ variable\n"); 
	if (err)
		USAGE(); 

	return do_args(i, argc, argv); 
}

/* USER CODE */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

static void
catcher() {
	opts.stop++; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	char *cmd; 
	struct stat st; 
	int have_warned; 

	if (ix == argc) {
		cmd = 0; 
	} else {
		int len = 0; 
		int save_ix = ix; 
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			len += strlen(s)+1; 
		}
		cmd = malloc(len+1); 
		if (!cmd) {
			perror("malloc"); 
			exit(1); 
		}
		len = 0; 
		for (ix=save_ix; ix<argc; ix++) {
			char *s = argv[ix]; 
			len += sprintf(cmd+len, "%s ", s); 
		}
	}
	signal(SIGINT, catcher); 
	signal(SIGTERM, catcher); 
	have_warned = 0; 
	for (;!opts.stop;) {
		if (stat(opts.mbox, &st) == 0 && 
		    st.st_size != 0 && 
		    st.st_atime < st.st_mtime) {
			if (!opts.p_ass && have_warned)
				goto skip; 

			have_warned = 1; 
			if (cmd) {
				system(cmd); 
				goto skip; 
			}

			/* default action */
			if (isatty(2)) fprintf(stderr, "\
\a\a%s: You have received mail...\a\a\n", argv[0]); 
		} else {
			have_warned = 0; 
		}
	skip: 
		sleep(opts.nr_secs); 
	}
	if (cmd) 	
		free(cmd); 
	return 0; 
}
