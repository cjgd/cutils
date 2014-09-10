map IF :r /cdua/templates/c-args
so /cdua/vi-lib/c.bg
so /cdua/vi-lib/c.author
map K I * j
map q 0i * j
map  0f*Xxx

map »/ I/* A */j
map »& ^xxx$xxxj

abbr STRH #include <string.h>
abbr STDI #include <stdio.h>
abbr STDL #include <stdlib.h>
abbr CTYP #include <ctype.h>
abbr STDD #include <stddef.h>

map! «f mx1G/(.*)[ 	]*;Y`xp:s/^[ 	]*\*[ 	]*\(.*\);.*$/\1/o{}O	
map! «m intmain(int argc, char *argv[]){	return 0;}kO

abbr uchar unsigned char
abbr ushort unsigned short
abbr uint unsigned int
abbr ulong unsigned long
abbr reg register
abbr FALL /* FALLTHROUGH */
abbr FALLT /* FALLTHROUGH */

