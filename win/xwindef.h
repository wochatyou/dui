#ifndef __WOCHAT_XWINDEF_H__
#define __WOCHAT_XWINDEF_H__

#define DEFAULT_BACKGROUND_COLOR	0xFFF9F3F1
#define DEFAULT_SEPERATELINE_COLOR	0xFFECECEC

#define XWIN_MAX_STRING				128
#define XWIN_MAX_INPUTSTRING		(1<<16)		// maximu input string

#define XWIN0_WIDTH		60
#define XWIN1_WIDTH		300
#define XWIN1_HEIGHT	48
#define XWIN3_WIDTH		400
#define XWIN3_HEIGHT	XWIN1_HEIGHT
#define XWIN4_HEIGHT	100
#define XWIN5_HEIGHT	160

#define XWIN_DEFAULT_COLOR			RGB(241, 243, 249)

#define WM_WIN0_MESSAGE    (WM_USER + 200)
#define WM_WIN1_MESSAGE    (WM_USER + 201)
#define WM_WIN2_MESSAGE    (WM_USER + 202)
#define WM_WIN3_MESSAGE    (WM_USER + 203)
#define WM_WIN4_MESSAGE    (WM_USER + 204)
#define WM_WIN5_MESSAGE    (WM_USER + 205)

#define WM_INIT_THREAD     (WM_USER + 206)
#define WM_MQTT_PUBMSG	   (WM_USER + 207)
#define WM_MQTT_SUBMSG	   (WM_USER + 208)

#define DEFAULT_MQTT_SERVER		("im.wochat.org")
#define DEFAULT_MQTT_PORT		1883


#define DECLARE_XWND_CLASS(WndClassName, uIcon, uIconSmall) \
static ATL::CWndClassInfo& GetWndClassInfo() \
{ \
	static ATL::CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, StartWindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("") \
	}; \
	if (0 != uIcon) \
		wc.m_wc.hIcon = LoadIcon(ATL::_AtlBaseModule.GetModuleInstance(), MAKEINTRESOURCE(uIcon)); \
	if (0 != uIconSmall) \
		wc.m_wc.hIconSm = LoadIcon(ATL::_AtlBaseModule.GetModuleInstance(), MAKEINTRESOURCE(uIconSmall)); \
	return wc; \
}

#endif  /* __WOCHAT_XWINDEF_H__ */

