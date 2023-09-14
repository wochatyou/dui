#ifndef __WOCHAT_WINDOWS3_H__
#define __WOCHAT_WINDOWS3_H__

#include "xwindui.h"

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

public:
	XWindow3()
	{
		m_backgroundColor = 0xFFF5F5F5;
		m_buttonStartIdx = XWIN3_BUTTON_DOT;
		m_buttonEndIdx = XWIN3_BUTTON_DOT;
		m_property |= XWIN_PROP_MOVEWIN;
		m_message = WM_WIN3_MESSAGE;
	}

	~XWindow3()
	{
	}

	BLFont   m_font;
	uint16_t* m_title = (uint16_t*)titlewin3;

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

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int ret = 0;
		BLResult blResult;

		InitButtons();

		blResult = m_font.createFromFace(g_fontFace, 17.0f);
		if (BL_SUCCESS != blResult)
			return (-1);

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

		if (nullptr != m_title)
		{
			BLRgba32 color(0xFF535353u);
			BLRgba32 bkcolor(m_backgroundColor);
			BLRgba32 selcolor(0xFFFACE87u);
			BLImageData imgdata = { 0 };
			BLGlyphBuffer gb;
			BLImage img;
			BLResult blResult = img.create(WOCHAT_ALIGN_DEFAULT32(w - 100), h, BL_FORMAT_PRGB32);
			if (BL_SUCCESS == blResult)
			{
				BLContext ctx(img);
				ctx.fillAll(bkcolor);
				//ctx.fillBox(0, 24, 90, 45, selcolor);
				gb.setUtf16Text((const uint16_t*)m_title);
				m_font.shape(gb);
				ctx.fillGlyphRun(BLPoint(4, 40), m_font, gb.glyphRun(), color);
				ctx.end();

				blResult = img.getData(&imgdata);
				if (BL_SUCCESS == blResult)
				{
					ScreenDrawRect(m_screen, w, h, (U32*)imgdata.pixelData, imgdata.size.w, imgdata.size.h, 0, 0);
				}
			}
		}
		return 0;
	}

	int UpdateTitle(uint16_t* title)
	{
		int ret = XWIN_STATUS_NODRAW;
		if (nullptr != title)
		{
			m_title = title;
			m_status |= XWIN_STATUS_NEEDRAW;
			ret = XWIN_STATUS_NEEDRAW;
		}
		return ret;
	}
};

#endif  /* __WOCHAT_WINDOWS3_H__ */

