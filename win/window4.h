#ifndef __WOCHAT_WINDOWS4_H__
#define __WOCHAT_WINDOWS4_H__

#include "dui/dui_win.h"

class XWindow4 : public XWindowT <XWindow4>
{
private:
	enum {
		GAP_TOP3 = 40,
		GAP_BOTTOM3 = 10,
		GAP_LEFT3 = 0,
		GAP_RIGHT3 = 0,
	};

public:
	XWindow4()
	{
		m_backgroundColor = 0xFFF5F5F5;
		m_message = WM_WIN4_MESSAGE;
		m_property |= DUI_PROP_HASVSCROLL;
	}

	~XWindow4()
	{
	}

	int UpdateChatHistory(uint16_t* msgText, U8 msgtype = 0)
	{
		return 0;
	}

	int LoadChatHistory()
	{
		return 0;
	}

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int ret = 0;

		m_sizeAll.cy = 3000;
		m_sizeLine.cy = 128;

		return 0;
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int h = m_area.bottom - m_area.top;
		m_ptOffset.y = m_sizeAll.cy - h;

		return 0;
	}

	U16 TextLayout(int width, uint16_t* txt, uint16_t* idxTab)
	{
		return 1;
	}

	int Draw()
	{
		return 0;
	}

	int DoMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		return 0;
	}

	int DoLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		return 0;
	}

	int DoLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		return 0;
	}
};

#endif  /* __WOCHAT_WINDOWS4_H__ */

