
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_VARIABLES_H
#define GAME_VARIABLES_H
#undef GAME_VARIABLES_H // this file will be included several times

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
MACRO_CONFIG_INT(ClNameplatesWarReason, cl_nameplates_war_reason, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show war reason in name plates")
MACRO_CONFIG_INT(ClWarListAutoReload, cl_war_list_auto_reload, 10, 0, 600, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Reload warlist every x seconds 0=off")

// remotecontrol
MACRO_CONFIG_INT(ClRemoteControl, cl_remote_control, 0, 0, 1, CFGFLAG_CLIENT, "Execute whisper in local console")
MACRO_CONFIG_STR(ClRemoteControlToken, cl_remote_control_token, 512, "spaces_do_not_work", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Remote control token (see cl_remote_control)")
MACRO_CONFIG_STR(ClRemoteControlTokenAdmin, cl_remote_control_token_admin, 512, "spaces_do_not_work_", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Remote control admin token (see cl_remote_control)")

// client
MACRO_CONFIG_INT(ClPredict, cl_predict, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict client movements")
MACRO_CONFIG_INT(ClPredictDummy, cl_predict_dummy, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict dummy movements")
MACRO_CONFIG_INT(ClAntiPingLimit, cl_antiping_limit, 0, 0, 200, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Antiping limit (0 to disable)")
MACRO_CONFIG_INT(ClAntiPing, cl_antiping, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable antiping, i. e. more aggressive prediction.")
MACRO_CONFIG_INT(ClAntiPingPlayers, cl_antiping_players, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict other player's movement more aggressively (only enabled if cl_antiping is set to 1)")
MACRO_CONFIG_INT(ClAntiPingGrenade, cl_antiping_grenade, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict grenades (only enabled if cl_antiping is set to 1)")
MACRO_CONFIG_INT(ClAntiPingWeapons, cl_antiping_weapons, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict weapon projectiles (only enabled if cl_antiping is set to 1)")
MACRO_CONFIG_INT(ClAntiPingSmooth, cl_antiping_smooth, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Make the prediction of other player's movement smoother")
MACRO_CONFIG_INT(ClAntiPingGunfire, cl_antiping_gunfire, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict gunfire and show predicted weapon physics (with cl_antiping_grenade 1 and cl_antiping_weapons 1)")
MACRO_CONFIG_INT(ClPredictionMargin, cl_prediction_margin, 10, 1, 2000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Prediction margin in ms (adds latency, can reduce lag from ping jumps)")

MACRO_CONFIG_INT(ClNameplates, cl_nameplates, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show name plates")
MACRO_CONFIG_INT(ClAfkEmote, cl_afk_emote, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show zzz emote next to afk players")
MACRO_CONFIG_INT(ClNameplatesAlways, cl_nameplates_always, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Always show name plates disregarding of distance")
MACRO_CONFIG_INT(ClNameplatesTeamcolors, cl_nameplates_teamcolors, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Use team colors for name plates")
MACRO_CONFIG_INT(ClNameplatesSize, cl_nameplates_size, 50, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of the name plates from 0 to 100%")
MACRO_CONFIG_INT(ClNameplatesClan, cl_nameplates_clan, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show clan in name plates")
MACRO_CONFIG_INT(ClNameplatesClanSize, cl_nameplates_clan_size, 30, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of the clan plates from 0 to 100%")
MACRO_CONFIG_INT(ClNameplatesIDs, cl_nameplates_ids, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show IDs in name plates")
MACRO_CONFIG_INT(ClNameplatesOwn, cl_nameplates_own, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show own name plate (useful for demo recording)")
MACRO_CONFIG_INT(ClNameplatesFriendMark, cl_nameplates_friendmark, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show friend mark (♥) in name plates")
MACRO_CONFIG_INT(ClNameplatesStrong, cl_nameplates_strong, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show strong/weak in name plates (0 - off, 1 - icons, 2 - icons + numbers)")
MACRO_CONFIG_INT(ClTextEntities, cl_text_entities, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Render textual entity data")
MACRO_CONFIG_INT(ClTextEntitiesSize, cl_text_entities_size, 100, 1, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of textual entity data from 1 to 100%")

MACRO_CONFIG_COL(ClAuthedPlayerColor, cl_authed_player_color, 5898211, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Color of name of authenticated player in scoreboard")
MACRO_CONFIG_COL(ClSameClanColor, cl_same_clan_color, 5898211, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Clan color of players with the same clan as you in scoreboard.")

MACRO_CONFIG_INT(ClEnablePingColor, cl_enable_ping_color, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether ping is colored in scoreboard.")
MACRO_CONFIG_INT(ClAutoswitchWeapons, cl_autoswitch_weapons, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Auto switch weapon on pickup")
MACRO_CONFIG_INT(ClAutoswitchWeaponsOutOfAmmo, cl_autoswitch_weapons_out_of_ammo, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Auto switch weapon when out of ammo")

MACRO_CONFIG_INT(ClShowhud, cl_showhud, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame HUD")
MACRO_CONFIG_INT(ClShowhudHealthAmmo, cl_showhud_healthammo, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame HUD (Health + Ammo)")
MACRO_CONFIG_INT(ClShowhudScore, cl_showhud_score, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame HUD (Score)")
MACRO_CONFIG_INT(ClShowhudTimer, cl_showhud_timer, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame HUD (Timer)")
MACRO_CONFIG_INT(ClShowhudDummyActions, cl_showhud_dummy_actions, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame HUD (Dummy Actions)")
MACRO_CONFIG_INT(ClShowhudPlayerPosition, cl_showhud_player_position, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame HUD (Player Position)")
MACRO_CONFIG_INT(ClShowhudPlayerSpeed, cl_showhud_player_speed, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame HUD (Player Speed)")
MACRO_CONFIG_INT(ClShowhudPlayerAngle, cl_showhud_player_angle, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame HUD (Player Aim Angle)")
MACRO_CONFIG_INT(ClShowhudDDRace, cl_showhud_ddrace, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Show ingame HUD (DDRace HUD)")
MACRO_CONFIG_INT(ClShowhudJumpsIndicator, cl_showhud_jumps_indicator, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Show ingame HUD (Jumps you have and have used)")
MACRO_CONFIG_INT(ClShowFreezeBars, cl_show_freeze_bars, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Whether to show a freeze bar under frozen players to indicate the thaw time")
MACRO_CONFIG_INT(ClFreezeBarsAlphaInsideFreeze, cl_freezebars_alpha_inside_freeze, 0, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Opacity of freeze bars inside freeze (0 invisible, 100 fully visible)")
MACRO_CONFIG_INT(ClShowRecord, cl_showrecord, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show old style DDRace client records")
MACRO_CONFIG_INT(ClShowNotifications, cl_shownotifications, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Make the client notify when someone highlights you")
MACRO_CONFIG_INT(ClShowEmotes, cl_showemotes, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show tee emotes")
MACRO_CONFIG_INT(ClShowChat, cl_showchat, 1, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show chat (2 to always show large chat area)")
MACRO_CONFIG_INT(ClShowChatFriends, cl_show_chat_friends, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show only chat messages from friends")
MACRO_CONFIG_INT(ClShowChatSystem, cl_show_chat_system, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show chat messages from the server")
MACRO_CONFIG_INT(ClShowKillMessages, cl_showkillmessages, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show kill messages")
MACRO_CONFIG_INT(ClShowVotesAfterVoting, cl_show_votes_after_voting, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show votes window after voting")
MACRO_CONFIG_INT(ClShowLocalTimeAlways, cl_show_local_time_always, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Always show local time")
MACRO_CONFIG_INT(ClShowfps, cl_showfps, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame FPS counter")
MACRO_CONFIG_INT(ClShowpred, cl_showpred, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame prediction time in milliseconds")
MACRO_CONFIG_INT(ClEyeWheel, cl_eye_wheel, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show eye wheel along together with emotes")
MACRO_CONFIG_INT(ClEyeDuration, cl_eye_duration, 999999, 1, 999999, CFGFLAG_CLIENT | CFGFLAG_SAVE, "How long the eyes emotes last")

MACRO_CONFIG_INT(ClAirjumpindicator, cl_airjumpindicator, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show the air jump indicator")
MACRO_CONFIG_INT(ClThreadsoundloading, cl_threadsoundloading, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Load sound files threaded")

MACRO_CONFIG_INT(ClWarningTeambalance, cl_warning_teambalance, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Warn about team balance")

MACRO_CONFIG_INT(ClMouseDeadzone, cl_mouse_deadzone, 0, 0, 3000, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Deadzone for the camera to follow the cursor")
MACRO_CONFIG_INT(ClMouseFollowfactor, cl_mouse_followfactor, 0, 0, 200, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Factor for the camera to follow the cursor")
MACRO_CONFIG_INT(ClMouseMaxDistance, cl_mouse_max_distance, 400, 0, 5000, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Maximum cursor distance")
MACRO_CONFIG_INT(ClMouseMinDistance, cl_mouse_min_distance, 0, 0, 5000, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Minimum cursor distance")

MACRO_CONFIG_INT(ClDyncam, cl_dyncam, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Enable dyncam")
MACRO_CONFIG_INT(ClDyncamMaxDistance, cl_dyncam_max_distance, 1000, 0, 2000, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Maximum dynamic camera cursor distance")
MACRO_CONFIG_INT(ClDyncamMinDistance, cl_dyncam_min_distance, 0, 0, 2000, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Minimum dynamic camera cursor distance")
MACRO_CONFIG_INT(ClDyncamMousesens, cl_dyncam_mousesens, 0, 0, 100000, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Mouse sens used when dyncam is toggled on")
MACRO_CONFIG_INT(ClDyncamDeadzone, cl_dyncam_deadzone, 300, 1, 1300, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Deadzone for the dynamic camera to follow the cursor")
MACRO_CONFIG_INT(ClDyncamFollowFactor, cl_dyncam_follow_factor, 60, 0, 200, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Factor for the dynamic camera to follow the cursor")

MACRO_CONFIG_INT(ClDyncamSmoothness, cl_dyncam_smoothness, 0, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Transition amount of the camera movement, 0=instant, 100=slow and smooth")
MACRO_CONFIG_INT(ClDyncamStabilizing, cl_dyncam_stabilizing, 0, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Amount of camera slowdown during fast cursor movement. High value can cause delay in camera movement")

MACRO_CONFIG_INT(ClMultiViewSensitivity, cl_multiview_sensitivity, 100, 0, 200, CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Set how fast the camera will move to the desired location (higher = faster)")
MACRO_CONFIG_INT(ClMultiViewZoomSmoothness, cl_multiview_zoom_smoothness, 1300, 50, 5000, CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Set the smoothness of the multi-view zoom (in ms, higher = slower)")

MACRO_CONFIG_INT(EdAutosaveInterval, ed_autosave_interval, 10, 0, 240, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Interval in minutes at which a copy of the current editor map is automatically saved to the 'auto' folder (0 for off)")
MACRO_CONFIG_INT(EdAutosaveMax, ed_autosave_max, 10, 0, 1000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Maximum number of autosaves that are kept per map name (0 = no limit)")
MACRO_CONFIG_INT(EdSmoothZoomTime, ed_smooth_zoom_time, 250, 0, 5000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Time of smooth zoom animation in the editor in ms (0 for off)")
MACRO_CONFIG_INT(EdLimitMaxZoomLevel, ed_limit_max_zoom_level, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Specifies, if zooming in the editor should be limited or not (0 = no limit)")
MACRO_CONFIG_INT(EdZoomTarget, ed_zoom_target, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Zoom to the current mouse target")
MACRO_CONFIG_INT(EdShowkeys, ed_showkeys, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show pressed keys")

MACRO_CONFIG_INT(ClShowWelcome, cl_show_welcome, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show welcome message indicating the first launch of the client")
MACRO_CONFIG_INT(ClMotdTime, cl_motd_time, 10, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "How long to show the server message of the day")

// http map download
MACRO_CONFIG_STR(ClMapDownloadUrl, cl_map_download_url, 100, "https://maps.ddnet.org", CFGFLAG_CLIENT | CFGFLAG_SAVE, "URL used to download maps (can start with http:// or https://)")
MACRO_CONFIG_INT(ClMapDownloadConnectTimeoutMs, cl_map_download_connect_timeout_ms, 2000, 0, 100000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "HTTP map downloads: timeout for the connect phase in milliseconds (0 to disable)")
MACRO_CONFIG_INT(ClMapDownloadLowSpeedLimit, cl_map_download_low_speed_limit, 4000, 0, 100000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "HTTP map downloads: Set low speed limit in bytes per second (0 to disable)")
MACRO_CONFIG_INT(ClMapDownloadLowSpeedTime, cl_map_download_low_speed_time, 3, 0, 100000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "HTTP map downloads: Set low speed limit time period (0 to disable)")

MACRO_CONFIG_STR(ClLanguagefile, cl_languagefile, 255, "", CFGFLAG_CLIENT | CFGFLAG_SAVE, "What language file to use")
MACRO_CONFIG_STR(ClSkinDownloadUrl, cl_skin_download_url, 100, "https://skins.ddnet.org/skin/", CFGFLAG_CLIENT | CFGFLAG_SAVE, "URL used to download skins")
MACRO_CONFIG_STR(ClSkinCommunityDownloadUrl, cl_skin_community_download_url, 100, "https://skins.ddnet.org/skin/community/", CFGFLAG_CLIENT | CFGFLAG_SAVE, "URL used to download community skins")
MACRO_CONFIG_INT(ClVanillaSkinsOnly, cl_vanilla_skins_only, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Only show skins available in Vanilla Teeworlds")
MACRO_CONFIG_INT(ClDownloadSkins, cl_download_skins, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Download skins from cl_skin_download_url on-the-fly")
MACRO_CONFIG_INT(ClDownloadCommunitySkins, cl_download_community_skins, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Allow to download skins created by the community. Uses cl_skin_community_download_url instead of cl_skin_download_url for the download")
MACRO_CONFIG_INT(ClAutoStatboardScreenshot, cl_auto_statboard_screenshot, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Automatically take game over statboard screenshot")
MACRO_CONFIG_INT(ClAutoStatboardScreenshotMax, cl_auto_statboard_screenshot_max, 10, 0, 1000, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Maximum number of automatically created statboard screenshots (0 = no limit)")

MACRO_CONFIG_INT(ClDefaultZoom, cl_default_zoom, 10, 0, 20, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Default zoom level")
MACRO_CONFIG_INT(ClSmoothZoomTime, cl_smooth_zoom_time, 250, 0, 5000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Time of smooth zoom animation ingame in ms (0 for off)")
MACRO_CONFIG_INT(ClLimitMaxZoomLevel, cl_limit_max_zoom_level, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Specifies, if zooming ingame should be limited or not (0 = no limit)")

MACRO_CONFIG_INT(ClPlayerUseCustomColor, player_use_custom_color, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Toggles usage of custom colors")
MACRO_CONFIG_COL(ClPlayerColorBody, player_color_body, 65408, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_COLLIGHT | CFGFLAG_INSENSITIVE, "Player body color")
MACRO_CONFIG_COL(ClPlayerColorFeet, player_color_feet, 65408, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_COLLIGHT | CFGFLAG_INSENSITIVE, "Player feet color")
MACRO_CONFIG_STR(ClPlayerSkin, player_skin, 24, "default", CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Player skin")
MACRO_CONFIG_INT(ClPlayerDefaultEyes, player_default_eyes, 0, 0, 5, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Player eyes when joining server. 0 = normal, 1 = pain, 2 = happy, 3 = surprise, 4 = angry, 5 = blink")
MACRO_CONFIG_STR(ClSkinPrefix, cl_skin_prefix, 12, "", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Replace the skins by skins with this prefix (e.g. kitty, santa)")
MACRO_CONFIG_INT(ClFatSkins, cl_fat_skins, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable fat skins")

MACRO_CONFIG_INT(UiPage, ui_page, 9, 6, 10, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Interface page")
MACRO_CONFIG_INT(UiSettingsPage, ui_settings_page, 0, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Interface settings page")
MACRO_CONFIG_INT(UiToolboxPage, ui_toolbox_page, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Toolbox page")
MACRO_CONFIG_STR(UiServerAddress, ui_server_address, 1024, "localhost:8303", CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Interface server address")
MACRO_CONFIG_INT(UiMousesens, ui_mousesens, 200, 1, 100000, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Mouse sensitivity for menus/editor")
MACRO_CONFIG_INT(UiControllerSens, ui_controller_sens, 100, 1, 100000, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Controller sensitivity for menus/editor")
MACRO_CONFIG_INT(UiSmoothScrollTime, ui_smooth_scroll_time, 500, 0, 5000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Time of smooth scrolling animation in menus/editor in ms (0 for off)")

MACRO_CONFIG_COL(UiColor, ui_color, 0xE4A046AF, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_COLALPHA, "Interface color") // 160 70 175 228 hasalpha

MACRO_CONFIG_INT(UiColorizePing, ui_colorize_ping, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Highlight ping")
MACRO_CONFIG_INT(UiColorizeGametype, ui_colorize_gametype, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Highlight gametype")

MACRO_CONFIG_INT(UiCloseWindowAfterChangingSetting, ui_close_window_after_changing_setting, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Close window after changing setting")
MACRO_CONFIG_INT(UiUnreadNews, ui_unread_news, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether there is unread news")

MACRO_CONFIG_INT(GfxNoclip, gfx_noclip, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Disable clipping")

// dummy
MACRO_CONFIG_STR(ClDummyName, dummy_name, 16, "", CFGFLAG_SAVE | CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Name of the dummy")
MACRO_CONFIG_STR(ClDummyClan, dummy_clan, 12, "", CFGFLAG_SAVE | CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Clan of the dummy")
MACRO_CONFIG_INT(ClDummyCountry, dummy_country, -1, -1, 1000, CFGFLAG_SAVE | CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Country of the Dummy")
MACRO_CONFIG_INT(ClDummyUseCustomColor, dummy_use_custom_color, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Toggles usage of custom colors")
MACRO_CONFIG_COL(ClDummyColorBody, dummy_color_body, 65408, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_COLLIGHT | CFGFLAG_INSENSITIVE, "Dummy body color")
MACRO_CONFIG_COL(ClDummyColorFeet, dummy_color_feet, 65408, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_COLLIGHT | CFGFLAG_INSENSITIVE, "Dummy feet color")
MACRO_CONFIG_STR(ClDummySkin, dummy_skin, 24, "default", CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Dummy skin")
MACRO_CONFIG_INT(ClDummyDefaultEyes, dummy_default_eyes, 0, 0, 5, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Dummy eyes when joining server. 0 = normal, 1 = pain, 2 = happy, 3 = surprise, 4 = angry, 5 = blink")
MACRO_CONFIG_INT(ClDummy, cl_dummy, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "0 - player / 1 - dummy")
MACRO_CONFIG_INT(ClDummyHammer, cl_dummy_hammer, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Whether dummy is hammering for a hammerfly")
MACRO_CONFIG_INT(ClDummyResetOnSwitch, cl_dummy_resetonswitch, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Whether dummy or player should stop pressing keys when you switch. 0 = off, 1 = dummy, 2 = player")
MACRO_CONFIG_INT(ClDummyRestoreWeapon, cl_dummy_restore_weapon, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Whether dummy should switch to last weapon after hammerfly")
MACRO_CONFIG_INT(ClDummyCopyMoves, cl_dummy_copy_moves, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Whether dummy should copy your moves")

// more controllable dummy command
MACRO_CONFIG_INT(ClDummyControl, cl_dummy_control, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Whether can you control dummy at the same time (cl_dummy_jump, cl_dummy_fire, cl_dummy_hook)")
MACRO_CONFIG_INT(ClDummyJump, cl_dummy_jump, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Whether dummy is jumping (requires cl_dummy_control 1)")
MACRO_CONFIG_INT(ClDummyFire, cl_dummy_fire, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Whether dummy is firing (requires cl_dummy_control 1)")
MACRO_CONFIG_INT(ClDummyHook, cl_dummy_hook, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_INSENSITIVE, "Whether dummy is hooking (requires cl_dummy_control 1)")

// start menu
MACRO_CONFIG_INT(ClShowStartMenuImages, cl_show_start_menu_images, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show start menu images")
MACRO_CONFIG_INT(ClSkipStartMenu, cl_skip_start_menu, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Skip the start menu")

// server
MACRO_CONFIG_INT(SvWarmup, sv_warmup, 0, 0, 0, CFGFLAG_SERVER, "Number of seconds to do warmup before round starts")
MACRO_CONFIG_STR(SvMotd, sv_motd, 900, "", CFGFLAG_SERVER, "Message of the day to display for the clients")
MACRO_CONFIG_INT(SvTournamentMode, sv_tournament_mode, 0, 0, 1, CFGFLAG_SERVER, "Tournament mode. When enabled, players joins the server as spectator")
MACRO_CONFIG_INT(SvSpamprotection, sv_spamprotection, 1, 0, 1, CFGFLAG_SERVER, "Spam protection")

MACRO_CONFIG_INT(SvSpectatorSlots, sv_spectator_slots, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "Number of slots to reserve for spectators")
MACRO_CONFIG_INT(SvInactiveKickTime, sv_inactivekick_time, 0, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before taking care of inactive players")
MACRO_CONFIG_INT(SvInactiveKick, sv_inactivekick, 0, 0, 2, CFGFLAG_SERVER, "How to deal with inactive players (0=move to spectator, 1=move to free spectator slot/kick, 2=kick)")

MACRO_CONFIG_INT(SvStrictSpectateMode, sv_strict_spectate_mode, 0, 0, 1, CFGFLAG_SERVER, "Restricts information in spectator mode")
MACRO_CONFIG_INT(SvVoteSpectate, sv_vote_spectate, 1, 0, 1, CFGFLAG_SERVER, "Allow voting to move players to spectators")
MACRO_CONFIG_INT(SvVoteSpectateRejoindelay, sv_vote_spectate_rejoindelay, 3, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before a player can rejoin after being moved to spectators by vote")
MACRO_CONFIG_INT(SvVoteKick, sv_vote_kick, 1, 0, 1, CFGFLAG_SERVER, "Allow voting to kick players")
MACRO_CONFIG_INT(SvVoteKickMin, sv_vote_kick_min, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "Minimum number of players required to start a kick vote")
MACRO_CONFIG_INT(SvVoteKickBantime, sv_vote_kick_bantime, 5, 0, 1440, CFGFLAG_SERVER, "The time in seconds to ban a player if kicked by vote. 0 makes it just use kick")
MACRO_CONFIG_INT(SvJoinVoteDelay, sv_join_vote_delay, 300, 0, 1000, CFGFLAG_SERVER, "Add a delay before recently joined players can call any vote or participate in a kick/spec vote (in seconds)")
MACRO_CONFIG_INT(SvOldTeleportWeapons, sv_old_teleport_weapons, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Teleporting of all weapons (deprecated, use special entities instead)")
MACRO_CONFIG_INT(SvOldTeleportHook, sv_old_teleport_hook, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Hook through teleporter (deprecated, use special entities instead)")
MACRO_CONFIG_INT(SvTeleportHoldHook, sv_teleport_hold_hook, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Hold hook when teleported")
MACRO_CONFIG_INT(SvTeleportLoseWeapons, sv_teleport_lose_weapons, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Lose weapons when teleported (useful for some race maps)")
MACRO_CONFIG_INT(SvDeepfly, sv_deepfly, 1, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Allow fire non auto weapons when deep")
MACRO_CONFIG_INT(SvDestroyBulletsOnDeath, sv_destroy_bullets_on_death, 1, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Destroy bullets when their owner dies")
MACRO_CONFIG_INT(SvDestroyLasersOnDeath, sv_destroy_lasers_on_death, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Destroy lasers when their owner dies")

MACRO_CONFIG_INT(SvMapUpdateRate, sv_mapupdaterate, 5, 1, 100, CFGFLAG_SERVER, "64 player id <-> vanilla id players map update rate")

MACRO_CONFIG_STR(SvServerType, sv_server_type, 64, "none", CFGFLAG_SERVER, "Type of the server (novice, moderate, ...)")

MACRO_CONFIG_INT(SvSendVotesPerTick, sv_send_votes_per_tick, 5, 1, 15, CFGFLAG_SERVER, "Number of vote options being send per tick")

MACRO_CONFIG_INT(SvRescue, sv_rescue, 0, 0, 1, CFGFLAG_SERVER, "Allow /rescue command so players can teleport themselves out of freeze (setting only works in initial config)")
MACRO_CONFIG_INT(SvRescueDelay, sv_rescue_delay, 1, 0, 1000, CFGFLAG_SERVER, "Number of seconds between two rescues")
MACRO_CONFIG_INT(SvPractice, sv_practice, 1, 0, 1, CFGFLAG_SERVER, "Enable practice mode for teams. Means you can use /rescue, but in turn your rank doesn't count.")

MACRO_CONFIG_INT(ClVideoPauseWithDemo, cl_video_pausewithdemo, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Pause video rendering when demo playing pause")
MACRO_CONFIG_INT(ClVideoShowhud, cl_video_showhud, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame HUD when rendering video")
MACRO_CONFIG_INT(ClVideoShowChat, cl_video_showchat, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show chat when rendering video")
MACRO_CONFIG_INT(ClVideoSndEnable, cl_video_sound_enable, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Use sound when rendering video")
MACRO_CONFIG_INT(ClVideoShowHookCollOther, cl_video_show_hook_coll_other, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show other players' hook collision lines when rendering video")
MACRO_CONFIG_INT(ClVideoShowDirection, cl_video_show_direction, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show players' key presses when rendering video (1 = other players', 2 = also your own)")
MACRO_CONFIG_INT(ClVideoX264Crf, cl_video_crf, 18, 0, 51, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Set crf when encode video with libx264 (0 for highest quality, 51 for lowest)")
MACRO_CONFIG_INT(ClVideoX264Preset, cl_video_preset, 5, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Set preset when encode video with libx264, default is 5 (medium), 0 is ultrafast, 9 is placebo (the slowest, not recommend)")

// debug
#ifdef CONF_DEBUG // this one can crash the server if not used correctly
MACRO_CONFIG_INT(DbgDummies, dbg_dummies, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "(Debug build only)")
#endif

MACRO_CONFIG_INT(DbgTuning, dbg_tuning, 0, 0, 2, CFGFLAG_CLIENT, "Display information about the tuning parameters that affect the own player (0 = off, 1 = show changed, 2 = show all)")

#endif
