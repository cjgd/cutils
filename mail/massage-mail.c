/* massage-mail.c -- removes from start upto beginning of binary contents */
/* Carlos Duarte <cgd@teleweb.pt>, 991123 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s {-p prefix, -s suffix} files...\n\
\n\
for each named FILE, builds a new PREFIX+FILE+SUFFIX file, with \n\
initial mail header and non-binary portion removed.\n\
\n\
does not work with stdin\n\
", argv[0]), exit(2)

struct options {
	char *prefix; 
	char *suffix; 
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
		case 'p': 	
			opts.prefix = ARG(); 
			break; 
		case 's': 	
			opts.suffix = ARG(); 
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

static int 
get_size(char *pref, char *name, char *suff)
{
	int sz = 0; 
	sz++; 		/* last \0 */
	if (pref) sz += strlen(pref); 
	if (name) sz += strlen(name); 
	if (suff) sz += strlen(suff); 
	return sz; 
}

static void
do_scan(char *fn)
{
	char buf[120];
	FILE *rf, *wf; 
	int state=0; 
	fpos_t pos_temp, pos; 
	int len = 0; 

	rf = fopen(fn, "r"); 
	if (!rf) {
		fprintf(stderr, "can't open %s\n", fn); 
		return; 
	}
	for (;;) {
		fgetpos(rf, &pos_temp);
		if (!fgets(buf, sizeof buf, rf))
			break; 

		if (strncmp(buf, "begin ", 6) == 0) {
			fsetpos(rf, &pos_temp); 
			goto print; 
		}

		switch (state) {
		case 0: 
			memcpy(&pos, &pos_temp, sizeof(pos)); 
			if (strncmp(buf, "Content-", 8) == 0) 
				state=1; 
			break; 
		case 1: 
			if (buf[0] == '\n')
				state=2; 
			break; 
		case 2: 
			len = strlen(buf); 
			state=3; 
			break; 
		case 3: case 4: case 5: case 6: 
			if (len == strlen(buf))
				state++; 
			else 
				state=0; 
			break; 
		default: 
			fsetpos(rf, &pos); 
			goto print; 
		}
	}

print: 
	if (!ferror(rf) && !feof(rf)) {
		char name_buf[30], *name; 
		int sz = get_size(opts.prefix, fn, opts.suffix); 
		if (sz > sizeof name_buf) {
			name = malloc(sz); 
			if (!name) {
				perror("malloc"); 
				goto err; 
			}
		} else 
			name = name_buf; 
		*name = 0; 
		if (opts.prefix) strcat(name, opts.prefix);
		strcat(name, fn); 
		if (opts.suffix) strcat(name, opts.suffix);

		wf = fopen(name, "w"); 
		if (!wf) {
			perror(name); 
			goto err; 
		}

		while (fgets(buf, sizeof buf, rf)) {
			fputs(buf, wf); 
		}
		fclose(wf); 
	}
err: 
	fclose(rf); 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	if ((opts.prefix == NULL && opts.suffix == NULL) || ix == argc)
		USAGE(); 

	for (; ix<argc; ix++) {
		char *s = argv[ix]; 
		do_scan(s); 
	}
	return 0; 
}
