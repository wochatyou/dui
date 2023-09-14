#ifndef __WOCHAT_H__
#define __WOCHAT_H__

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
#include <dwrite.h>
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

#include "akelpad/AkelBuild.h"
#include "dui/duiscreen.h"
#include "blend2d/src/blend2d.h"
#include "utils/mempool.h"

#define S8      int8_t
#define S16     int16_t
#define S32     int32_t
#define S64     int64_t

#define U8      uint8_t
#define U16     uint16_t
#define U32     uint32_t
#define U64     uint64_t

/* WOCHAT_ALIGN() is only to be used to align on a power of 2 boundary */
#define WOCHAT_ALIGN(size, boundary) (((size) + ((boundary) -1)) & ~((boundary) - 1))
#define WOCHAT_ALIGN_DEFAULT32(size)	WOCHAT_ALIGN(size, 4)
#define WOCHAT_ALIGN_DEFAULT64(size)	WOCHAT_ALIGN(size, 8)		/** Default alignment */
#define WOCHAT_ALIGN_PAGE(size)		    WOCHAT_ALIGN(size, 1<<16)

template <class T> void SafeRelease(T** ppT)
{
	if (nullptr != *ppT)
	{
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

typedef union XTimeStamp
{
	U8   data_[8];
	U64  timestamp_;  // the time stamp. 
} XTimeStamp;

typedef struct XChatMessage
{
	XChatMessage* next_;
	U32* icon_;        // the bitmap data of this icon
	U8   w_;           // the width in pixel of this icon
	U8   h_;           // the height in pixel of this icon
	U16  height_;	   // in pixel
	U16  state_;
	U16  msgLen_;
	XTimeStamp  ts_;  // the time stamp. 
	U16* name_;       // The name of this people      
	U16* message_;    // real message
	U8* obj_;         // point to GIF/APNG/Video/Pic etc
} XChatMessage;

typedef struct XChatGroup
{
	XChatGroup* next_;
	U16  id_;				// Group ID
	U32* icon_;				// the bitmap data of this icon
	U8   w_;				// the width in pixel of this icon
	U8   h_;				// the height in pixel of this icon
	U16  height_;			// in pixel
	U16  unread_;			// how many unread messages? if more than 254, use ... 
	U16  member_;			// how many members in this group?
	U16* name_;				// the group name
	XTimeStamp  ts_;		// the time stamp. 
	U16* lastmsg_;			// the last message of this group
	XChatMessage* message_; // The list of the message in this group
	XChatMessage* msgTail_; // The last message in this group;
	MemoryContext mempool_; // The memory pool of this group
} XChatGroup;

typedef struct XMQTTMessage
{
	char* host;
	int port;
	char* topic;
	char* message;
	int msglen;
} XMQTTMessage;

typedef enum
{
	modeMe = 0,
	modeTalk,
	modeFriend,
	modeQuan,
	modeCoin,
	modeFavorite,
	modeFile,
	modeSetting
} AppMode;

extern UINT				g_Quit;
extern UINT				g_DotsPerInch;
extern LONG				g_threadCount;
extern ID2D1Factory*	g_pD2DFactory;
extern BLFontFace		g_fontFace;
extern LOGFONTW         g_logFont;
extern HINSTANCE		g_hInstance;

#endif  /* __WOCHAT_H__ */

