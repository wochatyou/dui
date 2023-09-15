#ifndef __WOCHAT_XWINDOW_H__
#define __WOCHAT_XWINDOW_H__

#ifndef __cplusplus
#error Wochat requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
#error xwindow.h requires atlbase.h to be included first
#endif

#ifndef __ATLWIN_H__
#error xwindow.h requires atlwin.h to be included first
#endif

#include "dui/dui.h"

#include "window0.h"
#include "window1.h"
#include "window2.h"
#include "window3.h"
#include "window4.h"
#include "window5.h"

/************************************************************************************************
*  The layout of the Main Window
* 
* +------+------------------------+-------------------------------------------------------------+
* |      |         Win1           |                       Win3                                  |
* |      +------------------------+-------------------------------------------------------------+
* |      |                        |                                                             |
* |      |                        |                                                             |
* |      |                        |                       Win4                                  |
* | Win0 |                        |                                                             |
* |      |         Win2           |                                                             |
* |      |                        +-------------------------------------------------------------+
* |      |                        |                                                             |
* |      |                        |                       Win5                                  |
* |      |                        |                                                             |
* +------+------------------------+-------------------------------------------------------------+
*
* 
* We have one vertical bar and two horizonal bars.
*
*************************************************************************************************
*/

template <class T> 
void SafeRelease(T** ppT)
{
	if (nullptr != *ppT)
	{
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

// Splitter panes constants
#define SPLIT_PANE_LEFT			 0
#define SPLIT_PANE_RIGHT		 1
#define SPLIT_PANE_TOP			 SPLIT_PANE_LEFT
#define SPLIT_PANE_BOTTOM		 SPLIT_PANE_RIGHT
#define SPLIT_PANE_NONE			-1

// Splitter extended styles
#define SPLIT_PROPORTIONAL		0x00000001
#define SPLIT_NONINTERACTIVE	0x00000002
#define SPLIT_LEFTALIGNED		0x00000004
#define SPLIT_GRADIENTBAR		0x00000008
#define SPLIT_FLATBAR			0x00000020
#define SPLIT_FIXEDBARSIZE		0x00000010

UINT				g_DotsPerInch = 96;
ID2D1Factory*		g_pD2DFactory = nullptr;

class XWindow : public ATL::CWindowImpl<XWindow>
{
private:
	enum { 
		m_nPanesCount = 2, m_nPropMax = INT_MAX, m_cxyStep = 1, 
		SPLITLINE_WIDTH = 2, 
		INIT_VPOS = XWIN0_WIDTH + XWIN1_WIDTH
	};

	// the memory to hold the context to paint to the screen
	U32* m_screenBuff = nullptr;
	U32  m_screenSize = 0;

	RECT m_rectClient;
	int m_windowWidth = 0;
	int m_windowHeight = 0;

	int m_splitterVPos = -1;               // splitter bar position
	int m_splitterVPosNew = -1;            // internal - new position while moving
	int m_splitterVPosOld = -1;            // keep align value
	int m_splitterVPosToLeft  = 300;       // the minum pixel to the left of the client area.
	int m_splitterVPosToRight = 400;       // the minum pixel to the right of the client area.

	int m_marginLeft = 64;
	int m_marginRight = 0;

	int m_splitterHPos = -1;
	int m_splitterHPosNew = -1;
	int m_splitterHPosOld = -1;

	int m_splitterHPosfix0 = XWIN1_HEIGHT;
	int m_splitterHPosfix1 = XWIN3_HEIGHT;

	int m_nDefActivePane = SPLIT_PANE_NONE;
	int m_cxySplitBar = SPLITLINE_WIDTH;  // splitter bar width/height

	HCURSOR m_hCursorWE = nullptr;
	HCURSOR m_hCursorNS = nullptr;

	int m_cxyMin = 0;                   // minimum pane size
	int m_cxyBarEdge = 0;              	// splitter bar edge
	int m_cxyDragOffset = 0;		// internal
	int m_nProportionalPos = 0;
	bool m_bUpdateProportionalPos = true;
	DWORD m_dwExtendedStyle = SPLIT_LEFTALIGNED;    // splitter specific extended styles
	int m_nSinglePane = SPLIT_PANE_NONE;              // single pane mode
	int m_xySplitterDefPos = -1;            // default position
	bool m_bProportionalDefPos = false;     // porportinal def pos

	XWindow0 m_win0;
	XWindow1 m_win1;
	XWindow2 m_win2;
	XWindow3 m_win3;
	XWindow4 m_win4;
	XWindow5 m_win5;

	ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;
	ID2D1Bitmap*           m_pixelBitmap = nullptr;

public:
	DECLARE_XWND_CLASS(NULL, IDR_MAINFRAME, 0)

	XWindow() 
	{
		m_rectClient.left = m_rectClient.right = m_rectClient.top = m_rectClient.bottom = 0;

		m_win0.SetWindowId((const U8*)"DUIWin0", 7);
		m_win1.SetWindowId((const U8*)"DUIWin1", 7);
		m_win2.SetWindowId((const U8*)"DUIWin2", 7);
		m_win3.SetWindowId((const U8*)"DUIWin3", 7);
		m_win4.SetWindowId((const U8*)"DUIWin4", 7);
		m_win5.SetWindowId((const U8*)"DUIWin5", 7);
	}

	~XWindow() 
	{
		if (nullptr != m_screenBuff)
			VirtualFree(m_screenBuff, 0, MEM_RELEASE);

		m_screenBuff = nullptr;
		m_screenSize = 0;
	}

	bool IsInteractive() const
	{
		return ((m_dwExtendedStyle & SPLIT_NONINTERACTIVE) == 0);
	}

	BEGIN_MSG_MAP(XWindow)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDoubleClick)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		//MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0; // don't want flicker
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_win0.OnDestroy(uMsg, wParam, lParam);
		m_win1.OnDestroy(uMsg, wParam, lParam);
		m_win2.OnDestroy(uMsg, wParam, lParam);
		m_win3.OnDestroy(uMsg, wParam, lParam);
		m_win4.OnDestroy(uMsg, wParam, lParam);
		m_win5.OnDestroy(uMsg, wParam, lParam);

		SafeRelease(&m_pixelBitmap);
		SafeRelease(&m_pD2DRenderTarget);
		SafeRelease(&g_pD2DFactory);

		PostQuitMessage(0);

		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HRESULT hr;
		int r0, r1 = 0, r2 = 0, r3 = 0, r4 = 0, r5 = 0;

		m_hCursorWE = ::LoadCursor(NULL, IDC_SIZEWE);
		m_hCursorNS = ::LoadCursor(NULL, IDC_SIZENS);

		if(NULL == m_hCursorWE || NULL == m_hCursorNS)
		{
			MessageBox(_T("The calling of LoadCursor() is failed"), _T("DUI Error"), MB_OK);
			PostMessage(WM_CLOSE);
			return 0;
		}

		g_pD2DFactory = nullptr;
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
		if (S_OK != hr || nullptr == g_pD2DFactory)
		{
			MessageBox(_T("The calling of D2D1CreateFactory() is failed"), _T("DUI Error"), MB_OK);
			PostMessage(WM_CLOSE);
			return 0;
		}
		g_DotsPerInch = GetDpiForWindow(m_hWnd);
		
		r0 = m_win0.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		r1 = m_win1.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		r2 = m_win2.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		r3 = m_win3.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		r4 = m_win4.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		r5 = m_win5.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);

		if (0 != r0 || 0 != r1 || 0 != r2)
		{
			MessageBox(_T("WM_CREATE failed!"), _T("Error"), MB_OK);
			PostMessage(WM_CLOSE);
			return 0;
		}

		Init();

		return 0;
	}

	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

		lpMMI->ptMinTrackSize.x = 800;
		lpMMI->ptMinTrackSize.y = 600;

		return 0;
	}

	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(((HWND)wParam == m_hWnd) && (LOWORD(lParam) == HTCLIENT))
		{
			DWORD dwPos = ::GetMessagePos();
			POINT ptPos = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
			ScreenToClient(&ptPos);
			if(IsOverSplitterBar(ptPos.x, ptPos.y))
				return 1;
		}

		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		U16 W, H;

		if (SIZE_MINIMIZED == wParam)
			return 0;

		GetClientRect(&m_rectClient);
		ATLASSERT(0 == m_rectClient.left);
		ATLASSERT(0 == m_rectClient.top);
		ATLASSERT(m_rectClient.right > 0);
		ATLASSERT(m_rectClient.bottom > 0);

		m_windowWidth = m_rectClient.right - m_rectClient.left;
		m_windowHeight = m_rectClient.bottom - m_rectClient.top;

		W = (U16)(m_rectClient.right - m_rectClient.left);
		H = (U16)(m_rectClient.bottom - m_rectClient.top);

		m_screenBuff = nullptr;
		m_screenSize = DUI_ALIGN_PAGE(W * H * sizeof(U32));
		ATLASSERT(m_screenSize >= (W * H * sizeof(U32)));

		m_screenBuff = (U32*)VirtualAlloc(NULL, m_screenSize, MEM_COMMIT, PAGE_READWRITE);
		if (nullptr == m_screenBuff)
		{
			m_screenSize = 0;
			return 0;
		}

		if(m_splitterVPos < 0)
		{
			m_splitterVPos = INIT_VPOS;
			if (m_splitterVPos < m_splitterVPosToLeft)
				m_splitterVPos = m_splitterVPosToLeft;

			if (m_splitterVPos > (m_rectClient.right - m_rectClient.left - m_splitterVPosToRight))
			{
				m_splitterVPos = (m_rectClient.right - m_rectClient.left - m_splitterVPosToRight);
				ATLASSERT(m_splitterVPos > m_splitterVPosToLeft);
			}

			if(SPLIT_LEFTALIGNED & m_dwExtendedStyle)  // it is left aligned
			{
				m_splitterVPosOld = m_splitterVPos;
			}
			else  // it is right aligned
			{
				m_splitterVPosOld = (m_rectClient.right - m_rectClient.left) - m_splitterVPos;
			}
		}

		if(SPLIT_LEFTALIGNED & m_dwExtendedStyle)  // it is left aligned
		{
			m_splitterVPos = m_splitterVPosOld;
		}
		else  // it is right aligned
		{
			m_splitterVPos = (m_rectClient.right - m_rectClient.left) - m_splitterVPosOld;
		}

		ATLASSERT(m_splitterVPosToLeft >= 0);
		ATLASSERT(m_splitterVPosToRight >= 0);

		if(m_splitterVPos < m_splitterVPosToLeft)
			m_splitterVPos = m_splitterVPosToLeft;

		if(m_splitterVPos > (m_rectClient.right - m_rectClient.left - m_splitterVPosToRight))
		{
			m_splitterVPos = (m_rectClient.right - m_rectClient.left - m_splitterVPosToRight);
			ATLASSERT(m_splitterVPos > m_splitterVPosToLeft);
		}
		
		if (m_splitterHPos < 0)
		{
			m_splitterHPos = m_rectClient.bottom - XWIN5_HEIGHT;
		}

		SafeRelease(&m_pD2DRenderTarget);

		if (nullptr != m_screenBuff)
		{
			U32* dst = m_screenBuff;
			U32 size;
			RECT area;
			RECT* r =&area;

			r->left = m_rectClient.left;
			r->right = XWIN0_WIDTH;
			r->top = m_rectClient.top;
			r->bottom = m_rectClient.bottom;
			m_win0.OnSize(uMsg, wParam, (LPARAM)r, dst);
			size = (U32)((r->right - r->left) * (r->bottom - r->top));
			dst += size;

			r->left = XWIN0_WIDTH;
			r->right = m_splitterVPos;
			r->top = m_rectClient.top;
			r->bottom = m_splitterHPosfix0;
			m_win1.OnSize(uMsg, wParam, (LPARAM)r, dst);
			size = (U32)((r->right - r->left) * (r->bottom - r->top));
			dst += size;

			r->left = XWIN0_WIDTH;
			r->right = m_splitterVPos;
			r->top = m_splitterHPosfix0 + SPLITLINE_WIDTH;
			r->bottom = m_rectClient.bottom;
			m_win2.OnSize(uMsg, wParam, (LPARAM)r, dst);
			size = (U32)((r->right - r->left) * (r->bottom - r->top));
			dst += size;

			r->left = m_splitterVPos + SPLITLINE_WIDTH;
			r->right = m_rectClient.right;
			r->top = m_rectClient.top;
			r->bottom = m_splitterHPosfix1;
			m_win3.OnSize(uMsg, wParam, (LPARAM)r, dst);
			size = (U32)((r->right - r->left) * (r->bottom - r->top));
			dst += size;

			r->left = m_splitterVPos + SPLITLINE_WIDTH;
			r->right = m_rectClient.right;
			r->top = m_splitterHPosfix1 + SPLITLINE_WIDTH;
			r->bottom = m_splitterHPos;
			m_win4.OnSize(uMsg, wParam, (LPARAM)r, dst);
			size = (U32)((r->right - r->left) * (r->bottom - r->top));
			dst += size;

			r->left = m_splitterVPos + SPLITLINE_WIDTH;
			r->right = m_rectClient.right;
			r->top = m_splitterHPos + SPLITLINE_WIDTH;
			r->bottom = m_rectClient.bottom;
			m_win5.OnSize(uMsg, wParam, (LPARAM)r, dst);
		}

		Invalidate();

		return 0;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int needReDraw = 0;
		int needReDraw0 = 0;
		int needReDraw1 = 0;
		int needReDraw2 = 0;
		int needReDraw3 = 0;
		int needReDraw4 = 0;
		int needReDraw5 = 0;

		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if(::GetCapture() == m_hWnd)
		{
			int	newSplitPosV = xPos - m_cxyDragOffset;

			if(newSplitPosV == -1)   // avoid -1, that means default position
				newSplitPosV = -2;

			if(m_splitterVPos != newSplitPosV)
			{
				if (newSplitPosV >= m_splitterVPosToLeft && newSplitPosV < (m_rectClient.right - m_rectClient.left - SPLITLINE_WIDTH - m_splitterVPosToRight))
				{
					if (SetSplitterPos(newSplitPosV, true))
						needReDraw = 1;
				}
			}

			if(needReDraw);
			{
				// window 0 does not need to change
				RECT area;
				XRECT* xr = m_win0.GetWindowArea();

				area.left = xr->left;  area.top = xr->top; area.right = xr->right; area.bottom = xr->bottom;
				RECT* r = &area;

				U32* dst = m_screenBuff;
				U32 size = (U32)((r->right - r->left) * (r->bottom - r->top));
				dst += size;

				// windows 1
				r->left = r->right; r->right = m_splitterVPos; r->top = m_rectClient.top; r->bottom = m_splitterHPosfix0;
				size = (U32)((r->right - r->left) * (r->bottom - r->top));
				m_win1.SetPosition(r, dst, size);
				dst += size;

				// windows 2
				r->top = m_splitterHPosfix0 + SPLITLINE_WIDTH; r->bottom = m_rectClient.bottom;
				size = (U32)((r->right - r->left) * (r->bottom - r->top));
				m_win2.SetPosition(r, dst, size);

				// windows 3
				dst += size;
				r->left = m_splitterVPos + SPLITLINE_WIDTH; r->right = m_rectClient.right; r->top = m_rectClient.top; r->bottom = m_splitterHPosfix1;
				size = (U32)((r->right - r->left) * (r->bottom - r->top));
				m_win3.SetPosition(r, dst, size);

				// windows 4
				dst += size;
				r->top = m_splitterHPosfix1 + SPLITLINE_WIDTH; r->bottom = m_splitterHPos;
				size = (U32)((r->right - r->left) * (r->bottom - r->top));
				m_win4.SetPosition(r, dst, size);

				// windows 5
				dst += size;
				r->top = m_splitterHPos + SPLITLINE_WIDTH; r->bottom = m_rectClient.bottom;
				size = (U32)((r->right - r->left) * (r->bottom - r->top));
				m_win5.SetPosition(r, dst, size);
			}
		}
		else		// not dragging, just set cursor
		{
			if(IsOverSplitterBar(xPos, yPos))
			{
				ATLASSERT(nullptr != m_hCursorWE);
				::SetCursor(m_hCursorWE);
			}
		}

		needReDraw0 = m_win0.OnMouseMove(uMsg, wParam, lParam);
		needReDraw1 = m_win1.OnMouseMove(uMsg, wParam, lParam);
		needReDraw2 = m_win2.OnMouseMove(uMsg, wParam, lParam);
		needReDraw3 = m_win3.OnMouseMove(uMsg, wParam, lParam);
		needReDraw4 = m_win4.OnMouseMove(uMsg, wParam, lParam);
		needReDraw5 = m_win5.OnMouseMove(uMsg, wParam, lParam);

		if (needReDraw || needReDraw0 || needReDraw1 || needReDraw2 || needReDraw3 || needReDraw4 || needReDraw5)
			Invalidate();

		return 0;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int needReDraw0;
		int needReDraw1;
		int needReDraw2;
		int needReDraw3;
		int needReDraw4;
		int needReDraw5;

		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if((::GetCapture() != m_hWnd) && IsOverSplitterBar(xPos, yPos))
		{
			m_splitterVPosNew = m_splitterVPos;
			SetCapture();
			::SetCursor(m_hCursorWE);
			m_cxyDragOffset = xPos - m_splitterVPos;
		}
		else if((::GetCapture() == m_hWnd) && !IsOverSplitterBar(xPos, yPos))
		{
			::ReleaseCapture();
		}

		needReDraw0 = m_win0.OnLButtonDown(uMsg, wParam, lParam);
		needReDraw1 = m_win1.OnLButtonDown(uMsg, wParam, lParam);
		needReDraw2 = m_win2.OnLButtonDown(uMsg, wParam, lParam);
		needReDraw3 = m_win3.OnLButtonDown(uMsg, wParam, lParam);
		needReDraw4 = m_win4.OnLButtonDown(uMsg, wParam, lParam);
		needReDraw5 = m_win5.OnLButtonDown(uMsg, wParam, lParam);

		if (needReDraw0 || needReDraw1 || needReDraw2 || needReDraw3 || needReDraw4 || needReDraw5)
			Invalidate();

		return 0;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int needReDraw0;
		int needReDraw1;
		int needReDraw2;
		int needReDraw3;
		int needReDraw4;
		int needReDraw5;

		if(::GetCapture() == m_hWnd)
		{
			m_splitterVPosNew = m_splitterVPos;
			::ReleaseCapture();
		}

		needReDraw0 = m_win0.OnLButtonUp(uMsg, wParam, lParam);
		needReDraw1 = m_win1.OnLButtonUp(uMsg, wParam, lParam);
		needReDraw2 = m_win2.OnLButtonUp(uMsg, wParam, lParam);
		needReDraw3 = m_win3.OnLButtonUp(uMsg, wParam, lParam);
		needReDraw4 = m_win4.OnLButtonUp(uMsg, wParam, lParam);
		needReDraw5 = m_win5.OnLButtonUp(uMsg, wParam, lParam);

		if (needReDraw0 || needReDraw1 || needReDraw2 || needReDraw3 || needReDraw4 || needReDraw5)
			Invalidate();

		return 0;
	}

	LRESULT OnLButtonDoubleClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if((m_splitterVPosNew != -1) && (m_splitterVPos != m_splitterVPosNew))
		{
			m_splitterVPos = m_splitterVPosNew;
			m_splitterVPosNew = -1;
			if(SPLIT_LEFTALIGNED & m_dwExtendedStyle)  // it is left aligned
			{
				m_splitterVPosOld = m_splitterVPos;
			}
			else  // it is right aligned
			{
				m_splitterVPosOld = (m_rectClient.right - m_rectClient.left) - m_splitterVPos;
			}

			UpdateSplitterLayout();
			UpdateWindow();
		}

		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(::GetCapture() == m_hWnd)
		{
			switch(wParam)
			{
			case VK_RETURN:
				m_splitterVPosNew = m_splitterVPos;
				// FALLTHROUGH
			case VK_ESCAPE:
				::ReleaseCapture();
				break;
			case VK_LEFT:
			case VK_RIGHT:
				{
					POINT pt = {};
					::GetCursorPos(&pt);
					int xyPos = m_splitterVPos + ((wParam == VK_LEFT) ? -m_cxyStep : m_cxyStep);
					int cxyMax = m_rectClient.right - m_rectClient.left;

					ATLASSERT(m_splitterVPosToLeft > 100);
					ATLASSERT(m_splitterVPosToRight > 100);

					if (xyPos >= m_splitterVPosToLeft && xyPos < (cxyMax - SPLITLINE_WIDTH - m_splitterVPosToRight))
					{
						pt.x += xyPos - m_splitterVPos;
						::SetCursorPos(pt.x, pt.y);
					}
				}
				break;
			case VK_UP:
			case VK_DOWN:
#if 0
				if(!m_bVertical)
				{
					POINT pt = {};
					::GetCursorPos(&pt);
					int xyPos = m_splitterVPos + ((wParam == VK_UP) ? -m_cxyStep : m_cxyStep);
					int cxyMax = m_rectClient.bottom - m_rectClient.top;
					if(xyPos < (m_cxyMin + m_cxyBarEdge))
						xyPos = m_cxyMin;
					else if(xyPos > (cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin))
						xyPos = cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin;
					pt.y += xyPos - m_splitterVPos;
					::SetCursorPos(pt.x, pt.y);
				}
#endif
				break;
			default:
				break;
			}
		}
		else
		{
			bHandled = FALSE;
		}

		return 0;
	}

	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM, BOOL& bHandled)
	{
		if(::GetCapture() != m_hWnd)
		{
			if(m_nSinglePane == SPLIT_PANE_NONE)
			{
				if((m_nDefActivePane == SPLIT_PANE_LEFT) || (m_nDefActivePane == SPLIT_PANE_RIGHT))
				{
					//::SetFocus(m_hWndPane[m_nDefActivePane]);
				}
			}
			else
			{
				//::SetFocus(m_hWndPane[m_nSinglePane]);
			}
		}

		return 0;
	}

	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
		if((lRet == MA_ACTIVATE) || (lRet == MA_ACTIVATEANDEAT))
		{
			DWORD dwPos = ::GetMessagePos();
			POINT pt = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
			ScreenToClient(&pt);
			RECT rcPane = {};
			for(int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if(GetSplitterPaneRect(nPane, &rcPane) && (::PtInRect(&rcPane, pt) != FALSE))
				{
					m_nDefActivePane = nPane;
					break;
				}
			}
		}

		return 0;
	}

	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		GetSystemSettings(true);

		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		HRESULT hr = S_OK;
		PAINTSTRUCT ps;
		BeginPaint(&ps);

		if (nullptr == m_pD2DRenderTarget)
		{
			D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);

			D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
			renderTargetProperties.dpiX = g_DotsPerInch;
			renderTargetProperties.dpiY = g_DotsPerInch;
			renderTargetProperties.pixelFormat = pixelFormat;

			D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRenderTragetproperties
				= D2D1::HwndRenderTargetProperties(m_hWnd, D2D1::SizeU(m_rectClient.right - m_rectClient.left, m_rectClient.bottom - m_rectClient.top));

			hr = g_pD2DFactory->CreateHwndRenderTarget(renderTargetProperties, hwndRenderTragetproperties, &m_pD2DRenderTarget);
			if (S_OK == hr && nullptr != m_pD2DRenderTarget)
			{
				U32 pixel[1] = { 0xFFEEEEEE };
				SafeRelease(&m_pixelBitmap);
				hr = m_pD2DRenderTarget->CreateBitmap(
					D2D1::SizeU(1, 1), pixel, 4, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
					&m_pixelBitmap);
			}
		}

		if (S_OK == hr && nullptr != m_pD2DRenderTarget && nullptr != m_pixelBitmap)
		{
			int w, h;
			U32* src = nullptr;
			RECT area;
			RECT* r = &area;

			m_pD2DRenderTarget->BeginDraw();
			////////////////////////////////////////////////////////////////////////////////////////////////////
			//m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			//m_pD2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			// draw seperator lines
			if (nullptr != m_pixelBitmap)
			{
				if(m_splitterVPos > 0)
				{
					D2D1_RECT_F rect = D2D1::RectF(
						static_cast<FLOAT>(m_splitterVPos),
						static_cast<FLOAT>(m_rectClient.top),
						static_cast<FLOAT>(m_splitterVPos + SPLITLINE_WIDTH), // m_cxySplitBar),
						static_cast<FLOAT>(m_rectClient.bottom)
					);
					m_pD2DRenderTarget->DrawBitmap(m_pixelBitmap, &rect);
				}
				if (m_splitterHPosfix0 > 0)
				{
					D2D1_RECT_F rect = D2D1::RectF(
						static_cast<FLOAT>(XWIN0_WIDTH),
						static_cast<FLOAT>(m_splitterHPosfix0),
						static_cast<FLOAT>(m_splitterVPos),
						static_cast<FLOAT>(m_splitterHPosfix0 + SPLITLINE_WIDTH)
					);
					m_pD2DRenderTarget->DrawBitmap(m_pixelBitmap, &rect);
				}

				if (m_splitterHPosfix1 > 0)
				{
					D2D1_RECT_F rect = D2D1::RectF(
						static_cast<FLOAT>(m_splitterVPos + SPLITLINE_WIDTH),
						static_cast<FLOAT>(m_splitterHPosfix1),
						static_cast<FLOAT>(m_rectClient.right),
						static_cast<FLOAT>(m_splitterHPosfix1 + SPLITLINE_WIDTH)
					);
					m_pD2DRenderTarget->DrawBitmap(m_pixelBitmap, &rect);
				}

				if (m_splitterHPos > 0)
				{
					D2D1_RECT_F rect = D2D1::RectF(
						static_cast<FLOAT>(m_splitterVPos + SPLITLINE_WIDTH),
						static_cast<FLOAT>(m_splitterHPos),
						static_cast<FLOAT>(m_rectClient.right),
						static_cast<FLOAT>(m_splitterHPos + SPLITLINE_WIDTH)
					);
					m_pD2DRenderTarget->DrawBitmap(m_pixelBitmap, &rect);
				}
			}

			// draw window 0
			src = m_win0.Render();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				XRECT* xr = m_win0.GetWindowArea();
				w = xr->right - xr->left; h = xr->bottom - xr->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(xr->left), static_cast<FLOAT>(xr->top), static_cast<FLOAT>(xr->right), static_cast<FLOAT>(xr->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			// draw window 1
			src = m_win1.Render();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				XRECT* xr = m_win1.GetWindowArea();
				w = xr->right - xr->left; h = xr->bottom - xr->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(xr->left), static_cast<FLOAT>(xr->top), static_cast<FLOAT>(xr->right), static_cast<FLOAT>(xr->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			// draw window 2
			src = m_win2.Render();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				XRECT* xr = m_win2.GetWindowArea();
				w = xr->right - xr->left; h = xr->bottom - xr->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(xr->left), static_cast<FLOAT>(xr->top), static_cast<FLOAT>(xr->right), static_cast<FLOAT>(xr->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			// draw window 3
			src = m_win3.Render();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				XRECT* xr = m_win3.GetWindowArea();
				w = xr->right - xr->left; h = xr->bottom - xr->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(xr->left), static_cast<FLOAT>(xr->top), static_cast<FLOAT>(xr->right), static_cast<FLOAT>(xr->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			// draw window 4
			src = m_win4.Render();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				XRECT* xr = m_win4.GetWindowArea();
				w = xr->right - xr->left; h = xr->bottom - xr->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(xr->left), static_cast<FLOAT>(xr->top), static_cast<FLOAT>(xr->right), static_cast<FLOAT>(xr->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			// draw window 5
			src = m_win5.Render();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				XRECT* xr = m_win5.GetWindowArea();
				w = xr->right - xr->left; h = xr->bottom - xr->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(xr->left), static_cast<FLOAT>(xr->top), static_cast<FLOAT>(xr->right), static_cast<FLOAT>(xr->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			hr = m_pD2DRenderTarget->EndDraw();
			if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
			{
				SafeRelease(&m_pD2DRenderTarget);
			}
		}

		EndPaint(&ps);
		return 0;
	}


	void GetSplitterRect(LPRECT lpRect) const
	{
		ATLASSERT(lpRect != NULL);
		*lpRect = m_rectClient;
	}

	bool SetSplitterPos(int xyPos = -1, bool bUpdate = true)
	{
		int cxyMax = 0;
		bool bRet;

		if(xyPos == -1)   // -1 == default position
		{
			if(m_bProportionalDefPos)
			{
				ATLASSERT((m_xySplitterDefPos >= 0) && (m_xySplitterDefPos <= m_nPropMax));
				xyPos = ::MulDiv(m_xySplitterDefPos, m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge, m_nPropMax);
			}
			else if(m_xySplitterDefPos != -1)
			{
				xyPos = m_xySplitterDefPos;
			}
			else   // not set, use middle position
			{
				xyPos = (m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge) / 2;
			}
		}

		// Adjust if out of valid range
		cxyMax = m_rectClient.right - m_rectClient.left;

		if(xyPos < m_cxyMin + m_cxyBarEdge)
			xyPos = m_cxyMin;
		else if(xyPos > (cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin))
			xyPos = cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin;

		// Set new position and update if requested
		bRet = (m_splitterVPos != xyPos);
		m_splitterVPos = xyPos;

		if(m_splitterVPos < m_splitterVPosToLeft)
			m_splitterVPos = m_splitterVPosToLeft;

		if(m_splitterVPos > (m_rectClient.right - m_rectClient.left - m_splitterVPosToRight))
		{
			m_splitterVPos = (m_rectClient.right - m_rectClient.left - m_splitterVPosToRight);
			ATLASSERT(m_splitterVPos > m_splitterVPosToLeft);
		}

		if(SPLIT_LEFTALIGNED & m_dwExtendedStyle)  // it is left aligned
		{
			m_splitterVPosOld = m_splitterVPos;
		}
		else  // it is right aligned
		{
			m_splitterVPosOld = (m_rectClient.right - m_rectClient.left) - m_splitterVPos;
		}


		if(m_bUpdateProportionalPos)
		{
			if(IsRightAligned())
				StoreRightAlignPos();
		}
		else
		{
			m_bUpdateProportionalPos = true;
		}

		if(bUpdate && bRet)
			UpdateSplitterLayout();

		return bRet;
	}

	int GetSplitterPos() const
	{
		return m_splitterVPos;
	}

	DWORD GetSplitterExtendedStyle() const
	{
		return m_dwExtendedStyle;
	}

	DWORD SetSplitterExtendedStyle(DWORD dwExtendedStyle, DWORD dwMask = 0)
	{
		DWORD dwPrevStyle = m_dwExtendedStyle;
		if(dwMask == 0)
			m_dwExtendedStyle = dwExtendedStyle;
		else
			m_dwExtendedStyle = (m_dwExtendedStyle & ~dwMask) | (dwExtendedStyle & dwMask);

#ifdef _DEBUG
		if(IsProportional() && IsRightAligned())
		{
			ATLASSERT(FALSE);
			//ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::SetSplitterExtendedStyle - SPLIT_PROPORTIONAL and SPLIT_LEFTALIGNED are mutually exclusive, defaulting to SPLIT_PROPORTIONAL.\n"));
		}
#endif // _DEBUG

		return dwPrevStyle;
	}

// Implementation - internal helpers
	void Init()
	{
		GetSystemSettings(false);
	}

	void UpdateSplitterLayout()
	{
		if((m_nSinglePane == SPLIT_PANE_NONE) && (m_splitterVPos == -1))
			return;

		RECT rect = {};
		if(m_nSinglePane == SPLIT_PANE_NONE)
		{
			if(GetSplitterBarRect(&rect))
				InvalidateRect(&rect);

			for(int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if(GetSplitterPaneRect(nPane, &rect))
				{
					InvalidateRect(&rect);
				}
			}
		}
		else
		{
			if(GetSplitterPaneRect(m_nSinglePane, &rect))
			{
				InvalidateRect(&rect);
			}
		}
	}

	bool GetSplitterBarRect(LPRECT lpRect) const
	{
		ATLASSERT(lpRect != NULL);
		if((m_nSinglePane != SPLIT_PANE_NONE) || (m_splitterVPos == -1))
			return false;

		lpRect->left = m_rectClient.left + m_splitterVPos;
		lpRect->top = m_rectClient.top;
		lpRect->right = m_rectClient.left + m_splitterVPos + m_cxySplitBar + m_cxyBarEdge;
		lpRect->bottom = m_rectClient.bottom;

		return true;
	}

	bool GetSplitterPaneRect(int nPane, LPRECT lpRect) const
	{
		ATLASSERT((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT));
		ATLASSERT(lpRect != NULL);
		bool bRet = true;
		if(m_nSinglePane != SPLIT_PANE_NONE)
		{
			if(nPane == m_nSinglePane)
				*lpRect = m_rectClient;
			else
				bRet = false;
		}
		else if(nPane == SPLIT_PANE_LEFT)
		{
			lpRect->left = m_rectClient.left;
			lpRect->top = m_rectClient.top;
			lpRect->right = m_rectClient.left + m_splitterVPos;
			lpRect->bottom = m_rectClient.bottom;
		}
		else if(nPane == SPLIT_PANE_RIGHT)
		{
			lpRect->left = m_rectClient.left + m_splitterVPos + m_cxySplitBar + m_cxyBarEdge;
			lpRect->top = m_rectClient.top;
			lpRect->right = m_rectClient.right;
			lpRect->bottom = m_rectClient.bottom;
		}
		else
		{
			bRet = false;
		}

		return bRet;
	}

	bool IsOverSplitterRect(int x, int y) const
	{
		// -1 == don't check
		return (((x == -1) || ((x >= m_rectClient.left) && (x < m_rectClient.right))) &&
			((y == -1) || ((y >= m_rectClient.top) && (y < m_rectClient.bottom))));
	}

	bool IsOverSplitterBar(int x, int y) const
	{
		if((m_splitterVPos == -1) || !IsOverSplitterRect(x, y))
			return false;

		return ((x >= m_splitterVPos) && (x < (m_splitterVPos + SPLITLINE_WIDTH)));
	}

	void GetSystemSettings(bool bUpdate)
	{
		if((m_dwExtendedStyle & SPLIT_FIXEDBARSIZE) == 0)
		{
			m_cxySplitBar = ::GetSystemMetrics(SM_CXSIZEFRAME);
		}

		if((GetExStyle() & WS_EX_CLIENTEDGE) != 0)
		{
			m_cxyBarEdge = 2 * ::GetSystemMetrics(SM_CXEDGE);
			m_cxyMin = 0;
		}
		else
		{
			m_cxyBarEdge = 0;
			m_cxyMin = 2 * ::GetSystemMetrics(SM_CXEDGE);
		}

		if(bUpdate)
			UpdateSplitterLayout();
	}

	bool IsProportional() const
	{
		return ((m_dwExtendedStyle & SPLIT_PROPORTIONAL) != 0);
	}

	void StoreProportionalPos()
	{
		int cxyTotal = (m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge);
		if(cxyTotal > 0)
			m_nProportionalPos = ::MulDiv(m_splitterVPos, m_nPropMax, cxyTotal);
		else
			m_nProportionalPos = 0;
		//ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::StoreProportionalPos - %i\n"), m_nProportionalPos);
	}

	bool IsRightAligned() const
	{
		return ((m_dwExtendedStyle & SPLIT_LEFTALIGNED) != 0);
	}

	void StoreRightAlignPos()
	{
		int cxyTotal = (m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge);
		if(cxyTotal > 0)
			m_nProportionalPos = cxyTotal - m_splitterVPos;
		else
			m_nProportionalPos = 0;
		//ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::StoreRightAlignPos - %i\n"), m_nProportionalPos);
	}
};

#endif /* __WOCHAT_XWINDOW_H__ */