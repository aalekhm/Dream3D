#ifdef USE_YAGUI
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WWidgetManager.h"

WContainer::WContainer() : m_pActiveComponent(0) {
	m_IsPostRender = false;
	m_iComponentID = -1;
}

H_WND WContainer::Create(		const char* lpClassName, 
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
	WComponent::Create(	lpClassName, 
									lpWindowName, 
									dwStyle, 
									x, 
									y, 
									width, 
									height, 
									hwndParent, 
									hMenu, 
									lParam,
									bIsContainer, 
									bIsChild);

	return this;
}

WContainer::~WContainer() {
	
	std::vector<WComponent*>::iterator itrBegin = m_pChildren.begin();
	int size = m_pChildren.size();
	int i = size - 1;
	while(i >= 0) {
		WComponent* pComp = m_pChildren[i];
		SAFE_DELETE(pComp);

		m_pChildren.erase(itrBegin + i);
		i--;
	}
}

void WContainer::frameUpdate() {

	if(!isVisible())
		return;

	updateComponentPosition();

	// Update the container itself
	onUpdate();

	//update the childrens too
	for(size_t i = 0; i < m_pChildren.size(); i++) {
		m_pChildren[i]->frameUpdate();
	}
}

void WContainer::frameRender() {
	
	if(!isVisible() || !canUpdateOrRender()) {
		return;
	}

	RectF reclaimRect;
	WWidgetManager::GetClipBounds(&reclaimRect);

	// Render the container itself
	if(WWidgetManager::onEvent((H_WND)this , WN__PAINT, getComponentID(), (LPARAM)&reclaimRect) <= 0) 	{
		onRender();
	}
	else {
		WWidgetManager::getInstance()->setGLStates();
	}

	///////////////////////////////////////////////////
	bool bRenderChilds = true;
	if(m_ClientRect.Width > 0) {
		RectF clipRectForChilds(	getLeft() + m_ClientRect.X,
											getTop() + m_ClientRect.Y,
											m_ClientRect.Width,
											m_ClientRect.Height);

		RectF clipRect;
		if(m_pParent && m_pParent->isContainer()) {
			if(m_ClientRect.Height <= 0)
				bRenderChilds = false;
			else
				clipRect.Intersect(clipRect, reclaimRect, clipRectForChilds);
		}
		else {
			clipRect = clipRectForChilds;
		}
		
		WWidgetManager::setClip(clipRect.X, clipRect.Y, clipRect.Width, clipRect.Height);
	}
	///////////////////////////////////////////////////

	//render the children's too
	if(bRenderChilds) {
		for(size_t i = 0; i < m_pChildren.size(); i++) {
			if(m_pChildren[i]->isVisible() && !m_pChildren[i]->isPostRender()) {
				m_pChildren[i]->frameRender();
			}
		}
	}
	
	WWidgetManager::setClip(reclaimRect.X, reclaimRect.Y, reclaimRect.Width, reclaimRect.Height);

	postRender();
}

void WContainer::postRender() {
	for(size_t i = 0; i < m_pChildren.size(); i++) {
		if(m_pChildren[i]->isVisible() && m_pChildren[i]->isPostRender())
			m_pChildren[i]->frameRender();
	}

	postRenderEx();
}

bool WContainer::canUpdateOrRender() {

	RectF reclaimRect;
	WWidgetManager::GetClipBounds(&reclaimRect);

	RectF testRect;
	RectF myRect(getLeft(), getTop(), getWidth(), getHeight());
	testRect.Intersect(testRect, reclaimRect, myRect);
	bool bRenderMe = (testRect.Width > 0 && testRect.Height > 0);

	return bRenderMe;
}

void WContainer::onMouseDown(int x, int y, int iButton) {
//printf("WContainer::onMouseDown\n");

	int activeComponent = -1;

	// Check touches with the "Integral" components of a Container(For Eg. ScrollBar of a WFrame).
	// An "Integral" component is the one which sits with the Container.
	// It is a child of the Container but treated as the Container itself.
	for(int i = 0; i < (int)m_pChildren.size(); i++) {
		WComponent* pComponent = m_pChildren[i];
		if (pComponent->isVisible() && pComponent->isIntegral()) {
			if (pComponent->isPointInside(x, y)) {
				activeComponent = i;
				break;
			}
		}
	}

	// select active component
	// traversal is done in z-order (starting from the end of the
	// list where the top child is) in order to eliminate problems
	// due to overlapping
	if(activeComponent == -1) {
		for(int i = (int)m_pChildren.size()-1; i >= 0; i--) {
			WComponent* pComponent = m_pChildren[i];
			if (pComponent->isVisible()) {
				if (pComponent->isPointInside(x, y)) {
					activeComponent = i;
					break;
				}
			}
		}
	}

	// Change the current active component
	WComponent* pOldActiveComponent = m_pActiveComponent;
	m_pActiveComponent = (activeComponent >= 0)?m_pChildren[activeComponent]:0;

	// Deactive the previous active component
	if(pOldActiveComponent && pOldActiveComponent != m_pActiveComponent) {
		pOldActiveComponent->releaseFocus();
		pOldActiveComponent->onDeactivate();

//printf("pOldActiveComponent = %d\n", pOldActiveComponent);
	}

	if(m_pActiveComponent) {
		// Bring the new active component on top (method respects
		// fixedZOrder flag)
		//bringChildToFront(activeComponent);//TODO Later

		// Activate the new component
		m_pActiveComponent->onActivate();
//printf("m_pActiveComponent = %d\n", m_pActiveComponent);
		// switch focus from this component to the child
		//releaseFocus();
		m_pActiveComponent->getFocus();

		// get the local coordinates inside the child component:
		m_pActiveComponent->onMouseDown(x, y, iButton);
	}
	else {
		// Custom mouse handling
		onMouseDownEx(x, y, iButton);
	}
}

void WContainer::onMouseUp(int x, int y, int iButton) {

	// call mouseUp of active component
	// (without updating the active component)
	if (m_pActiveComponent) {

		// Transform coordinates.
		m_pActiveComponent->onMouseUp(x, y, iButton);
	}
	else {

		// custom handling
		onMouseUpEx(x, y, iButton);
	}
}

void WContainer::onMouseEnter(int mCode, int x, int y, int prevX, int prevY) {
	for(int i = (int)m_pChildren.size()-1; i >= 0; i--) {
		if( m_pChildren[i]->isPointInside(x, y) ) {
			if( !m_pChildren[i]->isPointInside(prevX, prevY) )
				m_pChildren[i]->onMouseEnter(mCode, x, y, prevX, prevY);
		}
	}

	onMouseEnterEx(mCode, x, y, prevX, prevY);
}

void WContainer::onMouseHover(int mCode, int x, int y, int prevX, int prevY) {
	for(int i = (int)m_pChildren.size()-1; i >= 0; i--) {
		if(m_pChildren[i]->isPointInside(x, y)) {
			if(!m_pChildren[i]->isPointInside(prevX, prevY)) {
				m_pChildren[i]->onMouseEnter(mCode, x, y, prevX, prevY);
			}
			else {
					m_pChildren[i]->onMouseHover(mCode, x, y, prevX, prevY);
			}
		}
		else {
			if(m_pChildren[i]->isPointInside(prevX, prevY)) {
				m_pChildren[i]->onMouseLeave(mCode, x, y, prevX, prevY);
			}
		}
	}

	onMouseHoverEx(mCode, x, y, prevX, prevY);
}

void WContainer::onMouseLeave(int mCode, int x, int y, int prevX, int prevY) {
	for(int i = (int)m_pChildren.size()-1; i >= 0; i--) {
		if( m_pChildren[i]->isPointInside(prevX, prevY) ) {
			if( !m_pChildren[i]->isPointInside(x, y) )
				m_pChildren[i]->onMouseLeave(mCode, x, y, prevX, prevY);
		}
	}

	onMouseLeaveEx(mCode, x, y, prevX, prevY);
}

void WContainer::onMouseMove(int mCode, int x, int y, int prevX, int prevY) {
	if(!m_pActiveComponent || (m_pActiveComponent && !m_pActiveComponent->hasMouseFocus())) {
		for(int i = (int)m_pChildren.size()-1; i >= 0; i--) {
			
			if(m_pChildren[i]->isPointInside(x, y)) {
				m_pChildren[i]->onMouseMove(mCode, x, y, prevX, prevY);
			}
		}
	}

	// call mouseMove of active component
	// (without updating the active component)
	if(m_pActiveComponent) {
		
		if(m_pActiveComponent->hasMouseFocus()) {
			m_pActiveComponent->onMouseMove(mCode, x, y, prevX, prevY);
		}
	}
	else {
		// custom handling
		onMouseMoveEx(mCode, x, y, prevX, prevY);
	}
}

void WContainer::onMouseMoveEx(int mCode, int x, int y, int prevX, int prevY) {

	// Drag window around:
	int diffX = (x - prevX);
	int diffY = (y - prevY);

	if((mCode & MK_LBUTTON) != 0) {
		if(m_pParent != NULL) {
			setPosition(getOffsetX() + diffX, getOffsetY() + diffY);
		}
	}
}

void WContainer::onMouseWheel(WPARAM wParam, LPARAM lParam) {
//printf("WContainer::onMouseWheel\n");
	// call onMouseWheel of active component
	// (without updating the active component)
	if (m_pActiveComponent) {

		// Transform coordinates.
		m_pActiveComponent->onMouseWheel(wParam, lParam);
	}
	else {

		// custom handling
		onMouseWheelEx(wParam, lParam);
	}
}

void WContainer::onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch) {
	// if key is tab, change the active component
	// to the next in the tab order
	//TODO

	if(m_pActiveComponent) {
		m_pActiveComponent->onKeyBDown(iVirtualKeycode, ch);
	}
	else
		onKeyBDownEx(iVirtualKeycode, ch);
}

void WContainer::onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch) {
	if(m_pActiveComponent) {
		m_pActiveComponent->onKeyBUp(iVirtualKeycode, ch);
	}
	else
		onKeyBUpEx(iVirtualKeycode, ch);
}

void WContainer::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

}

void WContainer::bringChildToFront(int id) {
	WComponent* pComp = m_pChildren[id];
	if(pComp->hasFixedZOrder())
		return;

	// Change the order in the children array to bring the child to the top
	// (top child is the last in the array)
	// respect alwaysOnTop components.
	int numChildrens = (int)m_pChildren.size();
	int i = -1;
	for(i = id; i < numChildrens-1; i++) {
		if(m_pChildren[i]->isAlwaysOnTop())
			break;

		m_pChildren[i] = m_pChildren[i+1];
	}

	m_pChildren[i] = pComp;
}

void WContainer::addComponent(WComponent* pC) {
	
	// top child is the last in the array. Respect alwaysOnTop
	// flags when adding the child
	if(m_pChildren.size() == 0) {
		m_pChildren.push_back(pC);
	}
	else {
		int i = (int)m_pChildren.size();

		if(!pC->isAlwaysOnTop()) {
			//for (; i >= 0; --i) {
			//	if (!m_pChildren[i]->isAlwaysOnTop())
			//		break;
			//}
		}

		//m_pChildren.insert(m_pChildren.begin()+(i+1), pC);
		m_pChildren.push_back(pC);
	}

	pC->setParent(this);
}

void WContainer::removeComponent(WComponent *pC)
{
	for (size_t i = 0; i < m_pChildren.size(); ++i) {
		if (m_pChildren[i] == pC) {
			m_pChildren.erase(m_pChildren.begin() + i);
			return;
		}
	}
}

void WContainer::addBaseSkinChild(CHILD* pC) {
	if(pC == NULL)
		return;

	m_pBaseSkinChilds.push_back(pC);
}

void WContainer::removeBaseSkinChild(CHILD* pC) {
	if(pC == NULL)
		return;

	for (size_t i = 0; i < m_pBaseSkinChilds.size(); ++i) {
		if (m_pBaseSkinChilds[i] == pC) {
			m_pBaseSkinChilds.erase(m_pBaseSkinChilds.begin() + i);
			return;
		}
	}
}

H_WND WContainer::GetWindow(UINT ID_WINDOW) {

	for (size_t i = 0; i < m_pChildren.size(); ++i) {
		WComponent* comp = m_pChildren[i];
		if(comp->getComponentID() == ID_WINDOW)
			return m_pChildren[i];
		else
		if(comp->isContainer()) {
			H_WND hWnd = ((WContainer*)comp)->GetWindow(ID_WINDOW);
			if(hWnd != NULL)
				return hWnd;
		}
	}

	return NULL;
}

H_WND WContainer::FindWindowQ(LPCSTR lpClassName, LPCSTR lpWindowName) {

	for (size_t i = 0; i < m_pChildren.size(); ++i) {
		WComponent* comp = m_pChildren[i];
		
		if(strcmp(comp->getClassName(), lpClassName) == 0 && strcmp(comp->getWindowName(), lpWindowName) == 0)
			return m_pChildren[i];
		else
			if(comp->isContainer()) {
				H_WND hWnd = ((WContainer*)comp)->FindWindowQ(lpClassName, lpWindowName);
				if(hWnd != NULL)
					return hWnd;
			}
	}

	return NULL;
}

int WContainer::getIndex(const WComponent* pComp) {

	int iRet = -1;
	for (size_t i = 0; i < m_pChildren.size(); ++i) {
		WComponent* comp = m_pChildren[i];
		if(pComp == comp) {
			iRet = i;
			break;
		}
	}

	return iRet;
}
#endif