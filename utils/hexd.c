/* hexd.c -- hexadecimal dumper */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990612/990623 */

/*
TODO
	. -r, -rt -- reverse operation
	. error check for writes to output?
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int do_args(int first, int ac, char *av[]); 

#define ARG() \
s[1] ? (t=s+1, s+=strlen(s)-1, t) : ++i <argc ? argv[i] : (USAGE(),(char *)0)

#define USAGE() fprintf(stderr, "\
Usage: %s [-u] [-s#] [-n#] [-m] [-fN[:M[:B]]] [-o] [-t] [-r[t]] [files]\n\
\n\
  -u    upper case hex letters\n\
  -s#   start at # bytes, if -# start at # bytes before end\n\
  -n#   only process # bytes\n\
  -mp prefix\n\
  -ms suffix\n\
	adds prefix and suffix to each group \n\
	ex: -mp \"0x\" -ms \", \", produce: \"0x11aa, 0x22bb, \" ...\n\
  -f N[:M[:B]]\n\
	output format: N bytes per row, grouped M by M bytes, on base B \n\
	(def: 16:2:16)\n\
  -o    do not output offset\n\
  -t    do not append text (ascii)\n\
  -r    reverse operation: hexdump to binary\n\
  -rt     prefer information given on text portion, where aplicable\n\
\n", argv[0]), exit(2)

struct options {
	int upper; 
	int start; 
	int length; 
	char *len_buf;
	char *prefix; 
	char *suffix; 
	int n_per_line; 
	int m_per_group; 
	int base; 
	int no_offset; 
	int no_text; 
	int reverse; 
	int rev_use_text; 
} opts; 

int
main(int argc, char *argv[]) 
{
	int i; 

	opts.n_per_line = 16; 
	opts.m_per_group = 2; 
	opts.base = 16; 
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
		case 'u': 
			opts.upper++; 
			break; 
		case 's': 
			opts.start = atoi(ARG()); 
			break; 
		case 'n': 
			opts.length = atoi(ARG()); 
			break; 
		case 'm': 
			++s; 
			if (*s == 'p')
				opts.prefix = ARG(); 
			else if (*s == 's')
				opts.suffix = ARG();
			else 
				USAGE();
			break; 
		case 'f': {
			char *arg = ARG(); 
			char *p; 
#define load(what) \
			if ((p = strchr(arg, ':'))) 	\
				*p = 0; 		\
			what = atoi(arg); 		\
			if (!p)				\
				break; 			\
			arg = p+1; 
			do {
				load(opts.n_per_line); 
				load(opts.m_per_group); 
				load(opts.base); 
			} while (0);
#undef load
			if (opts.n_per_line == 0 || opts.m_per_group == 0)
				USAGE(); 
			if (opts.n_per_line < opts.m_per_group)
				USAGE();
			break; 
		}
		case 'o': 
			opts.no_offset++; 
			break; 
		case 't': 
			opts.no_text++; 
			break; 
		case 'r': 
			opts.reverse++; 
			if (s[1] == 't') {
				if (s[2] != '\0')
					USAGE(); 
				opts.rev_use_text++; 
			}
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

#include <ctype.h>
#include <stdarg.h>
#include <errno.h>

static void die(char *msg, ... ) {
#define sz 1024
	char mymsg[sz+1]; 
	int i; 
	va_list ap; 
	char *s = msg; 

	for (i=0; *s && i<sz; ) {
		if (s[0] != '$' || s[1] != '!') 
			mymsg[i++] = *s++;
		else {
			char *err = strerror(errno); 
			s += 2; 
			while (i<sz && *err) 
				mymsg[i++] = *err++; 
		}
	}
	mymsg[sz] = 0; 
	if (i<=sz)
		mymsg[i] = 0; 
		
	va_start(ap, msg); 
	vfprintf(stderr, mymsg, ap); 
	va_end(ap); 
	exit(1);
#undef sz
}

static int
print_base(unsigned int c, int base) 
{
	static char lmap[] = "abcdefghijklmnopqrstuvwxyz"; 
	static char umap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
	char buf[8+1];
	char *s, *map; 
	unsigned int big; 

	if (base < 2 && base > 36)
		base = 16; 

	map = lmap; 
	if (opts.upper)
		map = umap; 

	s = &buf[8+1]; 
	*--s = 0; 
	c &= 0xff;
	big = 0xff; 
	do {
		int d = c%base; 
		if (d < 10)
			*--s = d+'0'; 
		else
			*--s = map[d-10]; 
		c /= base; 
	} while ((big /= base) != 0); 
	return printf("%s", s); 
}

static void 
hexd(char *s) {
	FILE *f = stdin; 
	int offset; 
	int c=0; 
	char *fn = s; 
	int count; 

	if (!fn)
		fn = "stdin"; 

	if (s != NULL && strcmp(s, "-") != 0) {
		f = fopen(s, "r"); 
		if (!f) 
			die("%s: $!\n", s); 

	}
	if (opts.start) {
		if (opts.start<0) {
			if (fseek(f, opts.start, SEEK_END) != 0)
				die("can't seek on %s: $!\n", fn);
		} else {
			if (fseek(f, opts.start, SEEK_SET) != 0) {
				int n = opts.start; 
				while (n--) 
					if (getc(f) == EOF)
						break; 
			}
		}
	}
	offset = ftell(f); 
	c = getc(f); 
	count=1; 
	while (c != EOF) {
		int i; 
		for (i=0; i<opts.n_per_line; i++) {
			int l=0; 
			if (!i && !opts.no_offset)
				printf("%07x: ", offset);
			if (i && i%opts.m_per_group == 0)
				printf(" "); 
			if (isprint(c) && !isspace(c))
				opts.len_buf[i] = c; 
			else 
				opts.len_buf[i] = '.'; 
			offset++; 
			if (opts.prefix) 
				l += printf("%s", opts.prefix); 
			l += print_base(c, opts.base); 
			if (opts.suffix) 
				l += printf("%s", opts.suffix); 
			if ((c = getc(f)) == EOF || 
			    (opts.length && ++count > opts.length)) {
				int n = opts.n_per_line; 
				int m = opts.m_per_group;
				int k = n*l + n/m-(n%m==0);
				++i; 
				    k-= i*l + i/m-(i%m==0);
				while (k-- >0)
					printf(" ");
				c = EOF; 
				break;
			}
		}
		opts.len_buf[i] = 0; 
		if (!opts.no_text)
			printf(" %s", opts.len_buf); 
		printf("\n"); 
	}
	if (ferror(f))
		die("error reading %s: $!\n", fn); 
	if (f != stdin && fclose(f)!=0)
		die("error closing %s: $!\n", fn); 
}

static int
do_args(int ix, int argc, char *argv[]) 
{
	opts.len_buf = malloc(opts.n_per_line+1); 
	if (!opts.len_buf) 
		die("mallocing %d bytes: $!", opts.n_per_line); 
		
	if (ix == argc) {
		hexd(0); 
	} else {
		for (; ix<argc; ix++) {
			char *s = argv[ix]; 
			hexd(s); 
		}
	}

	if (opts.len_buf)
		free(opts.len_buf); 

	return 0; 
}
