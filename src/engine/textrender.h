/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef ENGINE_TEXTRENDER_H
#define ENGINE_TEXTRENDER_H
#include "kernel.h"

#include <base/color.h>

#include <engine/graphics.h>

#include <cstdint>

enum
{
	TEXTFLAG_RENDER = 1 << 0,
	TEXTFLAG_DISALLOW_NEWLINE = 1 << 1,
	TEXTFLAG_STOP_AT_END = 1 << 2,
	TEXTFLAG_ELLIPSIS_AT_END = 1 << 3,
};

enum ETextAlignment
{
	TEXTALIGN_LEFT = 1 << 0,
	TEXTALIGN_CENTER = 1 << 1,
	TEXTALIGN_RIGHT = 1 << 2,
};

enum ETextRenderFlags
{
	TEXT_RENDER_FLAG_NO_X_BEARING = 1 << 0,
	TEXT_RENDER_FLAG_NO_Y_BEARING = 1 << 1,
	TEXT_RENDER_FLAG_ONLY_ADVANCE_WIDTH = 1 << 2,
	TEXT_RENDER_FLAG_NO_PIXEL_ALIGMENT = 1 << 3,
	TEXT_RENDER_FLAG_KERNING = 1 << 4,
	TEXT_RENDER_FLAG_NO_OVERSIZE = 1 << 5,
	TEXT_RENDER_FLAG_NO_FIRST_CHARACTER_X_BEARING = 1 << 6,
	TEXT_RENDER_FLAG_NO_LAST_CHARACTER_ADVANCE = 1 << 7,
	TEXT_RENDER_FLAG_NO_AUTOMATIC_QUAD_UPLOAD = 1 << 8,
	// text is only rendered once and then discarded (a hint for buffer creation)
	TEXT_RENDER_FLAG_ONE_TIME_USE = 1 << 9,
};

enum
{
	TEXT_FONT_ICON_FONT = 0,
};

namespace FontIcons {
// Each font icon is named according to its official name in Font Awesome
MAYBE_UNUSED static const char *FONT_ICON_LOCK = "\xEF\x80\xA3";
MAYBE_UNUSED static const char *FONT_ICON_MAGNIFYING_GLASS = "\xEF\x80\x82";
MAYBE_UNUSED static const char *FONT_ICON_HEART = "\xEF\x80\x84";
MAYBE_UNUSED static const char *FONT_ICON_STAR = "\xEF\x80\x85";
MAYBE_UNUSED static const char *FONT_ICON_CHECK = "\xEF\x80\x8C";
MAYBE_UNUSED static const char *FONT_ICON_XMARK = "\xEF\x80\x8D";
MAYBE_UNUSED static const char *FONT_ICON_ARROW_ROTATE_LEFT = "\xEF\x83\xA2";
MAYBE_UNUSED static const char *FONT_ICON_ARROW_ROTATE_RIGHT = "\xEF\x80\x9E";
MAYBE_UNUSED static const char *FONT_ICON_CERTIFICATE = "\xEF\x82\xA3";
MAYBE_UNUSED static const char *FONT_ICON_FLAG_CHECKERED = "\xEF\x84\x9E";
MAYBE_UNUSED static const char *FONT_ICON_BAN = "\xEF\x81\x9E";
MAYBE_UNUSED static const char *FONT_ICON_CIRCLE_CHEVRON_DOWN = "\xEF\x84\xBA";

MAYBE_UNUSED static const char *FONT_ICON_HOUSE = "\xEF\x80\x95";
MAYBE_UNUSED static const char *FONT_ICON_NEWSPAPER = "\xEF\x87\xAA";
MAYBE_UNUSED static const char *FONT_ICON_POWER_OFF = "\xEF\x80\x91";
MAYBE_UNUSED static const char *FONT_ICON_GEAR = "\xEF\x80\x93";
MAYBE_UNUSED static const char *FONT_ICON_PEN_TO_SQUARE = "\xEF\x81\x84";
MAYBE_UNUSED static const char *FONT_ICON_CLAPPERBOARD = "\xEE\x84\xB1";
MAYBE_UNUSED static const char *FONT_ICON_EARTH_AMERICAS = "\xEF\x95\xBD";

MAYBE_UNUSED static const char *FONT_ICON_SLASH = "\xEF\x9C\x95";
MAYBE_UNUSED static const char *FONT_ICON_PLAY = "\xEF\x81\x8B";
MAYBE_UNUSED static const char *FONT_ICON_PAUSE = "\xEF\x81\x8C";
MAYBE_UNUSED static const char *FONT_ICON_STOP = "\xEF\x81\x8D";
MAYBE_UNUSED static const char *FONT_ICON_CHEVRON_LEFT = "\xEF\x81\x93";
MAYBE_UNUSED static const char *FONT_ICON_CHEVRON_RIGHT = "\xEF\x81\x94";
MAYBE_UNUSED static const char *FONT_ICON_BACKWARD = "\xEF\x81\x8A";
MAYBE_UNUSED static const char *FONT_ICON_FORWARD = "\xEF\x81\x8E";
MAYBE_UNUSED static const char *FONT_ICON_RIGHT_FROM_BRACKET = "\xEF\x8B\xB5";
MAYBE_UNUSED static const char *FONT_ICON_RIGHT_TO_BRACKET = "\xEF\x8B\xB6";
MAYBE_UNUSED static const char *FONT_ICON_ARROW_UP_RIGHT_FROM_SQUARE = "\xEF\x82\x8E";
MAYBE_UNUSED static const char *FONT_ICON_BACKWARD_STEP = "\xEF\x81\x88";
MAYBE_UNUSED static const char *FONT_ICON_FORWARD_STEP = "\xEF\x81\x91";
MAYBE_UNUSED static const char *FONT_ICON_KEYBOARD = "\xE2\x8C\xA8";

MAYBE_UNUSED static const char *FONT_ICON_FOLDER = "\xEF\x81\xBB";
MAYBE_UNUSED static const char *FONT_ICON_FOLDER_TREE = "\xEF\xA0\x82";
MAYBE_UNUSED static const char *FONT_ICON_FILM = "\xEF\x80\x88";
MAYBE_UNUSED static const char *FONT_ICON_MAP = "\xEF\x89\xB9";
MAYBE_UNUSED static const char *FONT_ICON_IMAGE = "\xEF\x80\xBE";
MAYBE_UNUSED static const char *FONT_ICON_MUSIC = "\xEF\x80\x81";
MAYBE_UNUSED static const char *FONT_ICON_FILE = "\xEF\x85\x9B";

MAYBE_UNUSED static const char *FONT_ICON_ARROWS_LEFT_RIGHT = "\xEF\x8C\xB7";
MAYBE_UNUSED static const char *FONT_ICON_ARROWS_UP_DOWN = "\xEF\x81\xBD";
MAYBE_UNUSED static const char *FONT_ICON_CIRCLE_PLAY = "\xEF\x85\x84";
MAYBE_UNUSED static const char *FONT_ICON_BORDER_ALL = "\xEF\xA1\x8C";
MAYBE_UNUSED static const char *FONT_ICON_EYE = "\xEF\x81\xAE";
MAYBE_UNUSED static const char *FONT_ICON_EYE_SLASH = "\xEF\x81\xB0";

MAYBE_UNUSED static const char *FONT_ICON_DICE_ONE = "\xEF\x94\xA5";
MAYBE_UNUSED static const char *FONT_ICON_DICE_TWO = "\xEF\x94\xA8";
MAYBE_UNUSED static const char *FONT_ICON_DICE_THREE = "\xEF\x94\xA7";
MAYBE_UNUSED static const char *FONT_ICON_DICE_FOUR = "\xEF\x94\xA4";
MAYBE_UNUSED static const char *FONT_ICON_DICE_FIVE = "\xEF\x94\xA3";
MAYBE_UNUSED static const char *FONT_ICON_DICE_SIX = "\xEF\x94\xA6";
} // end namespace FontIcons

class CFont;

enum ETextCursorSelectionMode
{
	// ignore any kind of selection
	TEXT_CURSOR_SELECTION_MODE_NONE = 0,
	// calculates the selection based on the mouse press and release cursor position
	TEXT_CURSOR_SELECTION_MODE_CALCULATE,
	// sets the selection based on the character start and end count(these values have to be decoded character offsets)
	TEXT_CURSOR_SELECTION_MODE_SET,
};

enum ETextCursorCursorMode
{
	// ignore any kind of cursor
	TEXT_CURSOR_CURSOR_MODE_NONE = 0,
	// calculates the cursor based on the mouse release cursor position
	TEXT_CURSOR_CURSOR_MODE_CALCULATE,
	// sets the cursor based on the current character (this value has to be decoded character offset)
	TEXT_CURSOR_CURSOR_MODE_SET,
};

class CTextCursor
{
public:
	int m_Flags;
	int m_LineCount;
	int m_GlyphCount;
	int m_CharCount;
	int m_MaxLines;

	float m_StartX;
	float m_StartY;
	float m_LineWidth;
	float m_X, m_Y;
	float m_MaxCharacterHeight;

	float m_LongestLineWidth;

	CFont *m_pFont;
	float m_FontSize;
	float m_AlignedFontSize;

	ETextCursorSelectionMode m_CalculateSelectionMode;

	// these coordinates are repsected if selection mode is set to calculate @see ETextCursorSelectionMode
	int m_PressMouseX;
	int m_PressMouseY;
	// these coordinates are repsected if selection/cursor mode is set to calculate @see ETextCursorSelectionMode / @see ETextCursorCursorMode
	int m_ReleaseMouseX;
	int m_ReleaseMouseY;

	// note m_SelectionStart can be bigger than m_SelectionEnd, depending on how the mouse cursor was dragged
	// also note, that these are the character offsets decoded
	int m_SelectionStart;
	int m_SelectionEnd;

	ETextCursorCursorMode m_CursorMode;
	// note this is the decoded character offset
	int m_CursorCharacter;

	float Height() const
	{
		return m_LineCount * m_AlignedFontSize;
	}
};

class ITextRender : public IInterface
{
	MACRO_INTERFACE("textrender", 0)
public:
	virtual void SetCursor(CTextCursor *pCursor, float x, float y, float FontSize, int Flags) const = 0;
	virtual void MoveCursor(CTextCursor *pCursor, float x, float y) const = 0;
	virtual void SetCursorPosition(CTextCursor *pCursor, float x, float y) const = 0;

	virtual CFont *LoadFont(const char *pFilename, unsigned char *pBuf, size_t Size) = 0;
	virtual bool LoadFallbackFont(CFont *pFont, const char *pFilename, unsigned char *pBuf, size_t Size) const = 0;
	virtual CFont *GetFont(size_t FontIndex) = 0;
	virtual CFont *GetFont(const char *pFilename) = 0;

	virtual void SetDefaultFont(CFont *pFont) = 0;
	virtual void SetCurFont(CFont *pFont) = 0;

	virtual void SetRenderFlags(unsigned Flags) = 0;
	virtual unsigned GetRenderFlags() const = 0;

	ColorRGBA DefaultTextColor() const { return ColorRGBA(1, 1, 1, 1); }
	ColorRGBA DefaultTextOutlineColor() const { return ColorRGBA(0, 0, 0, 0.3f); }
	ColorRGBA DefaultSelectionColor() const { return ColorRGBA(0, 0, 1.0f, 1.0f); }

	//
	virtual void TextEx(CTextCursor *pCursor, const char *pText, int Length) = 0;
	virtual bool CreateTextContainer(int &TextContainerIndex, CTextCursor *pCursor, const char *pText, int Length = -1) = 0;
	virtual void AppendTextContainer(int TextContainerIndex, CTextCursor *pCursor, const char *pText, int Length = -1) = 0;
	// either creates a new text container or appends to a existing one
	virtual bool CreateOrAppendTextContainer(int &TextContainerIndex, CTextCursor *pCursor, const char *pText, int Length = -1) = 0;
	// just deletes and creates text container
	virtual void RecreateTextContainer(int &TextContainerIndex, CTextCursor *pCursor, const char *pText, int Length = -1) = 0;
	virtual void RecreateTextContainerSoft(int &TextContainerIndex, CTextCursor *pCursor, const char *pText, int Length = -1) = 0;
	virtual void DeleteTextContainer(int &TextContainerIndex) = 0;

	virtual void UploadTextContainer(int TextContainerIndex) = 0;

	virtual void RenderTextContainer(int TextContainerIndex, const ColorRGBA &TextColor, const ColorRGBA &TextOutlineColor) = 0;
	virtual void RenderTextContainer(int TextContainerIndex, const ColorRGBA &TextColor, const ColorRGBA &TextOutlineColor, float X, float Y) = 0;

	virtual void UploadEntityLayerText(void *pTexBuff, size_t ImageColorChannelCount, int TexWidth, int TexHeight, int TexSubWidth, int TexSubHeight, const char *pText, int Length, float x, float y, int FontHeight) = 0;
	virtual int AdjustFontSize(const char *pText, int TextLength, int MaxSize, int MaxWidth) const = 0;
	virtual float GetGlyphOffsetX(int FontSize, char TextCharacter) const = 0;
	virtual int CalculateTextWidth(const char *pText, int TextLength, int FontWidth, int FontHeight) const = 0;

	virtual bool SelectionToUTF8OffSets(const char *pText, int SelStart, int SelEnd, int &OffUTF8Start, int &OffUTF8End) const = 0;
	virtual bool UTF8OffToDecodedOff(const char *pText, int UTF8Off, int &DecodedOff) const = 0;
	virtual bool DecodedOffToUTF8Off(const char *pText, int DecodedOff, int &UTF8Off) const = 0;

	// old foolish interface
	virtual void TextColor(float r, float g, float b, float a) = 0;
	virtual void TextColor(ColorRGBA rgb) = 0;
	virtual void TextOutlineColor(float r, float g, float b, float a) = 0;
	virtual void TextOutlineColor(ColorRGBA rgb) = 0;
	virtual void TextSelectionColor(float r, float g, float b, float a) = 0;
	virtual void TextSelectionColor(ColorRGBA rgb) = 0;
	virtual void Text(float x, float y, float Size, const char *pText, float LineWidth) = 0;
	virtual float TextWidth(float Size, const char *pText, int StrLength, float LineWidth, int Flags = 0, float *pHeight = nullptr, float *pAlignedFontSize = nullptr, float *pMaxCharacterHeightInLine = nullptr) = 0;

	virtual ColorRGBA GetTextColor() const = 0;
	virtual ColorRGBA GetTextOutlineColor() const = 0;
	virtual ColorRGBA GetTextSelectionColor() const = 0;

	virtual void OnWindowResize() = 0;
};

class IEngineTextRender : public ITextRender
{
	MACRO_INTERFACE("enginetextrender", 0)
public:
	virtual void Init() = 0;
};

extern IEngineTextRender *CreateEngineTextRender();

#endif
