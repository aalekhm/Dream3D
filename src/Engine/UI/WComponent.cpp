
#include "Engine/UI/WComponent.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WWidgetManager.h"

WComponent::WComponent() :	m_iLeft(0), 
											m_iTop(0), 
											m_iRight(0), 
											m_iBottom(0),

											m_iOffsetX(0), m_iOffsetY(0),

											m_iMainX(0), m_iMainY(0),

											m_pParent(NULL),
		
											m_bAlwaysOnTop(false),
											m_bVisible(true),
											m_bHasMouseFocus(false),
											m_bHasKeyBFocus(false),
											m_bActive(false),
											m_bEnabled(true),
											m_bFixedOrder(false),
											m_bIsContainer(false),
											m_bIsChild(false),
											m_IsPostRender(false),
											m_bMovable(true),
											m_bIntegralChild(false),
											m_iComponentID(-1),

											m_pClassName(NULL),
											m_pWindowName(NULL)
{
	m_pClassName = new char[255];
	memset(m_pClassName, 0, 255);

	m_pWindowName = new char[255];
	memset(m_pWindowName, 0, 255);
}

H_WND WComponent::Create(		const char* lpClassName, 
												const char* lpWindowName, 
												DWORD dwStyle, 
												int x, 
												int y, 
												int width, 
												int height, 
												H_WND hwndParent, 
												HMENU hMenu, 
												LPVOID lParam,
												bool bIsContainer, 
												bool bIsChild
) {
		memset(m_pClassName, 0, 255);
		sprintf(m_pClassName, "%s", lpClassName);

		setWindowName(lpWindowName);

		m_iMainX = m_iMainY = 0;
		m_iOffsetX = x;
		m_iOffsetY = y;

		m_pParent = (WComponent*)hwndParent;
		if(m_pParent) {
			m_iLeft = m_pParent->getLeft() + m_iOffsetX + m_iMainX;
			m_iTop = m_pParent->getTop() + m_iOffsetY + m_iMainY;
		}

		m_iRight = m_iLeft + width;
		m_iBottom = m_iTop + height;

		setComponentID((int)hMenu);
		setIsContainer(bIsContainer);
		setComponentAsChild(bIsChild);

		if(m_pParent != NULL)
			((WContainer*)m_pParent)->addComponent(this);

		onCreateEx(lParam);
		WWidgetManager::onEvent((H_WND)this , WM__CREATE, getComponentID(), 0);

		return this;
}

const char* WComponent::getClassName() {
	return m_pClassName;
}

const char* WComponent::getWindowName() {
	return m_pWindowName;
}

void WComponent::setWindowName(const char* sWindowName) {
	if(sWindowName != NULL && strlen(sWindowName) > 0) {
		memset(m_pWindowName, 0, 255);
		sprintf(m_pWindowName, "%s", sWindowName);
	}
};

void WComponent::getFocus() {
	getMouseFocus();
	getKeyBFocus();
}

void WComponent::releaseFocus() {
	releaseMouseFocus();
	releaseKeyBFocus();
}

void WComponent::getMouseFocus() {
	m_bHasMouseFocus = true;
}

void WComponent::getKeyBFocus() {
	m_bHasKeyBFocus = true;
}

void WComponent::releaseMouseFocus() {
	m_bHasMouseFocus = false;
}

void WComponent::releaseKeyBFocus() {
	m_bHasKeyBFocus = false;
}

void WComponent::updateComponentPosition() {
	////////////////////////////////////////////////////////////
	int w = getWidth();
	int h = getHeight();

	if(m_pParent) {
		m_iLeft = m_pParent->getLeft() + m_iOffsetX;
		m_iTop = m_pParent->getTop() + m_iOffsetY;

		 if(isComponentAChild()) {
			 m_iLeft += (isMovable() ? m_pParent->m_iMainX : 0);
			 m_iTop += (isMovable() ? m_pParent->m_iMainY : 0);
		 }
	}
	
	m_iRight = m_iLeft + w;
	m_iBottom = m_iTop + h;
	////////////////////////////////////////////////////////////
}

void WComponent::onRender() {

}

void WComponent::onMouseDown(int x, int y, int iButton) {
//printf("WComponent::onMouseDown\n");
}

void WComponent::onMouseEnter(int mCode, int x, int y, int prevX, int prevY) {

}

void WComponent::onMouseHover(int mCode, int x, int y, int prevX, int prevY) {

}

void WComponent::onMouseMove(int mCode, int x, int y, int prevX, int prevY) {

}

void WComponent::onMouseLeave(int mCode, int x, int y, int prevX, int prevY) {

}

void WComponent::onMouseUp(int x, int y, int iButton) {

}

void WComponent::onMouseWheel(WPARAM wParam, LPARAM lParam) {

}

void WComponent::onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch) {

}

void WComponent::onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch) {

}

void WComponent::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

}

bool WComponent::isPointInside(int x, int y) {
	if(x < m_iLeft || x > m_iRight || y < m_iTop || y > m_iBottom)
		return false;

	return true;
}

void WComponent::frameRender() {
	
	if(!isVisible())
		return;

	// Render the component
	onRender();
}

void WComponent::frameUpdate() {
	
	if(!isVisible())
		return;

	updateComponentPosition();

	// Update the component
	onUpdate();
}

LRESULT WComponent::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	return -1;
}

WComponent::~WComponent() {

}