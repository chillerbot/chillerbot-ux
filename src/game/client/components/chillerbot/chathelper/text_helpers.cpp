// chillerbot-ux reply to ping

#include <engine/client/notifications.h>
#include <engine/config.h>
#include <engine/console.h>
#include <engine/graphics.h>
#include <engine/keys.h>
#include <engine/shared/protocol.h>
#include <engine/textrender.h>
#include <game/client/animstate.h>
#include <game/client/components/camera.h>
#include <game/client/components/chat.h>
#include <game/client/components/chillerbot/chathelper.h>
#include <game/client/components/chillerbot/version.h>
#include <game/client/components/controls.h>
#include <game/client/components/menus.h>
#include <game/client/components/voting.h>
#include <game/client/gameclient.h>
#include <game/client/prediction/entities/character.h>
#include <game/client/race.h>
#include <game/client/render.h>
#include <game/version.h>
#include <generated/client_data.h>
#include <generated/protocol.h>

#include "replytoping.h"

int CReplyToPing::GetSuffixLen(const char *pStr, const char *pSuffix)
{
	if(str_endswith(pStr, pSuffix))
		return str_length(pSuffix);
	return 0;
}

void CReplyToPing::StripSpacesAndPunctuationAndOwnName(const char *pStr, char *pStripped, int SizeOfStripped)
{
	if(!pStripped)
		return;
	dbg_assert(SizeOfStripped < 512, "too big to strip");
	if(pStr == pStripped)
	{
		pStripped[0] = '\0';
		return;
	}
	if(!pStr)
	{
		pStripped[0] = '\0';
		return;
	}
	if(!pStripped)
	{
		pStripped[0] = '\0';
		return;
	}
	if(SizeOfStripped < 1)
	{
		pStripped[0] = '\0';
		return;
	}
	char aBuf[512];
	str_copy(aBuf, pStr, sizeof(aBuf));
	char aPuncts[][4] = {"?", "!", ".", ",", "¿", " "};
	for(int i = 0; i < 10; i++) // strip up to 10 alternating punctuations
		for(auto const &Punc : aPuncts)
			while(str_endswith(aBuf, Punc)) // cut off punctuation and spaces
				aBuf[str_length(aBuf) - str_length(Punc)] = '\0';
	int Offset = 0;
	const char *pName = ChatHelper()->PlayerName();
	const char *pDummyName = ChatHelper()->DummyName();
	char aName[128];
	if(pName[0])
	{
		str_format(aName, sizeof(aName), "%s: ", pName);
		if(!Offset && str_startswith(pStr, aName))
			Offset = str_length(aName);
		str_format(aName, sizeof(aName), "%s ", pName);
		if(!Offset && str_startswith(pStr, aName))
			Offset = str_length(aName);
		str_format(aName, sizeof(aName), "%s", pName);
		if(!Offset && str_startswith(pStr, aName))
			Offset = str_length(aName);
	}
	if(pDummyName[0])
	{
		str_format(aName, sizeof(aName), "%s: ", pDummyName);
		if(!Offset && str_startswith(pStr, aName))
			Offset = str_length(aName);
		str_format(aName, sizeof(aName), "%s ", pDummyName);
		if(!Offset && str_startswith(pStr, aName))
			Offset = str_length(aName);
		str_format(aName, sizeof(aName), "%s", pDummyName);
		if(!Offset && str_startswith(pStr, aName))
			Offset = str_length(aName);
	}
	if(Offset >= str_length(aBuf))
		pStripped[0] = '\0';
	else
		str_copy(pStripped, aBuf + Offset, SizeOfStripped);
}

bool CReplyToPing::IsEmptyStr(const char *pStr)
{
	if(!pStr)
		return true;
	for(int i = 0; pStr[i] != '\0'; i++)
		if(pStr[i] != ' ' && pStr[i] != 0x7)
			return false;
	return true;
}
