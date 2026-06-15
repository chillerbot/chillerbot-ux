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

bool CReplyToPing::ListWars()
{
	const char *pWho = str_find_nocase(m_pMessage, "who ");
	if(pWho)
		pWho = pWho + str_length("who ");
	if(!pWho)
		if((pWho = str_find_nocase(m_pMessage, "which")))
			pWho = pWho + str_length("which");
	if(!pWho)
		if((pWho = str_find_nocase(m_pMessage, "got any")))
			pWho = pWho + str_length("got any");
	if(!pWho)
		if((pWho = str_find_nocase(m_pMessage, "have any")))
			pWho = pWho + str_length("have any");
	if(!pWho)
		if((pWho = str_find_nocase(m_pMessage, "wer ")))
			pWho = pWho + str_length("wer ");
	if(!pWho)
		if((pWho = str_find_nocase(m_pMessage, "wie viele")))
			pWho = pWho + str_length("wie viele");
	if(!pWho)
		if((pWho = str_find_nocase(m_pMessage, " how ")))
			pWho = pWho + str_length(" how ");
	if(!pWho)
		pWho = str_startswith_nocase(m_pMessage, "how ");
	if(!pWho)
		if((pWho = str_find_nocase(m_pMessage, "howm"))) // howmani howmany howm any
			pWho = pWho + str_length("howm");
	if(!pWho)
		if((pWho = str_find_nocase(m_pMessage, "u got")))
			pWho = pWho + str_length("u got");
	if(pWho)
	{
		const char *pEnemy = nullptr;
		pEnemy = str_find_nocase(pWho, "enemi");
		if(pEnemy)
			pEnemy = pEnemy + str_length("enemi");
		else if((pEnemy = str_find_nocase(pWho, "enemy")))
			pEnemy = pEnemy + str_length("enemy");
		else if((pEnemy = str_find_nocase(pWho, "ennemy")))
			pEnemy = pEnemy + str_length("ennemy");
		else if((pEnemy = str_find_nocase(pWho, "ennemi")))
			pEnemy = pEnemy + str_length("ennemi");
		else if((pEnemy = str_find_nocase(pWho, "gegner")))
			pEnemy = pEnemy + str_length("gegner");
		else if((pEnemy = str_find_nocase(pWho, "against")))
			pEnemy = pEnemy + str_length("against");
		else if((pEnemy = str_find_nocase(pWho, "opponent")))
			pEnemy = pEnemy + str_length("opponent");
		else if((pEnemy = str_find_nocase(pWho, "raid")))
			pEnemy = pEnemy + str_length("raid");
		else if((pEnemy = str_find_nocase(pWho, "block")))
			pEnemy = pEnemy + str_length("block");
		else if((pEnemy = str_find_nocase(pWho, "kill")))
			pEnemy = pEnemy + str_length("kill");
		else if((pEnemy = str_find_nocase(pWho, "spaik")))
			pEnemy = pEnemy + str_length("spaik");
		else if((pEnemy = str_find_nocase(pWho, "spike")))
			pEnemy = pEnemy + str_length("spike");
		else if((pEnemy = str_find_nocase(pWho, "bad")))
			pEnemy = pEnemy + str_length("bad");
		else if((pEnemy = str_find_nocase(pWho, "war")))
			pEnemy = pEnemy + str_length("war");
		if(pEnemy)
		{
			char aEnemyList[256]; // 255 max msg len
			aEnemyList[0] = '\0';
			int NumEnemies = 0;
			for(auto &Client : GameClient()->m_aClients)
			{
				if(!Client.m_Active)
					continue;
				if(!GameClient()->m_WarList.IsWarlist(Client.m_aName) &&
					!GameClient()->m_WarList.IsWarClanlist(Client.m_aClan))
					continue;

				NumEnemies++;
				if(aEnemyList[0])
				{
					str_append(aEnemyList, ", ", sizeof(aEnemyList));
				}
				str_append(aEnemyList, Client.m_aName, sizeof(aEnemyList));
			}
			if(NumEnemies)
				str_format(m_pResponse, m_SizeOfResponse, "%s %d of my %d enemies are online: %s", m_pMessageAuthor, NumEnemies, GameClient()->m_WarList.NumEnemies(), aEnemyList);
			else
				str_format(m_pResponse, m_SizeOfResponse, "%s currently 0 of my %d enemies are connected", m_pMessageAuthor, GameClient()->m_WarList.NumEnemies());
			return true;
		}
		const char *pFriend = nullptr;
		pFriend = str_find_nocase(pWho, "team");
		if(pFriend)
			pFriend = pFriend + str_length("team");
		else if((pFriend = str_find_nocase(pWho, "friend")))
			pFriend = pFriend + str_length("friend");
		else if((pFriend = str_find_nocase(pWho, "frint")))
			pFriend = pFriend + str_length("frint");
		else if((pFriend = str_find_nocase(pWho, "peace")))
			pFriend = pFriend + str_length("peace");
		else if((pFriend = str_find_nocase(pWho, "like")))
			pFriend = pFriend + str_length("like");
		else if((pFriend = str_find_nocase(pWho, "good")))
			pFriend = pFriend + str_length("good");
		else if((pFriend = str_find_nocase(pWho, "help")))
			pFriend = pFriend + str_length("help");
		else if((pFriend = str_find_nocase(pWho, "side")))
			pFriend = pFriend + str_length("side");
		else if((pFriend = str_find_nocase(pWho, "with u")))
			pFriend = pFriend + str_length("with u");
		else if((pFriend = str_find_nocase(pWho, "with y")))
			pFriend = pFriend + str_length("with y");
		else if((pFriend = str_find_nocase(pWho, "u with")))
			pFriend = pFriend + str_length("u with");
		if(pFriend)
		{
			char aFriendList[256]; // 255 max msg len
			aFriendList[0] = '\0';
			int NumFriends = 0;
			for(auto &Client : GameClient()->m_aClients)
			{
				if(!Client.m_Active)
					continue;
				if(!GameClient()->m_WarList.IsTeamlist(Client.m_aName) &&
					!GameClient()->m_WarList.IsTeamClanlist(Client.m_aClan))
					continue;

				NumFriends++;
				if(aFriendList[0])
				{
					str_append(aFriendList, ", ", sizeof(aFriendList));
				}
				str_append(aFriendList, Client.m_aName, sizeof(aFriendList));
			}
			if(NumFriends)
				str_format(m_pResponse, m_SizeOfResponse, "%s %d of my %d friends are online: %s", m_pMessageAuthor, NumFriends, GameClient()->m_WarList.NumTeam(), aFriendList);
			else
				str_format(m_pResponse, m_SizeOfResponse, "%s currently 0 of my %d friends are connected", m_pMessageAuthor, GameClient()->m_WarList.NumTeam());
			return true;
		}
	}
	return false;
}
