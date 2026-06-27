// ChillerDragon 2020 - chillerbot ux

#include "chillerbotux.h"

#include <base/color.h>
#include <base/log.h>
#include <base/secure.h>
#include <base/time.h>

#include <engine/client/notifications.h>
#include <engine/config.h>
#include <engine/console.h>
#include <engine/engine.h>
#include <engine/graphics.h>
#include <engine/keys.h>
#include <engine/shared/chillerbot/ddnet-custom-clients/custom_clients_ids.h>
#include <engine/shared/config.h>
#include <engine/shared/http.h>
#include <engine/shared/json.h>
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
#include <game/client/components/countryflags.h>
#include <game/client/components/menus.h>
#include <game/client/components/voting.h>
#include <game/client/gameclient.h>
#include <game/client/prediction/entities/character.h>
#include <game/client/race.h>
#include <game/client/render.h>
#include <game/mapitems.h>
#include <game/version.h>

#include <optional>

static const char *HttpStateToStr(EHttpState State)
{
	switch(State)
	{
	case EHttpState::ERROR: return "ERROR";
	case EHttpState::QUEUED: return "QUEUED";
	case EHttpState::RUNNING: return "RUNNING";
	case EHttpState::DONE: return "DONE";
	case EHttpState::ABORTED: return "ABORTED";
	default: return "UNKNOWN";
	}
	return "UNKNOWN";
}

// OMG MERGING DDNET IS ANNOYING

// TODO: use MapScreenToWorld instead lol
void CChillerBotUX::MapScreenToGroup(float CenterX, float CenterY, CMapItemGroup *pGroup, float Zoom)
{
	float ParallaxZoom = std::clamp((double)(std::max(pGroup->m_ParallaxX, pGroup->m_ParallaxY)), 0., 100.);
	float aPoints[4];
	Graphics()->MapScreenToWorld(CenterX, CenterY, pGroup->m_ParallaxX, pGroup->m_ParallaxY, ParallaxZoom,
		pGroup->m_OffsetX, pGroup->m_OffsetY, Graphics()->ScreenAspect(), Zoom, aPoints);
	Graphics()->MapScreen(aPoints[0], aPoints[1], aPoints[2], aPoints[3]);
}

void CChillerBotUX::OnRender()
{
	if(time_get() % 10 == 0)
	{
		CheckEmptyTick();
		SendPlayTimeTick();
		SkinStealTick();
		// if tabbing into tw and going afk set to inactive again over time
		if(m_AfkActivity && time_get() % 100 == 0)
			m_AfkActivity--;
	}
	if(m_AfkTill)
	{
		char aBuf[128];
		str_format(aBuf, sizeof(aBuf), "(%d/%d)", m_AfkActivity, 200);
		SetComponentNoteShort("afk", aBuf);
	}
	RenderSpeedHud();
	RenderEnabledComponents();
	RenderWeaponHud();
	FinishRenameTick();
	ChangeTileNotifyTick();
	TraceSpikes();
	m_ForceDir = 0;
	CampHackTick();
	RenderDbgIntersect();
	if(!m_ForceDir && m_LastForceDir)
	{
		GameClient()->m_Controls.m_aInputDirectionRight[g_Config.m_ClDummy] = 0;
		GameClient()->m_Controls.m_aInputDirectionLeft[g_Config.m_ClDummy] = 0;
		if(GameClient()->m_VibeBot.IsVibing(g_Config.m_ClDummy))
		{
			GameClient()->m_Controls.m_aInputData[g_Config.m_ClDummy].m_Direction = 0;
			GameClient()->m_VibeBot.m_aInputData[g_Config.m_ClDummy].m_Direction = 0;
		}
	}
	m_LastForceDir = m_ForceDir;
}

bool CChillerBotUX::OnSendChat(int Team, const char *pLine)
{
	char aTrimmedLine[512];
	str_copy(aTrimmedLine, pLine);

	int Length = 0;
	char *p = aTrimmedLine;
	char *pEnd = nullptr;
	while(*p)
	{
		char *pStrOld = p;
		int Code = str_utf8_decode((const char **)(&p));

		// check if unicode is not empty
		if(!str_utf8_isspace(Code))
		{
			pEnd = nullptr;
		}
		else if(pEnd == nullptr)
			pEnd = pStrOld;

		if(++Length >= 256)
		{
			*p = '\0';
			break;
		}
	}
	if(pEnd != nullptr)
		*pEnd = '\0';

	ReturnFromAfk(aTrimmedLine);

	int ClientId = GameClient()->m_aLocalIds[g_Config.m_ClDummy];
	if(GameClient()->m_ChatCommand.OnChatMsg(ClientId, Team, aTrimmedLine))
	{
		if(g_Config.m_ClSilentChatCommands)
			return false;
	}

	return true;
}

bool CChillerBotUX::OnSnapInput(bool WouldSend, CNetObj_PlayerInput *pInput)
{
#if defined(CONF_CURSES_CLIENT)
	WouldSend = GameClient()->m_TerminalUI.OnSnapInput(WouldSend, pInput);
#endif

	// register your chillerbot-ux component below!

	return WouldSend;
}

void CChillerBotUX::OnStateChange(int NewState, int OldState)
{
	if(NewState == IClient::STATE_OFFLINE && GameClient()->Client()->ReconnectTime() == 0)
	{
		if(g_Config.m_ClAlwaysReconnect)
			GameClient()->Client()->SetReconnectTime(time_get() + time_freq() * g_Config.m_ClReconnectTimeout + 10);
	}
}

void CChillerBotUX::OnReset()
{
	for(auto &ClientData : m_aClientData)
	{
		ClientData.Reset();
	}
}

int CChillerBotUX::GetPlayTimeHours() const
{
	if(m_PlaytimeMinutes == -1)
		return 0;
	return m_PlaytimeMinutes / 60;
}

// function originally from Kaizo Client by +KZ, credit if used
int CChillerBotUX::InsertCustomClientIdIntoSkinColor(int Color)
{
	if(!g_Config.m_ClSendClientType)
	{
		return Color;
	}

	union
	{
		int m_C = 0;
		unsigned char m_B[4];
	} Temp;

	Temp.m_C = Color;

	//alpha is unused
	Temp.m_B[3] = (unsigned char)CCID_COLOR_BODY_CHILLERBOTUX;
	Color = Temp.m_C;

	return Color;
}

// function originally from Kaizo Network by +KZ, credit if used
bool CChillerBotUX::IsCustomClientId(int Country)
{
	return (size_t)Country > GameClient()->m_CountryFlags.Num();
}

// code originally from Kaizo Network by +KZ, credit if used
int CChillerBotUX::HandleClientCountry(int Country, int ClientId)
{
	if(IsCustomClientId(Country)) //if it is a custom client id, set custom client id and keep country
	{
		m_aClientData[ClientId].m_CustomClient = Country;
		return GameClient()->m_aClients[ClientId].m_Country;
	}
	else //otherwise, set country
	{
		return Country;
	}
}

void CChillerBotUX::HandleNewSnapshot(const IClient::CSnapItem *pItem)
{
	if(pItem->m_Type == NETOBJTYPE_CLIENTINFO)
	{
		const CNetObj_ClientInfo *pInfo = (const CNetObj_ClientInfo *)pItem->m_pData;
		int ClientId = pItem->m_Id;
		if(ClientId < MAX_CLIENTS && ClientId >= 0)
		{
			CChillerClientData *pClient = &m_aClientData[ClientId];

			// identify clients
			// code originally from Kaizo Client by +KZ, credit if used
			union
			{
				int m_C = 0;
				unsigned char m_B[4];
			} Temp;

			Temp.m_C = pInfo->m_ColorBody;

			if(Temp.m_B[3] == CCID_COLOR_BODY_KAIZO_CLIENT)
			{
				pClient->m_CustomClient = CUSTOM_CLIENT_ID_KAIZO_NETWORK;
			}
			else if(Temp.m_B[3] == CCID_COLOR_BODY_CHILLERBOTUX)
			{
				pClient->m_CustomClient = CUSTOM_CLIENT_ID_CHILLERBOTUX;
			}
			else if(Temp.m_B[3] == CCID_COLOR_BODY_PDUCKCLIENT)
			{
				pClient->m_CustomClient = CUSTOM_CLIENT_ID_PDUCKCLIENT;
			}
		}
	}
}

void CChillerBotUX::PrintPlaytime()
{
	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), "Hours: %d (tracking_id=%s)", GetPlayTimeHours(), g_Config.m_ClChillerbotId);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillerbot", aBuf);
}

inline bool CChillerBotUX::IsPlayerInfoAvailable(int ClientId) const
{
	const void *pPrevInfo = Client()->SnapFindItem(IClient::SNAP_PREV, NETOBJTYPE_PLAYERINFO, ClientId);
	const void *pInfo = Client()->SnapFindItem(IClient::SNAP_CURRENT, NETOBJTYPE_PLAYERINFO, ClientId);
	return pPrevInfo && pInfo;
}

void CChillerBotUX::SkinStealTick()
{
	if(!g_Config.m_ClSkinStealer)
		return;
	if(m_NextSkinSteal > time_get())
		return;
	if(!GameClient()->m_Snap.m_pLocalCharacter)
		return;

	int LocalClientId = GameClient()->m_Snap.m_LocalClientId;
	for(int ClientId = 0; ClientId < MAX_CLIENTS; ClientId++)
	{
		if(ClientId == LocalClientId || !GameClient()->m_Snap.m_aCharacters[ClientId].m_Active || !IsPlayerInfoAvailable(ClientId))
			continue;

		// only steal close by
		vec2 *pRenderPos = &GameClient()->m_aClients[ClientId].m_RenderPos;
		vec2 Current = vec2(GameClient()->m_Snap.m_pLocalCharacter->m_X, GameClient()->m_Snap.m_pLocalCharacter->m_Y);
		float Dist = distance(*pRenderPos, Current);
		if(Dist > 32 * g_Config.m_ClSkinStealRadius)
			continue;

		str_copy(g_Config.m_ClPlayerSkin, GameClient()->m_aClients[ClientId].m_aSkinName, sizeof(g_Config.m_ClPlayerSkin));
		if(g_Config.m_ClSkinStealColor)
		{
			g_Config.m_ClPlayerUseCustomColor = GameClient()->m_aClients[ClientId].m_UseCustomColor;
			g_Config.m_ClPlayerColorBody = GameClient()->m_aClients[ClientId].m_ColorBody;
			g_Config.m_ClPlayerColorFeet = GameClient()->m_aClients[ClientId].m_ColorFeet;
		}
		GameClient()->SendInfo(false);
		// only steal skin every 10 seconds to not get ratelimited
		m_NextSkinSteal = time_get() + time_freq() * 10;
		dbg_msg("chillerbot", "cl_skin_stealer yoinked skin '%s'", g_Config.m_ClPlayerSkin);
	}
}

void CChillerBotUX::CheckEmptyTick()
{
	if(!g_Config.m_ClReconnectWhenEmpty)
		return;

	static int s_LastPlayerCount = 0;
	int PlayerCount = CountOnlinePlayers();
	if(s_LastPlayerCount > PlayerCount && PlayerCount == 1)
		GameClient()->Client()->Connect(GameClient()->Client()->ConnectAddressString());
	else
		s_LastPlayerCount = CountOnlinePlayers();
}

void CChillerBotUX::SendPlayTimeTick()
{
	if(!g_Config.m_ClSendOnlineTime)
		return;

	if(m_NextHeartbeat < time_get())
	{
		if(m_HeartbeatState == STATE_DONE)
		{
			m_NextHeartbeat = time_get() + time_freq() * 60;
			m_HeartbeatState = STATE_WANTREFRESH;
		}
	}
	if(m_HeartbeatState == STATE_WANTREFRESH)
	{
		char aUrl[1024];
		char aEscaped[128];
		str_copy(aUrl, "https://chillerbot.zillyhuhn.com/api/v1/beat/", sizeof(aUrl));
		EscapeUrl(aEscaped, sizeof(aEscaped), g_Config.m_ClChillerbotId);
		str_append(aUrl, aEscaped, sizeof(aUrl));
		str_append(aUrl, "/ux-" CHILLERBOT_VERSION "/", sizeof(aUrl));
		if(g_Config.m_PlayerName[0])
			EscapeUrl(aEscaped, sizeof(aEscaped), g_Config.m_PlayerName);
		else
			str_copy(aEscaped, "nameless%20tee", sizeof(aEscaped));
		str_append(aUrl, aEscaped, sizeof(aUrl));

		m_pAliveGet = HttpGet(aUrl);
		m_pAliveGet->LogProgress(HTTPLOG::NONE);
		m_pAliveGet->Timeout(CTimeout{10000, 0, 500, 10});
		m_pAliveGet->IpResolve(IPRESOLVE::V4);
		Http()->Run(m_pAliveGet);
		m_HeartbeatState = STATE_REFRESHING;
	}
	else if(m_HeartbeatState == STATE_REFRESHING)
	{
		if(!m_pAliveGet->Done())
		{
			return;
		}
		m_HeartbeatState = STATE_DONE;
		std::shared_ptr<CHttpRequest> pGetServers = nullptr;
		std::swap(m_pAliveGet, pGetServers);

		if(pGetServers->State() != EHttpState::DONE)
		{
			log_error("chillerbot", "failed to hearthbeat (unexpected state: %s)", HttpStateToStr(pGetServers->State()));
			return;
		}

		bool Success = true;
		json_value *pJson = pGetServers->ResultJson();
		Success = Success && pJson;
		if(Success)
		{
			const json_value &Response = *pJson;
			if(Response.type == json_null)
			{
				// no playtime yet
				m_PlaytimeMinutes = -1;
			}
			else if(Response.type == json_integer)
			{
				m_PlaytimeMinutes = Response.u.integer;
			}
		}
		json_value_free(pJson);
		if(!Success)
			log_error("chillerbot", "failed to hearthbeat (no success)");
	}
}

void CChillerBotUX::ChangeTileNotifyTick()
{
	if(!g_Config.m_ClChangeTileNotification)
		return;
	if(!GameClient()->m_Snap.m_pLocalCharacter)
		return;

	float X = GameClient()->m_Snap.m_aCharacters[GameClient()->m_aLocalIds[g_Config.m_ClDummy]].m_Cur.m_X;
	float Y = GameClient()->m_Snap.m_aCharacters[GameClient()->m_aLocalIds[g_Config.m_ClDummy]].m_Cur.m_Y;
	int CurrentTile = Collision()->GetTileIndex(Collision()->GetPureMapIndex(X, Y));
	if(m_LastTile != CurrentTile && m_LastNotification + time_freq() * 10 < time_get())
	{
		IEngineGraphics *pGraphics = ((IEngineGraphics *)Kernel()->RequestInterface<IEngineGraphics>());
		if(pGraphics && !pGraphics->WindowActive() && Graphics())
		{
			Client()->Notify("chillerbot-ux", "current tile changed");
			Graphics()->NotifyWindow();
		}
		m_LastNotification = time_get();
	}
	m_LastTile = CurrentTile;
}

void CChillerBotUX::RenderWeaponHud()
{
	if(!g_Config.m_ClWeaponHud)
		return;
	if(CCharacter *pChar = GameClient()->m_GameWorld.GetCharacterById(GameClient()->m_aLocalIds[g_Config.m_ClDummy]))
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

		SetComponentNoteLong("weapon hud", aWeapons);
	}
}

void CChillerBotUX::RenderSpeedHud()
{
	if(!g_Config.m_ClShowSpeed || !GameClient()->m_Snap.m_pLocalCharacter || !GameClient()->m_Snap.m_pLocalPrevCharacter)
		return;

	float Width = 300 * Graphics()->ScreenAspect();
	Graphics()->MapScreen(0, 0, Width, 300);
	// float Velspeed = length(vec2(GameClient()->m_Snap.m_pLocalCharacter->m_VelX / 256.0f, GameClient()->m_Snap.m_pLocalCharacter->m_VelY / 256.0f)) * 50;

	const char *paStrings[] = {"Vel X:", "Vel Y:"};
	const int Num = sizeof(paStrings) / sizeof(char *);
	const float LineHeight = 12.0f;
	const float Fontsize = 15.0f;

	static int s_LastVelX = 0;
	static int s_LastVelXChange = 0;
	int CurVelX = abs(GameClient()->m_Snap.m_pLocalCharacter->m_VelX);
	if(s_LastVelX < CurVelX)
		s_LastVelXChange = 1;
	else if(s_LastVelX > CurVelX)
		s_LastVelXChange = -1;
	s_LastVelX = CurVelX;

	static int s_LastVelY = 0;
	static int s_LastVelYChange = 0;
	int CurVelY = abs(GameClient()->m_Snap.m_pLocalCharacter->m_VelY);
	if(s_LastVelY < CurVelY)
		s_LastVelYChange = 1;
	else if(s_LastVelY > CurVelY)
		s_LastVelYChange = -1;
	s_LastVelY = CurVelY;

	float x = Width - 100.0f, y = 50.0f;
	for(int i = 0; i < Num; ++i)
		TextRender()->Text(x, y + i * LineHeight, Fontsize, paStrings[i], -1.0f);

	x = Width - 10.0f;
	char aBuf[128];

	if(s_LastVelXChange == 1)
		TextRender()->TextColor(0.0f, 1.0f, 0.0f, 1.0f);
	else if(s_LastVelXChange == -1)
		TextRender()->TextColor(1.0f, 0.0f, 0.0f, 1.0f);

	str_format(aBuf, sizeof(aBuf), "%.0f", GameClient()->m_Snap.m_pLocalCharacter->m_VelX / 32.f);
	float w = TextRender()->TextWidth(Fontsize, aBuf, -1, -1.0f);
	TextRender()->Text(x - w, y, Fontsize, aBuf, -1.0f);
	y += LineHeight;

	if(s_LastVelYChange == 1)
		TextRender()->TextColor(0.0f, 1.0f, 0.0f, 1.0f);
	else if(s_LastVelYChange == -1)
		TextRender()->TextColor(1.0f, 0.0f, 0.0f, 1.0f);

	str_format(aBuf, sizeof(aBuf), "%.0f", GameClient()->m_Snap.m_pLocalCharacter->m_VelY / 32.f);
	w = TextRender()->TextWidth(Fontsize, aBuf, -1, -1.0f);
	TextRender()->Text(x - w, y, Fontsize, aBuf, -1.0f);
	TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void CChillerBotUX::RenderEnabledComponents()
{
	if(GameClient()->m_Menus.IsActive())
		return;
	if(GameClient()->m_Voting.IsVoting())
		return;
	if(GameClient()->m_GameConsole.IsActive())
		return;
	if(!g_Config.m_ClChillerbotHud)
		return;
	int Offset = 0;
	if(m_IsLeftSidedBroadcast && Client()->GameTick(g_Config.m_ClDummy) < m_BroadcastTick)
		Offset = 2;
	for(auto &EnabledComponent : m_aEnabledComponents)
	{
		if(EnabledComponent.m_aName[0] == '\0')
			continue;
		float TwName = TextRender()->TextWidth(10.0f, EnabledComponent.m_aName, -1, -1);
		float TwNoteShort = 2.f;
		if(EnabledComponent.m_aNoteShort[0])
			TwNoteShort += TextRender()->TextWidth(10.0f, EnabledComponent.m_aNoteShort, -1, -1);
		Graphics()->DrawRect(4.0f, 60.f + Offset * 15, TwName + TwNoteShort, 14.0f, ColorRGBA(0, 0, 0, 0.5f), IGraphics::CORNER_ALL, 3.0f);

		TextRender()->Text(5.0f, 60.f + Offset * 15, 10.0f, EnabledComponent.m_aName, -1);
		TextRender()->Text(5.0f + TwName + 2.f, 60.f + Offset * 15, 10.0f, EnabledComponent.m_aNoteShort, -1);
		TextRender()->Text(5.0f, 60.f + Offset * 15 + 10, 4.0f, EnabledComponent.m_aNoteLong, -1);
		Offset++;
	}
}

void CChillerBotUX::EnableComponent(const char *pComponent, const char *pNoteShort, const char *pNoteLong)
{
	// update
	for(auto &Component : m_aEnabledComponents)
	{
		if(str_comp(Component.m_aName, pComponent))
			continue;
		str_copy(Component.m_aName, pComponent, sizeof(Component.m_aName));
		if(pNoteShort)
			str_copy(Component.m_aNoteShort, pNoteShort, sizeof(Component.m_aNoteShort));
		if(pNoteLong)
			str_copy(Component.m_aNoteLong, pNoteLong, sizeof(Component.m_aNoteLong));
		return;
	}
	// insert
	for(auto &Component : m_aEnabledComponents)
	{
		if(Component.m_aName[0] != '\0')
			continue;
		str_copy(Component.m_aName, pComponent, sizeof(Component.m_aName));
		Component.m_aNoteShort[0] = '\0';
		Component.m_aNoteLong[0] = '\0';
		if(pNoteShort)
			str_copy(Component.m_aNoteShort, pNoteShort, sizeof(Component.m_aNoteShort));
		if(pNoteLong)
			str_copy(Component.m_aNoteLong, pNoteLong, sizeof(Component.m_aNoteLong));
		return;
	}
}

void CChillerBotUX::DisableComponent(const char *pComponent)
{
	// update
	for(auto &Component : m_aEnabledComponents)
	{
		if(str_comp(Component.m_aName, pComponent))
			continue;
		Component.m_aName[0] = '\0';
		return;
	}
}

bool CChillerBotUX::SetComponentNoteShort(const char *pComponent, const char *pNote)
{
	if(!pNote)
		return false;
	for(auto &Component : m_aEnabledComponents)
	{
		if(str_comp(Component.m_aName, pComponent))
			continue;
		str_copy(Component.m_aNoteShort, pNote, sizeof(Component.m_aNoteShort));
		return true;
	}
	return false;
}

bool CChillerBotUX::SetComponentNoteLong(const char *pComponent, const char *pNote)
{
	if(!pNote)
		return false;
	for(auto &Component : m_aEnabledComponents)
	{
		if(str_comp(Component.m_aName, pComponent))
			continue;
		str_copy(Component.m_aNoteLong, pNote, sizeof(Component.m_aNoteLong));
		return true;
	}
	return false;
}

void CChillerBotUX::RenderDbgIntersect()
{
	if(!Config()->m_ClDbgIntersect)
		return;

	vec2 Position = GameClient()->m_aClients[GameClient()->m_Snap.m_LocalClientId].m_RenderPos;
	float Angle = 0.0f;
	if(Client()->State() != IClient::STATE_DEMOPLAYBACK)
	{
		// just use the direct input if it's the local player we are rendering
		Angle = angle(GameClient()->m_Controls.m_aMousePos[g_Config.m_ClDummy]);
	}
	vec2 Direction = direction(Angle);
	vec2 ExDirection = Direction;

	if(Client()->State() != IClient::STATE_DEMOPLAYBACK)
	{
		ExDirection = normalize(vec2((int)GameClient()->m_Controls.m_aMousePos[g_Config.m_ClDummy].x, (int)GameClient()->m_Controls.m_aMousePos[g_Config.m_ClDummy].y));

		// fix direction if mouse is exactly in the center
		if(!(int)GameClient()->m_Controls.m_aMousePos[g_Config.m_ClDummy].x && !(int)GameClient()->m_Controls.m_aMousePos[g_Config.m_ClDummy].y)
			ExDirection = vec2(1, 0);
	}
	vec2 InitPos = Position;
	vec2 FinishPos = InitPos + ExDirection * (GameClient()->m_aTuning[g_Config.m_ClDummy].m_HookLength - 42.0f);

	vec2 OutCol;
	vec2 OutBeforeCol;
	ColorRGBA Color = ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);

	float ScreenX0, ScreenY0, ScreenX1, ScreenY1;
	Graphics()->GetScreen(&ScreenX0, &ScreenY0, &ScreenX1, &ScreenY1);
	MapScreenToGroup(GameClient()->m_Camera.m_Center.x, GameClient()->m_Camera.m_Center.y, Layers()->GameGroup(), GameClient()->m_Camera.m_Zoom);

	if(Collision()->IntersectLine(InitPos, FinishPos, &OutCol, &OutBeforeCol))
	{
		Graphics()->TextureClear();
		Color = ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
		Graphics()->SetColor(Color);
		Graphics()->DrawRect(OutCol.x, OutCol.y, 10, 10, ColorRGBA(0.0f, 0.0f, 1.0f, 0.5f), IGraphics::CORNER_ALL, 3.0f);
		Graphics()->DrawRect(OutBeforeCol.x, OutBeforeCol.y, 10, 10, ColorRGBA(0.0f, 1.0f, 0.0f, 0.5f), IGraphics::CORNER_ALL, 3.0f);
	}
	else
	{
		Graphics()->DrawRect(OutCol.x, OutCol.y, 10, 10, ColorRGBA(0.0f, 0.0f, 1.0f, 0.5f), IGraphics::CORNER_ALL, 3.0f);
		Graphics()->DrawRect(OutBeforeCol.x, OutBeforeCol.y, 10, 10, ColorRGBA(0.0f, 1.0f, 0.0f, 0.5f), IGraphics::CORNER_ALL, 3.0f);
	}

	Graphics()->TextureClear();
	Graphics()->LinesBegin();
	Graphics()->SetColor(Color);
	IGraphics::CLineItem LineItem(InitPos.x, InitPos.y, FinishPos.x, FinishPos.y);
	Graphics()->LinesDraw(&LineItem, 1);
	Graphics()->LinesEnd();

	Graphics()->MapScreen(ScreenX0, ScreenY0, ScreenX1, ScreenY1);
}

void CChillerBotUX::CampHackTick()
{
	if(!GameClient()->m_Snap.m_pLocalCharacter)
		return;
	if(!g_Config.m_ClCampHack)
		return;
	if(Layers()->GameGroup())
	{
		float ScreenX0, ScreenY0, ScreenX1, ScreenY1;
		Graphics()->GetScreen(&ScreenX0, &ScreenY0, &ScreenX1, &ScreenY1);
		MapScreenToGroup(GameClient()->m_Camera.m_Center.x, GameClient()->m_Camera.m_Center.y, Layers()->GameGroup(), GameClient()->m_Camera.m_Zoom);
		Graphics()->DrawRect(m_CampHackX1, m_CampHackY1, 20.0f, 20.0f, ColorRGBA(0, 0, 0, 0.4f), IGraphics::CORNER_ALL, 3.0f);
		Graphics()->DrawRect(m_CampHackX2, m_CampHackY2, 20.0f, 20.0f, ColorRGBA(0, 0, 0, 0.4f), IGraphics::CORNER_ALL, 3.0f);
		if(m_CampHackX1 && m_CampHackX2 && m_CampHackY1 && m_CampHackY2)
		{
			if(m_CampHackX1 < m_CampHackX2)
				Graphics()->DrawRect(m_CampHackX1, m_CampHackY1, m_CampHackX2 - m_CampHackX1, m_CampHackY2 - m_CampHackY1, ColorRGBA(0, 1, 0, 0.2f), IGraphics::CORNER_ALL, 3.0f);
			else
				Graphics()->DrawRect(m_CampHackX1, m_CampHackY1, m_CampHackX2 - m_CampHackX1, m_CampHackY2 - m_CampHackY1, ColorRGBA(1, 0, 0, 0.2f), IGraphics::CORNER_ALL, 3.0f);
		}
		TextRender()->Text(m_CampHackX1, m_CampHackY1, 10.0f, "1", -1);
		TextRender()->Text(m_CampHackX2, m_CampHackY2, 10.0f, "2", -1);
		Graphics()->MapScreen(ScreenX0, ScreenY0, ScreenX1, ScreenY1);
	}
	if(!m_CampHackX1 || !m_CampHackX2 || !m_CampHackY1 || !m_CampHackY2)
		return;
	if(g_Config.m_ClCampHack < 2 || GameClient()->m_Snap.m_pLocalCharacter->m_Weapon != WEAPON_HAMMER)
		return;
	if(m_CampHackX1 > GameClient()->m_Snap.m_pLocalCharacter->m_X)
	{
		GameClient()->m_Controls.m_aInputDirectionRight[g_Config.m_ClDummy] = 1;
		GameClient()->m_Controls.m_aInputDirectionLeft[g_Config.m_ClDummy] = 0;
		m_ForceDir = 1;
		if(GameClient()->m_VibeBot.IsVibing(g_Config.m_ClDummy))
		{
			GameClient()->m_Controls.m_aInputData[g_Config.m_ClDummy].m_Direction = 1;
			GameClient()->m_VibeBot.m_aInputData[g_Config.m_ClDummy].m_Direction = 1;
		}
	}
	else if(m_CampHackX2 < GameClient()->m_Snap.m_pLocalCharacter->m_X)
	{
		GameClient()->m_Controls.m_aInputDirectionRight[g_Config.m_ClDummy] = 0;
		GameClient()->m_Controls.m_aInputDirectionLeft[g_Config.m_ClDummy] = 1;
		m_ForceDir = -1;
		if(GameClient()->m_VibeBot.IsVibing(g_Config.m_ClDummy))
		{
			GameClient()->m_Controls.m_aInputData[g_Config.m_ClDummy].m_Direction = -1;
			GameClient()->m_VibeBot.m_aInputData[g_Config.m_ClDummy].m_Direction = -1;
		}
	}
}

bool CChillerBotUX::OnCursorMove(float x, float y, IInput::ECursorType CursorType)
{
	if(time_get() % 10 == 0)
		ReturnFromAfk();
	return false;
}

bool CChillerBotUX::OnInput(const IInput::CEvent &Event)
{
	ReturnFromAfk();
	SelectCampArea(Event.m_Key);

	if(Event.m_Flags & IInput::FLAG_PRESS && Event.m_Key == KEY_ESCAPE)
	{
		if(g_Config.m_ClReleaseMouse)
		{
			if(m_MouseModeAbs)
				Input()->MouseModeRelative();
			else
				Input()->MouseModeAbsolute();
			m_MouseModeAbs = !m_MouseModeAbs;
		}
	}

	return false;
}

void CChillerBotUX::SelectCampArea(int Key)
{
	if(!GameClient()->m_Snap.m_pLocalCharacter)
		return;
	if(g_Config.m_ClCampHack != 1)
		return;
	if(Key != KEY_MOUSE_1)
		return;
	if(GameClient()->m_Snap.m_pLocalCharacter->m_Weapon != WEAPON_GUN)
		return;
	m_CampClick++;
	if(m_CampClick % 2 == 0)
	{
		// UNSET IF CLOSE
		vec2 Current = vec2(GameClient()->m_Snap.m_pLocalCharacter->m_X, GameClient()->m_Snap.m_pLocalCharacter->m_Y);
		vec2 CrackPos1 = vec2(m_CampHackX1, m_CampHackY1);
		float Dist = distance(CrackPos1, Current);
		if(Dist < 100.0f)
		{
			m_CampHackX1 = 0;
			m_CampHackY1 = 0;
			GameClient()->m_Chat.AddLine(-2, 0, "Unset camp[1]");
			return;
		}
		vec2 CrackPos2 = vec2(m_CampHackX2, m_CampHackY2);
		Dist = distance(CrackPos2, Current);
		if(Dist < 100.0f)
		{
			m_CampHackX2 = 0;
			m_CampHackY2 = 0;
			GameClient()->m_Chat.AddLine(-2, 0, "Unset camp[2]");
			return;
		}
		// SET OTHERWISE
		if(m_CampClick == 2)
		{
			m_CampHackX1 = GameClient()->m_Snap.m_pLocalCharacter->m_X;
			m_CampHackY1 = GameClient()->m_Snap.m_pLocalCharacter->m_Y;
		}
		else
		{
			m_CampHackX2 = GameClient()->m_Snap.m_pLocalCharacter->m_X;
			m_CampHackY2 = GameClient()->m_Snap.m_pLocalCharacter->m_Y;
		}
		char aBuf[128];
		str_format(aBuf, sizeof(aBuf),
			"Set camp[%d] %d",
			m_CampClick == 2 ? 1 : 2,
			GameClient()->m_Snap.m_pLocalCharacter->m_X / 32);
		GameClient()->m_Chat.AddLine(-2, 0, aBuf);
	}
	if(m_CampClick > 3)
		m_CampClick = 0;
}

void CChillerBotUX::FinishRenameTick()
{
	if(!GameClient()->m_Snap.m_pLocalCharacter)
		return;
	if(!g_Config.m_ClFinishRename)
		return;
	vec2 Pos = GameClient()->m_PredictedChar.m_Pos;
	if(GameClient()->RaceHelper()->IsNearFinish(Pos))
	{
		if(Client()->State() == IClient::STATE_ONLINE && !GameClient()->m_Menus.IsActive() && g_Config.m_ClEditor == 0)
		{
			Graphics()->DrawRect(10.0f, 30.0f, 150.0f, 50.0f, ColorRGBA(0, 0, 0, 0.5f), IGraphics::CORNER_ALL, 10.0f);
			TextRender()->Text(20.0f, 30.f, 20.0f, "chillerbot-ux", -1);
			TextRender()->Text(50.0f, 60.f, 10.0f, "finish rename", -1);
		}
		if(!m_IsNearFinish)
		{
			m_IsNearFinish = true;
			GameClient()->SendFinishName();
		}
	}
	else
	{
		m_IsNearFinish = false;
	}
}

void CChillerBotUX::OnInit()
{
	m_pChatHelper = &GameClient()->m_ChatHelper;

	m_AfkTill = 0;
	m_AfkActivity = 0;
	m_aAfkMessage[0] = '\0';

	for(auto &c : m_aEnabledComponents)
	{
		c.m_aName[0] = '\0';
		c.m_aNoteShort[0] = '\0';
		c.m_aNoteLong[0] = '\0';
	}
	UpdateComponents();
	m_aLastKiller[0][0] = '\0';
	m_aLastKiller[1][0] = '\0';
	m_aLastKillerTime[0][0] = '\0';
	m_aLastKillerTime[1][0] = '\0';
	m_BroadcastTick = 0;
	m_IsLeftSidedBroadcast = false;
	m_HeartbeatState = STATE_WANTREFRESH;
	m_NextHeartbeat = 0;
	m_PlaytimeMinutes = -1;
	// TODO: replace this with priv pub key pairs otherwise account ownership claims are trash
	if(!g_Config.m_ClChillerbotId[0])
		secure_random_password(g_Config.m_ClChillerbotId, sizeof(g_Config.m_ClChillerbotId), 16);
	m_NextSkinSteal = 0;
	m_LastTile = -1;
}

void CChillerBotUX::OnShutdown()
{
	RestoreSkins();
}

void CChillerBotUX::UpdateComponents()
{
	// TODO: make this auto or nicer
	if(g_Config.m_ClChillerbotHud)
		EnableComponent("chillerbot hud");
	else
		DisableComponent("chillerbot hud");
	if(g_Config.m_ClCampHack)
		EnableComponent("camp hack");
	else
		DisableComponent("camp hack");
	if(g_Config.m_ClAutoReply)
		EnableComponent("auto reply");
	else
		DisableComponent("auto reply");
	if(g_Config.m_ClFinishRename)
		EnableComponent("finish rename");
	else
		DisableComponent("finish rename");
	if(g_Config.m_ClWarList)
		EnableComponent("war list");
	else
		DisableComponent("war list");
	if(g_Config.m_ClShowLastKiller)
		EnableComponent("last killer");
	else
		DisableComponent("last killer");
	if(g_Config.m_ClShowLastPing)
		EnableComponent("last ping");
	else
		DisableComponent("last ping");
	if(g_Config.m_ClShowWallet)
		EnableComponent("money");
	else
		DisableComponent("money");
	if(g_Config.m_ClWeaponHud)
		EnableComponent("weapon hud");
	else
		DisableComponent("weapon hud");
	if(g_Config.m_ClSkinStealer)
		EnableComponent("skin stealer");
	else
		DisableComponent("skin stealer");
}

void CChillerBotUX::OnConsoleInit()
{
	Console()->Register("playtime", "", CFGFLAG_CLIENT, ConPlaytime, this, "Get your time spent in this chillerbot-ux (cl_chillerbot_id, cl_send_online_time)");
	Console()->Register("afk", "?i[minutes]?r[message]", CFGFLAG_CLIENT, ConAfk, this, "Activate afk mode (auto chat respond)");
	Console()->Register("camp", "?i[left]i[right]?s[tile|raw]", CFGFLAG_CLIENT, ConCampHack, this, "Activate camp mode relative to tee");
	Console()->Register("camp_abs", "i[x1]i[y1]i[x2]i[y2]?s[tile|raw]", CFGFLAG_CLIENT, ConCampHackAbs, this, "Activate camp mode absolute in the map");
	Console()->Register("uncamp", "", CFGFLAG_CLIENT, ConUnCampHack, this, "Same as cl_camp_hack 0 but resets walk input");
	Console()->Register("load_map", "s[file]", CFGFLAG_CLIENT, ConLoadMap, this, "Load mapfile");
	Console()->Register("dump_players", "?s[search]", CFGFLAG_CLIENT, ConDumpPlayers, this, "Prints players to console");
	Console()->Register("force_quit", "", CFGFLAG_CLIENT, ConForceQuit, this, "Forces a dirty client quit all data will be lost");

	Console()->Chain("cl_camp_hack", ConchainCampHack, this);
	Console()->Chain("cl_chillerbot_hud", ConchainChillerbotHud, this);
	Console()->Chain("cl_weapon_hud", ConchainWeaponHud, this);
	Console()->Chain("cl_auto_reply", ConchainAutoReply, this);
	Console()->Chain("cl_finish_rename", ConchainFinishRename, this);
	Console()->Chain("cl_show_last_killer", ConchainShowLastKiller, this);
	Console()->Chain("cl_show_last_ping", ConchainShowLastPing, this);
	Console()->Chain("cl_skin_stealer", ConchainSkinStealer, this);
}

void CChillerBotUX::ConForceQuit(IConsole::IResult *pResult, void *pUserData)
{
	exit(0);
}

void CChillerBotUX::ConDumpPlayers(IConsole::IResult *pResult, void *pUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	pSelf->DumpPlayers(pResult->GetString(0));
}

void CChillerBotUX::ConchainCampHack(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->GetInteger(0))
		pSelf->EnableComponent("camp hack");
	else
		pSelf->DisableComponent("camp hack");
}

void CChillerBotUX::ConchainChillerbotHud(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->GetInteger(0))
		pSelf->EnableComponent("chillerbot hud");
	else
		pSelf->DisableComponent("chillerbot hud");
	pSelf->UpdateComponents();
}

void CChillerBotUX::ConchainWeaponHud(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->GetInteger(0))
		pSelf->EnableComponent("weapon hud");
	else
		pSelf->DisableComponent("weapon hud");
	pSelf->UpdateComponents();
}

void CChillerBotUX::ConchainAutoReply(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->GetInteger(0))
		pSelf->EnableComponent("auto reply");
	else
		pSelf->DisableComponent("auto reply");
}

void CChillerBotUX::ConchainFinishRename(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->GetInteger(0))
		pSelf->EnableComponent("finish rename");
	else
		pSelf->DisableComponent("finish rename");
}

void CChillerBotUX::ConchainShowLastKiller(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->GetInteger(0))
		pSelf->EnableComponent("last killer");
	else
		pSelf->DisableComponent("last killer");
}

void CChillerBotUX::ConchainShowLastPing(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->GetInteger(0))
		pSelf->EnableComponent("last ping");
	else
		pSelf->DisableComponent("last ping");
}

void CChillerBotUX::ConchainSkinStealer(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	if(pResult->GetInteger(0) == g_Config.m_ClSkinStealer)
	{
		dbg_msg("chillerbot", "skin stealer is already %s", g_Config.m_ClSkinStealer ? "on" : "off");
		return;
	}
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->NumArguments() == 0)
		return;
	if(pResult->GetInteger(0))
	{
		pSelf->SaveSkins();
		pSelf->EnableComponent("skin stealer");
	}
	else
	{
		pSelf->RestoreSkins();
		pSelf->GameClient()->SendInfo(false);
		pSelf->DisableComponent("skin stealer");
	}
}

void CChillerBotUX::SaveSkins()
{
	dbg_msg("chillerbot", "saved player skin '%s'", g_Config.m_ClPlayerSkin);
	str_copy(g_Config.m_ClSavedPlayerSkin, g_Config.m_ClPlayerSkin, sizeof(g_Config.m_ClSavedPlayerSkin));
	g_Config.m_ClSavedPlayerUseCustomColor = g_Config.m_ClPlayerUseCustomColor;
	g_Config.m_ClSavedPlayerColorBody = g_Config.m_ClPlayerColorBody;
	g_Config.m_ClSavedPlayerColorFeet = g_Config.m_ClPlayerColorFeet;

	dbg_msg("chillerbot", "saved dummy skin '%s'", g_Config.m_ClDummySkin);
	str_copy(g_Config.m_ClSavedDummySkin, g_Config.m_ClDummySkin, sizeof(g_Config.m_ClSavedDummySkin));
	g_Config.m_ClSavedDummyUseCustomColor = g_Config.m_ClDummyUseCustomColor;
	g_Config.m_ClSavedDummyColorBody = g_Config.m_ClDummyColorBody;
	g_Config.m_ClSavedDummyColorFeet = g_Config.m_ClDummyColorFeet;
}

void CChillerBotUX::RestoreSkins()
{
	dbg_msg("chillerbot", "restored player skin '%s'", g_Config.m_ClSavedPlayerSkin);
	str_copy(g_Config.m_ClPlayerSkin, g_Config.m_ClSavedPlayerSkin, sizeof(g_Config.m_ClPlayerSkin));
	g_Config.m_ClPlayerUseCustomColor = g_Config.m_ClSavedPlayerUseCustomColor;
	g_Config.m_ClPlayerColorBody = g_Config.m_ClSavedPlayerColorBody;
	g_Config.m_ClPlayerColorFeet = g_Config.m_ClSavedPlayerColorFeet;

	dbg_msg("chillerbot", "restored dummy skin '%s'", g_Config.m_ClSavedDummySkin);
	str_copy(g_Config.m_ClDummySkin, g_Config.m_ClSavedDummySkin, sizeof(g_Config.m_ClDummySkin));
	g_Config.m_ClDummyUseCustomColor = g_Config.m_ClSavedDummyUseCustomColor;
	g_Config.m_ClDummyColorBody = g_Config.m_ClSavedDummyColorBody;
	g_Config.m_ClDummyColorFeet = g_Config.m_ClSavedDummyColorFeet;
}

void CChillerBotUX::ConAfk(IConsole::IResult *pResult, void *pUserData)
{
	((CChillerBotUX *)pUserData)->GoAfk(pResult->NumArguments() ? pResult->GetInteger(0) : -1, pResult->GetString(1));
}

void CChillerBotUX::ConPlaytime(IConsole::IResult *pResult, void *pUserData)
{
	((CChillerBotUX *)pUserData)->PrintPlaytime();
}

void CChillerBotUX::ConCampHackAbs(IConsole::IResult *pResult, void *pUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	int Tile = 32;
	if(!str_comp(pResult->GetString(0), "raw"))
		Tile = 1;
	g_Config.m_ClCampHack = 2;
	pSelf->EnableComponent("camp hack");
	// absolute all coords
	if(pResult->NumArguments() > 1)
	{
		if(pSelf->GameClient()->m_Snap.m_pLocalCharacter)
		{
			pSelf->m_CampHackX1 = Tile * pResult->GetInteger(0);
			pSelf->m_CampHackY1 = Tile * pResult->GetInteger(1);
			pSelf->m_CampHackX2 = Tile * pResult->GetInteger(2);
			pSelf->m_CampHackY2 = Tile * pResult->GetInteger(3);
		}
		return;
	}
}

void CChillerBotUX::DumpPlayers(const char *pSearch)
{
	char aBuf[128];
	char aTime[128];
	char aLine[512];
	int OldDDTeam = -1;
	dbg_msg("dump_players", "+----------+--+----------------+----------------+---+-------+");
	dbg_msg("dump_players", "|score     |id|name            |clan            |lat|team   |");
	dbg_msg("dump_players", "+----------+--+----------------+----------------+---+-------+");
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		const CNetObj_PlayerInfo *pInfo = GameClient()->m_Snap.m_apInfoByDDTeamScore[i];
		if(!pInfo)
			continue;

		bool IsMatch = !(pSearch && pSearch[0] != 0);
		aLine[0] = '\0';
		// score
		if(GameClient()->m_GameInfo.m_TimeScore)
		{
			if(pInfo->m_Score == -9999)
				str_format(aBuf, sizeof(aBuf), "|%10s|", " ");
			else
			{
				str_time((int64_t)abs(pInfo->m_Score) * 100, ETimeFormat::HOURS, aTime, sizeof(aTime));
				str_format(aBuf, sizeof(aBuf), "|%10s|", aTime);
			}
		}
		else
			str_format(aBuf, sizeof(aBuf), "|%10d|", std::clamp(pInfo->m_Score, -999, 99999));
		str_append(aLine, aBuf, sizeof(aLine));

		// id | name
		if(pSearch && pSearch[0] != 0)
			if(str_find_nocase(GameClient()->m_aClients[pInfo->m_ClientId].m_aName, pSearch))
				IsMatch = true;
		str_format(aBuf, sizeof(aBuf), "%2d|%16s|", pInfo->m_ClientId, GameClient()->m_aClients[pInfo->m_ClientId].m_aName);
		str_append(aLine, aBuf, sizeof(aLine));

		// clan
		if(pSearch && pSearch[0] != 0)
			if(str_find_nocase(GameClient()->m_aClients[pInfo->m_ClientId].m_aClan, pSearch))
				IsMatch = true;
		str_format(aBuf, sizeof(aBuf), "%16s|", GameClient()->m_aClients[pInfo->m_ClientId].m_aClan);
		str_append(aLine, aBuf, sizeof(aLine));

		// ping
		str_format(aBuf, sizeof(aBuf), "%3d|", std::clamp(pInfo->m_Latency, 0, 999));
		str_append(aLine, aBuf, sizeof(aLine));

		// team
		int DDTeam = GameClient()->m_Teams.Team(pInfo->m_ClientId);
		int NextDDTeam = 0;

		for(int j = i + 1; j < MAX_CLIENTS; j++)
		{
			const CNetObj_PlayerInfo *pInfo2 = GameClient()->m_Snap.m_apInfoByDDTeamScore[j];

			if(!pInfo2)
				continue;

			NextDDTeam = GameClient()->m_Teams.Team(pInfo2->m_ClientId);
			break;
		}

		if(OldDDTeam == -1)
		{
			for(int j = i - 1; j >= 0; j--)
			{
				const CNetObj_PlayerInfo *pInfo2 = GameClient()->m_Snap.m_apInfoByDDTeamScore[j];

				if(!pInfo2)
					continue;

				OldDDTeam = GameClient()->m_Teams.Team(pInfo2->m_ClientId);
				break;
			}
		}

		if(DDTeam != TEAM_FLOCK)
		{
			if(NextDDTeam != DDTeam)
			{
				if(GameClient()->m_Snap.m_aTeamSize[0] > 8)
					str_format(aBuf, sizeof(aBuf), "%7d|", DDTeam);
				else
					str_format(aBuf, sizeof(aBuf), "Team %2d|", DDTeam);
			}
			else
				str_format(aBuf, sizeof(aBuf), "%7s|", " ");
			str_append(aLine, aBuf, sizeof(aLine));
			if(OldDDTeam != DDTeam)
				dbg_msg("dump_players", "+----------+--+----------------+----------------+---+-------/");
			if(NextDDTeam != DDTeam)
				dbg_msg("dump_players", "+----------+--+----------------+----------------+---+-------\\");
		}

		OldDDTeam = DDTeam;
		if(IsMatch)
			dbg_msg("dump_players", "%s", aLine);
	}
	dbg_msg("dump_players", "+----------+--+----------------+----------------+---+-------+");
}

void CChillerBotUX::ConCampHack(IConsole::IResult *pResult, void *pUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	int Tile = 32;
	if(!str_comp(pResult->GetString(0), "raw"))
		Tile = 1;
	g_Config.m_ClCampHack = 2;
	pSelf->EnableComponent("camp hack");
	if(!pResult->NumArguments())
	{
		if(pSelf->GameClient()->m_Snap.m_pLocalCharacter)
		{
			pSelf->m_CampHackX1 = pSelf->GameClient()->m_Snap.m_pLocalCharacter->m_X - 32 * 3;
			pSelf->m_CampHackY1 = pSelf->GameClient()->m_Snap.m_pLocalCharacter->m_Y;
			pSelf->m_CampHackX2 = pSelf->GameClient()->m_Snap.m_pLocalCharacter->m_X + 32 * 3;
			pSelf->m_CampHackY2 = pSelf->GameClient()->m_Snap.m_pLocalCharacter->m_Y;
		}
		return;
	}
	// relative left and right
	if(pResult->NumArguments() > 1)
	{
		if(pSelf->GameClient()->m_Snap.m_pLocalCharacter)
		{
			pSelf->m_CampHackX1 = pSelf->GameClient()->m_Snap.m_pLocalCharacter->m_X - Tile * pResult->GetInteger(0);
			pSelf->m_CampHackY1 = pSelf->GameClient()->m_Snap.m_pLocalCharacter->m_Y;
			pSelf->m_CampHackX2 = pSelf->GameClient()->m_Snap.m_pLocalCharacter->m_X + Tile * pResult->GetInteger(1);
			pSelf->m_CampHackY2 = pSelf->GameClient()->m_Snap.m_pLocalCharacter->m_Y;
		}
		return;
	}
}

void CChillerBotUX::ConUnCampHack(IConsole::IResult *pResult, void *pUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	g_Config.m_ClCampHack = 0;
	pSelf->DisableComponent("camp hack");
	pSelf->GameClient()->m_Controls.m_aInputDirectionRight[g_Config.m_ClDummy] = 0;
	pSelf->GameClient()->m_Controls.m_aInputDirectionLeft[g_Config.m_ClDummy] = 0;
}

void CChillerBotUX::ConLoadMap(IConsole::IResult *pResult, void *pUserData)
{
	CChillerBotUX *pSelf = (CChillerBotUX *)pUserData;
	pSelf->GameClient()->Client()->ChillerBotLoadMap(pResult->GetString(0));
}

void CChillerBotUX::TraceSpikes()
{
	if(!g_Config.m_ClSpikeTracer)
		return;
	if(!GameClient()->m_Snap.m_pLocalCharacter)
		return;

	// int CurrentX = (int)(GameClient()->m_Snap.m_aCharacters[GameClient()->m_aLocalIds[0]].m_Cur.m_X / 32);
	// int CurrentY = (int)(GameClient()->m_Snap.m_aCharacters[GameClient()->m_aLocalIds[0]].m_Cur.m_Y / 32);
	int CurrentX = (int)(GameClient()->m_Snap.m_pLocalCharacter->m_X / 32);
	int CurrentY = (int)(GameClient()->m_Snap.m_pLocalCharacter->m_Y / 32);
	int FromX = std::max(0, CurrentX - g_Config.m_ClSpikeTracer);
	int ToX = std::min(Collision()->GetWidth(), CurrentX + g_Config.m_ClSpikeTracer);
	int FromY = std::max(0, CurrentY - g_Config.m_ClSpikeTracer);
	int ToY = std::min(Collision()->GetHeight(), CurrentY + g_Config.m_ClSpikeTracer);
	float ScreenX0;
	float ScreenX1;
	float ScreenY0;
	float ScreenY1;
	Graphics()->GetScreen(&ScreenX0, &ScreenY0, &ScreenX1, &ScreenY1);
	MapScreenToGroup(GameClient()->m_Camera.m_Center.x, GameClient()->m_Camera.m_Center.y, Layers()->GameGroup(), GameClient()->m_Camera.m_Zoom);
	for(int x = FromX; x < ToX; x++)
	{
		for(int y = FromY; y < ToY; y++)
		{
			int Tile = Collision()->GetIndex(x, y);
			if(Tile == TILE_DEATH)
			{
				Graphics()->TextureClear();
				bool IsIntersect = false;
				if(Collision()->IntersectLine(vec2(x * 32, y * 32), vec2(CurrentX * 32, CurrentY * 32), nullptr, nullptr))
				{
					IsIntersect = true;
					if(!g_Config.m_ClSpikeTracerWalls)
						continue;
				}
				Graphics()->LinesBegin();
				if(IsIntersect)
					Graphics()->SetColor(1.f, 1.f, 1.f, 0.45f);
				else
					Graphics()->SetColor(1.f, 0.f, 0.f, 0.75f);
				IGraphics::CLineItem LineItem(x * 32, y * 32, CurrentX * 32, CurrentY * 32);
				Graphics()->LinesDraw(&LineItem, 1);
				Graphics()->LinesEnd();
			}
		}
	}
	Graphics()->MapScreen(ScreenX0, ScreenY0, ScreenX1, ScreenY1);
}

void CChillerBotUX::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_KILLMSG)
	{
		CNetMsg_Sv_KillMsg *pMsg = (CNetMsg_Sv_KillMsg *)pRawMsg;

		// unpack messages
		CKillMsg Kill;
		Kill.m_aVictimName[0] = '\0';
		Kill.m_aKillerName[0] = '\0';

		Kill.m_VictimId = pMsg->m_Victim;
		if(Kill.m_VictimId >= 0 && Kill.m_VictimId < MAX_CLIENTS)
		{
			Kill.m_VictimTeam = GameClient()->m_aClients[Kill.m_VictimId].m_Team;
			Kill.m_VictimDDTeam = GameClient()->m_Teams.Team(Kill.m_VictimId);
			str_copy(Kill.m_aVictimName, GameClient()->m_aClients[Kill.m_VictimId].m_aName, sizeof(Kill.m_aVictimName));
			Kill.m_VictimRenderInfo = GameClient()->m_aClients[Kill.m_VictimId].m_RenderInfo;
		}

		Kill.m_KillerId = pMsg->m_Killer;
		if(Kill.m_KillerId >= 0 && Kill.m_KillerId < MAX_CLIENTS)
		{
			Kill.m_KillerTeam = GameClient()->m_aClients[Kill.m_KillerId].m_Team;
			str_copy(Kill.m_aKillerName, GameClient()->m_aClients[Kill.m_KillerId].m_aName, sizeof(Kill.m_aKillerName));
			Kill.m_KillerRenderInfo = GameClient()->m_aClients[Kill.m_KillerId].m_RenderInfo;
		}

		Kill.m_Weapon = pMsg->m_Weapon;
		Kill.m_ModeSpecial = pMsg->m_ModeSpecial;
		Kill.m_Tick = Client()->GameTick(g_Config.m_ClDummy);

		Kill.m_FlagCarrierBlue = GameClient()->m_Snap.m_pGameDataObj ? GameClient()->m_Snap.m_pGameDataObj->m_FlagCarrierBlue : -1;

		bool KillMsgValid = (Kill.m_VictimRenderInfo.m_CustomColoredSkin && Kill.m_VictimRenderInfo.m_ColorableRenderSkin.m_Body.IsValid()) || (!Kill.m_VictimRenderInfo.m_CustomColoredSkin && Kill.m_VictimRenderInfo.m_OriginalRenderSkin.m_Body.IsValid());
		// if killer != victim, killer must be valid too
		KillMsgValid &= Kill.m_KillerId == Kill.m_VictimId || ((Kill.m_KillerRenderInfo.m_CustomColoredSkin && Kill.m_KillerRenderInfo.m_ColorableRenderSkin.m_Body.IsValid()) || (!Kill.m_KillerRenderInfo.m_CustomColoredSkin && Kill.m_KillerRenderInfo.m_OriginalRenderSkin.m_Body.IsValid()));
		if(KillMsgValid && Kill.m_KillerId != Kill.m_VictimId)
		{
			for(int i = 0; i < 2; i++)
			{
				if(GameClient()->m_aLocalIds[i] != Kill.m_VictimId)
					continue;

				str_copy(m_aLastKiller[i], Kill.m_aKillerName, sizeof(m_aLastKiller[i]));
				char aBuf[512];
				str_timestamp_format(m_aLastKillerTime[i], sizeof(m_aLastKillerTime[i]), "%H:%M");
				str_format(
					aBuf,
					sizeof(aBuf),
					"[%s]main: %s [%s]dummy: %s",
					m_aLastKillerTime[0],
					m_aLastKiller[0],
					m_aLastKillerTime[1],
					m_aLastKiller[1]);
				SetComponentNoteLong("last killer", aBuf);
			}
		}
	}
	else if(MsgType == NETMSGTYPE_SV_BROADCAST)
	{
		CNetMsg_Sv_Broadcast *pMsg = (CNetMsg_Sv_Broadcast *)pRawMsg;
		str_copy(m_aBroadcastText, pMsg->m_pMessage, sizeof(m_aBroadcastText));
		m_BroadcastTick = Client()->GameTick(g_Config.m_ClDummy) + Client()->GameTickSpeed() * 10;
		m_IsLeftSidedBroadcast = str_find(m_aBroadcastText, "                                ") != nullptr;
	}
	else if(MsgType == NETMSGTYPE_SV_VOTESET)
	{
		if(g_Config.m_ClRunOnVoteStart[0])
			Console()->ExecuteLine(g_Config.m_ClRunOnVoteStart, IConsole::CLIENT_ID_UNSPECIFIED);
	}
	else if(MsgType == NETMSGTYPE_SV_MODIFYTILE)
	{
		CNetMsg_Sv_ModifyTile *pMsg = (CNetMsg_Sv_ModifyTile *)pRawMsg;
		Collision()->ModifyTile(pMsg->m_X, pMsg->m_Y, pMsg->m_Group, pMsg->m_Layer, pMsg->m_Index, pMsg->m_Flags);

		// fixes tiles not being updated if the gpu supports tile buffering
		GameClient()->m_MapLayersForeground.RefreshTileBuffers([&]() {});
		GameClient()->m_MapLayersBackground.RefreshTileBuffers([&]() {});

		// fixes skipped tiles that were empty
		Collision()->Layers()->Init(GameClient()->Map(), false, false);

		// doesnt help
		// Collision()->Init(Layers());
	}
}

void CChillerBotUX::GoAfk(int Minutes, const char *pMsg)
{
	if(pMsg)
	{
		str_copy(m_aAfkMessage, pMsg, sizeof(m_aAfkMessage));
		if((unsigned long)str_length(pMsg) > sizeof(m_aAfkMessage))
		{
			char aBuf[128];
			str_format(aBuf, sizeof(aBuf), "error: afk message too long %d/%" PRIzu, str_length(pMsg), sizeof(m_aAfkMessage));
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
			return;
		}
	}
	m_AfkTill = time_get() + time_freq() * 60 * Minutes;
	m_AfkActivity = 0;
	m_pChatHelper->ClearLastAfkPingMessage();
	EnableComponent("afk");
	EnableComponent("last killer");
	g_Config.m_ClShowLastKiller = 1;
}

void CChillerBotUX::ReturnFromAfk(const char *pChatMessage)
{
	if(!m_AfkTill)
		return;
	if(pChatMessage && pChatMessage[0] != '/')
	{
		if(m_IgnoreChatAfk > 0)
			m_IgnoreChatAfk--;
		else
			m_AfkActivity += 400;
	}
	m_AfkActivity++;
	if(m_AfkActivity < 200)
		return;
	GameClient()->m_Chat.AddLine(-2, 0, "[chillerbot-ux] welcome back :)");
	m_AfkTill = 0;
	DisableComponent("afk");
}

int CChillerBotUX::CountOnlinePlayers()
{
	// Code from scoreboard. There is probably a better way to do this
	int Num = 0;
	for(const auto *pInfo : GameClient()->m_Snap.m_apInfoByDDTeamScore)
		if(pInfo)
			Num++;
	return Num;
}

int CChillerBotUX::GetTotalJumps()
{
	int ClientId = GameClient()->m_aLocalIds[g_Config.m_ClDummy];
	CCharacterCore *pCharacter = &GameClient()->m_aClients[ClientId].m_Predicted;
	if(GameClient()->m_Snap.m_aCharacters[ClientId].m_HasExtendedDisplayInfo)
		return std::max(std::min(abs(pCharacter->m_Jumps), 10), 0);
	else
		return abs(GameClient()->m_Snap.m_aCharacters[ClientId].m_ExtendedData.m_Jumps);
}

int CChillerBotUX::GetUnusedJumps()
{
	int ClientId = GameClient()->m_aLocalIds[g_Config.m_ClDummy];
	CCharacterCore *pCharacter = &GameClient()->m_aClients[ClientId].m_Predicted;
	int TotalJumpsToDisplay = 0, AvailableJumpsToDisplay = 0;
	if(GameClient()->m_Snap.m_aCharacters[ClientId].m_HasExtendedDisplayInfo)
	{
		bool Grounded = false;
		if(Collision()->CheckPoint(pCharacter->m_Pos.x + CCharacterCore::PhysicalSize() / 2,
			   pCharacter->m_Pos.y + CCharacterCore::PhysicalSize() / 2 + 5))
		{
			Grounded = true;
		}
		if(Collision()->CheckPoint(pCharacter->m_Pos.x - CCharacterCore::PhysicalSize() / 2,
			   pCharacter->m_Pos.y + CCharacterCore::PhysicalSize() / 2 + 5))
		{
			Grounded = true;
		}

		int UsedJumps = pCharacter->m_JumpedTotal;
		if(pCharacter->m_Jumps > 1)
		{
			UsedJumps += !Grounded;
		}
		else if(pCharacter->m_Jumps == 1)
		{
			// If the player has only one jump, each jump is the last one
			UsedJumps = pCharacter->m_Jumped & 2;
		}
		else if(pCharacter->m_Jumps == -1)
		{
			// The player has only one ground jump
			UsedJumps = !Grounded;
		}

		if(pCharacter->m_EndlessJump && UsedJumps >= abs(pCharacter->m_Jumps))
		{
			UsedJumps = abs(pCharacter->m_Jumps) - 1;
		}

		int UnusedJumps = abs(pCharacter->m_Jumps) - UsedJumps;
		if(!(pCharacter->m_Jumped & 2) && UnusedJumps <= 0)
		{
			// In some edge cases when the player just got another number of jumps, UnusedJumps is not correct
			UnusedJumps = 1;
		}
		TotalJumpsToDisplay = std::max(std::min(abs(pCharacter->m_Jumps), 10), 0);
		AvailableJumpsToDisplay = std::max(std::min(UnusedJumps, TotalJumpsToDisplay), 0);
	}
	else
	{
		AvailableJumpsToDisplay = abs(GameClient()->m_Snap.m_aCharacters[ClientId].m_ExtendedData.m_Jumps);
	}
	return AvailableJumpsToDisplay;
}

void CChillerBotUX::CChillerClientData::Reset()
{
	m_CustomClient = 0;
}

bool CChillerBotUX::IsOurClientId(int ClientId)
{
	if(ClientId == GameClient()->m_aLocalIds[0])
		return true;
	if(!GameClient()->Client()->DummyConnected())
		return false;
	return ClientId == GameClient()->m_aLocalIds[1];
}

const char *CChillerBotUX::GetClientNameCallback(int ClientId, void *pUser)
{
	CGameClient *pSelf = static_cast<CGameClient *>(pUser);
	if(ClientId < 0 || ClientId >= MAX_CLIENTS)
		return "";
	auto &Client = pSelf->m_aClients[ClientId];
	if(!Client.m_Active)
		return "";
	return Client.m_aName;
}

const char *CChillerBotUX::GetClientClanCallback(int ClientId, void *pUser)
{
	CGameClient *pSelf = static_cast<CGameClient *>(pUser);
	if(ClientId < 0 || ClientId >= MAX_CLIENTS)
		return "";
	auto &Client = pSelf->m_aClients[ClientId];
	if(!Client.m_Active)
		return "";
	return Client.m_aClan;
}

CChillerBotReplyTee CChillerBotUX::GetClientCallback(int ClientId, void *pUser)
{
	CGameClient *pSelf = static_cast<CGameClient *>(pUser);
	if(ClientId < 0 || ClientId >= MAX_CLIENTS)
		return {.m_Active = false};
	auto &Client = pSelf->m_aClients[ClientId];
	if(!Client.m_Active)
		return {.m_Active = false};

	std::optional<float> PosX = std::nullopt;
	std::optional<float> PosY = std::nullopt;
	const CNetObj_PlayerInfo *pInfo = pSelf->m_Snap.m_apPlayerInfos[ClientId];
	if(pInfo)
	{
		if(pSelf->m_Snap.m_aCharacters[ClientId].m_Active)
		{
			const vec2 RenderPos = pSelf->m_aClients[ClientId].m_RenderPos;
			PosX = RenderPos.x;
			PosY = RenderPos.y;
		}
		else if(pSelf->m_aClients[ClientId].m_SpecCharPresent)
		{
			const vec2 RenderPos = pSelf->m_aClients[ClientId].m_SpecChar;
			PosX = RenderPos.x;
			PosY = RenderPos.y;
		}
	}

	return {
		.m_Active = true,
		.m_ClientId = ClientId,
		.m_pName = Client.m_aName,
		.m_pClan = Client.m_aClan,
		.m_PosX = PosX,
		.m_PosY = PosY};
}

int CChillerBotUX::GetTotalJumpsCallback(void *pUser)
{
	CChillerBotUX *pSelf = &static_cast<CGameClient *>(pUser)->m_ChillerBotUX;
	return pSelf->GetTotalJumps();
}

int CChillerBotUX::GetUnusedJumpsCallback(void *pUser)
{
	CChillerBotUX *pSelf = &static_cast<CGameClient *>(pUser)->m_ChillerBotUX;
	return pSelf->GetUnusedJumps();
}
