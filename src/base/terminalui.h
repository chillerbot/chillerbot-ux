#ifndef BASE_TERMINALUI_H
#define BASE_TERMINALUI_H

#if defined(CONF_CURSES_CLIENT)

#include <base/curses.h>
#include <base/io.h>
#include <base/math.h>
#include <base/str.h>
#include <base/types.h>

#define CHILLER_LOGGER_WIDTH 1024 * 4
#define CHILLER_LOGGER_HEIGHT 2048

#define INFO_WIN_HEIGHT 3

class CTermWindow
{
	char m_aTextTopYellow[128];
	char m_aTextTop[128];

public:
	CTermWindow();
	void SetTextTopLeftYellow(const char *pText);
	void SetTextTopLeft(const char *pText);
	void DrawBorders();
	void DrawBorders(int x, int y, int w, int h);

	WINDOW *m_pCursesWin;
};

class CInputWindow : public CTermWindow
{
public:
	CInputWindow()
	{
		CTermWindow();
		m_Search = false;
		m_CursorOffset = 0;
		Open();
		OpenMenu();
		m_SelectedMenuItem = MENU_ITEM_BROWSER;
	}
	bool m_Search;
	int m_CursorOffset;
	int m_Height;
	bool m_Active;
	bool m_Menu;
	int m_SelectedMenuItem;
	enum
	{
		MENU_ITEM_BROWSER,
		MENU_ITEM_HELP,
		MENU_ITEM_QUIT,
		NUM_MENU_ITEMS,
	};
	void OpenMenu()
	{
		m_Menu = true;
		// curs_set(0); // hiding cursor does not work
	}
	/*
		CloseMenu

		do not call directly if you want to
		see the result instantly

		call the wrapper CTerminalUI::CloseMenu()
		instead
	*/
	void CloseMenu()
	{
		m_Menu = false;
	}
	void Open()
	{
		m_Height = 3;
		m_Active = true;
		CloseMenu();
	}
	void Close()
	{
		m_Height = 0;
		m_Active = false;
		CloseMenu();
	}
	// TODO: move the whole menu thing out of base
	//       into components
	//       only search needs to be here
	//       since it is part of the border
	//       the menu is content
	void NextMenuItem()
	{
		if(!IsMenu())
			return;
		m_SelectedMenuItem = std::clamp(m_SelectedMenuItem + 1, 0, (int)(NUM_MENU_ITEMS - 1));
		DrawBorders();
		refresh();
	}
	void PrevMenuItem()
	{
		if(!IsMenu())
			return;
		m_SelectedMenuItem = std::clamp(m_SelectedMenuItem - 1, 0, (int)NUM_MENU_ITEMS);
		DrawBorders();
		refresh();
	}
	int Item() { return m_SelectedMenuItem; }
	bool IsActive() { return m_Active; }
	bool IsMenu() { return m_Menu; }
	/**
	 * Draw menu option in the input window
	 *
	 * @param pName name of the menu item
	 * @param Active is the item selected?
	 * @param Offset current render offset
	 *
	 * @return returns the x coordinate on the screen
	 *         of the end. Which can be used by the next
	 *         item to start drawing there.
	 */
	int RenderMenuItem(const char *pName, bool Active, int Offset);
	bool IsSearch() { return m_Search; }
	void SetSearch(bool Search)
	{
		m_Search = Search;
		m_CursorOffset = Search ? 3 : 0;
	}
	void DrawBorders();
};

extern CTermWindow g_LogWindow;
extern CTermWindow g_GameWindow;
extern CTermWindow g_InfoWin;
extern CInputWindow g_InputWin;

extern int g_ParentX;
extern int g_ParentY;
extern int g_NewX;
extern int g_NewY;
extern bool g_TriggerResize;
extern char g_aInfoStr[1024];
extern char g_aInfoStr2[1024];
extern char g_aInputStr[1024];
extern bool gs_NeedLogDraw;
extern int gs_LogsPushed;
/*
	gs_LogScroll

	print offset of the log.
	to keep track of scrolling to older pages.
*/
extern int gs_LogScroll;
extern int gs_LogNumFilled;
extern IOHANDLE gs_Logfile;

struct SLOG_COLOR
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

class CCursesLogMsg
{
public:
	CCursesLogMsg()
	{
		m_aMessage[0] = '\0';
		m_HaveColor = false;
		m_Color.r = 0;
		m_Color.g = 0;
		m_Color.b = 0;
	}
	char m_aMessage[CHILLER_LOGGER_WIDTH];
	bool m_HaveColor;
	SLOG_COLOR m_Color;
};

void curses_init();
void log_draw();

/*
	set_curses_log_scroll

	set the bottom offset of the log scroll
	0 is latest message
	and CHILLER_LOGGER_WIDTH - (terminal height)
	is oldest message
*/
int set_curses_log_scroll(int offset);
int get_curses_log_scroll();
int scroll_curses_log(int offset);
void curses_refresh_windows();
void curses_log_push(const char *pStr, const SLOG_COLOR *pColor = nullptr);
// void curses_logf(const char *sys, const char *fmt, ...);

#endif

#endif
