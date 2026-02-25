// ChillerDragon 2022 - chillerbot ux

#include "edit_map.h"

#include <base/process.h>

#include <engine/client/notifications.h>
#include <engine/config.h>
#include <engine/console.h>
#include <engine/engine.h>
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

/*
    needed setup

    mkdir -p ~/.teeworlds/maps/tmp
    cd scripts/
    ln -s "$(pwd)/set_gametile.py" ~/.teeworlds/maps/tmp/
*/

void CEditMapJob::Run()
{
	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), "cd $HOME/.teeworlds/ && /usr/bin/python3 ./maps/tmp/set_gametile.py %s %s %d %d %d",
		m_aSource, m_aDestination, m_X, m_Y, m_Index);
	if(system(aBuf))
	{
		dbg_msg("edit_map", "process failed: %s", aBuf);
	}
	m_State = STATE_DONE;
}

CEditMap::CEditMap()
{
	m_pSetGameTileJob = nullptr;
}

void CEditMap::OnInit()
{
	char aDest[IO_MAX_PATH_LENGTH];
	GetEditedMapPath(aDest, sizeof(aDest));
	Storage()->RemoveFile(aDest, IStorage::TYPE_SAVE);
}

void CEditMap::OnShutdown()
{
	char aDest[IO_MAX_PATH_LENGTH];
	GetEditedMapPath(aDest, sizeof(aDest));
	Storage()->RemoveFile(aDest, IStorage::TYPE_SAVE);
}

void CEditMap::OnConsoleInit()
{
	Console()->Register("set_gametile", "i[x]i[y]i[index]", CFGFLAG_CLIENT, ConSetGametile, this, "Some ugly wip map edit hack");
}

void CEditMap::OnRender()
{
	if(m_pSetGameTileJob)
	{
		if(m_pSetGameTileJob->State() == CEditMapJob::STATE_DONE)
		{
			char aDest[IO_MAX_PATH_LENGTH];
			GetEditedMapPath(aDest, sizeof(aDest));
			dbg_msg("edit_map", "thread finished load map: %s", aDest);
			GameClient()->Client()->ChillerBotLoadMap(aDest);
			m_pSetGameTileJob = nullptr;
		}
	}
}

void CEditMap::GetEditedMapPath(char *pPath, int Size)
{
	str_format(pPath, Size, "maps/tmp/chillerbot-ux-%d.map.tmp", process_id());
}

void CEditMap::GetSourceMapPath(char *pPath, int Size)
{
	GetEditedMapPath(pPath, Size);
	char aBuf[IO_MAX_PATH_LENGTH];
	if(Storage()->FindFile(pPath + 9, "maps", IStorage::TYPE_SAVE, aBuf, sizeof(aBuf)))
	{
		if(!str_comp(pPath, aBuf))
		{
			dbg_msg("edit_map", "edit map found add to it '%s'", aBuf);
			str_copy(pPath, aBuf, Size);
			return;
		}
		else
		{
			dbg_msg("edit_map", "found map does not match '%s' != '%s'", pPath, aBuf);
		}
	}
	dbg_msg("edit_map", "'%s' not found in maps/ use current map", pPath + 9);
	str_copy(pPath, GameClient()->Map()->Path(), Size);
}

void CEditMap::ConSetGametile(IConsole::IResult *pResult, void *pUserData)
{
	CEditMap *pSelf = (CEditMap *)pUserData;
	char aDest[IO_MAX_PATH_LENGTH];
	pSelf->GetEditedMapPath(aDest, sizeof(aDest));
	char aSrc[IO_MAX_PATH_LENGTH];
	pSelf->GetSourceMapPath(aSrc, sizeof(aSrc));
	dbg_msg("edit_map", "load map from %s", aSrc);
	dbg_msg("edit_map", "save to %s", aDest);
	int x = pResult->GetInteger(0);
	int y = pResult->GetInteger(1);
	int Index = pResult->GetInteger(2);
	pSelf->m_pSetGameTileJob = std::make_shared<CEditMapJob>(aSrc, aDest, x, y, Index);
	pSelf->GameClient()->Engine()->AddJob(pSelf->m_pSetGameTileJob);
}
