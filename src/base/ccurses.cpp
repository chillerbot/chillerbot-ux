#if defined(CONF_CURSES_CLIENT)

#include <base/curses.h>

#include "ccurses.h"

WINDOW *ccurses_stdscr()
{
	return stdscr;
}

#endif
