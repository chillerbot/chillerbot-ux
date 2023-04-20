#include <base/system.h>

#include "pad_utf8.h"

void str_pad_right_utf8(char *pStr, int size, int pad_len)
{
	char aBuf[2048];
	str_copy(aBuf, pStr, sizeof(aBuf));
	int ByteSize;
	int LetterCount;
	str_utf8_stats(pStr, sizeof(aBuf), sizeof(aBuf), &ByteSize, &LetterCount);
	int pad_len_utf8 = pad_len + (ByteSize - LetterCount);
	str_format(pStr, size, "%-*s", pad_len_utf8, aBuf);
	// dbg_msg(
	// 	"pad",
	// 	"pad_len=%d pad_len_utf8=%d ByteSize=%d LetterCount=%d res='%s'",
	// 	pad_len, pad_len_utf8, ByteSize, LetterCount, pStr);
}
