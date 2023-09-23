#ifndef __WOCHAT_WINDOWS1_H__
#define __WOCHAT_WINDOWS1_H__

#include "dui/dui_win.h"
#include "dui/imstb_textedit.h"

U16 txt[] = { 0x0044,0x0042,0x0041,0x57f9,0x8bad,0x7fa4,0x0028,0x0032,0x0035,0x0037,0x0029,0x0000 };

enum
{
	XWIN1_BUTTON_SEARCH = 0
};

class XWindow1 : public XWindowT <XWindow1>
{
private:
	enum
	{
		 XWIN1_BITMAP_SEARCHN = 0	// Normal
		,XWIN1_BITMAP_SEARCHH		// Hover
		,XWIN1_BITMAP_SEARCHP		// Press
		,XWIN1_BITMAP_SEARCHA		// Active
	};

public:
	XWindow1()
	{
		m_backgroundColor = 0xFFEAECED;
		m_buttonEndIdx = XWIN1_BUTTON_SEARCH;
		m_property |= DUI_PROP_MOVEWIN;
		m_message = WM_XWINDOWS01;
		InitButtons();
	}

	~XWindow1()	{}

	U16*	m_text = (U16*)txt;

	void InitBitmap()
	{
		U8 id;
		XBitmap* bmp;

		int w = 27;
		int h = 27;
		id = XWIN1_BITMAP_SEARCHN; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSerachN; bmp->w = w; bmp->h = h;
		id = XWIN1_BITMAP_SEARCHH; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSerachH; bmp->w = w; bmp->h = h;
		id = XWIN1_BITMAP_SEARCHP; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSerachP; bmp->w = w; bmp->h = h;
		id = XWIN1_BITMAP_SEARCHA; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSerachH; bmp->w = w; bmp->h = h;
	}

	int InitButtons()
	{
		int offset = 10, gap = 20;
		U16 w, h;
		U8 id;
		U32 size, bytes;
		XButton* button;
		XBitmap* bitmap;
		
		InitBitmap(); // inital all bitmap resource

		id = XWIN1_BUTTON_SEARCH; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN1_BITMAP_SEARCHN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN1_BITMAP_SEARCHH]; button->imgHover  = bitmap;
		bitmap = &m_bitmap[XWIN1_BITMAP_SEARCHP]; button->imgPress  = bitmap;
		bitmap = &m_bitmap[XWIN1_BITMAP_SEARCHA]; button->imgActive = bitmap;
		return 0;
	}

public:
	void UpdateButtonPosition()
	{
		int gap = 10; // pixel
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		XButton* button = &m_button[XWIN1_BUTTON_SEARCH];
		XBitmap* bmp = button->imgNormal;
		if (w > bmp->w + gap && h > bmp->h)
		{
			button->top = (h - bmp->h) >> 1;
			button->bottom = button->top + bmp->h;
			button->right = w - gap;
			button->left = button->right - bmp->w;
		}
	}

	int Draw()
	{
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		return 0;
	}
};

#endif  /* __WOCHAT_WINDOWS1_H__ */

