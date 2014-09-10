/* tee.c -- copy stdin to both output and named files */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990404 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-ai] [files]\n\
  -a     append to specified files, do not overwrite\n\
  -i     ignore interrupts\n", argv[0]), exit(2)

struct options {
	int no_intr; 
	int append; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

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
		case 'a': 	
			opts.append++; 
			break; 
		case 'i': 	
			opts.no_intr++; 
			break; 
		case '?': 
		default: 
			USAGE();
			break;
		}
	}
	return do_args(i, argc, argv); 
}

/* USER CODE */

#include <signal.h>

#ifdef HAVE_SYS_RESOURCE_H 
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

static int 
max_nr_files() {
	struct rlimit rlimbuf; 
	if (getrlimit(RLIMIT_NOFILE, &rlimbuf) == -1)
		return 15; 
	return rlimbuf.rlim_cur; 
}
#else
#define max_nr_files() 	(18)
#endif

int
tee(FILE *rf, char *av[], int nfiles)
{
	FILE *wf[256]; 
	int maxf = max_nr_files()-3;  /* stdin, out, err */
	char *mode; 
	int i, j; 
	FILE *orig_rf = rf; 

	if (!nfiles) {
		int c; 
		while ((c = getc(rf)) != EOF)
			putchar(c); 
		return 0; 
	}

	if (opts.append)
		mode = "ab"; 
	else
		mode = "wb"; 

	if (maxf > sizeof(wf)/sizeof(*wf))
		maxf = sizeof(wf)/sizeof(*wf); 

	for (i=0; i<nfiles; i += maxf) {
		int top = i+maxf; 
		int n; 
		char buf[2048]; 

		if (top >= nfiles)
			top = nfiles; 
		for (j=i; j<top; j++) {
			wf[j-i] = fopen(av[j], mode); 
			if (wf[j-i] == NULL)
				fprintf(stderr, "%s: can't open\n", av[j]); 
		}
		while ((n = fread(buf, sizeof(*buf), sizeof(buf), rf)) > 0) {
			/* on first run, write also to stdout */
			if (i == 0)
				fwrite(buf, sizeof(*buf), n, stdout); 
			for (j=i; j<top; j++) {
				if (wf[j-i] == NULL)
					continue; 
				fwrite(buf, sizeof(*buf), n, wf[j-i]); 
			}
		}
		for (j=i; j<top; j++) {
			if (wf[j-i] != NULL)
				fclose(wf[j-i]); 
		}
		/* at end of first run: change initial RF to first 
		 * written file --> shold have the same contents and
		 * rewindable */
		if (orig_rf == rf) {
			for (j=i; j<top; j++) {
				if (wf[j-i])
					break; 
			}
			if (j == top) {
				/* did not any open file, 
				 * hope rf is rewindable */
				if (fseek(rf, 0L, SEEK_SET) == -1) {
					fprintf(stderr, "\
error: reference file is not rewindable, and could not open any\n\
       other for reading/reading.  Exiting now...\n"); 
					exit(1); 
				}
			} else {
				if (rf != stdin) 
					fclose(rf); 
				rf = fopen(av[j], "r"); 
				if (!rf) {
					fprintf(stderr, "\
error: can not open reference file %s, exiting...\n", av[j]); 
					exit(1); 
				}
			}
		} else 
			fseek(rf, 0L, SEEK_SET); 
	}
	/* even orig_rf isn't stdin, we don't close it anyway, since
	 * caller might want to do something there */
	if (rf != orig_rf)
		fclose(rf); 
	return 0; 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	if (opts.no_intr) 
		signal(SIGINT, SIG_IGN); 

	return tee(stdin, &argv[ix], argc-ix); 
}
