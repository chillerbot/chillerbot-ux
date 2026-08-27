#if defined(CONF_CURSES_CLIENT)

#include "pad_utf8.h"

#include <base/str.h>

int str_terminal_width(const char *pText)
{
	// YES THIS IS WRONG XD
	return str_length(pText);
}

void str_pad_right_utf8(char *pStr, int size, int pad_len)
{
	char aBuf[2048];
	str_copy(aBuf, pStr, sizeof(aBuf));
	int full_width_length = str_terminal_width(pStr);
	int c_len = str_length(pStr);
	int pad_len_utf8_rust = pad_len - (full_width_length - c_len);

	str_format(pStr, size, "%-*s", pad_len_utf8_rust, aBuf);
	// dbg_msg(
	// 	"pad",
	// 	"pad_len=%d pad_len_utf8_rust=%d res='%s'",
	// 	pad_len, pad_len_utf8_rust, pStr);
}

#endif
