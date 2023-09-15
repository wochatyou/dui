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
//#include <winsock2.h>
#include <memoryapi.h>

#include <cassert>
#include <string>

#define S8      int8_t
#define S16     int16_t
#define S32     int32_t
#define S64     int64_t

#define U8      uint8_t
#define U16     uint16_t
#define U32     uint32_t
#define U64     uint64_t

/* DUI_ALIGN() is only to be used to align on a power of 2 boundary */
#define DUI_ALIGN(size, boundary) (((size) + ((boundary) -1)) & ~((boundary) - 1))
#define DUI_ALIGN_DEFAULT32(size)	DUI_ALIGN(size, 4)
#define DUI_ALIGN_DEFAULT64(size)	DUI_ALIGN(size, 8)		/** Default alignment */
#define DUI_ALIGN_PAGE(size)		    DUI_ALIGN(size, 1<<16)

extern UINT				g_Quit;
extern LONG				g_threadCount;
extern HINSTANCE		g_hInstance;

#endif  /* __DUIAPP_H__ */

