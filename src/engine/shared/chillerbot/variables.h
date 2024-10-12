// This file can be included several times.

#ifndef MACRO_CONFIG_INT
#error "The config macros must be defined"
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc) ;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc) ;
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc) ;
#endif

// chillerbot-ux
MACRO_CONFIG_INT(ClNoParticels, cl_no_particles, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Skip particle rendering (not all just some annoying ones)")
MACRO_CONFIG_INT(ClRenderPic, cl_render_pic, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Render nice thumbnails of players if there are some")
MACRO_CONFIG_INT(ClRenderPicWidth, cl_render_pic_width, 64, 2, 1024, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of player pics")
MACRO_CONFIG_INT(ClRenderPicHeight, cl_render_pic_height, 64, 2, 1024, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of player pics")
MACRO_CONFIG_INT(ClRenderPicAlpha, cl_render_pic_alpha, 100, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "alpha of cl_render_pic 1")
MACRO_CONFIG_INT(ClSaveMapInfo, cl_save_map_info, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Save map info ( off for tml compability on for everything else)")
MACRO_CONFIG_INT(ClFinishRename, cl_finish_rename, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Change name to cl_finish_name if finish is near.")
MACRO_CONFIG_STR(ClFinishName, cl_finish_name, 32, "chillerbot.png", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Change to this name if cl_finish_rename is active.")
MACRO_CONFIG_INT(ClCampHack, cl_camp_hack, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "0=off 1=mark with gun 2=auto walk")
MACRO_CONFIG_INT(ClAutoReply, cl_auto_reply, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Respond with cl_auto_reply_msg when pinged in chat.")
MACRO_CONFIG_STR(ClAutoReplyMsg, cl_auto_reply_msg, 32, "%n (chillerbot-ux auto reply)", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Message to reply when pinged in chat and cl_auto_reply is set to 1")
MACRO_CONFIG_INT(ClTabbedOutMsg, cl_tabbed_out_msg, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Let others know when pinged in chat that you are tabbed out")
MACRO_CONFIG_INT(ClChillerbotHud, cl_chillerbot_hud, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show enabled chillerbot components in hud")
MACRO_CONFIG_INT(ClShowSpeed, cl_show_speed, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show speed in hud")
MACRO_CONFIG_INT(ClChangeTileNotification, cl_change_tile_notification, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Notify when leaving current tile type")
MACRO_CONFIG_INT(ClShowLastKiller, cl_show_last_killer, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show last killer in hud requires cl_chillerbot_hud 1")
MACRO_CONFIG_INT(ClShowLastPing, cl_show_last_ping, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show last chat ping in hud requires cl_chillerbot_hud 1")
MACRO_CONFIG_INT(ClRenderLaserHead, cl_render_laser_head, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Render laser bubbles")
MACRO_CONFIG_STR(ClPasswordFile, cl_password_file, 512, "chillerbot/chillpw_secret.txt", CFGFLAG_CLIENT | CFGFLAG_SAVE, "File to load passwords for autologin")
MACRO_CONFIG_INT(ClShowWallet, cl_show_wallet, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show your city mod money in the hud")
MACRO_CONFIG_INT(ClWeaponHud, cl_weapon_hud, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Render additional weapon hud")
MACRO_CONFIG_INT(ClAlwaysReconnect, cl_always_reconnect, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Reconnect to last server after some time no matter what")
MACRO_CONFIG_INT(ClSendOnlineTime, cl_send_online_time, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Notify zillyhuhn.com when you are online")
MACRO_CONFIG_STR(ClChillerbotId, cl_chillerbot_id, 64, "", CFGFLAG_SAVE | CFGFLAG_CLIENT, "chillerbot id do not change")
MACRO_CONFIG_INT(ClChatSpamFilter, cl_chat_spam_filter, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Filter spam pings in chat (0=off 1=hide 2=hide and autoreply)")
MACRO_CONFIG_INT(ClChatSpamFilterInsults, cl_chat_spam_filter_insults, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Activates insult filtering when cl_chat_spam_filter is 1")
MACRO_CONFIG_INT(ClReconnectWhenEmpty, cl_reconnect_when_empty, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Reconnect to the current server when the last player leaves")
MACRO_CONFIG_INT(ClSpikeTracer, cl_spike_tracer, 0, 0, 512, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Radius in which spike tiles are traced (0=off)")
MACRO_CONFIG_INT(ClSpikeTracerWalls, cl_spike_tracer_walls, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Trace spikes through walls")
MACRO_CONFIG_INT(ClPenTest, cl_pentest, 0, 0, 1, CFGFLAG_CLIENT, "penetrate servers to test stability")
MACRO_CONFIG_STR(ClPenTestFile, cl_pentest_file, 128, "pentest.txt", CFGFLAG_CLIENT, "chat messages file for pentestmode")
MACRO_CONFIG_INT(ClDbgIntersect, cl_dbg_intersect, 0, 0, 1, CFGFLAG_CLIENT, "Show graphical output for CCollsion::IntersectLine")
MACRO_CONFIG_INT(ClReleaseMouse, cl_release_mouse, 0, 0, 1, CFGFLAG_CLIENT, "Release the mouse (you probably do not want this)")
MACRO_CONFIG_STR(ClRunOnVoteStart, cl_run_on_vote_start, 512, "", CFGFLAG_CLIENT | CFGFLAG_SAVE, "console command to run when a vote is called")
MACRO_CONFIG_INT(ClMineTeeEditor, cl_mine_tee_editor, 0, 0, 1, CFGFLAG_CLIENT, "send modify map packets to the server while brushing in the editor")
MACRO_CONFIG_INT(ClSilentChatCommands, cl_silent_chat_commands, 0, 0, 1, CFGFLAG_CLIENT, "do not send internal chat commands like !help to the server")

#if defined(CONF_CURSES_CLIENT)
MACRO_CONFIG_INT(ClTermHistory, cl_term_history, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Persist input history in filesystem for curses client")
MACRO_CONFIG_INT(ClTermBrowserSearchTop, cl_term_browser_search_top, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "When opening the search (/) in server browser show it on the top")
#endif

// skin stealer
MACRO_CONFIG_INT(ClSkinStealer, cl_skin_stealer, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Automatically adapt skin of close by tees (see also cl_skin_steal_radius)")
MACRO_CONFIG_INT(ClSkinStealRadius, cl_skin_steal_radius, 5, 1, 16, CFGFLAG_CLIENT | CFGFLAG_SAVE, "How many tiles away can a tee be to get skin stolen (needs cl_skin_stealer 1)")
MACRO_CONFIG_INT(ClSkinStealColor, cl_skin_steal_color, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Also steal skin color settings (needs cl_skin_stealer 1)")
MACRO_CONFIG_INT(ClSavedPlayerUseCustomColor, saved_player_use_custom_color, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Saved to restore for cl_skin_stealer")
MACRO_CONFIG_COL(ClSavedPlayerColorBody, saved_player_color_body, 65408, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_COLLIGHT | CFGFLAG_INSENSITIVE, "Saved to restore for cl_skin_stealer")
MACRO_CONFIG_COL(ClSavedPlayerColorFeet, saved_player_color_feet, 65408, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_COLLIGHT | CFGFLAG_INSENSITIVE, "Saved to restore for cl_skin_stealer")
MACRO_CONFIG_STR(ClSavedPlayerSkin, saved_player_skin, 24, "default", CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Saved to restore for cl_skin_stealer")
MACRO_CONFIG_INT(ClSavedDummyUseCustomColor, saved_dummy_use_custom_color, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Saved to restore for cl_skin_stealer")
MACRO_CONFIG_COL(ClSavedDummyColorBody, saved_dummy_color_body, 65408, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_COLLIGHT | CFGFLAG_INSENSITIVE, "Saved to restore for cl_skin_stealer")
MACRO_CONFIG_COL(ClSavedDummyColorFeet, saved_dummy_color_feet, 65408, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_COLLIGHT | CFGFLAG_INSENSITIVE, "Saved to restore for cl_skin_stealer")
MACRO_CONFIG_STR(ClSavedDummySkin, saved_dummy_skin, 24, "default", CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Saved to restore for cl_skin_stealer")

// warlist
MACRO_CONFIG_INT(ClWarList, cl_war_list, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Display nameplate color based on chillerbot/warlist directory")
MACRO_CONFIG_INT(ClWarListAdvanced, cl_war_list_advanced, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Activates more complex warlist mode")
MACRO_CONFIG_INT(ClNameplatesWarReason, cl_nameplates_war_reason, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show war reason in name plates")
MACRO_CONFIG_INT(ClWarListAutoReload, cl_war_list_auto_reload, 10, 0, 600, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Reload warlist every x seconds 0=off")

// remotecontrol
MACRO_CONFIG_INT(ClRemoteControl, cl_remote_control, 0, 0, 1, CFGFLAG_CLIENT, "Execute whisper in local console")
MACRO_CONFIG_STR(ClRemoteControlToken, cl_remote_control_token, 512, "spaces_do_not_work", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Remote control token (see cl_remote_control)")
MACRO_CONFIG_STR(ClRemoteControlTokenAdmin, cl_remote_control_token_admin, 512, "spaces_do_not_work_", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Remote control admin token (see cl_remote_control)")
