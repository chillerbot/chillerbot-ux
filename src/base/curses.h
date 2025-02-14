// This file can be included several times.

#define NCURSES_WIDECHAR 1
#if __has_include(<ncursesw/ncurses.h>)
#include <ncursesw/ncurses.h>
#elif __has_include(<cursesw.h>)
#include <cursesw.h>
#elif __has_include(<ncurses.h>)
#include <ncurses.h>
#else
#include <curses.h>
#endif /* HAS_CURSES */

// https://github.com/ddnet/ddnet/pull/9248#issuecomment-2496038033
#undef COLORS

// /home/chiller/Desktop/git/chillerbot-ux/src/game/client/gameclient.cpp:4204:22: error: expected unqualified-id
//  4204 |         case EMapBugUpdate::OK:
//       |                             ^
// /usr/include/curses.h:425:17: note: expanded from macro 'OK'
//   425 | #define OK      (0)
//       |                 ^
// 1 error generated.
#undef OK
