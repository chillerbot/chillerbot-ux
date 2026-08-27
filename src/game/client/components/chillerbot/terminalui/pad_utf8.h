
#ifndef GAME_CLIENT_COMPONENTS_CHILLERBOT_TERMINALUI_PAD_UTF8_H
#define GAME_CLIENT_COMPONENTS_CHILLERBOT_TERMINALUI_PAD_UTF8_H

#if defined(CONF_CURSES_CLIENT)

// Get the amount of terminal columns this string will take up.
// It supports multi byte utf-8 characters and also wide characters
int str_terminal_width(const char *pText);

/*
    Function: str_pad_right_utf8

    Pad string with spaces supporting more than ascii
    Supporting also CJK full width characters

    using the rust crate unicode-width under the hood
*/
void str_pad_right_utf8(char *pStr, int size, int pad_len);

#endif

#endif
