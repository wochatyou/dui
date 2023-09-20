#ifndef __DUI_WIN_H__
#define __DUI_WIN_H__

#include <stdint.h>
#include "dui_mempool.h"

#define S8      int8_t
#define S16     int16_t
#define S32     int32_t
#define S64     int64_t

#define U8      uint8_t
#define U16     uint16_t
#define U32     uint32_t
#define U64     uint64_t

#define DUI_DEBUG	1

/* DUI_ALIGN() is only to be used to align on a power of 2 boundary */
#define DUI_ALIGN(size, boundary) (((size) + ((boundary) -1)) & ~((boundary) - 1))
#define DUI_ALIGN_DEFAULT32(size)   DUI_ALIGN(size, 4)
#define DUI_ALIGN_DEFAULT64(size)   DUI_ALIGN(size, 8)      /** Default alignment */
#define DUI_ALIGN_PAGE(size)        DUI_ALIGN(size, 1<<16)


int ScreenClear(uint32_t* dst, uint32_t size, uint32_t color);

int ScreenDrawHLine(uint32_t* dst, int w, int h, int position, int stroke, uint32_t color);

int ScreenDrawRect(uint32_t* dst, int w, int h, uint32_t* src, int sw, int sh, int dx, int dy);

int ScreenFillRect(uint32_t* dst, int w, int h, uint32_t color, int sw, int sh, int dx, int dy);

int ScreenDrawRectRound(uint32_t* dst, int w, int h, uint32_t* src, int sw, int sh, int dx, int dy, uint32_t color0, uint32_t color1);

int ScreenFillRectRound(uint32_t* dst, int w, int h, uint32_t color, int sw, int sh, int dx, int dy, uint32_t c1, uint32_t c2);


#ifdef _MSC_VER
#define DUI_NO_VTABLE __declspec(novtable)
#else
#define DUI_NO_VTABLE
#endif

typedef struct tagXPOINT
{
    int  x;
    int  y;
} XPOINT;

typedef struct tagXSIZE
{
    int  cx;
    int  cy;
} XSIZE;

typedef struct tagXRECT
{
    int    left;
    int    top;
    int    right;
    int    bottom;
} XRECT;

// a pure 32-bit true color bitmap object
typedef struct XBitmap
{
    U8    id;
    U32*  data;
    int   w;
    int   h;
} XBitmap;

enum XButtonProperty
{
    XBUTTON_PROP_NONE   = 0x00,
    XBUTTON_PROP_ROUND  = 0x01,
    XBUTTON_PROP_STATIC = 0x02
};

enum XButtonState
{
    XBUTTON_STATE_HIDDEN = 0,
    XBUTTON_STATE_NORMAL,
    XBUTTON_STATE_HOVERED,
    XBUTTON_STATE_PRESSED,
    XBUTTON_STATE_ACTIVE
};

typedef struct XButton
{
    U8       id        : 6;
    U8       property  : 2;
    U8       state     : 4;
    U8       statePrev : 4;
    int      left;
    int      top;
    int      right;
    int      bottom;
    // all XBitmpas should have extactly the same size
    XBitmap* imgNormal;  
    XBitmap* imgHover;
    XBitmap* imgPress;
    XBitmap* imgActive;
    int (*pfAction) (void* obj, U32 uMsg, U64 wParam, U64 lParam);
} XButton;

// determin if one object is hitted
#define XWinPointInRect(x, y, OBJ)      (((x) >= ((OBJ)->left)) && ((x) < ((OBJ)->right)) && ((y) >= ((OBJ)->top)) && ((y) < ((OBJ)->bottom)))

#define DUI_MAX_EDITSTRING      (1<<10)     // maximu input string

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

enum
{
    XEDIT_STATUS_NONE   = 0x00,
    XEDIT_STATUS_FOCUS  = 0x01,
    XEDIT_STATUS_CARET  = 0x02
};

class XEditBox
{
public:
    int left;
    int top;
    int right;
    int bottom;
    int _cursorX;
    int _cursorY;
    int _cursorW;
    int _cursorH;
    U32 _cursorColor;
    U32 _status;
    U32 _backgroundColor;
    U16 _text[DUI_MAX_EDITSTRING] = { 0 };
    U32* _buffer;
    U32  _size;

    XEditBox()
    {
        _status = XEDIT_STATUS_NONE;
        _cursorX = _cursorY = 4;
        _cursorW = 1;
        _cursorH = 22;
        _buffer = nullptr;
        _size = 0;
        _backgroundColor = 0xFFFFFFFF;
        _cursorColor = 0xFF000000;
    }

    bool IsFocused()
    {
        return (_status & XEDIT_STATUS_FOCUS);
    }

    void ClearFocusedStatus()
    {
        _status &= ~XEDIT_STATUS_FOCUS;
    }

    void SetFocusedStatus()
    {
        _status |= XEDIT_STATUS_FOCUS;
    }

    void AttachScreenBuffer(U32* buf, U32 size)
    {
        _buffer = buf;
        _size = size;
    }

    int Draw()
    {
        if(0 == (_status & XEDIT_STATUS_FOCUS))
            return 0;

        ScreenClear(_buffer, _size, _backgroundColor);
        
        if(XEDIT_STATUS_CARET & _status)
        {
            _status &= ~XEDIT_STATUS_CARET;
        }
        else
        {
            _status |= XEDIT_STATUS_CARET;
        }

        if(XEDIT_STATUS_CARET & _status)
        {
            ScreenFillRect(_buffer, right - left, bottom - top, _cursorColor, _cursorW, _cursorH, _cursorX, _cursorY);
        }

        return 0;
    }
};

enum
{
    DUI_STATUS_NODRAW    = 0x00,    // do not need to draw      
    DUI_STATUS_VISIBLE   = 0x01,    // is this virtual window visible?
    DUI_STATUS_VSCROLL   = 0x02,    // is this virtual window has vertical scroll bar?
    DUI_STATUS_HSCROLL   = 0x04,    // is this virtual window has horizonal scroll bar?
    DUI_STATUS_ISFOCUS   = 0x08,    // is the input redirected into this virutal window?
    DUI_STATUS_NEEDRAW   = 0x10,    // does this virtual windows need to be redraw?
    DUI_STATUS_HASCARET  = 0x20,    // is this virutal window has a caret?
};

enum
{
    DUI_PROP_NONE         = 0x00,   // None Properties
    DUI_PROP_MOVEWIN      = 0x01,   // Move the whole window while LButton is pressed
    DUI_PROP_BTNACTIVE    = 0x02,   // no active button on this virutal window
    DUI_PROP_HASVSCROLL   = 0x04,    // have vertical scroll bar
    DUI_PROP_HASHSCROLL   = 0x08,
    DUI_PROP_HANDLEVWHEEL = 0x10,   // does this window need to handle mouse wheel?
    DUI_PROP_HANDLEHWHEEL = 0x20,
    DUI_PROP_HANDLETIMER  = 0x40
};

enum
{
    DEFAULT_BORDER_COLOR        = 0xFFBBBBBB,
    DEFAULT_SCROLLBKG_COLOR     = 0xFFF9F3F1,
    DEFAULT_SCROLLTHUMB_COLOR   = 0xFFC0C1C4,
    DEFAULT_SCROLLTHUMB_COLORA  = 0xFFAAABAD
};


template <class T>
class DUI_NO_VTABLE XWindowT
{
private:    
    enum class XDragMode { DragNone, DragVertical, DragHorizonal };

public:
    void*   m_hWnd = nullptr;
    U32*    m_screen = nullptr;
    U32     m_size = 0;
    U8      m_Id[8] = { 0 }; // for debugging 
    XRECT   m_area = { 0 };  // the area of this window in the client area of parent window

    MemoryContext m_pool = nullptr;

    int     m_buttonStartIdx = 0;
    int     m_buttonEndIdx = -1;
    int     m_buttonActiveIdx = -1;

    int     m_scrollWidth = 8; // in pixel

    XPOINT m_ptOffset = { 0 };
    XPOINT m_ptOffsetOld = { 0 };
    XSIZE  m_sizeAll = { 0 };
    XSIZE  m_sizeLine = { 0 };
    XSIZE  m_sizePage = { 0 };
    int    m_cxyDragOffset = 0;
    XPOINT m_ptMouse = { 0 };

    XDragMode  m_DragMode = XDragMode::DragNone;

#ifdef _WIN32
    HCURSOR  m_cursorHand = nullptr;
#else
    void*  m_cursorHand = nullptr;
#endif
    U32  m_status = DUI_STATUS_VISIBLE;
    U32  m_property = DUI_PROP_NONE;
    U32  m_message = 0;
    U32  m_backgroundColor = DEFAULT_BACKGROUND_COLOR;
    U32  m_scrollbarColor = DEFAULT_SCROLLBKG_COLOR;
    U32  m_thumbColor = DEFAULT_SCROLLTHUMB_COLOR;

    enum {
        MAX_XBUTTONS = 16,
        MAX_XBUTTOn_BITMAPS = (MAX_XBUTTONS << 2)
    };

    XButton  m_button[MAX_XBUTTONS];
    XBitmap  m_bitmap[MAX_XBUTTOn_BITMAPS];

public:
    XWindowT()
    {
        U8 id;
        XButton* button;
        XBitmap* bmp;
#ifdef _WIN32        
        m_cursorHand = ::LoadCursor(nullptr, IDC_HAND);
#else
        #error You have to provide platform-specified load cursor function here
#endif        
        assert(NULL != m_cursorHand);

        static_assert(MAX_XBUTTONS < (1 << 6));
        
        // initialize the button's status
        for (id = 0; id < MAX_XBUTTONS; id++)
        {
            button = &m_button[id];
            button->id = id;
            button->property = XBUTTON_PROP_NONE;
            button->statePrev = button->state = XBUTTON_STATE_NORMAL;
            button->left = button->right = button->top = button->bottom = 0;
            button->imgNormal = button->imgHover = button->imgPress = button->imgActive = nullptr;
            button->pfAction = nullptr;
        }

        // initialize the bitmap's status
        for (id = 0; id < MAX_XBUTTOn_BITMAPS; id++)
        {
            bmp = &m_bitmap[id];
            bmp->id = id;
            bmp->data = nullptr;
            bmp->w = bmp->h = 0;
        }
    }

    ~XWindowT() 
    {
        if(nullptr != m_pool)
            mempool_destroy(m_pool);
    }

    bool IsRealWindow(void* hwnd)
    {
#ifdef _WIN32
        return (::IsWindow((HWND)hwnd));
#else
        return false;
#endif
    }

    // Windows Id is used for debugging purpose
    void SetWindowId(const U8* id, U8 bytes)
    {
        if (bytes > 7)
            bytes = 7;

        for(U8 i = 0; i < bytes; i++)
            m_Id[i] = *id++;

        m_Id[bytes] = 0;
    }

    bool IsVisible() const
    {
        return (0 != (m_status & DUI_STATUS_VISIBLE));
    }

    XRECT* GetWindowArea()
    {
        return &m_area;
    }

    void PostWindowHide() {}
    void WindowHide()
    {
        m_status &= (~DUI_STATUS_VISIBLE);

        T* pT = static_cast<T*>(this);
        pT->PostWindowHide();
    }

    void PostWindowShow() {}
    void WindowShow()
    {
        m_status |= DUI_STATUS_VISIBLE;

        T* pT = static_cast<T*>(this);
        pT->PostWindowShow();

    }

    int NotifyParent(U32 uMsg, U64 wParam, U64 lParam)
    {
        PostWindowMessage(uMsg, wParam, lParam);
        return 0;
    }

    int ClearButtonStatus() 
    { 
        int ret = DUI_STATUS_NODRAW;
        U8 state;
        XButton* button;

        assert(m_buttonEndIdx < MAX_XBUTTONS);
        assert(m_buttonStartIdx >= 0 &&  m_buttonStartIdx < MAX_XBUTTONS);

        for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
        {
            button = &m_button[i]; 

            if (XBUTTON_STATE_NORMAL != button->state && XBUTTON_STATE_ACTIVE != button->state)
            {
                m_status |= DUI_STATUS_NEEDRAW; // Invalidate the screen
                ret = DUI_STATUS_NEEDRAW;
            }

            button->state = XBUTTON_STATE_NORMAL;
        }

        if (m_buttonActiveIdx >= m_buttonStartIdx && m_buttonActiveIdx <= m_buttonEndIdx)
        {
            button = &m_button[m_buttonActiveIdx];
            button->state = XBUTTON_STATE_ACTIVE;
        }

        return ret;
    }

    U32* GetScreenBuffer()
    {
        return m_screen;
    }

    void* SetWindowCapture()
    {
        void* winhandle = nullptr;

        assert(IsRealWindow(m_hWnd));
        
        if (IsRealWindow(m_hWnd))
        {
#if defined(_WIN32)
            winhandle = ::SetCapture((HWND)m_hWnd);
#endif
        }

        return winhandle;
    }

    void* GetWindowCapture()
    {
        void* winhandle = nullptr;
        
        assert(IsRealWindow(m_hWnd));

#if defined(_WIN32)
        winhandle = ::GetCapture();
#endif
        return winhandle;
    }

    void ReleaseWindowCapture()
    {
#if defined(_WIN32)
        ::ReleaseCapture();
#endif
    }

    bool PostWindowMessage(U32 message, U64 wParam = 0, U64 lParam = 0)
    {
        bool bRet = false;

        assert(IsRealWindow(m_hWnd));

        if (IsRealWindow(m_hWnd))
        {
#if defined(_WIN32)
            bRet = ::PostMessage((HWND)m_hWnd, (UINT)message, (WPARAM)wParam, (LPARAM)lParam);
#endif
        }

        return bRet;
    }

    void DrawButton(XButton* button)
    {
        U32* dst;
        U32* src;
        int dx = button->left;
        int dy = button->top;
        int w = m_area.right - m_area.left;
        int h = m_area.bottom - m_area.top;

        dst = m_screen;
        assert(nullptr != dst);
        assert(nullptr != button);

        if (XBUTTON_STATE_HIDDEN != button->state) // this button is visible
        {

            XBitmap* bitmap = button->imgNormal;
            switch (button->state)
            {
            case XBUTTON_STATE_PRESSED:
                bitmap = button->imgPress;
                break;
            case XBUTTON_STATE_HOVERED:
                bitmap = button->imgHover;
                break;
            case XBUTTON_STATE_ACTIVE:
                bitmap = button->imgActive;
                break;
            default:
                break;
            }

            assert(nullptr != bitmap);

            src = bitmap->data;

            assert(nullptr != src);

            if (XBUTTON_PROP_ROUND & button->property)
            {
                ScreenDrawRectRound(dst, w, h, src, bitmap->w, bitmap->h, dx, dy, m_backgroundColor, m_backgroundColor);
            }
            else
            {
                ScreenDrawRect(dst, w, h, src, bitmap->w, bitmap->h, dx, dy);
            }
        }
    }

    void UpdatePosition() {}
    void SetPosition(RECT* r, U32* screen, U32 size = 0)
    {
        if (nullptr != r)
        {
            m_area.left   = r->left; 
            m_area.top    = r->top; 
            m_area.right  = r->right; 
            m_area.bottom = r->bottom;
        }
        else
        {
            m_area.left = m_area.top = m_area.right = m_area.bottom = 0;
        }

        m_screen = screen;
        
        if(0 != size)
            m_size = size;
        else
            m_size = (U32)((m_area.right - m_area.left) * (m_area.bottom - m_area.top));

        m_status |= DUI_STATUS_NEEDRAW;
        
        if (nullptr != r)
        {
            T* pT = static_cast<T*>(this);
            pT->UpdatePosition();
        }
    }

    int Draw() { return 0; }

    U32* Render() 
    {
        U32* screenBuf = nullptr;
        U8 status = m_status & (DUI_STATUS_VISIBLE | DUI_STATUS_NEEDRAW);

        // We only draw this virtual window when 1: it is visible, and 2: it needs draw
        if ((DUI_STATUS_VISIBLE | DUI_STATUS_NEEDRAW) == status)
        {
            int w = m_area.right - m_area.left;
            int h = m_area.bottom - m_area.top;
            XButton* button;

            assert(nullptr != m_screen);
            // fill the whole screen of this virutal window with a single color
            ScreenClear(m_screen, m_size, m_backgroundColor);

            if (DUI_STATUS_VSCROLL & m_status) // We have the vertical scroll bar to draw
            {
                int thumb_start, thumb_height, thumb_width;
                int vOffset = m_ptOffset.y;
                int vHeight = m_sizeAll.cy;
                assert(m_sizeAll.cy > h);

                thumb_width = m_scrollWidth - 2;
                thumb_start = (vOffset * h) / vHeight;
                thumb_height = (h * h) / vHeight;
                if(thumb_height < 16)
                    thumb_height = 16; // we keep the thumb mini size to 16 pixels

                // Draw the vertical scroll bar
                ScreenFillRect(m_screen, w, h, m_scrollbarColor, m_scrollWidth, h, w - m_scrollWidth, 0);
                ScreenFillRectRound(m_screen, w, h, m_thumbColor, thumb_width, thumb_height, w - m_scrollWidth + 1, thumb_start, m_scrollbarColor, 0xFFD6D3D2);
            }

            for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
            {
                button = &m_button[i];
                DrawButton(button);
                button->statePrev = button->state;
            }

            T* pT = static_cast<T*>(this);
            pT->Draw();

            screenBuf = m_screen;
        }

        // to avoid another needless draw
        m_status &= (~DUI_STATUS_NEEDRAW);
        
        return screenBuf;
    }

    int DoSize(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int OnSize(U32 uMsg, U64 wParam, U64 lParam, void* lpData)
    {
        XRECT* r = (XRECT*)lParam;
        if (nullptr != r)
        {
            m_area.left   = r->left;
            m_area.top    = r->top;
            m_area.right  = r->right;
            m_area.bottom = r->bottom;
            m_size = (U32)((r->right - r->left) * (r->bottom - r->top));
        }
        else
        {
            m_area.left = m_area.top = m_area.right = m_area.bottom = 0;
            m_size = 0;
        }

        m_screen = (U32*)lpData;
        assert(nullptr != m_screen);

        if (nullptr != r)
        {
            T* pT = static_cast<T*>(this);
            pT->DoSize(uMsg, wParam, lParam, lpData);
        }

        m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window

        return DUI_STATUS_NEEDRAW;
    }

    int DoTimer(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int OnTimer(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = DUI_STATUS_NODRAW;
        if(DUI_PROP_HANDLETIMER & m_status)
        {
            T* pT = static_cast<T*>(this);
            r = pT->DoTimer(uMsg, wParam, lParam, lpData);
            if(DUI_STATUS_NODRAW != r)
                m_status |= DUI_STATUS_NEEDRAW;
        }

        return r;
    }

    int DoMouseWheel(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int OnMouseWheel(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        if (DUI_PROP_HANDLEVWHEEL & m_property)
        {
            int r0 = DUI_STATUS_NODRAW;
            int r1 = DUI_STATUS_NODRAW;
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            if (XWinPointInRect(xPos, yPos, &m_area))
            {
                int h = m_area.bottom - m_area.top;
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);

                if (m_sizeAll.cy > h)
                {
                    r0 = DUI_STATUS_NEEDRAW;
                    assert(m_sizeLine.cy > 0);
                    if (delta < 0)
                        m_ptOffset.y += m_sizeLine.cy;
                    else
                        m_ptOffset.y -= m_sizeLine.cy;

                    if (m_ptOffset.y < 0)
                        m_ptOffset.y = 0;
                    if (m_ptOffset.y > (m_sizeAll.cy - h))
                        m_ptOffset.y = m_sizeAll.cy - h;
                }

                {  // let the derived class to do its stuff
                    T* pT = static_cast<T*>(this);
                    r1 = pT->DoMouseWheel(uMsg, wParam, lParam, lpData);
                }
            }

            if (DUI_STATUS_NODRAW != r0 || DUI_STATUS_NODRAW != r1)
            {
                m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window
                return DUI_STATUS_NEEDRAW;
            }
        }

        return DUI_STATUS_NODRAW;

    }

    int DoMouseMove(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int OnMouseMove(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        XButton* button;
        int r0  = DUI_STATUS_NODRAW;
        int r1  = DUI_STATUS_NODRAW;
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int w = m_area.right - m_area.left;
        int h = m_area.bottom - m_area.top;

        m_ptMouse.x = xPos; m_ptMouse.y = yPos;

        if (GetWindowCapture() != m_hWnd) // we are not in drage mode
        {
            for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
            {
                button = &m_button[i];
                button->state = XBUTTON_STATE_NORMAL;
            }

            if (m_buttonActiveIdx >= m_buttonStartIdx)
            {
                assert(0 == m_buttonStartIdx);
                assert(m_buttonActiveIdx <= m_buttonEndIdx);
                button = &m_button[m_buttonActiveIdx];
                button->state = XBUTTON_STATE_ACTIVE;
            }
        }

        if (XDragMode::DragVertical == m_DragMode)
        {
            m_status |= DUI_STATUS_VSCROLL;

            m_ptOffset.y = m_ptOffsetOld.y + ((yPos - m_cxyDragOffset) * m_sizeAll.cy) / h;

            if (m_ptOffset.y < 0)
                m_ptOffset.y = 0;
            if (m_ptOffset.y > (m_sizeAll.cy - h))
                m_ptOffset.y = m_sizeAll.cy - h;
        }
        else 
        {
            if (XWinPointInRect(xPos, yPos, &m_area)) // the mosue is in this area
            {
                int hit = -1;  // no hit so far

                if (DUI_PROP_HASVSCROLL & m_property) // handle the vertical bar
                {
                    U32 status = m_status;  // save previous state
                    m_status &= (~DUI_STATUS_VSCROLL);

                    assert(m_area.right > m_scrollWidth);
                    
                    if (m_sizeAll.cy > h) // the virutal window size is bigger than the real window size
                    {
                        U32 thumbColor = m_thumbColor;
                        m_status |= DUI_STATUS_VSCROLL;
                        m_thumbColor = DEFAULT_SCROLLTHUMB_COLOR;
                        if(xPos >= (m_area.right - m_scrollWidth))
                            m_thumbColor = DEFAULT_SCROLLTHUMB_COLORA;

                        if(thumbColor != m_thumbColor)
                            r0 = DUI_STATUS_NEEDRAW;
                    }

                    if ((DUI_STATUS_VSCROLL & status) != (DUI_STATUS_VSCROLL & m_status))
                        r0 = DUI_STATUS_NEEDRAW;
                }

                if (GetWindowCapture() != m_hWnd)
                {
                    // transfer the coordination from real window to local virutal window
                    xPos -= m_area.left;
                    yPos -= m_area.top;
                    assert(xPos >= 0);
                    assert(yPos >= 0);

                    for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
                    {
                        button = &m_button[i];
                        if (XWinPointInRect(xPos, yPos, button)) // the mouse is over this button
                        {
                            hit = i;
                            break;
                        }
                    }
                    if (-1 != hit) // we are hovering on some button
                    {
                        assert(0 == m_buttonStartIdx);
                        assert(m_buttonEndIdx >= m_buttonStartIdx);

                        button = &m_button[hit];
                        if (0 == (XBUTTON_PROP_STATIC & button->property)) // it is not a static button
                        {
                            SetCursor(m_cursorHand);
                            button->state = XBUTTON_STATE_HOVERED;
                            r0 = DUI_STATUS_NEEDRAW;
                        }
                    }
                }
            }
            else // the mouse is not in our area
            {
                // handle the vertical bar
                if (DUI_STATUS_VSCROLL & m_status)
                {
                    m_status &= (~DUI_STATUS_VSCROLL); // we should not dispaly the vertical bar
                    r0 = DUI_STATUS_NEEDRAW;
                }
            }
        }

        // if the state is not equal to the previous state, we need to redraw it
        for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
        {
            button = &m_button[i];
            if (button->state != button->statePrev)
            {
                r0 = DUI_STATUS_NEEDRAW;
                break;
            }
        }

        {  // let the derived class to do its stuff
            T* pT = static_cast<T*>(this);
            r1 = pT->DoMouseMove(uMsg, wParam, lParam, lpData);
        }

        if (DUI_STATUS_NODRAW != r0 || DUI_STATUS_NODRAW != r1 || XDragMode::DragNone != m_DragMode)
        {
            m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window
            return DUI_STATUS_NEEDRAW;
        }
        return DUI_STATUS_NODRAW;
    }

    int DoFocusGet(U32 uMsg, int xPos, int yPos, void* lpData = nullptr) { return 0; }
    int DoFocusLose(U32 uMsg, int xPos, int yPos, void* lpData = nullptr) { return 0; } 

    int DoLButtonDown(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int OnLButtonDown(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int rx = DUI_STATUS_NODRAW;
        int r0 = DUI_STATUS_NODRAW;
        int r1 = DUI_STATUS_NODRAW;

        XButton* button;
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        m_ptMouse.x = xPos; m_ptMouse.y = yPos;

        m_DragMode = XDragMode::DragNone;
        
        for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
        {
            button = &m_button[i];
            button->state = XBUTTON_STATE_NORMAL;
        }

        if (m_buttonActiveIdx >= m_buttonStartIdx)
        {
            assert(0 == m_buttonStartIdx);
            assert(m_buttonActiveIdx <= m_buttonEndIdx);
            button = &m_button[m_buttonActiveIdx];
            button->state = XBUTTON_STATE_ACTIVE;
        }

        if (XWinPointInRect(xPos, yPos, &m_area))
        {
            int hit = -1;
            int w = m_area.right - m_area.left;
            int h = m_area.bottom - m_area.top;

            // the mouse click my area, so I have the focus
            m_status |= DUI_STATUS_ISFOCUS; 
            
            T* pT = static_cast<T*>(this);
            rx = pT->DoFocusGet(uMsg, xPos, yPos, lpData);

            // handle the vertical bar
            if (DUI_PROP_HASVSCROLL & m_property)
            {
                if (xPos >= (m_area.right - m_scrollWidth))
                {
                    U32 status = m_status;
                    m_status &= (~DUI_STATUS_VSCROLL);

                    if (m_sizeAll.cy > h)
                    {
                        int thumb_start = (m_ptOffset.y * h) / m_sizeAll.cy;
                        int thumb_height = (h * h) / m_sizeAll.cy;
                
                        if(thumb_height < 16)
                            thumb_height = 16; // we keep the thumb mini size to 16 pixels

                        m_status |= DUI_STATUS_VSCROLL;

                        assert(m_ptOffset.y >= 0);
                        assert(m_ptOffset.y <= m_sizeAll.cy - h);

                        if (yPos > (m_area.top + thumb_start) && yPos < (m_area.top + thumb_start + thumb_height))
                        {
                            // we hit the thumb
                            m_cxyDragOffset = yPos;
                            m_ptOffsetOld.y = m_ptOffset.y;
                            m_DragMode = XDragMode::DragVertical;
                            SetWindowCapture();
                        } 
                        else
                        {
                            int thumb_start_new = (yPos - m_area.top) - (thumb_height >> 1);
                            if (thumb_start_new < 0)
                                thumb_start_new = 0;
                            if (thumb_start_new > h - thumb_height)
                                thumb_start_new = h - thumb_height;

                            m_ptOffset.y = (thumb_start_new * m_sizeAll.cy)/h;
                        }

                        m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window
                        return DUI_STATUS_NEEDRAW;
                    }

                    if ((DUI_STATUS_VSCROLL & status) != (DUI_STATUS_VSCROLL & m_status))
                        r0 = DUI_STATUS_NEEDRAW;
                }
            }
            else if (DUI_PROP_HASHSCROLL & m_property) // handle the horizonal bar
            {
                // TODO
            }

            // transfer the coordination from real window to local virutal window
            xPos -= m_area.left; 
            yPos -= m_area.top;
            assert(xPos >= 0);
            assert(yPos >= 0);

            for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
            {
                button = &m_button[i];
                if (XWinPointInRect(xPos, yPos, button))
                {
                    hit = i;
                    break;
                }
            }
            if (-1 != hit) // we are hitting some button
            {
                button = &m_button[hit];
                if (0 == (XBUTTON_PROP_STATIC & button->property)) // it is not a static button
                {
                    SetCursor(m_cursorHand);
                    button->state = XBUTTON_STATE_PRESSED;
                    r0 = DUI_STATUS_NEEDRAW;
                }
            }
            else
            {   // if the mouse does not hit the button, we can move the whole real window
                if (DUI_PROP_MOVEWIN & m_property)
                    PostWindowMessage(WM_NCLBUTTONDOWN, HTCAPTION, lParam);
            }
        }
        else
        {
            m_status &= ~DUI_STATUS_ISFOCUS; // this window lose focus
            T* pT = static_cast<T*>(this);
            rx = pT->DoFocusLose(uMsg, xPos, yPos, lpData);
        }

        // if the state is not equal to the previous state, we need to redraw it
        for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
        {
            button = &m_button[i];
            if (button->state != button->statePrev)
            {
                r0 = DUI_STATUS_NEEDRAW;
                break;
            }
        }

        {
            T* pT = static_cast<T*>(this);
            r1 = pT->DoLButtonDown(uMsg, wParam, lParam, lpData);
        }

        if (DUI_STATUS_NODRAW != r0 || DUI_STATUS_NODRAW != r1 || DUI_STATUS_NODRAW != rx)
        {
            m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window
            return DUI_STATUS_NEEDRAW;
        }
        return DUI_STATUS_NODRAW;

    }

    int DoLButtonUp(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int OnLButtonUp(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r0 = DUI_STATUS_NODRAW;
        int r1 = DUI_STATUS_NODRAW;
        XButton* button;
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        m_ptMouse.x = xPos; m_ptMouse.y = yPos;

        if (GetWindowCapture() == m_hWnd)
        {
            ReleaseWindowCapture();
        }

        m_DragMode = XDragMode::DragNone;
        m_ptOffsetOld.x = -1, m_ptOffsetOld.y = -1;

        for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
        {
            button = &m_button[i];
            button->state = XBUTTON_STATE_NORMAL;
        }

        if (m_buttonActiveIdx >= m_buttonStartIdx)
        {
            assert(0 == m_buttonStartIdx);
            assert(m_buttonActiveIdx <= m_buttonEndIdx);
            button = &m_button[m_buttonActiveIdx];
            button->state = XBUTTON_STATE_ACTIVE;
        }

        if (XWinPointInRect(xPos, yPos, &m_area))
        {
            int hit = -1;
            // transfer the coordination from real window to local virutal window
            xPos -= m_area.left;
            yPos -= m_area.top;
            assert(xPos >= 0);
            assert(yPos >= 0);
            for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
            {
                button = &m_button[i];
                if (XWinPointInRect(xPos, yPos, button))
                {
                    hit = i;
                    break;
                }
            }
            if (-1 != hit) // we are hitting some button
            {
                button = &m_button[hit];
                if (0 == (XBUTTON_PROP_STATIC & button->property)) // it is not a static button
                {
                    SetCursor(m_cursorHand);
                    if (m_buttonActiveIdx >= m_buttonStartIdx)
                        m_button[m_buttonActiveIdx].state = XBUTTON_STATE_NORMAL;

                    if (DUI_PROP_BTNACTIVE & m_property)
                    {
                        m_buttonActiveIdx = hit;
                        button->state = XBUTTON_STATE_ACTIVE;
                    }
                    else
                    {
                        m_buttonActiveIdx = -1;
                        button->state = XBUTTON_STATE_HOVERED;
                    }
                    r0 = DUI_STATUS_NEEDRAW;

                    // call the Action binded to this button
                    if (nullptr != button->pfAction)
                        button->pfAction(this, m_message, (WPARAM)hit , 0);
                }
            }
        }

        // if the state is not equal to the previous state, we need to redraw it
        for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
        {
            button = &m_button[i];
            if (button->state != button->statePrev)
            {
                r0 = DUI_STATUS_NEEDRAW;
                break;
            }
        }

        {
            T* pT = static_cast<T*>(this);
            r1 = pT->DoLButtonUp(uMsg, wParam, lParam, lpData);
        }

        if (DUI_STATUS_NODRAW != r0 || DUI_STATUS_NODRAW != r1)
        {
            m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window
            return DUI_STATUS_NEEDRAW;
        }

        return DUI_STATUS_NODRAW;
    }

    int DoLButtonDoubleClick(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int OnLButtonDoubleClick(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int ret = DUI_STATUS_NODRAW;
        T* pT = static_cast<T*>(this);

        ret = pT->DoLButtonDoubleClick(uMsg, wParam, lParam, lpData);

        if (DUI_STATUS_NODRAW != ret)
            m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window

        return ret;
    }

    int DoCreate(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0;  }
    int OnCreate(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int ret = DUI_STATUS_NODRAW;
        m_hWnd = (void*)wParam;

        assert(IsRealWindow(m_hWnd));

        T* pT = static_cast<T*>(this);
        ret = pT->DoCreate(uMsg, wParam, lParam, lpData);

        if (DUI_STATUS_NODRAW != ret)
            m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window

        return ret;
    }

    int DoDestroy(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int OnDestroy(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        T* pT = static_cast<T*>(this);
        int ret = pT->DoDestroy(uMsg, wParam, lParam, lpData);
        return ret;
    }

    int DoSetCursor(U32 uMsg, int xPos, int yPos, void* lpData = nullptr) { return 0; }
    int OnSetCursor(U32 uMsg, int xPos, int yPos, void* lpData = nullptr)
    {
        T* pT = static_cast<T*>(this);
        int ret = pT->DoSetCursor(uMsg, xPos, yPos, lpData);
        return ret;
    }

};

#endif  /* __DUI_WIN_H__ */

