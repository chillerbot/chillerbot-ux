#ifndef GAME_CLIENT_COMPONENTS_CHILLERBOT_MMOTEE_H
#define GAME_CLIENT_COMPONENTS_CHILLERBOT_MMOTEE_H

#include <game/client/component.h>

class CMmoTee : public CComponent
{
private:
	virtual void OnRender() override;
	virtual void OnInit() override;
	virtual void OnConsoleInit() override;
	virtual void OnMessage(int MsgType, void *pRawMsg) override;

public:
	virtual int Sizeof() const override { return sizeof(*this); }

	bool FilterChat(int ClientId, int Team, const char *pLine);
};

#endif
