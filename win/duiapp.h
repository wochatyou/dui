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

#include "blend2d/src/blend2d.h"

extern UINT				g_Quit;
extern LONG				g_threadCount;
extern HINSTANCE		g_hInstance;
extern BLFontFace		g_fontFace;

#endif  /* __DUIAPP_H__ */

