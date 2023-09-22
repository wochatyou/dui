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

	int		m_lineHeight0 = 0;
	int		m_lineHeight1 = 0;
	int     m_widthOld = 0;

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
		
		for (size_t i = 0; i < 1; i++)
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
#if 0
		double lineHeight = 0;

		U8 testxt[34] = {
			0xE5,0xB2,0x81,0xE5,0xAF,0x92,0xE7,0x84,0xB6,0xE5,0x90,0x8E,0xE7,0x9F,0xA5,0xE6,
			0x9D,0xBE,0xE6,0x9F,0x8F,0xE4,0xB9,0x8B,0xE5,0x90,0x8E,0xE5,0x87,0x8B,0xE4,0xB9,
			0x9F, 0
		};
#endif		
		// detect the line height
		m_lineHeight0 = 0;
		cairo_surface_t* cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 128, 64);
		cairo_status_t cs = cairo_surface_status(cairo_surface);
		if (CAIRO_STATUS_SUCCESS == cs)
		{
			cairo_t* cr = cairo_create(cairo_surface);
			cs = cairo_status(cr);
			if (CAIRO_STATUS_SUCCESS == cs)
			{
				cairo_glyph_t* cairo_glyphs;
				cairo_font_extents_t font_extents;

				assert(nullptr != g_ftFace0);
				cairo_font_face_t* cairo_face = cairo_ft_font_face_create_for_ft_face(g_ftFace0, 0);
				assert(nullptr != cairo_face);
				cairo_set_font_face(cr, cairo_face);
				cairo_set_font_size(cr, XFONT_SIZE0);
				cairo_font_extents(cr, &font_extents);
				
				m_lineHeight0 = 1 + (int)font_extents.height;
#if 0
				hb_font_t* hb_font = hb_ft_font_create(g_ftFace0, NULL);
				assert(nullptr != hb_font);
				hb_buffer_t* hb_buffer = hb_buffer_create();
				hb_bool_t hs = hb_buffer_allocation_successful(hb_buffer);
				if (hs)
				{
					unsigned int i, glyphLen;
					double current_x = 0;
					double current_y = 0;

					hb_glyph_info_t* hbinfo;
					hb_glyph_position_t* hbpos;
					cairo_text_extents_t text_extents;

					cairo_glyphs = cairo_glyph_allocate(16);
					if (NULL != cairo_glyphs)
					{
						hb_buffer_add_utf8(hb_buffer, (const char*)testxt, 33, 0, -1);
						hb_buffer_guess_segment_properties(hb_buffer);
						hb_shape(hb_font, hb_buffer, NULL, 0);
						glyphLen = hb_buffer_get_length(hb_buffer);
						hbinfo = hb_buffer_get_glyph_infos(hb_buffer, NULL);
						hbpos = hb_buffer_get_glyph_positions(hb_buffer, NULL);

						current_x = 0; current_y = 0;
						for (i = 0; i < glyphLen; i++)
						{
							cairo_glyphs[i].index = hbinfo[i].codepoint;
							cairo_glyphs[i].x = current_x + hbpos[i].x_offset / 64.;
							cairo_glyphs[i].y = -(current_y + hbpos[i].y_offset / 64.);
							current_x += hbpos[i].x_advance / 64.;
							current_y += hbpos[i].y_advance / 64.;
						}

						cairo_set_source_rgba(cr, 0, 0, 0, 1);
						cairo_show_glyphs(cr, cairo_glyphs, glyphLen);

						cairo_text_extents(cr, (const char*)testxt, &text_extents);
						lineHeight = text_extents.height;
						cairo_glyph_free(cairo_glyphs);
					}
					hb_buffer_destroy(hb_buffer);
					hb_font_destroy(hb_font);
				}
#endif
				cairo_font_face_destroy(cairo_face);
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

	void UpdatePosition() 
	{
		int W, H;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		U16 i, lines, u16Num;
		XChatMessage* p = m_rootMessage;
		
		if (0 != m_widthOld)
		{
			// the width does not change, we do not need to re-calculate the text layout
			if (w == m_widthOld) 
			{
				m_ptOffset.y = (m_sizeAll.cy > h) ? m_sizeAll.cy - h : 0;
				return;
			}
		}

		m_widthOld = w;

		assert(w > 300);
		W = DUI_ALIGN_DEFAULT32(w - 220);

		m_sizeAll.cy = GAP_MESSAGE;

		while (nullptr != p)
		{
			p->width_ = 18 + TextLayout(W, p->message_, p->msgLen_);
				
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

	int TextLayout(int width, U16* txt, U16 characters)
	{
		int w, MaxWidth = 0;
		U16  charMaxLen, charLen, charBaseIdx, i, idx;

		assert(characters < MAX_INPUT_MESSAGE_LEN);
		assert(characters > 0);

		_TextWrapIdxTab[0] = 0; // clear the wrap table

		charMaxLen = characters;
		charBaseIdx = 0;

		assert(nullptr != g_ftFace0);
		hb_font_t* hb_font = hb_ft_font_create(g_ftFace0, NULL);
		assert(nullptr != hb_font);
		hb_buffer_t* hb_buffer = hb_buffer_create();
		hb_bool_t hs = hb_buffer_allocation_successful(hb_buffer);
		if (hs)
		{
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
						w = TextLayoutOneLine(width, txt, charBaseIdx, charLen, hb_font, hb_buffer);
						if (MaxWidth < w)
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

			if (charMaxLen > 0)
			{
				if (charBaseIdx <= charMaxLen - 1) // we still have characters after the last "\n"
				{
					w = TextLayoutOneLine(width, txt, charBaseIdx, (charMaxLen - charBaseIdx), hb_font, hb_buffer);
					if (MaxWidth < w)
						MaxWidth = w;
				}
			}
		}

		return MaxWidth;
	}


	// We assume that txt is a single line, which means there is no "\n" in it.
	int TextLayoutOneLine(int width, U16* txt, U16 base, U16 len, hb_font_t* hb_font, hb_buffer_t* hb_buffer)
	{
		int w, MaxWidth = 0;
		U16 charBaseIdx, charOldLen, charLen, charRemaining;
		U16 charBaseLen, halfSize, lines, i;
		U16* p;
		U16* wrapIdx;
		unsigned int glyphLen;
		hb_glyph_info_t* hbinfo;
		hb_glyph_position_t* hbpos;

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

				hb_buffer_reset(hb_buffer);
				hb_buffer_add_utf16(hb_buffer, (const uint16_t*)p, charLen, 0, charLen);
				hb_buffer_guess_segment_properties(hb_buffer);
				hb_shape(hb_font, hb_buffer, NULL, 0);
				glyphLen = hb_buffer_get_length(hb_buffer);
				hbinfo = hb_buffer_get_glyph_infos(hb_buffer, NULL);
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
					hb_buffer_add_utf16(hb_buffer, (const uint16_t*)p, charLen, 0, charLen);
					hb_buffer_guess_segment_properties(hb_buffer);
					hb_shape(hb_font, hb_buffer, NULL, 0);
					glyphLen = hb_buffer_get_length(hb_buffer);
					hbinfo = hb_buffer_get_glyph_infos(hb_buffer, NULL);
					hbpos = hb_buffer_get_glyph_positions(hb_buffer, NULL);
					w = 0;
					for (i = 0; i < glyphLen; i++)
					{
						w += (DUI_ALIGN_TRUETYPE(hbpos[i].x_advance) >> 6);
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
		_TextWrapIdxTab[0] = lines;

#ifdef DUI_DEBUG
		assert(charSum == len);
#endif
		return MaxWidth;
	}

	int Draw()
	{
		int x, y, dx, dy, pos, width;
		U16 i, lines, baseIdx, charNum;
		unsigned int idx, glyphLen;
		U16* m;
		U16* msg;
		U32* crdata;
		U32 color1;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		
		double R, G, B;
		double current_x = 0;
		double current_y = 0;

		R = G = B = 1;

		int H, W = DUI_ALIGN_DEFAULT32(w - 190);
		
		cairo_surface_t* cairo_surface;
		cairo_t* cr;
		cairo_status_t cs;
		cairo_glyph_t* cairo_glyphs;
		cairo_font_extents_t font_extents;
		double baseline;

		hb_font_t* hb_font;
		hb_buffer_t* hb_buffer;
		hb_bool_t hs;
		hb_glyph_info_t* hbinfo;
		hb_glyph_position_t* hbpos;

		assert(nullptr != g_ftFace0);
		cairo_font_face_t* cairo_face = cairo_ft_font_face_create_for_ft_face(g_ftFace0, 0);
		if (NULL == cairo_face)
			return 0;

		hb_font = hb_ft_font_create(g_ftFace0, NULL);
		if (NULL == hb_font)
		{
			cairo_font_face_destroy(cairo_face);
			return 0;
		}
		hb_buffer = hb_buffer_create();
		hs = hb_buffer_allocation_successful(hb_buffer);
		if (!hs)
		{
			hb_font_destroy(hb_font);
			cairo_font_face_destroy(cairo_face);
			return 0;
		}

		cairo_glyphs = cairo_glyph_allocate(1024);
		if (NULL == cairo_glyphs)
		{
			hb_buffer_destroy(hb_buffer);
			hb_font_destroy(hb_font);
			cairo_font_face_destroy(cairo_face);
			return 0;
		}

		pos = GAP_MESSAGE;
		XChatMessage* p = m_rootMessage;
		while (nullptr != p)
		{
			H = p->height_;

			if (pos + H > m_ptOffset.y && pos < m_ptOffset.y + h)
			{
				cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, W, H - GAP_MESSAGE);
				cairo_status_t cs = cairo_surface_status(cairo_surface);
				if (CAIRO_STATUS_SUCCESS == cs)
				{
					cr = cairo_create(cairo_surface);
					cs = cairo_status(cr);
					if (CAIRO_STATUS_SUCCESS == cs)
					{
						cairo_set_font_face(cr, cairo_face);
						cairo_set_font_size(cr, XFONT_SIZE0);
						cairo_font_extents(cr, &font_extents);
						baseline = (XFONT_SIZE0 - font_extents.height) * 0.5 + font_extents.ascent;
						cairo_translate(cr, 0, baseline + 12);

						msg = p->message_;
						hb_buffer_reset(hb_buffer);
						hb_buffer_add_utf16(hb_buffer, (const uint16_t*)msg, -1, 0, -1);
						hb_buffer_guess_segment_properties(hb_buffer);
						hb_shape(hb_font, hb_buffer, NULL, 0);
						glyphLen = hb_buffer_get_length(hb_buffer);
						assert(glyphLen < 1024);
						hbinfo = hb_buffer_get_glyph_infos(hb_buffer, NULL);
						hbpos = hb_buffer_get_glyph_positions(hb_buffer, NULL);

						current_x = 0; current_y = 0;
						for (idx = 0; idx < glyphLen; idx++)
						{
							cairo_glyphs[idx].index = hbinfo[idx].codepoint;
							cairo_glyphs[idx].x = current_x + hbpos[idx].x_offset / 64.;
							cairo_glyphs[idx].y = -(current_y + hbpos[idx].y_offset / 64.);
							current_x += hbpos[idx].x_advance / 64.;
							current_y += hbpos[idx].y_advance / 64.;
						}
						cairo_set_source_rgba(cr, R, G, B, 1);
						cairo_paint(cr);
						cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 1);
						cairo_show_glyphs(cr, cairo_glyphs, glyphLen);

						crdata = (U32*)cairo_image_surface_get_data(cairo_surface);
						ScreenDrawRect(m_screen, w, h, crdata, W, H - GAP_MESSAGE, 100, pos - m_ptOffset.y);
						cairo_destroy(cr);

						x = w - m_scrollWidth - p->w_ - 6;
						ScreenDrawRectRound(m_screen, w, h, (U32*)p->icon_, p->w_, p->h_, x, pos - m_ptOffset.y, m_backgroundColor, m_backgroundColor);
					}
					cairo_surface_destroy(cairo_surface);
				}
			}

			if (pos >= m_ptOffset.y + h)
				break;
			pos += H;
			p = p->next_;
		}
		cairo_glyph_free(cairo_glyphs);
		hb_buffer_destroy(hb_buffer);
		hb_font_destroy(hb_font);
		cairo_font_face_destroy(cairo_face);

		return 0;
#if 0
		int W, H, x, y, dx, dy, pos, width;
		U16 i, lines, baseIdx, charNum;
		U16* m;
		U16* msg;
		U32 color1;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		
		BLResult blResult;
		BLRgba32 bkcolor0(0xFF6AEA9Eu);
		BLRgba32 bkcolor1(0xFFFFFFFFu);
		BLRgba32 textColor0(0xFF232323u);
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
				if(p->width_ < W)
					width = DUI_ALIGN_DEFAULT32(p->width_);

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
						ScreenDrawRectRound(m_screen, w, h, (U32*)p->icon_, p->w_, p->h_, x, pos - m_ptOffset.y, m_backgroundColor, m_backgroundColor);

						color1 = (dx > 60) ? 0xFFA9EFC5 : m_backgroundColor;
						ScreenDrawRectRound(m_screen, w, h, (U32*)imgdata.pixelData, imgdata.size.w, imgdata.size.h, dx, pos - m_ptOffset.y, m_backgroundColor, color1);

						if (p->state_ % 2)
							ScreenDrawRect(m_screen, w, h, (U32*)littleArrowMe, 4, 8, dx + imgdata.size.w, pos - m_ptOffset.y + 13);
						
					}
				}
			}
			pos += H;
			p = p->next_;
		}
#endif
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

