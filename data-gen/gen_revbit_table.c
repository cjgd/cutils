/* geb_revbit_table.c -- generates a table for bit reversal */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 970119 */

#include <stdio.h>
#include <string.h>

static int
rev(int i)
{
	int j = 0;
	int k = 7;

	j |= (i & 1) << (k--); i >>= 1;
	j |= (i & 1) << (k--); i >>= 1;
	j |= (i & 1) << (k--); i >>= 1;
	j |= (i & 1) << (k--); i >>= 1;
	j |= (i & 1) << (k--); i >>= 1;
	j |= (i & 1) << (k--); i >>= 1;
	j |= (i & 1) << (k--); i >>= 1;
	j |= (i & 1) << (k--);

	return j;
}

static int
do_test(void)
{
	int i;
	int err = 0;

	for (i=0; i<256; i++) {
		if (rev(rev(i)) != i) {
			printf("error on %d\n", i);
			err++;
		}
	}
	return err;
}

static void
p_table(void)
{
	int i;
	printf("static int table[] = {");
	for (i=0; i<256; i++) {
		if ((i % 8) == 0)
			printf("\n\t");
		printf("0x%02X", rev(i));
		if (i != 255)
			printf(", ");
	}
	printf("\n};\n");
}

int
main(int argc, char *argv[])
{
	int test;

	test = 0;
	if (argc == 2 && strcmp(argv[1], "-t") == 0)
		test++;

	if (test)
		return do_test();

	p_table();
	return 0;
}
