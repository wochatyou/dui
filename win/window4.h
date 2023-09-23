#ifndef __WOCHAT_WINDOWS4_H__
#define __WOCHAT_WINDOWS4_H__

#include "dui/dui_win.h"

#include "testdata.h"

#define MAX_INPUT_MESSAGE_LEN	(1<<16)  // we only allow maximum 64KB -1 characters input

U16 xname[] = { 4, 0x7b11,0x50b2,0x6c5f,0x6e56 };

U32 littleArrowMe[4 * 8] = { 0 };

#define IsAlphabet(c)		(((c) >= 0x41 && (c) <= 0x5A) || ((c) >= 0x61 && (c) <= 0x7A))

typedef struct XChatMessage
{
	XChatMessage* next;
	XChatMessage* prev;
	U32  id;
	U32* icon;     // the bitmap data of this icon
	U8   w;        // the width in pixel of this icon
	U8   h;        // the height in pixel of this icon
	int  height;   // in pixel
	int  width;    // in pixel
	U16  state;
	U64  ts;		// the time stamp. 
	U16* name;      // The name of this people      
	U16* message;  // real message
	U16* textWrapTab; // the struct of this table is the same as _TextWrapIdxTab
	U8*  obj;       // point to GIF/APNG/Video/Pic etc
} XChatMessage;

// _TextWrapIdxTab[0] is the length of the elements behind it.
// The first two bytes are the lines parsered. Each element has two parts: 2-byte string base index, and 2-byte string length
// 
//        The line number
//          | 
//          V
//         [X][B][L][B][L][B][L][B][L][B][L][B][L][B][L][B][L][B][L][B][L][B][L].......
//             ^  ^                    ^  ^
//             |  |                    |  |
// base of line 1 |                   line 5  
//   character number of line 1
//
static U16 _TextWrapIdxTab[MAX_INPUT_MESSAGE_LEN + 1];

U32 TextLayoutThread(void* lpData);

class XWindow4 : public XWindowT <XWindow4>
{
private:
	enum {
		GAP_TOP4 = 40,
		GAP_BOTTOM4 = 10,
		GAP_LEFT4 = 10,
		GAP_RIGHT4 = 10,
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

	int		m_lineHeight0 = 0;
	int		m_lineHeight1 = 0;
	int     m_widthOld = 0;

	// cairo/harfbuzz issue to cache to speed up
	cairo_font_extents_t m_font_extents = { 0 };
	cairo_glyph_t* m_cairo_glyphs = nullptr;
	cairo_font_face_t* m_cairo_face = nullptr;
	hb_font_t* m_hb_font0 = nullptr;
	hb_font_t* m_hb_font1 = nullptr;
	hb_buffer_t* m_hb_buffer = nullptr;

	XChatMessage* m_headMessage = nullptr;
	XChatMessage* m_tailMessage = nullptr;

	int UpdateChatHistory(uint16_t* msgText, U8 msgtype = 0)
	{
		return 0;
	}

	int LoadChatHistory()
	{
		U16 i, idx, len, c;
		U16* pTxt;
		U16* m;
		XChatMessage* p = nullptr;
		XChatMessage* q = nullptr;

		InitalizeTestText();

		assert(m_pool);
		size_t size = sizeof(txtdata)/sizeof(U16*);
		for (i = 0; i < (U16)size; i++)
		{
			p = (XChatMessage*)palloc0(m_pool, sizeof(XChatMessage));
			if (nullptr == p)
				break;

			p->next = p->prev = nullptr;

			if (nullptr == m_headMessage)
				m_headMessage = p;

			m_tailMessage = p;

			if (nullptr != q)
			{
				q->next = p;
				p->prev = q;
			}

			pTxt = txtdata[i];
			assert(pTxt);
			len = 0;
			while (0 != *pTxt++ && len < MAX_INPUT_MESSAGE_LEN)  // we only allow MAX_INPUT_MESSAGE_LEN - 1 characters in one message
				len++;
			p->message = (U16*)palloc(m_pool, sizeof(U16)*(len+1));
			if (nullptr != p->message)
			{
				p->message[0] = len; // save the string length to the first 2 bytes
				m = p->message + 1;
				pTxt = txtdata[i];
				for (idx = 0; idx < len; idx++) // copy the whole string
					m[idx] = pTxt[idx];
			}
			q = p;
			p->icon = (0 == i % 2) ? (U32*)xbmpHeadGirl : (U32*)xbmpHeadMe;
			p->w = p->h = 34;
			p->state = i;
			p->id = i;
			p->name = (U16*)xname;
		}

		return 0;
	}

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int ret = 0;
		hb_bool_t hs = 0;

		assert(nullptr == m_cairo_glyphs);
		m_cairo_glyphs = cairo_glyph_allocate(1024);
		if (nullptr == m_cairo_glyphs)
			return(-1);

		assert(nullptr != g_ftFace0);
		assert(nullptr != g_ftFace1);
		assert(nullptr == m_cairo_face);
		m_cairo_face = cairo_ft_font_face_create_for_ft_face(g_ftFace0, 0);
		if (nullptr == m_cairo_face)
		{
			cairo_glyph_free(m_cairo_glyphs);
			m_cairo_glyphs = nullptr;
			return (-2);
		}

		assert(nullptr == m_hb_font0);
		m_hb_font0 = hb_ft_font_create(g_ftFace0, NULL);
		if (nullptr == m_hb_font0)
		{
			cairo_glyph_free(m_cairo_glyphs);
			m_cairo_glyphs = nullptr;
			cairo_font_face_destroy(m_cairo_face);
			m_cairo_face = nullptr;
			return (-3);
		}

		assert(nullptr == m_hb_font1);
		m_hb_font1 = hb_ft_font_create(g_ftFace1, NULL);
		if (nullptr == m_hb_font1)
		{
			cairo_glyph_free(m_cairo_glyphs);
			m_cairo_glyphs = nullptr;
			cairo_font_face_destroy(m_cairo_face);
			m_cairo_face = nullptr;
			hb_font_destroy(m_hb_font0);
			m_hb_font0 = nullptr;
			return (-3);
		}

		assert(nullptr == m_hb_buffer);
		m_hb_buffer = hb_buffer_create();
		hs = hb_buffer_allocation_successful(m_hb_buffer);
		if (0 == hs || nullptr == m_hb_buffer)
		{
			cairo_glyph_free(m_cairo_glyphs);
			m_cairo_glyphs = nullptr;
			cairo_font_face_destroy(m_cairo_face);
			m_cairo_face = nullptr;

			hb_font_destroy(m_hb_font0);
			m_hb_font0 = nullptr;
			hb_font_destroy(m_hb_font1);
			m_hb_font1 = nullptr;

			return (-4);
		}

		// detect the line height
		m_lineHeight0 = 0;
		cairo_surface_t* cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 64, 64);
		cairo_status_t cs = cairo_surface_status(cairo_surface);
		if (CAIRO_STATUS_SUCCESS == cs)
		{
			cairo_t* cr = cairo_create(cairo_surface);
			cs = cairo_status(cr);
			if (CAIRO_STATUS_SUCCESS == cs)
			{
				cairo_set_font_face(cr, m_cairo_face);
				cairo_set_font_size(cr, XFONT_SIZE0);
				cairo_font_extents(cr, &m_font_extents);
				
				m_lineHeight0 = (int)m_font_extents.height;
				cairo_destroy(cr);
			}
			cairo_surface_destroy(cairo_surface);
		}

		if (0 == m_lineHeight0)
			return(-1);

		m_sizeLine.cy = m_lineHeight0;


		m_pool = mempool_create(0, 0, 0);
		if (nullptr == m_pool)
			return (-2);

		LoadChatHistory();

		return 0;
	}

	int DoDestroy(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		assert(nullptr != m_cairo_glyphs);
		cairo_glyph_free(m_cairo_glyphs);
		m_cairo_glyphs = nullptr;

		assert(nullptr != m_hb_buffer);
		hb_buffer_destroy(m_hb_buffer);
		m_hb_buffer = nullptr;

		assert(nullptr != m_hb_font0);
		hb_font_destroy(m_hb_font0);
		m_hb_font0 = nullptr;

		assert(nullptr != m_hb_font1);
		hb_font_destroy(m_hb_font1);
		m_hb_font1 = nullptr;

		assert(nullptr != m_cairo_face);
		cairo_font_face_destroy(m_cairo_face);
		m_cairo_face = nullptr;

		return 0;
	}

	// If we have many chat message, the text layout job will use long time.
	// So we will do the text layout from the bottom to the top, thus we can 
	// paint the window very quickly.
	// We put the complete text layout job in a background thread which will
	// do the text layout from top to bottom (head to tail)
	void UpdatePosition() 
	{
		int ret = ReWrapFromTail();
		if(0 != ret)
			CreatePlatformThread(TextLayoutThread, this);
	}

	int ReWrapFromHead()
	{
		int W, H, width;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		U16 i, lines, u16Num;
		U16* m;
		U16* wrapTab;
		XChatMessage* p;

		hb_buffer_t* hb_buf;
		hb_bool_t hs = 0;

		assert(nullptr != g_ftFace0);
		hb_font_t* hb_font = hb_ft_font_create(g_ftFace0, NULL);
		if (nullptr == hb_font)
		{
			return 0;
		}
		hb_buf = hb_buffer_create();
		hs = hb_buffer_allocation_successful(hb_buf);
		if (nullptr == hb_font)
		{
			hb_font_destroy(hb_font);
			return 0;
		}

		wrapTab = _TextWrapIdxTab;
		assert(w > 300);
		W = DUI_ALIGN_DEFAULT32(w - 200);

		assert(nullptr != m_pool);
		m_sizeAll.cy = GAP_MESSAGE;
		p = m_headMessage;

		while (nullptr != p)
		{
			p->height = GAP_MESSAGE;
			if (nullptr != p->message)
			{
				u16Num = p->message[0];
				m = p->message + 1;
				width = TextLayout(W, m, u16Num, wrapTab, hb_font, hb_buf);
				p->width = GAP_LEFT4 + width + GAP_RIGHT4;

				lines = wrapTab[0];
				if (lines > 0)
				{
					u16Num = 1 + (lines << 1);
					pfree(p->textWrapTab);
					p->textWrapTab = (U16*)palloc(m_pool, sizeof(U16) * u16Num);
					if (nullptr != p->textWrapTab)
					{
						for (i = 0; i < u16Num; i++)
							p->textWrapTab[i] = wrapTab[i];

						H = m_lineHeight0 * (lines + 1);
						p->height += H;
					}
				}
			}
			m_sizeAll.cy += p->height;
			p = p->next;
		}
		m_ptOffset.y = (m_sizeAll.cy > h) ? m_sizeAll.cy - h : 0;

		hb_buffer_destroy(hb_buf);
		hb_font_destroy(hb_font);

		m_status |= DUI_STATUS_NEEDRAW;
		NotifyParent(WM_REDRAW_WINDOW, 0, 0);

		return 0;

	}

	int ReWrapFromTail()
	{
		int W, H, width, height;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		U16 i, lines, u16Num;
		U16* m;
		U16* wrapTab;
		XChatMessage* p;

		if (m_widthOld > 0)
		{
			// the width does not change, we do not need to re-calculate the text layout
			if (w == m_widthOld)
			{
				m_ptOffset.y = (m_sizeAll.cy > h) ? m_sizeAll.cy - h : 0;
				return 0;
			}
		}

		m_widthOld = w;
		wrapTab = _TextWrapIdxTab;
		assert(w > 300);
		W = DUI_ALIGN_DEFAULT32(w - 200);

		assert(nullptr != m_pool);

		height = 0;
		p = m_tailMessage;
		while (nullptr != p)
		{
			p->height = GAP_MESSAGE;
			if (nullptr != p->message)
			{
				u16Num = p->message[0];
				m = p->message + 1;
				width = TextLayout(W, m, u16Num, wrapTab, m_hb_font0, m_hb_buffer);
				p->width = GAP_LEFT4 + width + GAP_RIGHT4;

				lines = wrapTab[0];
				if (lines > 0)
				{
					u16Num = 1 + (lines << 1);
					pfree(p->textWrapTab);
					p->textWrapTab = (U16*)palloc(m_pool, sizeof(U16) * u16Num);
					if (nullptr != p->textWrapTab)
					{
						for (i = 0; i < u16Num; i++)
							p->textWrapTab[i] = wrapTab[i];

						H = m_lineHeight0 * (lines + 1);
						p->height += H;
					}
				}
			}
			height += p->height;
			if (height > h)
				break;
			p = p->prev;
		}

		// if m_sizeAll.cy == -1, that means we have a big window
		// but we do not know its exact value. The exact value should
		// be calculated by the background text layout thread
		m_sizeAll.cy = (height > h) ? (-1) : height;

		m_ptOffset.y = (m_sizeAll.cy > h) ? m_sizeAll.cy - h : 0;

		return 1;
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		UpdatePosition();
		return 0;
	}

	int TextLayout(int width, U16* txt, U16 characters, U16* wrapTab, hb_font_t* hb_font, hb_buffer_t* hb_buffer)
	{
		int w, MaxWidth = 0;
		U16  charMaxLen, charLen, charBaseIdx, i, idx;

		assert(characters < MAX_INPUT_MESSAGE_LEN);
		assert(characters > 0);
		assert(nullptr != wrapTab);
		assert(nullptr != hb_font);
		assert(nullptr != hb_buffer);

		wrapTab[0] = 0; // clear the wrap table

		charMaxLen = characters;
		charBaseIdx = 0;

		// we scan the whole string, maybe we can find "\n", maybe not
		for (i = 0; i < characters; i++)
		{
			if (0 == txt[i])  // it is a zero-terminated string
			{
				charMaxLen = i;
				break;
			}

			if (0x0A == txt[i])
			{
				charLen = i - charBaseIdx;
				if (charLen > 0)
				{
					w = TextLayoutOneLine(width, txt, charBaseIdx, charLen, wrapTab, hb_font, hb_buffer);
					if (MaxWidth < w)
						MaxWidth = w;
				}
				else  // "\n\n\n\n"
				{
					idx = wrapTab[0] << 1;
					wrapTab[idx + 1] = charBaseIdx;
					wrapTab[idx + 2] = 0;
					wrapTab[0]++;
				}
				charBaseIdx = i + 1; // pointing to the next character of "\n"
			}
		}

		if (charMaxLen > 0)
		{
			if (charBaseIdx <= charMaxLen - 1) // we still have characters after the last "\n"
			{
				w = TextLayoutOneLine(width, txt, charBaseIdx, (charMaxLen - charBaseIdx), wrapTab, hb_font, hb_buffer);
				if (MaxWidth < w)
					MaxWidth = w;
			}
		}

		return MaxWidth;
	}

	// We assume that txt is a single line, which means there is no "\n" in it.
	int TextLayoutOneLine(int width, U16* txt, U16 base, U16 len, U16* wrapTab, hb_font_t* hb_font, hb_buffer_t* hb_buffer)
	{
		int w, MaxWidth = 0;
		U16 charBaseIdx, charOldLen, charLen, charRemaining;
		U16 charBaseLen, halfSize, lines, i;
		U16* p;
		U16* wrapIdx;
		unsigned int idx, glyphLen;
		hb_glyph_info_t* hbinfo;
		hb_glyph_position_t* hbpos;

		assert(nullptr != wrapTab);
#ifdef DUI_DEBUG
		U16 charSum = 0;
#endif			
		lines = wrapTab[0];
		charRemaining = len;
		wrapIdx = wrapTab + 1 + (lines<<1) ; // wrapTab[0] is used to save the length
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

				hb_buffer_reset(hb_buffer);
				hb_buffer_add_utf16(hb_buffer, (const uint16_t*)p, charLen, 0, charLen);
				hb_buffer_guess_segment_properties(hb_buffer);
				hb_shape(hb_font, hb_buffer, NULL, 0);
				glyphLen = hb_buffer_get_length(hb_buffer);
				hbpos = hb_buffer_get_glyph_positions(hb_buffer, NULL);
				w = 0;
				for (i = 0; i < glyphLen; i++)
				{
					w += (DUI_ALIGN_TRUETYPE(hbpos[i].x_advance) >> 6);
				}
				if (w < width)
				{
					if (MaxWidth < w)
						MaxWidth = w;
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
					hb_buffer_reset(hb_buffer);
					hb_buffer_add_utf16(hb_buffer, (const uint16_t*)p, charLen + i, 0, charLen + i);
					hb_buffer_guess_segment_properties(hb_buffer);
					hb_shape(hb_font, hb_buffer, NULL, 0);
					glyphLen = hb_buffer_get_length(hb_buffer);
					hbpos = hb_buffer_get_glyph_positions(hb_buffer, NULL);
					w = 0;
					for (idx = 0; idx < glyphLen; idx++)
					{
						w += (DUI_ALIGN_TRUETYPE(hbpos[idx].x_advance) >> 6);
					}
					if (w >= width)
					{
						MaxWidth = width;
						break;
					}
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
		wrapTab[0] = lines;

#ifdef DUI_DEBUG
		assert(charSum == len);
#endif
		return MaxWidth;
	}

	int DrawFromBottom2Top()
	{
		return 0;
	}

	int Draw()
	{
		int x, y, dx, dy, pos, width;
		U16 i, lines, baseIdx, charNum;
		unsigned int idx, glyphLen;
		U16* m;
		U16 strLen;
		U16* utf16String;
		U32* crdata;
		U32 color;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		double R, G, B;
		double current_x, current_y;
		int H, W = DUI_ALIGN_DEFAULT32(w - 190);
		cairo_surface_t* cairo_surface;
		cairo_t* cr;
		cairo_status_t cs;
		double baseline;
		hb_glyph_info_t* hbinfo;
		hb_glyph_position_t* hbpos;

		if (m_sizeAll.cy < 0)
		{
			// because we do not know the correct m_sizeAll.cy value, we should
			// draw from bottom to the top. 
			return DrawFromBottom2Top();
		}

		R = G = B = 1;
		pos = GAP_MESSAGE;
		XChatMessage* p = m_headMessage;
		while (nullptr != p)
		{
			H = p->height;
			if (pos + H > m_ptOffset.y && pos < m_ptOffset.y + h)
			{
				dy = pos - m_ptOffset.y;
				W = p->width;
				m = p->textWrapTab;
				cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, W, H - GAP_MESSAGE);
				cairo_status_t cs = cairo_surface_status(cairo_surface);
				if (nullptr != m && CAIRO_STATUS_SUCCESS == cs)
				{
					cr = cairo_create(cairo_surface);
					cs = cairo_status(cr);
					if (CAIRO_STATUS_SUCCESS == cs)
					{
						if (p->state % 2) // me
						{
							B = 0.6196; G = 0.917647; R = 0.4156862745;
							dx = w - W - 56;
							x = w - m_scrollWidth - p->w - 6;
						}
						else
						{
							R = G = B = 1;
							dx = 56;
							x = 10;
						}
						cairo_set_font_face(cr, m_cairo_face);
						cairo_set_font_size(cr, XFONT_SIZE0);
						cairo_font_extents(cr, &m_font_extents);
						baseline = (XFONT_SIZE0 - m_font_extents.height) * 0.5 + m_font_extents.ascent + 12;
						cairo_translate(cr, GAP_LEFT4, baseline);
						cairo_set_source_rgba(cr, R, G, B, 1);
						cairo_paint(cr);
						cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 1);

						lines = *m++;
						strLen = p->message[0];
						utf16String = p->message + 1; // the first 2 bytes are the string length
						for (i = 0; i < lines; i++)
						{
							baseIdx = *m++;
							charNum = *m++;
							if (charNum > 0)
							{
								hb_buffer_reset(m_hb_buffer);
								hb_buffer_add_utf16(m_hb_buffer, (const uint16_t*)utf16String, strLen, baseIdx, charNum);
								hb_buffer_guess_segment_properties(m_hb_buffer);
								hb_shape(m_hb_font0, m_hb_buffer, NULL, 0);
								glyphLen = hb_buffer_get_length(m_hb_buffer);
								assert(glyphLen < 1024);
								hbinfo = hb_buffer_get_glyph_infos(m_hb_buffer, NULL);
								hbpos = hb_buffer_get_glyph_positions(m_hb_buffer, NULL);

								current_x = current_y = 0;
								for (idx = 0; idx < glyphLen; idx++)
								{
									m_cairo_glyphs[idx].index = hbinfo[idx].codepoint;
									m_cairo_glyphs[idx].x = current_x + hbpos[idx].x_offset / 64.;
									m_cairo_glyphs[idx].y = -(current_y + hbpos[idx].y_offset / 64.);
									current_x += hbpos[idx].x_advance / 64.;
									current_y += hbpos[idx].y_advance / 64.;
								}
								cairo_show_glyphs(cr, m_cairo_glyphs, glyphLen);
							}
							cairo_translate(cr, 0, m_lineHeight0);
						}

						crdata = (U32*)cairo_image_surface_get_data(cairo_surface);

						color = (dx > 60) ? 0xFFA9EFC5 : m_backgroundColor;
						ScreenDrawRectRound(m_screen, w, h, crdata, W, H - GAP_MESSAGE, dx, dy, m_backgroundColor, color);

						cairo_destroy(cr);

						if (p->state % 2)
							ScreenDrawRect(m_screen, w, h, (U32*)littleArrowMe, 4, 8, dx + W, dy + 13);

						ScreenDrawRectRound(m_screen, w, h, (U32*)p->icon, p->w, p->h, x, dy, m_backgroundColor, m_backgroundColor);

					}
					cairo_surface_destroy(cairo_surface);
				}
			}
			pos += H;
			p = p->next;
			if (pos >= m_ptOffset.y + h) // out of the scope of the drawing area
				break;
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

U32 TextLayoutThread(void* lpData)
{
	XWindow4* xw = (XWindow4*)lpData;
	if (nullptr != xw)
	{
		xw->ReWrapFromHead();
	}
	return 0;
}

#endif  /* __WOCHAT_WINDOWS4_H__ */

