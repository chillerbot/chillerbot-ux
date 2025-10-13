#ifndef GAME_CLIENT_COMPONENTS_CHILLERBOT_REMOTECONTROL_H
#define GAME_CLIENT_COMPONENTS_CHILLERBOT_REMOTECONTROL_H

#include <game/client/component.h>

class CRemoteControl : public CComponent
{
	void ExecuteWhitelisted(const char *pCommand, const char *pWhitelistFile = "chillerbot/rc_commands.txt");

	void OnChatMessage(int ClientId, int Team, const char *pMsg);

	void OnMessage(int MsgType, void *pRawMsg) override;

public:
	int Sizeof() const override { return sizeof(*this); }
};

#endif
