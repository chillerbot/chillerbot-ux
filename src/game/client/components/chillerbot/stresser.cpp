// chillerbot-ux
// The stresser component
// used to stress test servers and find crashbugs

#include "stresser.h"

#include <base/io.h>
#include <base/str.h>

#include <engine/shared/config.h>
#include <engine/shared/linereader.h>

#include <game/client/gameclient.h>

void CStresser::OnInit()
{
	m_RequestCmdlist = 0;
	m_PenDelay = 0;
	for(auto &SendData : m_SendData)
		SendData = false;
}

void CStresser::OnMapLoad()
{
	m_RequestCmdlist = -(time_get() + time_freq() * 5); // wait a few seconds before requesting it
}

void CStresser::OnConsoleInit()
{
	Console()->Chain("cl_pentest", ConchainPenTest, this);
}

void CStresser::ConchainPenTest(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	CStresser *pSelf = (CStresser *)pUserData;
	pfnCallback(pResult, pCallbackUserData);
	for(auto &SendData : pSelf->m_SendData)
		SendData = false;
}

void CStresser::RandomMovements()
{
	float t = Client()->LocalTime();
	mem_zero(&m_aInputData[g_Config.m_ClDummy], sizeof(m_aInputData[0]));

	m_aInputData[g_Config.m_ClDummy].m_Direction = rand() % 3 - 1;
	m_aInputData[g_Config.m_ClDummy].m_Jump = ((int)t);
	m_aInputData[g_Config.m_ClDummy].m_Fire = ((int)(t * 10));
	m_aInputData[g_Config.m_ClDummy].m_Hook = ((int)(t * 2)) & 1;
	m_aInputData[g_Config.m_ClDummy].m_WantedWeapon = ((int)t) % NUM_WEAPONS;
	m_aInputData[g_Config.m_ClDummy].m_TargetX = (int)(sinf(t * 3) * 100.0f);
	m_aInputData[g_Config.m_ClDummy].m_TargetY = (int)(cosf(t * 3) * 100.0f);

	m_SendData[g_Config.m_ClDummy] = true;
}

void CStresser::OnRender()
{
	if(!Config()->m_ClPenTest)
		return;

	RandomMovements();

	if(m_RequestCmdlist < 0) // waiting to send
	{
		if(time_get() >= -m_RequestCmdlist)
		{
			GameClient()->m_Chat.SendChat(0, "/cmdlist");
			m_RequestCmdlist = time_get();
		}
		// dbg_msg("pentest", "time=%lld req=%lld diff=%lld", time_get(), -m_RequestCmdlist, (-m_RequestCmdlist - time_get()) / time_freq());
		return;
	}
	else if(m_RequestCmdlist)
	{
		int64_t SecsPassed = -(m_RequestCmdlist - time_get()) / time_freq();
		// str_format(aBuf, sizeof(aBuf), "sent message %lld secs ago", SecsPassed);
		// m_pChat->SayChat(aBuf);
		if(SecsPassed > 1)
		{
			char aBuf[2048];
			str_copy(aBuf, "", sizeof(aBuf));
			for(const auto &ChatCmd : m_vChatCmds)
			{
				str_append(aBuf, ChatCmd, sizeof(aBuf));
				// dbg_msg("pentest", "append chat cmd=%s", ChatCmd);
			}
			// m_pChat->SayChat(aBuf);
			dbg_msg("pentest", "found chat cmds=%s", aBuf);
			// m_pChat->SayChat("stopped waiting.");
			m_RequestCmdlist = 0; // finished waiting for response
		}
		return;
	}

	m_PenDelay--;
	if(m_PenDelay > 0)
		return;
	m_PenDelay = 100 + rand() % 50;

	// chat messages
	if(rand() % 2) // parsed chat cmds
	{
		char aChatCmd[128];
		char aArg[64];
		const char *pCharset = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"§$%&/()=?{[]}\\<>|-.,;:+#*'~'@_/";
		str_copy(aArg, "", sizeof(aArg));
		int Len = rand() % 64;
		for(int i = 0; i < Len; i++)
		{
			char aBuf[2];
			str_format(aBuf, sizeof(aBuf), "%c", pCharset[rand() % str_length(pCharset)]);
			str_append(aArg, aBuf, sizeof(aArg));
		}
		str_format(aChatCmd, sizeof(aChatCmd), "/%s %s", GetRandomChatCommand(), aArg);
		GameClient()->m_Chat.SendChat(0, aChatCmd);
	}
	else // file messages
	{
		char aCmd[512];
		if(GetPentestCommand(g_Config.m_ClPenTestFile, aCmd, sizeof(aCmd)))
		{
			GameClient()->m_Chat.SendChat(0, aCmd);
		}
		else
		{
			const int NumCmds = 3;
			char aaCmds[NumCmds][512] = {
				"/register xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 12831237189237189231982371938712893798",
				"todo: configure me ( pentest file not found)",
				"/login bang baz baz"};
			GameClient()->m_Chat.SendChat(0, aaCmds[rand() % NumCmds]);
		}
	}
}

const char *CStresser::GetRandomChatCommand()
{
	if(m_vChatCmds.empty())
		return "";
	return m_vChatCmds[rand() % m_vChatCmds.size()];
}

bool CStresser::GetPentestCommand(char const *pFileName, char *pCmd, int CmdSize)
{
	IOHANDLE File = Storage()->OpenFile(pFileName, IOFLAG_READ, IStorage::TYPE_ALL);
	if(!File)
		return false;

	std::vector<const char *> v;
	const char *pLine;
	CLineReader LineReader = CLineReader();

	if(!LineReader.OpenFile(File))
	{
		char aBuf[512];
		str_format(aBuf, sizeof(aBuf), "failed to open '%s'", pFileName);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "stresser", aBuf);
		return false;
	}

	while((pLine = LineReader.Get()))
		if(str_length(pLine))
			if(pLine[0] != '#')
				v.push_back(pLine);
	const char *pCharset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"§$%&/()=?{[]}\\<>|-.,;:+#*'~'@_/";
	const char *pMessage = v[rand() % v.size()];
	str_copy(pCmd, pMessage, CmdSize);
	for(int i = 0; pCmd[i] != 0; i++)
	{
		if(pCmd[i] == '?')
			pCmd[i] = pCharset[rand() % str_length(pCharset)];
	}
	return true;
}

void CStresser::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_CHAT)
	{
		CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;
		OnChatMessage(pMsg->m_ClientId, pMsg->m_Team, pMsg->m_pMessage);
	}
}

void CStresser::OnChatMessage(int ClientId, int Team, const char *pMsg)
{
	char aBuf[2048];
	if(ClientId == -1) // server message
	{
		if(m_RequestCmdlist > 0)
		{
			// int64 SecsPassed = -(GameClient()->m_RequestCmdlist - time_get()) / time_freq();
			// str_format(aBuf, sizeof(aBuf), "sent message %lld secs ago", SecsPassed);
			// Say(0, aBuf);
			// dbg_msg("pentest", "cmdlist: %s", pMsg);
			str_copy(aBuf, pMsg, sizeof(aBuf));
			char aToken[64];
			for(const char *Tok = aBuf; (Tok = str_next_token(Tok, ",", aToken, sizeof(aToken)));)
			{
				char *pBuf = (char *)malloc(64 * sizeof(char));
				str_copy(pBuf, str_skip_whitespaces(aToken), 64);
				m_vChatCmds.push_back(pBuf);
				// dbg_msg("pentest", "found chat command: %s", pBuf);
			}
		}
	}
}
