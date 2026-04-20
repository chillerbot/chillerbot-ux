#include <base/math.h>
#include <base/str.h>

#include <engine/console.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/shared/linereader.h>
#include <engine/storage.h>
#include <engine/textrender.h>

#include <generated/client_data.h>
#include <generated/protocol.h>

#include <game/client/animstate.h>
#include <game/client/components/camera.h>
#include <game/client/components/controls.h>
#include <game/client/components/countryflags.h>
#include <game/client/components/nameplates.h>
#include <game/client/gameclient.h>

// nobo copy of countryflags.cpp
#include "playerpics.h"

int CPlayerPics::LoadImageByName(const char *pImgName, int IsDir, int DirType, void *pUser)
{
	CPlayerPics *pSelf = (CPlayerPics *)pUser;

	if(IsDir || !str_endswith(pImgName, ".png"))
		return 0;

	dbg_msg("chiller", "load image '%s'", pImgName);

	// chop .png off
	char aName[128];
	str_copy(aName, pImgName, sizeof(aName));
	aName[str_length(aName) - 4] = 0;
	// return 0;

	char aBuf[128];
	CImageInfo Info;
	str_format(aBuf, sizeof(aBuf), "playerpics/%s.png", aName);
	if(!pSelf->Graphics()->LoadPng(Info, aBuf, IStorage::TYPE_ALL))
	{
		char aMsg[128];
		str_format(aMsg, sizeof(aMsg), "failed to load '%s'", aBuf);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "playerpics", aMsg);
		return 0;
	}
	else
	{
		char aMsg[128];
		str_format(aMsg, sizeof(aMsg), "SUCCESS loading '%s'", aBuf);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "playerpics", aMsg);
	}

	// add entry
	CPlayerPic CountryFlag;
	str_copy(CountryFlag.m_aPlayerName, aName, sizeof(CountryFlag.m_aPlayerName));
	CountryFlag.m_Texture = pSelf->Graphics()->LoadTextureRaw(Info, 0);
	if(g_Config.m_Debug)
	{
		str_format(aBuf, sizeof(aBuf), "loaded player pic '%s'", pImgName);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "playerpics", aBuf);
	}
	pSelf->m_vPlayerPics.push_back(CountryFlag);
	return 0;
}

void CPlayerPics::LoadPlayerpicsIndexfile()
{
	Storage()->ListDirectory(IStorage::TYPE_ALL, "playerpics", LoadImageByName, this);
	std::sort(m_vPlayerPics.begin(), m_vPlayerPics.end());
}

int CPlayerPics::Num() const
{
	return m_vPlayerPics.size();
}

const CPlayerPics::CPlayerPic *CPlayerPics::GetByName(const char *pName) const
{
	int i = 0;
	for(const auto &PlayerPic : m_vPlayerPics)
	{
		if(str_comp(PlayerPic.m_aPlayerName, pName) == 0)
			return GetByIndex(i);
		i++;
	}
	return NULL;
}

const CPlayerPics::CPlayerPic *CPlayerPics::GetByIndex(int Index) const
{
	return &m_vPlayerPics[Index % m_vPlayerPics.size()];
}

void CPlayerPics::Render(const char *pName, const vec4 *pColor, float x, float y, float w, float h)
{
	const CPlayerPic *pFlag = GetByName(pName);
	if(!pFlag)
		return;

	if(pFlag->m_Texture.IsValid())
	{
		float ScreenX0, ScreenY0, ScreenX1, ScreenY1;
		Graphics()->GetScreen(&ScreenX0, &ScreenY0, &ScreenX1, &ScreenY1);
		GameClient()->m_ChillerBotUX.MapScreenToGroup(GameClient()->m_Camera.m_Center.x, GameClient()->m_Camera.m_Center.y, Layers()->GameGroup(), GameClient()->m_Camera.m_Zoom);
		Graphics()->TextureSet(pFlag->m_Texture);
		Graphics()->QuadsBegin();
		Graphics()->SetColor(pColor->r, pColor->g, pColor->b, pColor->a);
		IGraphics::CQuadItem QuadItem(x, y, w, h);
		Graphics()->QuadsDrawTL(&QuadItem, 1);
		Graphics()->QuadsEnd();
		Graphics()->MapScreen(ScreenX0, ScreenY0, ScreenX1, ScreenY1);
	}
}

void CPlayerPics::RenderNameplate(
	const CNetObj_Character *pPrevChar,
	const CNetObj_Character *pPlayerChar,
	const CNetObj_PlayerInfo *pPlayerInfo)
{
	int ClientId = pPlayerInfo->m_ClientId;

	vec2 Position;
	if(ClientId >= 0 && ClientId < MAX_CLIENTS)
		Position = GameClient()->m_aClients[ClientId].m_RenderPos;
	else
		Position = mix(vec2(pPrevChar->m_X, pPrevChar->m_Y), vec2(pPlayerChar->m_X, pPlayerChar->m_Y), Client()->IntraGameTick(g_Config.m_ClDummy));

	RenderNameplatePos(Position, pPlayerInfo, 1.0f);
}

void CPlayerPics::RenderNameplatePos(vec2 Position, const CNetObj_PlayerInfo *pPlayerInfo, float Alpha, bool ForceAlpha)
{
	int ClientId = pPlayerInfo->m_ClientId;
	float FontSizeClan = 18.0f + 20.0f * g_Config.m_ClNamePlatesClanSize / 100.0f;
	float YOffset = Position.y - (g_Config.m_ClRenderPicHeight + (g_Config.m_ClNamePlatesClan ? 90.0f : 60.0f));

	// render playerpic
	if(!pPlayerInfo->m_Local || g_Config.m_ClNamePlatesOwn)
	{
		const char *pName = GameClient()->m_aClients[pPlayerInfo->m_ClientId].m_aName;
		if(g_Config.m_ClRenderPic)
		{
			// render player pics
			vec4 Color(1.0f, 1.0f, 1.0f, g_Config.m_ClRenderPicAlpha / 100.0f);
			Render(pName, &Color, Position.x - (g_Config.m_ClRenderPicWidth / 2), YOffset, g_Config.m_ClRenderPicWidth, g_Config.m_ClRenderPicHeight);
		}

		TextRender()->TextColor(1, 1, 1, 1);
		TextRender()->TextOutlineColor(0.0f, 0.0f, 0.0f, 0.3f);

		TextRender()->SetRenderFlags(0);

		char aWarReason[128];
		GameClient()->m_WarList.GetWarReason(pName, aWarReason, sizeof(aWarReason));
		if(g_Config.m_ClNameplatesWarReason && aWarReason[0])
		{
			if(str_comp(aWarReason, m_aNamePlates[ClientId].m_aWarReason) != 0 || FontSizeClan != m_aNamePlates[ClientId].m_WarReasonTextFontSize)
			{
				mem_copy(m_aNamePlates[ClientId].m_aWarReason, aWarReason, sizeof(m_aNamePlates[ClientId].m_aWarReason));
				m_aNamePlates[ClientId].m_WarReasonTextFontSize = FontSizeClan;

				if(m_aNamePlates[ClientId].m_WarReasonTextContainerIndex.Valid())
					TextRender()->DeleteTextContainer(m_aNamePlates[ClientId].m_WarReasonTextContainerIndex);

				CTextCursor Cursor;
				Cursor.m_FontSize = FontSizeClan;
				Cursor.m_Flags = TEXTFLAG_RENDER;
				Cursor.m_LineWidth = -1;

				// create nameplates at standard zoom
				float ScreenX0, ScreenY0, ScreenX1, ScreenY1;
				Graphics()->GetScreen(&ScreenX0, &ScreenY0, &ScreenX1, &ScreenY1);
				GameClient()->m_ChillerBotUX.MapScreenToGroup(GameClient()->m_Camera.m_Center.x, GameClient()->m_Camera.m_Center.y, Layers()->GameGroup(), GameClient()->m_Camera.m_Zoom);

				m_aNamePlates[ClientId].m_WarReasonTextWidth = TextRender()->TextWidth(FontSizeClan, aWarReason, -1, -1.0f);

				TextRender()->CreateTextContainer(m_aNamePlates[ClientId].m_WarReasonTextContainerIndex, &Cursor, aWarReason);
				Graphics()->MapScreen(ScreenX0, ScreenY0, ScreenX1, ScreenY1);
			}
			YOffset += FontSizeClan;
			ColorRGBA TColor = ColorRGBA(0.0f, 0.0f, 0.0f, 0.6f);
			ColorRGBA TOutlineColor = ColorRGBA(1.0f, 0.0f, 0.0f, 0.7f);
			if(m_aNamePlates[ClientId].m_WarReasonTextContainerIndex.Valid())
				TextRender()->RenderTextContainer(m_aNamePlates[ClientId].m_WarReasonTextContainerIndex, TColor, TOutlineColor, Position.x - m_aNamePlates[ClientId].m_WarReasonTextWidth / 2.0f, YOffset);
		}
	}
}

void CPlayerPics::OnRender()
{
	if(!g_Config.m_ClNamePlates)
		return;

	// get screen edges to avoid rendering offscreen
	float ScreenX0, ScreenY0, ScreenX1, ScreenY1;
	Graphics()->GetScreen(&ScreenX0, &ScreenY0, &ScreenX1, &ScreenY1);
	// expand the edges to prevent popping in/out onscreen
	//
	// it is assumed that the nameplate and all its components fit into a 800x800 box placed directly above the tee
	// this may need to be changed or calculated differently in the future
	ScreenX0 -= 400;
	ScreenX1 += 400;
	//ScreenY0 -= 0;
	ScreenY1 += 800;

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		const CNetObj_PlayerInfo *pInfo = GameClient()->m_Snap.m_apPlayerInfos[i];
		if(!pInfo)
		{
			continue;
		}

		vec2 *pRenderPos;
		if(GameClient()->m_aClients[i].m_SpecCharPresent)
		{
			// Each player can also have a spec char whose nameplate is displayed independently
			pRenderPos = &GameClient()->m_aClients[i].m_SpecChar;
			// don't render offscreen
			if(!(pRenderPos->x < ScreenX0) && !(pRenderPos->x > ScreenX1) && !(pRenderPos->y < ScreenY0) && !(pRenderPos->y > ScreenY1))
			{
				RenderNameplatePos(GameClient()->m_aClients[i].m_SpecChar, pInfo, 0.4f, true);
			}
		}
		if(GameClient()->m_Snap.m_aCharacters[i].m_Active)
		{
			// Only render nameplates for active characters
			pRenderPos = &GameClient()->m_aClients[i].m_RenderPos;
			// don't render offscreen
			if(!(pRenderPos->x < ScreenX0) && !(pRenderPos->x > ScreenX1) && !(pRenderPos->y < ScreenY0) && !(pRenderPos->y > ScreenY1))
			{
				RenderNameplate(
					&GameClient()->m_Snap.m_aCharacters[i].m_Prev,
					&GameClient()->m_Snap.m_aCharacters[i].m_Cur,
					pInfo);
			}
		}
	}
}

void CPlayerPics::ResetNamePlates()
{
	for(auto &NamePlate : m_aNamePlates)
	{
		TextRender()->DeleteTextContainer(NamePlate.m_NameTextContainerIndex);
		TextRender()->DeleteTextContainer(NamePlate.m_WarReasonTextContainerIndex);

		NamePlate.Reset();
	}
}

void CPlayerPics::OnWindowResize()
{
	ResetNamePlates();
}

void CPlayerPics::OnInit()
{
	ResetNamePlates();
	// load country flags
	m_vPlayerPics.clear();
	LoadPlayerpicsIndexfile();
	if(m_vPlayerPics.empty())
	{
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "playerpics", "failed to load directory 'playerpics/'.");
		CPlayerPic DummyEntry;
		mem_zero(DummyEntry.m_aPlayerName, sizeof(DummyEntry.m_aPlayerName));
		m_vPlayerPics.push_back(DummyEntry);
	}
}
