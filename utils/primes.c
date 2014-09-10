/* primes.c -- a simple prime number tester */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 960613 */

#include <stdio.h>
#include <stdlib.h>

static int 
prime_test(unsigned int n)
{
	unsigned int k, k2;

	switch (n) {
	case 0:
	case 4:
	case 6:
	case 8:
	case 9:
		return 0;
	case 1:
	case 2:
	case 3:
	case 5:
	case 7:
		return 1;
	default:
		break;
	}

	/* ignore even numbers */
	if ((n & 1) == 0)
		return 0;

	/* start testing division by 3 and up, two in two */
	k = 3;
	k2 = 9;

	do {
		if ((n % k) == 0)
			return 0;

		k += 2;
	} while ((k2 += (k << 2) - 4) <= n);

	return 1;
}

int
main(int argc, char *argv[])
{
	unsigned int fr, to;

	if (argc < 2 || argc > 3) {
		fprintf(stderr, "\
usage: %s <number> | <from_number> <to_number>\n", argv[0]);
		exit(1);
	}
	to=0; 
	if (argc >= 2) fr = strtoul(argv[1], 0, 0);
	if (argc >= 3) to = strtoul(argv[2], 0, 0);

	if (!to)
		to = fr;

	if (fr == to) {
		printf("%d: %s a prime.\n", fr, prime_test(fr) ? "is" : "not");
		return 0;
	}
	fr--;
	while (++fr <= to)
		if (prime_test(fr))
			printf("%u: is prime\n", fr);
	return 0;
}
