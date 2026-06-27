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

// where are you

bool CReplyToPing::Where()
{
	if(str_find_nocase(m_pMessage, "where are you") || str_find_nocase(m_pMessage, "where r u") || str_find_nocase(m_pMessage, "where r yo") ||
		str_find_nocase(m_pMessage, "where u") ||
		str_find_nocase(m_pMessage, "wo bist"))
	{
		for(auto &Client : GameClient()->m_aClients)
		{
			if(!Client.m_Active)
				continue;
			if(str_comp(Client.m_aName, m_pMessageAuthor))
				continue;

			// TODO: dont get current dummy but the pinged dummy, but if it differs the reply chat message will be confusing??
			//       if that is done it has to say "my dummy xx is at pos yy"
			CCharacter *pChar = GameClient()->m_GameWorld.GetCharacterById(GameClient()->m_aLocalIds[g_Config.m_ClDummy]);
			if(!pChar)
				continue;
			vec2 Self = pChar->m_Pos;
			vec2 Other = Client.m_RenderPos;
			float DistY = std::abs(Self.y - Other.y);
			float DistX = std::abs(Self.x - Other.x);
			// instead of some unimaginable long distance number in tiles
			// prefer saying "finish" or "end" without any relation to the other player
			// since it does not matter in this case
			if(DistX > 50 * 32 && DistY > 50 * 32)
			{
				if(GameClient()->m_Snap.m_pLocalCharacter && GameClient()->RaceHelper()->IsClusterRangeFinish(GameClient()->m_PredictedChar.m_Pos, 32))
				{
					str_format(m_pResponse, m_SizeOfResponse, "%s I am at the finish line", m_pMessageAuthor);
					return true;
				}
				else if(GameClient()->m_Snap.m_pLocalCharacter && GameClient()->RaceHelper()->IsClusterRangeStart(GameClient()->m_PredictedChar.m_Pos, 32))
				{
					str_format(m_pResponse, m_SizeOfResponse, "%s I am at start", m_pMessageAuthor);
					return true;
				}
			}
			if(DistX < 5 * 32 && DistY < 5 * 32)
			{
				if(DistX > DistY)
				{
					if(Self.x > Other.x)
					{
						str_format(m_pResponse, m_SizeOfResponse, "%s i am literally next to you (on your right)", m_pMessageAuthor);
						return true;
					}
					else
					{
						str_format(m_pResponse, m_SizeOfResponse, "%s i am literally next to you (on your left)", m_pMessageAuthor);
						return true;
					}
				}
				else
				{
					if(Self.y > Other.y)
					{
						str_format(m_pResponse, m_SizeOfResponse, "%s i am literally next to you (below you)", m_pMessageAuthor);
						return true;
					}
					else
					{
						str_format(m_pResponse, m_SizeOfResponse, "%s i am literally next to you (above you)", m_pMessageAuthor);
						return true;
					}
				}
			}
			else if(DistY > DistX * 8)
			{
				if(Self.y > Other.y)
				{
					str_format(m_pResponse, m_SizeOfResponse, "%s below you (%d tiles)", m_pMessageAuthor, (int)(DistY / 32));
					return true;
				}
				else
				{
					str_format(m_pResponse, m_SizeOfResponse, "%s above you (%d tiles)", m_pMessageAuthor, (int)(DistY / 32));
					return true;
				}
			}
			else if(Self.x > Other.x)
			{
				if(DistY < 15 * 32)
				{
					str_format(m_pResponse, m_SizeOfResponse, "%s i am on your right (%d tiles away)", m_pMessageAuthor, (int)(DistX / 32));
					return true;
				}
				else if(Self.y > Other.y)
				{
					str_format(m_pResponse, m_SizeOfResponse, "%s i am on your right (%d tiles away) and %d tiles below you", m_pMessageAuthor, (int)(DistX / 32), (int)(DistY / 32));
					return true;
				}
				else
				{
					str_format(m_pResponse, m_SizeOfResponse, "%s i am on your right (%d tiles away) and %d tiles above you", m_pMessageAuthor, (int)(DistX / 32), (int)(DistY / 32));
					return true;
				}
			}
			else
			{
				if(DistY < 15 * 32)
				{
					str_format(m_pResponse, m_SizeOfResponse, "%s i am on your left (%d tiles away)", m_pMessageAuthor, (int)(DistX / 32));
					return true;
				}
				else if(Self.y > Other.y)
				{
					str_format(m_pResponse, m_SizeOfResponse, "%s i am on your left (%d tiles away) and %d tiles below you", m_pMessageAuthor, (int)(DistX / 32), (int)(DistY / 32));
					return true;
				}
				else
				{
					str_format(m_pResponse, m_SizeOfResponse, "%s i am on your left (%d tiles away) and %d tiles above you", m_pMessageAuthor, (int)(DistX / 32), (int)(DistY / 32));
					return true;
				}
			}
		}
		if(GameClient()->m_Snap.m_pLocalCharacter && GameClient()->RaceHelper()->IsNearFinish(GameClient()->m_PredictedChar.m_Pos, 32))
			str_format(m_pResponse, m_SizeOfResponse, "%s I am at the finish line", m_pMessageAuthor);
		else if(GameClient()->m_Snap.m_pLocalCharacter && GameClient()->RaceHelper()->IsNearStart(GameClient()->m_PredictedChar.m_Pos, 32))
			str_format(m_pResponse, m_SizeOfResponse, "%s I am at start", m_pMessageAuthor);
		else
			str_format(m_pResponse, m_SizeOfResponse, "%s no idea. Where are you?", m_pMessageAuthor);
		return true;
	}
	return false;
}
