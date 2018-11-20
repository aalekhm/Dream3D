#ifdef USE_YAGUI
#include "Engine/UI/WCheckbox.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WWidgetManager.h"

#define CHECKBOX_STRING_STARTX 25
#define CHECKBOX_TOP_GUTTER 2

unsigned int WCheckbox::CHECKBOX_TEXT_HEIGHT;

WCheckbox::WCheckbox() {

}

H_WND WCheckbox::Create(		const char* lpClassName, 
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
	WCheckbox* pWCheckbox = new WCheckbox();
	((WContainer*)pWCheckbox)->Create(	lpClassName, 
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

	return pWCheckbox;
}

void WCheckbox::onCreateEx(LPVOID lpVoid) {

	m_pTitle = new char[255];
	memset(m_pTitle, 0, 255);
	sprintf(m_pTitle, "%s", getWindowName());

	CHECKBOX_TEXT_HEIGHT = WWidgetManager::CHARACTER_HEIGHT + (CHECKBOX_TOP_GUTTER << 1);

	m_iLeft = m_pParent->getLeft() + m_iOffsetX + m_pParent->m_iMainX;
	m_iTop = m_pParent->getTop() + m_iOffsetY + m_pParent->m_iMainY;
	m_iRight = m_iLeft + CHECKBOX_STRING_STARTX + WWidgetManager::getStringWidthTillPos(m_pTitle, strlen(m_pTitle));
	m_iBottom = m_iTop + CHECKBOX_TEXT_HEIGHT;

	m_bChecked = false;
	m_State = NORMAL;

	m_pCBCheckedStateNameNormal = new char[32];
	sprintf(m_pCBCheckedStateNameNormal, "CheckBox_Normal_Checked");

	m_pCBCheckedStateNameHighlighted = new char[32];
	sprintf(m_pCBCheckedStateNameHighlighted, "CheckBox_Highlighted_Checked");

	m_pCBCheckedStateNamePushed = new char[32];
	sprintf(m_pCBCheckedStateNamePushed, "CheckBox_Pushed_Checked");

	m_pCBCheckedStateNameDisabled = new char[32];
	sprintf(m_pCBCheckedStateNameDisabled, "CheckBox_Inactive_Checked");

	m_pCBUnCheckedStateNameNormal = new char[32];
	sprintf(m_pCBUnCheckedStateNameNormal, "CheckBox_Normal");

	m_pCBUnCheckedStateNameHighlighted = new char[32];
	sprintf(m_pCBUnCheckedStateNameHighlighted, "CheckBox_Highlighted");

	m_pCBUnCheckedStateNamePushed = new char[32];
	sprintf(m_pCBUnCheckedStateNamePushed, "CheckBox_Pushed");

	m_pCBUnCheckedStateNameDisabled = new char[32];
	sprintf(m_pCBUnCheckedStateNameDisabled, "CheckBox_Inactive");
}

void WCheckbox::deactivate() {
	m_State = INACTIVE;
}

void WCheckbox::activate() {
	m_State = NORMAL;
}

void WCheckbox::onUpdate() {

}

void WCheckbox::onRender() {
	
	char* checkButtonStateName = NULL;

	if(m_bChecked) {
		switch(m_State) {
			case NORMAL:
				checkButtonStateName = m_pCBCheckedStateNameNormal;
			break;
			case HIGHLIGHTED:
				checkButtonStateName = m_pCBCheckedStateNameHighlighted;
			break;
			case PUSHED:
				checkButtonStateName = m_pCBCheckedStateNamePushed;
			break;
			case INACTIVE:
				checkButtonStateName = m_pCBCheckedStateNameDisabled;
			break;
		}
	}
	else {
		switch(m_State) {
			case NORMAL:
				checkButtonStateName = m_pCBUnCheckedStateNameNormal;
			break;
			case HIGHLIGHTED:
				checkButtonStateName = m_pCBUnCheckedStateNameHighlighted;
			break;
			case PUSHED:
				checkButtonStateName = m_pCBUnCheckedStateNamePushed;
			break;
			case INACTIVE:
				checkButtonStateName = m_pCBUnCheckedStateNameDisabled;
			break;
		}
	}
	
	WIDGET* widget = WWidgetManager::getWidget(checkButtonStateName);
	RectF renderRect(getLeft(), getTop() + (CHECKBOX_TEXT_HEIGHT /2) - (widget->widgetSize.height / 2), getWidth(), getHeight());
	WWidgetManager::renderWidget(checkButtonStateName, &renderRect);
	
	WWidgetManager::setColor(0xff000000);//ABGR
	WWidgetManager::drawStringFont(m_pTitle, getLeft() + CHECKBOX_STRING_STARTX, getTop() + CHECKBOX_TOP_GUTTER, 0);
	if(m_bChecked)//Make it look BOLD
		WWidgetManager::drawStringFont(m_pTitle, getLeft() + CHECKBOX_STRING_STARTX + 1, getTop() + CHECKBOX_TOP_GUTTER, 0);
	WWidgetManager::resetColor();
}

void WCheckbox::onMouseDown(int x, int y, int iButton) {
	if(m_State == INACTIVE)
		return;

	m_State = PUSHED;
}

void WCheckbox::onMouseUp(int x, int y, int iButton) {
	if(m_State == INACTIVE)
		return;
	
	if(isPointInside(x, y)) {
		m_bChecked = !m_bChecked;
		
		WWidgetManager::onEvent(this, m_bChecked ? WM_CBX_CHECKED : WM_CBX_UNCHECKED, getComponentID(), 0);
	}

	m_State = NORMAL;
}

void WCheckbox::onMouseEnter(int mCode, int x, int y, int prevX, int prevY) {
	m_State = HIGHLIGHTED;
}

void WCheckbox::onMouseHover(int mCode, int x, int y, int prevX, int prevY) {
	m_State = HIGHLIGHTED;
}

void WCheckbox::onMouseLeave(int mCode, int x, int y, int prevX, int prevY) {
	m_State = NORMAL;
}

void WCheckbox::onMouseMove(int mCode, int x, int y, int prevX, int prevY) {
	if(	!(	m_State == INACTIVE
			||
			m_State == PUSHED
		)
	)
		m_State = HIGHLIGHTED;
}

void WCheckbox::onMouseWheel(WPARAM wParam, LPARAM lParam) {

}

void WCheckbox::onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch) {
	
}

void WCheckbox::onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch) {

}

void WCheckbox::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

}

LRESULT WCheckbox::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

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
			return 1;
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
			return 1;
		}
		break;
	case BM__DISABLE:
		{
			deactivate();
			return 1;
		}
		break;
	case BM__GET_STATE:
		{
			return (m_State == INACTIVE)?0:1;
		}
		break;
	}

	return -1;
}

WCheckbox::~WCheckbox() {
	delete[] m_pTitle;

	delete[] m_pCBCheckedStateNameNormal;
	delete[] m_pCBCheckedStateNameHighlighted;
	delete[] m_pCBCheckedStateNamePushed;
	delete[] m_pCBCheckedStateNameDisabled;

	delete[] m_pCBUnCheckedStateNameNormal;
	delete[] m_pCBUnCheckedStateNameHighlighted;
	delete[] m_pCBUnCheckedStateNamePushed;
	delete[] m_pCBUnCheckedStateNameDisabled;
}
#endif