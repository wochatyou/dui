#ifndef __DUI_H__
#define __DUI_H__

#include <stdint.h>

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
#define DUI_ALIGN_PAGE(size)		DUI_ALIGN(size, 1<<16)


int ScreenClear(uint32_t* dst, uint32_t size, uint32_t color);

int ScreenDrawHLine(uint32_t* dst, int w, int h, int position, int stroke, uint32_t color);

int ScreenDrawRect(uint32_t* dst, int w, int h, uint32_t* src, int sw, int sh, int dx, int dy);

int ScreenFillRect(uint32_t* dst, int w, int h, uint32_t color, int sw, int sh, int dx, int dy);

int ScreenDrawRectRound(uint32_t* dst, int w, int h, uint32_t* src, int sw, int sh, int dx, int dy, uint32_t color);

int ScreenFillRectRound(uint32_t* dst, int w, int h, uint32_t color, int sw, int sh, int dx, int dy, uint32_t c1, uint32_t c2);


#endif // __DUI_H__

