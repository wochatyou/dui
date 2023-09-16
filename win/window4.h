#ifndef __WOCHAT_WINDOWS4_H__
#define __WOCHAT_WINDOWS4_H__

#include "dui/dui_win.h"

class XWindow4 : public XWindowT <XWindow4>
{
private:
	enum {
		GAP_TOP3 = 40,
		GAP_BOTTOM3 = 10,
		GAP_LEFT3 = 0,
		GAP_RIGHT3 = 0,
	};

public:
	XWindow4()
	{
		m_backgroundColor = 0xFFF5F5F5;
		m_message = WM_WIN4_MESSAGE;
		m_property |= DUI_PROP_HASVSCROLL;
	}

	~XWindow4()
	{
	}

	BLFont	m_font;

	int UpdateChatHistory(uint16_t* msgText, U8 msgtype = 0)
	{
		return 0;
	}

	int LoadChatHistory()
	{
		return 0;
	}

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int ret = 0;

		m_sizeAll.cy = 3000;
		m_sizeLine.cy = 128;

		BLResult blResult = m_font.createFromFace(g_fontFace, 128.0f);
		if (BL_SUCCESS != blResult)
			return (-1);

		return 0;
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int h = m_area.bottom - m_area.top;
		m_ptOffset.y = m_sizeAll.cy - h;

		return 0;
	}

	U16 TextLayout(int width, uint16_t* txt, uint16_t* idxTab)
	{
		return 1;
	}

	int Draw()
	{
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		U16 txt[2] = { 0x57f9, 0 };

		BLImage img;
		BLResult blResult = img.create(DUI_ALIGN_DEFAULT32(w - 10), h, BL_FORMAT_PRGB32);

		if (BL_SUCCESS == blResult)
		{
			BLRgba32 color(0xFFA3A3A3u);
			BLRgba32 bkcolor(m_backgroundColor);
			BLRgba32 selcolor(0xFFFACE87u);
			BLImageData imgdata = { 0 };
			BLGlyphBuffer gb;
			BLContext ctx(img);

			ctx.fillAll(bkcolor);
			//ctx.fillBox(0, 14, 60, 35, selcolor);
			gb.setUtf16Text((const uint16_t*)txt, 1);
			m_font.shape(gb);
			ctx.fillGlyphRun(BLPoint(10, 100), m_font, gb.glyphRun(), color);
			ctx.end();

			blResult = img.getData(&imgdata);
			if (BL_SUCCESS == blResult)
			{
				ScreenDrawRect(m_screen, w, h, (U32*)imgdata.pixelData, imgdata.size.w, imgdata.size.h, 50, 200);
			}
		}


		return 0;
	}

	int DoMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		return 0;
	}

	int DoLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		return 0;
	}

	int DoLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		return 0;
	}
};

#endif  /* __WOCHAT_WINDOWS4_H__ */

