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

		BLResult blResult = m_font.createFromFace(g_fontFace, 16.0f);
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
		U16 txt[7] = { 0x57f9, 0x8bad, 0x57fa, 0x5730, 0x0061, 0x0062, 0x0063 };

		BLImage img;
		BLResult blResult = img.create(DUI_ALIGN_DEFAULT32(w - 4), h, BL_FORMAT_PRGB32);

		if (BL_SUCCESS == blResult)
		{
			int x, y;
			BLRgba32 color(0xFF333333u);
			BLRgba32 bkcolor(0xFFFFFFFFu);
			BLRgba32 selcolor(0xFFFACE87u);
			BLImageData imgdata = { 0 };
			BLGlyphBuffer gb;
			BLTextMetrics tm;
			BLPath path;
			BLContext ctx(img);
			
			ctx.fillAll(bkcolor);
			//ctx.fillBox(0, 14, 60, 35, selcolor);
			gb.setUtf16Text((const uint16_t*)txt, 7);
			m_font.shape(gb);
			m_font.getTextMetrics(gb, tm);
			BLFontMetrics fm = m_font.metrics();
			double line = fm.ascent + fm.descent + fm.lineGap;
			double Y = fm.ascent + fm.descent + fm.lineGap;
			double X = (tm.boundingBox.x1 - tm.boundingBox.x0);

			x = 10, y = (int)Y;
			ctx.fillGlyphRun(BLPoint(x, y), m_font, gb.glyphRun(), color);
#if 0
			//ctx.setStrokeWidth(10);
			path.moveTo(BLPoint(x, y));
			path.lineTo(BLPoint(x + 500, y));
			path.lineTo(BLPoint(x+500, y+1));
			path.lineTo(BLPoint(x, y+1));
			path.lineTo(BLPoint(x, y));
			ctx.fillPath(path, BLRgba32(0xFFFFFFFF));

			path.moveTo(BLPoint(x, y));
			path.lineTo(BLPoint(x, 1));
			path.lineTo(BLPoint(x+1, 1));
			path.lineTo(BLPoint(x+1, y));
			path.lineTo(BLPoint(x, y));
			ctx.fillPath(path, BLRgba32(0xFFFFFFFF));

			x = 10 + (int)X, y = (int)Y;
			path.moveTo(BLPoint(x, y));
			path.lineTo(BLPoint(x, 1));
			path.lineTo(BLPoint(x + 1, 1));
			path.lineTo(BLPoint(x + 1, y));
			path.lineTo(BLPoint(x, y));
			ctx.fillPath(path, BLRgba32(0xFFFFFFFF));
#endif
			x = 10, y = (int)Y + (int)(line);
			ctx.fillGlyphRun(BLPoint(x, y), m_font, gb.glyphRun(), color);
#if 0
			path.moveTo(BLPoint(x, y));
			path.lineTo(BLPoint(x + 500, y));
			path.lineTo(BLPoint(x + 500, y + 1));
			path.lineTo(BLPoint(x, y + 1));
			path.lineTo(BLPoint(x, y));
			ctx.fillPath(path, BLRgba32(0xFFFFFFFF));
#endif
			y += (int)(line);
			ctx.fillGlyphRun(BLPoint(x, y), m_font, gb.glyphRun(), color);
			ctx.end();

			blResult = img.getData(&imgdata);
			if (BL_SUCCESS == blResult)
			{
				ScreenDrawRect(m_screen, w, h, (U32*)imgdata.pixelData, imgdata.size.w, imgdata.size.h, 0, 0);
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

