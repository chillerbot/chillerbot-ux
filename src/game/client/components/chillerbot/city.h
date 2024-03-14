#ifndef GAME_CLIENT_COMPONENTS_CHILLERBOT_CITY_H
#define GAME_CLIENT_COMPONENTS_CHILLERBOT_CITY_H

#include <game/client/component.h>

class CCityHelper : public CComponent
{
private:
	virtual void OnRender() override;
	virtual void OnInit() override;
	virtual void OnConsoleInit() override;
	virtual void OnMessage(int MsgType, void *pRawMsg) override;

	static void ConchainShowWallet(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);

	static void ConAutoDropMoney(IConsole::IResult *pResult, void *pUserData);

	void SetAutoDrop(bool Drop, int Delay, int ClientId);
	void OnServerMsg(const char *pMsg);
	void OnChatMsg(int ClientId, int Team, const char *pMsg);
	int ClosestClientIdToPos(vec2 Pos, int Dummy);

	bool m_AutoDropMoney[NUM_DUMMIES];

	int m_WalletMoney[NUM_DUMMIES];
	int64_t m_NextWalletDrop[NUM_DUMMIES];
	int m_WalletDropDelay[NUM_DUMMIES];
	int m_LastDummy;

	void AddWalletEntry(std::vector<std::pair<std::string, int>> *pVec, const std::pair<std::string, int> &Entry);
	void SetWalletMoney(int Money, int ClientId = -1);
	void AddWalletMoney(int Money, int ClientId = -1);

public:
	virtual int Sizeof() const override { return sizeof(*this); }

	std::vector<std::pair<std::string, int>> m_vWalletMain;
	std::vector<std::pair<std::string, int>> m_vWalletDummy;

	void PrintWalletToChat(int ClientId = -1, const char *pWhisper = NULL);
	void DropAllMoney(int ClientId);
	int WalletMoney(int ClientId = -1);
};

#endif
