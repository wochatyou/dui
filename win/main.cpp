
#include "duiapp.h"
#include "xbitmapdata.h"
#include "resource.h"
#include "xwindef.h"
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
BLFontFace			g_fontFace;

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
		BLResult blRet;
		BLFontData fd;
		BLString fontName;
		blRet = fd.createFromData(fontData, fontSize);
		if (BL_SUCCESS != blRet)
		{
			MessageBox(NULL, _T("Cannot create font data the default font resource within the exe file!"), _T("Error"), MB_OK);
			return (-1);
		}
		blRet = blFontFaceInit(&g_fontFace);
		if (BL_SUCCESS != blRet)
		{
			MessageBox(NULL, _T("Cannot Initialize g_fontFace!"), _T("WoChat Error"), MB_OK);
			return (-1);
		}
		blRet = g_fontFace.createFromData(fd, 0);
		if (BL_SUCCESS != blRet)
		{
			MessageBox(NULL, _T("Cannot create font face the default font resource within the exe file!"), _T("Error"), MB_OK);
			return (-1);
		}
		fontName = g_fontFace.familyName();
		fontName = g_fontFace.fullName();
		fontName = g_fontFace.subfamilyName();
	}


	return iRet;
}

static void ExitInstance(HINSTANCE hInstance)
{
	UINT i, tries;

	// tell all threads to quit
	InterlockedIncrement(&g_Quit);

	// wait for all threads to quit gracefully
	tries = 10;
	while (g_threadCount && tries) 
	{
		Sleep(1000);
		tries--;
	}

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

