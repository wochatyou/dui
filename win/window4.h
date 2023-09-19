#ifndef __WOCHAT_WINDOWS4_H__
#define __WOCHAT_WINDOWS4_H__

#include "dui/dui_win.h"

#include "testdata.h"

#define MAX_INPUT_MESSAGE_LEN	(1<<16)  // we only allow maximum 64KB -1 characters input

U16 name[] = { 0x7b11,0x50b2,0x6c5f,0x6e56, 0 };

U32 littleArrowMe[4 * 8] = { 0 };

#define IsAlphabet(c)		(((c) >= 0x41 && (c) <= 0x5A) || ((c) >= 0x61 && (c) <= 0x7A))

typedef struct XChatMessage
{
	XChatMessage* next_;
	U32* icon_;     // the bitmap data of this icon
	U8   w_;        // the width in pixel of this icon
	U8   h_;        // the height in pixel of this icon
	int  height_;   // in pixel
	int  width_;    // in pixel
	U16  state_;
	U16  msgLen_;
	U64  ts_;		// the time stamp. 
	U16* name_;     // The name of this people      
	U16* message_;  // real message
	U16* textWrapTab_; // the struct of this table is the same as _TextWrapIdxTab
	U8*  obj_;       // point to GIF/APNG/Video/Pic etc
} XChatMessage;

// _TextWrapIdxTab[0] is the length of the elements behind it.
// The first two bytes are the lines parsered. Each element has two parts: 2-byte string base index, and 2-byte string length
static U16 _TextWrapIdxTab[MAX_INPUT_MESSAGE_LEN];

class XWindow4 : public XWindowT <XWindow4>
{
private:
	enum {
		GAP_TOP4 = 40,
		GAP_BOTTOM4 = 10,
		GAP_LEFT4 = 0,
		GAP_RIGHT4 = 0,
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
			while (0 != *pTxt++ && len < MAX_INPUT_MESSAGE_LEN)  // we only allow MAX_INPUT_MESSAGE_LEN - 1 characters in one message
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
		U16 i, lines, u16Num;
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
				u16Num = 1 + (lines << 1);
				pfree(p->textWrapTab_);
				p->textWrapTab_ = (U16*)palloc(m_pool, sizeof(U16) * u16Num);
				if (nullptr != p->textWrapTab_)
				{
					for (i = 0; i <= u16Num; i++)
						p->textWrapTab_[i] = _TextWrapIdxTab[i];

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

#define DUI_DEBUG	1

	int TextLayout(int width, U16* txt, U16 characters)
	{
		int w, MaxWidth = 0;
		U16  charMaxLen, charLen, charBaseIdx, i, idx;

		assert(characters < MAX_INPUT_MESSAGE_LEN);
		assert(characters > 0);

		_TextWrapIdxTab[0] = 0; // clear the wrap table

		charMaxLen = characters;
		charBaseIdx = 0;
		for(i=0; i<characters; i++)  // we scan the whole string, maybe we find "\n", maybe not
		{
			if(0 == txt[i])  // it is a zero-terminated string
			{
				charMaxLen = i;
				break;
			}

			if(0x0A == txt[i])
			{
				charLen = i - charBaseIdx;
				if(charLen > 0)
				{
					w = TextLayoutOneLine(width, txt, charBaseIdx, charLen);
					if(MaxWidth < w)
						MaxWidth = w;
				}
				else  // "\n\n\n\n"
				{
					idx = _TextWrapIdxTab[0] << 1;
					_TextWrapIdxTab[idx + 1] = charBaseIdx;
					_TextWrapIdxTab[idx + 2] = 0;
					_TextWrapIdxTab[0]++;
				}
				charBaseIdx = i + 1; // pointing to the next character of "\n"
			}
		}

		if(charMaxLen > 0)
		{
			if(charBaseIdx < charMaxLen-1) // we still have characters after the last "\n"
			{
				w = TextLayoutOneLine(width, txt, charBaseIdx, (charMaxLen - charBaseIdx));
				if(MaxWidth < w)
					MaxWidth = w;
			}
		}

		return MaxWidth;
	}


	// We assume that txt is a single line, which means there is no "\n" in it.
	int TextLayoutOneLine(int width, U16* txt, U16 base, U16 len)
	{
		int MaxWidth = width;
		BLTextMetrics tm;
		BLGlyphBuffer gb;
		double wm;
		double WM = (double)width;
		U16 charBaseIdx, charOldLen, charLen, charRemaining;
		U16 charBaseLen, halfSize, lines, i;
		U16* p;
		U16* wrapIdx;

#ifdef DUI_DEBUG
		U16 charSum = 0;
#endif			
		lines = _TextWrapIdxTab[0];
		charRemaining = len;
		wrapIdx = _TextWrapIdxTab + 1 + (lines<<1) ; // _TextWrapIdxTab[0] is used to save the length
		charBaseIdx = base;
		p = txt + charBaseIdx;

		while(true)
		{
			charBaseLen = 0;
			halfSize = charRemaining;
			while (0 != halfSize)
			{
				charLen = charBaseLen + halfSize;
				assert(charLen <= charRemaining);

				gb.setUtf16Text(p, charLen);
				m_font0.shape(gb);
				m_font0.getTextMetrics(gb, tm);
				wm = tm.boundingBox.x1 - tm.boundingBox.x0;
				if (wm < WM)
				{
					charBaseLen = charLen;
					if (charLen == charRemaining)
						break;
				}
				halfSize >>= 1;
			}

			assert(charLen > 0);
			assert(charLen <= charRemaining);			

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
				charLen += i; // we find the maxium characters that fit one line
				charLen--;

				if(IsAlphabet(p[charLen-1]) && IsAlphabet(p[charLen])) // we cannot split the whole word
				{
					charOldLen = charLen;
					while(charLen > 0)
					{
						charLen--;
						if(!IsAlphabet(p[charLen-1]))
							break;
					}
					if(0 == charLen)  // all characters of this line is a-z or A-Z, so we have to split the long word!
						charLen = charOldLen;
				}
			}

			*wrapIdx++ = charBaseIdx;  
			*wrapIdx++ = charLen;  // push the maximum characters per line into the wrap index table
			charBaseIdx += charLen;
			lines++;

#ifdef DUI_DEBUG
			charSum += charLen;
#endif		
			assert(charLen <= charRemaining);

			charRemaining -= charLen;

			if(0 == charRemaining) // all charachters have been processed
				break;
			
			p += charLen;
		}

		assert(lines < (MAX_INPUT_MESSAGE_LEN >> 1));
		_TextWrapIdxTab[0] = lines;

#ifdef DUI_DEBUG
			assert(charSum == len);
#endif
		if(1 == lines) // we only have one line, we can shrink the width
			MaxWidth = (int)wm;
		return MaxWidth;
	}

	int Draw()
	{
		int W, H, x, y, dx, dy, pos, width;
		U16 i, lines, baseIdx, charNum;
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

					m = p->textWrapTab_;
					assert(nullptr != m);
					lines = *m++;
					msg = p->message_;
					y = m_lineHeight0;
					for (i = 0; i < lines; i++)
					{
						baseIdx = *m++;
						charNum = *m++;
						if(charNum > 0)
						{
							gb.setUtf16Text(msg + baseIdx, charNum);
							m_font0.shape(gb);
							ctx.fillGlyphRun(BLPoint(10, y + 5), m_font0, gb.glyphRun(), textColor0);
						}
						y += m_lineHeight0;
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

