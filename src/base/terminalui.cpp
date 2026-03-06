#if defined(CONF_CURSES_CLIENT)

#include "terminalui.h"

#include "chillerbot/curses_colors.h"

#include <base/curses.h>
#include <base/math.h>
#include <base/system.h>

static CCursesLogMsg gs_aChillerLogger[CHILLER_LOGGER_HEIGHT];

CTermWindow g_LogWindow;
CTermWindow g_GameWindow;
CTermWindow g_InfoWin;
CInputWindow g_InputWin;

int g_ParentX;
int g_ParentY;
int g_NewX;
int g_NewY;
bool g_TriggerResize;
char g_aInfoStr[1024];
char g_aInfoStr2[1024];
char g_aInputStr[1024];
bool gs_NeedLogDraw;
int gs_LogsPushed = 0;
int gs_LogScroll = 0;
int gs_LogNumFilled = 0;
IOHANDLE gs_Logfile = 0;

void curses_init()
{
	for(auto &Msg : gs_aChillerLogger)
		Msg.m_aMessage[0] = '\0';
}

void curses_refresh_windows()
{
	if(gs_NeedLogDraw)
	{
		wrefresh(g_LogWindow.m_pCursesWin);
		wclear(g_LogWindow.m_pCursesWin);
		g_LogWindow.DrawBorders();
	}
	wrefresh(g_InfoWin.m_pCursesWin);
	if(g_InputWin.IsActive())
		wrefresh(g_InputWin.m_pCursesWin);
	gs_NeedLogDraw = false;
}

int set_curses_log_scroll(int offset)
{
	int _x, y;
	getmaxyx(g_LogWindow.m_pCursesWin, y, _x);
	offset = std::clamp(offset, 0, gs_LogNumFilled - y);
	gs_LogScroll = offset;
	return gs_LogScroll;
}

int get_curses_log_scroll()
{
	return gs_LogScroll;
}

int scroll_curses_log(int offset)
{
	return set_curses_log_scroll(get_curses_log_scroll() + offset);
}

void log_draw()
{
	if(!gs_NeedLogDraw)
		return;
	int x, y;
	getmaxyx(g_LogWindow.m_pCursesWin, y, x);
	int Max = CHILLER_LOGGER_HEIGHT > y ? y : CHILLER_LOGGER_HEIGHT;
	Max -= 3;
	// int Top = CHILLER_LOGGER_HEIGHT - 2;
	// int line = 0;

	/*
		Bottom

		lowest index

		be aware that we render from highest
		index to lowest index
	*/
	int Bottom = gs_LogScroll;

	/*
		Top

		highest index

		be aware that we render from highest
		index to lowest index
	*/
	int Top = Bottom + Max;

	for(int k = Max, i = Top; i >= Bottom; i--)
	{
		if(gs_aChillerLogger[i].m_aMessage[0] == '\0')
			continue;
		char aBuf[1024 * 4 + 16];
		// str_format(aBuf, sizeof(aBuf), "%2d|%2d|%s", line++, k, gs_aChillerLogger[i].m_aMessage);
		// aBuf[x - 2] = '\0'; // prevent line wrapping and cut on screen border
		str_copy(aBuf, gs_aChillerLogger[i].m_aMessage, x - 2);

		if(gs_aChillerLogger[i].m_HaveColor)
		{
			int ColorPair = rgb_to_text_color_pair(gs_aChillerLogger[i].m_Color.r, gs_aChillerLogger[i].m_Color.g, gs_aChillerLogger[i].m_Color.b);
			wattron(g_LogWindow.m_pCursesWin, COLOR_PAIR(ColorPair));
			// refresh();
		}

		mvwprintw(g_LogWindow.m_pCursesWin, Max - k-- + 1, 1, "%s", aBuf);

		if(gs_aChillerLogger[i].m_HaveColor)
		{
			wattron(g_LogWindow.m_pCursesWin, COLOR_PAIR(WHITE_ON_BLACK));
			// refresh();
		}
	}
}

CTermWindow::CTermWindow()
{
	m_aTextTopYellow[0] = '\0';
	m_aTextTop[0] = '\0';
	// str_copy(m_aTextTop, "gaming", sizeof(m_aTextTop));
	m_pCursesWin = nullptr;
}

void CTermWindow::SetTextTopLeft(const char *pText)
{
	unsigned int i = 0;
	const char *pCursor = pText;
	while(true)
	{
		if(*pCursor == 0)
			break;
		if(i + 1 >= sizeof(m_aTextTop))
			break;
		// TODO: think of something cool for multi line broadcast
		if(*pCursor == '\n')
			m_aTextTop[i++] = ' ';
		else
			m_aTextTop[i++] = *pCursor;
		pCursor++;
	}
	m_aTextTop[i] = '\0';
	// str_copy(m_aTextTop, pText, sizeof(m_aTextTop));
}

void CTermWindow::SetTextTopLeftYellow(const char *pText)
{
	unsigned int i = 0;
	const char *pCursor = pText;
	while(true)
	{
		if(*pCursor == 0)
			break;
		if(i + 1 >= sizeof(m_aTextTopYellow))
			break;
		// TODO: think of something cool for multi line broadcast
		if(*pCursor == '\n')
			m_aTextTopYellow[i++] = ' ';
		else
			m_aTextTopYellow[i++] = *pCursor;
		pCursor++;
	}
	m_aTextTopYellow[i] = '\0';
	// str_copy(m_aTextTop, pText, sizeof(m_aTextTop));
}

void CTermWindow::DrawBorders(int x, int y, int w, int h)
{
	WINDOW *screen = m_pCursesWin;
	// 4 corners
	mvwprintw(screen, y, x, "+");
	mvwprintw(screen, y + h - 1, x, "+");
	mvwprintw(screen, y, x + w - 1, "+");
	mvwprintw(screen, y + h - 1, x + w - 1, "+");

	// sides
	// for(int i = 1; i < ((y + h) - 1); i++)
	// {
	// 	mvwprintw(screen, i, y, "|");
	// 	mvwprintw(screen, i, (x + w) - 1, "|");
	// }

	// top and bottom
	for(int i = 1; i < (w - 1); i++)
	{
		mvwprintw(screen, y, x + i, "-");
		mvwprintw(screen, (y + h) - 1, x + i, "-");
	}
}

void CTermWindow::DrawBorders()
{
	WINDOW *screen = m_pCursesWin;
	int x, y, i;

	getmaxyx(screen, y, x);

	// 4 corners
	mvwprintw(screen, 0, 0, "+");
	mvwprintw(screen, y - 1, 0, "+");
	mvwprintw(screen, 0, x - 1, "+");
	mvwprintw(screen, y - 1, x - 1, "+");

	// sides
	for(i = 1; i < (y - 1); i++)
	{
		mvwprintw(screen, i, 0, "|");
		mvwprintw(screen, i, x - 1, "|");
	}

	// top and bottom
	for(i = 1; i < (x - 1); i++)
	{
		mvwprintw(screen, 0, i, "-");
		mvwprintw(screen, y - 1, i, "-");
	}

	// title
	if(x < 8)
		return;

	int TitleOffset = 1;
	char aBuf[1024 * 4];
	if(m_aTextTop[0] != '\0' || m_aTextTopYellow[0] != '\0')
	{
		mvwprintw(screen, 0, TitleOffset, "%s", "-[ ");
		TitleOffset += str_length("-[ ");
	}
	int AvailableWidth = x - 3;
	// for poor mans goto
	while(true)
	{
		if(m_aTextTopYellow[0] != '\0')
		{
			wattron(screen, COLOR_PAIR(YELLOW_ON_BLACK));
			wattron(screen, A_BOLD);
			str_copy(aBuf, m_aTextTopYellow, sizeof(aBuf));
			aBuf[std::clamp(AvailableWidth - TitleOffset, 0, (int)sizeof(aBuf))] = '\0';
			mvwprintw(screen, 0, TitleOffset, "%s", aBuf);
			TitleOffset += str_length(aBuf);
			refresh();
			wattroff(screen, A_BOLD);
			wattroff(screen, COLOR_PAIR(CYAN_ON_BLACK));
			if(TitleOffset >= AvailableWidth)
			{
				TitleOffset = AvailableWidth - 1;
				break;
			}
		}
		if(m_aTextTop[0] != '\0')
		{
			str_copy(aBuf, m_aTextTop, sizeof(aBuf));
			aBuf[std::clamp(AvailableWidth - TitleOffset, 0, (int)sizeof(aBuf))] = '\0';
			mvwprintw(screen, 0, TitleOffset, "%s", aBuf);
			TitleOffset += str_length(aBuf);
			if(TitleOffset >= AvailableWidth)
			{
				TitleOffset = AvailableWidth - 1;
				break;
			}
		}
		break;
	}
	if(m_aTextTop[0] != '\0' || m_aTextTopYellow[0] != '\0')
	{
		mvwprintw(screen, 0, TitleOffset, "%s", " ]-");
	}
}

void CInputWindow::DrawBorders()
{
	if(!IsActive())
		return;

	CTermWindow::DrawBorders();

	if(IsSearch())
	{
		mvwprintw(m_pCursesWin, 0, 0, "%s", "+--+");
		mvwprintw(m_pCursesWin, 1, 0, "%s", "|🔍|");
		mvwprintw(m_pCursesWin, 2, 0, "%s", "+--+");
	}
	else if(IsMenu())
	{
		int offset = 1;
		offset = RenderMenuItem("browser", m_SelectedMenuItem == MENU_ITEM_BROWSER, offset);
		offset = RenderMenuItem("help", m_SelectedMenuItem == MENU_ITEM_HELP, offset);
		RenderMenuItem("quit", m_SelectedMenuItem == MENU_ITEM_QUIT, offset);
	}
}

int CInputWindow::RenderMenuItem(const char *pName, bool Active, int Offset)
{
	if(Active)
	{
		wattron(m_pCursesWin, COLOR_PAIR(YELLOW_ON_BLACK));
		wattron(m_pCursesWin, A_BOLD);
	}
	mvwprintw(m_pCursesWin, 1, Offset, " [%s]", pName);
	if(Active)
	{
		refresh();
		wattroff(m_pCursesWin, A_BOLD);
		wattroff(m_pCursesWin, COLOR_PAIR(CYAN_ON_BLACK));
	}
	return str_length(pName) + Offset + 3;
}

void curses_log_push(const char *pStr, const SLOG_COLOR *pColor)
{
	// if ncurses is not initialized yet (terminalui.OnInit()) just print to stdout
	if(!g_LogWindow.m_pCursesWin)
	{
		puts(pStr);
		return;
	}
	if(gs_LogNumFilled < CHILLER_LOGGER_HEIGHT)
		gs_LogNumFilled++;
	str_copy(g_aInfoStr, "?=help", sizeof(g_aInfoStr));
	gs_NeedLogDraw = true;
	// shift all
	for(int i = CHILLER_LOGGER_HEIGHT - 1; i > 0; i--)
		str_copy(gs_aChillerLogger[i].m_aMessage, gs_aChillerLogger[i - 1].m_aMessage, sizeof(gs_aChillerLogger[i].m_aMessage));
	// insert newest on the bottom
	str_copy(gs_aChillerLogger[0].m_aMessage, pStr, sizeof(gs_aChillerLogger[0].m_aMessage));
	gs_aChillerLogger[0].m_HaveColor = pColor != nullptr;
	if(gs_aChillerLogger[0].m_HaveColor)
	{
		gs_aChillerLogger[0].m_Color.r = pColor->r;
		gs_aChillerLogger[0].m_Color.g = pColor->g;
		gs_aChillerLogger[0].m_Color.b = pColor->b;
	}
	gs_LogsPushed++;
	// scared of integer overflows xd
	if(gs_LogsPushed > 1000)
		gs_LogsPushed = 0;
}

// ChillerDragon: no fucking idea why on macOS vdbg needs it but dbg doesn't
//				  yes this is a format vuln but only caused if used wrong same as in dbg_msg
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
void curses_logf(const char *sys, const char *fmt, ...)
{
	va_list args;
	char str[1024 * 4];
	char *msg;
	int len;

	char timestr[80];
	str_timestamp_format(timestr, sizeof(timestr), TimestampFormat::SPACE);

	str_format(str, sizeof(str), "[%s][%s]: ", timestr, sys);

	len = strlen(str);
	msg = (char *)str + len;

	va_start(args, fmt);
#if defined(CONF_FAMILY_WINDOWS) && !defined(__GNUC__)
	_vsprintf_p(msg, sizeof(str) - len, fmt, args);
#else
	vsnprintf(msg, sizeof(str) - len, fmt, args);
#endif
	va_end(args);

	// printf("%s\n", str);
	curses_log_push(str);
	if(gs_Logfile)
	{
		io_write(gs_Logfile, str, str_length(str));
		io_write(gs_Logfile, "\n", 1);
	}
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
