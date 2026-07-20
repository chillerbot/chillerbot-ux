// ChillerDragon 2021 - chillerbot ux

#include "remotecontrol.h"

#include <base/io.h>

#include <engine/console.h>
#include <engine/serverbrowser.h>
#include <engine/shared/linereader.h>

#include <generated/protocol.h>

#include <game/client/components/chat.h>
#include <game/client/components/chillerbot/chathelper.h>
#include <game/client/components/chillerbot/chillerbotux.h>
#include <game/client/gameclient.h>

void CRemoteControl::OnChatMessage(int ClientId, int Team, const char *pMsg)
{
	if(!g_Config.m_ClRemoteControl)
		return;
	if(ClientId < 0 || ClientId >= 64)
		return;
	// the value can be -1 caught by asan and this is also checked everywhere else
	// but not sure when exactly that happens
	// is this the "do you know someone who uses a bot?" message?
	// that gets sent by the server before we know our own client id
	// or are we dropping some interesting messages here?
	if(GameClient()->m_aLocalIds[0] < 0 || GameClient()->m_aLocalIds[0] >= MAX_CLIENTS)
		return;

	char aName[64];
	str_copy(aName, GameClient()->m_aClients[ClientId].m_aName, sizeof(aName));
	if(ClientId == 63 && !str_comp_num(GameClient()->m_aClients[ClientId].m_aName, " ", 2))
	{
		GameClient()->m_ChatHelper.Get128Name(pMsg, aName);
	}
	// ignore own and dummys messages
	if(!str_comp(aName, GameClient()->m_aClients[GameClient()->m_aLocalIds[0]].m_aName))
		return;
	if(Client()->DummyConnected() && !str_comp(aName, GameClient()->m_aClients[GameClient()->m_aLocalIds[1]].m_aName))
		return;
	if(Team != 3) // whisper only
		return;

	const CServerInfo &ServerInfo = Client()->ServerInfo();
	bool IsFDDRace = !str_comp(ServerInfo.m_aGameType, "F-DDrace");

	char aBuf[128];
	int Num = 0;
	char aMsg[3][2048] = {{0}};
	for(int i = 0, k = 0; pMsg[i]; i++, k++)
	{
		char c = pMsg[i];
		if(c == ' ' && Num < (IsFDDRace ? 2 : 1))
		{
			Num++;
			k = -1;
			continue;
		}
		aMsg[Num][k] = c;
	}
	if(Num == 0)
	{
		str_format(aBuf, sizeof(aBuf), "Error: %s missing token (usage: '/whisper name token command')", aName);
		GameClient()->m_ChatHelper.SayBuffer(aBuf, Team == 1 ? CChatHelper::BUFFER_CHAT_TEAM : CChatHelper::BUFFER_CHAT_ALL);
		return;
	}
	else if(Num == 1 && IsFDDRace)
	{
		str_format(aBuf, sizeof(aBuf), "Error: %s missing command (usage: '/whisper name token command')", aName);
		GameClient()->m_ChatHelper.SayBuffer(aBuf, Team == 1 ? CChatHelper::BUFFER_CHAT_TEAM : CChatHelper::BUFFER_CHAT_ALL);
		return;
	}
	if(!str_comp(aMsg[IsFDDRace ? 1 : 0], g_Config.m_ClRemoteControlTokenAdmin))
		GameClient()->Console()->ExecuteLine(aMsg[IsFDDRace ? 2 : 1], IConsole::CLIENT_ID_UNSPECIFIED);
	else if(!str_comp(aMsg[IsFDDRace ? 1 : 0], g_Config.m_ClRemoteControlToken))
		ExecuteWhitelisted(aMsg[IsFDDRace ? 2 : 1]);
	else
	{
		dbg_msg("chillerbot", "whisper='%s'", pMsg);
		dbg_msg(
			"chillerbot",
			"Error: %s failed to remote control (invalid token attempt='%s' token='%s' admin='%s')",
			aName, aMsg[IsFDDRace ? 1 : 0], g_Config.m_ClRemoteControlToken, g_Config.m_ClRemoteControlTokenAdmin);
		str_format(
			aBuf,
			sizeof(aBuf),
			"Error: %s failed to remote control (invalid token)",
			aName);
		GameClient()->m_ChatHelper.SayBuffer(aBuf, Team == 1 ? CChatHelper::BUFFER_CHAT_TEAM : CChatHelper::BUFFER_CHAT_ALL);
		return;
	}
}

void CRemoteControl::ExecuteWhitelisted(const char *pCommand, const char *pWhitelistFile)
{
	if(!Storage())
		return;

	// exec the file
	IOHANDLE File = Storage()->OpenFile(pWhitelistFile, IOFLAG_READ, IStorage::TYPE_ALL);

	char aBuf[128];
	if(!File)
	{
		str_format(aBuf, sizeof(aBuf), "failed to open remote control whitelist file '%s'", pWhitelistFile);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillerbot", aBuf);
		return;
	}
	const char *pLine;
	CLineReader Reader;

	str_format(aBuf, sizeof(aBuf), "loading remote control whitelist file '%s'", pWhitelistFile);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillerbot", aBuf);

	if(!Reader.OpenFile(File))
	{
		str_format(aBuf, sizeof(aBuf), "failed to open '%s'", pWhitelistFile);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillerbot", aBuf);
		return;
	}

	while((pLine = Reader.Get()))
	{
		if(!str_comp_nocase(pLine, pCommand))
		{
			str_format(aBuf, sizeof(aBuf), "executing whitelisted command '%s'", pCommand);
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillerbot", aBuf);
			GameClient()->Console()->ExecuteLine(pCommand, IConsole::CLIENT_ID_UNSPECIFIED);
			return;
		}
	}

	str_format(aBuf, sizeof(aBuf), "command '%s' not whitelisted", pCommand);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillerbot", aBuf);
}

void CRemoteControl::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_CHAT)
	{
		CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;
		OnChatMessage(pMsg->m_ClientId, pMsg->m_Team, pMsg->m_pMessage);
	}
}
