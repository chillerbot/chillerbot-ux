#ifndef GAME_CLIENT_COMPONENTS_CHILLERBOT_UNIX_H
#define GAME_CLIENT_COMPONENTS_CHILLERBOT_UNIX_H

#include <engine/console.h>

#include <game/client/component.h>

class CUnix : public CComponent
{
	void OnConsoleInit() override;
	void OnInit() override;

	void ls();

	static int ListDirCallback(const CFsFileInfo *pInfo, int IsDir, int StorageType, void *pUser);

	static void ConLs(IConsole::IResult *pResult, void *pUserData);

public:
	int Sizeof() const override { return sizeof(*this); }
};

#endif
