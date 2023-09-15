#ifndef __WOCHAT_WINDOWS5_H__
#define __WOCHAT_WINDOWS5_H__

#include "dui/dui_win.h"

uint16_t inputMessage[XWIN_MAX_INPUTSTRING + 1] = { 0 };

enum {
	XWIN5_BUTTON_EMOJI = 0
	, XWIN5_BUTTON_UPLOAD
	, XWIN5_BUTTON_CAPTURE
	, XWIN5_BUTTON_CHATHISTORY
	, XWIN5_BUTTON_LIVESTREAM
	, XWIN5_BUTTON_VIDEOCALL
	, XWIN5_BUTTON_SENDMESSAGE
};

class XWindow5 : public XWindowT <XWindow5>
{
private:
	enum 
	{
		GAP_TOP4 = 40,
		GAP_BOTTOM4 = 40,
		GAP_LEFT4 = 4,
		GAP_RIGHT4 = 4
	};

	enum 
	{
		XWIN5_BITMAP_EMOJIN
		, XWIN5_BITMAP_EMOJIH				// Hover
		, XWIN5_BITMAP_EMOJIP				// Press
		, XWIN5_BITMAP_EMOJIA				// Active

		, XWIN5_BITMAP_UPLOADN				// Normal
		, XWIN5_BITMAP_UPLOADH				// Hover
		, XWIN5_BITMAP_UPLOADP				// Press
		, XWIN5_BITMAP_UPLOADA				// Active

		, XWIN5_BITMAP_CAPTUREN				// Normal
		, XWIN5_BITMAP_CAPTUREH				// Hover
		, XWIN5_BITMAP_CAPTUREP				// Press
		, XWIN5_BITMAP_CAPTUREA				// Active

		, XWIN5_BITMAP_CHATHISTORYN			// Normal
		, XWIN5_BITMAP_CHATHISTORYH			// Hover
		, XWIN5_BITMAP_CHATHISTORYA			// Press
		, XWIN5_BITMAP_CHATHISTORYP			// Active

		, XWIN5_BITMAP_LIVESTREAMN			// Normal
		, XWIN5_BITMAP_LIVESTREAMH			// Hover
		, XWIN5_BITMAP_LIVESTREAMP			// Press
		, XWIN5_BITMAP_LIVESTREAMA			// Active

		, XWIN5_BITMAP_VIDEOCALLN			// Normal
		, XWIN5_BITMAP_VIDEOCALLH			// Hover
		, XWIN5_BITMAP_VIDEOCALLP			// Press
		, XWIN5_BITMAP_VIDEOCALLA			// Active

		, XWIN5_BITMAP_SENDMESSAGEN			// Normal			
		, XWIN5_BITMAP_SENDMESSAGEH			// Hover
		, XWIN5_BITMAP_SENDMESSAGEP			// Press
		, XWIN5_BITMAP_SENDMESSAGEA			// Active

	};

public:
	XWindow5()
	{
		m_backgroundColor = 0xFFFFFFFF;
		m_buttonStartIdx = XWIN5_BUTTON_EMOJI;
		m_buttonEndIdx = XWIN5_BUTTON_SENDMESSAGE;
		m_property |= DUI_PROP_MOVEWIN;
		m_message = WM_WIN5_MESSAGE;

		InitButtons();
	}

	~XWindow5()
	{
	}

	static int ButtonAction(void* obj, U32 uMsg, U64 wParam, U64 lParam)
	{
		int ret = 0;
		XWindow5* xw = (XWindow5*)obj;
		if (nullptr != xw)
			ret = xw->NotifyParent(uMsg, wParam, lParam);
		return ret;
	}

	void InitBitmap()
	{
		U8 id;
		XBitmap* bmp;

		int w = 19;
		int h = 19;
		id = XWIN5_BITMAP_EMOJIN;       bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpEmojiN;       bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_EMOJIH;       bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpEmojiH;       bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_EMOJIP;       bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpEmojiP;       bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_EMOJIA;       bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpEmojiN;       bmp->w = w; bmp->h = h;

		id = XWIN5_BITMAP_UPLOADN;      bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpUploadN;      bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_UPLOADH;      bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpUploadH;      bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_UPLOADP;      bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpUploadP;      bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_UPLOADA;      bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpUploadN;      bmp->w = w; bmp->h = h;

		id = XWIN5_BITMAP_CAPTUREN;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCaptureN;     bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_CAPTUREH;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCaptureH;     bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_CAPTUREP;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCaptureP;     bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_CAPTUREA;     bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpCaptureN;     bmp->w = w; bmp->h = h;

		id = XWIN5_BITMAP_CHATHISTORYN; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpChatHistoryN; bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_CHATHISTORYH; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpChatHistoryH; bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_CHATHISTORYP; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpChatHistoryP; bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_CHATHISTORYA; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpChatHistoryN; bmp->w = w; bmp->h = h;

		id = XWIN5_BITMAP_LIVESTREAMN;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpLiveStreamN;  bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_LIVESTREAMH;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpLiveStreamH;  bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_LIVESTREAMP;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpLiveStreamP;  bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_LIVESTREAMA;  bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpLiveStreamN;  bmp->w = w; bmp->h = h;

		id = XWIN5_BITMAP_VIDEOCALLN;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpVideoCallN;   bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_VIDEOCALLH;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpVideoCallH;   bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_VIDEOCALLP;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpVideoCallP;   bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_VIDEOCALLA;   bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpVideoCallN;   bmp->w = w; bmp->h = h;

		w = 52; h = 34;
		id = XWIN5_BITMAP_SENDMESSAGEN; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSendMessageN; bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_SENDMESSAGEH; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSendMessageH; bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_SENDMESSAGEP; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSendMessageP; bmp->w = w; bmp->h = h;
		id = XWIN5_BITMAP_SENDMESSAGEA; bmp = &m_bitmap[id]; bmp->id = id; bmp->data = (U32*)xbmpSendMessageH; bmp->w = w; bmp->h = h;
	}


	int InitButtons()
	{
		int offset = 10, gap = 20;
		U16 w, h;
		U8 id;
		U32 i, size, bytes;
		XButton* button;
		XBitmap* bitmap;

		InitBitmap(); // inital all bitmap resource

		// Initialize All buttons
		for (int i = m_buttonStartIdx; i <= m_buttonEndIdx; i++)
		{
			button = &m_button[i];
			button->top = offset;
			m_button[i].pfAction = ButtonAction;
		}

		id = XWIN5_BUTTON_EMOJI; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN5_BITMAP_EMOJIN];  button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_EMOJIH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_EMOJIP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_EMOJIA]; button->imgActive = bitmap;
		button->left = offset;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;

		offset = button->right + gap;
		id = XWIN5_BUTTON_UPLOAD; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN5_BITMAP_UPLOADN];  button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_UPLOADH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_UPLOADP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_UPLOADA]; button->imgActive = bitmap;
		button->left = offset;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;

		offset = button->right + gap;
		id = XWIN5_BUTTON_CAPTURE; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN5_BITMAP_CAPTUREN];  button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_CAPTUREH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_CAPTUREP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_CAPTUREA]; button->imgActive = bitmap;
		button->left = offset;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;

		offset = button->right + gap;
		id = XWIN5_BUTTON_CHATHISTORY; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN5_BITMAP_CHATHISTORYN];  button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_CHATHISTORYH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_CHATHISTORYP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_CHATHISTORYA]; button->imgActive = bitmap;
		button->left = offset;
		button->right = button->left + bitmap->w;
		button->bottom = button->top + bitmap->h;

		// the below 3 buttons have dynamtic position which will be determined later
		id = XWIN5_BUTTON_LIVESTREAM; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN5_BITMAP_LIVESTREAMN];  button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_LIVESTREAMH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_LIVESTREAMP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_LIVESTREAMA]; button->imgActive = bitmap;

		id = XWIN5_BUTTON_VIDEOCALL; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN5_BITMAP_VIDEOCALLN];  button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_VIDEOCALLH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_VIDEOCALLP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_VIDEOCALLA]; button->imgActive = bitmap;

		id = XWIN5_BUTTON_SENDMESSAGE; button = &m_button[id]; button->id = id;
		bitmap = &m_bitmap[XWIN5_BITMAP_SENDMESSAGEN]; button->imgNormal = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_SENDMESSAGEH]; button->imgHover = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_SENDMESSAGEP]; button->imgPress = bitmap;
		bitmap = &m_bitmap[XWIN5_BITMAP_SENDMESSAGEA]; button->imgActive = bitmap;

		return 0;
	}

	void UpdateButtonPosition()
	{
		int id;
		int gap = 10; // pixel
		XButton* button;
		XButton* buttonPrev;
		XBitmap* bmp;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;

		id = XWIN5_BUTTON_SENDMESSAGE;  button = &m_button[id]; bmp = button->imgNormal;
		button->right = w - 5;
		button->bottom = h - 5;
		button->left = button->right - bmp->w;
		button->top = button->bottom - bmp->h;

		id = XWIN5_BUTTON_VIDEOCALL;  button = &m_button[id]; bmp = button->imgNormal;
		button->right = w - (gap << 1);
		button->left = button->right - bmp->w;
		button->top = gap;
		button->bottom = button->top + bmp->h;

		buttonPrev = button;
		id = XWIN5_BUTTON_LIVESTREAM;  button = &m_button[id]; bmp = button->imgNormal;
		button->right = buttonPrev->left - gap;
		button->left = button->right - bmp->w;
		button->top = gap;
		button->bottom = button->top + bmp->h;
	}


public:

	void PostWindowHide()
	{
	}

	void PostWindowShow()
	{
	}

	void UpdatePosition()
	{
		UpdateButtonPosition();
		UpdateEditorWindowPosition();
	}

	int Draw()
	{
		return 0;
	}

	bool CreateEditWindow()
	{
		bool bRet = true;
		return bRet;;
	}

	int UpdateEditorWindowPosition()
	{
		return 0;
	}

	int DoCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		int ret = 0;

		InitButtons();

		if (false == CreateEditWindow())
			return (-1);

		return 0;
	}

	int DoSize(UINT uMsg, WPARAM wParam, LPARAM lParam, void* lpData = nullptr)
	{
		UpdateButtonPosition();
		UpdateEditorWindowPosition();
		return 0;
	}

};

#endif  /* __WOCHAT_WINDOWS5_H__ */

