#ifndef __DUI_WINDOW0_H__
#define __DUI_WINDOW0_H__

#include "dui/dui_win.h"

enum 
{
	XWIN0_BUTTON_ME = 0
	, XWIN0_BUTTON_TALK
	, XWIN0_BUTTON_FRIEND
	, XWIN0_BUTTON_QUAN
	, XWIN0_BUTTON_COIN
	, XWIN0_BUTTON_FAVORITE
	, XWIN0_BUTTON_FILE
	, XWIN0_BUTTON_SETTING
	, XWIN0_BUTTON_NETWORK
	//, XWIN0_BUTTON_BITCOIN
};

class XWindow0 : public XWindowT <XWindow0>
{
private:
	enum 
	{
		XWIN0_BITMAP_ME = 0
		, XWIN0_BITMAP_TALKN			// Normal
		, XWIN0_BITMAP_TALKH			// Hover
		, XWIN0_BITMAP_TALKP			// Press
		, XWIN0_BITMAP_TALKA			// Active

		, XWIN0_BITMAP_FRIENDN		// Normal
		, XWIN0_BITMAP_FRIENDH		// Hover
		, XWIN0_BITMAP_FRIENDP		// Press
		, XWIN0_BITMAP_FRIENDA		// Active

		, XWIN0_BITMAP_QUANN			// Normal
		, XWIN0_BITMAP_QUANH			// Hover
		, XWIN0_BITMAP_QUANP			// Press
		, XWIN0_BITMAP_QUANA			// Active

		, XWIN0_BITMAP_FAVORITEN		// Normal
		, XWIN0_BITMAP_FAVORITEH		// Hover
		, XWIN0_BITMAP_FAVORITEP		// Press
		, XWIN0_BITMAP_FAVORITEA		// Active

		, XWIN0_BITMAP_COINN			// Normal
		, XWIN0_BITMAP_COINH			// Hover
		, XWIN0_BITMAP_COINP			// Press
		, XWIN0_BITMAP_COINA			// Active

		, XWIN0_BITMAP_FILEN			// Normal
		, XWIN0_BITMAP_FILEH			// Hover
		, XWIN0_BITMAP_FILEP			// Press
		, XWIN0_BITMAP_FILEA			// Active

		, XWIN0_BITMAP_SETTINGN		// Normal
		, XWIN0_BITMAP_SETTINGH		// Hover
		, XWIN0_BITMAP_SETTINGP		// Press
		, XWIN0_BITMAP_SETTINGA		// Active

		, XWIN0_BITMAP_NETWORKN		// Normal
		, XWIN0_BITMAP_NETWORKH		// Hover
		, XWIN0_BITMAP_NETWORKP		// Press
		, XWIN0_BITMAP_NETWORKA		// Active

		, XWIN0_BITMAP_BITCOINN		// Normal
		, XWIN0_BITMAP_BITCOINH		// Hover
		, XWIN0_BITMAP_BITCOINP		// Press
		, XWIN0_BITMAP_BITCOINA		// Active

	};

public:
	XWindow0()
	{
		m_backgroundColor = 0xFF2A2928;
		m_buttonStartIdx = XWIN0_BUTTON_ME;
		m_buttonEndIdx = XWIN0_BUTTON_NETWORK;
		m_property |= (DUI_PROP_MOVEWIN | DUI_PROP_BTNACTIVE);
		m_message = WM_WIN0_MESSAGE;
	}
	~XWindow0() {}

public:
	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int ret = InitButtons();
		return ret;
	}

	void UpdateButtonPosition()
	{
		int id, bottom;
		int gap = 10; // pixel
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		XButton* button;
		XBitmap* bmp;

		id = XWIN0_BUTTON_NETWORK; button = &m_button[id];
		bmp = button->imgNormal;
		button->bottom = h - gap;
		button->top = button->bottom - bmp->h;
		bottom = button->top;
		button->left = ((w - bmp->w) >> 1);
		button->right = button->left + bmp->w;

		// get the new position of Setting button
		id = XWIN0_BUTTON_SETTING; button = &m_button[id];
		bmp = button->imgNormal;
		button->bottom = bottom - (gap>>1);
		button->top = button->bottom - bmp->h;
	}

	void UpdatePosition()
	{
		UpdateButtonPosition();
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		UpdateButtonPosition();
		return DUI_STATUS_NEEDRAW;
	}

private:
	static int ButtonAction(void* obj, U32 uMsg, U64 wParam, U64 lParam)
	{
		int ret = 0;
		XWindow0* xw = (XWindow0*)obj;
		if (nullptr != xw)
			ret = xw->NotifyParent(uMsg, wParam, lParam);
		return ret;
	}

	void InitBitmap()
	{
		U8 id;
		XBitmap* bmp;

		int w = 48;
		int h = 48;
		id = XWIN0_BITMAP_ME;        bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpMe;        bmp->w = w; bmp->h = h;

		w = 32; h = 32;
		id = XWIN0_BITMAP_TALKN;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpTalkN;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_TALKH;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpTalkH;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_TALKP;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpTalkP;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_TALKA;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpTalkA;     bmp->w = w; bmp->h = h;

		id = XWIN0_BITMAP_FRIENDN;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFriendN;   bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FRIENDH;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFriendH;   bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FRIENDP;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFriendP;   bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FRIENDA;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFriendA;   bmp->w = w; bmp->h = h;

		id = XWIN0_BITMAP_QUANN;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpQuanN;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_QUANH;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpQuanH;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_QUANP;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpQuanP;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_QUANA;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpQuanA;     bmp->w = w; bmp->h = h;

		id = XWIN0_BITMAP_SETTINGN;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSettingN;  bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_SETTINGH;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSettingH;  bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_SETTINGP;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSettingP;  bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_SETTINGA;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSettingA;  bmp->w = w; bmp->h = h;

		id = XWIN0_BITMAP_FILEN;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFileN;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FILEH;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFileH;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FILEP;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFileP;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FILEA;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFileA;     bmp->w = w; bmp->h = h;

		id = XWIN0_BITMAP_FAVORITEN; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFavoriteN; bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FAVORITEH; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFavoriteH; bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FAVORITEP; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFavoriteP; bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FAVORITEA; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpFavoriteA; bmp->w = w; bmp->h = h;

		id = XWIN0_BITMAP_COINN;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCoinN;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_COINH;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCoinH;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_COINP;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCoinP;     bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_COINA;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCoinA;     bmp->w = w; bmp->h = h;

		w = 13; h = 13;
		id = XWIN0_BITMAP_NETWORKN;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpNetWorkN;  bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_NETWORKH;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpNetWorkN;  bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_NETWORKP;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpNetWorkN;  bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_NETWORKA;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpNetWorkN;  bmp->w = w; bmp->h = h;

		id = XWIN0_BITMAP_BITCOINN;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpNetWorkN;  bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_BITCOINH;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpNetWorkN;  bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_BITCOINP;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpNetWorkN;  bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_BITCOINA;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpNetWorkN;  bmp->w = w; bmp->h = h;
	}

	int InitButtons()
	{
		int top;
		XButton* button;
		XBitmap* bitmap;

		InitBitmap();

		int	w = XWIN0_WIDTH;
		for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
		{
			button = &m_button[i];
			button->pfAction = ButtonAction;
		}
		m_buttonActiveIdx = XWIN0_BUTTON_TALK;

		// the position of the buttons is related to the virtual window, not the client area.
		int id = XWIN0_BUTTON_ME; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN0_BITMAP_ME];
		button->imgNormal = button->imgHover = button->imgPress = button->imgActive = bitmap;
		button->property |= XBUTTON_PROP_ROUND;
		button->left = ((w - bitmap->w) >> 1);
		button->top = button->left;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;
		top = button->bottom + 20;

		id = XWIN0_BUTTON_TALK; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN0_BITMAP_TALKN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_TALKH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_TALKP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_TALKA]; button->imgActive = bitmap;
		button->left = ((w - bitmap->w) >> 1);
		button->top = top;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;
		top = button->bottom + bitmap->h;

		id = XWIN0_BUTTON_FRIEND; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN0_BITMAP_FRIENDN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_FRIENDH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_FRIENDP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_FRIENDA]; button->imgActive = bitmap;
		button->left = ((w - bitmap->w) >> 1);
		button->top = top;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;
		top = button->bottom + bitmap->h;

		id = XWIN0_BUTTON_QUAN; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN0_BITMAP_QUANN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_QUANH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_QUANP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_QUANA]; button->imgActive = bitmap;
		button->left = ((w - bitmap->w) >> 1);
		button->top = top;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;
		top = button->bottom + bitmap->h;

		id = XWIN0_BUTTON_COIN; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN0_BITMAP_COINN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_COINH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_COINP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_COINA]; button->imgActive = bitmap;
		button->left = ((w - bitmap->w) >> 1);
		button->top = top;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;
		top = button->bottom + bitmap->h;

		id = XWIN0_BUTTON_FAVORITE; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN0_BITMAP_FAVORITEN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_FAVORITEH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_FAVORITEP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_FAVORITEA]; button->imgActive = bitmap;
		button->left = ((w - bitmap->w) >> 1);
		button->top = top;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;
		top = button->bottom + bitmap->h;

		id = XWIN0_BUTTON_FILE; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN0_BITMAP_FILEN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_FILEH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_FILEP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_FILEA]; button->imgActive = bitmap;
		button->left = ((w - bitmap->w) >> 1);
		button->top = top;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;
		top = button->bottom + bitmap->h;

		id = XWIN0_BUTTON_SETTING; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN0_BITMAP_SETTINGN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_SETTINGH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_SETTINGP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_SETTINGA]; button->imgActive = bitmap;
		button->left = ((w - bitmap->w) >> 1);
		button->right = button->left + bitmap->w;

		id = XWIN0_BUTTON_NETWORK; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN0_BITMAP_NETWORKN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_NETWORKH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_NETWORKP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN0_BITMAP_NETWORKA]; button->imgActive = bitmap;
		button->left = ((w - bitmap->w) >> 1);
		button->right = button->left + bitmap->w;
		button->property |= XBUTTON_PROP_STATIC;
		return 0;
	}

};

#endif  /* __DUI_WINDOW0_H__ */

