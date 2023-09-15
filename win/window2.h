#ifndef __WOCHAT_WINDOWS2_H__
#define __WOCHAT_WINDOWS2_H__

#include "dui/dui_win.h"

U16 msg[] = { 0x5468,0x661f,0x661f,0xff1a,0x4e0d,0x77e5,0x6211,0x8005,0x8c13,0x6211,0x4f55,0x6c42,0x00 };

U16 gname1[] ={ 0x0044,0x0042,0x0041,0x57f9,0x8bad,0x7fa4,0x0000 };
U16 gname2[] = { 0x6587, 0x4ef6, 0x4f20, 0x8f93, 0x00 };
U16 gname3[] = { 0x0041,0x0049,0x804a,0x5929,0x673a,0x5668,0x4eba, 0x00 };

enum
{
	WM_XWIN2_GROUPCHANGED = 0x01
};

class XWindow2 : public XWindowT <XWindow2>
{
private:
	enum {
		SELECTED_COLOR = 0xFFC6C6C8,
		HOVERED_COLOR  = 0xFFD9DADC,
		DEFAULT_COLOR  = 0xFFE5E5E6
	};

	enum {
		ITEM_HEIGHT = 64,
		ICON_HEIGHT = 40
	};

	U16			m_chatgroupCount      = 0;
#if 0
	XChatGroup* m_chatgroupRoot       = nullptr;
	XChatGroup* m_chatgroupSelected   = nullptr;
	XChatGroup* m_chatgroupSelectPrev = nullptr;
	XChatGroup* m_chatgroupHovered    = nullptr;
#endif
public:
	XWindow2()
	{
		m_backgroundColor = DEFAULT_COLOR;
		m_scrollbarColor = DEFAULT_COLOR;
		m_message = WM_WIN2_MESSAGE;
		m_property |= DUI_PROP_HASVSCROLL;
	}

	~XWindow2()	{}

public:


	int LoadChatGroupList()
	{
#if 0
		if (nullptr == m_chatgroupRoot)
		{
			int total = 0;
			XChatGroup* p;
			XChatGroup* q;

			p = (XChatGroup*)palloc0(m_pool, sizeof(XChatGroup));
			if (nullptr == p)
				return (-1);
			p->mempool_ = mempool_create(0, 0);
			if (nullptr == p->mempool_)
				return (-1);

			p->id_ = 0;
			p->icon_ = (U32*)xbmpGroup;
			p->name_ = (U16*)gname1;
			p->w_ = ICON_HEIGHT;
			p->h_ = ICON_HEIGHT;
			p->height_ = ITEM_HEIGHT;
			p->lastmsg_ = (U16*)msg;
			p->ts_.timestamp_ = 0xFFFFFFFF;
			p->next_ = nullptr;

			m_chatgroupRoot = p;
			m_chatgroupSelected = m_chatgroupRoot;
			m_chatgroupHovered = nullptr;
			total++;

			for (int i = 0; i < 21; i++)
			{
				q = (XChatGroup*)palloc0(m_pool, sizeof(XChatGroup));
				if (nullptr == q)
					break;
				q->mempool_ = mempool_create(0, 0);
				if (nullptr == q->mempool_)
					break;

				total++;

				p->next_ = q;
				p = q;
				p->next_ = nullptr;
				p->id_ = i + 1;
				p->w_ = ICON_HEIGHT;
				p->h_ = ICON_HEIGHT;
				p->height_ = ITEM_HEIGHT;
				p->lastmsg_ = (U16*)msg;
				p->ts_.timestamp_ = 0xFFFFFFFF;

				switch (i % 3)
				{
				case 0:
					p->icon_ = (U32*)xbmpFileTransfer;
					p->name_ = (U16*)gname2;
					break;
				case 1:
					p->icon_ = (U32*)xbmpChatGPT;
					p->name_ = (U16*)gname3;
					break;
				case 2:
					p->icon_ = (U32*)xbmpGroup;
					p->name_ = (U16*)gname1;
					break;
				default:
					assert(0);
					break;
				}
			}

			m_chatgroupCount = total;
			m_totalHeight = m_chatgroupCount * ITEM_HEIGHT;
			m_vscrollOffset = 0;
			m_vscrollStep = ITEM_HEIGHT;
		}
#endif
		return 0;
	}

	int DoCreate(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{ 
		int ret = 0;
		m_sizeAll.cy = 4096;
		m_sizeLine.cy = 100;
		return ret; 
	}

	int DoDestroy(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		return 0;
	}

	int Draw()
	{
		return 0;
	}

	int DoMouseMove(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{ 
		return 0;
	}

	int DoLButtonDown(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{ 
		return 0;
	}

	int DoLButtonUp(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{ 
		return 0;
	}

};

#endif  /* __WOCHAT_WINDOWS2_H__ */

