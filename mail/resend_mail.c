/* resend_mail.c -- resends mail, without looping */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990609 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SENDMAIL "/usr/lib/sendmail"

#include "die_warn.c"

static char *swallow(FILE *f, int *outlen) 
{
	char *buf; 
	int c, len, size; 

	len = size = 0; 
	buf = 0; 

	while ((c = getc(f)) != EOF) {
		if (len >= size) {
			size += 1024; 
			buf = buf ? realloc(buf, size) : malloc(size); 
			if (!buf)
				die("alloc: $!");
		}
		buf[len++] = c; 
	}
	if (outlen)
		*outlen = len; 
	return buf; 
}

static char *get_from(char *buf, int len)
{
	char *from, *f; 
	char *t, *s = buf; 

	while (s-buf<len && *s != ' ')
		s++; 

	if (s-buf>=len) {
		char *p;
	err:
		p = strchr(t=buf, '\n'); 
		if (p)
			*p = 0; 
		die("`%s': bad headers", buf); 
	}
	s++; 
	t=s; 
	while (t-buf<len && !isspace(*t))
		t++; 

	if (t-buf>=len) 
		goto err; 
	from = malloc(t-s);
	if (!from)
		die("alloc: $!"); 
	f = from; 
	while (s<t)
		*f++ = *s++; 
	*f = 0; 
	return from; 
}

int main(int argc, char *argv[]) {
	char **dest; 
	int i, j, n, len; 
	char *buf, *from, *cmd; 
	FILE *p; 
	int have_dest; 

	set_progname(argv[0]); 
	if (argc < 2) 
		die("usage: $0 dest1 [dest2...]"); 

	dest = malloc(argc*sizeof(*dest)); 
	if (!dest)
		die("malloc: $!"); 

	for (i=1; i<argc; i++)
		dest[i-1] = argv[i]; 
	dest[i] = 0; 

	buf = swallow(stdin, &len); 
	from = get_from(buf, len); 

	n = 0; 
	for (i=0; dest[i]; i++) {
		if (strcmp(dest[i], from) == 0)
			dest[i] = 0; 
		else
			n += strlen(dest[i])+1;
	}
	cmd = malloc(strlen(SENDMAIL)+1+n);
	if (!cmd)
		die("alloc");

	strcpy(cmd, SENDMAIL); 
	have_dest=0; 
	for (j=0; j<i; j++) {
		if (dest[j]) {
			strcat(cmd, " "); 
			strcat(cmd, dest[j]); 
			have_dest++; 
		}
	}
	if (!have_dest) 
		goto exit; /* no destinataires */

#ifndef TEST
	if ((p = popen(cmd, "w")) == NULL) 
		die("popen: can't exec %s: $!", cmd); 
	if (fwrite(buf, 1, len, p) != len) 
		die("fwrite: $!"); 

	pclose(p); 
#else
	puts(cmd); 
#endif

exit: 
	free(dest); 
	free(buf); 
	free(from); 
	free(cmd); 
	exit(0); 
}
