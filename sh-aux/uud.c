/* uud.c --
 * 	take an input consisting of several spitted
 * 	uuencode files, with possible garbage, and 
 * 	pass only the valid data to uudecode(1)
 */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970221 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void *
xrealloc(void *ptr, int size)
{
	void *new = realloc(ptr, size);
	if (new)
		return new;
	perror("realloc"); 
	exit(1);
}

void *
xmalloc(int size)
{
	void *ret = malloc(size);
	if (ret)
		return ret;
	perror("malloc"); 
	exit(1);
}

FILE *
xpopen(char *cmd, char *mode)
{
	FILE *f = popen(cmd, mode);
	if (f)
		return f;
	perror(cmd); 
	exit(1);
}

int
isuud(char *line)
{
	int len;
	int ulen;

	len = strlen(line);

	if (len > 62)
		return 0;
	if (strcmp(line, "`\n") == 0)
		return 1;

	ulen = *line - ' ';
	if (ulen % 3) {
		ulen = (ulen / 3 + 1) * 4;
	} else {
		ulen = ulen / 3 * 4;
	}
	return ulen == len - 2;
}

char *
nextline(int *cp, char ***vp)
{
	static FILE *f;
	static struct {
		char *ptr;
		int size;
	} ln = {
		0, 0
	};

	int c;
	char *s, *top;

	if (f && feof(f)) {
		fclose(f);
		f = 0;
	}
	while (!f) {
		if (*cp == 0) {
			if (ln.ptr)
				free(ln.ptr);
			return 0;
		}
		if ((*vp)[0][0] == '-' && (*vp)[0][1] == '\0')
			f = stdin;
		else
			f = fopen((*vp)[0], "r");

		(*cp)--;
		(*vp)++;
	}

	if (ln.size == 0) {
		ln.size += 1024;
		ln.ptr = xmalloc(ln.size);
	}

	top = ln.ptr+ln.size-1;
	s = ln.ptr;
	for (;;) {
		c = getc(f);
		if (c == EOF)
			break;
		if (s >= top) {
			char *new;
			ln.size *= 2;
			new = xrealloc(ln.ptr, ln.size);
			s = new + (s - ln.ptr);
			ln.ptr = new;
			top = ln.ptr+ln.size-1;
		}
		*s++ = c;
		if (c == '\n')
			break;
	}
	*s = 0;
	return ln.ptr;
}

int
main(int argc, char *argv[])
{
	char *fakeargv[1];
	int r; unsigned int mode;

	char **av;
	int ac;

	char *line;

	FILE *out = 0;

	if (argc == 1) {
		fakeargv[0] = "-";
		ac = 1;
		av = fakeargv;
	} else {
		av = &argv[1];
		ac = argc - 1;
	}

	for (;;) {
		line = nextline(&ac, &av);
		if (!line)
			goto end_of_input;

		r = sscanf(line, "begin %o %*s\n", &mode);
		if (r != 1)
			continue;

		/*
		out = xpopen("cat", "w");
		*/
		out = xpopen("uudecode", "w");
		fputs(line, out);
		for (;;) {
			line = nextline(&ac, &av);
			if (line == 0)
				goto end_of_input;

			if (strcmp(line, "end\n") == 0) 
				break; 

			if (isuud(line))
				fputs(line, out);
		}
		fputs(line, out);
		pclose(out);
		out = 0;
	}
end_of_input:
	if (out)
		pclose(out);
	exit(0);
	return(0);
}
