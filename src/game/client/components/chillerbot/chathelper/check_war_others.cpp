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
			ChopEnding = GetSuffixLen(pStr, aOnYourWarlist);
			if(ChopEnding)
				return ChopEnding;
			for(const auto &aYou : aaYous)
			{
				str_format(aOnYourWarlist, sizeof(aOnYourWarlist), "%s%s%s", aPrefix, aYou, aWarlist);
				if((ChopEnding = GetSuffixLen(pStr, aOnYourWarlist)))
					return ChopEnding;
			}
		}
	}
	return ChopEnding;
}

bool CReplyToPing::IsWarName()
{
	// check if a player has war or not
	const char *pDoYou = nullptr;
	if(!pDoYou && (pDoYou = str_find_nocase(m_pMessage, "you war ")))
		pDoYou = pDoYou + str_length("you war ");
	if(!pDoYou && (pDoYou = str_find_nocase(m_pMessage, "you in war with ")))
		pDoYou = pDoYou + str_length("you in war with ");
	// "hast du war mit"
	// "hast du eig war mit"
	// "hast du eigentlich war mit"
	// "hast du überhaupt war mit"
	// "hast du einen war mit"
	if(!pDoYou && (pDoYou = LangParser::StrFindOrder(m_pMessage, 2, "hast du ", "war mit ")))
		pDoYou = pDoYou + str_length("war mit ");
	if(pDoYou)
		if(WhyWar(pDoYou, true))
			return true;

	// check war reason for others
	const char *pWhy = str_find_nocase(m_pMessage, "why has ");
	if(pWhy)
		pWhy = pWhy + str_length("why has ");
	if(!pWhy)
		if((pWhy = str_find_nocase(m_pMessage, "why")))
			pWhy = pWhy + str_length("why");
	if(!pWhy)
		if((pWhy = str_find_nocase(m_pMessage, "warum hat ")))
			pWhy = pWhy + str_length("warum hat ");
	if(!pWhy)
		if((pWhy = str_find_nocase(m_pMessage, "warum")))
			pWhy = pWhy + str_length("warum");
	if(!pWhy)
		pWhy = str_find_nocase(m_pMessage, "stop");
	if(!pWhy)
		pWhy = str_find_nocase(m_pMessage, "do not");
	if(!pWhy)
		pWhy = str_find_nocase(m_pMessage, "don't");
	if(!pWhy)
		pWhy = str_find_nocase(m_pMessage, "do u");
	if(!pWhy)
		pWhy = str_find_nocase(m_pMessage, "do you");
	if(!pWhy)
		pWhy = str_find_nocase(m_pMessage, "is u");
	if(!pWhy)
		pWhy = str_find_nocase(m_pMessage, "is you");
	if(!pWhy)
	{
		pWhy = str_find_nocase(m_pMessage, "is war");
	}
	if(!pWhy)
		pWhy = str_find_nocase(m_pMessage, "are u");
	if(!pWhy)
		pWhy = str_find_nocase(m_pMessage, "are you");
	if(pWhy)
	{
		const char *pKill = nullptr;
		pKill = str_find_nocase(pWhy, "kill "); // why do you kill foo?
		if(pKill)
			pKill = pKill + str_length("kill ");
		else if((pKill = str_find_nocase(pWhy, "kil "))) // why kil foo?
			pKill = pKill + str_length("kil ");
		else if((pKill = str_find_nocase(pWhy, "killed "))) // why killed foo?
			pKill = pKill + str_length("killed ");
		else if((pKill = str_find_nocase(pWhy, "block "))) // why do you block foo?
			pKill = pKill + str_length("block ");
		else if((pKill = str_find_nocase(pWhy, "blocked "))) // why do you blocked foo?
			pKill = pKill + str_length("blocked ");
		else if((pKill = str_find_nocase(pWhy, "help "))) // why no help foo?
			pKill = pKill + str_length("help ");
		else if((pKill = str_find_nocase(pWhy, "war with "))) // why do you have war with foo?
			pKill = pKill + str_length("war with ");
		else if((pKill = str_find_nocase(pWhy, "war "))) // why war foo?
			pKill = pKill + str_length("war ");
		else if((pKill = str_find_nocase(pWhy, "wared "))) // why wared foo?
			pKill = pKill + str_length("wared ");
		else if((pKill = str_find_nocase(pWhy, "waring "))) // why waring foo?
			pKill = pKill + str_length("waring ");
		else if((pKill = str_find_nocase(pWhy, "bully "))) // why bully foo?
			pKill = pKill + str_length("bully ");
		else if((pKill = str_find_nocase(pWhy, "bullying "))) // why bullying foo?
			pKill = pKill + str_length("bullying ");
		else if((pKill = str_find_nocase(pWhy, "bullied "))) // why bullied foo?
			pKill = pKill + str_length("bullied ");
		else if((pKill = str_find_nocase(pWhy, "freeze "))) // why freeze foo?
			pKill = pKill + str_length("freeze ");
		else if((pKill = str_find_nocase(pWhy, "warlist "))) // is warlist foo?
			pKill = pKill + str_length("warlist ");
		else if((pKill = str_find_nocase(pWhy, "enemi "))) // is enemi foo?
			pKill = pKill + str_length("enemi ");
		else if((pKill = str_find_nocase(pWhy, "enemy "))) // is enemy foo?
			pKill = pKill + str_length("enemy ");

		if(pKill && WhyWar(pKill))
			return true;

		// "why foo war?"
		// chop off the "war" at the end
		char aWhy[128];
		str_copy(aWhy, pWhy, sizeof(aWhy));

		int CutOffWar = -1;
		if((CutOffWar = LangParser::StrFindIndex(aWhy, " war")) != -1)
			aWhy[CutOffWar] = '\0';
		else if((CutOffWar = LangParser::StrFindIndex(aWhy, " kill")) != -1)
			aWhy[CutOffWar] = '\0';

		// trim
		int Trim = 0;
		while(aWhy[Trim] == ' ')
			Trim++;

		if(CutOffWar != -1)
			if(WhyWar(aWhy + Trim))
				return true;

		if(pKill)
		{
			char aStripped[128];
			StripSpacesAndPunctuationAndOwnName(pKill, aStripped, sizeof(aStripped));
			if(!IsEmptyStr(aStripped))
			{
				str_format(m_pResponse, m_SizeOfResponse, "%s: '%s' is not on my warlist.", m_pMessageAuthor, aStripped);
				return true;
			}
		}
	}
	return false;
}

bool CReplyToPing::NameIsWar()
{
	// still check war for others but now different order
	// also cover "name is war?" in addition to "is war name?"
	char aStrippedMsg[256];
	StripSpacesAndPunctuationAndOwnName(m_pMessage, aStrippedMsg, sizeof(aStrippedMsg));
	int ChopEnding = IsWarCheckSuffix(aStrippedMsg);
	bool Strict = false;
	if(!ChopEnding)
	{
		Strict = true;
		ChopEnding = GetSuffixLen(aStrippedMsg, " bad");
	}
	if(!ChopEnding)
	{
		Strict = true;
		ChopEnding = GetSuffixLen(aStrippedMsg, " good");
	}
	if(!ChopEnding)
	{
		Strict = true;
		ChopEnding = GetSuffixLen(aStrippedMsg, " friend");
	}
	if(!ChopEnding)
	{
		Strict = true;
		ChopEnding = GetSuffixLen(aStrippedMsg, " frien");
	}
	if(!ChopEnding)
	{
		Strict = true;
		ChopEnding = GetSuffixLen(aStrippedMsg, " frent");
	}
	if(!ChopEnding)
	{
		Strict = true;
		ChopEnding = GetSuffixLen(aStrippedMsg, " fren");
	}
	if(!ChopEnding)
	{
		Strict = true;
		ChopEnding = GetSuffixLen(aStrippedMsg, " frint");
	}
	if(!ChopEnding)
	{
		Strict = true;
		ChopEnding = GetSuffixLen(aStrippedMsg, " enemy");
	}

	if(ChopEnding)
	{
		unsigned int Cut = str_length(aStrippedMsg) - ChopEnding;
		if(Cut > 0 && Cut < sizeof(aStrippedMsg))
		{
			aStrippedMsg[str_length(aStrippedMsg) - ChopEnding] = '\0';
			if(WhyWar(aStrippedMsg))
				return true;
			bool FoundName = false;
			if(Strict)
			{
				for(const auto &Client : GameClient()->m_aClients)
				{
					if(!Client.m_Active)
						continue;
					if(!str_comp(Client.m_aName, aStrippedMsg))
					{
						if(GameClient()->m_WarList.IsTeamlist(Client.m_aName))
						{
							str_format(m_pResponse, m_SizeOfResponse, "%s: '%s' is on my friend list.", m_pMessageAuthor, Client.m_aName);
							return true;
						}
						if(GameClient()->m_WarList.IsTeamClanlist(Client.m_aClan))
						{
							str_format(m_pResponse, m_SizeOfResponse, "%s: '%s's clan %s is on my friend list.", m_pMessageAuthor, Client.m_aName, Client.m_aClan);
							return true;
						}
						FoundName = true;
						break;
					}
				}
			}
			const char *pPrefixStripped = aStrippedMsg;
			// second search strip some prefix
			if(!FoundName)
			{
				Strict = false;
				if(str_startswith_nocase(aStrippedMsg, " is "))
				{
					pPrefixStripped = aStrippedMsg + str_length(" is ");
					if(WhyWar(pPrefixStripped))
						return true;
				}
				else if(str_startswith_nocase(aStrippedMsg, "is "))
				{
					pPrefixStripped = aStrippedMsg + str_length("is ");
					if(WhyWar(pPrefixStripped))
						return true;
				}
				else if(str_startswith_nocase(aStrippedMsg, " do you "))
				{
					pPrefixStripped = aStrippedMsg + str_length("is ");
					if(WhyWar(pPrefixStripped))
						return true;
				}
				else if(str_startswith_nocase(aStrippedMsg, "do you "))
				{
					pPrefixStripped = aStrippedMsg + str_length("is ");
					if(WhyWar(pPrefixStripped))
						return true;
				}
				else if(str_startswith_nocase(aStrippedMsg, " on "))
				{
					pPrefixStripped = aStrippedMsg + str_length("is ");
					if(WhyWar(pPrefixStripped))
						return true;
				}
				else if(str_startswith_nocase(aStrippedMsg, "on "))
				{
					pPrefixStripped = aStrippedMsg + str_length("is ");
					if(WhyWar(pPrefixStripped))
						return true;
				}
				else
					Strict = true;
			}
			if(!Strict || FoundName)
			{
				str_format(m_pResponse, m_SizeOfResponse, "%s: '%s' is not on my warlist.", m_pMessageAuthor, pPrefixStripped);
				return true;
			}
		}
	}
	return false;
}
