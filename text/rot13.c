/* rot13.c -- simple rot13 program */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970523 */


/*
 * performs a sort of, tr "[a-m][n-z][A-M][N-Z]" "[n-z][a-m][N-Z][A-M]", 
 * on input files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [files]\n", argv[0]), exit(2)

/*
struct options {
	int f_n; 
	char *farg; 
} opts; 
*/

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
		case '?': 
		default: 
			USAGE();
			break;
		}
	}
	return do_args(i, argc, argv); 
}

/* USER CODE */

char table[256] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
	18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 
	33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
	58, 59, 60, 61, 62, 63, 64, 
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
	91, 92, 93, 94, 95, 96, 
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
	123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 
	136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 
	149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 
	162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 
	175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 
	188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 
	201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 
	214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 
	227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 
	253, 254, 255 
};

void
rot13(char *fn) {
	FILE *f = stdin;
	int c; 

	if (fn && strcmp(fn, "-") != 0 && (f = fopen(fn, "r")) == NULL) {
		perror(fn); 
		return; 
	}

	for (;;) {
		c = getc(f);
		if (c == EOF) 
			break; 

#ifdef NO_TABLE
		if (c>='a' && c<='m') 
			putchar(c+'n'-'a'); 
		else if (c>='n' && c<='z')
			putchar(c+'a'-'n'); 
		else if (c>='A' && c<='M')
			putchar(c+'N'-'A');
		else if (c>='N' && c<='Z')
			putchar(c+'A'-'N'); 
		else 
			putchar(c); 
#else
		putchar(table[(unsigned char)c]); 
#endif
	}
	if (f != stdin)
		fclose(f); 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	if (ix == argc) {
		rot13(0); 
	} else {
		rot13(argv[ix]);
	}
	return 0; 
}
