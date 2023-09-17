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
		m_buttonStartIdx = XWIN1_BUTTON_SEARCH;
		m_buttonEndIdx = XWIN1_BUTTON_SEARCH;
		m_property |= DUI_PROP_MOVEWIN;

		m_message = WM_WIN1_MESSAGE;
	}

	~XWindow1()	{}

	BLFont	m_font;
	U16*	m_text = (U16*)txt;

	static int ButtonAction(void* obj, U32 uMsg, U64 wParam, U64 lParam)
	{
		int ret = 0;
		XWindow1* xw = (XWindow1*)obj;
		if (nullptr != xw)
			ret = xw->NotifyParent(uMsg, wParam, lParam);
		return ret;
	}

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

	void UpdatePosition()
	{
		UpdateButtonPosition();
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		UpdateButtonPosition();
		return 0;
	}

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int ret = 0;
		BLResult blResult;

		InitButtons();

		blResult = m_font.createFromFace(g_fontFace, 15.0f);
		if (BL_SUCCESS != blResult)
			return (-1);

		return ret;
	}

	int Draw()
	{
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		if (nullptr != m_text)
		{
			BLImage img;
			BLResult blResult = img.create(DUI_ALIGN_DEFAULT32(w - 100), h, BL_FORMAT_PRGB32);
			if (BL_SUCCESS == blResult)
			{
				BLRgba32 color(0xFF535353u);
				BLRgba32 bkcolor(m_backgroundColor);
				BLRgba32 selcolor(0xFFFACE87u);
				BLImageData imgdata = { 0 };
				BLGlyphBuffer gb;
				BLContext ctx(img);

				ctx.fillAll(bkcolor);
				//ctx.fillBox(0, 14, 60, 35, selcolor);
				gb.setUtf16Text((const uint16_t*)m_text);
				m_font.shape(gb);
				ctx.fillGlyphRun(BLPoint(4, 30), m_font, gb.glyphRun(), color);
				ctx.end();

				blResult = img.getData(&imgdata);
				if (BL_SUCCESS == blResult)
				{
					ScreenDrawRect(m_screen, w, h, (U32*)imgdata.pixelData, imgdata.size.w, imgdata.size.h, 40, 0);
				}
			}
		}
		return 0;
	}

	int DoTimer(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) 
	{ 
		if (DUI_STATUS_ISFOCUS & m_status)
		{
			return 1;
		}
		return 0; 
	}
};

#endif  /* __WOCHAT_WINDOWS1_H__ */

