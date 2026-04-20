// ChillerDragon 2021 - chillerbot ux

#include "mmotee.h"

#include "chillerbotux.h"

#include <base/vmath.h>

#include <engine/config.h>
#include <engine/serverbrowser.h>
#include <engine/shared/config.h>

#include <game/client/components/chillerbot/chathelper.h>
#include <game/client/components/controls.h>
#include <game/client/gameclient.h>

void CMmoTee::OnInit()
{
}

void CMmoTee::OnConsoleInit()
{
}

void CMmoTee::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_BROADCAST)
	{
		// CNetMsg_Sv_Broadcast *pMsg = (CNetMsg_Sv_Broadcast *)pRawMsg;
	}
	else if(MsgType == NETMSGTYPE_SV_CHAT)
	{
		// CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;
	}
}

bool CMmoTee::FilterChat(int ClientId, int Team, const char *pLine)
{
	if(!g_Config.m_ClMmoTee)
		return false;
	if(ClientId != -1)
		return false;

	if(str_startswith(pLine, "You obtained an '"))
	{
		const char *pObtained = pLine + sizeof("You obtained an '") - 1;
		if(str_startswith(pObtained, "Copper ore x"))
		{
			// m_pClient->m_Chat.AddLine(-2, 0, "Mined Copper ore");
			return true;
		}
		else if(str_startswith(pObtained, "Coal x"))
		{
			// m_pClient->m_Chat.AddLine(-2, 0, "Mined Coal");
			return true;
		}
		else if(str_startswith(pObtained, "Iron ore x"))
		{
			// m_pClient->m_Chat.AddLine(-2, 0, "Mined Iron ore");
			return true;
		}
		else if(str_startswith(pObtained, "Stone x"))
		{
			// m_pClient->m_Chat.AddLine(-2, 0, "Mined Stone");
			return true;
		}
		else if(str_startswith(pObtained, "Wheat x"))
		{
			// m_pClient->m_Chat.AddLine(-2, 0, "Farmed Wheat");
			return true;
		}
		else if(str_startswith(pObtained, "Tomato x"))
		{
			// m_pClient->m_Chat.AddLine(-2, 0, "Farmed Tomato");
			return true;
		}
		else if(str_startswith(pObtained, "Corn x"))
		{
			// m_pClient->m_Chat.AddLine(-2, 0, "Farmed Corn");
			return true;
		}
		else if(str_startswith(pObtained, "Strawberries x"))
		{
			// m_pClient->m_Chat.AddLine(-2, 0, "Farmed Strawberries");
			return true;
		}
	}

	return false;
}

void CMmoTee::OnRender()
{
	if(!g_Config.m_ClMmoTee)
		return;
}
