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

bool CReplyToPing::SmallTalk(int NameLen, int MsgLen)
{
	// small talk
	if(str_find_nocase(m_pMessage, "how are you") ||
		str_find_nocase(m_pMessage, "how r u") ||
		str_find_nocase(m_pMessage, "how ru ") ||
		str_find_nocase(m_pMessage, "how ru?") ||
		str_find_nocase(m_pMessage, "how r you") ||
		str_find_nocase(m_pMessage, "how are u") ||
		str_find_nocase(m_pMessage, "how is it going") ||
		str_find_nocase(m_pMessage, "ca va") ||
		(str_find_nocase(m_pMessage, "как") && str_find_nocase(m_pMessage, "дела")))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s good, and you? :)", m_pMessageAuthor);
		return true;
	}
	if(str_find_nocase(m_pMessage, "wie gehts") || str_find_nocase(m_pMessage, "wie geht es") || str_find_nocase(m_pMessage, "was geht"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s gut, und dir? :)", m_pMessageAuthor);
		return true;
	}
	if(str_find_nocase(m_pMessage, "ca va") ||
		str_find_nocase(m_pMessage, "Ça va") ||
		str_find_nocase(m_pMessage, "ça va") ||
		(str_find_nocase(m_pMessage, "cv") && ((MsgLen - NameLen) < 6)) ||
		(str_find_nocase(m_pMessage, "cv ") && ((MsgLen - NameLen) < 14)) ||
		(str_find_nocase(m_pMessage, "cv?") && ((MsgLen - NameLen) < 14)) ||
		(str_find_nocase(m_pMessage, "çv ") && ((MsgLen - NameLen) < 14)) ||
		(str_find_nocase(m_pMessage, "çv?") && ((MsgLen - NameLen) < 14)))
	{
		str_format(m_pResponse, m_SizeOfResponse, "je vais bien, et toi %s ?", m_pMessageAuthor);
		return true;
	}
	if(str_find_nocase(m_pMessage, "et toi"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s moi aussi, merci", m_pMessageAuthor);
		return true;
	}
	if(str_find_nocase(m_pMessage, "about you") ||
		str_find_nocase(m_pMessage, " and you") ||
		str_startswith(m_pMessage, "and you") ||
		str_find_nocase(m_pMessage, " and u") ||
		str_startswith(m_pMessage, "and u") ||
		(str_find_nocase(m_pMessage, "u?") && MsgLen < NameLen + 5) ||
		(str_find_nocase(m_pMessage, "wbu") && MsgLen < NameLen + 8) ||
		(str_find_nocase(m_pMessage, "hbu") && MsgLen < NameLen + 8))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s good", m_pMessageAuthor);
		return true;
	}
	return false;
}
