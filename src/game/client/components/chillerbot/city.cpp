// ChillerDragon 2021 - chillerbot ux

#include <base/vmath.h>
#include <engine/config.h>
#include <engine/shared/config.h>
#include <game/client/components/controls.h>
#include <game/client/gameclient.h>

#include <engine/serverbrowser.h>

#include "chillerbotux.h"
#include "game/client/components/chillerbot/chathelper.h"

#include "city.h"

void CCityHelper::OnInit()
{
	m_AutoDropMoney[0] = false;
	m_AutoDropMoney[1] = false;
	m_WalletDropDelay[0] = 1;
	m_WalletDropDelay[1] = 1;
	m_NextWalletDrop[0] = 0;
	m_NextWalletDrop[1] = 0;
	m_LastDummy = 0;
}

void CCityHelper::SetAutoDrop(bool Drop, int Delay, int ClientId)
{
	char aBuf[128];
	m_AutoDropMoney[ClientId] = Drop;
	m_WalletDropDelay[ClientId] = Delay;
	str_format(aBuf, sizeof(aBuf), "turn drop %s on %s", Drop ? "on" : "off", ClientId ? "dummy" : "main");
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "city", aBuf);
	str_format(aBuf, sizeof(aBuf), "drop main=%d dummy=%d", m_AutoDropMoney[0], m_AutoDropMoney[1]);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "city", aBuf);
}

void CCityHelper::OnConsoleInit()
{
	Console()->Register("auto_drop_money", "?i[delay]?i[dummy]?s[on|off]", CFGFLAG_CLIENT, ConAutoDropMoney, this, "");

	Console()->Chain("cl_show_wallet", ConchainShowWallet, this);
}

void CCityHelper::ConchainShowWallet(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	CWarList *pSelf = (CWarList *)pUserData;
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->GetInteger(0))
		pSelf->GameClient()->m_ChillerBotUX.EnableComponent("money");
	else
		pSelf->GameClient()->m_ChillerBotUX.DisableComponent("money");
}

void CCityHelper::PrintWalletToChat(int ClientId, const char *pWhisper)
{
	if(ClientId == -1)
		ClientId = g_Config.m_ClDummy;

	char aWallet[128];
	char aBuf[512];
	str_format(aWallet, sizeof(aWallet), "money: %d", WalletMoney(ClientId));
	if(!ClientId)
	{
		for(auto &Entry : m_vWalletMain)
		{
			str_format(aBuf, sizeof(aBuf), ", \"%s\": %d", Entry.first.c_str(), Entry.second);
			str_append(aWallet, aBuf, sizeof(aWallet));
		}
	}
	else
	{
		for(auto &Entry : m_vWalletDummy)
		{
			str_format(aBuf, sizeof(aBuf), ", \"%s\": %d", Entry.first.c_str(), Entry.second);
			str_append(aWallet, aBuf, sizeof(aWallet));
		}
	}
	if(pWhisper && pWhisper[0])
	{
		str_format(aBuf, sizeof(aBuf), "/whisper \"%s\" %s", pWhisper, aWallet);
		m_pClient->m_Chat.SendChat(0, aBuf);
	}
	else
		m_pClient->m_Chat.SendChat(0, aWallet);
}

int CCityHelper::WalletMoney(int ClientId)
{
	if(ClientId == -1)
		ClientId = g_Config.m_ClDummy;
	if(m_WalletMoney[ClientId] == 0)
	{
		if(ClientId)
			m_vWalletDummy.clear();
		else
			m_vWalletMain.clear();
	}
	return m_WalletMoney[ClientId];
}

void CCityHelper::SetWalletMoney(int Money, int ClientId)
{
	if(ClientId == -1)
		ClientId = g_Config.m_ClDummy;
	m_WalletMoney[ClientId] = Money;
	if(g_Config.m_ClShowWallet)
	{
		char aBuf[128];
		str_format(aBuf, sizeof(aBuf), "%d", m_WalletMoney[ClientId]);
		m_pClient->m_ChillerBotUX.SetComponentNoteLong("money", aBuf);
	}
}

void CCityHelper::AddWalletMoney(int Money, int ClientId)
{
	if(ClientId == -1)
		ClientId = g_Config.m_ClDummy;
	SetWalletMoney(WalletMoney(ClientId) + Money);
}

void CCityHelper::ConAutoDropMoney(IConsole::IResult *pResult, void *pUserData)
{
	CCityHelper *pSelf = (CCityHelper *)pUserData;
	int ClientId = pResult->NumArguments() > 1 ? pResult->GetInteger(1) : g_Config.m_ClDummy;
	bool Drop = !pSelf->m_AutoDropMoney[ClientId];
	int Delay = pResult->NumArguments() > 0 ? pResult->GetInteger(0) : 4;
	if(pResult->NumArguments() > 2)
		Drop = !str_comp(pResult->GetString(2), "on");
	pSelf->SetAutoDrop(
		Drop,
		Delay < 1 ? 1 : Delay,
		ClientId);
}

void CCityHelper::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_BROADCAST)
	{
		CNetMsg_Sv_Broadcast *pMsg = (CNetMsg_Sv_Broadcast *)pRawMsg;
		const char *pMoney = str_find(pMsg->m_pMessage, "Money [");
		if(!pMoney)
			return;
		char aMoney[128] = {0};
		char aAmount[128] = {0};
		str_copy(aMoney, pMoney + 7, sizeof(aMoney));
		for(int i = 0; i < 128; i++)
		{
			if(aMoney[i] == ']' || aMoney[i] == '\0' || aMoney[i] == '\n')
			{
				aAmount[i] = '\0';
				break;
			}
			aAmount[i] = aMoney[i];
		}
		SetWalletMoney(atoi(aAmount));
		;
	}
	else if(MsgType == NETMSGTYPE_SV_CHAT)
	{
		CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;
		if(pMsg->m_ClientId == -1 && pMsg->m_Team < 2)
			OnServerMsg(pMsg->m_pMessage);
		else
			OnChatMsg(pMsg->m_ClientId, pMsg->m_Team, pMsg->m_pMessage);
	}
}

void CCityHelper::AddWalletEntry(std::vector<std::pair<std::string, int>> *pVec, const std::pair<std::string, int> &Entry)
{
	for(auto &E : *pVec)
	{
		if(E.first == Entry.first)
		{
			E.second += Entry.second;
			return;
		}
	}
	pVec->push_back(Entry);
}

void CCityHelper::OnServerMsg(const char *pMsg)
{
	int Money = 0;
	int n = sscanf(pMsg, "Collected %d money", &Money);
	if(n == 1)
	{
		int Owner = ClosestClientIdToPos(
			vec2(m_pClient->m_LocalCharacterPos.x, m_pClient->m_LocalCharacterPos.y),
			g_Config.m_ClDummy);
		if(Owner != -1)
		{
			const char *pName = m_pClient->m_aClients[Owner].m_aName;
			std::pair<std::string, int> Pair;
			Pair.first = std::string(pName);
			Pair.second = Money;
			if(!g_Config.m_ClDummy)
				AddWalletEntry(&m_vWalletMain, Pair);
			else
				AddWalletEntry(&m_vWalletDummy, Pair);
		}
		AddWalletMoney(Money);
		return;
	}
	n = sscanf(pMsg, "You withdrew %d money from your bank account to your wallet.", &Money);
	if(n == 1)
	{
		AddWalletMoney(Money);
		return;
	}
	n = sscanf(pMsg, "You deposited %d money from your wallet to your bank account.", &Money);
	if(n == 1)
	{
		AddWalletMoney(-Money);
		return;
	}
	n = sscanf(pMsg, "Wallet [%d]", &Money);
	if(n == 1)
	{
		SetWalletMoney(Money);
		return;
	}
	if(!str_comp(pMsg, "You don't have enough money in your wallet"))
	{
		m_pClient->m_ChatHelper.SayBuffer("/stats", CChatHelper::BUFFER_CHAT_ALL, true);
		return;
	}
	if(!str_comp(pMsg, "No such command: money."))
	{
		m_AutoDropMoney[0] = false;
		m_AutoDropMoney[1] = false;
		return;
	}
}

int CCityHelper::ClosestClientIdToPos(vec2 Pos, int Dummy)
{
	float ClosestRange = 0.f;
	int ClosestId = -1;
	vec2 ClosestPos = vec2(-1, -1);

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		int ClientId = m_pClient->m_aLocalIds[Dummy];
		if(!m_pClient->m_Snap.m_aCharacters[i].m_Active)
			continue;
		if(ClientId == i)
			continue;

		const void *pPrevInfo = Client()->SnapFindItem(IClient::SNAP_PREV, NETOBJTYPE_PLAYERINFO, i);
		const void *pInfo = Client()->SnapFindItem(IClient::SNAP_CURRENT, NETOBJTYPE_PLAYERINFO, i);

		if(pPrevInfo && pInfo)
		{
			vec2 OtherPos = m_pClient->m_aClients[i].m_Predicted.m_Pos;
			float len = distance(OtherPos, Pos);
			if(len < ClosestRange || !ClosestRange)
			{
				ClosestRange = len;
				ClosestPos = OtherPos;
				ClosestId = i;
			}
		}
	}
	return ClosestId;
}

void CCityHelper::OnChatMsg(int ClientId, int Team, const char *pMsg)
{
	// in demos m_aLocalIds are bugged
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
		return;

	if(m_pClient->m_aLocalIds[0] == -1)
		return;

	// TODO: move this to chat helper? or do I want a new chat command system in each component? -.-
	const char *pName = m_pClient->m_aClients[m_pClient->m_aLocalIds[0]].m_aName;
	const char *pDummyName = "";
	if(m_pClient->Client()->DummyConnected() && m_pClient->m_aLocalIds[1] != -1)
		pDummyName = m_pClient->m_aClients[m_pClient->m_aLocalIds[1]].m_aName;
	int NameLen = 0;

	if(str_startswith(pMsg, pName))
		NameLen = str_length(pName);
	else if(m_pClient->Client()->DummyConnected() && str_startswith(pMsg, pDummyName))
		NameLen = str_length(pDummyName);

	if(!NameLen && Team < 2)
		return;

	CServerInfo ServerInfo;
	Client()->GetServerInfo(&ServerInfo);
	bool IsFDDRace = !str_comp(ServerInfo.m_aGameType, "F-DDrace");
	int MsgOffset = 0;
	char aName[64];
	aName[0] = '\0';
	// if whisper respond in whisper
	if(Team >= 2)
	{
		str_copy(aName, m_pClient->m_aClients[ClientId].m_aName, sizeof(aName));
		if(IsFDDRace && ClientId == 63 && !str_comp_num(m_pClient->m_aClients[ClientId].m_aName, " ", 2))
		{
			MsgOffset = m_pClient->m_ChatHelper.Get128Name(pMsg, aName);
			if(MsgOffset == -1)
				MsgOffset = 0;
		}
	}

	char aMsg[2048];
	char aCmd[2048] = {0};
	str_copy(aMsg, pMsg + NameLen, sizeof(aMsg));
	for(unsigned int i = MsgOffset; i < sizeof(aMsg); i++)
	{
		if(aMsg[i] == ' ' || aMsg[i] == ':')
			continue;

		if(aMsg[i] == '!')
			str_copy(aCmd, &aMsg[i + 1], sizeof(aCmd));
		break;
	}
	if(aCmd[0] == '\0')
		return;
	// char aBuf[128];
	// str_format(aBuf, sizeof(aBuf), "cmd '%s'", aCmd);
	// m_pClient->m_Chat.Say(0, aBuf);
	if(!str_comp(aCmd, "wallet"))
		PrintWalletToChat(g_Config.m_ClDummy, aName);
}

void CCityHelper::DropAllMoney(int ClientId)
{
	if(WalletMoney(ClientId) < 1)
		return;

	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "/money drop %d", WalletMoney(ClientId) > 100000 ? 100000 : WalletMoney(ClientId));

	CMsgPacker Msg(NETMSGTYPE_CL_SAY, false);
	Msg.AddInt(0);
	Msg.AddString(aBuf, -1);
	Client()->SendMsg(ClientId, &Msg, MSGFLAG_VITAL);
}

void CCityHelper::OnRender()
{
	if(m_LastDummy != g_Config.m_ClDummy)
	{
		m_LastDummy = g_Config.m_ClDummy;
		SetWalletMoney(WalletMoney());
	}
	for(int i = 0; i < NUM_DUMMIES; i++)
	{
		if(!m_AutoDropMoney[i])
			continue;
		if(time_get() < m_NextWalletDrop[i])
			continue;

		m_NextWalletDrop[i] = time_get() + time_freq() * m_WalletDropDelay[i];
		DropAllMoney(i);
	}
}
