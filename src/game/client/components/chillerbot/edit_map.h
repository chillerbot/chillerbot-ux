#ifndef GAME_CLIENT_COMPONENTS_CHILLERBOT_EDIT_MAP_H
#define GAME_CLIENT_COMPONENTS_CHILLERBOT_EDIT_MAP_H

#include <base/str.h>

#include <engine/console.h>
#include <engine/shared/jobs.h>

#include <game/client/component.h>

class CEditMapJob : public IJob
{
	char m_aSource[IO_MAX_PATH_LENGTH];
	char m_aDestination[IO_MAX_PATH_LENGTH];
	int m_X;
	int m_Y;
	int m_Index;

public:
	CEditMapJob(const char *pSource, const char *pDestination, int x, int y, int Index)
	{
		str_copy(m_aSource, pSource, sizeof(m_aSource));
		str_copy(m_aDestination, pDestination, sizeof(m_aDestination));
		m_X = x;
		m_Y = y;
		m_Index = Index;
		m_State = STATE_RUNNING;
	}
	void Run() override;

	enum
	{
		STATE_DONE,
		STATE_RUNNING,
	};

	int m_State;

	int State() const { return m_State; }
};

class CEditMap : public CComponent
{
	void OnRender() override;
	void OnInit() override;
	void OnShutdown() override;
	void OnConsoleInit() override;

	static void ConSetGametile(IConsole::IResult *pResult, void *pUserData);

	std::shared_ptr<CEditMapJob> m_pSetGameTileJob;

	void GetEditedMapPath(char *pPath, int Size);
	void GetSourceMapPath(char *pPath, int Size);

public:
	int Sizeof() const override { return sizeof(*this); }

	CEditMap();
};

#endif
