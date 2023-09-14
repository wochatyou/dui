#ifndef __WOCHAT_WINDOWS2_H__
#define __WOCHAT_WINDOWS2_H__

#include "xwindui.h"

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
	XChatGroup* m_chatgroupRoot       = nullptr;
	XChatGroup* m_chatgroupSelected   = nullptr;
	XChatGroup* m_chatgroupSelectPrev = nullptr;
	XChatGroup* m_chatgroupHovered    = nullptr;

public:
	XWindow2()
	{
		m_backgroundColor = DEFAULT_COLOR;
		m_scrollbarColor = DEFAULT_COLOR;
		m_message = WM_WIN2_MESSAGE;
		m_property |= XWIN_PROP_HASVSCROLL;
	}

	~XWindow2()	{}

	BLFont   m_font0;
	BLFont   m_font1;

public:

	XChatGroup* GetFirstChatGroup()
	{
		return m_chatgroupRoot;
	}

	int LoadChatGroupList()
	{
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
		return 0;
	}

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr) 
	{ 
		int ret = 0;
		BLResult blResult;

		blResult = m_font0.createFromFace(g_fontFace, 16.0f);
		if (BL_SUCCESS != blResult)
			return (-1);
		
		blResult = m_font1.createFromFace(g_fontFace, 13.0f);
		if (BL_SUCCESS != blResult)
			return (-1);

		m_pool = mempool_create(0, 0);
		if (nullptr == m_pool)
			return (-1);

		LoadChatGroupList();

		return ret; 
	}

	int DoDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		XChatGroup* p = m_chatgroupRoot;

		while (nullptr != p)  // clean up the memory
		{
			mempool_destroy(p->mempool_);
			p = p->next_;
		}

		return 0;
	}

	int Draw()
	{
		U32 color;
		int dx, dy, pos, margin = 0;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		BLResult blResult;
		BLRgba32 selectedColor(SELECTED_COLOR);
		BLRgba32 hoveredColor(HOVERED_COLOR);
		BLRgba32 defaultColor(DEFAULT_COLOR);
		BLRgba32 textColor0(0xFF333333u);
		BLRgba32 textColor1(0xFF777777u);
		BLRgba32 textColor2(0xFF555555u);

		BLImageData imgdata = { 0 };
		BLGlyphBuffer gb;
		BLTextMetrics tm;
		BLFontMetrics fm;
		BLImage img;
		
		if (XWIN_STATUS_VSCROLL & m_status)
			margin = m_scrollWidth;

		dx = ((ITEM_HEIGHT - ICON_HEIGHT) >> 1);
		int W = WOCHAT_ALIGN_DEFAULT32(w - dx - dx - ICON_HEIGHT - margin);
		blResult = img.create(W, ITEM_HEIGHT, BL_FORMAT_PRGB32);
		if (BL_SUCCESS == blResult)
		{
			BLContext ctx(img);
			pos = 0;
			XChatGroup* p = m_chatgroupRoot;
			while (nullptr != p)
			{
				if (pos + ITEM_HEIGHT > m_vscrollOffset)
				{
					if (p == m_chatgroupSelected)
					{
						color = SELECTED_COLOR;
						ctx.fillAll(selectedColor);
					}
					else if (p == m_chatgroupHovered)
					{
						color = HOVERED_COLOR;
						ctx.fillAll(hoveredColor);
					}
					else
					{
						color = DEFAULT_COLOR;
						ctx.fillAll(defaultColor);
					}
					ScreenFillRect(m_screen, w, h, color, w - margin, ITEM_HEIGHT, 0, pos - m_vscrollOffset);

					dy = pos - m_vscrollOffset + dx;
					ScreenDrawRectRound(m_screen, w, h, p->icon_, p->w_, p->h_, dx, dy, color);

					if (nullptr != p->name_)
					{
						gb.setUtf16Text((const uint16_t*)p->name_);
						m_font0.shape(gb);
						ctx.fillGlyphRun(BLPoint(0, 24), m_font0, gb.glyphRun(), textColor0);
					}

					{
						const char* str = (const char*)"09:18 AM";
						gb.setUtf8Text(str);
						m_font1.shape(gb);
						m_font1.getTextMetrics(gb, tm);
						double x = (tm.boundingBox.x1 - tm.boundingBox.x0);
						ctx.fillGlyphRun(BLPoint((double)W - x - 16 + margin, 24), m_font1, gb.glyphRun(), textColor1);
					}

					if (nullptr != p->lastmsg_)
					{
						gb.setUtf16Text((const uint16_t*)p->lastmsg_);
						m_font1.shape(gb);
						ctx.fillGlyphRun(BLPoint(0, 48), m_font1, gb.glyphRun(), textColor2);
					}

					blResult = img.getData(&imgdata);
					if (BL_SUCCESS == blResult)
					{
						ScreenDrawRect(m_screen, w, h, (U32*)imgdata.pixelData, imgdata.size.w, imgdata.size.h, dx + dx + ICON_HEIGHT, pos - m_vscrollOffset);
					}
				}

				p = p->next_;
				pos += ITEM_HEIGHT;
				if (pos >= (m_vscrollOffset + h))
					break;
			}
			ctx.end();
		}

		return 0;
	}

	int DoMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr) 
	{ 
		int hit = -1;
		int ret = XWIN_STATUS_NODRAW;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		XChatGroup* hovered = m_chatgroupHovered;

		RECT area = { 0 };
		RECT* r = &area;
		r->left = 0;
		r->right = w;

		if (XWinPointInRect(xPos, yPos, &m_area))
		{
			int pos = 0;
			// transfer the coordination from screen to local virutal window
			xPos -= m_area.left;
			yPos -= m_area.top;
			
			assert(xPos >= 0);
			assert(yPos >= 0);

			XChatGroup* p = m_chatgroupRoot;
			while (nullptr != p)
			{
				r->top = pos; r->bottom = r->top + ITEM_HEIGHT;
				if (XWinPointInRect(xPos, yPos + m_vscrollOffset, r))
				{
					m_chatgroupHovered = p;
					hit++;
					if(hovered != m_chatgroupHovered) // The hovered item is changed, we need to redraw
						ret = XWIN_STATUS_NEEDRAW;
					break;
				}
				p = p->next_;
				pos += ITEM_HEIGHT;
				if (pos >= (m_vscrollOffset + h))
					break;
			}
		}
		if (-1 == hit)
		{
			if (nullptr != m_chatgroupHovered)
			{
				m_chatgroupHovered = nullptr;
				ret = XWIN_STATUS_NEEDRAW;
			}
		}

		return ret; 
	}

	int DoLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr) 
	{ 
		int hit = -1;
		int ret = XWIN_STATUS_NODRAW;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		RECT area = { 0 };
		RECT* r = &area;
		r->left = 0;
		r->right = w;
		if (XWinPointInRect(xPos, yPos, &m_area))
		{
			int pos = 0;

			// transfer the coordination from screen to local virutal window
			xPos -= m_area.left;
			yPos -= m_area.top;

			XChatGroup* p = m_chatgroupRoot;
			while (nullptr != p)
			{
				r->top = pos; r->bottom = r->top + ITEM_HEIGHT;
				if (XWinPointInRect(xPos, yPos + m_vscrollOffset, r))
				{
					m_chatgroupHovered = p;
					m_chatgroupSelected = p;
					hit++;
					ret = XWIN_STATUS_NEEDRAW;
					break;
				}
				p = p->next_;
				pos += ITEM_HEIGHT;
				if (pos >= (m_vscrollOffset + h))
					break;
			}
		}
		if (-1 == hit)
		{
			if (nullptr != m_chatgroupHovered)
			{
				m_chatgroupHovered = nullptr;
				ret = XWIN_STATUS_NEEDRAW;
			}
		}

		return ret;
	}

	int DoLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr) 
	{ 
		int hit = -1;
		int ret = XWIN_STATUS_NODRAW;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		RECT area = { 0 };
		RECT* r = &area;
		r->left = 0;
		r->right = w;
		if (XWinPointInRect(xPos, yPos, &m_area))
		{
			int pos = 0;
			// transfer the coordination from screen to local virutal window
			xPos -= m_area.left;
			yPos -= m_area.top;

			XChatGroup* p = m_chatgroupRoot;
			while (nullptr != p)
			{
				r->top = pos; r->bottom = r->top + ITEM_HEIGHT;
				if (XWinPointInRect(xPos, yPos + m_vscrollOffset, r))
				{
					m_chatgroupHovered = p;
					m_chatgroupSelected = p;
					hit++;
					ret = XWIN_STATUS_NEEDRAW;
					break;
				}
				p = p->next_;
				pos += ITEM_HEIGHT;
				if (pos >= (m_vscrollOffset + h))
					break;
			}
		}
		if (-1 == hit)
		{
			if (nullptr != m_chatgroupHovered)
			{
				m_chatgroupHovered = nullptr;
				ret = XWIN_STATUS_NEEDRAW;
			}
		}
		else // we hit some item
		{
			assert(nullptr != m_chatgroupSelected);
			if(m_chatgroupSelectPrev != m_chatgroupSelected)
				NotifyParent(m_message, WM_XWIN2_GROUPCHANGED, (U64)m_chatgroupSelected);

			m_chatgroupSelectPrev = m_chatgroupSelected; // prevent double selection
		}

		return ret;
	}

};

#endif  /* __WOCHAT_WINDOWS2_H__ */

