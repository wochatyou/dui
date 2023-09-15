#include "dui.h"

/* fill the whole screen with one color */
int ScreenClear(uint32_t* dst, uint32_t size, uint32_t color)
{
	uint64_t newColor = (uint64_t)(color);
	newColor <<= 32;
	newColor |= (uint64_t)color;

	uint64_t* p64 = (uint64_t*)dst;
	for (uint32_t i = 0; i < (size >> 1); i++)
		*p64++ = newColor;

	if (1 & size)  // fix the last pixel if the whole size is not even number
	{
		uint32_t* p32 = dst + (size - 1);
		*p32 = color;
	}

	return 0;
}

int ScreenDrawRect(uint32_t* dst, int w, int h, uint32_t* src, int sw, int sh, int dx, int dy)
{
	uint32_t* startDST;
	uint32_t* startSRC;
	uint32_t* p;
	int SW, SH;

	if (dx >= w || dy >= h) // not in the scope
		return 0;

	if (dy < 0)
	{
		src += ((-dy) * sw);
		sh += dy;
		dy = 0;
	}

	SW = sw;
	SH = sh;

	if (sw + dx > w)
		SW = w - dx;
	if (sh + dy > h)
		SH = h - dy;

	for (int i = 0; i < SH; i++)
	{
		startDST = dst + w * (dy + i) + dx;
		startSRC = src + i * sw;
		for (int k = 0; k < SW; k++)
			*startDST++ = *startSRC++;
	}

	return 0;
}

int ScreenDrawRectRound(uint32_t* dst, int w, int h, uint32_t* src, int sw, int sh, int dx, int dy, uint32_t color)
{
	uint32_t* startDST;
	uint32_t* startSRC;
	int SW, SH;
	int normal = 1;

	if (dx >= w || dy >= h) // not in the scope
		return 0;

	if (dy < 0)
	{
		src += ((-dy) * sw);
		sh += dy;
		dy = 0;
		normal = 0;
	}

	SW = sw;
	SH = sh;

	if (sw + dx > w)
	{
		SW = w - dx;
		normal = 0;
	}
	if (sh + dy > h)
	{
		SH = h - dy;
		normal = 0;
	}

	for (int i = 0; i < SH; i++)
	{
		startDST = dst + w * (dy + i) + dx;
		startSRC = src + i * sw;
		for (int k = 0; k < SW; k++)
			*startDST++ = *startSRC++;
	}

	if (0 != normal)
	{
		uint32_t* p = dst + w * dy + dx;
		*p = color; *(p + 1) = color;
		p += (sw - 2);
		*p++ = color; *p = color;
		p = dst + w * (dy + 1) + dx;
		*p = color;
		p += (sw - 1);
		*p = color;

		p = dst + w * (dy + sh - 2) + dx;
		*p = color;
		p += (sw - 1);
		*p = color;
		p = dst + w * (dy + sh - 1) + dx;
		*p = color; *(p + 1) = color;
		p += (sw - 2);
		*p++ = color; *p = color;
	}

	return 0;
}

int ScreenFillRect(uint32_t* dst, int w, int h, uint32_t color, int sw, int sh, int dx, int dy)
{
	uint32_t* startDST;
	uint32_t* p;
	int SW, SH;

	if (dx >= w || dy >= h) // not in the scope
		return 0;
	if (dy < 0)
	{
		sh += dy;
		dy = 0;
	}

	SW = sw;
	SH = sh;

	if (sw + dx > w)
		SW = w - dx;
	if (sh + dy > h)
		SH = h - dy;

	for (int i = 0; i < SH; i++)
	{
		startDST = dst + w * (dy + i) + dx;
		for (int k = 0; k < SW; k++)
			*startDST++ = color;
	}

	return 0;
}

int ScreenFillRectRound(uint32_t* dst, int w, int h, uint32_t color, int sw, int sh, int dx, int dy, uint32_t c1, uint32_t c2)
{
	uint32_t* startDST;
	uint32_t* p;
	int SW = sw;
	int SH = sh;

	if (dx >= w || dy >= h) // not in the scope
		return 0;
	if (sw + dx > w)
		SW = w - dx;
	if (sh + dy > h)
		SH = h - dy;

	for (int i = 0; i < SH; i++)
	{
		startDST = dst + w * (dy + i) + dx;
		for (int k = 0; k < SW; k++)
			*startDST++ = color;
	}
	// Fix the round coner
	startDST = dst + w * dy + dx;
	p = startDST;
	*p++ = c2; *p = c1;
	p = startDST + sw - 2;
	*p++ = c1; *p++ = c2;
	p = startDST + w; *p = c1;
	p += sw - 1; *p = c1;

	p = startDST + w * (sh - 2);
	*p = c1;
	p += sw - 1; *p = c1;

	p = startDST + w * (sh - 1);
	*p++ = c2; *p = c1;
	p += (sw - 3);
	*p++ = c1; *p = c2;

	return 0;
}

int ScreenDrawHLine(uint32_t* dst, int w, int h, int position, int stroke, uint32_t color)
{
	uint64_t newColor = (uint64_t)(color);
	newColor <<= 32;
	newColor |= (uint64_t)color;

	int size = w * stroke;
	uint32_t* startPos = dst + w * position;
	uint64_t* p64 = (uint64_t*)startPos;
	for (uint32_t i = 0; i < (size >> 1); i++)
		*p64++ = newColor;

	if (1 & size)  // fix the last pixel if the whole size is not even number
	{
		uint32_t* p32 = startPos + (size - 1);
		*p32 = color;
	}

	return 0;
}
