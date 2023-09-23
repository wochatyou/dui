#ifndef __WOCHAT_WINDOWS3_H__
#define __WOCHAT_WINDOWS3_H__

#include "dui/dui_win.h"

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
		m_property |= DUI_PROP_MOVEWIN;
		m_message = WM_WIN3_MESSAGE;
	}

	~XWindow3()
	{
	}

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

		assert(nullptr != bmp);

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

	int DoDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		return 0;
	}

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		FT_Error ft_error;

		InitButtons();

		return 0;
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		UpdateButtonPosition();
		return 0;
	}

	int Draw()
	{
#if 0
		U32 len;
		int i, W, H;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		hb_font_t* hb_font;
		assert(nullptr != g_ftFace0);

		hb_font = hb_ft_font_create(g_ftFace0, NULL);
		hb_buffer_t* hb_buffer = hb_buffer_create();
		hb_buffer_add_utf16(hb_buffer, (const uint16_t*)titlewin3, -1, 0, -1);
		hb_buffer_guess_segment_properties(hb_buffer);
		/* Shape it! */
		hb_shape(hb_font, hb_buffer, NULL, 0);

		/* Get glyph information and positions out of the buffer. */
		len = hb_buffer_get_length(hb_buffer);
		hb_glyph_info_t* info = hb_buffer_get_glyph_infos(hb_buffer, NULL);
		hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(hb_buffer, NULL);

		W = 4; H = 0;
		for (i = 0; i < len; i++)
		{
			W += (DUI_ALIGN_TRUETYPE(pos[i].x_advance) >> 6);
			H -= (DUI_ALIGN_TRUETYPE(pos[i].y_advance) >> 6);
		}
		if (HB_DIRECTION_IS_HORIZONTAL(hb_buffer_get_direction(hb_buffer)))
			H += (XFONT_SIZE0 + 8);

		cairo_surface_t* cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, W, H);
		cairo_t* cr = cairo_create(cairo_surface);
		cairo_set_source_rgba(cr, 0., 1., 1., 1.);
		cairo_paint(cr);
		cairo_set_source_rgba(cr, 1., 0., 0., 1.);
		cairo_translate(cr, 2, 4);
		/* Set up cairo font face. */
		cairo_font_face_t* cairo_face;
		cairo_face = cairo_ft_font_face_create_for_ft_face(g_ftFace0, 0);
		cairo_set_font_face(cr, cairo_face);
		cairo_set_font_size(cr, XFONT_SIZE0);
		if (HB_DIRECTION_IS_HORIZONTAL(hb_buffer_get_direction(hb_buffer)))
		{
			cairo_font_extents_t font_extents;
			cairo_font_extents(cr, &font_extents);
			double baseline = (XFONT_SIZE0 - font_extents.height) * .5 + font_extents.ascent;
			cairo_translate(cr, 0, baseline);
		}
		else
		{
			cairo_translate(cr, XFONT_SIZE0 * .5, 0);
		}
		cairo_glyph_t* cairo_glyphs = cairo_glyph_allocate(len);

		double current_x = 0;
		double current_y = 0;

		for (i = 0; i < len; i++)
		{
			cairo_glyphs[i].index = info[i].codepoint;
			cairo_glyphs[i].x = current_x + pos[i].x_offset / 64.;
			cairo_glyphs[i].y = -(current_y + pos[i].y_offset / 64.);
			current_x += pos[i].x_advance / 64.;
			current_y += pos[i].y_advance / 64.;
		}
		cairo_show_glyphs(cr, cairo_glyphs, len);
		cairo_glyph_free(cairo_glyphs);

		unsigned char* data = cairo_image_surface_get_data(cairo_surface);
		int stride = cairo_image_surface_get_stride(cairo_surface);

		ScreenDrawRect(m_screen, w, h, (U32*)data, W, H, 10, 10);

		cairo_font_face_destroy(cairo_face);
		cairo_destroy(cr);
		cairo_surface_destroy(cairo_surface);
		hb_buffer_destroy(hb_buffer);
		hb_font_destroy(hb_font);
#endif
		return 0;
	}

	int UpdateTitle(uint16_t* title)
	{
		return 0;
	}
};

#endif  /* __WOCHAT_WINDOWS3_H__ */

