#ifndef __DUI_WIN_H__
#define __DUI_WIN_H__

#include "dui.h"

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
	int	  w;
	int	  h;
} XBitmap;

typedef struct XButton
{
	U8       id        : 6;
	U8       property  : 2;
	U8       state     : 4;
	U8		 statePrev : 4;
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

enum XButtonProperty
{
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

// determin if one object is hitted
#define XWinPointInRect(x, y, OBJ)		(((x) >= ((OBJ)->left)) && ((x) < ((OBJ)->right)) && ((y) >= ((OBJ)->top)) && ((y)<((OBJ)->bottom)))

enum
{
	DUI_STATUS_NODRAW    = 0x00,	// do not need to draw      
	DUI_STATUS_VISIBLE   = 0x01,	// is this virtual window visible?
	DUI_STATUS_VSCROLL   = 0x02,	// is this virtual window has vertical scroll bar?
	DUI_STATUS_HSCROLL   = 0x04,	// is this virtual window has horizonal scroll bar?
	DUI_STATUS_ISFOCUS   = 0x08,	// is the input redirected into this virutal window?
	DUI_STATUS_NEEDRAW   = 0x10,	// does this virtual windows need to be redraw?
	DUI_STATUS_HASCARET  = 0x20,	// is this virutal window has a caret?
};

enum
{
	DUI_PROP_NONE		  = 0x00,   // None Properties
	DUI_PROP_MOVEWIN	  = 0x01,   // Move the whole window while LButton is pressed
	DUI_PROP_BTNACTIVE	  = 0x02,   // no active button on this virutal window
	DUI_PROP_HASVSCROLL  = 0x04,    // have vertical scroll bar
	DUI_PROP_HASHSCROLL  = 0x08   
};

enum
{
	DEFAULT_BORDER_COLOR		= 0xFFBBBBBB,
	DEFAULT_SCROLLBKG_COLOR		= 0xFFF9F3F1,
	DEFAULT_SCROLLTHUMB_COLOR   = 0xFFB9B4B2
};


template <class T>
class DUI_NO_VTABLE XWindowT
{
public:
	void*	m_hWnd = nullptr;
	U32*    m_screen = nullptr;
	U32		m_size = 0;
	U8      m_Id[8] = { 0 }; // for debugging 
	XRECT   m_area = { 0 };

	int     m_buttonStartIdx = 0;
	int     m_buttonEndIdx = -1;
	int     m_buttonActiveIdx = -1;

	int     m_scrollWidth = 8; // in pixel

	XPOINT m_ptOffset = { 0 };
	XSIZE  m_sizeAll = { 0 };
	XSIZE  m_sizeLine = { 0 };
	XSIZE  m_sizePage = { 0 };

	bool   m_InDragMode = false;

#ifdef _WIN32
	HCURSOR  m_cursorHand = nullptr;
#else
	void*  m_cursorHand = nullptr;
#endif
	U32	     m_message = 0;
	U32      m_backgroundColor = DEFAULT_BACKGROUND_COLOR;
	U32      m_scrollbarColor = DEFAULT_SCROLLBKG_COLOR;
	U32      m_thumbColor = DEFAULT_SCROLLTHUMB_COLOR;

	enum {
		MAX_BUTTONS = 16,
		MAX_BUTTON_BITMAPS = (MAX_BUTTONS << 2)
	};

	XButton	 m_button[MAX_BUTTONS];
	XBitmap	 m_bitmap[MAX_BUTTON_BITMAPS];

	U8	m_status = DUI_STATUS_VISIBLE;
	U8  m_property = DUI_PROP_NONE;

public:
	XWindowT()
	{
		U8 id;
		XButton* button;
		XBitmap* bmp;
		m_cursorHand = ::LoadCursor(nullptr, IDC_HAND);
		assert(NULL != m_cursorHand);

		static_assert(MAX_BUTTONS < (1 << 6));
		
		// initialize the button's status
		for (id = 0; id < MAX_BUTTONS; id++)
		{
			button = &m_button[id];
			button->id = id;
			button->property = 0;
			button->statePrev = button->state = XBUTTON_STATE_NORMAL;
			button->left = button->right = button->top = button->bottom = 0;
			button->imgNormal = button->imgHover = button->imgPress = button->imgActive = nullptr;
			button->pfAction = nullptr;
		}

		// initialize the bitmap's status
		for (id = 0; id < MAX_BUTTON_BITMAPS; id++)
		{
			bmp = &m_bitmap[id];
			bmp->id = id;
			bmp->data = nullptr;
			bmp->w = bmp->h = 0;
		}
	}

	~XWindowT() 
	{
	}

	bool IsRealWindow(void* hwnd)
	{
#ifdef _WIN32
		return (::IsWindow((HWND)hwnd));
#else
		return false;
#endif
	}
	void SetWindowId(const U8* id, U8 bytes)
	{
		if (bytes > 7)
			bytes = 7;

		for(U8 i=0; i<bytes; i++)
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

#if 0
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
#endif

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

	void DrawButton(U32* dst, int w, int h, XButton* button)
	{
		U32* src;
		int dx = button->left;
		int dy = button->top;

		if (XBUTTON_STATE_HIDDEN != button->state) // this button is visible
		{
			assert(nullptr != dst);
			assert(nullptr != button);

			XBitmap* bitmap = nullptr;
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
				bitmap = button->imgNormal;
				break;
			}

			assert(nullptr != bitmap);
			src = bitmap->data;

			assert(nullptr != src);
			if (XBUTTON_PROP_ROUND & button->property)
			{
				ScreenDrawRectRound(dst, w, h, src, bitmap->w, bitmap->h, dx, dy, m_backgroundColor);
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
			m_area.left = r->left; 
			m_area.top = r->top; 
			m_area.right = r->right; 
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

				// Draw the vertical scroll bar
				ScreenFillRect(m_screen, w, h, m_scrollbarColor, m_scrollWidth, h, w - m_scrollWidth, 0);
				ScreenFillRectRound(m_screen, w, h, m_thumbColor, thumb_width, thumb_height, w - m_scrollWidth + 1, thumb_start
					,0xFFD6D3D2
					,m_scrollbarColor);
			}

			for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
			{
				button = &m_button[i];
				DrawButton(m_screen, w, h, button);
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
		RECT* r = (RECT*)lParam;
		if (nullptr != r)
		{
			m_area.left = r->left;
			m_area.top = r->top;
			m_area.right = r->right;
			m_area.bottom = r->bottom;
			m_size = (U32)((r->right - r->left) * (r->bottom - r->top));
		}
		else
		{
			m_area.left = m_area.top = m_area.right = m_area.bottom = 0;
			m_size = 0;
		}
		m_screen = (U32*)lpData;

		if (nullptr != r)
		{
			T* pT = static_cast<T*>(this);
			pT->DoSize(uMsg, wParam, lParam, lpData);
		}
		m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window

		return DUI_STATUS_NEEDRAW;
	}

	int DoMouseMove(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
	int OnMouseMove(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		int r0  = DUI_STATUS_NODRAW;
		int r1  = DUI_STATUS_NODRAW;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		XButton* button;
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
			int hit = -1;  // no hit so far
			// handle the vertical bar
			if (DUI_PROP_HASVSCROLL & m_property)
			{
				U8 status = m_status;
				m_status &= (~DUI_STATUS_VSCROLL);

				if (xPos >= (m_area.right - m_scrollWidth))
				{
					if (m_sizeAll.cy > h)
					{
						int thumb_start = (m_ptOffset.y * h) / m_sizeAll.cy;
						int thumb_height = (h * h) / m_sizeAll.cy;
						m_status |= DUI_STATUS_VSCROLL;
						if (m_InDragMode)
						{
							int y = yPos -= m_area.top;
							int offset = y - thumb_start;
						}
					}
				}
				if ((DUI_STATUS_VSCROLL & status) != (DUI_STATUS_VSCROLL & m_status))
					r0 = DUI_STATUS_NEEDRAW;
			}

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
		else
		{
			// handle the vertical bar
			if (DUI_STATUS_VSCROLL & m_status)
			{
				m_status &= (~DUI_STATUS_VSCROLL);
				r0 = DUI_STATUS_NEEDRAW;
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
			r1 = pT->DoMouseMove(uMsg, wParam, lParam, lpData);
		}

		if (DUI_STATUS_NODRAW != r0 || DUI_STATUS_NODRAW != r1)
		{
			m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window
			return DUI_STATUS_NEEDRAW;
		}
		return DUI_STATUS_NODRAW;
	}

	int DoLButtonDown(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
	int OnLButtonDown(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		int r0 = DUI_STATUS_NODRAW;
		int r1 = DUI_STATUS_NODRAW;

		XButton* button;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		m_InDragMode = false;

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

			// handle the vertical bar
			if (DUI_PROP_HASVSCROLL & m_property)
			{
				int thumb_start, thumb_height;
				U8 status = m_status;
				m_status &= (~DUI_STATUS_VSCROLL);

				if (xPos >= (m_area.right - m_scrollWidth))
				{
					if (m_sizeAll.cy > h)
					{
						m_status |= DUI_STATUS_VSCROLL;
						thumb_start = (m_ptOffset.y * h) / m_sizeAll.cy;
						thumb_height = (h * h) / m_sizeAll.cy;
						if (yPos < (m_area.top + thumb_start))
						{
							assert(m_sizeLine.cy > 0);
							m_ptOffset.y -= m_sizeLine.cy;
							r0 = DUI_STATUS_NEEDRAW;
						} 
						else if (yPos > (m_area.top + thumb_start + thumb_height))
						{
							assert(m_sizeLine.cy > 0);
							m_ptOffset.y += m_sizeLine.cy;
							r0 = DUI_STATUS_NEEDRAW;
						}
						else // we hit the thumb
						{
							m_InDragMode = true;
						}

						if (m_ptOffset.y < 0)
							m_ptOffset.y = 0;
						if (m_ptOffset.y > (m_sizeAll.cy - h))
							m_ptOffset.y = m_sizeAll.cy - h;

						m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window
						return DUI_STATUS_NEEDRAW;
					}

				}
				if ((DUI_STATUS_VSCROLL & status) != (DUI_STATUS_VSCROLL & m_status))
					r0 = DUI_STATUS_NEEDRAW;
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
			{	// if the mouse does not hit the button, we can move the whole real window
				if (DUI_PROP_MOVEWIN & m_property)
					PostWindowMessage(WM_NCLBUTTONDOWN, HTCAPTION, lParam);
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
			r1 = pT->DoLButtonDown(uMsg, wParam, lParam, lpData);
		}

		if (DUI_STATUS_NODRAW != r0 || DUI_STATUS_NODRAW != r1)
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

		m_InDragMode = false;

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

};

#endif  /* __DUI_WIN_H__ */

