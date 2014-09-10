/* dupinfo.c -- print all duplicated info page sets found on info path */
/* $Id: dupinfo.c,v 1.1 1997/10/13 00:32:31 cdua Exp cdua $ */
/* Carlos Duarte, 970829/971013 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

/* /usr/local/info/gcc.info.Z 
 * 	file	-> gcc.info.Z
 * 	path	-> /usr/local/info
 *	info 	-> gcc
 */
struct dupinfo_s {
	char *file;		/* file name */
	char *path; 		/* dir of file */
	char *info; 		/* canonic name of info */
}; 

typedef struct dupinfo_s DUPINFO; 

char *def_info_path = "/usr/info:/usr/local/info"; 

void *
xmalloc(int size)
{
	void *x; 

	x = malloc(size); 
	if (!x) {
		perror("malloc"); 
		exit(1); 
	}
	return x; 
}

void *
xrealloc(void *x, int size) 
{
	x = realloc(x, size); 
	if (!x) {
		perror("realloc"); 
		exit(1); 
	}
	return x; 
}

char *
xstrdup(char *str)
{
	char *x; 

	x = xmalloc(strlen(str)+1); 
	strcpy(x, str); 
	return x; 
}

void
dup_fill(DUPINFO *dup, char *dir, char *file)
{
	char *s, *t, *tt; 

	dup->path = xstrdup(dir); 
	dup->file = xstrdup(file); 

	s = xstrdup(file); 
	t = s+strlen(s); 
	if (t-3 >= s && strcmp(t-3, ".gz") == 0) { t -= 3; *t = 0; }
	if (t-2 >= s && strcmp(t-2, ".Z") == 0) { t -= 2; *t = 0; }
	tt = t; 
	while (--t >= s && isdigit(*t))
		; 
	if (t >= s && *t == '-') {
		*t = 0; 
		tt = t; 
	} 
	if (tt-5 >= s && strcmp(tt-5, ".info") == 0) { tt -= 5; * tt = 0; }
	dup->info = s; 
}

int 
dup_cmp(const void *x, const void *y) 
{
	DUPINFO *d1 = (DUPINFO *)x; 
	DUPINFO *d2 = (DUPINFO *)y; 

	return strcmp(d1->info, d2->info); 
}

/* print records from FROM to TO inclusive both */
void
dup_print(DUPINFO *from, DUPINFO *to)
{
	while (from <= to) {
#if 0
		printf("%s/%s [%s]\n", from->path, from->file, from->info); 
#else
		printf("%s/%s\n", from->path, from->file); 
#endif
		from++; 
	}
}

int
main()
{
	char *s, *infopath, *dir; 
	DUPINFO *dup, *dup_ptr, *dup_top, *mark; 
	int dup_entries, state; 

	s = getenv("INFOPATH"); 
	if (s) 
		infopath = xstrdup(s); 
	else
		infopath = xstrdup(def_info_path); 

	dup_entries = 1024; 
	dup = xmalloc(sizeof(*dup)*dup_entries); 
	dup_ptr = dup; 
	dir = strtok(infopath, ":"); 
	while (dir && *dir) {
		DIR *d; 
		struct dirent *dd; 

		d = opendir(dir); 
		if (!d) {
			perror(dir); 
			exit(1); 
		}
		for (;;) {
			dd = readdir(d); 
			if (!dd) break; 

			if (dd->d_name[0] == '.' &&
			     (dd->d_name[1] == 0 || 
			       (dd->d_name[1] == '.' && dd->d_name[2] == 0)))
				continue; 
			
			/* warn: knows that dup_ptr only grows by 1 each loop */
			if ((dup_ptr - dup) == dup_entries) {
				dup = xrealloc(dup, dup_entries*2*sizeof(*dup));
				dup_ptr = dup + dup_entries; 
				dup_entries *= 2; 
			}
			dup_fill(dup_ptr, dir, dd->d_name); 
			dup_ptr++; 
		}
		closedir(d); 
		dir = strtok(0, ":"); 
	}

	/* here we have dup structure filled with paths, filenames, and 
	   canonic info names */
	qsort(dup, dup_ptr - dup, sizeof(*dup), dup_cmp); 

	dup_top = dup_ptr; 
	dup_ptr = &dup[1]; 

	mark = 0; 
	state = 0; 
	for (; dup_ptr < dup_top; dup_ptr++) {

		switch (state) {
		case 0: 
			if (strcmp(dup_ptr[0].info, dup_ptr[-1].info) != 0)
				continue; 

			mark = &dup_ptr[-1];
			if (strcmp(dup_ptr[0].path, dup_ptr[-1].path) == 0)
				state = 1; 
			else 
				state = 2; 
			break; 
		case 1:  
			if (strcmp(dup_ptr[0].info, dup_ptr[-1].info) != 0) {
				state = 0; 
				mark = 0; 
				break; 
			}
			if (strcmp(dup_ptr[0].path, dup_ptr[-1].path) != 0) {
				state = 2; 
				break; 
			}
			break; 
		case 2: 
			if (strcmp(dup_ptr[0].info, dup_ptr[-1].info) != 0) {
		last:		
				dup_print(mark, &dup_ptr[-1]); 
				putchar('\n'); 
				mark = 0; 
				state = 0; 
			}
			break; 
		}
	}
	if (mark && state == 2) goto last; 
	for (dup_ptr = dup; dup_ptr < dup_top; dup_ptr++) {
		free(dup_ptr->file); 
		free(dup_ptr->path); 
		free(dup_ptr->info); 
	}
	free(dup); 
	return(0); 
}
