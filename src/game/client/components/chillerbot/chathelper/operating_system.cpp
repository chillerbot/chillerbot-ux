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

bool CReplyToPing::WhatOs()
{
	if((str_find_nocase(m_pMessage, "?") || LangParser::IsQuestionWhoWhichWhat(m_pMessage) || str_find_nocase(m_pMessage, "do you") || str_find_nocase(m_pMessage, "u use") || str_find_nocase(m_pMessage, "u on")) &&
		(str_find_nocase(m_pMessage, "linux") || str_find_nocase(m_pMessage, "linus") || str_find_nocase(m_pMessage, "unix") || str_find_nocase(m_pMessage, "solaris") ||
			str_find_nocase(m_pMessage, "freebsd") || str_find_nocase(m_pMessage, "openbsd") || str_find_nocase(m_pMessage, "netbsd") || str_find_nocase(m_pMessage, "templeos") ||
			str_find_nocase(m_pMessage, "serenity") || str_find_nocase(m_pMessage, "haiku") ||
			str_find_nocase(m_pMessage, "windows") || str_find_nocase(m_pMessage, "mac") || str_find_nocase(m_pMessage, "apple") ||
			str_find_nocase(m_pMessage, "microsoft") || str_find_nocase(m_pMessage, "android") || str_find_nocase(m_pMessage, "ios") ||
			str_find_nocase(m_pMessage, "ubuntu") || str_find_nocase(m_pMessage, "debian") || str_find_nocase(m_pMessage, "kali") || str_find_nocase(m_pMessage, "popos") ||
			str_find_nocase(m_pMessage, "manjaro") || str_find_nocase(m_pMessage, "arch") || str_find_nocase(m_pMessage, "gentoo") || str_find_nocase(m_pMessage, "opensuse")))
	{
#if defined(CONF_FAMILY_WINDOWS)
		str_format(m_pResponse, m_SizeOfResponse, "%s I currently use windows.", m_pMessageAuthor);
#elif defined(CONF_PLATFORM_LINUX)
		str_format(m_pResponse, m_SizeOfResponse, "%s I currently use linux.", m_pMessageAuthor);
#elif defined(CONF_PLATFORM_MACOS)
		str_format(m_pResponse, m_SizeOfResponse, "%s I currently use macOS.", m_pMessageAuthor);
#elif defined(CONF_PLATFORM_ANDROID)
		str_format(m_pResponse, m_SizeOfResponse, "%s I currently use android.", m_pMessageAuthor);
#elif defined(CONF_PLATFORM_OPENBSD)
		str_format(m_pResponse, m_SizeOfResponse, "%s I currently use openBSD.", m_pMessageAuthor);
#elif defined(CONF_PLATFORM_FREEBSD)
		str_format(m_pResponse, m_SizeOfResponse, "%s I currently use freeBSD.", m_pMessageAuthor);
#elif defined(CONF_PLATFORM_NETBSD)
		str_format(m_pResponse, m_SizeOfResponse, "%s I currently use netBSD.", m_pMessageAuthor);
#elif defined(CONF_PLATFORM_HAIKU)
		str_format(m_pResponse, m_SizeOfResponse, "%s I currently use Haiku.", m_pMessageAuthor);
#else
		str_format(m_pResponse, m_SizeOfResponse, "%s No idea which operating system I use.", m_pMessageAuthor);
#endif
		return true;
	}
	return false;
}
