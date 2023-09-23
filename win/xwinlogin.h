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

	// cairo/harfbuzz issue to cache to speed up
	cairo_font_extents_t m_font_extents = { 0 };
	cairo_glyph_t* m_cairo_glyphs = nullptr;
	cairo_font_face_t* m_cairo_face = nullptr;
	hb_font_t* m_hb_font0 = nullptr;
	hb_font_t* m_hb_font1 = nullptr;
	hb_buffer_t* m_hb_buffer = nullptr;

public:
	XDUILogin()
	{
		m_backgroundColor = 0xFFCC5356;
		m_message = WM_WIN6_MESSAGE;
		m_property |= DUI_PROP_MOVEWIN;
		m_buttonStartIdx = XWIN6_BUTTON_LOGIN;
		m_buttonEndIdx = XWIN6_BUTTON_CREATE;

		InitButtons();
	}

	~XDUILogin() {}

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
		int id, margin;
		int gap = 20; // pixel
		XButton* button;
		XButton* buttonPrev;
		XBitmap* bmp;

		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		id = XWIN6_BUTTON_CREATE;  button = &m_button[id]; bmp = button->imgNormal;
		margin = (w - bmp->w) >> 1;
		button->bottom = h - gap;
		button->top = button->bottom - bmp->h;
		button->left = margin;
		button->right = button->left + bmp->w;
		buttonPrev = button;

		id = XWIN6_BUTTON_LOGIN;  button = &m_button[id]; bmp = button->imgNormal;
		button->left = margin;
		button->right = button->left + bmp->w;
		button->bottom = buttonPrev->top - gap;
		button->top = button->bottom - bmp->h;
	}

	void UpdatePosition()
	{
		UpdateButtonPosition();
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		UpdateButtonPosition();
		return 0;
	}

	static int ButtonAction(void* obj, U32 uMsg, U64 wParam, U64 lParam)
	{
		int ret = 0;
		XDUILogin* xw = (XDUILogin*)obj;
		if (nullptr != xw)
			ret = xw->NotifyParent(uMsg, wParam, lParam);
		return ret;
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

	BEGIN_MSG_MAP(XWinLogin)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_WIN6_MESSAGE, OnWin6Message)
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
		m_loginWin.OnDestroy(uMsg, wParam, lParam);
		SafeRelease(&m_pixelBitmap);
		SafeRelease(&m_pD2DRenderTarget);
		PostQuitMessage(0);

		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int r = m_loginWin.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		if (r)
		{
			MessageBox(_T("WM_CREATE failed!"), _T("Error"), MB_OK);
			PostMessage(WM_CLOSE);
			return 0;
		}
		return 0;
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnWin6Message(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
			int r = m_loginWin.OnSetCursor(uMsg, ptPos.x, ptPos.y);
			if (r)
				bHandled = FALSE;
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
		return 0;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int r = m_loginWin.OnMouseMove(uMsg, wParam, lParam);
		if (r)
			Invalidate();

		return 0;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int r = m_loginWin.OnLButtonDown(uMsg, wParam, lParam);
		if (r)
			Invalidate();

		return 0;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int r = m_loginWin.OnLButtonUp(uMsg, wParam, lParam);
		if (r)
			Invalidate();

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
		return 0;
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
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