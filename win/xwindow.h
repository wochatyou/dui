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

#if 0
#include "windows0.h"
#include "windows1.h"
#include "windows2.h"
#include "windows3.h"
#include "windows4.h"
#include "windows5.h"
#endif

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

// Note: SPLIT_PROPORTIONAL and SPLIT_LEFTALIGNED/SPLIT_BOTTOMALIGNED are 
// mutually exclusive. If both are set, splitter defaults to SPLIT_PROPORTIONAL.
// Also, SPLIT_FLATBAR overrides SPLIT_GRADIENTBAR if both are set.

UINT				g_DotsPerInch = 96;
ID2D1Factory*		g_pD2DFactory = nullptr;

class XWindow : public ATL::CWindowImpl<XWindow>
{
private:
	enum { m_nPanesCount = 2, m_nPropMax = INT_MAX, m_cxyStep = 1, SPLITLINE_WIDTH = 2 };

	bool m_bVertical = true;
	RECT m_rectClient;
	int m_splitterVPos = -1;               // splitter bar position
	int m_splitterVPosNew = -1;            // internal - new position while moving
	int m_splitterVPosOld = -1;            // keep align value
	int m_splitterVPosToLeft  = 100;       // the minum pixel to the left of the client area.
	int m_splitterVPosToRight = 100;       // the minum pixel to the right of the client area.


	int m_nDefActivePane = SPLIT_PANE_NONE;
	int m_cxySplitBar = SPLITLINE_WIDTH;  // splitter bar width/height

	HCURSOR m_hCursorWE = nullptr;
	HCURSOR m_hCursorNS = nullptr;

	int m_cxyMin = 0;                   // minimum pane size
	int m_cxyBarEdge = 0;              	// splitter bar edge
	bool m_bFullDrag = true;
	int m_cxyDragOffset = 0;		// internal
	int m_nProportionalPos = 0;
	bool m_bUpdateProportionalPos = true;
	DWORD m_dwExtendedStyle = SPLIT_LEFTALIGNED;    // splitter specific extended styles
	int m_nSinglePane = SPLIT_PANE_NONE;              // single pane mode
	int m_xySplitterDefPos = -1;            // default position
	bool m_bProportionalDefPos = false;     // porportinal def pos

	ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;
	ID2D1Bitmap*           m_pixelBitmap = nullptr;

	int m_splitterPos[32];
	U32 m_splitterOrientation; // 0 : vertical / 1: horizonal

public:
	DECLARE_XWND_CLASS(NULL, IDR_MAINFRAME, 0)

	XWindow() 
	{
		m_rectClient.left = m_rectClient.right = m_rectClient.top = m_rectClient.bottom = 0;
	}

	~XWindow() {}

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

		SafeRelease(&m_pixelBitmap);
		SafeRelease(&m_pD2DRenderTarget);
		SafeRelease(&g_pD2DFactory);

		PostQuitMessage(0);

		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HRESULT hr;

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
		if (SIZE_MINIMIZED == wParam)
			return 0;

		GetClientRect(&m_rectClient);
		ATLASSERT(0 == m_rectClient.left);
		ATLASSERT(0 == m_rectClient.top);
		ATLASSERT(m_rectClient.right > 0);
		ATLASSERT(m_rectClient.bottom > 0);

		if(m_splitterVPos < 0)
		{
			m_splitterVPos = (m_rectClient.right - m_rectClient.left) >> 1;
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
		
		SafeRelease(&m_pD2DRenderTarget);

		Invalidate();

		return 0;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if(::GetCapture() == m_hWnd)
		{
			int	newSplitPosV = xPos - m_cxyDragOffset;

			if(newSplitPosV == -1)   // avoid -1, that means default position
				newSplitPosV = -2;

			if(m_splitterVPos != newSplitPosV)
			{
				if(m_bFullDrag)
				{
					if(SetSplitterPos(newSplitPosV, true))
						UpdateWindow();
				}
				else
				{
					DrawGhostBar();
					SetSplitterPos(newSplitPosV, false);
					DrawGhostBar();
				}
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

		return 0;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if((::GetCapture() != m_hWnd) && IsOverSplitterBar(xPos, yPos))
		{
			m_splitterVPosNew = m_splitterVPos;
			SetCapture();
			::SetCursor(m_hCursorWE);
			if(!m_bFullDrag)
				DrawGhostBar();
			m_cxyDragOffset = xPos - m_splitterVPos;
		}
		else if((::GetCapture() == m_hWnd) && !IsOverSplitterBar(xPos, yPos))
		{
			::ReleaseCapture();
		}

		return 0;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(::GetCapture() == m_hWnd)
		{
			m_splitterVPosNew = m_splitterVPos;
			::ReleaseCapture();
		}

		return 0;
	}

	LRESULT OnLButtonDoubleClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		SetSplitterPos();   // default

		return 0;
	}

	LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(!m_bFullDrag)
			DrawGhostBar();

		if((m_splitterVPosNew != -1) && (!m_bFullDrag || (m_splitterVPos != m_splitterVPosNew)))
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
				if(m_bVertical)
				{
					POINT pt = {};
					::GetCursorPos(&pt);
					int xyPos = m_splitterVPos + ((wParam == VK_LEFT) ? -m_cxyStep : m_cxyStep);
					int cxyMax = m_rectClient.right - m_rectClient.left;
					if(xyPos < (m_cxyMin + m_cxyBarEdge))
						xyPos = m_cxyMin;
					else if(xyPos > (cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin))
						xyPos = cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin;
					pt.x += xyPos - m_splitterVPos;
					::SetCursorPos(pt.x, pt.y);
				}
				break;
			case VK_UP:
			case VK_DOWN:
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
				U32 pixel[1] = { 0xFF000000 };
				SafeRelease(&m_pixelBitmap);
				hr = m_pD2DRenderTarget->CreateBitmap(
					D2D1::SizeU(1, 1), pixel, 4, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
					&m_pixelBitmap);
			}
		}

		if (S_OK == hr && nullptr != m_pD2DRenderTarget && nullptr != m_pixelBitmap)
		{
			m_pD2DRenderTarget->BeginDraw();
			////////////////////////////////////////////////////////////////////////////////////////////////////
			m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pD2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

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

				if(m_bVertical)
					xyPos = ::MulDiv(m_xySplitterDefPos, m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge, m_nPropMax);
				else
					xyPos = ::MulDiv(m_xySplitterDefPos, m_rectClient.bottom - m_rectClient.top - m_cxySplitBar - m_cxyBarEdge, m_nPropMax);
			}
			else if(m_xySplitterDefPos != -1)
			{
				xyPos = m_xySplitterDefPos;
			}
			else   // not set, use middle position
			{
				if(m_bVertical)
					xyPos = (m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge) / 2;
				else
					xyPos = (m_rectClient.bottom - m_rectClient.top - m_cxySplitBar - m_cxyBarEdge) / 2;
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
#if 0
		if(m_splitterVPos < m_splitterVPosToLeft)
			m_splitterVPos = m_splitterVPosToLeft;

		if(m_splitterVPos > (m_rectClient.right - m_rectClient.left - m_splitterVPosToRight))
		{
			m_splitterVPos = (m_rectClient.right - m_rectClient.left - m_splitterVPosToRight);
			ATLASSERT(m_splitterVPos > m_splitterVPosToLeft);
		}
#endif
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
			if(IsProportional())
				StoreProportionalPos();
			else if(IsRightAligned())
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

	void SetSplitterPosPct(int nPct, bool bUpdate = true)
	{
		ATLASSERT((nPct >= 0) && (nPct <= 100));

		m_nProportionalPos = ::MulDiv(nPct, m_nPropMax, 100);
		UpdateProportionalPos();

		if(bUpdate)
			UpdateSplitterLayout();
	}

	int GetSplitterPosPct() const
	{
		int cxyTotal = m_bVertical ? (m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge) : (m_rectClient.bottom - m_rectClient.top - m_cxySplitBar - m_cxyBarEdge);
		return ((cxyTotal > 0) && (m_splitterVPos >= 0)) ? ::MulDiv(m_splitterVPos, 100, cxyTotal) : -1;
	}

	bool SetSinglePaneMode(int nPane = SPLIT_PANE_NONE)
	{
		ATLASSERT((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT) || (nPane == SPLIT_PANE_NONE));
		if(!((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT) || (nPane == SPLIT_PANE_NONE)))
			return false;

		if(nPane != SPLIT_PANE_NONE)
		{
//			if(::IsWindowVisible(m_hWndPane[nPane]) == FALSE)
//				::ShowWindow(m_hWndPane[nPane], SW_SHOW);
			int nOtherPane = (nPane == SPLIT_PANE_LEFT) ? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT;
//			::ShowWindow(m_hWndPane[nOtherPane], SW_HIDE);
			if(m_nDefActivePane != nPane)
				m_nDefActivePane = nPane;
		}
		else if(m_nSinglePane != SPLIT_PANE_NONE)
		{
			int nOtherPane = (m_nSinglePane == SPLIT_PANE_LEFT) ? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT;
//			::ShowWindow(m_hWndPane[nOtherPane], SW_SHOW);
		}

		m_nSinglePane = nPane;
		UpdateSplitterLayout();

		return true;
	}

	int GetSinglePaneMode() const
	{
		return m_nSinglePane;
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

	// call to initiate moving splitter bar with keyboard
	void MoveSplitterBar()
	{
		int x = 0;
		int y = 0;
		if(m_bVertical)
		{
			x = m_splitterVPos + (m_cxySplitBar / 2) + m_cxyBarEdge;
			y = (m_rectClient.bottom - m_rectClient.top - m_cxySplitBar - m_cxyBarEdge) / 2;
		}
		else
		{
			x = (m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge) / 2;
			y = m_splitterVPos + (m_cxySplitBar / 2) + m_cxyBarEdge;
		}

		POINT pt = { x, y };
		ClientToScreen(&pt);
		::SetCursorPos(pt.x, pt.y);

		m_splitterVPosNew = m_splitterVPos;
		SetCapture();
		//m_hWndFocusSave = pT->SetFocus();
		::SetCursor(m_hCursorWE);
		if(!m_bFullDrag)
			DrawGhostBar();
		if(m_bVertical)
			m_cxyDragOffset = x - m_rectClient.left - m_splitterVPos;
		else
			m_cxyDragOffset = y - m_rectClient.top - m_splitterVPos;
	}

// Overrideables

	// called only if pane is empty
	void DrawSplitterPane(int nPane)
	{
#if 0		
		RECT rect = {};
		if(GetSplitterPaneRect(nPane, &rect))
		{
			if((GetExStyle() & WS_EX_CLIENTEDGE) == 0)
				dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
			dc.FillRect(&rect, COLOR_APPWORKSPACE);
		}
#endif		
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
//					if(m_hWndPane[nPane] != NULL)
//						::SetWindowPos(m_hWndPane[nPane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
//					else
						InvalidateRect(&rect);
				}
			}
		}
		else
		{
			if(GetSplitterPaneRect(m_nSinglePane, &rect))
			{
//				if(m_hWndPane[m_nSinglePane] != NULL)
//					::SetWindowPos(m_hWndPane[m_nSinglePane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
//				else
					InvalidateRect(&rect);
			}
		}
	}

	bool GetSplitterBarRect(LPRECT lpRect) const
	{
		ATLASSERT(lpRect != NULL);
		if((m_nSinglePane != SPLIT_PANE_NONE) || (m_splitterVPos == -1))
			return false;

		if(m_bVertical)
		{
			lpRect->left = m_rectClient.left + m_splitterVPos;
			lpRect->top = m_rectClient.top;
			lpRect->right = m_rectClient.left + m_splitterVPos + m_cxySplitBar + m_cxyBarEdge;
			lpRect->bottom = m_rectClient.bottom;
		}
		else
		{
			lpRect->left = m_rectClient.left;
			lpRect->top = m_rectClient.top + m_splitterVPos;
			lpRect->right = m_rectClient.right;
			lpRect->bottom = m_rectClient.top + m_splitterVPos + m_cxySplitBar + m_cxyBarEdge;
		}

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
			if(m_bVertical)
			{
				lpRect->left = m_rectClient.left;
				lpRect->top = m_rectClient.top;
				lpRect->right = m_rectClient.left + m_splitterVPos;
				lpRect->bottom = m_rectClient.bottom;
			}
			else
			{
				lpRect->left = m_rectClient.left;
				lpRect->top = m_rectClient.top;
				lpRect->right = m_rectClient.right;
				lpRect->bottom = m_rectClient.top + m_splitterVPos;
			}
		}
		else if(nPane == SPLIT_PANE_RIGHT)
		{
			if(m_bVertical)
			{
				lpRect->left = m_rectClient.left + m_splitterVPos + m_cxySplitBar + m_cxyBarEdge;
				lpRect->top = m_rectClient.top;
				lpRect->right = m_rectClient.right;
				lpRect->bottom = m_rectClient.bottom;
			}
			else
			{
				lpRect->left = m_rectClient.left;
				lpRect->top = m_rectClient.top + m_splitterVPos + m_cxySplitBar + m_cxyBarEdge;
				lpRect->right = m_rectClient.right;
				lpRect->bottom = m_rectClient.bottom;
			}
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

		return ((y >= m_splitterVPos) && (y < (m_splitterVPos + SPLITLINE_WIDTH)));
	}

	void DrawGhostBar()
	{
	}

	void GetSystemSettings(bool bUpdate)
	{
		if((m_dwExtendedStyle & SPLIT_FIXEDBARSIZE) == 0)
		{
			m_cxySplitBar = ::GetSystemMetrics(m_bVertical ? SM_CXSIZEFRAME : SM_CYSIZEFRAME);
		}

		if((GetExStyle() & WS_EX_CLIENTEDGE) != 0)
		{
			m_cxyBarEdge = 2 * ::GetSystemMetrics(m_bVertical ? SM_CXEDGE : SM_CYEDGE);
			m_cxyMin = 0;
		}
		else
		{
			m_cxyBarEdge = 0;
			m_cxyMin = 2 * ::GetSystemMetrics(m_bVertical ? SM_CXEDGE : SM_CYEDGE);
		}

		::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &m_bFullDrag, 0);

		if(bUpdate)
			UpdateSplitterLayout();
	}

	bool IsProportional() const
	{
		return ((m_dwExtendedStyle & SPLIT_PROPORTIONAL) != 0);
	}

	void StoreProportionalPos()
	{
		int cxyTotal = m_bVertical ? (m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge) : (m_rectClient.bottom - m_rectClient.top - m_cxySplitBar - m_cxyBarEdge);
		if(cxyTotal > 0)
			m_nProportionalPos = ::MulDiv(m_splitterVPos, m_nPropMax, cxyTotal);
		else
			m_nProportionalPos = 0;
		//ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::StoreProportionalPos - %i\n"), m_nProportionalPos);
	}

	void UpdateProportionalPos()
	{
		int cxyTotal = m_bVertical ? (m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge) : (m_rectClient.bottom - m_rectClient.top - m_cxySplitBar - m_cxyBarEdge);
		if(cxyTotal > 0)
		{
			int xyNewPos = ::MulDiv(m_nProportionalPos, cxyTotal, m_nPropMax);
			m_bUpdateProportionalPos = false;
			SetSplitterPos(xyNewPos, false);
		}
	}

	bool IsRightAligned() const
	{
		return ((m_dwExtendedStyle & SPLIT_LEFTALIGNED) != 0);
	}

	void StoreRightAlignPos()
	{
		int cxyTotal = m_bVertical ? (m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge) : (m_rectClient.bottom - m_rectClient.top - m_cxySplitBar - m_cxyBarEdge);
		if(cxyTotal > 0)
			m_nProportionalPos = cxyTotal - m_splitterVPos;
		else
			m_nProportionalPos = 0;
		//ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::StoreRightAlignPos - %i\n"), m_nProportionalPos);
	}

	void UpdateRightAlignPos()
	{
		int cxyTotal = m_bVertical ? (m_rectClient.right - m_rectClient.left - m_cxySplitBar - m_cxyBarEdge) : (m_rectClient.bottom - m_rectClient.top - m_cxySplitBar - m_cxyBarEdge);
		if(cxyTotal > 0)
		{
			m_bUpdateProportionalPos = false;
			SetSplitterPos(cxyTotal - m_nProportionalPos, false);
		}
	}

};

#endif /* __WOCHAT_XWINDOW_H__ */