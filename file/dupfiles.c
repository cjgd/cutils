/* dupfiles.c -- print dupicated files found at dirs */
/* $Id$ */
/* Carlos Duarte, 970829/970830 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <sys/types.h>
#include <dirent.h>

#include <sys/stat.h>
#include <unistd.h>

char *program_name; 
int verbose = 0; 

typedef struct {
	char *path; 
	char *alias; 
	int inum; 
} STORE; 

void
usage(void)
{
	fprintf(stderr, "\
Usage: %s dirs ...\n\
\n\
  Print duped files (with same name) found at dirs.\n", program_name); 
	
	exit(0); 
}

void 
dam(char *s)
{
	perror(s); 
	exit(1); 
}

void *
xmalloc(int size)
{
	void *x = malloc(size); 
	if (!x) dam("malloc"); 
	return x; 
}

void *
xrealloc(void *x, int size)
{	
	x = realloc(x, size); 
	if (!x) dam("realloc"); 
	return x; 
}

char *
xstrdup(char *s)
{
	return strcpy(xmalloc(strlen(s)+1), s); 
}

char *
xconcat(char *str, ...)
{
	va_list ap; 
	char *x, *s; 
	int n; 

	n = 1024; 
	x = xmalloc(n); 
	*x = 0; 
#define DO_CPY(s)                                                            \
do {                                                                         \
	while (strlen(s)+strlen(x)+1 >= n)                                   \
		x = xrealloc(x, n*=2);                                       \
	strcat(x, s);                                                        \
} while (0)

	DO_CPY(str); 
	va_start(ap, str); 
	for (;;) {
		s = va_arg(ap, char *); 
		if (!s) break; 
		DO_CPY(s); 
	}
	va_end(ap); 
#undef DO_CPY
	x = xrealloc(x, strlen(x)+1); 
	return x; 
}

char *
get_alias(char *str)
{
	char *s = xstrdup(str); 
	char *t; 

	t = s+strlen(s); 
	t -= 3; if (t>=s && strcmp(t, ".gz") == 0) *t = 0; else t += 3; 
	t -= 2; if (t>=s && strcmp(t, ".Z") == 0) *t = 0; else t += 2; 
	return s;  
}

int
st_cmp_inode(const void *a, const void *b)
{
	STORE *x = (STORE *)a; 
	STORE *y = (STORE *)b; 

	return x->inum - y->inum; 
}

int
st_cmp_alias(const void *a, const void *b)
{
	STORE *x = (STORE *)a; 
	STORE *y = (STORE *)b; 

	return strcmp(x->alias, y->alias); 
}

void
xstat(char *name, struct stat *stat_buf)
{
	char buf[32]; 
	if (lstat(name, stat_buf) == -1) {
		sprintf(buf, "fstat: %s", name); 
		dam(buf); 
	}
}

int
main(int argc, char *argv[])
{
	STORE *ss, *sp; 
	STORE *top; 
	STORE *last; 
	int i; 
	int sn; 
	int were_eq; 

	program_name = argv[0]; 

	if (argc >= 2 && strcmp(argv[1], "-v") == 0) {
		verbose++; 
		argc--; 
		argv++; 
	}

	if (argc < 2) {
		usage(); 
	}

	sn = 1024; 
	ss = xmalloc(sn*sizeof(*ss)); 
	sp = ss; 	
	for (i=1; i < argc; i++) {
		DIR *d; 
		struct dirent *dr; 
		char *dir; 

		dir = argv[i]; 
		d = opendir(dir); 
		if (!d) dam(dir); 
		if (verbose) 
			printf("Entering directory `%s'...\n", dir); 
		while ((dr = readdir(d)) != 0) {
			struct stat stat_buf; 

			if (dr->d_name[0] == '.' && (dr->d_name[1] == 0 || 
			     (dr->d_name[1] == '.' && dr->d_name[2] == 0)))
				continue; 

			if (sp-ss == sn) {
				ss = xrealloc(ss, sn*2*sizeof(*ss)); 
				sp = &ss[sn];
				sn *= 2; 
			}
			sp->path = xconcat(dir, "/", dr->d_name, 0); 
			sp->alias = get_alias(dr->d_name); 
			xstat(sp->path, &stat_buf); 
			sp->inum = stat_buf.st_ino; 

			if (verbose)
				printf("Reading file `%s'...\n", sp->path); 
			sp++; 
		}
		closedir(d); 
	}
	top = sp;

	/* unify files with same inode into one newline separated path */
	qsort(ss, top-ss, sizeof(*ss), st_cmp_inode); 
	last = sp; 
	for (sp=ss+1; sp<top; sp++) {
		if (last->inum == sp->inum) {
			last->path = xrealloc(last->path,
					strlen(last->path)+strlen(sp->path)+2); 
			strcat(last->path, "\n"); 
			strcat(last->path, sp->path); 
			free(sp->path); free(sp->alias); sp->path = 0; 
		} else {
			last = sp;
		}
	}
	last = ss; 
	for (sp=ss; sp<top; sp++) {
		if (sp->path != 0)
			*last++ = *sp; 
	}
	top = last; 
		
	/* check duped alias */
	qsort(ss, top-ss, sizeof(*ss), st_cmp_alias); 
	were_eq=0; 
	for (sp = ss+1; sp<top; sp++) {
		if (strcmp(sp[0].alias, sp[-1].alias) == 0) {
			puts(sp[-1].path); 
			were_eq++; 
		} else if (were_eq) {
		last: 
			were_eq = 0; 
			puts(sp[-1].path);
			putchar('\n');
		}
	}
	
	if (were_eq) goto last; 

	for (sp = ss; sp<top; sp++) {
		free(sp->alias); 
		free(sp->path); 
	}
	free(ss); 
	return 0; 
}
