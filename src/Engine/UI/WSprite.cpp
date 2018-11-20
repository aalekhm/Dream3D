#ifdef USE_YAGUI
#include "Engine/UI/WSprite.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WWidgetManager.h"

#define BUTTON_GUTTER_X	10
#define BUTTON_TOP_GUTTER	2

WSprite::WSprite() {

}

H_WND WSprite::Create(		const char* lpClassName, 
										const char* lpWindowName, 
										DWORD dwStyle, 
										int x, 
										int y, 
										int width, 
										int height, 
										H_WND hwndParent, 
										HMENU hMenu, 
										LPVOID lpVoid
) {
	WSprite* pWSprite = new WSprite();
	((WContainer*)pWSprite)->Create(	lpClassName, 
														lpWindowName, 
														dwStyle, 
														x, 
														y, 
														width, 
														height, 
														hwndParent, 
														hMenu, 
														lpVoid,
														false, 
														true);

	return pWSprite;
}

void WSprite::onCreateEx(LPVOID lpVoid) {
	m_State = NORMAL;

	m_pSpriteWidget = new char[32];
	sprintf(m_pSpriteWidget, "%s", (const char*)lpVoid);
}

void WSprite::onUpdate(float deltaTimeMs) {

}

void WSprite::deactivate() {
	m_State = INACTIVE;
}

void WSprite::activate() {
	m_State = NORMAL;
}

void WSprite::onRender() {
	
	WWidgetManager* pWWidgetManager = WWidgetManager::getInstance();
	RectF renderRect(getLeft(), getTop(), getWidth(), getHeight());

	switch(m_State) {
		case NORMAL:
		break;
		case HOVER:
			pWWidgetManager->setColor(0xff00f0f0);//ABGR
		break;
		case PUSHED:
			pWWidgetManager->setColor(0xfff0f000);//ABGR
		break;
		case INACTIVE:
			pWWidgetManager->setColor(0xff808080);//ABGR
		break;
	}
	
	WWidgetManager::renderWidget(m_pSpriteWidget, &renderRect);

	pWWidgetManager->resetColor();
}

void WSprite::onMouseDownEx(int x, int y, int iButton) {
	if(m_State != INACTIVE)
		m_State = PUSHED;

	if(m_pParent) {
		m_pParent->onMessage((H_WND)this, MOUSE_DOWN, getComponentID(), 0);
	}

	if((iButton & MK_LBUTTON) > 0)
		WWidgetManager::onEvent((H_WND)this , WM_BTN_LBUTTONDOWN, getComponentID(), 0);
	else
	if((iButton & MK_MBUTTON) > 0)
		WWidgetManager::onEvent((H_WND)this, WM_BTN_MBUTTONDOWN, getComponentID(), 0);
	else
	if((iButton & MK_RBUTTON) > 0)
		WWidgetManager::onEvent((H_WND)this, WM_BTN_RBUTTONDOWN, getComponentID(), 0);
}

void WSprite::onMouseUpEx(int x, int y, int iButton) {
	if(m_State != INACTIVE) {
		m_State = HOVER;
	}
		
	if(m_pParent) {
		m_pParent->onMessage((H_WND)this, MOUSE_UP, getComponentID(), 0);
	}

	if((iButton & MK_LBUTTON) > 0)
		WWidgetManager::onEvent((H_WND)this, WM_BTN_LBUTTONUP, getComponentID(), 0);
	else
	if((iButton & MK_MBUTTON) > 0)
		WWidgetManager::onEvent((H_WND)this, WM_BTN_MBUTTONUP, getComponentID(), 0);
	else
	if((iButton & MK_RBUTTON) > 0)
		WWidgetManager::onEvent((H_WND)this, WM_BTN_RBUTTONUP, getComponentID(), 0);
}

void WSprite::onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY) {
	m_State = HOVER;
}

void WSprite::onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY) {
	m_State = HOVER;
	WWidgetManager::setCursor(IDC__ARROW);

	if(m_pParent) {
		int dwDiffX = (-(prevX-x) & 0xffff);
		int dwDiffY = (-(prevY-y) & 0xffff);
		DWORD dwDiff = (dwDiffX <<16) | dwDiffY;
		m_pParent->onMessage((H_WND)this, MOUSE_HOVER, (mCode<<16)|(getComponentID()), (LPARAM)&dwDiff);
	}
}

void WSprite::onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY) {
	m_State = NORMAL;
}

void WSprite::onMouseMoveEx(int mCode, int x, int y, int prevX, int prevY) {
	if(m_pParent) {
		int dwDiffX = (-(prevX-x) & 0xffff);
		int dwDiffY = (-(prevY-y) & 0xffff);
		DWORD dwDiff = (dwDiffX <<16) | dwDiffY;
		m_pParent->onMessage((H_WND)this, MOUSE_MOVE, (mCode<<16)|(getComponentID()), (LPARAM)&dwDiff);
	}
}

void WSprite::onMouseWheelEx(WPARAM wParam, LPARAM lParam) {

}

void WSprite::onKeyBDownEx(unsigned int iVirtualKeycode, unsigned short ch) {
	
}

void WSprite::onKeyBUpEx(unsigned int iVirtualKeycode, unsigned short ch) {

}

void WSprite::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

}

LRESULT WSprite::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {

	case WM__GETRECT:
		{
			Rect* listRect = (Rect*)lParam;
			listRect->X = getLeft();
			listRect->Y = getTop();
			listRect->Width = getWidth();
			listRect->Height = getHeight();

			return WM__OKAY;
		}
		break;
	case WM__GETPOS:
		{
			DWORD* dwPos = (DWORD*)lParam;
			*dwPos |= (getLeft() & 0xffff);
			*dwPos |= ((getTop() & 0xffff) << 16);

			return WM__OKAY;
		}
		break;	
	case WM__SETPOS:
		{
			DWORD dwPos = (DWORD)lParam;
			int x = (dwPos & 0xffff);
			int y = ((dwPos >> 16) & 0xffff);
			setPosition((dwPos & 0xffff), ((dwPos >> 16) & 0xffff));
			return WM__OKAY;
		}
		break;
	case BM__CLICK:
		{
			Rect* bounds = (Rect*)lParam;
			onMouseDown(bounds->X + (bounds->Width >> 1), bounds->Y + (bounds->Height >> 1), MK_LBUTTON);
			onMouseUp(bounds->X + (bounds->Width >> 1), bounds->Y + (bounds->Height >> 1), MK_LBUTTON);

			return WM__OKAY;
		}
		break;
	case BM__ENABLE:
		{
			activate();
			return WM__OKAY;
		}
		break;
	case BM__DISABLE:
		{
			deactivate();
			return WM__OKAY;
		}
		break;
	case BM__GET_STATE:
		{
			return (m_State == INACTIVE)?0:1;
		}
		break;
	}

	return WM__ERR;
}

WSprite::~WSprite() {

	delete[] m_pSpriteWidget;
}
#endif