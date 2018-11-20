#include "Engine/UI/WInspector.h"
#include "Engine/UI/WWidgetManager.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/UI/WTextField.h"
#include "Engine/UI/WComboBox.h"

WInspector::WInspector() :
	m_bResizing(false),
	m_bMoving(false),
	m_bResizable(true),
	m_bLButtonDown(false)
{
	
}

WInspector::~WInspector() {

}

H_WND WInspector::Create(		const char* lpClassName, 
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
	WInspector* pWInspector = new WInspector();

	((WContainer*)pWInspector)->Create(	lpClassName, 
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

	return pWInspector;
}

void WInspector::onCreateEx(LPVOID lpVoid) {
	
	H_WND hWnd = NULL;

	RectF vDestRect;
	RectF hDestRect;
	RectF wndRect;
	RectF idealRect;

	m_bHasScrollBars = (bool)lpVoid;
	m_FrameWidget = WWidgetManager::getWidget("InspectorC_Scroll");

	///////////////////////////////////////////////////
	if(m_bHasScrollBars) {
		CHILD* verticalSBChild = m_FrameWidget->getChild("VScroll");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = verticalSBChild->posOffsets.x;
		idealRect.Y = verticalSBChild->posOffsets.y;
		idealRect.Width = verticalSBChild->posOffsets.w; 
		idealRect.Height = verticalSBChild->posOffsets.h;
		WWidgetManager::getDestinationRect(	vDestRect,
												m_FrameWidget->widgetSize.width,
												m_FrameWidget->widgetSize.height,
												&wndRect,
												&idealRect,
												verticalSBChild->align.iHAlign,
												verticalSBChild->align.iVAlign
												);
		H_WND hWnd = 
		CreateComponent(	"WScrollbar", 
									"", 
									0, 
									vDestRect.X - m_iLeft, 
									vDestRect.Y - m_iTop,
									vDestRect.Width, 
									vDestRect.Height,
									this, 
									HMENU(ID_VERTICAL_SCROLLBAR), 
								(LPVOID)1);
		m_sbVertical = (WScrollbar*)hWnd;
		m_sbVertical->hasBG(true);
		m_sbVertical->setPostRender(true);
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		CHILD* horizontalSBChild = m_FrameWidget->getChild("HScroll");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = horizontalSBChild->posOffsets.x;
		idealRect.Y = horizontalSBChild->posOffsets.y;
		idealRect.Width = horizontalSBChild->posOffsets.w; 
		idealRect.Height = horizontalSBChild->posOffsets.h;
		WWidgetManager::getDestinationRect(	hDestRect,
												m_FrameWidget->widgetSize.width,
												m_FrameWidget->widgetSize.height,
												&wndRect,
												&idealRect,
												horizontalSBChild->align.iHAlign,
												horizontalSBChild->align.iVAlign
												);
		hWnd = 
		CreateComponent(	"WScrollbar", 
									"", 
									0, 
									hDestRect.X - m_iLeft, 
									hDestRect.Y - m_iTop,
									hDestRect.Width, 
									hDestRect.Height,
									this, 
									HMENU(ID_HORIZONTAL_SCROLLBAR), 
									(LPVOID)0);
		m_sbHorizontal = (WScrollbar*)hWnd;
		m_sbHorizontal->hasBG(true);
		m_sbHorizontal->setPostRender(true);
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		CHILD* btnChild = m_FrameWidget->getChild("WindowResizeLeft");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = btnChild->posOffsets.x;
		idealRect.Y = btnChild->posOffsets.y;
		idealRect.Width = btnChild->posOffsets.w; 
		idealRect.Height = btnChild->posOffsets.h;
		WWidgetManager::getDestinationRect(	hDestRect,
												m_FrameWidget->widgetSize.width,
												m_FrameWidget->widgetSize.height,
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
									HMENU(ID_RESIZE_LEFT), 
									"WindowResizeLeft");
		m_ButtonWResizeLeft = (WButton*)hWnd;
		m_ButtonWResizeLeft->setPostRender(true);
		m_ButtonWResizeLeft->setMovable(false);
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		btnChild = m_FrameWidget->getChild("WindowResizeRight");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = btnChild->posOffsets.x;
		idealRect.Y = btnChild->posOffsets.y;
		idealRect.Width = btnChild->posOffsets.w; 
		idealRect.Height = btnChild->posOffsets.h;
		WWidgetManager::getDestinationRect(	hDestRect,
												m_FrameWidget->widgetSize.width,
												m_FrameWidget->widgetSize.height,
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
									HMENU(ID_RESIZE_RIGHT), 
									"WindowResizeRight");
		m_ButtonWResizeRight = (WButton*)hWnd;
		m_ButtonWResizeRight->setPostRender(true);
		m_ButtonWResizeRight->setMovable(false);
		///////////////////////////////////////////////////
	}

	bool bHasClientArea = (m_FrameWidget->clientAreas.size() > 0);
	if(bHasClientArea) {
		CLIENT_AREA* clientArea = m_FrameWidget->getClientAreaAt(0);
		RectF destRect;
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = clientArea->posOffsets.x;
		idealRect.Y = clientArea->posOffsets.y;
		idealRect.Width = clientArea->posOffsets.w; 
		idealRect.Height = clientArea->posOffsets.h;
		WWidgetManager::getDestinationRect(	destRect,
												m_FrameWidget->widgetSize.width,
												m_FrameWidget->widgetSize.height,
												&wndRect,
												&idealRect,
												clientArea->align.iHAlign,
												clientArea->align.iVAlign
												);
		m_ClientRect.X = destRect.X - getLeft();
		m_ClientRect.Y = destRect.Y - getTop();
		m_ClientRect.Width = destRect.Width;
		m_ClientRect.Height = destRect.Height;

		m_iClientRectW = m_ClientRect.Width;
		m_iClientRectH = m_ClientRect.Height;
	}
	///////////////////////////////////////////
}

void WInspector::onUpdate() {
	if(!m_bHasScrollBars)
		return;

	//////////////////////////////////
	WInspectorTab* inspChild = 0;
	int yy = TOP_MARGIN_WIDTH;
	int prevInspectorTabHeight = 0;
	for(int i = 0; i < m_vInspectorTabs.size(); i++) {
		inspChild = (WInspectorTab*)m_vInspectorTabs[i];

		inspChild->setPositionY(yy);

		prevInspectorTabHeight = inspChild->getHeight();
		yy += prevInspectorTabHeight;
	}
	//////////////////////////////////

	m_iMaxWidthPixels = m_iMaxHeightPixels = 0;
	if(m_pChildren.size() > 0) {
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
	}

	setVScrollbarLength();
	setHScrollbarLength();

	bool bVertSBVisibility = (m_iMaxHeightPixels > m_ClientRect.Height);
	m_sbVertical->setDisable(bVertSBVisibility);
	if(!bVertSBVisibility) {
		m_ClientRect.Width = m_iClientRectW + m_sbVertical->getWidth();
		
		WInspectorTab* inspChild = 0;
		for(int i = 0; i < m_vInspectorTabs.size(); i++) {
			inspChild = (WInspectorTab*)m_vInspectorTabs[i];

			inspChild->setWidth(m_iMaxInspectorTabWidth + m_sbVertical->getWidth());
		}
	}
	else {
		m_ClientRect.Width = m_iClientRectW;

		WInspectorTab* inspChild = 0;
		for(int i = 0; i < m_vInspectorTabs.size(); i++) {
			inspChild = (WInspectorTab*)m_vInspectorTabs[i];

			inspChild->setWidth(m_iMaxInspectorTabWidth);
		}
	}

	bool bHoriSBVisibility = (m_iMaxWidthPixels > m_ClientRect.Width);
	m_sbHorizontal->setDisable(bHoriSBVisibility);
}

void WInspector::setVScrollbarLength() {
	float _part = m_ClientRect.Height;
	float _total = m_iMaxHeightPixels;

	if(_total > 0) {
		float _percentage = (_part / _total) * 100;

		if(_percentage <= 100) {
			m_sbVertical->setLength(_percentage);
		}
	}
}

void WInspector::setHScrollbarLength() {
	float _part = m_ClientRect.Width;
	float _total = m_iMaxWidthPixels;
	float _percentage = (_part / _total) * 100;

	if(_percentage <= 100)
		m_sbHorizontal->setLength(_percentage);
}

void WInspector::updateVBarCursorPosition() {
	float _part = abs(m_iMainY);
	float _total = m_iMaxHeightPixels;
	float _percentage = (_part / _total) * 100;

	m_sbVertical->setCursorPositionInPercent(_percentage);
}

void WInspector::updateHBarCursorPosition() {
	float _part = abs(m_iMainX);
	float _total = m_iMaxWidthPixels;
	float _percentage = (_part / _total) * 100;

	m_sbHorizontal->setCursorPositionInPercent(_percentage);
}

void WInspector::onRender() {
	if(!m_bHasScrollBars)
		return;

	WWidgetManager* renderer =  WWidgetManager::getInstance();

	RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
		
	CHILD* cWnd = 0;
	cWnd = m_FrameWidget->getChild("FrameWindow");

	renderer->renderChild(m_FrameWidget, cWnd, &thisWndRect);
	//renderer->renderClientArea(m_FrameWidget, 0, &thisWndRect);
}

void WInspector::onMouseDownEx(int x, int y, int iButton) {

	// Check if click was on the title bar
	// Check if click was on the border and the window is resizable
	//TODO

	// Click was on the empty space, and window is movable.
	// Start moving.
	m_bLButtonDown = true;
}

void WInspector::onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy) {

	// If click was on the title bar, drag window
	// If resizing, resize window
	
	if (isMovable() && m_bLButtonDown) {

		if (x-prevx!=0 || y-prevy!=0)
			m_bMoving = true;

		// Drag window around:
		int diffX = (x - prevx);
		int diffY = (y - prevy);

		setPosition(getOffsetX() + diffX, getOffsetY() + diffY);
	}
}

void WInspector::onMouseUpEx(int x, int y, int iButton) {
	// stop dragging or
	// ..stop resizing
	m_bResizing = false;
	m_bMoving = false;
	m_bLButtonDown = false;
}

void WInspector::onMouseWheelEx(WPARAM wParam, LPARAM lParam){

}

bool WInspector::isPtInside(int x, int y) {
	// x,y in local coordinates!

	// Check title bar and related gap!
	//TODO

	if (x < m_iLeft || x > m_iRight)
		return false;
	if (y < m_iTop || y > m_iBottom)
		return false;

	return true;
}

void WInspector::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case MOUSE_DOWN:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_VERTICAL_SCROLLBAR:
					switch(lParam) {
						case BTN_SCROLLBAR_UP:
						{
							m_iMainY += 25;
							if(m_iMainY > 0)
								m_iMainY = 0;

							updateVBarCursorPosition();
						}
						break;
						case BTN_SCROLLBAR_DOWN:
						{
							m_iMainY -= 25;
							if(abs(m_iMainY) > (m_iMaxHeightPixels - m_ClientRect.Height))
								m_iMainY = -(m_iMaxHeightPixels - m_ClientRect.Height);

							updateVBarCursorPosition();
						}
						break;
					}
					break;
				case ID_HORIZONTAL_SCROLLBAR:
					switch(lParam) {
						case BTN_SCROLLBAR_LEFT:
						{
							m_iMainX += 25;
							if(m_iMainX > 0)
								m_iMainX = 0;

							updateHBarCursorPosition();
						}
						break;
						case BTN_SCROLLBAR_RIGHT:
						{
							m_iMainX -= 25;
							if(abs(m_iMainX) > (m_iMaxWidthPixels - m_ClientRect.Width))
								m_iMainX = -(m_iMaxWidthPixels - m_ClientRect.Width);

							updateHBarCursorPosition();
						}
						break;
				}
				break;
			}
		}
		break;
		case MOUSE_UP:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_VERTICAL_SCROLLBAR:
				break;
				case ID_HORIZONTAL_SCROLLBAR:
				break;
			}
		}
		break;
		case MOUSE_MOVE:
		{
			int buttonID = (wParam & 0xffff);
			int code = (wParam >> 16)& 0xffff;
			int diffX = (lParam >> 16)& 0xffff;
			int diffY = (lParam & 0xffff);
			switch(buttonID) {
				case ID_VERTICAL_SCROLLBAR:
				break;
				case ID_HORIZONTAL_SCROLLBAR:
				break;
				case ID_RESIZE_LEFT:
				break;
			}
		}
		break;
		case SCROLLER_POS_ON_DRAG:
		{
			int cursorPosInPercentage = (int)lParam;
			switch(wParam) {
				case ID_VERTICAL_SCROLLBAR:
				{
					float scrollMaterialHeight = (m_iMaxHeightPixels - m_ClientRect.Height);
					int mainYValue = (cursorPosInPercentage*scrollMaterialHeight)/100;
					m_iMainY = -mainYValue;
				}
				break;
				case ID_HORIZONTAL_SCROLLBAR:
				{
					float scrollMaterialWidth = (m_iMaxWidthPixels - m_ClientRect.Width);
					int mainXValue = (cursorPosInPercentage*scrollMaterialWidth)/100;
					m_iMainX = -mainXValue;
				}
				break;
			}
		}
		break;
	}
}

void WInspector::addTab() {
	H_WND hWnd = NULL;

	hWnd = 
	CreateComponent(	"WInspectorTab", 
								"Title1", 
								0, 
								LEFT_MARGIN_WIDTH,
								TOP_MARGIN_WIDTH, 
								getWidth() - (RIGHT_MARGIN_WIDTH + INSPECTOR_SCROLL_WIDTH), 
								250,
								this, 
								HMENU(1112), 
								(LPVOID)NULL);
	WInspectorTab* wInspTab = (WInspectorTab*)hWnd;
	m_vInspectorTabs.push_back(wInspTab);
	m_iMaxInspectorTabWidth = wInspTab->getWidth();

	{
		/////////////////////////////////////////
		hWnd = 
		CreateComponent(	"WTextField", 
									"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
									0, 
									20,
									40, 
									150, 
									23,
									wInspTab, 
									HMENU(99), 
									NULL);
		((WTextField*)hWnd)->setComponentAsChild(true);

		hWnd = 
		CreateComponent(	"WComboBox", 
									"", 
									0, 
									20,
									70, 
									250, 
									100,
									wInspTab, 
									HMENU(100), 
									NULL);

		hWnd = 
		CreateComponent(	"WButton", 
									"Simple Button", 
									0, 
									20,
									100, 
									125, 
									25,
									wInspTab, 
									HMENU(111), 
									"Button");
		((WButton*)hWnd)->setComponentAsChild(true);

		hWnd = 
		CreateComponent(	"WTextField", 
									"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
									0, 
									20,
									130, 
									150, 
									23,
									wInspTab, 
									HMENU(99), 
									NULL);
		((WTextField*)hWnd)->setComponentAsChild(true);

		hWnd = 
		CreateComponent(	"WComboBox", 
									"", 
									0, 
									20,
									160, 
									250, 
									100,
									wInspTab, 
									HMENU(100), 
									NULL);

		hWnd = 
		CreateComponent(	"WButton", 
									"Simple Button", 
									0, 
									20,
									190, 
									125, 
									25,
									wInspTab, 
									HMENU(111), 
									"Button");
		((WButton*)hWnd)->setComponentAsChild(true);
		/////////////////////////////////////////
	}

	m_iInspTabCount = m_vInspectorTabs.size();
}

