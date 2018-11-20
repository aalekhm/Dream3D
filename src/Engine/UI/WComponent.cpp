
#include "Engine/UI/WComponent.h"
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

const char* WComponent::getClassName() {
	return m_pClassName;
}

const char* WComponent::getWindowName() {
	return m_pWindowName;
}

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
	if(m_pParent && isComponentAChild()) {
		int w = getWidth();
		int h = getHeight();
		m_iLeft = m_pParent->getLeft() + m_iOffsetX + (isMovable() ? m_pParent->m_iMainX : 0);
		m_iTop = m_pParent->getTop() + m_iOffsetY + (isMovable() ? m_pParent->m_iMainY : 0);
		m_iRight = m_iLeft + w;
		m_iBottom = m_iTop + h;
	}
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

void WComponent::onMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

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

	if(m_pParent) {
		int w = getWidth();
		int h = getHeight();

		m_iLeft = m_pParent->getLeft() + m_iOffsetX;
		m_iTop = m_pParent->getTop() + m_iOffsetY;

		m_iRight = m_iLeft + w;
		m_iBottom = m_iTop + h;
	}

	// Update the component
	onUpdate();
}

LRESULT WComponent::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	return -1;
}

WComponent::~WComponent() {

}