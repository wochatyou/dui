#ifndef __WOCHAT_WINDOWS2_H__
#define __WOCHAT_WINDOWS2_H__

#include "dui/dui_win.h"

U16 msg[] = { 12, 0x5468,0x661f,0x661f,0xff1a,0x4e0d,0x77e5,0x6211,0x8005,0x8c13,0x6211,0x4f55,0x6c42 };

U16 gname1[] = { 6, 0x0044,0x0042,0x0041,0x57f9,0x8bad,0x7fa4 };
U16 gname2[] = { 4, 0x6587, 0x4ef6, 0x4f20, 0x8f93 };
U16 gname3[] = { 7, 0x0041,0x0049,0x804a,0x5929,0x673a,0x5668,0x4eba };
U16 timestamp[] = { 8, 0x0031,0x0037,0x003a,0x0033,0x0035,0x0020,0x0050,0x004d };

typedef struct XChatGroup
{
	XChatGroup* next_;
	U16  id_;			// Group ID
	U32* icon_;			// the bitmap data of this icon
	U8   w_;			// the width in pixel of this icon
	U8   h_;			// the height in pixel of this icon
	U16  height_;		// in pixel
	U16  unread_;		// how many unread messages? if more than 254, use ... 
	U16  member_;		// how many members in this group?
	U16* name_;			// the group name
	U64  ts_;			// the time stamp. 
	U16* lastmsg_;		// the last message of this group
} XChatGroup;

enum
{
	WM_XWIN2_GROUPCHANGED = 0x01
};

class XWindow2 : public XWindowT <XWindow2>
{
private:
	enum {
		SELECTED_COLOR = 0xFFC6C6C6,
		HOVERED_COLOR  = 0xFFDADADA,
		DEFAULT_COLOR  = 0xFFE5E5E5
	};

	enum {
		ITEM_HEIGHT = 64,
		ICON_HEIGHT = 40,
		ITEM_MARGIN = ((ITEM_HEIGHT - ICON_HEIGHT)>>1)
	};

	int		m_lineHeight0 = 0;
	int		m_lineHeight1 = 0;

	XChatGroup* m_chatgroupRoot       = nullptr;
	XChatGroup* m_chatgroupSelected   = nullptr;
	XChatGroup* m_chatgroupSelectPrev = nullptr;
	XChatGroup* m_chatgroupHovered    = nullptr;

	XChatGroup cg[64] = { 0 };

	// cairo/harfbuzz issue to cache to speed up
	cairo_font_extents_t m_font_extents = { 0 };
	cairo_glyph_t* m_cairo_glyphs = nullptr;
	cairo_font_face_t* m_cairo_face = nullptr;
	hb_font_t* m_hb_font0 = nullptr;
	hb_font_t* m_hb_font1 = nullptr;
	hb_buffer_t* m_hb_buffer = nullptr;

public:
	XWindow2()
	{
		m_backgroundColor = DEFAULT_COLOR;
		m_scrollbarColor = DEFAULT_COLOR;
		m_message = WM_WIN2_MESSAGE;
		m_property |= (DUI_PROP_HASVSCROLL | DUI_PROP_HANDLEVWHEEL);
	}

	~XWindow2()	{}

public:


	int LoadChatGroupList()
	{
		int i, total;
		XChatGroup* p;
		XChatGroup* q;

		assert(nullptr == m_chatgroupRoot);

		p = &cg[0];
		p->id_ = 0;
		p->icon_ = (U32*)xbmpGroup;
		p->name_ = (U16*)gname1;
		p->w_ = ICON_HEIGHT;
		p->h_ = ICON_HEIGHT;
		p->height_ = ITEM_HEIGHT;
		p->lastmsg_ = (U16*)msg;
		p->next_ = nullptr;

		m_chatgroupRoot = p;
		m_chatgroupSelected = m_chatgroupRoot;
		m_chatgroupSelectPrev = nullptr;
		m_chatgroupHovered = nullptr;
		total =  1;

		for (int i = 1; i < 64; i++)
		{
			q = &cg[i];
			total++;
			p->next_ = q;
			p = q;
			p->next_ = nullptr;
			p->id_ = i + 1;
			p->w_ = ICON_HEIGHT;
			p->h_ = ICON_HEIGHT;
			p->height_ = ITEM_HEIGHT;
			p->lastmsg_ = (U16*)msg;

			switch (i % 3)
			{
			case 0:
				p->icon_ = (U32*)xbmpFileTransfer;
				p->name_ = (U16*)gname2;
				break;
			case 1:
				p->icon_ = (U32*)xbmpChatGPT;
				p->name_ = (U16*)gname3;
				break;
			case 2:
				p->icon_ = (U32*)xbmpGroup;
				p->name_ = (U16*)gname1;
				break;
			}
		}

		m_sizeAll.cy = total * ITEM_HEIGHT;
		m_sizeLine.cy = ITEM_HEIGHT/3;

		return 0;
	}

	int DoCreate(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{ 
		int ret = 0;
		hb_bool_t hs = 0;

		assert(nullptr == m_cairo_glyphs);
		m_cairo_glyphs = cairo_glyph_allocate(256);
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

		ret = LoadChatGroupList();

		return ret; 
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

		return 0;
	}

	int Draw()
	{
		U32 color;
		int dx, dy, pos;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		XChatGroup* p;
		int margin = (DUI_STATUS_VSCROLL & m_status) ? m_scrollWidth : 0;
		int W = DUI_ALIGN_DEFAULT32(w - ITEM_MARGIN - ITEM_MARGIN - ICON_HEIGHT - margin - 4);
		int H = ITEM_HEIGHT;

		dx = ITEM_MARGIN;

		cairo_surface_t* cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, W, H);
		cairo_status_t cs = cairo_surface_status(cairo_surface);
		if (CAIRO_STATUS_SUCCESS == cs)
		{
			cairo_t* cr = cairo_create(cairo_surface);
			cs = cairo_status(cr);
			if (CAIRO_STATUS_SUCCESS == cs)
			{
				U32* crdata;
				U32 i, glyphLen;
				U16 strLen;
				U16* utf16String;
				double R, G, B, baseline;
				double current_x, current_y, delta0, delta1;
				hb_glyph_info_t* hbinfo;
				hb_glyph_position_t* hbpos;

				cairo_set_font_face(cr, m_cairo_face);
				cairo_set_font_size(cr, XFONT_SIZE0);
				cairo_font_extents(cr, &m_font_extents);
				baseline = (XFONT_SIZE0 - m_font_extents.height) * 0.5 + m_font_extents.ascent + 16;
				delta0 = 1;
				delta1 = 0;
				pos = 0;
				p = m_chatgroupRoot;
				while (nullptr != p)
				{
					if (pos + ITEM_HEIGHT > m_ptOffset.y)
					{
						if (p == m_chatgroupSelected)
						{
							color = SELECTED_COLOR;
							R = 0.77647; G = R; B = R;
						}
						else if (p == m_chatgroupHovered)
						{
							color = HOVERED_COLOR;
							R = 0.8549; G = R; B = R;
						}
						else
						{
							color = DEFAULT_COLOR;
							R = 0.8984375; G = 0.89804; B = G;
						}
						dy = pos - m_ptOffset.y;
						ScreenFillRect(m_screen, w, h, color, w - margin, ITEM_HEIGHT, 0, dy);
						ScreenDrawRectRound(m_screen, w, h, p->icon_, p->w_, p->h_, dx, dy + ITEM_MARGIN, color, color);

						// render the text by using harfbuzz/cairo/freetype etc.
						///////////////////////////////////////////////////////////////////////
						cairo_save(cr);
						strLen = p->name_[0];
						utf16String = p->name_ + 1; // the first 2 bytes are the string length
						hb_buffer_reset(m_hb_buffer);
						hb_buffer_add_utf16(m_hb_buffer, (const uint16_t*)utf16String, strLen, 0, strLen);
						hb_buffer_guess_segment_properties(m_hb_buffer);
						/* Shape it! */
						hb_shape(m_hb_font0, m_hb_buffer, NULL, 0);
						/* Get glyph information and positions out of the buffer. */
						glyphLen = hb_buffer_get_length(m_hb_buffer);
						assert(glyphLen < 256);
						hbinfo = hb_buffer_get_glyph_infos(m_hb_buffer, NULL);
						hbpos = hb_buffer_get_glyph_positions(m_hb_buffer, NULL);

						current_x = current_y = 0;
						for (i = 0; i < glyphLen; i++)
						{
							m_cairo_glyphs[i].index = hbinfo[i].codepoint;
							m_cairo_glyphs[i].x = current_x + (DUI_ALIGN_TRUETYPE(hbpos[i].x_offset) >> 6);
							m_cairo_glyphs[i].y = -(current_y + (DUI_ALIGN_TRUETYPE(hbpos[i].y_offset) >> 6));
							current_x += ((DUI_ALIGN_TRUETYPE(hbpos[i].x_advance) >> 6) + delta0);
							current_y += (DUI_ALIGN_TRUETYPE(hbpos[i].y_advance) >> 6);
						}
						cairo_set_source_rgba(cr, R, G, B, 1);
						cairo_paint(cr);
						cairo_set_source_rgba(cr, 0.333, 0.333, 0.333, 1);
						cairo_translate(cr, 0, baseline);
						cairo_set_font_size(cr, XFONT_SIZE0);
						cairo_show_glyphs(cr, m_cairo_glyphs, glyphLen);

						cairo_set_font_size(cr, XFONT_SIZE1);
						cairo_save(cr);

						strLen = timestamp[0];
						utf16String = timestamp + 1; // the first 2 bytes are the string length
						hb_buffer_reset(m_hb_buffer);
						hb_buffer_add_utf16(m_hb_buffer, (const uint16_t*)utf16String, strLen, 0, strLen);
						hb_buffer_guess_segment_properties(m_hb_buffer);
						/* Shape it! */
						hb_shape(m_hb_font1, m_hb_buffer, NULL, 0);
						/* Get glyph information and positions out of the buffer. */
						glyphLen = hb_buffer_get_length(m_hb_buffer);
						assert(glyphLen < 256);
						hbinfo = hb_buffer_get_glyph_infos(m_hb_buffer, NULL);
						hbpos = hb_buffer_get_glyph_positions(m_hb_buffer, NULL);

						current_x = current_y = 0;
						for (i = 0; i < glyphLen; i++)
						{
							m_cairo_glyphs[i].index = hbinfo[i].codepoint;
							m_cairo_glyphs[i].x = current_x + (DUI_ALIGN_TRUETYPE(hbpos[i].x_offset) >> 6);
							m_cairo_glyphs[i].y = -(current_y + (DUI_ALIGN_TRUETYPE(hbpos[i].y_offset) >> 6));
							current_x += ((DUI_ALIGN_TRUETYPE(hbpos[i].x_advance) >> 6) + delta1);
							current_y += (DUI_ALIGN_TRUETYPE(hbpos[i].y_advance) >> 6);
						}
						cairo_translate(cr, w - ITEM_MARGIN - ITEM_MARGIN - ICON_HEIGHT - 70, 0);
						//cairo_set_font_size(cr, XFONT_SIZE1);
						cairo_show_glyphs(cr, m_cairo_glyphs, glyphLen);

						cairo_restore(cr);
						strLen = p->lastmsg_[0];
						utf16String = p->lastmsg_ + 1; // the first 2 bytes are the string length
						hb_buffer_reset(m_hb_buffer);
						hb_buffer_add_utf16(m_hb_buffer, (const uint16_t*)utf16String, strLen, 0, strLen);
						hb_buffer_guess_segment_properties(m_hb_buffer);
						/* Shape it! */
						hb_shape(m_hb_font1, m_hb_buffer, NULL, 0);
						/* Get glyph information and positions out of the buffer. */
						glyphLen = hb_buffer_get_length(m_hb_buffer);
						assert(glyphLen < 256);
						hbinfo = hb_buffer_get_glyph_infos(m_hb_buffer, NULL);
						hbpos = hb_buffer_get_glyph_positions(m_hb_buffer, NULL);

						current_x = current_y = 0;
						for (i = 0; i < glyphLen; i++)
						{
							m_cairo_glyphs[i].index = hbinfo[i].codepoint;
							m_cairo_glyphs[i].x = current_x + (DUI_ALIGN_TRUETYPE(hbpos[i].x_offset) >> 6);
							m_cairo_glyphs[i].y = -(current_y + (DUI_ALIGN_TRUETYPE(hbpos[i].y_offset) >> 6));
							current_x += ((DUI_ALIGN_TRUETYPE(hbpos[i].x_advance) >> 6) + delta1);
							current_y += (DUI_ALIGN_TRUETYPE(hbpos[i].y_advance) >> 6);
						}
						cairo_translate(cr, 0, 20);
						//cairo_set_font_size(cr, XFONT_SIZE1);
						cairo_show_glyphs(cr, m_cairo_glyphs, glyphLen);

						crdata = (U32*)cairo_image_surface_get_data(cairo_surface);
						ScreenDrawRect(m_screen, w, h, crdata, W, H, ITEM_HEIGHT, dy);
						cairo_restore(cr);
					}

					p = p->next_;
					pos += ITEM_HEIGHT;

					if (pos >= (m_ptOffset.y + h))
						break;
				}
				cairo_destroy(cr);
			}
			cairo_surface_destroy(cairo_surface);
		}

		return 0;
	}

	int DoMouseMove(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{ 
		int hit = -1, margin;
		int ret = DUI_STATUS_NODRAW;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		XChatGroup* hovered = m_chatgroupHovered;

		RECT area = { 0 };
		RECT* r = &area;
		r->left = 0;
		margin = (DUI_STATUS_VSCROLL & m_status) ? m_scrollWidth : 0;
		r->right = w - margin;

		if (XWinPointInRect(xPos, yPos, &m_area))
		{
			int pos = 0;
			// transfer the coordination from screen to local virutal window
			xPos -= m_area.left;
			yPos -= m_area.top;

			assert(xPos >= 0);
			assert(yPos >= 0);

			XChatGroup* p = m_chatgroupRoot;
			while (nullptr != p)
			{
				r->top = pos; r->bottom = r->top + ITEM_HEIGHT;
				if (XWinPointInRect(xPos, yPos + m_ptOffset.y, r))
				{
					m_chatgroupHovered = p;
					hit++;
					if (hovered != m_chatgroupHovered) // The hovered item is changed, we need to redraw
						ret = DUI_STATUS_NEEDRAW;
					break;
				}
				p = p->next_;
				pos += ITEM_HEIGHT;
				if (pos >= (m_ptOffset.y + h))
					break;
			}
		}
		if (-1 == hit)
		{
			if (nullptr != m_chatgroupHovered)
			{
				m_chatgroupHovered = nullptr;
				ret = DUI_STATUS_NEEDRAW;
			}
		}

		return ret;
	}

	int DoLButtonDown(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{ 
		int hit = -1, margin;
		int ret = DUI_STATUS_NODRAW;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		RECT area = { 0 };
		RECT* r = &area;
		r->left = 0;
		margin = (DUI_STATUS_VSCROLL & m_status) ? m_scrollWidth : 0;
		r->right = w - margin;

		if (XWinPointInRect(xPos, yPos, &m_area))
		{
			int pos = 0;
			// transfer the coordination from screen to local virutal window
			xPos -= m_area.left;
			yPos -= m_area.top;

			XChatGroup* p = m_chatgroupRoot;
			while (nullptr != p)
			{
				r->top = pos; r->bottom = r->top + ITEM_HEIGHT;
				if (XWinPointInRect(xPos, yPos + m_ptOffset.y, r))
				{
					m_chatgroupHovered = p;
					m_chatgroupSelected = p;
					hit++;
					ret = DUI_STATUS_NEEDRAW;
					break;
				}
				p = p->next_;
				pos += ITEM_HEIGHT;
				if (pos >= (m_ptOffset.y + h))
					break;
			}
		}
		if (-1 == hit)
		{
			if (nullptr != m_chatgroupHovered)
			{
				m_chatgroupHovered = nullptr;
				ret = DUI_STATUS_NEEDRAW;
			}
		}

		return ret;
	}

	int DoLButtonUp(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{ 
		int hit = -1, margin;
		int ret = DUI_STATUS_NODRAW;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		RECT area = { 0 };
		RECT* r = &area;
		r->left = 0;
		margin = (DUI_STATUS_VSCROLL & m_status) ? m_scrollWidth : 0;
		r->right = w - margin;

		if (XWinPointInRect(xPos, yPos, &m_area))
		{
			int pos = 0;
			// transfer the coordination from screen to local virutal window
			xPos -= m_area.left;
			yPos -= m_area.top;

			XChatGroup* p = m_chatgroupRoot;
			while (nullptr != p)
			{
				r->top = pos; r->bottom = r->top + ITEM_HEIGHT;
				if (XWinPointInRect(xPos, yPos + m_ptOffset.y, r))
				{
					m_chatgroupHovered = p;
					m_chatgroupSelected = p;
					hit++;
					ret = DUI_STATUS_NEEDRAW;
					break;
				}
				p = p->next_;
				pos += ITEM_HEIGHT;
				if (pos >= (m_ptOffset.y + h))
					break;
			}
		}
		if (-1 == hit)
		{
			if (nullptr != m_chatgroupHovered)
			{
				m_chatgroupHovered = nullptr;
				ret = DUI_STATUS_NEEDRAW;
			}
		}
		else // we hit some item
		{
			assert(nullptr != m_chatgroupSelected);
			if (m_chatgroupSelectPrev != m_chatgroupSelected)
			{
				// do something here
			}
			m_chatgroupSelectPrev = m_chatgroupSelected; // prevent double selection
		}

		return ret;
	}

	int DoMouseWheel(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) 
	{ 
		return DoMouseMove(uMsg, wParam, lParam, lpData);
	}
};

#endif  /* __WOCHAT_WINDOWS2_H__ */

