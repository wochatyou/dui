#ifndef __WOCHAT_WINDOWS4_H__
#define __WOCHAT_WINDOWS4_H__

#include "dui/dui_win.h"
#include <vector>

U16 testxt[] = {
	0x6c38,0x548c,0x4e5d,0x5e74,0xff0c,0x5c81,0x5728,0x7678,0x4e11,0xff0c,0x66ae,0x6625,0x4e4b,0x521d,0xff0c,0x4f1a,
	0x4e8e,0x4f1a,0x7a3d,0x5c71,0x9634,0x4e4b,0x5170,0x4ead,0xff0c,0x4fee,0x798a,0x4e8b,0x4e5f,0x3002,0x7fa4,0x8d24,
	0x6bd5,0x81f3,0xff0c,0x5c11,0x957f,0x54b8,0x96c6,0x3002,0x6b64,0x5730,0x6709,0x5d07,0x5c71,0x5cfb,0x5cad,0xff0c,
	0x8302,0x6797,0x4fee,0x7af9,0xff1b,0x53c8,0x6709,0x6e05,0x6d41,0x6fc0,0x6e4d,0xff0c,0x6620,0x5e26,0x5de6,0x53f3,
	0xff0c,0x5f15,0x4ee5,0x4e3a,0x6d41,0x89de,0x66f2,0x6c34,0xff0c,0x5217,0x5750,0x5176,0x6b21,0x3002,0x867d,0x65e0,
	0x4e1d,0x7af9,0x7ba1,0x5f26,0x4e4b,0x76db,0xff0c,0x4e00,0x89de,0x4e00,0x548f,0xff0c,0x4ea6,0x8db3,0x4ee5,0x7545,
	0x53d9,0x5e7d,0x60c5,0x3002,0x662f,0x65e5,0x4e5f,0xff0c,0x5929,0x6717,0x6c14,0x6e05,0xff0c,0x60e0,0x98ce,0x548c,
	0x7545,0x3002,0x4ef0,0x89c2,0x5b87,0x5b99,0x4e4b,0x5927,0xff0c,0x4fef,0x5bdf,0x54c1,0x7c7b,0x4e4b,0x76db,0xff0c,
	0x6240,0x4ee5,0x6e38,0x76ee,0x9a8b,0x6000,0xff0c,0x8db3,0x4ee5,0x6781,0x89c6,0x542c,0x4e4b,0x5a31,0xff0c,0x4fe1,
	0x53ef,0x4e50,0x4e5f,0x3002,0x592b,0x4eba,0x4e4b,0x76f8,0x4e0e,0xff0c,0x4fef,0x4ef0,0x4e00,0x4e16,0xff0c,0x6216,
	0x53d6,0x8bf8,0x6000,0x62b1,0xff0c,0x609f,0x8a00,0x4e00,0x5ba4,0x4e4b,0x5185,0xff1b,0x6216,0x56e0,0x5bc4,0x6240,
	0x6258,0xff0c,0x653e,0x6d6a,0x5f62,0x9ab8,0x4e4b,0x5916,0x3002,0x867d,0x8da3,0x820d,0x4e07,0x6b8a,0xff0c,0x9759,
	0x8e81,0x4e0d,0x540c,0xff0c,0x5f53,0x5176,0x6b23,0x4e8e,0x6240,0x9047,0xff0c,0x6682,0x5f97,0x4e8e,0x5df1,0xff0c,
	0x5feb,0x7136,0x81ea,0x8db3,0xff0c,0x4e0d,0x77e5,0x8001,0x4e4b,0x5c06,0x81f3,0x3002,0x53ca,0x5176,0x6240,0x4e4b,
	0x65e2,0x5026,0xff0c,0x60c5,0x968f,0x4e8b,0x8fc1,0xff0c,0x611f,0x6168,0x7cfb,0x4e4b,0x77e3,0x3002,0x5411,0x4e4b,
	0x6240,0x6b23,0xff0c,0x4fef,0x4ef0,0x4e4b,0x95f4,0xff0c,0x5df2,0x4e3a,0x9648,0x8ff9,0xff0c,0x72b9,0x4e0d,0x80fd,
	0x4e0d,0x4ee5,0x4e4b,0x5174,0x6000,0x3002,0x51b5,0x4fee,0x77ed,0x968f,0x5316,0xff0c,0x7ec8,0x671f,0x4e8e,0x5c3d,
	0x3002,0x53e4,0x4eba,0x4e91,0xff1a,0x201c,0x6b7b,0x751f,0x4ea6,0x5927,0x77e3,0x3002,0x201d,0x5c82,0x4e0d,0x75db,
	0x54c9,0xff01,0x6bcf,0x89c8,0x6614,0x4eba,0x5174,0x611f,0x4e4b,0x7531,0xff0c,0x82e5,0x5408,0x4e00,0x5951,0xff0c,
	0x672a,0x5c1d,0x4e0d,0x4e34,0x6587,0x55df,0x60bc,0xff0c,0x4e0d,0x80fd,0x55bb,0x4e4b,0x4e8e,0x6000,0x3002,0x56fa,
	0x77e5,0x4e00,0x6b7b,0x751f,0x4e3a,0x865a,0x8bde,0xff0c,0x9f50,0x5f6d,0x6b87,0x4e3a,0x5984,0x4f5c,0x3002,0x540e,
	0x4e4b,0x89c6,0x4eca,0xff0c,0x4ea6,0x72b9,0x4eca,0x4e4b,0x89c6,0x6614,0x3002,0x60b2,0x592b,0xff01,0x6545,0x5217,
	0x53d9,0x65f6,0x4eba,0xff0c,0x5f55,0x5176,0x6240,0x8ff0,0xff0c,0x867d,0x4e16,0x6b8a,0x4e8b,0x5f02,0xff0c,0x6240,
	0x4ee5,0x5174,0x6000,0xff0c,0x5176,0x81f4,0x4e00,0x4e5f,0x3002,0x540e,0x4e4b,0x89c8,0x8005,0xff0c,0x4ea6,0x5c06,
	0x6709,0x611f,0x4e8e,0x65af,0x6587,0x3002,0x0000
};

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
		m_property |= (DUI_PROP_HASVSCROLL | DUI_PROP_HANDLEVWHEEL);
	}

	~XWindow4()
	{
	}

	BLFont	m_font0;
	BLFont	m_font1;
	int		m_lineHeight0 = 0;
	int		m_lineHeight1 = 0;

	std::vector<U16> m_wrapIdx;

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

		BLResult blResult = m_font0.createFromFace(g_fontFace, 16.0f);
		if (BL_SUCCESS != blResult)
			return (-1);

		BLFontMetrics fm = m_font0.metrics();
		m_lineHeight0 = (int)(fm.ascent + fm.descent + fm.lineGap);
		assert(m_lineHeight0 > 0);

		m_pool = mempool_create(0, 0, 0);

		if (nullptr == m_pool)
			return (-2);

		void* p = palloc(m_pool, 123);
		if (nullptr != p)
			pfree(p);

		return 0;
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int h = m_area.bottom - m_area.top;
		m_ptOffset.y = m_sizeAll.cy - h;

		return 0;
	}

	void TextLayout(int width, U16* txt, U16 characters)
	{
		int w;
		BLTextMetrics tm;
		BLGlyphBuffer gb;

		assert(characters > 0);
		m_wrapIdx.clear();

		gb.setUtf16Text(txt, characters);
		m_font0.shape(gb);
		m_font0.getTextMetrics(gb, tm);

		w = (int)(tm.boundingBox.x1 - tm.boundingBox.x0);
		if (w < width)
		{
			m_wrapIdx.push_back(characters);
		}
		else // the text needs to be wrapped.
		{
			U16 charBaseLen,charLen, halfSize, i;
			U16* p = txt;
			U16 charRemaining = characters;
			do
			{
				charBaseLen = 0;
				halfSize = charRemaining;
				while (0 != halfSize)
				{
					charLen = charBaseLen + halfSize;
					gb.setUtf16Text(p, charLen);
					m_font0.shape(gb);
					m_font0.getTextMetrics(gb, tm);
					w = (int)(tm.boundingBox.x1 - tm.boundingBox.x0);
					if (w < width)
					{
						charBaseLen = charLen;
						if (charLen >= charRemaining)
							break;
					}
					halfSize >>= 1;
				}

				if (charRemaining > charLen)
				{
					for (i = 0; i < charRemaining - charLen; i++)
					{
						gb.setUtf16Text(p, charLen + i);
						m_font0.shape(gb);
						m_font0.getTextMetrics(gb, tm);
						w = (int)(tm.boundingBox.x1 - tm.boundingBox.x0);
						if (w >= width)
							break;
					}
					charLen += (i - 1); // we find the maxium characters that fit one line
				}
				m_wrapIdx.push_back(charLen);
				p += charLen;
				if (charRemaining > charLen)
					charRemaining -= charLen;
			} 
			while (charRemaining > charLen);
			m_wrapIdx.push_back(charRemaining);
		}
	}

	int Draw()
	{
		int W, H, lines;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		BLRgba32 bkcolor0(0xFF6AEA9Eu);
		BLRgba32 bkcolor1(0xFFFFFFFFu);
		BLRgba32 textColor0(0xFF333333u);

		W = DUI_ALIGN_DEFAULT32(w - 200);

		U16 charNumTotal = 390;
		TextLayout(W - 10, testxt, charNumTotal);

		lines = (int)m_wrapIdx.size();
		assert(m_lineHeight0 > 0);
		H = (m_lineHeight0 + 1) * lines;

		ScreenDrawRectRound(m_screen, w, h, (U32*)xbmpHeadMe, 34, 34, w - 60, 20, m_backgroundColor);

		BLImage img;
		BLResult blResult = img.create(W, H, BL_FORMAT_PRGB32);

		if (BL_SUCCESS == blResult)
		{
			int x, y;
			U16 charNum;
			U16* p;

			BLRgba32 color(0xFF333333u);
			BLRgba32 selcolor(0xFFFACE87u);
			BLRgba32 textColor1(0xFF777777u);
			BLRgba32 textColor2(0xFF555555u);

			BLImageData imgdata = { 0 };
			BLGlyphBuffer gb;

			BLContext ctx(img);
			ctx.fillAll(bkcolor0);

			p = (U16*)testxt;
			y = m_lineHeight0;
			for (int i = 0; i < lines; i++)
			{
				charNum = m_wrapIdx[i];
				gb.setUtf16Text(p, charNum);
				m_font0.shape(gb);
				ctx.fillGlyphRun(BLPoint(5, y), m_font0, gb.glyphRun(), textColor0);
				y += m_lineHeight0;
				p += charNum;
			}

			blResult = img.getData(&imgdata);
			if (BL_SUCCESS == blResult)
			{
				ScreenDrawRect(m_screen, w, h, (U32*)imgdata.pixelData, imgdata.size.w, imgdata.size.h, 120, 20);
			}
		}

		ScreenDrawRectRound(m_screen, w, h, (U32*)xbmpHeadGirl, 34, 34, 10, 400, m_backgroundColor);
		BLImage img1;
		blResult = img1.create(60, m_lineHeight0<<1, BL_FORMAT_PRGB32);

		if (BL_SUCCESS == blResult)
		{
			U16 txt[] = { 0x725b, 0x903c, 0xff01 };
			BLContext ctx(img1);
			ctx.fillAll(bkcolor1);
			BLImageData imgdata = { 0 };
			BLGlyphBuffer gb;
			gb.setUtf16Text((const U16*)txt, 3);
			m_font0.shape(gb);
			ctx.fillGlyphRun(BLPoint(10, m_lineHeight0 + 4), m_font0, gb.glyphRun(), textColor0);

			blResult = img1.getData(&imgdata);
			if (BL_SUCCESS == blResult)
			{
				ScreenDrawRect(m_screen, w, h, (U32*)imgdata.pixelData, imgdata.size.w, imgdata.size.h, 50, 400);
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

