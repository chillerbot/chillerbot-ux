#if defined(CONF_CURSES_CLIENT)

#include <ncursesw/ncurses.h>

#include "ccurses.h"

WINDOW *ccurses_stdscr()
{
	return stdscr;
}

#endif
