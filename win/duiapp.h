#ifndef __DUIAPP_H__
#define __DUIAPP_H__

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif

#include <stdint.h>
#define _CRT_RAND_S
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <tchar.h>
#include <shellapi.h>

#include <atlbase.h>
#include <atlwin.h>

#include <dwmapi.h>
#include <d2d1.h>
#include <d2d1helper.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "bcrypt.lib")

#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <memoryapi.h>

#include <cassert>
#include <string>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>

//#include "blend2d/src/blend2d.h"

extern UINT				g_Quit;
extern LONG				g_threadCount;
extern HINSTANCE		g_hInstance;
extern ID2D1Factory*    g_pD2DFactory;

extern HCURSOR g_hCursorWE;
extern HCURSOR g_hCursorNS;
extern HCURSOR g_hCursorHand;
extern HCURSOR g_hCursorIBeam;


#define XFONT_SIZE0		16
#define XFONT_SIZE1		13

extern FT_Library		g_ftLibrary;
extern FT_Face			g_ftFace0;
extern FT_Face			g_ftFace1;

#endif  /* __DUIAPP_H__ */

