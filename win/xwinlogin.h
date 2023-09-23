#ifndef __WOCHAT_XWINLOGIN_H__
#define __WOCHAT_XWINLOGIN_H__

#include "dui/dui_win.h"

#ifndef __cplusplus
#error Wochat requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
#error xwindow.h requires atlbase.h to be included first
#endif

#ifndef __ATLWIN_H__
#error xwindow.h requires atlwin.h to be included first
#endif

enum {
	XWIN6_BUTTON_LOGIN = 0
	,XWIN6_BUTTON_CREATE
};

class XDUILogin : public XWindowT <XDUILogin>
{
	enum
	{
		XWIN6_BITMAP_LOGINN
		, XWIN6_BITMAP_LOGINH				// Hover
		, XWIN6_BITMAP_LOGINP				// Press
		, XWIN6_BITMAP_LOGINA				// Active

		, XWIN6_BITMAP_CREATEN
		, XWIN6_BITMAP_CREATEH
		, XWIN6_BITMAP_CREATEP
		, XWIN6_BITMAP_CREATEA
	};

public:
	XDUILogin()
	{
		m_backgroundColor = 0xFFCC5356;
		m_buttonEndIdx = XWIN6_BUTTON_CREATE;
		m_message = WM_XWINDOWS00;
		m_property |= (DUI_PROP_MOVEWIN | DUI_PROP_HANDLETIMER | DUI_PROP_HANDLEKEYBOARD);

		m_editName.SetProperty(XEDIT_PROP_ABCD123);
		m_editPasswd.SetProperty(XEDIT_PROP_PASSWORD);

		m_editName.SetBitmap((U32*)xbmpEditUname, 50, 19);
		m_editPasswd.SetBitmap((U32*)xbmpEditPasswd, 34, 19);

		InitButtons();
	}

	~XDUILogin() {}

	XEditBoxSL m_editName;
	XEditBoxSL m_editPasswd;

	void InitBitmap()
	{
		U8 id;
		XBitmap* bmp;

		int w = 120;
		int h = 30;
		id = XWIN6_BITMAP_LOGINN;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpLoginN;    bmp->w = w; bmp->h = h;
		id = XWIN6_BITMAP_LOGINH;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpLoginH;    bmp->w = w; bmp->h = h;
		id = XWIN6_BITMAP_LOGINP;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpLoginP;    bmp->w = w; bmp->h = h;
		id = XWIN6_BITMAP_LOGINA;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpLoginH;    bmp->w = w; bmp->h = h;

		id = XWIN6_BITMAP_CREATEN;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCreateN;   bmp->w = w; bmp->h = h;
		id = XWIN6_BITMAP_CREATEH;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCreateH;   bmp->w = w; bmp->h = h;
		id = XWIN6_BITMAP_CREATEP;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCreateP;   bmp->w = w; bmp->h = h;
		id = XWIN6_BITMAP_CREATEA;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCreateH;   bmp->w = w; bmp->h = h;
	}

	int InitButtons()
	{
		U8 id;
		XButton* button;
		XBitmap* bitmap;
		InitBitmap(); // inital all bitmap resource

		// Initialize All buttons
		for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
		{
			button = &m_button[i];
			m_button[i].pfAction = ButtonAction;;
		}

		id = XWIN6_BUTTON_LOGIN; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN6_BITMAP_LOGINN];  button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN6_BITMAP_LOGINH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN6_BITMAP_LOGINP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN6_BITMAP_LOGINA]; button->imgActive = bitmap;

		id = XWIN6_BUTTON_CREATE; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN6_BITMAP_CREATEN];  button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN6_BITMAP_CREATEH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN6_BITMAP_CREATEP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN6_BITMAP_CREATEA]; button->imgActive = bitmap;

		return 0;
	}

	void UpdateButtonPosition()
	{
		int id, margin, L, T, R, B;
		int gap = 30; // pixel
		int lineH;
		XButton* button;
		XButton* buttonPrev;
		XBitmap* bmp;

		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		id = XWIN6_BUTTON_LOGIN;  button = &m_button[id]; bmp = button->imgNormal;
		margin = (w - bmp->w) >> 1;
		lineH = m_editName.GetLineHeight();
		L = (margin >> 1); R = L + (w - margin);
		T = gap; B = T + ((lineH >= bmp->h) ? lineH : bmp->h);
		m_editName.UpdatePosition(L, T, R, B);

		lineH = m_editPasswd.GetLineHeight();
		T = B + gap; B = T + ((lineH >= bmp->h) ? lineH : bmp->h);
		m_editPasswd.UpdatePosition(L, T, R, B);

		button->top = B + gap;
		button->bottom = button->top + bmp->h;
		button->left = margin;
		button->right = button->left + bmp->w;
		buttonPrev = button;

		gap >>= 1;
		id = XWIN6_BUTTON_CREATE;  button = &m_button[id]; bmp = button->imgNormal;
		button->left = margin;
		button->right = button->left + bmp->w;
		button->top = buttonPrev->bottom + gap;
		button->bottom = button->top + bmp->h;
	}

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		assert(nullptr != g_ftFace2);
		int r0 = m_editName.Init(g_ftFace2);
		int r1 = m_editPasswd.Init(g_ftFace2);
		return (r0 | r1);
	}

	int DoDestroy(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		m_editName.Term();
		m_editPasswd.Term();
		return 0;
	}

	int DoMouseMove(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		int r = DUI_STATUS_NODRAW;
		int r0 = DUI_STATUS_NODRAW;
		int r1 = DUI_STATUS_NODRAW;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		// transfer the coordination from real window to local virutal window
		xPos -= m_area.left;
		yPos -= m_area.top;

		if (m_editName.IsHitted(xPos, yPos))
		{
			SetCursorIBeam();
			r0 = m_editName.SetState(XEditStatus::XEDIT_STATUS_HOVER);
		}
		else
			r0 = m_editName.UnSetState(XEditStatus::XEDIT_STATUS_HOVER);

		if (m_editPasswd.IsHitted(xPos, yPos))
		{
			SetCursorIBeam();
			r1 = m_editPasswd.SetState(XEditStatus::XEDIT_STATUS_HOVER);
		}
		else
			r1 = m_editPasswd.UnSetState(XEditStatus::XEDIT_STATUS_HOVER);

		if (r0 || r1)
			r = DUI_STATUS_NEEDRAW;

		return r;
	}

	int DoLButtonDown(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		int r = DUI_STATUS_NODRAW;
		int r0 = DUI_STATUS_NODRAW;
		int r1 = DUI_STATUS_NODRAW;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		// transfer the coordination from real window to local virutal window
		xPos -= m_area.left;
		yPos -= m_area.top;

		if (m_editName.IsHitted(xPos, yPos))
		{
			SetCursorIBeam();
			r0 = m_editName.SetFocusedStatus();
		}
		else
			r0 = m_editName.ClearFocusedStatus();

		if (m_editPasswd.IsHitted(xPos, yPos))
		{
			SetCursorIBeam();
			r1 = m_editPasswd.SetFocusedStatus();
		}
		else
			r1 = m_editPasswd.ClearFocusedStatus();

		if (r0 || r1)
			r = DUI_STATUS_NEEDRAW;

		return r;
	}

	int DoSetCursor(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		int xPos = (int)wParam;
		int yPos = (int)lParam;

		xPos -= m_area.left;
		yPos -= m_area.top;

		if (m_editName.IsHitted(xPos, yPos) || m_editPasswd.IsHitted(xPos, yPos))
		{
			return 1;
		}
		return 0;
	}

	int DoFocusGet(U32 uMsg, int xPos, int yPos, void* lpData = nullptr)
	{
		bool isFocused;
		int rn = DUI_STATUS_NODRAW;
		int rp = DUI_STATUS_NODRAW;

		xPos -= m_area.left;
		yPos -= m_area.top;

		rn = m_editName.ClearFocusedStatus();
		if (m_editName.IsHitted(xPos, yPos))
		{
			assert(DUI_STATUS_ISFOCUS & m_status);
			rn = m_editName.SetFocusedStatus();
		}
		rp = m_editPasswd.ClearFocusedStatus();
		if (m_editPasswd.IsHitted(xPos, yPos))
		{
			assert(DUI_STATUS_ISFOCUS & m_status);
			rp = m_editPasswd.SetFocusedStatus();
		}

		return (rn | rp);
	}

	int DoFocusLose(U32 uMsg, int xPos, int yPos, void* lpData = nullptr)
	{
#if 0
		m_editBox.ClearFocusedStatus();
#endif
		return DUI_STATUS_NEEDRAW;
	}

	int DoChar(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		int r = DUI_STATUS_NODRAW;
		U16 charCode = static_cast<U16>(wParam);

		if (m_editName.IsFocused())
		{
			r = m_editName.OnAddOneChar(charCode);
		}
		else if (m_editPasswd.IsFocused())
		{
			r = m_editPasswd.OnAddOneChar(charCode);
		}

		return r;
	}

	int DoTimer(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		int r = DUI_STATUS_NODRAW;

		if (m_editName.IsFocused())
		{
			m_editName.FlipCaret();
			r = DUI_STATUS_NEEDRAW;
		}
		if (m_editPasswd.IsFocused())
		{
			m_editPasswd.FlipCaret();
			r = DUI_STATUS_NEEDRAW;
		}
		return r;
	}

	int DoKeyPress(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		int rx = DUI_STATUS_NODRAW;
		int rn = DUI_STATUS_NODRAW;
		int rp = DUI_STATUS_NODRAW;

		U32 keyCode = static_cast<U32>(wParam);

		bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
		bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;

		switch (keyCode)
		{
		case VK_LEFT:
			rn = m_editName.MoveCursorLR(-1);
			rp = m_editPasswd.MoveCursorLR(-1);
			break;
		case VK_RIGHT:
			rn = m_editName.MoveCursorLR(1);
			rp = m_editPasswd.MoveCursorLR(1);
			break;
		case VK_BACK:
		case VK_RETURN:
		case VK_DELETE:
		case VK_TAB:
		case VK_UP:
		case VK_DOWN:
		case VK_HOME:
		case VK_END:
		case VK_INSERT:
		case 'C':
		case 'X':
		case 'A':
		case 'V':
		default:
			break;
		}

		if (DUI_STATUS_NODRAW != rn || DUI_STATUS_NODRAW != rp)
			rx = DUI_STATUS_NEEDRAW;
		return rx;
	}


	int Draw() 
	{ 
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		if (m_editName.HasData())
		{
			int sw = m_editName.GetWidth();
			int sh = m_editName.GetHeight();
			int dx = m_editName.GetLeftPos();
			int dy = m_editName.GetTopPos();
			m_editName.Draw();
			U32* editBuf = m_editName.GetDrawBuffer();
			assert(nullptr != editBuf);
			ScreenDrawRect(m_screen, w, h, editBuf, sw, sh, dx, dy);

			sw = m_editPasswd.GetWidth();
			sh = m_editPasswd.GetHeight();
			dx = m_editPasswd.GetLeftPos();
			dy = m_editPasswd.GetTopPos();
			m_editPasswd.Draw();
			editBuf = m_editPasswd.GetDrawBuffer();
			assert(nullptr != editBuf);
			ScreenDrawRect(m_screen, w, h, editBuf, sw, sh, dx, dy);

		}
		return 0; 
	}
};


class XWinLogin : public ATL::CWindowImpl<XWinLogin>
{
private:
	// the memory to hold the context to paint to the screen
	U32* m_screenBuff = nullptr;
	U32  m_screenSize = 0;

	RECT m_rectClient = { 0 };

	UINT m_nDPI = 96;

	bool m_bSuccessful = false;

	XDUILogin m_loginWin;

	ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;
	ID2D1Bitmap*           m_pixelBitmap = nullptr;

public:
	DECLARE_XWND_CLASS(TEXT("XWinLogin"), IDR_MAINFRAME, 0)

	XWinLogin() 
	{
	}

	~XWinLogin() 
	{
		if (nullptr != m_screenBuff)
			VirtualFree(m_screenBuff, 0, MEM_RELEASE);

		m_screenBuff = nullptr;
		m_screenSize = 0;
	}

	bool IsSuccessful() 
	{
		return m_bSuccessful; 
	}

	int DoDUIMessageProcess(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr, bool bUpdate = true)
	{
		int r = m_loginWin.HandleOSMessage((U32)uMsg, (U64)wParam, (U64)lParam, lpData);

		if (r > 0 && bUpdate)
			Invalidate();

		return r;
	}


	BEGIN_MSG_MAP(XWinLogin)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_XWINDOWS00, OnWin0Message)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDoubleClick)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0; // don't want flicker
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		KillTimer(XWIN_666MS_TIMER);

		DoDUIMessageProcess(uMsg, wParam, lParam);
		SafeRelease(&m_pixelBitmap);
		SafeRelease(&m_pD2DRenderTarget);
		PostQuitMessage(0);

		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int r = DoDUIMessageProcess(uMsg, (WPARAM)m_hWnd, lParam);
		r = (r > 0) ? r : 0;
		if (r)
		{
			MessageBox(_T("WM_CREATE failed!"), _T("Error"), MB_OK);
			PostMessage(WM_CLOSE);
			return 0;
		}

		SetTimer(XWIN_666MS_TIMER, 666);

		return 0;
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DoDUIMessageProcess(uMsg, wParam, lParam);
		return 0;
	}

	LRESULT OnWin0Message(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		U8 buttonId = (U8)wParam;

		if (0 == buttonId)
		{
			m_bSuccessful = true;
		}
		PostMessage(WM_CLOSE);

		return 0;
	}


	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		//LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		//lpMMI->ptMinTrackSize.x = 800;
		//lpMMI->ptMinTrackSize.y = 600;
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (((HWND)wParam == m_hWnd) && (LOWORD(lParam) == HTCLIENT))
		{
			DWORD dwPos = ::GetMessagePos();
			POINT ptPos = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
			ScreenToClient(&ptPos);
			int r = DoDUIMessageProcess(uMsg, (WPARAM)ptPos.x, (LPARAM)ptPos.y, nullptr, false);
			if (r > 0)
				bHandled = TRUE;
		}

		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		U16 W, H;
		RECT area;

		if (SIZE_MINIMIZED == wParam)
			return 0;

		GetClientRect(&m_rectClient);
		ATLASSERT(0 == m_rectClient.left);
		ATLASSERT(0 == m_rectClient.top);
		ATLASSERT(m_rectClient.right > 0);
		ATLASSERT(m_rectClient.bottom > 0);

		if (nullptr != m_screenBuff)
		{
			VirtualFree(m_screenBuff, 0, MEM_RELEASE);
			m_screenBuff = nullptr;
			m_screenSize = 0;
		}

		W = (U16)(m_rectClient.right - m_rectClient.left);
		H = (U16)(m_rectClient.bottom - m_rectClient.top);

		m_screenSize = DUI_ALIGN_PAGE(W * H * sizeof(U32));
		ATLASSERT(m_screenSize >= (W * H * sizeof(U32)));

		m_screenBuff = (U32*)VirtualAlloc(NULL, m_screenSize, MEM_COMMIT, PAGE_READWRITE);

		if (nullptr != m_screenBuff)
		{
			RECT* r = &area;
			r->left = m_rectClient.left;
			r->right = m_rectClient.right;
			r->top = m_rectClient.top;
			r->bottom = m_rectClient.bottom;
			m_loginWin.OnSize(uMsg, wParam, (LPARAM)r, m_screenBuff);
		}

		SafeRelease(&m_pD2DRenderTarget);
		Invalidate();
		return 0;
	}


	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DoDUIMessageProcess(uMsg, wParam, lParam);
		return 0;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DoDUIMessageProcess(uMsg, wParam, lParam);
		return 0;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DoDUIMessageProcess(uMsg, wParam, lParam);

		return 0;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DoDUIMessageProcess(uMsg, wParam, lParam);
		return 0;
	}

	LRESULT OnLButtonDoubleClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DoDUIMessageProcess(uMsg, wParam, lParam);
		return 0;
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DoDUIMessageProcess(uMsg, wParam, lParam);
		return 0;
	}

	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{

		return 0;
	}

	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
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
			renderTargetProperties.dpiX = m_nDPI;
			renderTargetProperties.dpiY = m_nDPI;
			renderTargetProperties.pixelFormat = pixelFormat;

			D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRenderTragetproperties
				= D2D1::HwndRenderTargetProperties(m_hWnd, D2D1::SizeU(m_rectClient.right - m_rectClient.left, m_rectClient.bottom - m_rectClient.top));

			ATLASSERT(nullptr != g_pD2DFactory);

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

			src = m_loginWin.Render();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				XRECT* xr = m_loginWin.GetWindowArea();
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

};

#endif /* __WOCHAT_XWINLOGIN_H__ */