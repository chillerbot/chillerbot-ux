#ifndef GAME_CLIENT_COMPONENTS_CHILLERBOT_VIBEBOT_H
#define GAME_CLIENT_COMPONENTS_CHILLERBOT_VIBEBOT_H

#include <base/time.h>

#include <engine/client/enums.h>
#include <engine/console.h>
#include <engine/shared/protocol.h>

#include <generated/protocol.h>

#include <game/client/component.h>
#include <game/mapitems.h>

class CVibeBot : public CComponent
{
private:
	void OnRender() override;
	void OnInit() override;
	void OnConsoleInit() override;

	static void ConVibe(IConsole::IResult *pResult, void *pUserData);
	static void ConVibes(IConsole::IResult *pResult, void *pUserData);
	static void ConUnVibe(IConsole::IResult *pResult, void *pUserData);
	static void ConEmoteBot(IConsole::IResult *pResult, void *pUserData);

	void Reset();

	void UpdateComponents();

	int m_Mode[NUM_DUMMIES];
	int m_EmoteBot[NUM_DUMMIES];
	int m_EmoteBotDelay[NUM_DUMMIES];
	int m_MoveId;

	enum
	{
		E_MUSIC = 5,
		E_GHOST = 7,
		E_SLEEPY = 12,
		E_WTF = 13,
		E_HAPPY = 14,
	};

	void VibeEmote(int Emoticon);
	void EmoteBotTick();

public:
	int Sizeof() const override { return sizeof(*this); }

	CNetObj_Character *GetCharacter() const;
	void Emote(int Emoticon);
	void Aim(int TargetX, int TargetY);
	bool SecsPassed(int Secs) { return (time_get() / time_freq()) % Secs == 0; }
	void AimTick();

	int64_t m_NextEmoteBot[NUM_DUMMIES];
	int64_t m_NextEmote[NUM_DUMMIES];
	int64_t m_NextAim[NUM_DUMMIES];
	int64_t m_NextAimMove[NUM_DUMMIES];
	vec2 m_CurrentAim[NUM_DUMMIES];
	vec2 m_WantedAim[NUM_DUMMIES];

	int MoveId() const { return m_MoveId; }
	bool IsVibing(int ClientId) { return m_Mode[ClientId] != VB_OFF; }

	enum
	{
		VB_OFF,
		VB_HAPPY,
		VB_GHOST,
		VB_MUSIC,
		VB_SLEEPY,
		VB_WTF,
	};

	enum
	{
		EB_OFF,
	};

	void SetMode(int Mode, int ClientId);
	void SetEmoteBot(int Mode, int Delay, int ClientId);
	void PrintInput(const CNetObj_PlayerInput *pInput);

	CNetObj_PlayerInput m_aInputData[NUM_DUMMIES];
	bool m_SendData[NUM_DUMMIES];
};

#endif
