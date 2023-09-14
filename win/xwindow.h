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

// fast base64 encode/decode
#include "turbobase64/turbob64.h"

#include "windows0.h"
#include "windows1.h"
#include "windows2.h"
#include "windows3.h"
#include "windows4.h"
#include "windows5.h"

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

static bool process_messages = true;
static int msg_count = 0;
static int last_mid = 0;
static bool timed_out = false;
static int connack_result = 0;
static bool connack_received = false;

static U16 xmsgUTF16[XWIN_MAX_INPUTSTRING + 1] = { 0 };
static U8  xmsgUTF8[XWIN_MAX_INPUTSTRING + XWIN_MAX_INPUTSTRING + 1] = { 0 };

static int PostMQTTMessage(HWND hWnd, const struct mosquitto_message* message, const mosquitto_property* properties)
{
	U8* p;
	U8* msg;
	size_t len;
	size_t bytes;

	assert(nullptr != message);

	msg = (U8*)message->payload;
	assert(nullptr != msg);
	len = (size_t)message->payloadlen;
	assert(len > 0);

	if (len > XWIN_MAX_INPUTSTRING)
		len = XWIN_MAX_INPUTSTRING;

	p = (U8*)xmsgUTF16;
	bytes = tb64dec((const unsigned char*)msg, len, (unsigned char*)p);
	p[bytes] = 0; p[bytes+1] = 0;

	if (::IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_MQTT_SUBMSG, (WPARAM)xmsgUTF16, (LPARAM)bytes/2);
	}

	return 0;
}

static void MQTT_Message_Callback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* message, const mosquitto_property* properties)
{
	int i;
	bool res;
	struct mosq_config* pMQTTConf;
	HWND hWnd;

	UNUSED(properties);

	pMQTTConf = (struct mosq_config*)obj;
	assert(nullptr != pMQTTConf);

	hWnd = (HWND)(pMQTTConf->userdata);
	assert(::IsWindow(hWnd));

	if (process_messages == false) return;

	if (pMQTTConf->retained_only && !message->retain && process_messages)
	{
		process_messages = false;
		if (last_mid == 0)
		{
			mosquitto_disconnect_v5(mosq, 0, pMQTTConf->disconnect_props);
		}
		return;
	}

	if (message->retain && pMQTTConf->no_retain)
		return;

	if (pMQTTConf->filter_outs)
	{
		for (i = 0; i < pMQTTConf->filter_out_count; i++)
		{
			mosquitto_topic_matches_sub(pMQTTConf->filter_outs[i], message->topic, &res);
			if (res) return;
		}
	}

	if (pMQTTConf->remove_retained && message->retain)
	{
		mosquitto_publish(mosq, &last_mid, message->topic, 0, NULL, 1, true);
	}

	PostMQTTMessage(hWnd, message, properties);

	if (pMQTTConf->msg_count > 0)
	{
		msg_count++;
		if (pMQTTConf->msg_count == msg_count)
		{
			process_messages = false;
			if (last_mid == 0)
			{
				mosquitto_disconnect_v5(mosq, 0, pMQTTConf->disconnect_props);
			}
		}
	}
}

static void MQTT_Connect_Callback(struct mosquitto* mosq, void* obj, int result, int flags, const mosquitto_property* properties)
{
	int i;
	struct mosq_config* pMQTTConf;

	UNUSED(flags);
	UNUSED(properties);

	pMQTTConf = (struct mosq_config*)obj;
	assert(nullptr != pMQTTConf);

	connack_received = true;

	connack_result = result;
	if (!result)
	{
		mosquitto_subscribe_multiple(mosq, NULL, pMQTTConf->topic_count, pMQTTConf->topics, pMQTTConf->qos, pMQTTConf->sub_opts, pMQTTConf->subscribe_props);

		for (i = 0; i < pMQTTConf->unsub_topic_count; i++)
		{
			mosquitto_unsubscribe_v5(mosq, NULL, pMQTTConf->unsub_topics[i], pMQTTConf->unsubscribe_props);
		}
	}
	else
	{
		if (result)
		{
			if (pMQTTConf->protocol_version == MQTT_PROTOCOL_V5)
			{
				if (result == MQTT_RC_UNSUPPORTED_PROTOCOL_VERSION)
				{
					//err_printf(&cfg, "Connection error: %s. Try connecting to an MQTT v5 broker, or use MQTT v3.x mode.\n", mosquitto_reason_string(result));
				}
				else
				{
					//err_printf(&cfg, "Connection error: %s\n", mosquitto_reason_string(result));
				}
			}
			else {
				//err_printf(&cfg, "Connection error: %s\n", mosquitto_connack_string(result));
			}
		}
		mosquitto_disconnect_v5(mosq, 0, pMQTTConf->disconnect_props);
	}
}

static void MQTT_Disconnect_Callback(struct mosquitto* mosq, void* obj, int result, const mosquitto_property* properties)
{

}

static void MQTT_Publish_Callback(struct mosquitto* mosq, void* obj, int mid, int reason_code, const mosquitto_property* properties)
{
	int i;

	i = 1;
}

static void MQTT_Subscribe_Callback(struct mosquitto* mosq, void* obj, int mid, int qos_count, const int* granted_qos)
{
	int i;
	struct mosq_config* pMQTTConf;
	bool some_sub_allowed = (granted_qos[0] < 128);
	bool should_print;

	pMQTTConf = (struct mosq_config*)obj;
	assert(nullptr != pMQTTConf);
	should_print = pMQTTConf->debug && !pMQTTConf->quiet;

#if 0
	if (should_print)
		printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
#endif
	for (i = 1; i < qos_count; i++)
	{
		//if (should_print) printf(", %d", granted_qos[i]);
		some_sub_allowed |= (granted_qos[i] < 128);
	}
	//if (should_print) printf("\n");

	if (some_sub_allowed == false)
	{
		mosquitto_disconnect_v5(mosq, 0, pMQTTConf->disconnect_props);
		//err_printf(&cfg, "All subscription requests were denied.\n");
	}

	if (pMQTTConf->exit_after_sub)
	{
		mosquitto_disconnect_v5(mosq, 0, pMQTTConf->disconnect_props);
	}

}

static void MQTT_Log_Callback(struct mosquitto* mosq, void* obj, int level, const char* str)
{
}


static MQTT_Methods mqtt_callback =
{
	MQTT_Message_Callback,
	MQTT_Connect_Callback,
	MQTT_Disconnect_Callback,
	MQTT_Subscribe_Callback,
	MQTT_Publish_Callback,
	MQTT_Log_Callback
};

static XMQTTMessage mqtt_message = 
{
	.host    = (char*)DEFAULT_MQTT_SERVER,
	.port    = DEFAULT_MQTT_PORT,
	.topic   = nullptr,
	.message = nullptr,
	.msglen  = 0
};

static HANDLE xMQTTHandles[2];

static DWORD WINAPI MQTTPubThread(LPVOID lpData)
{
	int ret;
	DWORD dwRet;
	Mosquitto q;
	HWND hWnd = (HWND)(lpData);

	ATLASSERT(::IsWindow(hWnd));

	InterlockedIncrement(&g_threadCount);

	q = MQTT::MQTT_PubInit(hWnd, mqtt_message.host, mqtt_message.port, &mqtt_callback);
	if (nullptr == q) // something is wrong in MQTT pub routine
	{
		PostMessage(hWnd, WM_INIT_THREAD, 2, 0);
		goto QuitMQTTPubThread;
	}

	while (true)
	{
		dwRet = MsgWaitForMultipleObjects(2, xMQTTHandles, FALSE, INFINITE, QS_ALLINPUT);
		switch (dwRet)
		{
		case WAIT_OBJECT_0 + 0:		// the request from UI thread
			{
				char* topic = mqtt_message.topic;
				char* message = mqtt_message.message;
				int msglen = mqtt_message.msglen;
				if (nullptr != topic && nullptr != message && 0 != msglen)
				{
					ret = MQTT::MQTT_PubMessage(q, topic, message, msglen);
				}
			}
			break;
		case WAIT_OBJECT_0 + 1:		// we have to quit
			goto QuitMQTTPubThread;
		default:
			break;
		}
	}

QuitMQTTPubThread:
	MQTT::MQTT_PubTerm(q);
	InterlockedDecrement(&g_threadCount);

	return 0;
}

static DWORD WINAPI MQTTSubThread(LPVOID lpData)
{
	int ret;
	Mosquitto q;
	HWND hWnd = (HWND)(lpData);

	ATLASSERT(::IsWindow(hWnd));

	InterlockedIncrement(&g_threadCount);

	// We put some long running intialized work in the seperated thread
	// to speed up the start of WoChat application.
	ret = BitCoinInit();
	if (0 != ret) // The intialization is failed
	{
		PostMessage(hWnd, WM_INIT_THREAD, 1, 0);
		goto QuitMQTTSubThread;
	}

	q = MQTT::MQTT_SubInit(hWnd, (char*)DEFAULT_MQTT_SERVER, DEFAULT_MQTT_PORT, &mqtt_callback);

	if (nullptr == q) // something is wrong in MQTT sub routine
	{
		PostMessage(hWnd, WM_INIT_THREAD, 2, 0);
		goto QuitMQTTSubThread;
	}

	ret = MQTT::MQTT_AddSubTopic(CLIENT_SUB, (char*)"ABC");
	if (0 != ret) // The intialization is failed
	{
		PostMessage(hWnd, WM_INIT_THREAD, 3, 0);
		goto QuitMQTTSubThread;
	}

	ret = MQTT::MQTT_AddSubTopic(CLIENT_SUB, (char*)"XYZ");
	if (0 != ret) // The intialization is failed
	{
		PostMessage(hWnd, WM_INIT_THREAD, 3, 0);
		goto QuitMQTTSubThread;
	}

	MQTT::MQTT_SubLoop(q);  // main loop go here.

	MQTT::MQTT_SubTerm(q);

	q = nullptr;

QuitMQTTSubThread:
	InterlockedDecrement(&g_threadCount);
	return 0;
}

// Splitter panes constants
#define SPLIT_OVER_NONE_BAR			0
#define SPLIT_OVER_VERTICAL_BAR		1
#define SPLIT_OVER_HORIZONAL_BAR	2

class XWindow : public ATL::CWindowImpl<XWindow>
{
public:
	DECLARE_XWND_CLASS(NULL, IDR_MAINFRAME, 0)

private:
	enum { STROKEWIDTH = 2, MOVESTEP = 1 };
	enum { m_leftRange = XWIN1_WIDTH, m_rightRange = XWIN3_WIDTH, m_topRange = XWIN3_HEIGHT, m_bottomRange = XWIN4_HEIGHT };
	enum { dragModeNone, dragModeV, dragModeH };

	RECT m_rcSplitter = { 0 };
	int m_xySplitterPosV       = m_leftRange;    // splitter bar position
	int m_xySplitterPosVNew    = -1;             // internal - new position while moving
	int m_xySplitterPosH       = -1;             // splitter bar position
	int m_xySplitterPosHNew    = -1;             // internal - new position while moving
	int m_xySplitterDefPosV    = -1;             // default position
	int m_xySplitterDefPosH    = -1;             // default position
	int m_xySplitterPosHfix    = XWIN1_HEIGHT;   // default position

	HCURSOR m_cursorSizeWE = nullptr;
	HCURSOR m_cursorSizeNS = nullptr;

	int m_cxyMin = 0;                // minimum pane size
	int m_dragMode = dragModeNone;
	int m_cxyDragOffsetV = 0;		// internal
	int m_cxyDragOffsetH = 0;		// internal

	int  m_toBottom = m_bottomRange; // keep track the horizonal bar to the bottom of Win4 and Win5

	ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;
	ID2D1Bitmap*           m_pixelBitmap = nullptr;

	// we have totoally 6 " virtual windows" within the main window.
	XWindow0 m_win0;
	XWindow1 m_win1;
	XWindow2 m_win2;
	XWindow3 m_win3;
	XWindow4 m_win4;
	XWindow5 m_win5;
	// we use 6 RECTs to track the position of the above 6 virtual windows
	RECT m_rect0 = { 0 };
	RECT m_rect1 = { 0 };
	RECT m_rect2 = { 0 };
	RECT m_rect3 = { 0 };
	RECT m_rect4 = { 0 };
	RECT m_rect5 = { 0 };

	AppMode m_mode = modeTalk;

	// the memory to hold the context to paint to the screen
	U32* m_screenBuff = nullptr;  
	U32  m_screenSize = 0;

public:
	XWindow() 
	{
		// We use the Id of each virtual window for debugging
		m_win0.SetWindowId(0);
		m_win1.SetWindowId(1);
		m_win2.SetWindowId(2);
		m_win3.SetWindowId(3);
		m_win4.SetWindowId(4);
		m_win5.SetWindowId(5);
	}

	~XWindow()
	{
		if (nullptr != m_screenBuff)
			VirtualFree(m_screenBuff, 0, MEM_RELEASE);

		m_screenBuff = nullptr;
		m_screenSize = 0;
	}

	BEGIN_MSG_MAP(XWindow)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_MQTT_PUBMSG, OnMQTTPubMessage)
		MESSAGE_HANDLER(WM_MQTT_SUBMSG, OnMQTTSubMessage)
		MESSAGE_HANDLER(WM_AKELPAD_MESSAGE, OnAkelPadMessage)
		MESSAGE_HANDLER(WM_WIN0_MESSAGE, OnWin0Message)
		MESSAGE_HANDLER(WM_WIN1_MESSAGE, OnWin1Message)
		MESSAGE_HANDLER(WM_WIN2_MESSAGE, OnWin2Message)
		MESSAGE_HANDLER(WM_WIN3_MESSAGE, OnWin3Message)
		MESSAGE_HANDLER(WM_WIN4_MESSAGE, OnWin4Message)
		MESSAGE_HANDLER(WM_WIN5_MESSAGE, OnWin5Message)
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
		MESSAGE_HANDLER(WM_INIT_THREAD, OnInitThread)
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

		SetEvent(xMQTTHandles[1]); // tell MQTT pub thread to quit gracefully
		Sleep(1000);
		CloseHandle(xMQTTHandles[0]);
		CloseHandle(xMQTTHandles[1]);

		PostQuitMessage(0);

		return 0;
	}

	LRESULT OnInitThread(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MessageBox(_T("Initilization failed!"), _T("WoChat"), MB_OK);

		PostMessage(WM_CLOSE);

		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int r0, r1, r2, r3, r4, r5;
		
		DWORD dwThreadID0;
		DWORD dwThreadID1;
		HANDLE hThread0 = ::CreateThread(NULL, 0, MQTTSubThread, m_hWnd, 0, &dwThreadID0);
		HANDLE hThread1 = ::CreateThread(NULL, 0, MQTTPubThread, m_hWnd, 0, &dwThreadID1);

		if (nullptr == hThread0 || nullptr == hThread1)
		{
			MessageBox(TEXT("MQTT thread creation is failed!"), TEXT("WoChat Error"), MB_OK);
		}

		m_cursorSizeWE = ::LoadCursor(NULL, IDC_SIZEWE);
		m_cursorSizeNS = ::LoadCursor(nullptr, IDC_SIZENS);

		if (nullptr == m_cursorSizeWE || nullptr == m_cursorSizeNS)
		{
			PostMessage(WM_CLOSE);
			return 0;
		}

		xMQTTHandles[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
		xMQTTHandles[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

		if(NULL == xMQTTHandles[0] || NULL == xMQTTHandles[1])
		{
			PostMessage(WM_CLOSE);
			return 0;
		}

		r0 = m_win0.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		r1 = m_win1.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		r2 = m_win2.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		r3 = m_win3.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		r4 = m_win4.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);
		r5 = m_win5.OnCreate(uMsg, (WPARAM)m_hWnd, lParam);

		if (0 != r0 || 0 != r1 || 0 != r2 || 0 != r3 || 0 != r4 || 0 != r5)
		{
			MessageBox(_T("WM_CREATE failed!"), _T("WoChat"), MB_OK);
			PostMessage(WM_CLOSE);
			return 0;
		}

		XChatGroup* cg = m_win2.GetFirstChatGroup();
		m_win4.SetChatGroup(cg);

		return 0;
	}

	LRESULT OnWin0Message(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		m_mode = (AppMode)wParam;
		U32* dst;
		U32 size;
		RECT* r;
		int bottom;

		m_win1.ChangeAppMode(m_mode);
		m_win2.ChangeAppMode(m_mode);
		m_win3.ChangeAppMode(m_mode);
		m_win4.ChangeAppMode(m_mode);
		m_win5.ChangeAppMode(m_mode);
		if (m_win5.IsVisible())
		{
			m_xySplitterPosH = m_rcSplitter.bottom - m_toBottom;
			bottom = m_xySplitterPosH;
		}
		else
		{
			m_xySplitterPosH = -1;  // do not draw the horizonal bar
			bottom = m_rcSplitter.bottom;
		}
		r = &m_rect4;
		r->left = m_xySplitterPosV + STROKEWIDTH;
		r->top = m_xySplitterPosHfix + STROKEWIDTH;
		r->right = m_rcSplitter.right;
		r->bottom = bottom;
		dst = m_win4.GetScreenBuffer();
		size = (U32)((r->right - r->left) * (r->bottom - r->top));
		m_win4.SetPosition(r, dst, size);

		dst += size;
		r = &m_rect5;
		if (m_win5.IsVisible())
		{
			r->left = m_xySplitterPosV + STROKEWIDTH;
			r->right = m_rcSplitter.right;
			r->top = m_xySplitterPosH + STROKEWIDTH;
			r->bottom = m_rcSplitter.bottom;
		}
		else
		{
			r->left = r->right = r->top = r->bottom = 0;
			dst = nullptr;
		}
		size = (U32)((r->right - r->left) * (r->bottom - r->top));
		m_win5.SetPosition(r, dst, size);

		Invalidate();

		return 0;
	}

	LRESULT OnMQTTPubMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnMQTTSubMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		U16* msg = (U16*)wParam;
		int len = (int)lParam;

		if (len > 0)
		{
			m_win4.UpdateChatHistory(msg, 1);
			Invalidate();
		}

		return 0;
	}

	LRESULT OnWin1Message(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnWin2Message(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int r3 = 0, r4 = 0;
		if (WM_XWIN2_GROUPCHANGED == wParam)
		{
			XChatGroup* cg = (XChatGroup*)lParam;

			assert(nullptr != cg);

			U16* title = cg->name_;
			r3 = m_win3.UpdateTitle(title);
			r4 = m_win4.SetChatGroup(cg);
			if (0 != r3 || 0 != r4)
				Invalidate();
		}
		return 0;
	}

	LRESULT OnWin3Message(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		WPARAM wp = wParam;
		return 0;
	}

	LRESULT OnWin4Message(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		WPARAM wp = wParam;
		//MessageBox(_T("Get from Win4 !"), _T("WoChat"), MB_OK);
		return 0;
	}

	LRESULT OnWin5Message(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		U8 buttonId = (U8)wParam;

		switch (buttonId)
		{
		case XWIN5_BUTTON_SENDMESSAGE :
			{
				U16 len;
				uint16_t* msg = m_win5.GetInputMessage(&len);
				if (nullptr != msg)
				{
					size_t size = tb64enc((const unsigned char*)msg, len<<1, xmsgUTF8);
					assert(size > 0);
					xmsgUTF8[size] = 0;
					mqtt_message.topic = (char*)"WOCHAT";
					mqtt_message.message = (char*)xmsgUTF8;
					mqtt_message.msglen = size;
					SetEvent(xMQTTHandles[0]); // send this message to remote
					m_win4.UpdateChatHistory(msg);
					Invalidate();
				}
			}
			break;

		default : 
			break;
		}
		return 0;
	}

	LRESULT OnAkelPadMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if (WM_MOUSEMOVE == wParam)
		{
			int r0 = m_win0.ClearButtonStatus();
			int r1 = m_win1.ClearButtonStatus();
			int r2 = m_win2.ClearButtonStatus();
			int r3 = m_win3.ClearButtonStatus();
			int r4 = m_win4.ClearButtonStatus();
			int r5 = m_win5.ClearButtonStatus();
			if(0 != r0 || 0 != r1 || 0 != r2 || 0 != r3 || 0 != r4 || 0 != r5)
				Invalidate();
		}

		if (WM_AKELPAD_RETURN == wParam)
		{
			U16 len;
			uint16_t* msg = m_win5.GetInputMessage(&len);
			if (nullptr != msg)
			{
				size_t size = tb64enc((const unsigned char*)msg, len<<1, xmsgUTF8);
				assert(size > 0);
				xmsgUTF8[size] = 0;
				mqtt_message.topic = (char*)"WOCHAT";
				mqtt_message.message = (char*)xmsgUTF8;
				mqtt_message.msglen = size;
				SetEvent(xMQTTHandles[0]); // send this message to remote
				m_win4.UpdateChatHistory(msg);
				Invalidate();
			}
		}

		return 0; 
	}

	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

		lpMMI->ptMinTrackSize.x = m_leftRange + m_rightRange + 100;
		lpMMI->ptMinTrackSize.y = m_topRange + m_bottomRange + 200;

		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		U32* dst;
		U32 size, W, H;
		RECT* r;
		int bottom;

		if (SIZE_MINIMIZED == wParam)
			return 0;

		SafeRelease(&m_pD2DRenderTarget);

		GetClientRect(&m_rcSplitter); // update the client area 

		ATLASSERT(0 == m_rcSplitter.left);
		ATLASSERT(0 == m_rcSplitter.top);

		W = (U32)(m_rcSplitter.right - m_rcSplitter.left);
		H = (U32)(m_rcSplitter.bottom - m_rcSplitter.top);

		// because the window size is changed, we need to realloc the screen buffer.
		if (nullptr != m_screenBuff)
			VirtualFree(m_screenBuff, 0, MEM_RELEASE);

		m_screenBuff = nullptr;
		m_screenSize = WOCHAT_ALIGN_PAGE(W * H * sizeof(U32));
		ATLASSERT(m_screenSize >= (W * H * sizeof(U32)));

		m_screenBuff = (U32*)VirtualAlloc(NULL, m_screenSize, MEM_COMMIT, PAGE_READWRITE);
		if (nullptr == m_screenBuff)
		{
			m_screenSize = 0;
			return 0;
		}

		// from here you can see the position relationship of window0/1/2/3/4/5
		// window0 is always visible and only the bottom can be changed.
		m_rect0 = m_rcSplitter;
		r = &m_rect0;
		r->right = XWIN0_WIDTH;
		dst = m_screenBuff;
		m_win0.OnSize(uMsg, wParam, (LPARAM)r, dst);
		size = (U32)((r->right - r->left) * (r->bottom - r->top));

		// calculate the position of window 1
		dst += size;
		r = &m_rect1;
		r->left = XWIN0_WIDTH;
		ATLASSERT(m_xySplitterPosV > 0 && m_xySplitterPosV < m_rcSplitter.right);
		r->right = m_xySplitterPosV;
		r->top = m_rcSplitter.top;
		r->bottom = m_xySplitterPosHfix;
		m_win1.OnSize(uMsg, wParam, (LPARAM)r, dst);
		size = (U32)((r->right - r->left) * (r->bottom - r->top));

		// calculate the position of window 2
		dst += size;
		r = &m_rect2;
		r->left = XWIN0_WIDTH;
		r->right = m_xySplitterPosV;
		r->top = m_xySplitterPosHfix + STROKEWIDTH;
		r->bottom = m_rcSplitter.bottom;
		m_win2.OnSize(uMsg, wParam, (LPARAM)r, dst);
		size = (U32)((r->right - r->left) * (r->bottom - r->top));

		// calculate the position of window 3
		dst += size;
		r = &m_rect3;
		r->left = m_xySplitterPosV + STROKEWIDTH;
		r->right = m_rcSplitter.right;
		r->top = m_rcSplitter.top;
		r->bottom = m_xySplitterPosHfix;
		m_win3.OnSize(uMsg, wParam, (LPARAM)r, dst);
		size = (U32)((r->right - r->left) * (r->bottom - r->top));

		// calculate the position of window 4
		dst += size;
		r = &m_rect4;
		r->left = m_xySplitterPosV + STROKEWIDTH;
		r->right = m_rcSplitter.right;
		r->top = m_xySplitterPosHfix + STROKEWIDTH;

		ATLASSERT(m_rcSplitter.bottom > m_toBottom);
		m_xySplitterPosH = m_rcSplitter.bottom - m_toBottom;
		if (m_win5.IsVisible())
			bottom = m_xySplitterPosH;
		else
			bottom = m_rcSplitter.bottom;

		r->bottom = bottom;
		m_win4.OnSize(uMsg, wParam, (LPARAM)r, dst);
		size = (U32)((r->right - r->left) * (r->bottom - r->top));

		// calculate the position of window 5
		dst += size;
		r = &m_rect5;
		if (m_win5.IsVisible())
		{
			ATLASSERT(m_xySplitterPosH > 0 && m_xySplitterPosH < m_rcSplitter.bottom);
			r->left = m_xySplitterPosV + STROKEWIDTH;
			r->right = m_rcSplitter.right;
			r->top = m_xySplitterPosH + STROKEWIDTH;
			r->bottom = m_rcSplitter.bottom;
		}
		else
		{
			r->left = r->right = r->top = r->bottom = 0;
			dst = nullptr;
		}
		m_win5.OnSize(uMsg, wParam, (LPARAM)r, dst);

		Invalidate();

		return 0;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		U32* dst;
		U32 size;
		RECT* r;
		int needReDraw  = 0;
		int needReDraw0 = 0;
		int needReDraw1 = 0;
		int needReDraw2 = 0;
		int needReDraw3 = 0;
		int needReDraw4 = 0;
		int needReDraw5 = 0;

		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if (::GetCapture() == m_hWnd)  // we are in drag mode
		{
			int xyNewSplitPosV = xPos - m_rcSplitter.left - m_cxyDragOffsetV;
			int	xyNewSplitPosH = yPos - m_rcSplitter.top - m_cxyDragOffsetH;

			if (xyNewSplitPosV == -1)   // avoid -1, that means default position
				xyNewSplitPosV = -2;

			if (xyNewSplitPosH == -1)   // avoid -1, that means default position
				xyNewSplitPosH = -2;

			if (dragModeV == m_dragMode)
			{
				if (m_xySplitterPosV != xyNewSplitPosV)
				{
					if (SetSplitterPos(m_dragMode, xyNewSplitPosV, xyNewSplitPosH, true))
						//UpdateWindow();
						needReDraw = 1;
				}
			}
			if (dragModeH == m_dragMode)
			{
				if (m_xySplitterPosH != xyNewSplitPosH)
				{
					if (SetSplitterPos(m_dragMode, xyNewSplitPosV, xyNewSplitPosH, true))
						//UpdateWindow();
						needReDraw = 1;
				}
			}

			if (needReDraw)  // Adjust the poistion of window1/2/3/4/5. 
			{
				int bottom;

				// window 0 does not need to change
				r = &m_rect0;
				dst = m_screenBuff;
				size = (U32)((r->right - r->left) * (r->bottom - r->top));

				ATLASSERT(m_xySplitterPosV > 0 && m_xySplitterPosV < m_rcSplitter.right);

				// windows 1
				dst += size;
				r = &m_rect1; r->left = XWIN0_WIDTH; r->right = m_xySplitterPosV; r->top = m_rcSplitter.top; r->bottom = m_xySplitterPosHfix;
				size = (U32)((r->right - r->left) * (r->bottom - r->top));
				m_win1.SetPosition(r, dst, size);

				dst += size;
				r = &m_rect2; r->left = XWIN0_WIDTH; r->right = m_xySplitterPosV; r->top = m_xySplitterPosHfix + STROKEWIDTH; r->bottom = m_rcSplitter.bottom;
				size = (U32)((r->right - r->left) * (r->bottom - r->top));
				m_win2.SetPosition(r, dst, size);

				dst += size;
				r = &m_rect3; r->left = m_xySplitterPosV + STROKEWIDTH;  r->right = m_rcSplitter.right; r->top = m_rcSplitter.top; r->bottom = m_xySplitterPosHfix;
				size = (U32)((r->right - r->left) * (r->bottom - r->top));
				m_win3.SetPosition(r, dst, size);

				dst += size;
				r = &m_rect4; r->left = m_xySplitterPosV + STROKEWIDTH;  r->right = m_rcSplitter.right; r->top = m_xySplitterPosHfix + STROKEWIDTH;
				if (m_win5.IsVisible())
				{
					ATLASSERT(m_xySplitterPosH > m_rcSplitter.top && m_xySplitterPosH < m_rcSplitter.bottom);
					bottom = m_xySplitterPosH;
				}
				else
				{
					ATLASSERT(-1 == m_xySplitterPosH);
					bottom = m_rcSplitter.bottom;
				}
				r->bottom = bottom;

				size = (U32)((r->right - r->left) * (r->bottom - r->top));
				m_win4.SetPosition(r, dst, size);

				dst += size;
				r = &m_rect5;
				if (m_win5.IsVisible())
				{
					ATLASSERT(m_xySplitterPosH > m_rcSplitter.top && m_xySplitterPosH < m_rcSplitter.bottom);
					r->left = m_xySplitterPosV + STROKEWIDTH; r->right = m_rcSplitter.right;
					r->top = m_xySplitterPosH + STROKEWIDTH; 	r->bottom = m_rcSplitter.bottom;
				}
				else
				{
					ATLASSERT(-1 == m_xySplitterPosH);
					r->left = r->right = r->top = r->bottom = 0;
					dst = nullptr;
				}
				size = (U32)((r->right - r->left) * (r->bottom - r->top));
				m_win5.SetPosition(r, dst, size);
			}
		}
		else	// not dragging, just set cursor
		{
			m_dragMode = dragModeNone;
			int ret = IsOverSplitterBar(xPos, yPos);

			if (SPLIT_OVER_VERTICAL_BAR == ret)
				::SetCursor(m_cursorSizeWE);
			else if (SPLIT_OVER_HORIZONAL_BAR == ret)
				::SetCursor(m_cursorSizeNS);
		}

		needReDraw0 = m_win0.OnMouseMove(uMsg, wParam, lParam);
		needReDraw1 = m_win1.OnMouseMove(uMsg, wParam, lParam);
		needReDraw2 = m_win2.OnMouseMove(uMsg, wParam, lParam);
		needReDraw3 = m_win3.OnMouseMove(uMsg, wParam, lParam);
		needReDraw4 = m_win4.OnMouseMove(uMsg, wParam, lParam);
		needReDraw5 = m_win5.OnMouseMove(uMsg, wParam, lParam);

		if (needReDraw || needReDraw0 || needReDraw1 || needReDraw2 || needReDraw3 || needReDraw4 || needReDraw5)
		{
			Invalidate();
		}

		return 0;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		int needReDraw0 = m_win0.OnLButtonDown(uMsg, wParam, lParam);
		int needReDraw1 = m_win1.OnLButtonDown(uMsg, wParam, lParam);
		int needReDraw2 = m_win2.OnLButtonDown(uMsg, wParam, lParam);
		int needReDraw3 = m_win3.OnLButtonDown(uMsg, wParam, lParam);
		int needReDraw4 = m_win4.OnLButtonDown(uMsg, wParam, lParam);
		int needReDraw5 = m_win5.OnLButtonDown(uMsg, wParam, lParam);

		int ret = IsOverSplitterBar(xPos, yPos);

		m_dragMode = dragModeNone;
		if (SPLIT_OVER_VERTICAL_BAR == ret)
			m_dragMode = dragModeV;
		else if (SPLIT_OVER_HORIZONAL_BAR == ret)
			m_dragMode = dragModeH;

		if ((::GetCapture() != m_hWnd) && (dragModeNone != m_dragMode))
		{
			m_xySplitterPosVNew = m_xySplitterPosV;
			m_xySplitterPosHNew = m_xySplitterPosH;

			SetCapture();

			if (SPLIT_OVER_VERTICAL_BAR == ret)
				::SetCursor(m_cursorSizeWE);
			else if (SPLIT_OVER_HORIZONAL_BAR == ret)
				::SetCursor(m_cursorSizeNS);

			m_cxyDragOffsetV = xPos - m_rcSplitter.left - m_xySplitterPosV;
			m_cxyDragOffsetH = yPos - m_rcSplitter.top - m_xySplitterPosH;
		}
		else if ((::GetCapture() == m_hWnd) && (dragModeNone == m_dragMode))
		{
			::ReleaseCapture();
		}

		if (needReDraw0 || needReDraw1 || needReDraw2 || needReDraw3 || needReDraw4 || needReDraw5)
			Invalidate();

		return 0;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		int needReDraw0 = m_win0.OnLButtonUp(uMsg, wParam, lParam);
		int needReDraw1 = m_win1.OnLButtonUp(uMsg, wParam, lParam);
		int needReDraw2 = m_win2.OnLButtonUp(uMsg, wParam, lParam);
		int needReDraw3 = m_win3.OnLButtonUp(uMsg, wParam, lParam);
		int needReDraw4 = m_win4.OnLButtonUp(uMsg, wParam, lParam);
		int needReDraw5 = m_win5.OnLButtonUp(uMsg, wParam, lParam);

		m_dragMode = dragModeNone;

		if (::GetCapture() == m_hWnd)
		{
			m_xySplitterPosVNew = m_xySplitterPosV;
			m_xySplitterPosHNew = m_xySplitterPosH;
			::ReleaseCapture();
		}

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
		if ((m_xySplitterPosVNew != -1) && (m_xySplitterPosV != m_xySplitterPosVNew))
		{
			m_xySplitterPosV = m_xySplitterPosVNew;
			m_xySplitterPosVNew = -1;
			Invalidate();
		}
		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
#if 0
		if (::GetCapture() == m_hWnd)
		{
			switch (wParam)
			{
			case VK_RETURN:
				m_xySplitterPosVNew = m_xySplitterPosV;
				m_xySplitterPosHNew = m_xySplitterPosH;
				// FALLTHROUGH
			case VK_ESCAPE:
				::ReleaseCapture();
				break;
			case VK_LEFT:
			case VK_RIGHT:
				if (dragModeV == m_dragMode)
				{
					POINT pt = { 0 };
					::GetCursorPos(&pt);
					int xyPos = m_xySplitterPosV + ((wParam == VK_LEFT) ? -MOVESTEP : MOVESTEP);
					int cxyMax = m_rcSplitter.right - m_rcSplitter.left;
					if (xyPos < m_cxyMin)
						xyPos = m_cxyMin;
					else if (xyPos > (cxyMax - STROKEWIDTH - m_cxyMin))
						xyPos = cxyMax - STROKEWIDTH - m_cxyMin;

					if (xyPos < m_leftRange)
						xyPos = m_leftRange;
					if (xyPos > m_rcSplitter.right - m_rightRange)
						xyPos = m_rcSplitter.right - m_rightRange;

					pt.x += xyPos - m_xySplitterPosV;
					::SetCursorPos(pt.x, pt.y);
				}
				break;
			case VK_UP:
			case VK_DOWN:
				if (dragModeH == m_dragMode)
				{
					POINT pt = { 0 };
					::GetCursorPos(&pt);
					int xyPos = m_xySplitterPosH + ((wParam == VK_UP) ? -MOVESTEP : MOVESTEP);
					int cxyMax = m_rcSplitter.bottom - m_rcSplitter.top;
					if (xyPos < m_cxyMin)
						xyPos = m_cxyMin;
					else if (xyPos > (cxyMax - STROKEWIDTH - m_cxyMin))
						xyPos = cxyMax - STROKEWIDTH - m_cxyMin;
					if (xyPos < m_topRange)
						xyPos = m_topRange;
					if (xyPos > m_rcSplitter.bottom - m_bottomRange)
						xyPos = m_rcSplitter.bottom - m_bottomRange;
					pt.y += xyPos - m_xySplitterPosH;
					::SetCursorPos(pt.x, pt.y);
				}
				break;
			default:
				break;
			}

			Invalidate();
		}
#endif
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
		HRESULT hr = S_OK;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(&ps);

		if (nullptr == m_screenBuff || 0 == m_screenSize)
		{
			EndPaint(&ps);
			return 0;
		}

		// re-create the device dependent resouce if it is failed.
		if (nullptr == m_pD2DRenderTarget)
		{
			D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);

			D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
			renderTargetProperties.dpiX = g_DotsPerInch;
			renderTargetProperties.dpiY = g_DotsPerInch;
			renderTargetProperties.pixelFormat = pixelFormat;

			D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRenderTragetproperties
				= D2D1::HwndRenderTargetProperties(m_hWnd, D2D1::SizeU(m_rcSplitter.right - m_rcSplitter.left, m_rcSplitter.bottom - m_rcSplitter.top));

			hr = g_pD2DFactory->CreateHwndRenderTarget(renderTargetProperties, hwndRenderTragetproperties, &m_pD2DRenderTarget);
			if (S_OK == hr && nullptr != m_pD2DRenderTarget)
			{
				U32 pixel[1] = { DEFAULT_SEPERATELINE_COLOR };
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
			RECT* r = nullptr;

			m_pD2DRenderTarget->BeginDraw();
			////////////////////////////////////////////////////////////////////////////////////////////////////
			m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			// draw seperator lines
			if (nullptr != m_pixelBitmap)
			{
				ATLASSERT(m_xySplitterPosV > 0 && m_xySplitterPosV < m_rcSplitter.right);
				// draw the vertical bar line
				D2D1_RECT_F rect0 = D2D1::RectF(
					static_cast<FLOAT>(m_xySplitterPosV),
					static_cast<FLOAT>(m_rcSplitter.top),
					static_cast<FLOAT>(m_xySplitterPosV + STROKEWIDTH),
					static_cast<FLOAT>(m_rcSplitter.bottom)
				);
				m_pD2DRenderTarget->DrawBitmap(m_pixelBitmap, &rect0);

				// draw the flexiable horizonal bar line between Win4 and Win5
				if (m_xySplitterPosH > 0)
				{
					D2D1_RECT_F rect1 = D2D1::RectF(
						static_cast<FLOAT>(m_xySplitterPosV + STROKEWIDTH),
						static_cast<FLOAT>(m_xySplitterPosH),
						static_cast<FLOAT>(m_rcSplitter.right),
						static_cast<FLOAT>(m_xySplitterPosH + STROKEWIDTH)
					);
					m_pD2DRenderTarget->DrawBitmap(m_pixelBitmap, &rect1);
				}

				if (m_xySplitterPosHfix > 0)
				{
					D2D1_RECT_F rect2 = D2D1::RectF(
						static_cast<FLOAT>(XWIN0_WIDTH),
						static_cast<FLOAT>(m_xySplitterPosHfix),
						static_cast<FLOAT>(m_rcSplitter.right),
						static_cast<FLOAT>(m_xySplitterPosHfix + STROKEWIDTH)
					);
					m_pD2DRenderTarget->DrawBitmap(m_pixelBitmap, &rect2);
				}
			}

			///////////////////////////////// Draw the 6 virtual windows ///////////////////////////////////////////////
			// draw window 0
			src = m_win0.UpdateScreen();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				r = &m_rect0; w = r->right - r->left; h = r->bottom - r->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(r->left), static_cast<FLOAT>(r->top), static_cast<FLOAT>(r->right), static_cast<FLOAT>(r->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			// draw window 1
			src = m_win1.UpdateScreen();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				r = &m_rect1; w = r->right - r->left; h = r->bottom - r->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(r->left), static_cast<FLOAT>(r->top), static_cast<FLOAT>(r->right), static_cast<FLOAT>(r->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			// draw window 2
			src = m_win2.UpdateScreen();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				r = &m_rect2; w = r->right - r->left; h = r->bottom - r->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(r->left), static_cast<FLOAT>(r->top), static_cast<FLOAT>(r->right), static_cast<FLOAT>(r->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			// draw window 3
			src = m_win3.UpdateScreen();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				r = &m_rect3; w = r->right - r->left; h = r->bottom - r->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(r->left), static_cast<FLOAT>(r->top), static_cast<FLOAT>(r->right), static_cast<FLOAT>(r->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			// draw window 4
			src = m_win4.UpdateScreen();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				r = &m_rect4; w = r->right - r->left; h = r->bottom - r->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(r->left), static_cast<FLOAT>(r->top), static_cast<FLOAT>(r->right), static_cast<FLOAT>(r->bottom));
					m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
				}
				SafeRelease(&pBitmap);
			}

			// draw window 5
			src = m_win5.UpdateScreen();
			if (nullptr != src)
			{
				ID2D1Bitmap* pBitmap = nullptr;
				r = &m_rect5; w = r->right - r->left; h = r->bottom - r->top;
				hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
				if (S_OK == hr && nullptr != pBitmap)
				{
					D2D1_RECT_F rect = D2D1::RectF(static_cast<FLOAT>(r->left), static_cast<FLOAT>(r->top), static_cast<FLOAT>(r->right), static_cast<FLOAT>(r->bottom));
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

	bool IsOverSplitterRect(int x, int y) const
	{
		// -1 == don't check
		return (((x == -1) || ((x >= m_rcSplitter.left) && (x <= m_rcSplitter.right))) &&
			((y == -1) || ((y >= m_rcSplitter.top) && (y <= m_rcSplitter.bottom))));
	}

	int IsOverSplitterBar(int x, int y) const
	{
		int xyV = x;
		int xyH = y;
		int xyOffV = m_rcSplitter.left;
		int xyOffH = m_rcSplitter.top;

		if ((xyV >= (xyOffV + m_xySplitterPosV)) && (xyV < (xyOffV + m_xySplitterPosV + STROKEWIDTH)))
			return SPLIT_OVER_VERTICAL_BAR;

		if ((xyV >= (xyOffV + m_xySplitterPosV + STROKEWIDTH)) &&
			(xyH >= (xyOffH + m_xySplitterPosH)) && (xyH < (xyOffH + m_xySplitterPosH + STROKEWIDTH)))
			return SPLIT_OVER_HORIZONAL_BAR;

		return SPLIT_OVER_NONE_BAR;
	}

	bool SetSplitterPos(int dragMode, int xPos = -1, int yPos = -1, bool bUpdate = true)
	{
		int cxyMaxV = m_rcSplitter.right - m_rcSplitter.left;
		int cxyMaxH = m_rcSplitter.bottom - m_rcSplitter.top;
		bool bRetV;
		bool bRetH;

		if (xPos == -1)   // -1 == default position
		{
			if (m_xySplitterDefPosV != -1)
			{
				xPos = m_xySplitterDefPosV;
			}
			else   // not set, use middle position
			{
				xPos = (m_rcSplitter.right - m_rcSplitter.left - STROKEWIDTH) / 2;
			}
		}

		if (yPos == -1)   // -1 == default position
		{
			if (m_xySplitterDefPosH != -1)
			{
				yPos = m_xySplitterDefPosH;
			}
			else   // not set, use middle position
			{
				yPos = (m_rcSplitter.bottom - m_rcSplitter.top - STROKEWIDTH) / 2;
			}
		}

		// Adjust if out of valid range
		if (xPos < m_cxyMin)
			xPos = m_cxyMin;
		else if (xPos > (cxyMaxV - STROKEWIDTH - m_cxyMin))
			xPos = cxyMaxV - STROKEWIDTH - m_cxyMin;

		if (yPos < m_cxyMin)
			yPos = m_cxyMin;
		else if (yPos > (cxyMaxH - STROKEWIDTH - m_cxyMin))
			yPos = cxyMaxH - STROKEWIDTH - m_cxyMin;

		if (xPos < m_leftRange)
			xPos = m_leftRange;
		if (xPos > cxyMaxV - m_rightRange)
			xPos = cxyMaxV - m_rightRange;

		if (yPos < m_topRange)
			yPos = m_topRange;

		if (yPos > cxyMaxH - m_bottomRange)
			yPos = cxyMaxH - m_bottomRange;

		// Set new position and update if requested
		bRetV = (m_xySplitterPosV != xPos);
		bRetH = (m_xySplitterPosH != yPos);

		if (dragModeV == dragMode)
			m_xySplitterPosV = xPos;

		if (dragModeH == dragMode)
		{
			m_xySplitterPosH = yPos;
			m_toBottom = m_rcSplitter.bottom - m_xySplitterPosH;
		}

		if (bUpdate && (bRetV || bRetH))
			Invalidate();

		return (bRetV || bRetH);
	}
};

#endif /* __WOCHAT_XWINDOW_H__ */