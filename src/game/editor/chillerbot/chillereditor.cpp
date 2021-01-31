#include <engine/input.h>
#include <engine/keys.h>

#include <game/editor/editor.h>

#include "chillereditor.h"

CChillerEditor::CChillerEditor()
{
	m_EditorMode = CE_MODE_NONE;
	m_TextIndexX = -1;
	m_TextIndexY = -1;
	m_TextLineLen = 0;
	m_NextCursorBlink = 0;
}

void CChillerEditor::Init(class CEditor *pEditor)
{
	m_CursorTextTexture = pEditor->Graphics()->LoadTexture("editor/cursor_text.png", IStorage::TYPE_ALL, CImageInfo::FORMAT_AUTO, 0);
}

void CChillerEditor::SetCursor(CEditor *pEditor)
{
	float wx = pEditor->UI()->MouseWorldX();
	float wy = pEditor->UI()->MouseWorldY();
	m_TextIndexX = (int)(wx / 32);
	m_TextIndexY = (int)(wy / 32);
	m_TextLineLen = 0;
}

void CChillerEditor::DoMapEditor(CEditor *pEditor, int Inside)
{
	if(pEditor->UI()->MouseButton(0))
	{
		SetCursor(pEditor);
	}
	if(pEditor->Input()->KeyPress(KEY_T) && m_EditorMode != CE_MODE_TEXT && Inside && pEditor->m_Dialog == DIALOG_NONE && pEditor->m_EditBoxActive == 0)
	{
		m_EditorMode = CE_MODE_TEXT;
		pEditor->m_Dialog = -1; // hack to not close editor when pressing ESC
		SetCursor(pEditor);
	}
	if(m_EditorMode == CE_MODE_TEXT)
	{
		pEditor->m_pTooltip = "Type on your keyboard to insert letters. Press ESC to end text mode.";
		CLayerTiles *pLayer = (CLayerTiles *)pEditor->GetSelectedLayerType(0, LAYERTYPE_TILES);
		if(m_TextIndexX < 0 || m_TextIndexX > pLayer->m_Width - 1)
			return;
		if(m_TextIndexY < 0 || m_TextIndexY > pLayer->m_Height - 1)
			return;
		if(time_get() > m_NextCursorBlink)
		{
			m_NextCursorBlink = time_get() + time_freq() / 1.5;
			m_DrawCursor ^= true;
		}
		if(m_DrawCursor)
		{
			CLayerGroup *g = pEditor->GetSelectedGroup();
			g->MapScreen();
			pEditor->Graphics()->WrapClamp();
			pEditor->Graphics()->TextureSet(m_CursorTextTexture);
			pEditor->Graphics()->QuadsBegin();
			pEditor->Graphics()->SetColor(1, 0, 0, 1);
			IGraphics::CQuadItem QuadItem(m_TextIndexX * 32, m_TextIndexY * 32, 32.0f, 32.0f);
			pEditor->Graphics()->QuadsDrawTL(&QuadItem, 1);
			pEditor->Graphics()->QuadsEnd();
			pEditor->Graphics()->WrapNormal();
			pEditor->Graphics()->MapScreen(pEditor->UI()->Screen()->x, pEditor->UI()->Screen()->y, pEditor->UI()->Screen()->w, pEditor->UI()->Screen()->h);
		}
		// handle key presses
		IInput::CEvent e = pEditor->Input()->GetEvent(0);
		if(!pEditor->Input()->IsEventValid(&e))
			return;
		if(pEditor->Input()->KeyPress(e.m_Key))
			return;
		// dbg_msg("chillerbot", "key=%d", e.m_Key);
		// letters
		if(e.m_Key > 3 && e.m_Key < 30)
		{
			CTile Tile;
			Tile.m_Index = e.m_Key - 3;
			pLayer->SetTile(m_TextIndexX++, m_TextIndexY, Tile);
			m_TextLineLen++;
			m_NextCursorBlink = time_get() + time_freq();
			m_DrawCursor = true;
		}
		// deletion
		if(e.m_Key == KEY_BACKSPACE)
		{
			CTile Tile;
			Tile.m_Index = 0;
			pLayer->SetTile(--m_TextIndexX, m_TextIndexY, Tile);
			m_TextLineLen--;
			m_NextCursorBlink = time_get() + time_freq();
			m_DrawCursor = true;
		}
		// space
		if(e.m_Key == KEY_SPACE)
		{
			CTile Tile;
			Tile.m_Index = 0;
			pLayer->SetTile(m_TextIndexX++, m_TextIndexY, Tile);
			m_TextLineLen++;
			m_NextCursorBlink = time_get() + time_freq();
			m_DrawCursor = true;
		}
		// newline
		if(e.m_Key == KEY_RETURN)
		{
			m_TextIndexY++;
			m_TextIndexX -= m_TextLineLen;
			m_TextLineLen = 0;
			m_NextCursorBlink = time_get() + time_freq();
			m_DrawCursor = true;
		}
		// escape -> end text mode
		if(e.m_Key == KEY_ESCAPE)
		{
			if(pEditor->m_Dialog == -1)
				pEditor->m_Dialog = 0;
			m_EditorMode = CE_MODE_NONE;
		}
		m_TextIndexX = clamp(m_TextIndexX, 0, pLayer->m_Width - 1);
		m_TextIndexY = clamp(m_TextIndexY, 0, pLayer->m_Height - 1);
	}
}
