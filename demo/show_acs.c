/* show_acs.c -- show ACS chars of ncurses */

/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 960727/990321 */

/* compile: cc show_acs.c -lncurses */

#include <curses.h>

int
main()
{
	initscr();
#if 0

taken from ncurses headers... 

#define ACS_ULCORNER    (acs_map['l'])  /* upper left corner */
#define ACS_LLCORNER    (acs_map['m'])  /* lower left corner */
#define ACS_URCORNER    (acs_map['k'])  /* upper right corner */
#define ACS_LRCORNER    (acs_map['j'])  /* lower right corner */
#define ACS_LTEE        (acs_map['t'])  /* tee pointing right */
#define ACS_RTEE        (acs_map['u'])  /* tee pointing left */
#define ACS_BTEE        (acs_map['v'])  /* tee pointing up */
#define ACS_TTEE        (acs_map['w'])  /* tee pointing down */
#define ACS_HLINE       (acs_map['q'])  /* horizontal line */
#define ACS_VLINE       (acs_map['x'])  /* vertical line */
#define ACS_PLUS        (acs_map['n'])  /* large plus or crossover */
#define ACS_S1          (acs_map['o'])  /* scan line 1 */
#define ACS_S9          (acs_map['s'])  /* scan line 9 */
#define ACS_DIAMOND     (acs_map['`'])  /* diamond */
#define ACS_CKBOARD     (acs_map['a'])  /* checker board (stipple) */
#define ACS_DEGREE      (acs_map['f'])  /* degree symbol */
#define ACS_PLMINUS     (acs_map['g'])  /* plus/minus */
#define ACS_BULLET      (acs_map['~'])  /* bullet */
#define ACS_LARROW      (acs_map[','])  /* arrow pointing left */
#define ACS_RARROW      (acs_map['+'])  /* arrow pointing right */
#define ACS_DARROW      (acs_map['.'])  /* arrow pointing down */
#define ACS_UARROW      (acs_map['-'])  /* arrow pointing up */
#define ACS_BOARD       (acs_map['h'])  /* board of squares */
#define ACS_LANTERN     (acs_map['I'])  /* lantern symbol */
#define ACS_BLOCK       (acs_map['0'])  /* solid square block */

to display above list, yank the following into a vi buffer, and
execute the buffer: "xyy@x

?^#if 0/^#defmm}k"wy'm/^#endo"wpmm}k:'m,.s/#define \(ACS_[A-Z0-9]*\) *.*/	addstr("\1 "); addch(\1); addch('\\n'); /

#endif

	addstr("ACS_ULCORNER "); addch(ACS_ULCORNER); addch('\n'); 
	addstr("ACS_LLCORNER "); addch(ACS_LLCORNER); addch('\n'); 
	addstr("ACS_URCORNER "); addch(ACS_URCORNER); addch('\n'); 
	addstr("ACS_LRCORNER "); addch(ACS_LRCORNER); addch('\n'); 
	addstr("ACS_LTEE "); addch(ACS_LTEE); addch('\n'); 
	addstr("ACS_RTEE "); addch(ACS_RTEE); addch('\n'); 
	addstr("ACS_BTEE "); addch(ACS_BTEE); addch('\n'); 
	addstr("ACS_TTEE "); addch(ACS_TTEE); addch('\n'); 
	addstr("ACS_HLINE "); addch(ACS_HLINE); addch('\n'); 
	addstr("ACS_VLINE "); addch(ACS_VLINE); addch('\n'); 
	addstr("ACS_PLUS "); addch(ACS_PLUS); addch('\n'); 
	addstr("ACS_S1 "); addch(ACS_S1); addch('\n'); 
	addstr("ACS_S9 "); addch(ACS_S9); addch('\n'); 
	addstr("ACS_DIAMOND "); addch(ACS_DIAMOND); addch('\n'); 
	addstr("ACS_CKBOARD "); addch(ACS_CKBOARD); addch('\n'); 
	addstr("ACS_DEGREE "); addch(ACS_DEGREE); addch('\n'); 
	addstr("ACS_PLMINUS "); addch(ACS_PLMINUS); addch('\n'); 
	addstr("ACS_BULLET "); addch(ACS_BULLET); addch('\n'); 
	addstr("ACS_LARROW "); addch(ACS_LARROW); addch('\n'); 
	addstr("ACS_RARROW "); addch(ACS_RARROW); addch('\n'); 
	addstr("ACS_DARROW "); addch(ACS_DARROW); addch('\n'); 
	addstr("ACS_UARROW "); addch(ACS_UARROW); addch('\n'); 
	addstr("ACS_BOARD "); addch(ACS_BOARD); addch('\n'); 
	addstr("ACS_LANTERN "); addch(ACS_LANTERN); addch('\n'); 
	addstr("ACS_BLOCK "); addch(ACS_BLOCK); addch('\n'); 

	refresh(); 
	endwin();
	exit(0);
	return(0);
}
