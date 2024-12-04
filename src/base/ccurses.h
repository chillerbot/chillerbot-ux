#ifndef BASE_CCURSES_H
#define BASE_CCURSES_H

// ChillerDragon's abstraction wraper on ncurses
// ccurses
// prefixes all curses methods with ccurses_

#if defined(CONF_CURSES_CLIENT)

#include <base/curses.h>
#include <base/math.h>

WINDOW *ccurses_stdscr();

#endif

#endif
