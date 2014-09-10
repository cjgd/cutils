/* mandelbrot.c -- text representation of mandelbrot fractal */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 980513 */

/* res 79x24 */

#include <stdio.h>
#include <stdlib.h>

#define FULL	'M'
#define EMPTY	' '
#define UPPER 	'"'
#define LOWER	'm'

int scale; 

int
f(int x, int y)
{
	int zr, zi, cr, ci, ir, i;

	zr = 0;
	zi = 0;
	cr = x * scale / 25;
	ci = y * scale / 20;

	for (i = 0; i < 100; i++) {

		ir = zr * zr / scale - zi * zi / scale;
		zi = zr * zi / scale + zi * zr / scale + ci;
		zr = ir + cr;
		if (zi > 2 * scale || zr > 2 * scale || 
		    zi < -2 * scale || zr < -2 * scale)
			return 1;
	}
	return 0;
}

int
main(int argc, char *argv[])
{
	int x, y, r;
	char line[80];
	int even;

	if (argc > 2) {
		printf("usage: %s [scale]\n", argv[0]); 
		exit(1);
	}
	if (argc == 2)
		scale = atoi(argv[1]); 

	if (scale == 0)
		scale = 100; 

	even = 0;
	for (y = -24; y < 25; y++) {
		for (x = -59; x < 20; x++) {
			r = f(x, y);
			if (even)
				line[x + 59] = line[x + 59] ?
				    r ? FULL : UPPER :
				    r ? LOWER : EMPTY;
			else
				line[x + 59] = r;
		}
		line[79] = 0;
		if (even)
			puts(line);
		even = !even;
	}
	return 0; 
}
