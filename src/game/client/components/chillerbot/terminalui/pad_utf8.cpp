#if defined(CONF_CURSES_CLIENT)

#include "pad_utf8.h"

#include <base/dbg.h>
#include <base/str.h>

#include <engine/external/uwidth/uwidth.h>

int str_to_uwidth_codepoints(const char *pStr, Uwidth_Code_Point *pOutArr, size_t OutArrMaxElements)
{
	const char *pSeek = pStr;
	int ElementIdx = 0;
	while(*pSeek)
	{
		int Codepoint = str_utf8_decode(&pSeek);
		if(Codepoint == -1)
		{
			return -1;
		}
		if(ElementIdx >= (int)OutArrMaxElements)
		{
			return -2;
		}
		pOutArr[ElementIdx++] = Codepoint;
	}
	return ElementIdx;
}

// from the docs
// https://github.com/telesvar/uwidth/tree/97b94a4b5945d7d19695c0576018ec0321501fee#using-the-library
static unsigned long measure(const Uwidth_Code_Point *text, unsigned int length)
{
	Uwidth_State state;
	Uwidth_Event event;
	unsigned long width = 0UL;
	unsigned int index;

	// TODO: calling init here for every string seems slow
	//       we should keep only one instance of this! and init it once!

	uwidth_init(&state, uwidth_profile_narrow);
	for(index = 0U; index < length; ++index)
	{
		if(uwidth_push(&state, text[index], &event) == uwidth_event_cluster)
		{
			width += event.width;
		}
	}
	if(uwidth_finish(&state, &event) == uwidth_event_cluster)
	{
		width += event.width;
	}
	return width;
}

int str_terminal_width(const char *pText)
{
	Uwidth_Code_Point aPoints[2048];
	int Length = str_to_uwidth_codepoints(pText, aPoints, sizeof(aPoints) / sizeof(aPoints[0]));
	if(Length < 0)
	{
		dbg_assert_failed("got error while trying to get width err=%d", Length);
	}
	return measure(aPoints, Length);
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
