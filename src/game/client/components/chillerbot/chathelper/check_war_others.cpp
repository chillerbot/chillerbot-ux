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
#include <external/chillerbot_reply/include/chillerbot_reply/text_helper.h>

// WARNING: DO NOT EDIT THIS METHOD THERE IS A COPY OF THIS IN reply lib
// FIXME: delete this as soon as we moved all callsites to reply lib
bool CReplyToPing::WhyWar(const char *pVictim, bool IsCheck)
{
	if(!pVictim)
		return false;

	if(!str_comp(pVictim, "me"))
		pVictim = m_pMessageAuthor;
	bool HasWar = true;
	// aVictim also has to hold the full own name to match the chop off
	char aVictim[MAX_NAME_LENGTH + 3 + MAX_NAME_LENGTH];
	str_copy(aVictim, pVictim, sizeof(aVictim));
	if(!GameClient()->m_WarList.IsWarlist(aVictim) && !GameClient()->m_WarList.IsTraitorlist(aVictim))
	{
		HasWar = false;
		while(str_endswith(aVictim, "?")) // cut off the question marks from the victim name
			aVictim[str_length(aVictim) - 1] = '\0';
		while(str_endswith(aVictim, " ")) // cut off spaces from victim name 'why war foo ?' -> 'foo ?' -> 'foo ' -> 'foo'
			aVictim[str_length(aVictim) - 1] = '\0';
		// cut off own name from the victime name if question in this format "why do you war foo (your name)"
		char aOwnName[MAX_NAME_LENGTH + 3];
		// main tee
		str_format(aOwnName, sizeof(aOwnName), " %s", GameClient()->m_aClients[GameClient()->m_aLocalIds[0]].m_aName);
		if(str_endswith_nocase(aVictim, aOwnName))
			aVictim[str_length(aVictim) - str_length(aOwnName)] = '\0';
		if(GameClient()->Client()->DummyConnected())
		{
			str_format(aOwnName, sizeof(aOwnName), " %s", GameClient()->m_aClients[GameClient()->m_aLocalIds[1]].m_aName);
			if(str_endswith_nocase(aVictim, aOwnName))
				aVictim[str_length(aVictim) - str_length(aOwnName)] = '\0';
		}

		// cut off descriptions like this
		// why do you block foo he is new here!
		// why do you block foo she is my friend!!
		for(int i = 0; i < str_length(aVictim); i++)
		{
			// c++ be like...
			if(i < 2)
				continue;
			if(aVictim[i - 1] != ' ')
				continue;
			if((aVictim[i] != 'h' || !aVictim[i + 1] || aVictim[i + 1] != 'e' || !aVictim[i + 2] || aVictim[i + 2] != ' ') &&
				(aVictim[i] != 's' || !aVictim[i + 1] || aVictim[i + 1] != 'h' || !aVictim[i + 2] || aVictim[i + 2] != 'e' || !aVictim[i + 3] || aVictim[i + 3] != ' '))
				continue;

			aVictim[i - 1] = '\0';
			break;
		}

		// do not kill my friend foo
		const char *pFriend = nullptr;
		if((pFriend = str_find_nocase(aVictim, " friend ")))
			pFriend += str_length(" friend ");
		else if((pFriend = str_find_nocase(aVictim, " frint ")))
			pFriend += str_length(" frint ");
		else if((pFriend = str_find_nocase(aVictim, " mate ")))
			pFriend += str_length(" mate ");
		else if((pFriend = str_find_nocase(aVictim, " bff ")))
			pFriend += str_length(" bff ");
		else if((pFriend = str_find_nocase(aVictim, " girlfriend ")))
			pFriend += str_length(" girlfriend ");
		else if((pFriend = str_find_nocase(aVictim, " boyfriend ")))
			pFriend += str_length(" boyfriend ");
		else if((pFriend = str_find_nocase(aVictim, " dog ")))
			pFriend += str_length(" dog ");
		else if((pFriend = str_find_nocase(aVictim, " gf ")))
			pFriend += str_length(" gf ");
		else if((pFriend = str_find_nocase(aVictim, " bf ")))
			pFriend += str_length(" bf ");

		if(pFriend)
			str_copy(aVictim, pFriend, sizeof(aVictim));
	}

	if(!str_comp(aVictim, "me"))
		str_copy(aVictim, m_pMessageAuthor, sizeof(aVictim));

	char aWarReason[128];
	if(HasWar || GameClient()->m_WarList.IsWarlist(aVictim) || GameClient()->m_WarList.IsTraitorlist(aVictim))
	{
		GameClient()->m_WarList.GetWarReason(aVictim, aWarReason, sizeof(aWarReason));
		if(aWarReason[0])
			str_format(m_pResponse, m_SizeOfResponse, "%s: %s has war because: %s", m_pMessageAuthor, aVictim, aWarReason);
		else
			str_format(m_pResponse, m_SizeOfResponse, "%s: the name %s is on my warlist.", m_pMessageAuthor, aVictim);
		return true;
	}
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		auto &Client = GameClient()->m_aClients[i];
		if(!Client.m_Active)
			continue;
		if(str_comp(Client.m_aName, aVictim))
			continue;

		if(GameClient()->m_WarList.IsWarClanlist(Client.m_aClan))
		{
			str_format(m_pResponse, m_SizeOfResponse, "%s i war %s because his clan %s is on my warlist.", m_pMessageAuthor, aVictim, Client.m_aClan);
			return true;
		}
		if(GameClient()->m_WarList.IsWarClanmate(i))
		{
			str_format(m_pResponse, m_SizeOfResponse, "%s i might kill %s because I war member from his clan %s", m_pMessageAuthor, aVictim, Client.m_aClan);
			return true;
		}
	}
	if(IsCheck && str_comp_nocase(aVictim, "me"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s: '%s' is not on my warlist.", m_pMessageAuthor, aVictim);
		return true;
	}
	return false;
}

// FIXME: delete this there is a copy in the lib
int CReplyToPing::IsWarCheckSuffix(const char *pStr)
{
	const char aaPrefixes[][16] = {" in", " on", " at", " is"};
	const char aaYous[][16] = {" u", " you", " your", " ur", " deiner", " deinen", " ju", " ti", " jour", " jur", " tu"};
	const char aaWarlists[][64] = {
		" warlist",
		" war list",
		" friendlist",
		" friend list",
		" team list",
		" peace list",
		" enemy list",
		" enemies",
		" enemys",
		" frint list",
		" frintlist",
		" frentlist",
		" frent list",
		" frenlist",
		" fren list",
		" friends",
		" frints",
		" frintss",
		" frents",
		" friend",
		" frint",
		" frent",
		" fren",
		" good",
		" war",
		" kill",
		" bad"};
	char aOnYourWarlist[128];
	int ChopEnding = 0;
	for(const auto &aPrefix : aaPrefixes)
	{
		for(const auto &aWarlist : aaWarlists)
		{
			str_format(aOnYourWarlist, sizeof(aOnYourWarlist), "%s%s", aPrefix, aWarlist);
			ChopEnding = TextHelper::GetSuffixLen(pStr, aOnYourWarlist);
			if(ChopEnding)
				return ChopEnding;
			for(const auto &aYou : aaYous)
			{
				str_format(aOnYourWarlist, sizeof(aOnYourWarlist), "%s%s%s", aPrefix, aYou, aWarlist);
				if((ChopEnding = TextHelper::GetSuffixLen(pStr, aOnYourWarlist)))
					return ChopEnding;
			}
		}
	}
	return ChopEnding;
}
