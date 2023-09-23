#ifndef _DUI_EDITBOX_H_
#define _DUI_EDITBOX_H_

#define DUI_MAX_EDITSTRING          (1<<8)     // maximu input string

// We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
#define XSTB_TEXTEDIT_K_LEFT         0x200000 // keyboard input to move cursor left
#define XSTB_TEXTEDIT_K_RIGHT        0x200001 // keyboard input to move cursor right
#define XSTB_TEXTEDIT_K_UP           0x200002 // keyboard input to move cursor up
#define XSTB_TEXTEDIT_K_DOWN         0x200003 // keyboard input to move cursor down
#define XSTB_TEXTEDIT_K_LINESTART    0x200004 // keyboard input to move cursor to start of line
#define XSTB_TEXTEDIT_K_LINEEND      0x200005 // keyboard input to move cursor to end of line
#define XSTB_TEXTEDIT_K_TEXTSTART    0x200006 // keyboard input to move cursor to start of text
#define XSTB_TEXTEDIT_K_TEXTEND      0x200007 // keyboard input to move cursor to end of text
#define XSTB_TEXTEDIT_K_DELETE       0x200008 // keyboard input to delete selection or character under cursor
#define XSTB_TEXTEDIT_K_BACKSPACE    0x200009 // keyboard input to delete selection or character left of cursor
#define XSTB_TEXTEDIT_K_UNDO         0x20000A // keyboard input to perform undo
#define XSTB_TEXTEDIT_K_REDO         0x20000B // keyboard input to perform redo
#define XSTB_TEXTEDIT_K_WORDLEFT     0x20000C // keyboard input to move cursor left one word
#define XSTB_TEXTEDIT_K_WORDRIGHT    0x20000D // keyboard input to move cursor right one word
#define XSTB_TEXTEDIT_K_PGUP         0x20000E // keyboard input to move cursor up a page
#define XSTB_TEXTEDIT_K_PGDOWN       0x20000F // keyboard input to move cursor down a page
#define XSTB_TEXTEDIT_K_SHIFT        0x400000

#if 0
#undef STB_TEXTEDIT_CHARTYPE
#define STB_TEXTEDIT_CHARTYPE           uint16_t
#define STB_TEXTEDIT_UNDOSTATECOUNT     99
#define STB_TEXTEDIT_UNDOCHARCOUNT      999

#define STB_TEXTEDIT_IMPLEMENTATION
#define STB_TEXTEDIT_memmove memmove
//#include "imstb_textedit.h"
#endif

static U8 asciiTab[128] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
///////////////////////////////////////
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 
};

#define XEDIT_PROP_NONE         0x00
#define XEDIT_PROP_PASSWORD     0x01
#define XEDIT_PROP_ABCD123      0x02

enum class XEditStatus
{
    XEDIT_STATUS_NONE  = 0x00,
    XEDIT_STATUS_FOCUS = 0x01,
    XEDIT_STATUS_CARET = 0x02,
    XEDIT_STATUS_HOVER = 0x04
};

class XEditBoxSL  // single line edit box
{
    enum
    {
        XEDIT_ACTIVE_COLOR   = 0xFFFFFFFF,
        XEDIT_INACTIVE_COLOR = 0xFFE8E8E8,
        XEDIT_CARET_COLOR    = 0xFF555555
    };

public:
    int  left   = 0;
    int  top    = 0;
    int  right  = 0;
    int  bottom = 0;
    int  m_cursorW = 2;
    int  m_cursorH = 22;
    U32  m_cursorColor = XEDIT_CARET_COLOR;
    U32  m_property = XEDIT_PROP_NONE;
    U32  m_status = (U32)XEditStatus::XEDIT_STATUS_NONE;
    U32  m_statusPrev = (U32)XEditStatus::XEDIT_STATUS_NONE;
    U32  m_bkgColor = XEDIT_INACTIVE_COLOR;
    U16  m_cursorChar = 0;
    U16  m_textChar[DUI_MAX_EDITSTRING + 1] = { 0 };
    U32* m_buffer = nullptr;
    U32  m_size = 0;
    XBitmap m_bitmap;

    // cairo/harfbuzz issue to cache to speed up
    cairo_glyph_t*       m_cairo_glyphs = nullptr;
    cairo_font_face_t*   m_cairo_face   = nullptr;
    hb_font_t*           m_hb_font      = nullptr;
    hb_buffer_t*         m_hb_buffer    = nullptr;
    U16                  m_fontSize     = 16;
    int                  m_lineHeight   = 0;  // in pixel

    XEditBoxSL()
    {
        m_textChar[0] = 0;
        m_bitmap.data = nullptr;
    }

    ~XEditBoxSL()
    {
        if (nullptr != m_buffer)
        {
            free(m_buffer);
            m_buffer = nullptr;
        }
    }

    void SetBitmap(U32* data, int w, int h)
    {
        XBitmap* bmp = &m_bitmap;
        bmp->data = data;
        bmp->w = w;
        bmp->h = h;
        bmp->id = 0;
    }

    int SetState(XEditStatus state)
    { 
        int r = 0;
        U32 S = (U32)state;
        U32 status = m_status & S;
        m_status |= S;
        if ((m_status & S) != status)
            r = 1;
        return r;
    }

    int UnSetState(XEditStatus state)
    { 
        int r = 0;
        U32 S = (U32)state;
        U32 status = m_status & S;
        m_status &= ~S;
        if ((m_status & S) != status)
            r = 1;
        return r;
    }

    bool HasData() { return (nullptr != m_buffer); }
    int GetWidth() { return (right - left);  }
    int GetHeight() { return (bottom - top); }
    int GetLeftPos() { return left;  }
    int GetTopPos() { return top;  }
    int GetLineHeight() { return m_lineHeight; }
    U32* GetDrawBuffer() {  return m_buffer; }
    void SetProperty(U32 prop) { m_property |= prop; }

    bool IsHitted(int xPos, int yPos)
    {
        bool hit = false;
        if (xPos >= left && xPos < right && yPos >= top && yPos < bottom)
            hit = true;
        return hit;
    }

    int UpdatePosition(int L, int T, int R, int B)
    {
        bool reAlloc = false;
        int w, h;
        assert(L >= 0);
        assert(T >= 0);
        assert(R > 0);
        assert(B > 0);

        left = L, top = T, right = R, bottom = B;
        w = right - left, h = bottom - top;

        if (nullptr == m_buffer || (R - L) != w || (B - T) != h)
            reAlloc = true;

        if (reAlloc)
        {
            if (nullptr != m_buffer)
            {
                free(m_buffer);
                m_buffer = nullptr;
            }
            m_size = DUI_ALIGN_DEFAULT64((U32)(w * h));
            m_buffer = (U32*)malloc(m_size * sizeof(U32));
        }

        return (nullptr != m_buffer) ? 0 : 1;
    }

    int Init(FT_Face ftFace, U16 fontSize=16)
    {
        hb_bool_t hs = 0;
        cairo_font_extents_t font_extents = { 0 };

        assert(nullptr != ftFace);

        if (nullptr != m_cairo_glyphs)
        {
            cairo_glyph_free(m_cairo_glyphs);
            m_cairo_glyphs = nullptr;
        }
        m_cairo_glyphs = cairo_glyph_allocate(DUI_MAX_EDITSTRING);
        if (nullptr == m_cairo_glyphs)
            return (-1);

        if (nullptr != m_cairo_face)
        {
            cairo_font_face_destroy(m_cairo_face);
            m_cairo_face = nullptr;
        }
        m_cairo_face = cairo_ft_font_face_create_for_ft_face(ftFace, 0);
        if (nullptr == m_cairo_face)
        {
            cairo_glyph_free(m_cairo_glyphs);
            m_cairo_glyphs = nullptr;
            return (-2);
        }

        if (nullptr == m_hb_font)
        {
            hb_font_destroy(m_hb_font);
            m_hb_font = nullptr;
        }
        m_hb_font = hb_ft_font_create(ftFace, NULL);
        if (nullptr == m_hb_font)
        {
            cairo_glyph_free(m_cairo_glyphs);
            m_cairo_glyphs = nullptr;
            cairo_font_face_destroy(m_cairo_face);
            m_cairo_face = nullptr;
            return (-3);
        }

        if (nullptr == m_hb_buffer)
        {
            hb_buffer_destroy(m_hb_buffer);
            m_hb_buffer = nullptr;
        }
        m_hb_buffer = hb_buffer_create();
        hs = hb_buffer_allocation_successful(m_hb_buffer);
        if (0 == hs || nullptr == m_hb_buffer)
        {
            cairo_glyph_free(m_cairo_glyphs);
            m_cairo_glyphs = nullptr;
            cairo_font_face_destroy(m_cairo_face);
            m_cairo_face = nullptr;
            hb_font_destroy(m_hb_font);
            m_hb_font = nullptr;
            return (-4);
        }

        // detect the line height
        m_lineHeight = 0;
        {
            cairo_surface_t* cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 64, 64);
            cairo_status_t cs = cairo_surface_status(cairo_surface);
            if (CAIRO_STATUS_SUCCESS == cs)
            {
                cairo_t* cr = cairo_create(cairo_surface);
                cs = cairo_status(cr);
                if (CAIRO_STATUS_SUCCESS == cs)
                {
                    cairo_set_font_face(cr, m_cairo_face);
                    cairo_set_font_size(cr, (double)fontSize);
                    cairo_font_extents(cr, &font_extents);

                    m_lineHeight = (int)font_extents.height;
                    cairo_destroy(cr);
                }
                cairo_surface_destroy(cairo_surface);
            }
        }
        if (0 == m_lineHeight)
            return(5);

        m_cursorH = m_lineHeight;
        m_fontSize = fontSize;
        return 0;
    }

    void Term()
    {
        if (nullptr != m_cairo_glyphs)
        {
            cairo_glyph_free(m_cairo_glyphs);
            m_cairo_glyphs = nullptr;
        }
        if (nullptr != m_hb_buffer)
        {
            hb_buffer_destroy(m_hb_buffer);
            m_hb_buffer = nullptr;
        }
        if (nullptr != m_hb_font)
        {
            hb_font_destroy(m_hb_font);
            m_hb_font = nullptr;
        }
        if (nullptr != m_cairo_face)
        {
            cairo_font_face_destroy(m_cairo_face);
            m_cairo_face = nullptr;
        }
    }

    bool IsFocused()
    {
        U32 s = (U32)XEditStatus::XEDIT_STATUS_FOCUS;
        return (m_status & s);
    }

    int MoveCursorLR(int direction)
    {
        int r = 0;
        U32 s = (U32)XEditStatus::XEDIT_STATUS_FOCUS;
        if (m_status & s) // we have the focus
        {
            U16 charNum = m_textChar[0];
            if (direction > 0)
            {
                if (m_cursorChar < charNum)
                {
                    m_cursorChar++;
                    r = 1;
                }
            }
            else
            {
                if (m_cursorChar > 0)
                {
                    m_cursorChar--;
                    r = 1;
                }
            }
        }
        return r;
    }

    int ClearFocusedStatus()
    {
        int r = 0;
        U32 s0 = (U32)XEditStatus::XEDIT_STATUS_FOCUS;
        U32 s1 = (U32)XEditStatus::XEDIT_STATUS_CARET;
        U32 status = m_status & s0;

        m_status &= ~(s0 | s1);
        if ((m_status & s0) != status)
            r = 1;
        return r;
    }

    int SetFocusedStatus()
    {
        int r = 0;
        U32 s0 = (U32)XEditStatus::XEDIT_STATUS_FOCUS;
        U32 s1 = (U32)XEditStatus::XEDIT_STATUS_CARET;
        U32 status = m_status & s0;

        m_status |= (s0 | s1);
        if ((m_status & s0) != status)
            r = 1;
        return r;
    }

    void FlipCaret()
    {
        U32 s = (U32)XEditStatus::XEDIT_STATUS_CARET;
        if (s & m_status)
            m_status &= ~s;
        else
            m_status |= s;
    }

    int OnAddOneChar(U16 charcode)
    {
        int r = 0;
        U8 isAllowed, ascII = charcode & 127;
        U16 charNum = m_textChar[0];
        if (charNum < DUI_MAX_EDITSTRING)
        {
            isAllowed = asciiTab[ascII];
            if (0 != isAllowed)
            {
                charNum++;
                m_textChar[0] = charNum;
                m_textChar[charNum] = charcode;
                m_cursorChar++;
                r = 1;
            }
        }
        return r;
    }

    int Draw()
    {
        U32 color = XEDIT_INACTIVE_COLOR;
        U32 s0 = (U32)XEditStatus::XEDIT_STATUS_FOCUS;
        U32 s1 = (U32)XEditStatus::XEDIT_STATUS_HOVER;
        int cursorX = 0;
        int w  = right - left;
        int h  = bottom - top;
        U16 strLen       = m_textChar[0];
        U16* utf16String = m_textChar + 1; // the first 2 bytes are the string length

        if(s0 & m_status || s1 & m_status)
            color = XEDIT_ACTIVE_COLOR;

        if (nullptr != m_buffer)
        {
            ScreenClear(m_buffer, m_size, color);

            if (0 == strLen && XEDIT_INACTIVE_COLOR == color) // no input and is not focused
            {
                XBitmap* bmp = &m_bitmap;
                if(nullptr != bmp->data)
                    ScreenDrawRect(m_buffer, w, h, bmp->data, bmp->w, bmp->h, 4, (h - bmp->h) >> 1);
            }
            else
            {
                hb_glyph_info_t* hbinfo;
                hb_glyph_position_t* hbpos;
                U16 passwdChar[DUI_MAX_EDITSTRING + 1];

                if (XEDIT_PROP_PASSWORD & m_property)
                {
                    for (U16 i = 0; i < strLen; i++)
                        passwdChar[i] = L'*';
                    utf16String = passwdChar;
                }

                hb_buffer_reset(m_hb_buffer);
                hb_buffer_add_utf16(m_hb_buffer, (const uint16_t*)utf16String, strLen, 0, strLen);
                hb_buffer_guess_segment_properties(m_hb_buffer);
                hb_shape(m_hb_font, m_hb_buffer, NULL, 0); /* Shape it! */
                U32 glyphLen = hb_buffer_get_length(m_hb_buffer); /* Get glyph information and positions out of the buffer. */

                if (glyphLen > 0)
                {
                    int W = 0, H = 0;
                    U32 charWidth, charHeight, xOffset, yOffset;
                    double baseline, current_x, current_y;
                    double R = 1, G = 1, B = 1;
                    //R = G = B = 0;
                    hbinfo = hb_buffer_get_glyph_infos(m_hb_buffer, NULL);
                    hbpos = hb_buffer_get_glyph_positions(m_hb_buffer, NULL);
                    
                    current_x = current_y = 0;
                    for (U32 i = 0; i < glyphLen; i++)
                    {
                        m_cairo_glyphs[i].index = hbinfo[i].codepoint;
                        charWidth  = (DUI_ALIGN_TRUETYPE(hbpos[i].x_advance) >> 6);
                        charHeight = (DUI_ALIGN_TRUETYPE(hbpos[i].y_advance) >> 6);
                        xOffset    = (DUI_ALIGN_TRUETYPE(hbpos[i].x_offset) >> 6);
                        yOffset    = (DUI_ALIGN_TRUETYPE(hbpos[i].y_offset) >> 6);

                        m_cairo_glyphs[i].x = current_x + xOffset;
                        m_cairo_glyphs[i].y = -(current_y + yOffset);
                        current_x += charWidth;
                        current_y += charHeight;
                        W += charWidth;
                        if (i <= m_cursorChar)
                            cursorX += charWidth;
                    }
                    // draw it by cairo
                    W = w; H = h;
                    cairo_surface_t* cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, W, H);
                    cairo_status_t cs = cairo_surface_status(cairo_surface);
                    if (CAIRO_STATUS_SUCCESS == cs)
                    {
                        cairo_t* cr = cairo_create(cairo_surface);
                        cs = cairo_status(cr);
                        if (CAIRO_STATUS_SUCCESS == cs)
                        {
                            cairo_font_extents_t font_extents = { 0 };

                            cairo_set_font_face(cr, m_cairo_face);
                            cairo_set_font_size(cr, m_fontSize);
                            cairo_font_extents(cr, &font_extents);
                            baseline = (m_fontSize - font_extents.height) * 0.5 + font_extents.ascent + 6;
                            cairo_set_source_rgba(cr, R, G, B, 1);
                            cairo_paint(cr);
#if 0
                            B = (double)(0x97) / (double)(0xFF); G = (double)(0xC6) / (double)(0xFF); R = double(0xEB) / (double)(0xFF);
                            cairo_set_source_rgba(cr, R, G, B, 1);
                            cairo_rectangle(cr, 0, 0, 100, 30);
                            cairo_fill(cr);
#endif
                            cairo_set_source_rgba(cr, 0.333, 0.333, 0.333, 1);
                            //cairo_set_source_rgba(cr, 1, 1, 1, 1);
                            cairo_translate(cr, 0, baseline);
                            cairo_show_glyphs(cr, m_cairo_glyphs, glyphLen); // draw the text here!
                            U32* crdata = (U32*)cairo_image_surface_get_data(cairo_surface);
                            ScreenDrawRect(m_buffer, w, h, crdata, W, H, 4, 0);

                            cairo_destroy(cr);
                        }
                        cairo_surface_destroy(cairo_surface);
                    }
                }
            }
        }

        s0 = (U32)XEditStatus::XEDIT_STATUS_CARET;
        if ( s0 & m_status) // show the caret
        {
            int dx = 4 + cursorX;
            assert(h > m_cursorH);
            ScreenFillRect(m_buffer, w, h, m_cursorColor, m_cursorW, m_cursorH, dx, (h- m_cursorH)>>1);
        }
        return 0;
    }
};


#endif // _DUI_EDITBOX_H_