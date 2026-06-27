// chillerbot-ux reply to ping

#include "external/chillerbot_reply/include/chillerbot_reply/text_helper.h"
#include "replytoping.h"

#include <engine/client/notifications.h>
#include <engine/config.h>
#include <engine/console.h>
#include <engine/graphics.h>
#include <engine/keys.h>
#include <engine/shared/protocol.h>
#include <engine/textrender.h>

#include <generated/client_data.h>
#include <generated/protocol.h>

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

#include <external/chillerbot_reply/include/chillerbot_reply/langparser.h>

/*
	Wide range check
	Only answer if on warlist
*/
bool CReplyToPing::WhyDoYouKillMe(int NameLen, int MsgLen)
{
	if(MsgLen - NameLen < 10 && str_find_nocase(m_pMessage, "got dj?"))
		return false;
	char aStripped[128];
	const char *pName = ChatHelper()->PlayerName();
	const char *pDummyName = ChatHelper()->DummyName();
	TextHelper::StripSpacesAndPunctuationAndOwnName(m_pMessage, pName, pDummyName, aStripped, sizeof(aStripped));
	if(LangParser::IsQuestionWhy(m_pMessage) || (str_find_nocase(m_pMessage, "?") && str_length(aStripped) < NameLen + 4) ||
		((str_find_nocase(aStripped, "what") || str_find_nocase(aStripped, "wat") || str_find_nocase(aStripped, "warum") || str_find_nocase(aStripped, "why") || str_find_nocase(aStripped, "waht")) && str_length(aStripped) < 8) ||
		((str_find_nocase(aStripped, "what") || str_find_nocase(aStripped, "wat") || str_find_nocase(aStripped, "was") || str_find_nocase(aStripped, "waht")) && str_find_nocase(aStripped, "problem")) ||
		((str_find_nocase(m_pMessage, "stop") || str_find_nocase(m_pMessage, "help")) && (GameClient()->m_WarList.IsWarlist(m_pMessageAuthor) || GameClient()->m_WarList.IsTraitorlist(m_pMessageAuthor))))
	{
		char aWarReason[128];
		if(GameClient()->m_WarList.IsWarlist(m_pMessageAuthor) || GameClient()->m_WarList.IsTraitorlist(m_pMessageAuthor))
		{
			GameClient()->m_WarList.GetWarReason(m_pMessageAuthor, aWarReason, sizeof(aWarReason));
			if(aWarReason[0])
				str_format(m_pResponse, m_SizeOfResponse, "%s has war because: %s", m_pMessageAuthor, aWarReason);
			else
				str_format(m_pResponse, m_SizeOfResponse, "%s you are on my warlist.", m_pMessageAuthor);
			return true;
		}
		else if(GameClient()->m_WarList.IsWarClanlist(m_pMessageAuthorClan))
		{
			str_format(m_pResponse, m_SizeOfResponse, "%s your clan is on my warlist.", m_pMessageAuthor);
			return true;
		}
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			auto &Client = GameClient()->m_aClients[i];
			if(!Client.m_Active)
				continue;
			if(str_comp(Client.m_aName, m_pMessageAuthor))
				continue;

			if(GameClient()->m_WarList.IsWarClanmate(i))
			{
				str_format(m_pResponse, m_SizeOfResponse, "%s i might kill you because i war member of your clan", m_pMessageAuthor);
				return true;
			}
		}
	}
	return false;
}
