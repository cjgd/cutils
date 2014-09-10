/* wc.c -- count words */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 960621 */

#include <stdio.h>
#include <stdlib.h>

static int nl, nc, nw; 
static int tl, tc, tw; 

char buffer[BUFSIZ*1024]; 

static int
wc(FILE *f)
{
	int next_space_will_end_word= 0; 
	int n; 
	char *s; 

	nl = nc = nw = 0;
	for(;;) {
		switch(n = fread(buffer, sizeof(char),
			sizeof(buffer)/sizeof(char), f)) {
		case -1: 
			perror("fread");
			return -1; 
		case 0: 
			if(next_space_will_end_word)
				nw++; 

			tc += nc; 
			tw += nw; 
			tl += nl;
			return 0; 

		default: 
			break;
		}

		nc += n; 
		s = buffer; 
		do {
			switch(*s++) {
			case '\n':
				nl++; 
				/* fall down */
			case '\r':
			case '\f':
			case '\t':
			case '\v':
			case ' ':
				if(next_space_will_end_word) {
					nw++; 
					next_space_will_end_word=0;
				}
				break;
			default:
				next_space_will_end_word++;
				break;
			}
		} while(--n > 0); 
	}
	return 0; 	/* NOT USED */
}

#define info(file) printf("%7d %7d %7d %s\n", nl, nw, nc, file)
#define info_tail() printf("%7d %7d %7d %s\n", tl, tw, tc, "total")

int
main(int argc, char **argv)
{
	FILE *f; 
	int flag; 

	flag = (argc > 2);

	if (argc <= 1) {
		wc(stdin);
		info("");
	} else {
		do {
			if (**++argv == '-' && argv[0][1] == '\0')
				f = stdin; 
			else if ((f = fopen(*argv, "r")) == 0) {
				perror(*argv);
				continue;
			}
			wc(f);
			info(*argv);
			if (f != stdin)
				fclose(f);
		} while(--argc > 1); 
	}
	if (flag)
		info_tail();

	exit(0);
	return 0; /* NOT USED */
}
