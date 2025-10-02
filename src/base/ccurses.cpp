#if defined(CONF_CURSES_CLIENT)

#include "ccurses.h"

#include <base/curses.h>

WINDOW *ccurses_stdscr()
{
	return stdscr;
}

#endif
