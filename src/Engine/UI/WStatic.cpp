#include "Engine/UI/WStatic.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WWidgetManager.h"

WStatic::WStatic() {
	setComponentAsChild(false);
}

H_WND WStatic::Create(		const char* lpClassName, 
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
	WStatic* pWStatic = new WStatic();
	return pWStatic->createWindow(lpClassName, lpWindowName, dwStyle, x, y, width, height, hwndParent, hMenu, lpVoid);
}

H_WND WStatic::createWindow(	const char* lpClassName, 
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
	sprintf(m_pClassName, "%s", lpClassName);

	int iLen = (int)((lpVoid <= 0) ? 255 : (int)lpVoid);
	m_pText = new char[iLen];
	memset(m_pText, 0, iLen);
	sprintf(m_pText, "%s", lpWindowName);

	m_HwndID = (int)hMenu;
	m_pParent = (WContainer*)hwndParent;

	m_iOffsetX = x;
	m_iOffsetY = y;

	m_iLeft = m_pParent->getLeft() + m_iOffsetX + m_pParent->m_iMainX;
	m_iTop = m_pParent->getTop() + m_iOffsetY + m_pParent->m_iMainY;
	m_iRight = m_iLeft + width;
	m_iBottom = m_iTop + height;

	m_State = NORMAL;

	setComponentID((int)hMenu);
	setComponentAsChild(true);
	((WContainer*)m_pParent)->addComponent(this);

	return this;
}

void WStatic::onUpdate() {
	updateComponentPosition();
}

void WStatic::frameRender() {
	WComponent::frameRender();
}

void WStatic::deactivate() {
	m_State = INACTIVE;
}

void WStatic::activate() {
	m_State = NORMAL;
}

void WStatic::onRender() {
	if(strcmp(m_pText, "") != 0) {
		WWidgetManager::drawStringFont(m_pText, getLeft(), getTop()+(getHeight()>>1)-6, 0);
	}
}

void WStatic::onMouseDown(int x, int y, int iButton) {
	if(m_pParent) {
		m_pParent->onMessage(MOUSE_DOWN, this->m_HwndID, 0);
	}
}

void WStatic::onMouseUp(int x, int y, int iButton) {
	if(m_pParent) {
		m_pParent->onMessage(MOUSE_UP, this->m_HwndID, 0);
	}
}

void WStatic::onMouseMove(int mCode, int x, int y, int prevX, int prevY) {
	if(m_pParent) {
		m_pParent->onMessage(MOUSE_MOVE, (mCode<<16)|(this->m_HwndID), ((-(prevX-x))<<16) | (-(prevY-y)));
	}
}

void WStatic::onMouseWheel(WPARAM wParam, LPARAM lParam) {

}

void WStatic::onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch) {
	
}

void WStatic::onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch) {

}

void WStatic::onMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

}

LRESULT WStatic::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {

		case WM__GETTEXTLENGTH:
		{
			return strlen(m_pText);
		}
		break;
		case WM__GETTEXT:
		{
			return sprintf_s((char*)lParam, (size_t)wParam, "%s", m_pText);
		}
		break;
		case WM__SETTEXT:
		{
			char* str = (char*)lParam;
			sprintf_s(m_pText, strlen(str) + 1, "%s", str);
			return 1;
		}
		break;
	}

	return -1;
}

WStatic::~WStatic() {
	delete[] m_pText;
}