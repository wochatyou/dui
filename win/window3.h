#ifndef __WOCHAT_WINDOWS3_H__
#define __WOCHAT_WINDOWS3_H__

#include "dui/dui_win.h"
#include <ft2build.h>
#include FT_FREETYPE_H

U32 ftbuf[4096];

uint16_t titlewin3[] = { 0x0044,0x0042,0x0041,0x57f9,0x8bad,0x7fa4,0x0028,0x0032,0x0035,0x0037,0x0029,0x0000 };
enum 
{
	XWIN3_BUTTON_DOT = 0
};

class XWindow3 : public XWindowT <XWindow3>
{
private:
	enum 
	{
		 XWIN3_BITMAP_DOTN = 0			// Normal
		,XWIN3_BITMAP_DOTH				// Hover
		,XWIN3_BITMAP_DOTP				// Press
		,XWIN3_BITMAP_DOTA				// Active
	};

	enum 
	{
		GAP_TOP3 = 40,
		GAP_BOTTOM3 = 10,
		GAP_LEFT3 = 0,
		GAP_RIGHT3 = 10
	};

	FT_Library m_ftLibrary = nullptr;
	FT_Face m_ftFace = nullptr;

public:
	XWindow3()
	{
		m_backgroundColor = 0xFFF5F5F5;
		m_buttonStartIdx = XWIN3_BUTTON_DOT;
		m_buttonEndIdx = XWIN3_BUTTON_DOT;
		m_property |= DUI_PROP_MOVEWIN;
		m_message = WM_WIN3_MESSAGE;
	}

	~XWindow3()
	{
	}

	static int ButtonAction(void* obj, U32 uMsg, U64 wParam, U64 lParam)
	{
		int ret = 0;
		XWindow3* xw = (XWindow3*)obj;
		if (nullptr != xw)
			ret = xw->NotifyParent(uMsg, wParam, lParam);
		return ret;
	}

	void InitBitmap()
	{
		U8 id;
		XBitmap* bmp;

		int w = 19;
		int h = 7;
		id = XWIN3_BITMAP_DOTN; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpDotN; bmp->w = w; bmp->h = h;
		id = XWIN3_BITMAP_DOTH; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpDotH; bmp->w = w; bmp->h = h;
		id = XWIN3_BITMAP_DOTP; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpDotP; bmp->w = w; bmp->h = h;
		id = XWIN3_BITMAP_DOTA; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpDotH; bmp->w = w; bmp->h = h;
	}

	int InitButtons()
	{
		int offset = 10, gap = 20;
		U16 w, h;
		U8 id;
		U32 i, size, bytes;
		XButton* button;
		XBitmap* bitmap;

		InitBitmap(); // inital all bitmap resource

		id = XWIN3_BUTTON_DOT; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN3_BITMAP_DOTN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN3_BITMAP_DOTH]; button->imgHover  = bitmap;
		bitmap = &m_bitmap[XWIN3_BITMAP_DOTP]; button->imgPress  = bitmap;
		bitmap = &m_bitmap[XWIN3_BITMAP_DOTA]; button->imgActive = bitmap;

		return 0;
	}

public:
	void UpdateButtonPosition()
	{
		int gap = 10; // pixel
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		XButton* button = &m_button[XWIN3_BUTTON_DOT];
		XBitmap* bmp = button->imgNormal;

		assert(nullptr != bmp);

		button->right = w - GAP_RIGHT3;
		button->bottom = h - GAP_BOTTOM3;
		button->left = button->right - bmp->w;
		button->top = button->bottom - bmp->h;

		assert(w > GAP_RIGHT3 + bmp->w);
		assert(h > GAP_BOTTOM3 + bmp->h);

	}

	void UpdatePosition()
	{
		UpdateButtonPosition();
	}

	int DoDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		assert(m_ftLibrary);
		assert(m_ftFace);

		FT_Done_Face(m_ftFace);
		FT_Done_FreeType(m_ftLibrary);

		return 0;
	}

#define FONT_SIZE 36
	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		FT_Bitmap* ftBmp;
		int row, col, dx, dy, w = 64, h = 64;
		unsigned char g;
		U32 G;
		U32* p;
		U8* q;
		wchar_t character = 'i';

		InitButtons();

		for (int i = 0; i < 4096; i++)
			ftbuf[i] = 0xFF000000;

		FT_Init_FreeType(&m_ftLibrary);
		if (nullptr == m_ftLibrary)
			return (-1);
		
		FT_New_Face(m_ftLibrary, "c:\\windev\\OPlusSans3Light.ttf", 0, &m_ftFace);
		if (nullptr == m_ftFace)
		{
			FT_Done_FreeType(m_ftLibrary);
			return (-2);
		}

		//FT_Set_Pixel_Sizes(m_ftFace, 0, 32);
		FT_Set_Char_Size(m_ftFace, FONT_SIZE * 64, FONT_SIZE * 64, 0, 0);

		FT_Load_Char(m_ftFace, character, FT_LOAD_RENDER);
		ftBmp = &(m_ftFace->glyph->bitmap);

		dx = 8; dy = 8;
		p = (U32*)ftbuf;
		for (row = 0; row < ftBmp->rows; row++)
		{
			p = (U32*)ftbuf + (dy + row) * w + dx;
			for (col = 0; col < ftBmp->width; col++)
			{
				g = ftBmp->buffer[row * ftBmp->width + col];
				if (g)
				{
					G = g;
					G = ((G << 16) | (G << 8) | G) | 0xFF000000;
					*p = G;
				}
				p++;
			}
		}

		return 0;
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		UpdateButtonPosition();
		return 0;
	}

	int Draw()
	{
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		//ScreenDrawRect(m_screen, w, h, (U32*)ftbuf, 64, 64, 0, 0);

		return 0;
	}

	int UpdateTitle(uint16_t* title)
	{
		return 0;
	}
};

#endif  /* __WOCHAT_WINDOWS3_H__ */

