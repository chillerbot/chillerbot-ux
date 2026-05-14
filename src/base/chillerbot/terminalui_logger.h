#ifndef BASE_CHILLERBOT_TERMINALUI_LOGGER_H
#define BASE_CHILLERBOT_TERMINALUI_LOGGER_H

#if defined(CONF_CURSES_CLIENT)

#include <base/aio.h>
#include <base/color.h>
#include <base/logger.h>
#include <base/terminalui.h>

#include <cstdio>

#if defined(CONF_FAMILY_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 /* required for mingw to get getaddrinfo to work */
#include <windows.h>
#else
#include <unistd.h>
#endif

class CTerminalUILogger : public ILogger
{
	ASYNCIO *m_pAio;
	bool m_AnsiTruecolor;
	bool m_Close;

public:
	CTerminalUILogger(IOHANDLE File, bool AnsiTruecolor, bool Close) :
		m_pAio(aio_new(File)),
		m_AnsiTruecolor(AnsiTruecolor),
		m_Close(Close)
	{
	}
	void Log(const CLogMessage *pMessage) override
	{
		aio_lock(m_pAio);
		if(m_AnsiTruecolor && pMessage->m_HaveColor)
		{
			SLOG_COLOR Color;
			Color.r = pMessage->m_Color.r;
			Color.g = pMessage->m_Color.g;
			Color.b = pMessage->m_Color.b;
			curses_log_push(pMessage->m_aLine, &Color);
		}
		else
		{
			curses_log_push(pMessage->m_aLine);
		}
		aio_unlock(m_pAio);
	}
	~CTerminalUILogger()
	{
		if(m_Close)
		{
			aio_close(m_pAio);
		}
		aio_wait(m_pAio);
		aio_free(m_pAio);
	}
	void GlobalFinish() override
	{
		if(m_Close)
		{
			aio_close(m_pAio);
		}
		aio_wait(m_pAio);
	}
};

#endif

#endif
