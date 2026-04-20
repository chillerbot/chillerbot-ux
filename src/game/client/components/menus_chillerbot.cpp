/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "menus.h"

#include <base/log.h>
#include <base/math.h>
#include <base/str.h>

#include <engine/console.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/shared/linereader.h>
#include <engine/shared/localization.h>
#include <engine/shared/protocol7.h>
#include <engine/storage.h>
#include <engine/textrender.h>
#include <engine/updater.h>

#include <generated/protocol.h>

#include <game/client/animstate.h>
#include <game/client/components/chat.h>
#include <game/client/components/chillerbot/chillerbotux.h>
#include <game/client/components/chillerbot/warlist.h>
#include <game/client/components/menu_background.h>
#include <game/client/components/sounds.h>
#include <game/client/gameclient.h>
#include <game/client/render.h>
#include <game/client/skin.h>
#include <game/client/ui.h>
#include <game/client/ui_listbox.h>
#include <game/client/ui_scrollregion.h>
#include <game/localization.h>

using namespace std::chrono_literals;

void CMenus::RenderSettingsChillerbot(CUIRect MainView)
{
	CUIRect Button, Label;
	MainView.HSplitTop(10.0f, 0, &MainView);

	char aBuf[128];

	// finish name
	{
		CUIRect Checkbox;
		MainView.HSplitTop(20.0f, &Button, &MainView);
		Button.VSplitLeft(120.0f, &Label, &Button);
		Button.VSplitLeft(200.0f, &Button, &Checkbox);
		Button.VSplitLeft(150.0f, &Button, 0);
		str_format(aBuf, sizeof(aBuf), "%s:", "Finish name");
		Ui()->DoLabel(&Label, aBuf, 14.0f, -1);
		static CLineInput s_NameInput;
		s_NameInput.SetBuffer(g_Config.m_ClFinishName, sizeof(g_Config.m_ClFinishName));
		s_NameInput.SetEmptyText("chillerbot-ux");
		Ui()->DoEditBox(&s_NameInput, &Button, 14.0f);
		if(DoButton_CheckBox(&g_Config.m_ClFinishRename, "Rename on finish", g_Config.m_ClFinishRename, &Checkbox))
		{
			g_Config.m_ClFinishRename ^= 1;
			GameClient()->m_ChillerBotUX.UpdateComponents();
		}
	}

	MainView.HSplitTop(5.0f, 0, &MainView);

	// auto reply
	{
		CUIRect Checkbox;
		MainView.HSplitTop(20.0f, &Button, &MainView);
		Button.VSplitLeft(120.0f, &Label, &Button);
		Button.VSplitLeft(200.0f, &Button, &Checkbox);
		Button.VSplitLeft(150.0f, &Button, 0);
		str_format(aBuf, sizeof(aBuf), "%s:", "auto reply msg");
		Ui()->DoLabel(&Label, aBuf, 14.0f, -1);
		static CLineInput s_ReplyMsg;
		s_ReplyMsg.SetBuffer(g_Config.m_ClAutoReplyMsg, sizeof(g_Config.m_ClAutoReplyMsg));
		s_ReplyMsg.SetEmptyText("I use chillerbot-ux");
		Ui()->DoEditBox(&s_ReplyMsg, &Button, 14.0f);
		if(DoButton_CheckBox(&g_Config.m_ClAutoReply, "Auto reply", g_Config.m_ClAutoReply, &Checkbox))
		{
			g_Config.m_ClAutoReply ^= 1;
			GameClient()->m_ChillerBotUX.UpdateComponents();
		}
	}

	MainView.HSplitTop(5.0f, 0, &MainView);

	// chillerbot hud
	{
		CUIRect Checkbox;
		MainView.HSplitTop(20.0f, &Checkbox, &MainView);
		if(DoButton_CheckBox(&g_Config.m_ClChillerbotHud, "show component hud", g_Config.m_ClChillerbotHud, &Checkbox))
		{
			g_Config.m_ClChillerbotHud ^= 1;
			GameClient()->m_ChillerBotUX.UpdateComponents();
		}
	}
	// war list
	{
		CUIRect Checkbox;
		MainView.HSplitTop(20.0f, &Checkbox, &MainView);
		if(DoButton_CheckBox(&g_Config.m_ClWarList, "war list", g_Config.m_ClWarList, &Checkbox))
		{
			g_Config.m_ClWarList ^= 1;
			GameClient()->m_ChillerBotUX.UpdateComponents();
			GameClient()->m_WarList.ReloadList();
		}
	}
	// notify tile change
	{
		CUIRect Checkbox;
		MainView.HSplitTop(20.0f, &Checkbox, &MainView);
		if(DoButton_CheckBox(&g_Config.m_ClChangeTileNotification, "notify when tee leaves current tile type (while tabbed out)", g_Config.m_ClChangeTileNotification, &Checkbox))
		{
			g_Config.m_ClChangeTileNotification ^= 1;
		}
	}
	// show last killer
	{
		CUIRect Checkbox;
		MainView.HSplitTop(20.0f, &Checkbox, &MainView);
		if(DoButton_CheckBox(&g_Config.m_ClShowLastKiller, "Show last killer", g_Config.m_ClShowLastKiller, &Checkbox))
		{
			g_Config.m_ClShowLastKiller ^= 1;
			g_Config.m_ClChillerbotHud = 1;
			GameClient()->m_ChillerBotUX.UpdateComponents();
		}
	}
	// render laser head
	{
		CUIRect Checkbox;
		MainView.HSplitTop(20.0f, &Checkbox, &MainView);
		if(DoButton_CheckBox(&g_Config.m_ClRenderLaserHead, "Render laser heads (bobbles)", g_Config.m_ClRenderLaserHead, &Checkbox))
			g_Config.m_ClRenderLaserHead ^= 1;
	}
	// skin stealer
	{
		CUIRect Checkbox;
		MainView.HSplitTop(20.0f, &Checkbox, &MainView);
		if(DoButton_CheckBox(&g_Config.m_ClSkinStealer, "Skin stealer", g_Config.m_ClSkinStealer, &Checkbox))
		{
			str_format(aBuf, sizeof(aBuf), "cl_skin_stealer %d", !g_Config.m_ClSkinStealer);
			Console()->ExecuteLine(aBuf, IConsole::CLIENT_ID_UNSPECIFIED);
		}
	}
	// Playtime
	{
		MainView.HSplitTop(20.0f, &Button, &MainView);
		Button.VSplitLeft(120.0f, &Label, &Button);
		str_format(aBuf, sizeof(aBuf), "chillerbot-ux playtime: %d hours", GameClient()->m_ChillerBotUX.GetPlayTimeHours());
		Ui()->DoLabel(&Label, aBuf, 14.0f, -1);
	}

	// Send and detect custom clients
	{
		CUIRect Checkbox;
		MainView.HSplitTop(20.0f, &Checkbox, &MainView);
		if(DoButton_CheckBox(&g_Config.m_ClSendClientType, "Let know other clients we are Chillerbot-UX", g_Config.m_ClSendClientType, &Checkbox))
			g_Config.m_ClSendClientType ^= 1;
	}
	{
		CUIRect Checkbox;
		MainView.HSplitTop(20.0f, &Checkbox, &MainView);
		if(DoButton_CheckBox(&g_Config.m_ClShowClientType, "Try to identify other clients and show a icon above player name", g_Config.m_ClShowClientType, &Checkbox))
			g_Config.m_ClShowClientType ^= 1;
	}
}
