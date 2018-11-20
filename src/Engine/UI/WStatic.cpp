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
	((WContainer*)pWStatic)->Create(	lpClassName, 
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

	return pWStatic;
}

void WStatic::onCreateEx(LPVOID lpVoid) {

	int iLen = (int)((lpVoid <= 0) ? 255 : (int)lpVoid);
	m_pText = new char[iLen];
	memset(m_pText, 0, iLen);
	sprintf(m_pText, "%s", getWindowName());

	m_State = NORMAL;
}

void WStatic::onUpdate() {

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
		WWidgetManager::setColor(0xff000000);//ABGR
		WWidgetManager::drawStringFont(m_pText, getLeft(), getTop()+(getHeight()>>1)-6, 0);
		WWidgetManager::resetColor();
	}
}

void WStatic::onMouseDown(int x, int y, int iButton) {
	if(m_pParent) {
		m_pParent->onMessage((H_WND)this, MOUSE_DOWN, getComponentID(), 0);
	}
}

void WStatic::onMouseUp(int x, int y, int iButton) {
	if(m_pParent) {
		m_pParent->onMessage((H_WND)this, MOUSE_UP, getComponentID(), 0);
	}
}

void WStatic::onMouseMove(int mCode, int x, int y, int prevX, int prevY) {
	if(m_pParent) {
		int dwDiffX = (-(prevX-x) & 0xffff);
		int dwDiffY = (-(prevY-y) & 0xffff);
		DWORD dwDiff = (dwDiffX <<16) | dwDiffY;
		m_pParent->onMessage((H_WND)this, MOUSE_MOVE, (mCode<<16)|(getComponentID()), (LPARAM)&dwDiff);
	}
}

void WStatic::onMouseWheel(WPARAM wParam, LPARAM lParam) {

}

void WStatic::onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch) {
	
}

void WStatic::onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch) {

}

void WStatic::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

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