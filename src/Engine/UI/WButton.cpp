#ifdef USE_YAGUI
#include "Engine/UI/WButton.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WWidgetManager.h"

#define BUTTON_GUTTER_X	10
#define BUTTON_TOP_GUTTER	2
unsigned int WButton::BUTTON_TEXT_HEIGHT;

WButton::WButton() {

}

H_WND WButton::Create(		const char* lpClassName, 
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
	WButton* pWButton = new WButton();
	((WContainer*)pWButton)->Create(	lpClassName, 
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

	return pWButton;
}

void WButton::onCreateEx(LPVOID lpVoid) {
	m_pTitle = new char[255];
	memset(m_pTitle, 0, 255);
	sprintf(m_pTitle, "%s", getWindowName());

	BUTTON_TEXT_HEIGHT = WWidgetManager::CHARACTER_HEIGHT + (BUTTON_TOP_GUTTER << 1);

	bool bHasText = (strlen(m_pTitle) > 0);

	m_State = NORMAL;

	m_pButtonStateNameNormal = new char[32];
	sprintf(m_pButtonStateNameNormal, "%s_Normal", (const char*)lpVoid);

	m_pButtonStateNameHighlighted = new char[32];
	sprintf(m_pButtonStateNameHighlighted, "%s_Highlighted", (const char*)lpVoid);

	m_pButtonStateNamePushed = new char[32];
	sprintf(m_pButtonStateNamePushed, "%s_Pushed", (const char*)lpVoid);

	m_pButtonStateNameDisabled = new char[32];
	sprintf(m_pButtonStateNameDisabled, "%s_Disabled", (const char*)lpVoid);
}

void WButton::onUpdate(float deltaTimeMs) {

}

void WButton::frameRender() {
	WComponent::frameRender();
}

void WButton::deactivate() {
	m_State = INACTIVE;
}

void WButton::activate() {
	m_State = NORMAL;
}

void WButton::onRender() {
	
	char* buttonStateName = NULL;

	switch(m_State) {
		case NORMAL:
			buttonStateName = m_pButtonStateNameNormal;
		break;
		case HOVER:
			buttonStateName = m_pButtonStateNameHighlighted;
		break;
		case PUSHED:
			buttonStateName = m_pButtonStateNamePushed;
		break;
		case INACTIVE:
			buttonStateName = m_pButtonStateNameDisabled;
		break;
	}

	RectF renderRect(getLeft(), getTop(), getWidth(), getHeight());
	WWidgetManager::renderWidget(buttonStateName, &renderRect);

	if(strcmp(m_pTitle, "") != 0) {
		WWidgetManager::setColor(0xff000000);//ABGR
		WWidgetManager::drawStringFont(m_pTitle, getLeft() + (getWidth()>>1), getTop() + BUTTON_TOP_GUTTER, 1);
		if(m_State == PUSHED)//Make it look BOLD
			WWidgetManager::drawStringFont(m_pTitle, getLeft() + (getWidth()>>1) + 1, getTop() + BUTTON_TOP_GUTTER, 1);
		WWidgetManager::resetColor();
	}
}

void WButton::onMouseDownEx(int x, int y, int iButton) {
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

void WButton::onMouseUpEx(int x, int y, int iButton) {
	if(m_State != INACTIVE)
		m_State = NORMAL;
		
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

void WButton::onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY) {
	m_State = HOVER;
}

void WButton::onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY) {
	m_State = HOVER;
	WWidgetManager::setCursor(IDC__ARROW);

	if(m_pParent) {
		int dwDiffX = (-(prevX-x) & 0xffff);
		int dwDiffY = (-(prevY-y) & 0xffff);
		DWORD dwDiff = (dwDiffX <<16) | dwDiffY;
		m_pParent->onMessage((H_WND)this, MOUSE_HOVER, (mCode<<16)|(getComponentID()), (LPARAM)&dwDiff);
	}
}

void WButton::onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY) {
	m_State = NORMAL;
}

void WButton::onMouseMoveEx(int mCode, int x, int y, int prevX, int prevY) {
	if(m_pParent) {
		int dwDiffX = (-(prevX-x) & 0xffff);
		int dwDiffY = (-(prevY-y) & 0xffff);
		DWORD dwDiff = (dwDiffX <<16) | dwDiffY;
		m_pParent->onMessage((H_WND)this, MOUSE_MOVE, (mCode<<16)|(getComponentID()), (LPARAM)&dwDiff);
	}
}

void WButton::onMouseWheelEx(WPARAM wParam, LPARAM lParam) {

}

void WButton::onKeyBDownEx(unsigned int iVirtualKeycode, unsigned short ch) {
	
}

void WButton::onKeyBUpEx(unsigned int iVirtualKeycode, unsigned short ch) {

}

void WButton::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

}

LRESULT WButton::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {

	case WM__GETTEXTLENGTH:
		{
			return strlen(m_pTitle);
		}
		break;
	case WM__GETTEXT:
		{
			return sprintf_s((char*)lParam, (size_t)wParam, "%s", m_pTitle);
		}
		break;
	case WM__SETTEXT:
		{
			char* str = (char*)lParam;
			sprintf_s(m_pTitle, strlen(str) + 1, "%s", str);
			return WM__OKAY;
		}
		break;
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

WButton::~WButton() {
	delete[] m_pTitle;

	delete[] m_pButtonStateNameNormal;
	delete[] m_pButtonStateNameHighlighted;
	delete[] m_pButtonStateNamePushed;
	delete[] m_pButtonStateNameDisabled;
}
#endif