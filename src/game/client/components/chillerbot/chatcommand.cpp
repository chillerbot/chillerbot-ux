// ChillerDragon 2023 - chillerbot ux

#include "chatcommand.h"

#include <base/str.h>

#include <engine/shared/protocol.h>

#include <game/client/components/chat.h>
#include <game/client/components/chillerbot/chillerbotux.h>
#include <game/client/gameclient.h>

#include <cinttypes>

void CChatCommand::OnServerMsg(const char *pMsg)
{
}

bool CChatCommand::OnChatMsg(int ClientId, int Team, const char *pMsg)
{
	if(!pMsg[1])
		return false;
	if(pMsg[0] == '.' || pMsg[0] == ':' || pMsg[0] == '!' || pMsg[0] == '#' || pMsg[0] == '$')
		if(ParseChatCmd(pMsg[0], ClientId, Team, pMsg + 1))
			return true;

	OnNoChatCommandMatches(ClientId, Team, pMsg);
	return false;
}

void CChatCommand::OnNoChatCommandMatches(int ClientId, int Team, const char *pMsg)
{
	// ux components

	// GameClient()->m_WarList.OnNoChatCommandMatches(ClientId, Team, pMsg);

	// zx components
}

bool CChatCommand::OnChatCmd(char Prefix, int ClientId, int Team, const char *pCmd, int NumArgs, const char **ppArgs, const char *pRawArgLine)
{
	bool Match = false;
	// ux components

	if(GameClient()->m_WarList.OnChatCmd(Prefix, ClientId, Team, pCmd, NumArgs, ppArgs, pRawArgLine))
		Match = true;

	// zx components

	return Match;
}

bool CChatCommand::ParseChatCmd(char Prefix, int ClientId, int Team, const char *pCmdWithArgs)
{
	char aRawArgLine[512];
	str_copy(aRawArgLine, pCmdWithArgs);
	const int MaxArgLen = 256;
	char aCmd[MaxArgLen];
	int i;
	for(i = 0; pCmdWithArgs[i] && i < MaxArgLen; i++)
	{
		if(pCmdWithArgs[i] == ' ')
			break;
		aCmd[i] = pCmdWithArgs[i];
	}

	int Skip = i;
	while(pCmdWithArgs[Skip] && str_isspace(pCmdWithArgs[Skip]))
		Skip++;
	str_copy(aRawArgLine, pCmdWithArgs + Skip);

	aCmd[i] = '\0';
	int RestOffset = GameClient()->m_ChatHelper.ChatCommandGetROffset(aCmd);

	// max 16 args of 128 len each
	const int MaxArgs = 16;
	char **ppArgs = new char *[MaxArgs];
	for(int x = 0; x < MaxArgs; ++x)
	{
		ppArgs[x] = new char[MaxArgLen];
		ppArgs[x][0] = '\0';
	}
	int NumArgs = 0;
	int k = 0;
	while(pCmdWithArgs[i])
	{
		if(k + 1 >= MaxArgLen)
		{
			dbg_msg("chillerbot", "ERROR: chat command has too long arg");
			break;
		}
		if(NumArgs + 1 >= MaxArgs)
		{
			dbg_msg("chillerbot", "ERROR: chat command has too many args");
			break;
		}
		if(pCmdWithArgs[i] == ' ')
		{
			// do not delimit on space
			// if we reached the r parameter
			if(NumArgs == RestOffset)
			{
				// strip spaces from the beginning
				// add spaces in the middle and end
				if(ppArgs[NumArgs][0])
				{
					ppArgs[NumArgs][k] = pCmdWithArgs[i];
					k++;
					i++;
					continue;
				}
			}
			else if(ppArgs[NumArgs][0])
			{
				ppArgs[NumArgs][k] = '\0';
				k = 0;
				NumArgs++;
			}
			i++;
			continue;
		}
		ppArgs[NumArgs][k] = pCmdWithArgs[i];
		k++;
		i++;
	}
	if(ppArgs[NumArgs][0])
	{
		ppArgs[NumArgs][k] = '\0';
		NumArgs++;
	}

	// char aArgsStr[128];
	// aArgsStr[0] = '\0';
	// for(i = 0; i < NumArgs; i++)
	// {
	// 	if(aArgsStr[0] != '\0')
	// 		str_append(aArgsStr, ", ", sizeof(aArgsStr));
	// 	str_append(aArgsStr, ppArgs[i], sizeof(aArgsStr));
	// }

	// char aBuf[512];
	// str_format(aBuf, sizeof(aBuf), "got cmd '%s' with %d args: %s", aCmd, NumArgs, aArgsStr);
	// Say(aBuf);
	bool Match = OnChatCmd(Prefix, ClientId, Team, aCmd, NumArgs, (const char **)ppArgs, aRawArgLine);
	for(int x = 0; x < MaxArgs; ++x)
		delete[] ppArgs[x];
	delete[] ppArgs;
	return Match;
}

void CChatCommand::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_CHAT)
	{
		CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;
		int ClientId = pMsg->m_ClientId;
		if(ClientId == -1 && pMsg->m_Team < 2)
		{
			OnServerMsg(pMsg->m_pMessage);
		}
		// ignore own messages
		// they get processed on send
		// if the server spoofs us we drop it
		else if(!GameClient()->m_ChillerBotUX.IsOurClientId(ClientId))
		{
			OnChatMsg(ClientId, pMsg->m_Team, pMsg->m_pMessage);
		}
	}
}
