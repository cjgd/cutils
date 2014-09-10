/* tr.c -- translate input, doing specified conversions NEED WORK */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990404 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-cds] S1 [S2]\n\
  -c    complement sense of S1 (NOT IMPLEMENTED)\n\
  -d    delete chars of S1 (works like: S1 '')\n\
  -s    squeezes occurences of chars of S1 (NOT IMPLEMENTED)\n\
", argv[0]), exit(2)

struct options {
	int complement; 
	int delete; 
	int squeeze; 
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
		case 'c': 
			opts.complement++; 
			break; 
		case 'd': 
			opts.delete++; 
			break; 
		case 's': 
			opts.squeeze++; 
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

struct set {
	char *p; 	/* pointer in string */
	int c; 		/* current char */
	int c2; 	/* second value in a range. -1 if no range */
}; 

/* return value at *tp, move *tp to last character converted */
static int 
get_val(char **tp) {
	char *t = *tp; 
	int val = 0; 

	if (t[0] == '\\') {
		++t; 
		switch (t[0]) {
		case 0: case 1: case 2: case 3: 
		case 4: case 5: case 6: case 7: 
			val = *t - '0'; 
			while (t[1] >= '0' && t[1] <= '9') {
				val = val*8 + *++t - '0'; 
			}
			break; 

		case 'n':  val = '\n'; break; 
		case 'v':  val = '\v'; break; 
		case 't':  val = '\t'; break; 
		case 'a':  val = '\a'; break; 
		case 'b':  val = '\b'; break; 
		case 'f':  val = '\f'; break; 
		case 'r':  val = '\r'; break; 
		case '\\': val = '\\'; break; 
		default: 
			val = *t; 
			break; 
		}
	} else 
		val = t[0]; 
	*tp = t; 
	return val; 
}

static int 
get_next(struct set *s) {
	char *t = s->p; 

redo: 
	if (t[0] == 0) {
		s->c = -1; 
		return 0; 
	}

	if (s->c2 != -1) {
		++s->c; 
range: 
		if (s->c <= s->c2) 
			return 1; 
		s->c2 = -1; 
		s->p = ++t; 
		goto redo; 
	}
	s->c = get_val(&t); 
	if (t[1] != '-' || t[2] == '\0') {
		s->p = ++t; 
		return 1; 
	}
	t += 2; 
	s->c2 = get_val(&t); 
	s->p = t; 
	goto range; 
}

static void
fill_map(short *map, char *set1, char *set2) {
	struct set s1 = { set1 ? set1 : "", -1, -1 }; 
	struct set s2 = { set2 ? set2 : "", -1, -1 }; 
	int i; 

	for (i=0; i<256; i++)
		map[i] = i; 

	while (get_next(&s1)) {
		get_next(&s2); 
		map[(unsigned char)s1.c] = s2.c; 
	}
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	short map[256]; 
	int c; 
	char *set2, *set1; 

	set1 = set2 = 0; 
	switch (argc-ix) {
	case 0: 
		fprintf(stderr, "At least one set must be given\n"); 
		USAGE(); 
	case 2: set2 = argv[ix+1]; 
	case 1: set1 = argv[ix]; 
		break; 
	default: 
		USAGE(); 
	}
	if (opts.delete && set2) {
		fprintf(stderr, "-d accepts only one set\n"); 
		USAGE(); 
	}

	fill_map(map, set1, set2); 

	while ((c = getchar()) != EOF) {
		unsigned char uc = c; 
		if (map[uc] < 0)
			continue; 
		putchar(map[uc]); 
	}
	return 0; 
}

/* 
TODO: 

valid combinations 

tr -ds [-c] string1 string2
	Delete all characters (or complemented characters) in string1.
	Squeeze all characters in string2

tr -d [-c] string1
	Delete all characters (or complemented characters) in string1

tr -s [-c] string1
	Squeeze all characters (or complemented characters) in string1

tr [-cs] string1 string2
	Replace all characters (or complemented characters) in string1 with
	the character in the same position in string2.  If the -s option is
	specified, squeeze all the characters in string2.
*/
