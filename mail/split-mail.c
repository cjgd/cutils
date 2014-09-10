/* split-mail.c -- split a MBOX file into several files: one per message */
/* Carlos Duarte <cgd@teleweb.pt>, 991123 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_PREFIX "xx"

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-p prefix] [files]\n\
\n\
  -p prefix     created files will be named: prefix.1 prefix.2 ...\n\
", argv[0]), exit(2)

struct options {
	char *prefix; 
	/***/
	int count; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.prefix = DEFAULT_PREFIX; 
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
		case 'p': 	
			/* one arg option */
			opts.prefix = ARG(); 
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

/* compute size needed to make string PREFIX + '.' + NUM */
static int 
get_size(char *prefix, int num)
{
	int sz=0; 
	sz += 1; /* last \0 */
	sz += strlen(prefix); 
	sz += 1; /* middle '.' */
	do {
		sz++; 
	} while (num /= 10); 
	return sz; 
}

static void
do_split(char *fn)
{
	char buf[99]; 
	char name_pool[16], *name=0; 
	FILE *rf, *wf=0; 

	if (fn == NULL || strcmp(fn, "-") == 0)
		rf = stdin; 
	else 
		rf = fopen(fn, "r"); 
	if (!rf) {
		fprintf(stderr, "warning: unable to read %s\n", 
			fn ? fn : "null"); 
		return; 
	}
	while (fgets(buf, sizeof buf, rf)) {
		if (strncmp(buf, "From ", 5) == 0) {
			int sz = get_size(opts.prefix, ++opts.count); 
			if (sz > sizeof(name_pool)) {
				name = malloc(sz); 
				if (!name) {
					fprintf(stderr, 
						"unable to malloc space"); 
					goto err; 
				} 
			} else name = name_pool; 
			sprintf(name, "%s.%d", opts.prefix, opts.count); 
			if (wf) {
				fclose(wf);
				wf=0; 
			}
		}
		if (!name)
			continue; 
		if (!wf) {
			wf = fopen(name, "w"); 
			if (!wf) {
				fprintf(stderr, 
					"unable to open %s for writing", name); 
				goto err; 
			}
			if (name != name_pool) free(name); 
		}
		fputs(buf, wf); 
		if (strchr(buf, '\n') == 0) {
			/* line bigger than sizeof buf, print remaining */
			int c; 
			while ((c = getc(rf)) != EOF) {
				putc(c, wf); 
				if (c == '\n')
					break;
			}
		}
	}
err: 
	if (name && name != name_pool) free(name); 
	if (wf) fclose(wf); 
	if (rf != stdin) fclose(rf); 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	if (opts.prefix[0] == '\0') USAGE(); 
	if (ix == argc) {
		do_split(0); 
	} else {
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			do_split(s); 
		}
	}
	return 0; 
}

