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
	ReplyBot.m_Context.m_pfnGetTotalJumpsCallback = CChillerBotUX::GetTotalJumpsCallback;
	ReplyBot.m_Context.m_pfnGetUnusedJumpsCallback = CChillerBotUX::GetUnusedJumpsCallback;
	ReplyBot.m_Context.m_pfnSendChat = CChillerBotUX::SendChatCallback;
	ReplyBot.m_Context.m_pfnGetWeaponGot = CChillerBotUX::GetWeaponGotCallback;
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
	ReplyBot.m_Context.m_Config.m_ClDummy = g_Config.m_ClDummy;
	ReplyBot.m_Context.m_Config.m_InpMousesens = g_Config.m_InpMousesens;
	ReplyBot.m_Context.m_Config.m_ClMouseMaxDistance = g_Config.m_ClMouseMaxDistance;
	ReplyBot.m_Context.m_Config.m_ClDyncam = g_Config.m_ClDyncam;
	if(ReplyBot.Reply(&Message, m_pResponse, m_SizeOfResponse))
		return true;

	// ping without further context
	if(MsgLen < NameLen + 2)
	{
		str_format(m_pResponse, m_SizeOfResponse, "%s ?", m_pMessageAuthor);
		return true;
	}

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
