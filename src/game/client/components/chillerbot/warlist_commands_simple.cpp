// ChillerDragon 2021 - chillerbot ux

#include "warlist.h"

#include <base/str.h>

#include <engine/config.h>
#include <engine/shared/linereader.h>
#include <engine/textrender.h>

#include <game/client/gameclient.h>

void CWarList::AddSimpleWar(const char *pName)
{
	if(!pName || pName[0] == '\0')
	{
		GameClient()->m_Chat.AddLine(-2, 0, "Error: missing argument <name>");
		return;
	}
	if(!Storage()->CreateFolder("chillerbot/warlist/war", IStorage::TYPE_SAVE))
	{
		GameClient()->m_Chat.AddLine(-2, 0, "Error: failed to create war folder");
		return;
	}
	if(!Storage()->CreateFolder("chillerbot/warlist/war/war", IStorage::TYPE_SAVE))
	{
		GameClient()->m_Chat.AddLine(-2, 0, "Error: failed to create war/war folder");
		return;
	}

	AddWar("war", pName);
}

void CWarList::RemoveSimpleWar(const char *pName)
{
	char aBuf[512];
	if(!RemoveWarNameFromVector("chillerbot/warlist/war/war", pName))
	{
		str_format(aBuf, sizeof(aBuf), "Name '%s' not found in the war list", pName);
		GameClient()->m_Chat.AddLine(-2, 0, aBuf);
		return;
	}
	if(!WriteWarNames("chillerbot/warlist/war/war"))
	{
		GameClient()->m_Chat.AddLine(-2, 0, "Error: failed to write war names");
	}
	str_format(aBuf, sizeof(aBuf), "Removed '%s' from the war list", pName);
	GameClient()->m_Chat.AddLine(-2, 0, aBuf);
	ReloadList();
}

void CWarList::AddSimpleTeam(const char *pName)
{
	if(!pName || pName[0] == '\0')
	{
		GameClient()->m_Chat.AddLine(-2, 0, "Error: missing argument <name>");
		return;
	}
	if(!Storage()->CreateFolder("chillerbot/warlist/team", IStorage::TYPE_SAVE))
	{
		GameClient()->m_Chat.AddLine(-2, 0, "Error: failed to create team folder");
		return;
	}
	if(!Storage()->CreateFolder("chillerbot/warlist/team/team", IStorage::TYPE_SAVE))
	{
		GameClient()->m_Chat.AddLine(-2, 0, "Error: failed to create team/team folder");
		return;
	}

	AddTeam("team", pName);
}

void CWarList::RemoveSimpleTeam(const char *pName)
{
	char aBuf[512];
	if(!RemoveTeamNameFromVector("chillerbot/warlist/team/team", pName))
	{
		str_format(aBuf, sizeof(aBuf), "Name '%s' not found in the war list", pName);
		GameClient()->m_Chat.AddLine(-2, 0, aBuf);
		return;
	}
	if(!WriteTeamNames("chillerbot/warlist/team/team"))
	{
		GameClient()->m_Chat.AddLine(-2, 0, "Error: failed to write war names");
	}
	str_format(aBuf, sizeof(aBuf), "Removed '%s' from the team list", pName);
	GameClient()->m_Chat.AddLine(-2, 0, aBuf);
	ReloadList();
}

bool CWarList::OnChatCmdSimple(char Prefix, int ClientId, int Team, const char *pCmd, int NumArgs, const char **ppArgs, const char *pRawArgLine)
{
	if(!str_comp(pCmd, "search")) // "search <name can contain spaces>"
	{
		GameClient()->m_Chat.AddLine(-2, 0, "Error: search only works in advanced warlist mode");
		return true;
	}
	else if(!str_comp(pCmd, "help"))
	{
		GameClient()->m_Chat.AddLine(-2, 0, "=== chillerbot-ux warlist ===");
		GameClient()->m_Chat.AddLine(-2, 0, "!war <name>");
		GameClient()->m_Chat.AddLine(-2, 0, "!peace <name>");
		GameClient()->m_Chat.AddLine(-2, 0, "!team <name>");
		GameClient()->m_Chat.AddLine(-2, 0, "!delteam <name>");
		// GameClient()->m_Chat.AddLine(-2, 0, "!search <name>");
	}
	else if(!str_comp(pCmd, "war") || !str_comp(pCmd, "addwar")) // "war <name>"
	{
		AddSimpleWar(pRawArgLine);
		return true;
	}
	else if(!str_comp(pCmd, "team") || !str_comp(pCmd, "addteam")) // "team <name>"
	{
		AddSimpleTeam(pRawArgLine);
		return true;
	}
	else if(!str_comp(pCmd, "delwar") || !str_comp(pCmd, "unwar") || !str_comp(pCmd, "peace")) // "delwar <name>"
	{
		RemoveSimpleWar(pRawArgLine);
		return true;
	}
	else if(!str_comp(pCmd, "delteam") || !str_comp(pCmd, "unteam") || !str_comp(pCmd, "unfriend")) // "delteam <name>"
	{
		RemoveSimpleTeam(pRawArgLine);
		return true;
	}
	else if(
		!str_comp(pCmd, "addreason") ||
		!str_comp(pCmd, "create") ||
		!str_comp(pCmd, "addtraitor"))
	{
		char aBuf[512];
		str_format(aBuf, sizeof(aBuf), "Error: %s only works in advanced warlist mode", pCmd);
		GameClient()->m_Chat.AddLine(-2, 0, aBuf);
		return true;
	}
	else
	{
		return false;
	}
	return true;
}
