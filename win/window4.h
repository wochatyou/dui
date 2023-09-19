#ifndef __WOCHAT_WINDOWS4_H__
#define __WOCHAT_WINDOWS4_H__

#include "dui/dui_win.h"

#include "testdata.h"

#define MAX_INPUT_MESSAGE_LEN	(1<<16)  // we only allow maximum 64KB -1 characters input

U16 name[] = { 0x7b11,0x50b2,0x6c5f,0x6e56, 0 };

U32 littleArrowMe[4 * 8] = { 0 };

typedef struct XChatMessage
{
	XChatMessage* next_;
	U32* icon_;        // the bitmap data of this icon
	U8   w_;           // the width in pixel of this icon
	U8   h_;           // the height in pixel of this icon
	int  height_;	   // in pixel
	int  width_;	   // in pixel
	U16  state_;
	U16  msgLen_;
	U64  ts_;  // the time stamp. 
	U16* name_;       // The name of this people      
	U16* message_;    // real message
	U16* wrapTab_;
	U8* obj_;         // point to GIF/APNG/Video/Pic etc
} XChatMessage;

// _TextWrapIdxTab[0] is the length of the elements behind it.
static U16 _TextWrapIdxTab[MAX_INPUT_MESSAGE_LEN];

class XWindow4 : public XWindowT <XWindow4>
{
private:
	enum {
		GAP_TOP3 = 40,
		GAP_BOTTOM3 = 10,
		GAP_LEFT3 = 0,
		GAP_RIGHT3 = 0,
		GAP_MESSAGE = 20
	};

public:
	XWindow4()
	{
		U32* p;
		U32* q;
		m_backgroundColor = 0xFFF5F5F5;
		m_message = WM_WIN4_MESSAGE;
		m_property |= (DUI_PROP_HASVSCROLL | DUI_PROP_HANDLEVWHEEL);

		p = (U32*)xbmpXMeArrow;
		q = (U32*)littleArrowMe;
		for (int i = 0; i < (4 * 8); i++)
		{
			*q++ = (0xFFFFFFFF == *p) ? m_backgroundColor : *p;
			p++;
		}
	}

	~XWindow4()
	{
	}

	BLFont	m_font0;
	BLFont	m_font1;
	int		m_lineHeight0 = 0;
	int		m_lineHeight1 = 0;

	XChatMessage* m_rootMessage = nullptr;

	int UpdateChatHistory(uint16_t* msgText, U8 msgtype = 0)
	{
		return 0;
	}

	int LoadChatHistory()
	{
		U16 len;
		U16* pTxt;
		XChatMessage* p = nullptr;
		XChatMessage* q = nullptr;

		InitalizeTestText();

		size_t size = sizeof(txtdata)/sizeof(U16*);
		
		for (size_t i = 0; i < size; i++)
		{
			pTxt = txtdata[i];
			assert(pTxt);
			len = 0;
			while (0 != *pTxt++)
				len++;
			
			assert(m_pool);
			p = (XChatMessage*)palloc0(m_pool, sizeof(XChatMessage));
			if (nullptr == p)
				break;

			if (nullptr == m_rootMessage)
				m_rootMessage = p;
			if (nullptr != q)
				q->next_ = p;

			q = p;
			p->icon_ = (0 == i % 2) ? (U32*)xbmpHeadGirl : (U32*)xbmpHeadMe;
			p->w_ = p->h_ = 34;
			p->state_ = i;
			p->name_ = (U16*)name;
			p->msgLen_ = len;
			p->message_ = txtdata[i];
			p->next_ = nullptr;
		}

		return 0;
	}

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int ret = 0;

		BLResult blResult = m_font0.createFromFace(g_fontFace, 16.0f);
		if (BL_SUCCESS != blResult)
			return (-1);

		BLFontMetrics fm = m_font0.metrics();
		m_lineHeight0 = (int)(fm.ascent + fm.descent + fm.lineGap);
		assert(m_lineHeight0 > 0);
		m_sizeLine.cy = m_lineHeight0;

		m_pool = mempool_create(0, 0, 0);
		if (nullptr == m_pool)
			return (-2);

		LoadChatHistory();

		return 0;
	}

	void UpdatePosition() 
	{
		int W, H;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		U16 i, lines;
		XChatMessage* p = m_rootMessage;
		
		assert(w > 300);
		W = DUI_ALIGN_DEFAULT32(w - 220);

		m_sizeAll.cy = GAP_MESSAGE;

		while (nullptr != p)
		{
			p->width_ = TextLayout(W, p->message_, p->msgLen_);
				
			lines = _TextWrapIdxTab[0];
			if (lines > 0)
			{
				pfree(p->wrapTab_);
				p->wrapTab_ = (U16*)palloc(m_pool, sizeof(U16) * (lines + 1));
				if (nullptr != p->wrapTab_)
				{
					for (i = 0; i <= lines; i++)
						p->wrapTab_[i] = _TextWrapIdxTab[i];

					H = m_lineHeight0 * (lines + 1);
					p->height_ = H + GAP_MESSAGE;
					m_sizeAll.cy += p->height_;
				}
			}
			p = p->next_;
		}

		m_ptOffset.y = (m_sizeAll.cy > h) ? m_sizeAll.cy - h : 0;
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		UpdatePosition();
		return 0;
	}

	int TextLayout(int width, U16* txt, U16 characters)
	{
		double wm, WM;
		U16* wrapIdx = _TextWrapIdxTab;
		BLTextMetrics tm;
		BLGlyphBuffer gb;

		assert(characters < MAX_INPUT_MESSAGE_LEN);

		gb.setUtf16Text(txt, characters);
		m_font0.shape(gb);
		m_font0.getTextMetrics(gb, tm);

		WM = (double)width;
		wm = tm.boundingBox.x1 - tm.boundingBox.x0;
		if (wm < WM)
		{
			wrapIdx[0] = 1;
			wrapIdx[1] = characters;
			return (int)wm;
		}
		else // the text needs to be wrapped.
		{
			U16 charBaseLen,charLen, halfSize, i;
			U16* p = txt;
			U16 lines = 0;
			U16 charRemaining = characters;

			wrapIdx = _TextWrapIdxTab + 1;

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
					wm = tm.boundingBox.x1 - tm.boundingBox.x0;
					if (wm < WM)
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
						wm = tm.boundingBox.x1 - tm.boundingBox.x0;
						if (wm >= WM)
							break;
					}
					charLen += (i - 1); // we find the maxium characters that fit one line
				}
				lines++;
				*wrapIdx++ = charLen;
				p += charLen;
				if (charRemaining > charLen)
					charRemaining -= charLen;
			} 
			while (charRemaining > charLen);
			lines++;
			*wrapIdx++ = charRemaining;
			assert(lines < MAX_INPUT_MESSAGE_LEN);
			_TextWrapIdxTab[0] = lines;
		}
		return width;
	}

	int Draw()
	{
		int W, H, x, y, dx, dy, pos, width;
		U16 i, lines, charNum;
		U16* m;
		U16* msg;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		BLResult blResult;
		BLRgba32 bkcolor0(0xFF6AEA9Eu);
		BLRgba32 bkcolor1(0xFFFFFFFFu);
		BLRgba32 textColor0(0xFF333333u);
		BLImageData imgdata = { 0 };
		BLGlyphBuffer gb;

		XChatMessage* p = m_rootMessage;

		assert(w > 200);
		W = DUI_ALIGN_DEFAULT32(w - 190);

		pos = GAP_MESSAGE;
		while (nullptr != p)
		{
			H = p->height_;
			
			assert(H > GAP_MESSAGE);

			if (pos + H > m_ptOffset.y && pos < m_ptOffset.y + h)
			{
				BLImage img;
				width = W;
				if(p->width_ < W - 16)
					width = DUI_ALIGN_DEFAULT32(p->width_ + 16);

				blResult = img.create(width, H - GAP_MESSAGE, BL_FORMAT_PRGB32);

				if (BL_SUCCESS == blResult)
				{
					BLContext ctx(img);
					if (p->state_ % 2) // me
					{
						ctx.fillAll(bkcolor0);
						dx = w - width - 60;
						x = w - m_scrollWidth - p->w_ - 6;
					}
					else // girl
					{
						ctx.fillAll(bkcolor1);
						dx = 60;
						x = 10;
					}

					m = p->wrapTab_;
					assert(nullptr != m);
					lines = *m++;
					msg = p->message_;
					y = m_lineHeight0;
					for (i = 0; i < lines; i++)
					{
						charNum = *m++;
						gb.setUtf16Text(msg, charNum);
						m_font0.shape(gb);
						ctx.fillGlyphRun(BLPoint(10, y + 5), m_font0, gb.glyphRun(), textColor0);
						y += m_lineHeight0;
						msg += charNum;
					}

					blResult = img.getData(&imgdata);
					if (BL_SUCCESS == blResult)
					{
						assert(nullptr != p->icon_);
						ScreenDrawRectRound(m_screen, w, h, (U32*)p->icon_, p->w_, p->h_, x, pos - m_ptOffset.y, m_backgroundColor);
						ScreenDrawRectRound(m_screen, w, h, (U32*)imgdata.pixelData, imgdata.size.w, imgdata.size.h, dx, pos - m_ptOffset.y, m_backgroundColor);
						if (p->state_ % 2)
							ScreenDrawRect(m_screen, w, h, (U32*)littleArrowMe, 4, 8, dx + imgdata.size.w, pos - m_ptOffset.y + 13);
						
					}
				}
			}
			pos += H;
			p = p->next_;
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

