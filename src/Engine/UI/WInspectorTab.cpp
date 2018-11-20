#ifdef USE_YAGUI
#include "Engine/UI/WInspectorTab.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/UI/WWidgetManager.h"

WInspectorTab::WInspectorTab() :
	m_bLButtonDown(false)
{
	setIsContainer(true);
	m_iMainX = m_iMainY = 0;

	m_iState = STATE_INSPECTOR_OPEN;
}

WInspectorTab::~WInspectorTab() {

}

H_WND WInspectorTab::Create(		const char* lpClassName, 
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
	WInspectorTab* pWInspectorTab = new WInspectorTab();
	((WContainer*)pWInspectorTab)->Create(	lpClassName, 
																lpWindowName, 
																dwStyle, 
																x, 
																y, 
																width, 
																height, 
																hwndParent, 
																hMenu, 
																lpVoid,
																true, 
																true);

	return pWInspectorTab;
}

void WInspectorTab::onCreateEx(LPVOID lpVoid) {
	
	H_WND hWnd = NULL;

	RectF vDestRect;
	RectF hDestRect;
	RectF wndRect;
	RectF idealRect;

	m_InspectorTabCWidget = WWidgetManager::getWidget("InspectorTabC_Scroll");

	///////////////////////////////////////////////////
	CHILD* btnChild = m_InspectorTabCWidget->getChild("ButtonPlus");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = btnChild->posOffsets.x;
		idealRect.Y = btnChild->posOffsets.y;
		idealRect.Width = btnChild->posOffsets.w; 
		idealRect.Height = btnChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	hDestRect,
											m_InspectorTabCWidget->widgetSize.width,
											m_InspectorTabCWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											btnChild->align.iHAlign,
											btnChild->align.iVAlign
										);
	hWnd = 
	CreateComponent(	"WButton", 
								"", 
								0, 
								hDestRect.X - m_iLeft,
								hDestRect.Y - m_iTop, 
								hDestRect.Width, 
								hDestRect.Height,
								this, 
								HMENU(BTN_PLUS), 
								"ButtonPlus");
	m_ButtonExpand = (WButton*)hWnd;
	m_ButtonExpand->setPostRender(true);
	m_ButtonExpand->setMovable(true);
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	btnChild = m_InspectorTabCWidget->getChild("ButtonMinus");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = btnChild->posOffsets.x;
		idealRect.Y = btnChild->posOffsets.y;
		idealRect.Width = btnChild->posOffsets.w; 
		idealRect.Height = btnChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	hDestRect,
											m_InspectorTabCWidget->widgetSize.width,
											m_InspectorTabCWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											btnChild->align.iHAlign,
											btnChild->align.iVAlign
										);
	hWnd = 
	CreateComponent(	"WButton", 
								"", 
								0, 
								hDestRect.X - m_iLeft,
								hDestRect.Y - m_iTop, 
								hDestRect.Width, 
								hDestRect.Height,
								this, 
								HMENU(BTN_MINUS), 
								"ButtonMinus");
	m_ButtonCollapse = (WButton*)hWnd;
	m_ButtonCollapse->setPostRender(true);
	m_ButtonCollapse->setMovable(true);
	m_ButtonCollapse->setVisible(false);
	///////////////////////////////////////////////////
	
	///////////////////////////////////////////////////
	bool bHasClientArea = (m_InspectorTabCWidget->clientAreas.size() > 0);
	if(bHasClientArea) {
		CLIENT_AREA* clientArea = m_InspectorTabCWidget->getClientAreaAt(0);
		RectF destRect;
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = clientArea->posOffsets.x;
		idealRect.Y = clientArea->posOffsets.y;
		idealRect.Width = clientArea->posOffsets.w; 
		idealRect.Height = clientArea->posOffsets.h;
		WWidgetManager::getDestinationRect(	destRect,
												m_InspectorTabCWidget->widgetSize.width,
												m_InspectorTabCWidget->widgetSize.height,
												&wndRect,
												&idealRect,
												clientArea->align.iHAlign,
												clientArea->align.iVAlign
												);
		m_ClientRect.X = destRect.X - getLeft();
		m_ClientRect.Y = destRect.Y - getTop();
		m_ClientRect.Width = destRect.Width;
		m_ClientRect.Height = destRect.Height;
	}
	///////////////////////////////////////////
}

void WInspectorTab::onUpdate() {

	m_iMaxWidthPixels = m_iMaxHeightPixels = 0;
	for(int i = 0; i < m_pChildren.size(); i++) {
		WComponent* comp = (WComponent*)m_pChildren[i];
		if(comp->isComponentAChild()) {
			int iRight = comp->getOffsetX() + comp->getWidth();
			if(iRight > m_iMaxWidthPixels)
				m_iMaxWidthPixels = iRight;

			int iBottom = comp->getOffsetY() + comp->getHeight();
			if(iBottom > m_iMaxHeightPixels)
				m_iMaxHeightPixels = iBottom;
		}
	}

	//ANIMATE//////////////////////////
	switch(m_iState) {
		case STATE_INSPECTOR_OPEN:
		{
			m_ButtonExpand->setVisible(false);
			m_ButtonCollapse->setVisible(true);

			m_iRate = 1;
		}
		break;
		case STATE_INSPECTOR_CLOSING:
		{
			m_iRate += 1;
			if(m_iRate > 2) m_iRate = 2;

			setHeight(getHeight() - m_iRate);
			m_ClientRect.Height = getHeight() - INSPECTOR_TITLEBAR_HEIGHT - INSPECTOR_BOTTOM_DECORATION;

			if(getHeight() <= INSPECTOR_TITLEBAR_HEIGHT) {
				setHeight(INSPECTOR_TITLEBAR_HEIGHT);

				//Set Client Height to '0' so that childrens are not drawn in WContainer::frameRender().
				m_ClientRect.Height = 0;

				setState(STATE_INSPECTOR_CLOSED);
			}
		}
		break;
		case STATE_INSPECTOR_CLOSED:
		{
			m_ButtonExpand->setVisible(true);
			m_ButtonCollapse->setVisible(false);

			m_iRate = 1;
		}
		break;
		case STATE_INSPECTOR_OPENING:
		{
			m_iRate += 1;
			if(m_iRate > 2) m_iRate = 2;

			setHeight(getHeight() + m_iRate);
			m_ClientRect.Height = getHeight() - INSPECTOR_TITLEBAR_HEIGHT - INSPECTOR_BOTTOM_DECORATION;

			if(getHeight() >= m_iMaxHeightPixels + INSPECTOR_TITLEBAR_HEIGHT) {
				setHeight(m_iMaxHeightPixels + INSPECTOR_TITLEBAR_HEIGHT);
				m_ClientRect.Height = getHeight() - INSPECTOR_TITLEBAR_HEIGHT - INSPECTOR_BOTTOM_DECORATION;

				setState(STATE_INSPECTOR_OPEN);
			}
		}
		break;
	}
	///////////////////////////////////
}

void WInspectorTab::onRender() {
	WWidgetManager* renderer =  WWidgetManager::getInstance();

	RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
		
	CHILD* cWnd = m_InspectorTabCWidget->getChild("InspectorTabC");
	renderer->renderChild(m_InspectorTabCWidget, cWnd, &thisWndRect);
	//renderer->renderClientArea(m_InspectorTabCWidget, 0, &thisWndRect);
}

void WInspectorTab::onMouseDownEx(int x, int y, int iButton) {

	// Check if click was on the title bar
	// Check if click was on the border and the window is resizable
	//TODO

	// Click was on the empty space, and window is movable.
	// Start moving.
	m_bLButtonDown = true;
}

void WInspectorTab::onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy) {

}

void WInspectorTab::onMouseUpEx(int x, int y, int iButton) {
	// stop dragging or
	// ..stop resizing
	m_bLButtonDown = false;
}

void WInspectorTab::onMouseWheelEx(WPARAM wParam, LPARAM lParam){

}

bool WInspectorTab::isPtInside(int x, int y) {
	// x,y in local coordinates!

	// Check title bar and related gap!
	//TODO

	if (x < m_iLeft+12 || x > m_iRight-14)
		return false;
	if (y < m_iTop+12 || y > m_iBottom-14)
		return false;

	return true;
}

void WInspectorTab::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case MOUSE_UP:
		{
			int buttonID = wParam;
			switch(wParam) {
				case BTN_PLUS:
					setState(STATE_INSPECTOR_OPENING);
				break;
				case BTN_MINUS:
					setState(STATE_INSPECTOR_CLOSING);
				break;
			}
		}
		break;
		case WIDTH_CHANGED:
		{
			int iLastWidth = (int)wParam;
			int iNewWidth = (int)lParam;

			RectF destRect;
			RectF wndRect;
			RectF idealRect;

			CHILD* btnChild = m_InspectorTabCWidget->getChild("ButtonPlus");
				wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
				idealRect.X = btnChild->posOffsets.x;
				idealRect.Y = btnChild->posOffsets.y;
				idealRect.Width = btnChild->posOffsets.w; 
				idealRect.Height = btnChild->posOffsets.h;
			WWidgetManager::getDestinationRect(	destRect,
													m_InspectorTabCWidget->widgetSize.width,
													m_InspectorTabCWidget->widgetSize.height,
													&wndRect,
													&idealRect,
													btnChild->align.iHAlign,
													btnChild->align.iVAlign
												);
			//m_ButtonExpand->setPositionX(destRect.X - m_iLeft);

			btnChild = m_InspectorTabCWidget->getChild("ButtonMinus");
				wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
				idealRect.X = btnChild->posOffsets.x;
				idealRect.Y = btnChild->posOffsets.y;
				idealRect.Width = btnChild->posOffsets.w; 
				idealRect.Height = btnChild->posOffsets.h;
			WWidgetManager::getDestinationRect(	destRect,
													m_InspectorTabCWidget->widgetSize.width,
													m_InspectorTabCWidget->widgetSize.height,
													&wndRect,
													&idealRect,
													btnChild->align.iHAlign,
													btnChild->align.iVAlign
												);
			//m_ButtonCollapse->setPositionX(destRect.X - m_iLeft);
		}
		break;
	}
}

void WInspectorTab::setState(int iState) {
	m_iState = iState;
}
#endif