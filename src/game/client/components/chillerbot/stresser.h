#ifndef GAME_CLIENT_COMPONENTS_CHILLERBOT_STRESSER_H
#define GAME_CLIENT_COMPONENTS_CHILLERBOT_STRESSER_H

#include <engine/client/enums.h>
#include <engine/console.h>

#include <generated/protocol.h>

#include <game/client/component.h>

class CStresser : public CComponent
{
public:
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;

	CNetObj_PlayerInput m_aInputData[NUM_DUMMIES];
	bool m_SendData[NUM_DUMMIES];

private:
	void OnRender() override;
	void OnMessage(int MsgType, void *pRawMsg) override;
	void OnMapLoad() override;
	void OnConsoleInit() override;

	static void ConchainPenTest(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);

	void OnChatMessage(int ClientId, int Team, const char *pMsg);

	bool GetPentestCommand(const char *pFilename, char *pCmd, int CmdSize);
	const char *GetRandomChatCommand();
	void RandomMovements();

	std::vector<char *> m_vChatCmds;
	int64_t m_RequestCmdlist;
	int m_PenDelay;
};
#endif
