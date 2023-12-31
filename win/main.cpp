#include "duiapp.h"
#include "xbitmapdata.h"
#include "resource.h"
#include "xwindef.h"
#include "xwinlogin.h"
#include "xwindow.h"


#if !defined(_WIN64)
#error DUIAPP can only compiled in X64 mode
#endif

#define XWIN_DEFAULT_DPI	96
///////////////////////////////////////////////////
// global variables
LONG 				g_threadCount = 0;
UINT				g_Quit = 0;
HINSTANCE			g_hInstance = nullptr;
FT_Library			g_ftLibrary = nullptr;
FT_Face				g_ftFace0 = nullptr;
FT_Face				g_ftFace1 = nullptr;
FT_Face				g_ftFace2 = nullptr;
ID2D1Factory*       g_pD2DFactory = nullptr;

HCURSOR g_hCursorWE    = nullptr;
HCURSOR g_hCursorNS    = nullptr;
HCURSOR g_hCursorHand  = nullptr;
HCURSOR g_hCursorIBeam = nullptr;


static CAtlWinModule _Module;

class CDUIThreadManager
{
public:
	// the main UI thread proc
	static DWORD WINAPI RunThread(LPVOID lpData)
	{
		DWORD dwRet = 0;
		RECT rw = { 0 };
		MSG msg = { 0 };

		InterlockedIncrement(&g_threadCount);

		XWindow winMain;

		rw.left = 100; rw.top = 100; rw.right = rw.left + 1024; rw.bottom = rw.top + 768;
		winMain.Create(NULL, rw, _T("WoChat"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE);
		if (FALSE == winMain.IsWindow())
			goto ExitMainUIThread;

		//winMain.CenterWindow();
		winMain.ShowWindow(SW_SHOW);

		// Main message loop:
		while (GetMessageW(&msg, nullptr, 0, 0))
		{
			if (!TranslateAcceleratorW(msg.hwnd, nullptr, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}

	ExitMainUIThread:
		InterlockedDecrement(&g_threadCount);
		return dwRet;
	}

	DWORD m_dwCount = 0;
	HANDLE m_arrThreadHandles[MAXIMUM_WAIT_OBJECTS - 1];

	CDUIThreadManager()
	{ 
		for (int i = 0; i < (MAXIMUM_WAIT_OBJECTS - 1); i++)
			m_arrThreadHandles[i] = nullptr;
	}

	// Operations
	DWORD AddThread(LPTSTR lpstrCmdLine, int nCmdShow)
	{
		if (m_dwCount == (MAXIMUM_WAIT_OBJECTS - 1))
		{
			::MessageBox(NULL, _T("ERROR: Cannot create ANY MORE threads!!!"), _T("WoChat"), MB_OK);
			return 0;
		}

		DWORD dwThreadID;
		HANDLE hThread = ::CreateThread(NULL, 0, RunThread, nullptr, 0, &dwThreadID);
		if (hThread == NULL)
		{
			::MessageBox(NULL, _T("ERROR: Cannot create thread!!!"), _T("WoChat"), MB_OK);
			return 0;
		}

		m_arrThreadHandles[m_dwCount] = hThread;
		m_dwCount++;
		return dwThreadID;
	}

	void RemoveThread(DWORD dwIndex)
	{
		::CloseHandle(m_arrThreadHandles[dwIndex]);
		if (dwIndex != (m_dwCount - 1))
			m_arrThreadHandles[dwIndex] = m_arrThreadHandles[m_dwCount - 1];
		m_dwCount--;
	}

	int Run(LPTSTR lpstrCmdLine, int nCmdShow)
	{
		MSG msg;
		// force message queue to be created
		::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

		AddThread(lpstrCmdLine, nCmdShow);

		int nRet = m_dwCount;
		DWORD dwRet;
		while (m_dwCount > 0)
		{
			dwRet = ::MsgWaitForMultipleObjects(m_dwCount, m_arrThreadHandles, FALSE, INFINITE, QS_ALLINPUT);

			if (dwRet == 0xFFFFFFFF)
			{
				::MessageBox(NULL, _T("ERROR: Wait for multiple objects failed!!!"), _T("WoChat"), MB_OK);
			}
			else if (dwRet >= WAIT_OBJECT_0 && dwRet <= (WAIT_OBJECT_0 + m_dwCount - 1))
			{
				RemoveThread(dwRet - WAIT_OBJECT_0);
			}
			else if (dwRet == (WAIT_OBJECT_0 + m_dwCount))
			{
				if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_USER)
						AddThread(nullptr, SW_SHOWNORMAL);
				}
			}
			else
			{
				::MessageBeep((UINT)-1);
			}
		}

		return nRet;
	}
};


static int InitInstance(HINSTANCE hInstance)
{
	int iRet = 0;
	BYTE* fontData;
	DWORD fontSize;
	HRSRC  res;
	HGLOBAL res_handle;
	HRESULT hr = S_OK;

	g_hCursorWE    = ::LoadCursor(NULL, IDC_SIZEWE);
	g_hCursorNS    = ::LoadCursor(NULL, IDC_SIZENS);
	g_hCursorHand  = ::LoadCursor(nullptr, IDC_HAND);
	g_hCursorIBeam = ::LoadCursor(NULL, IDC_IBEAM);

	if (NULL == g_hCursorWE || NULL == g_hCursorNS || NULL == g_hCursorHand || NULL == g_hCursorIBeam)
	{
		MessageBox(NULL, _T("The calling of LoadCursor() is failed"), _T("DUI Error"), MB_OK);
		return (-1);
	}

	g_pD2DFactory = nullptr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
	if (S_OK != hr || nullptr == g_pD2DFactory)
	{
		MessageBox(NULL, _T("The calling of D2D1CreateFactory() is failed"), _T("DUI Error"), MB_OK);
		return (-1);
	}

	/* load the build-in font file(*.ttf) */
	res = FindResource(hInstance, MAKEINTRESOURCE(IDR_DEFAULTFONT), RT_RCDATA);
	if (NULL == res)
	{
		MessageBox(NULL, _T("Cannot find the default font resource within the exe file!"), _T("Error"), MB_OK);
		return (-1);
	}

	res_handle = LoadResource(hInstance, res);
	if (NULL == res_handle)
	{
		MessageBox(NULL, _T("Cannot load the default font resource within the exe file!"), _T("Error"), MB_OK);
		return (-1);
	}

	fontData = (BYTE*)LockResource(res_handle);
	fontSize = SizeofResource(hInstance, res);
	if (NULL == fontData || 0 == fontSize)
	{
		MessageBox(NULL, _T("Cannot lock the default font resource within the exe file!"), _T("Error"), MB_OK);
		return (-1);
	}

	{
		FT_Error ft_error;
		g_ftLibrary = nullptr;
		g_ftFace0 = nullptr;
		g_ftFace1 = nullptr;

		ft_error = FT_Init_FreeType(&g_ftLibrary);
		if (ft_error || nullptr == g_ftLibrary)
		{
			MessageBox(NULL, _T("FT_Init_FreeType call is faied!"), _T("Error"), MB_OK);
			return (-1);
		}

		ft_error = FT_New_Memory_Face(g_ftLibrary, (const FT_Byte*)fontData, fontSize, 0, &g_ftFace0);
		if (ft_error || nullptr == g_ftFace0)
		{
			FT_Done_FreeType(g_ftLibrary);
			MessageBox(NULL, _T("FT_New_Memory_Face call is faied!"), _T("Error"), MB_OK);
			return (-2);
		}

		ft_error = FT_Set_Char_Size(g_ftFace0, XFONT_SIZE0 * 64, XFONT_SIZE0 * 64, 0, 0);
		if (ft_error)
		{
			MessageBox(NULL, _T("FT_Set_Char_Size call is faied!"), _T("Error"), MB_OK);
			FT_Done_Face(g_ftFace0);
			FT_Done_FreeType(g_ftLibrary);
			return (-3);
		}
		ft_error = FT_New_Memory_Face(g_ftLibrary, (const FT_Byte*)fontData, fontSize, 0, &g_ftFace1);
		if (ft_error || nullptr == g_ftFace1)
		{
			FT_Done_Face(g_ftFace0);
			FT_Done_FreeType(g_ftLibrary);
			MessageBox(NULL, _T("FT_New_Memory_Face call is faied!"), _T("Error"), MB_OK);
			return (-2);
		}

		ft_error = FT_Set_Char_Size(g_ftFace1, XFONT_SIZE1 * 64, XFONT_SIZE1 * 64, 0, 0);
		if (ft_error)
		{
			MessageBox(NULL, _T("FT_Set_Char_Size call is faied!"), _T("Error"), MB_OK);
			FT_Done_Face(g_ftFace0);
			FT_Done_Face(g_ftFace1);
			FT_Done_FreeType(g_ftLibrary);
			return (-3);
		}
	}

	/* load the build-in font file(*.ttf) */
	res = FindResource(hInstance, MAKEINTRESOURCE(IDR_ASCIIFONT), RT_RCDATA);
	if (NULL == res)
	{
		MessageBox(NULL, _T("Cannot find the default font resource within the exe file!"), _T("Error"), MB_OK);
		return (-1);
	}
	res_handle = LoadResource(hInstance, res);
	if (NULL == res_handle)
	{
		MessageBox(NULL, _T("Cannot load the default font resource within the exe file!"), _T("Error"), MB_OK);
		return (-1);
	}
	fontData = (BYTE*)LockResource(res_handle);
	fontSize = SizeofResource(hInstance, res);
	if (NULL == fontData || 0 == fontSize)
	{
		MessageBox(NULL, _T("Cannot lock the default font resource within the exe file!"), _T("Error"), MB_OK);
		return (-1);
	}

	{
		FT_Error ft_error;
		g_ftFace2 = nullptr;

		ft_error = FT_New_Memory_Face(g_ftLibrary, (const FT_Byte*)fontData, fontSize, 0, &g_ftFace2);
		if (ft_error || nullptr == g_ftFace2)
		{
			FT_Done_Face(g_ftFace0);
			FT_Done_Face(g_ftFace1);
			FT_Done_FreeType(g_ftLibrary);
			MessageBox(NULL, _T("FT_New_Memory_Face call is faied!"), _T("Error"), MB_OK);
			return (-2);
		}
		ft_error = FT_Set_Char_Size(g_ftFace2, XFONT_SIZE0 * 64, XFONT_SIZE0 * 64, 0, 0);
		if (ft_error)
		{
			FT_Done_Face(g_ftFace0);
			FT_Done_Face(g_ftFace1);
			FT_Done_Face(g_ftFace2);
			FT_Done_FreeType(g_ftLibrary);
			MessageBox(NULL, _T("FT_Set_Char_Size call is faied!"), _T("Error"), MB_OK);
			return (-3);
		}
	}

	iRet = DUI_Init();

	return iRet;
}

static void ExitInstance(HINSTANCE hInstance)
{
	UINT i, tries;

	// tell all threads to quit
	InterlockedIncrement(&g_Quit);

	DUI_Term();

	assert(nullptr != g_pD2DFactory);
	SafeRelease(&g_pD2DFactory);
	g_pD2DFactory = nullptr;

	if (nullptr != g_ftFace1)
	{
		FT_Done_Face(g_ftFace1);
		g_ftFace1 = nullptr;
	}

	if (nullptr != g_ftFace0)
	{
		FT_Done_Face(g_ftFace0);
		g_ftFace0 = nullptr;
	}

	if (nullptr != g_ftLibrary)
	{
		FT_Done_FreeType(g_ftLibrary);
		g_ftLibrary = nullptr;
	}

	// wait for all threads to quit gracefully
	tries = 10;
	while (g_threadCount && tries) 
	{
		Sleep(1000);
		tries--;
	}

}

int DoLogin()
{
	int ret;
	XWinLogin loginWin;
	RECT rw = { 0 };
	MSG msg = { 0 };

	rw.left = 0; rw.top = 0; rw.right = rw.left + 400; rw.bottom = rw.top + 300;

	loginWin.Create(NULL, rw, _T("WoChat Login"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_VISIBLE);
	if (FALSE == loginWin.IsWindow())
		return 1;

	loginWin.CenterWindow();
	loginWin.ShowWindow(SW_SHOW);

	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		if (!TranslateAcceleratorW(msg.hwnd, nullptr, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	ret = loginWin.IsSuccessful() ? 0 : 1;

	return ret;
}

int WINAPI  _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	int iRet = 0;
	HRESULT hr;

	g_Quit = 0;
	g_hInstance = hInstance;  // save the instance

	UNREFERENCED_PARAMETER(hPrevInstance);

	// The Microsoft Security Development Lifecycle recommends that all
	// applications include the following call to ensure that heap corruptions
	// do not go unnoticed and therefore do not introduce opportunities
	// for security exploits.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
		
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(S_OK != hr) 
		return 0;

	iRet = InitInstance(hInstance);
	if (0 != iRet)
		goto ExitThisApplication;

	iRet = DoLogin();
	if (0 != iRet)
		goto ExitThisApplication;

	// BLOCK: Run application
	{
		CDUIThreadManager mgr;
		iRet = mgr.Run(lpCmdLine, nShowCmd);
	}

ExitThisApplication:
	ExitInstance(hInstance);

	CoUninitialize();

	return iRet;
}

