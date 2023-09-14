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


// Splitter panes constants
#define SPLIT_OVER_NONE_BAR			0
#define SPLIT_OVER_VERTICAL_BAR		1
#define SPLIT_OVER_HORIZONAL_BAR	2

class XWindow : public ATL::CWindowImpl<XWindow>
{
public:
	DECLARE_XWND_CLASS(NULL, IDR_MAINFRAME, 0)

	XWindow() 
	{
	}

	~XWindow()
	{
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
		PostQuitMessage(0);

		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

		lpMMI->ptMinTrackSize.x = 800;
		lpMMI->ptMinTrackSize.y = 600;

		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (SIZE_MINIMIZED == wParam)
			return 0;

		Invalidate();

		return 0;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
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

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
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
		Invalidate();
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		PAINTSTRUCT ps;
		BeginPaint(&ps);
		EndPaint(&ps);
		return 0;
	}

};

#endif /* __WOCHAT_XWINDOW_H__ */