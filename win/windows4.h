#ifndef __WOCHAT_WINDOWS4_H__
#define __WOCHAT_WINDOWS4_H__

#include "xwindui.h"

#define CHAT_MYSELF_STATE		0x01
#define CHAT_OTHERS_STATE		0x02
#define CHAT_TEXTMG_STATE		0x04
#define CHAT_PICTURE_STATE		0x08
#define CHAT_VIDEO_STATE		0x10
#define CHAT_VOICE_STATE		0x20
#define CHAT_HINTS_STATE		0x40

U8 msg0[] = {
	0xE5,0x91,0xA8,0xE6,0x98,0x9F,0xE6,0x98,0x9F,0xEF,0xBC,0x9A,0xE5,0x85,0xB3,0xE5,0xB1,0xB1,0xE9,0x9A,0xBE,
	0xE8,0xB6,0x8A,0xEF,0xBC,0x8C,0xE8,0xB0,0x81,0xE6,0x82,0xB2,0xE5,0xA4,0xB1,0xE8,0xB7,0xAF,0xE4,0xB9,0x8B,
	0xE4,0xBA,0xBA,0xEF,0xBC,0x9F,0x00
};

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
		m_property |= XWIN_PROP_HASVSCROLL;
	}

	~XWindow4()
	{
	}

	BLFont   m_font0;
	BLFont   m_font1;
	int      m_lineHeight0;
	int      m_lineHeight1;

	XChatGroup* m_chatGroup = nullptr;

	int SetChatGroup(XChatGroup* cg)
	{
		m_chatGroup = cg;
		m_status |= XWIN_STATUS_NEEDRAW;

		return XWIN_STATUS_NEEDRAW;
	}

	int UpdateChatHistory(uint16_t* msgText, U8 msgtype = 0)
	{
		assert(nullptr != msgText);

		if (nullptr != m_chatGroup)
		{
			MemoryContext mempool = m_chatGroup->mempool_;
			assert(nullptr != mempool);

			XChatMessage* m = (XChatMessage*)palloc0(mempool, sizeof(XChatMessage));
			if (nullptr != m)
			{
				U16 i;
				uint16_t* p;
				uint16_t* q;
				m->next_ = nullptr;
				if (0 == msgtype)
				{
					m->state_ = CHAT_MYSELF_STATE | CHAT_TEXTMG_STATE;
					m->icon_ = (U32*)xbmpHeadMe;
				}
				else
				{
					m->state_ = CHAT_OTHERS_STATE | CHAT_TEXTMG_STATE;
					m->icon_ = (U32*)xbmpHeadGirl;
				}
				m->w_ = 34;
				m->h_ = 34;

				p = msgText;
				for (i = 0; i < 0xFFFF; i++)
				{
					if (0 == *p)
						break;
					p++;
				}
				assert(i > 0);
				m->msgLen_ = i;
				m->message_ = (uint16_t*)palloc(mempool, (Size)(sizeof(uint16_t) * (i + 1)));
				if (nullptr != m->message_)
				{
					p = m->message_;
					q = msgText;
					for (int k = 0; k < i; k++)
						*p++ = *q++;
					*p = 0;

					if (nullptr == m_chatGroup->message_)
					{
						m_chatGroup->message_ = m;
						m_chatGroup->msgTail_ = m;
					}
					else
					{
						assert(nullptr != m_chatGroup->msgTail_);
						m_chatGroup->msgTail_->next_ = m;
						m_chatGroup->msgTail_ = m;
					}
					m_status |= XWIN_STATUS_NEEDRAW;  // Invalidate the screen
				}
			}
		}

		return 0;
	}

	int LoadChatHistory()
	{
		return 0;
	}

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int ret = 0;
		BLResult blResult;
		BLFontMetrics fm;

		blResult = m_font0.createFromFace(g_fontFace, 15.0f);
		if (BL_SUCCESS != blResult)
			return (-1);

		blResult = m_font1.createFromFace(g_fontFace, 11.0f);
		if (BL_SUCCESS != blResult)
			return (-1);

		fm = m_font0.metrics();
		m_lineHeight0 = (int)(fm.ascent + fm.descent + fm.lineGap);
		m_vscrollStep = m_lineHeight0;

		LoadChatHistory();

		return 0;
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int h = m_area.bottom - m_area.top;
		if (m_totalHeight < 0)
			m_totalHeight = h;

		if (m_vscrollOffset < 0)
			m_vscrollOffset = m_totalHeight - h;

		return 0;
	}

	U16 TextLayout(int width, uint16_t* txt, uint16_t* idxTab)
	{
		return 1;
	}

	int Draw()
	{
		if (nullptr != m_chatGroup)
		{
			XChatMessage* p = m_chatGroup->message_;
			if (nullptr != p)
			{
				int i, k, pos, dx, dy, margin, W, H;
				int X, x = 0, y = 0;
				double offsetY = m_lineHeight0;
				U16 len, preIdx = 0;

				int itemHeight;
				U16 lineNumber;

				BLResult blResult;
				//BLRgba32 textColor0(0xFFFFFFFFu);
				BLRgba32 textColor0(0xFF333333u);
				BLRgba32 textColor1(0xFF777777u);
				BLRgba32 textColor2(0xFF555555u);
				BLRgba32 defaultColor(m_backgroundColor);
				//BLRgba32 defaultColor(0xFF0000FFu);
				BLImageData imgdata = { 0 };
				BLGlyphBuffer gb;

				int w = m_area.right - m_area.left;
				int h = m_area.bottom - m_area.top;
				assert(w > 300);

				margin = 0;
				if (XWIN_STATUS_VSCROLL & m_status)
					margin = m_scrollWidth;

				W = WOCHAT_ALIGN_DEFAULT32(w - 240);

				// Draw Chat History
				m_totalHeight = 0;
				pos = 0;
				while (nullptr != p)
				{
					assert(p->msgLen_ > 0);

					lineNumber = TextLayout(W, p->message_, nullptr);
					assert(lineNumber > 0);

					H = lineNumber * (m_lineHeight0 + 1);
					if (H < p->h_ + 20)
						H = p->h_ + 20;
					itemHeight = p->h_ + 20;
					m_totalHeight += itemHeight;

					if ((pos + itemHeight > m_vscrollOffset) && (pos < (m_vscrollOffset + h)))
					{
						BLImage img;
						if (CHAT_MYSELF_STATE & p->state_)
						{
							dx = w - p->w_ - m_scrollWidth - 4;
							X = 160;
						}
						else
						{
							dx = 10;
							X = 60;
						}

						dy = pos - m_vscrollOffset + (itemHeight - p->h_) / 2;
						ScreenDrawRectRound(m_screen, w, h, p->icon_, p->w_, p->h_, dx, dy, m_backgroundColor);

						blResult = img.create(W, H, BL_FORMAT_PRGB32);
						if (BL_SUCCESS == blResult)
						{
							BLContext ctx(img);
							ctx.fillAll(defaultColor);
							gb.setUtf16Text((const uint16_t*)p->message_);
							m_font0.shape(gb);
							ctx.fillGlyphRun(BLPoint(0, offsetY + y), m_font0, gb.glyphRun(), textColor0);
							blResult = img.getData(&imgdata);
							if (BL_SUCCESS == blResult)
							{
								ScreenDrawRect(m_screen, w, h, (U32*)imgdata.pixelData, imgdata.size.w, imgdata.size.h,
									X, pos - m_vscrollOffset + 10);
							}
							ctx.end();
						}
					}
					pos += itemHeight;
					p = p->next_;
				}

				m_vscrollOffset = 0;
				if (m_totalHeight > h)
					m_vscrollOffset = m_totalHeight - h;
			}
		}

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

