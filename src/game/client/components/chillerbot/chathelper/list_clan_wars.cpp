// chillerbot-ux reply to ping

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

bool CReplyToPing::ListClanWars()
{
	if((str_find_nocase(m_pMessage, "clan") || str_find_nocase(m_pMessage, "klan")) &&
		(LangParser::IsQuestionWhoWhichWhat(m_pMessage) || str_find(m_pMessage, "?")))
	{
		if(str_find_nocase(m_pMessage, "war") || str_find_nocase(m_pMessage, "enemy") || str_find_nocase(m_pMessage, "kill") || str_find_nocase(m_pMessage, "against") || str_find_nocase(m_pMessage, "bad"))
		{
			char aClans[256];
			GameClient()->m_WarList.GetWarClansStr(aClans, sizeof(aClans));
			if(aClans[0])
				str_format(m_pResponse, m_SizeOfResponse, "%s I war those clans: %s", m_pMessageAuthor, aClans);
			else
				str_format(m_pResponse, m_SizeOfResponse, "%s I currently do not war any clans.", m_pMessageAuthor);
			return true;
		}
	}
	return false;
}
