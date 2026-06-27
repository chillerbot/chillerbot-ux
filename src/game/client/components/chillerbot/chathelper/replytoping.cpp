// chillerbot-ux reply to ping

#include "replytoping.h"

#include <base/str.h>

#include <engine/client/notifications.h>
#include <engine/config.h>
#include <engine/console.h>
#include <engine/graphics.h>
#include <engine/keys.h>
#include <engine/shared/config.h>
#include <engine/shared/protocol.h>
#include <engine/textrender.h>

#include <generated/client_data.h>
#include <generated/protocol.h>

#include <game/client/animstate.h>
#include <game/client/components/camera.h>
#include <game/client/components/chat.h>
#include <game/client/components/chillerbot/chathelper.h>
#include <game/client/components/chillerbot/chillerbotux.h>
#include <game/client/components/chillerbot/version.h>
#include <game/client/components/chillerbot/warlist.h>
#include <game/client/components/controls.h>
#include <game/client/components/menus.h>
#include <game/client/components/voting.h>
#include <game/client/gameclient.h>
#include <game/client/prediction/entities/character.h>
#include <game/client/race.h>
#include <game/client/render.h>
#include <game/version.h>

#include <external/chillerbot_reply/include/chillerbot_reply/chillerbot_reply.h>
#include <external/chillerbot_reply/include/chillerbot_reply/clan.h>
#include <external/chillerbot_reply/include/chillerbot_reply/langparser.h>

CGameClient *CReplyToPing::GameClient() { return m_pChatHelper->GameClientUnprotected(); }

CReplyToPing::CReplyToPing(CChatHelper *pChatHelper, const char *pMessageAuthor, const char *pMessageAuthorClan, const char *pMessage, char *pResponse, long unsigned int SizeOfResponse)
{
	m_pChatHelper = pChatHelper;

	m_pMessageAuthor = pMessageAuthor;
	m_pMessageAuthorClan = pMessageAuthorClan;
	m_pMessage = pMessage;
	m_pResponse = pResponse;
	m_SizeOfResponse = SizeOfResponse;
}

bool CReplyToPing::Reply()
{
	if(!m_pResponse)
		return false;
	m_pResponse[0] = '\0';
	if(m_pMessageAuthor[0] == '\0')
		return false;
	if(m_pMessage[0] == '\0')
		return false;

	int MsgLen = str_length(m_pMessage);
	int NameLen = 0;
	const char *pName = GameClient()->m_aClients[GameClient()->m_aLocalIds[0]].m_aName;
	const char *pDummyName = GameClient()->m_aClients[GameClient()->m_aLocalIds[1]].m_aName;
	const char *pClan = GameClient()->m_aClients[GameClient()->m_aLocalIds[0]].m_aClan;
	const char *pDummyClan = GameClient()->m_aClients[GameClient()->m_aLocalIds[1]].m_aClan;

	if(ChatHelper()->LineShouldHighlight(m_pMessage, pName))
		NameLen = str_length(pName);
	else if(GameClient()->Client()->DummyConnected() && ChatHelper()->LineShouldHighlight(m_pMessage, pDummyName))
		NameLen = str_length(pDummyName);

	CChillerBotReplyChatMessage Message;
	Message.m_pMessage = m_pMessage;
	Message.m_Team = 0; // TODO: set this
	Message.m_pAuthor = m_pMessageAuthor;
	Message.m_pAuthorClan = m_pMessageAuthorClan;
	CChillerBotReply ReplyBot;
	ReplyBot.m_Context.m_IsDummyConnected = GameClient()->Client()->DummyConnected();
	ReplyBot.m_Context.m_ActiveTee = g_Config.m_ClDummy;
	ReplyBot.m_Context.m_aOwnTees[0].m_ClientId = GameClient()->m_aLocalIds[0];
	ReplyBot.m_Context.m_aOwnTees[0].m_pName = pName;
	ReplyBot.m_Context.m_aOwnTees[0].m_pClan = pClan;
	{
		if(g_Config.m_ClDummy == 0 && GameClient()->m_Snap.m_pLocalCharacter)
		{
			ReplyBot.m_Context.m_aOwnTees[0].m_PosX = GameClient()->m_PredictedChar.m_Pos.x;
			ReplyBot.m_Context.m_aOwnTees[0].m_PosY = GameClient()->m_PredictedChar.m_Pos.y;
		}
		else
		{
			CCharacter *pChr = GameClient()->m_GameWorld.GetCharacterById(GameClient()->m_aLocalIds[0]);
			if(pChr)
			{
				ReplyBot.m_Context.m_aOwnTees[0].m_PosX = pChr->m_Pos.x;
				ReplyBot.m_Context.m_aOwnTees[0].m_PosY = pChr->m_Pos.y;
			}
		}
	}
	ReplyBot.m_Context.m_aOwnTees[1].m_ClientId = GameClient()->m_aLocalIds[1];
	ReplyBot.m_Context.m_aOwnTees[1].m_pName = pDummyName;
	ReplyBot.m_Context.m_aOwnTees[1].m_pClan = pDummyClan;
	if(GameClient()->Client()->DummyConnected())
	{
		if(g_Config.m_ClDummy == 1 && GameClient()->m_Snap.m_pLocalCharacter)
		{
			ReplyBot.m_Context.m_aOwnTees[1].m_PosX = GameClient()->m_PredictedChar.m_Pos.x;
			ReplyBot.m_Context.m_aOwnTees[1].m_PosY = GameClient()->m_PredictedChar.m_Pos.x;
		}
		else
		{
			CCharacter *pChr = GameClient()->m_GameWorld.GetCharacterById(GameClient()->m_aLocalIds[1]);
			if(pChr)
			{
				ReplyBot.m_Context.m_aOwnTees[1].m_PosX = pChr->m_Pos.x;
				ReplyBot.m_Context.m_aOwnTees[1].m_PosY = pChr->m_Pos.y;
			}
		}
	}
	ReplyBot.m_Context.m_pUser = GameClient();
	ReplyBot.m_Context.m_pfnGetClientName = CChillerBotUX::GetClientNameCallback;
	ReplyBot.m_Context.m_pfnGetClientClan = CChillerBotUX::GetClientClanCallback;
	ReplyBot.m_Context.m_pfnGetClient = CChillerBotUX::GetClientCallback;
	ReplyBot.m_Context.m_pfnGetWarReason = CWarList::GetWarReasonCallback;
	ReplyBot.m_Context.m_pfnGetWarClansStr = CWarList::GetWarClansStrCallback;
	ReplyBot.m_Context.m_pfnIsWar = CWarList::IsWarCallback;
	ReplyBot.m_Context.m_pfnIsWarlist = CWarList::IsWarlistCallback;
	ReplyBot.m_Context.m_pfnIsTeamlist = CWarList::IsTeamlistCallback;
	ReplyBot.m_Context.m_pfnIsTraitorlist = CWarList::IsTraitorlistCallback;
	ReplyBot.m_Context.m_pfnIsWarClanlist = CWarList::IsWarClanlistCallback;
	ReplyBot.m_Context.m_pfnIsTeamClanlist = CWarList::IsTeamClanlistCallback;
	ReplyBot.m_Context.m_pfnIsWarClanmate = CWarList::IsWarClanmateCallback;
	ReplyBot.m_Context.m_pfnIsWarClanmateId = CWarList::IsWarClanmateCallback;
	ReplyBot.m_Context.m_pfnNumEnemies = CWarList::NumEnemiesCallback;
	ReplyBot.m_Context.m_pfnNumTeam = CWarList::NumTeamCallback;
	ReplyBot.m_Context.m_pfnIsStart = CRaceHelper::IsStartCallback;
	ReplyBot.m_Context.m_pfnIsFinish = CRaceHelper::IsFinishCallback;
	ReplyBot.m_Context.m_pfnIsNearFinish = CRaceHelper::IsNearFinishCallback;
	ReplyBot.m_Context.m_pfnIsNearStart = CRaceHelper::IsNearStartCallback;
	ReplyBot.m_Context.m_pfnIsClusterRangeFinish = CRaceHelper::IsClusterRangeFinishCallback;
	ReplyBot.m_Context.m_pfnIsClusterRangeStart = CRaceHelper::IsClusterRangeStartCallback;
	if(ReplyBot.Reply(&Message, m_pResponse, m_SizeOfResponse))
		return true;

	// ping without further context
	if(MsgLen < NameLen + 2)
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s ?", m_pMessageAuthor);
		return true;
	}

	// are you here?
	if((str_find_nocase(m_pMessage, "u here") || str_find_nocase(m_pMessage, "here?")) && (MsgLen < NameLen + str_length("yo brother are you here????")))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s yes I am here", m_pMessageAuthor);
		return true;
	}

	// how many jumps do you have?
	if(str_find_nocase(m_pMessage, "how") &&
		(str_find_nocase(m_pMessage, "mani") || str_find_nocase(m_pMessage, "many") || str_find_nocase(m_pMessage, "much")) &&
		(str_find_nocase(m_pMessage, "jamp") || str_find_nocase(m_pMessage, "jump") || str_find_nocase(m_pMessage, "jomp")))
	{
		int UnusedJumps = GameClient()->m_ChillerBotUX.GetUnusedJumps();
		int TotalJumps = GameClient()->m_ChillerBotUX.GetTotalJumps();
		str_format(m_pResponse, m_SizeOfResponse, "%s I currently have %d out of %d jumps", m_pMessageAuthor, UnusedJumps, TotalJumps);
		return true;
	}

	// // do you have double jump?
	if((str_find_nocase(m_pMessage, "have") || str_find_nocase(m_pMessage, "has") || str_find_nocase(m_pMessage, "got") || str_find_nocase(m_pMessage, "you") || str_find_nocase(m_pMessage, " u ")) &&
		(str_find_nocase(m_pMessage, " dj") || str_find_nocase(m_pMessage, "double") || str_find_nocase(m_pMessage, "lejump") || str_find_nocase(m_pMessage, "lejamp") || str_find_nocase(m_pMessage, "lejomp")))
	{
		int UnusedJumps = GameClient()->m_ChillerBotUX.GetUnusedJumps();
		int TotalJumps = GameClient()->m_ChillerBotUX.GetTotalJumps();
		if(UnusedJumps > 0)
			str_format(m_pResponse, m_SizeOfResponse, "%s Yes. I currently have %d out of %d jumps", m_pMessageAuthor, UnusedJumps, TotalJumps);
		else
			str_format(m_pResponse, m_SizeOfResponse, "%s No. I currently have %d out of %d jumps", m_pMessageAuthor, UnusedJumps, TotalJumps);
		return true;
	}

	// spec me
	if(str_find_nocase(m_pMessage, "spec") || str_find_nocase(m_pMessage, "watch") || (str_find_nocase(m_pMessage, "look") && !str_find_nocase(m_pMessage, "looks")) || str_find_nocase(m_pMessage, "schau"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "/pause %s", m_pMessageAuthor);
		GameClient()->m_Chat.SendChat(0, m_pResponse);
		str_format(m_pResponse, m_SizeOfResponse, "%s ok i am watching you", m_pMessageAuthor);
		return true;
	}
	// wanna? (always say no automated if motivated to do something type yes manually)
	if(str_find_nocase(m_pMessage, "wanna") || str_find_nocase(m_pMessage, "want"))
	{
		// TODO: fix tone
		// If you get asked to be given something "no sorry" sounds weird
		// If you are being asked to do something together "no thanks" sounds weird
		// the generic "no" might be a bit dry
		str_format(m_pResponse, m_SizeOfResponse, "%s no", m_pMessageAuthor);
		return true;
	}
	// help
	if(str_find_nocase(m_pMessage, "help") || str_find_nocase(m_pMessage, "hilfe"))
	{
		if(!str_find_nocase(m_pMessage, "helper"))
		{
			str_format(m_pResponse, m_SizeOfResponse, "%s where? what?", m_pMessageAuthor);
			return true;
		}
	}
	if(SmallTalk(NameLen, MsgLen))
		return true;
	// greetings
	if(LangParser::IsGreeting(m_pMessage))
	{
		str_format(m_pResponse, m_SizeOfResponse, "hi %s", m_pMessageAuthor);
		return true;
	}
	if(LangParser::IsGreetingQq(m_pMessage) && MsgLen < NameLen + 10)
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s qq", m_pMessageAuthor);
		return true;
	}
	// "Здравствуйте" => 25 bytes
	if(LangParser::IsGreetingRus(m_pMessage) && MsgLen < NameLen + 26)
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s привет", m_pMessageAuthor);
		return true;
	}
	if(LangParser::IsBye(m_pMessage))
	{
		str_format(m_pResponse, m_SizeOfResponse, "bye %s", m_pMessageAuthor);
		return true;
	}
	// chillerbot-ux features
	if(LangParser::IsQuestionHow(m_pMessage))
	{
		// feature: auto_drop_money
		if(str_find_nocase(m_pMessage, "drop") && (str_find_nocase(m_pMessage, "money") || str_find_nocase(m_pMessage, "moni") || str_find_nocase(m_pMessage, "coin") || str_find_nocase(m_pMessage, "cash") || str_find_nocase(m_pMessage, "geld")))
		{
			str_format(m_pResponse, m_SizeOfResponse, "%s I auto drop money using \"auto_drop_money\" in chillerbot-ux", m_pMessageAuthor);
			return true;
		}
		// feature: auto reply
		if((str_find_nocase(m_pMessage, "reply") && str_find_nocase(m_pMessage, "chat")) || (str_find_nocase(m_pMessage, "auto chat") || str_find_nocase(m_pMessage, "autochat")) ||
			str_find_nocase(m_pMessage, "message") ||
			((str_find_nocase(m_pMessage, "fast") || str_find_nocase(m_pMessage, "quick")) && str_find_nocase(m_pMessage, "chat")))
		{
			str_format(m_pResponse, m_SizeOfResponse, "%s I bound the chillerbot-ux command \"reply_to_last_ping\" to automate chat", m_pMessageAuthor);
			return true;
		}
	}
	// advertise chillerbot
	if(str_find_nocase(m_pMessage, "what client") || str_find_nocase(m_pMessage, "which client") || str_find_nocase(m_pMessage, "wat client") ||
		str_find_nocase(m_pMessage, "good client") || str_find_nocase(m_pMessage, "download client") || str_find_nocase(m_pMessage, "link client") || str_find_nocase(m_pMessage, "get client") ||
		str_find_nocase(m_pMessage, "where chillerbot") || str_find_nocase(m_pMessage, "download chillerbot") || str_find_nocase(m_pMessage, "link chillerbot") || str_find_nocase(m_pMessage, "get chillerbot") ||
		str_find_nocase(m_pMessage, "chillerbot url") || str_find_nocase(m_pMessage, "chillerbot download") || str_find_nocase(m_pMessage, "chillerbot link") || str_find_nocase(m_pMessage, "chillerbot website") ||
		((str_find_nocase(m_pMessage, "ddnet") || str_find_nocase(m_pMessage, "vanilla")) && str_find_nocase(m_pMessage, "?")))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s I use chillerbot-ux ( https://chillerbot.github.io )", m_pMessageAuthor);
		return true;
	}
	// whats your setting (mousesense, distance, dyn)
	if((str_find_nocase(m_pMessage, "?") ||
		   str_find_nocase(m_pMessage, "what") ||
		   str_find_nocase(m_pMessage, "which") ||
		   str_find_nocase(m_pMessage, "wat") ||
		   str_find_nocase(m_pMessage, "much") ||
		   str_find_nocase(m_pMessage, "many") ||
		   str_find_nocase(m_pMessage, "viel") ||
		   str_find_nocase(m_pMessage, "hoch")) &&
		(str_find_nocase(m_pMessage, "sens") || str_find_nocase(m_pMessage, "sesn") || str_find_nocase(m_pMessage, "snse") || str_find_nocase(m_pMessage, "senes") || str_find_nocase(m_pMessage, "inp") || str_find_nocase(m_pMessage, "speed")))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s my current inp_mousesens is %d", m_pMessageAuthor, g_Config.m_InpMousesens);
		return true;
	}
	if((str_find_nocase(m_pMessage, "?") || str_find_nocase(m_pMessage, "what") || str_find_nocase(m_pMessage, "which") || str_find_nocase(m_pMessage, "wat") || str_find_nocase(m_pMessage, "much") || str_find_nocase(m_pMessage, "many")) &&
		str_find_nocase(m_pMessage, "dist"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s my current cl_mouse_max_distance is %d", m_pMessageAuthor, g_Config.m_ClMouseMaxDistance);
		return true;
	}
	if((str_find_nocase(m_pMessage, "?") || str_find_nocase(m_pMessage, "do you") || str_find_nocase(m_pMessage, "do u")) &&
		str_find_nocase(m_pMessage, "dyn"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s my dyncam is currently %s", m_pMessageAuthor, g_Config.m_ClDyncam ? "on" : "off");
		return true;
	}
	// compliments
	if(str_find_nocase(m_pMessage, "good") ||
		str_find_nocase(m_pMessage, "happy") ||
		str_find_nocase(m_pMessage, "congrats") ||
		str_find_nocase(m_pMessage, "nice") ||
		str_find_nocase(m_pMessage, "pro ") ||
		str_find_nocase(m_pMessage, "pro!") ||
		str_endswith_nocase(m_pMessage, "pro"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s thanks", m_pMessageAuthor);
		return true;
	}
	// impatient
	if(str_find_nocase(m_pMessage, "answer") || str_find_nocase(m_pMessage, "ignore") || str_find_nocase(m_pMessage, "antwort") || str_find_nocase(m_pMessage, "ignorier"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s i am currently busy (automated reply)", m_pMessageAuthor);
		return true;
	}
	// ask to ask
	if(LangParser::IsAskToAsk(m_pMessage, m_pMessageAuthor, m_pResponse, m_SizeOfResponse))
		return true;
	// got weapon?
	if(str_find_nocase(m_pMessage, "got") || str_find_nocase(m_pMessage, "have") || str_find_nocase(m_pMessage, "hast"))
	{
		int Weapon = -1;
		if(str_find_nocase(m_pMessage, "hammer"))
			Weapon = WEAPON_HAMMER;
		else if(str_find_nocase(m_pMessage, "gun"))
			Weapon = WEAPON_GUN;
		else if(str_find_nocase(m_pMessage, "sg") || str_find_nocase(m_pMessage, "shotgun") || str_find_nocase(m_pMessage, "shotty"))
			Weapon = WEAPON_SHOTGUN;
		else if(str_find_nocase(m_pMessage, "nade") || str_find_nocase(m_pMessage, "rocket") || str_find_nocase(m_pMessage, "bazooka"))
			Weapon = WEAPON_GRENADE;
		else if(str_find_nocase(m_pMessage, "rifle") || str_find_nocase(m_pMessage, "laser") || str_find_nocase(m_pMessage, "sniper"))
			Weapon = WEAPON_LASER;
		CCharacter *pChar = GameClient()->m_GameWorld.GetCharacterById(GameClient()->m_aLocalIds[g_Config.m_ClDummy]);
		if(pChar && Weapon != -1)
		{
			char aWeapons[1024];
			aWeapons[0] = '\0';
			if(pChar->GetWeaponGot(WEAPON_HAMMER))
				str_append(aWeapons, "hammer", sizeof(aWeapons));
			if(pChar->GetWeaponGot(WEAPON_GUN))
				str_append(aWeapons, aWeapons[0] ? ", gun" : "gun", sizeof(aWeapons));
			if(pChar->GetWeaponGot(WEAPON_SHOTGUN))
				str_append(aWeapons, aWeapons[0] ? ", shotgun" : "shotgun", sizeof(aWeapons));
			if(pChar->GetWeaponGot(WEAPON_GRENADE))
				str_append(aWeapons, aWeapons[0] ? ", grenade" : "grenade", sizeof(aWeapons));
			if(pChar->GetWeaponGot(WEAPON_LASER))
				str_append(aWeapons, aWeapons[0] ? ", rifle" : "rifle", sizeof(aWeapons));

			if(pChar->GetWeaponGot(Weapon))
				str_format(m_pResponse, m_SizeOfResponse, "%s Yes I got those weapons: %s", m_pMessageAuthor, aWeapons);
			else
				str_format(m_pResponse, m_SizeOfResponse, "%s No I got those weapons: %s", m_pMessageAuthor, aWeapons);
			return true;
		}
	}
	if(WhatOs())
		return true;

	// fake?
	if(str_find_nocase(m_pMessage, "fake?") ||
		str_find_nocase(m_pMessage, "you fake") ||
		str_find_nocase(m_pMessage, "u fake") ||
		str_find_nocase(m_pMessage, "feker"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s yes i am fake", m_pMessageAuthor);
		return true;
	}

	// real?
	if(str_find_nocase(m_pMessage, "real?") ||
		str_find_nocase(m_pMessage, "you real") ||
		str_find_nocase(m_pMessage, "u real") ||
		str_find_nocase(m_pMessage, "you reel") ||
		str_find_nocase(m_pMessage, "u reel") ||
		str_find_nocase(m_pMessage, "reel?"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s: no u", m_pMessageAuthor);
		return true;
	}

	// when new map?
	if(MsgLen < NameLen + 16 && (str_find_nocase(m_pMessage, "when new map?") ||
					    str_find_nocase(m_pMessage, "when make new map") ||
					    str_find_nocase(m_pMessage, "when new gores map") ||
					    str_find_nocase(m_pMessage, "when new ddrace map")))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s new map this summer (maybe)", m_pMessageAuthor);
		return true;
	}

	// when new youtube?
	if(MsgLen < NameLen + 20 && (str_find_nocase(m_pMessage, "when new yt") ||
					    str_find_nocase(m_pMessage, "when youtub") ||
					    str_find_nocase(m_pMessage, "when new youtub") ||
					    str_find_nocase(m_pMessage, "new video") ||
					    str_find_nocase(m_pMessage, "when video") ||
					    str_find_nocase(m_pMessage, "new vidio") ||
					    str_find_nocase(m_pMessage, "when vidio") ||
					    str_find_nocase(m_pMessage, "new vido") ||
					    str_find_nocase(m_pMessage, "when vido") ||
					    str_find_nocase(m_pMessage, "new jutub") ||
					    str_find_nocase(m_pMessage, "when jutub")))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s new yt next years summer (maybe)", m_pMessageAuthor);
		return true;
	}

	// you legend
	if(MsgLen < NameLen + 16 && str_find_nocase(m_pMessage, "legend"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "you are legend %s", m_pMessageAuthor);
		return true;
	}

	// love
	if(str_find_nocase(m_pMessage, "<3"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s <3", m_pMessageAuthor);
		return true;
	}

	// weeb
	if(str_find_nocase(m_pMessage, "uwu"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s OwO", m_pMessageAuthor);
		return true;
	}
	if(str_find_nocase(m_pMessage, "owo"))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s UwU", m_pMessageAuthor);
		return true;
	}
	// no u
	if(MsgLen < NameLen + 8 && (str_find_nocase(m_pMessage, "no u") ||
					   str_find_nocase(m_pMessage, "no you") ||
					   str_find_nocase(m_pMessage, "noob") ||
					   str_find_nocase(m_pMessage, "nob") ||
					   str_find_nocase(m_pMessage, "nuub") ||
					   str_find_nocase(m_pMessage, "nub") ||
					   str_find_nocase(m_pMessage, "bad")))
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s no u", m_pMessageAuthor);
		return true;
	}
	return false;
}
