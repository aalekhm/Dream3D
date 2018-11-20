#ifdef USE_YAGUI
#include "Engine/UI/WWindow.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/UI/WWidgetManager.h"

#define WND_RIGHT_GUTTER 3
#define WND_BOTTOM_GUTTER 3
#define WND_RESIZE_GAP 5

WWindow::WWindow() :
	m_bResizable(true),

	m_sbVertical(NULL),
	m_sbHorizontal(NULL),

	m_ButtonClose(NULL),
	m_ButtonMaximize(NULL),
	m_ButtonWResizeLeft(NULL),
	m_ButtonWResizeRight(NULL),

	m_iResizingX(0)
{
}

WWindow::~WWindow() {

}

H_WND WWindow::Create(		const char* lpClassName, 
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
	WWindow* pWWindow = new WWindow();
	((WContainer*)pWWindow)->Create(	lpClassName, 
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

	return pWWindow;
}

// custom initialization
void WWindow::onCreateEx(LPVOID lpVoid) {

	H_WND hWnd = NULL;

	RectF vDestRect;
	RectF hDestRect;
	RectF wndRect;
	RectF idealRect;

	m_WndType = (int)lpVoid;
	m_WindowCWidget = WWidgetManager::getWidget("WindowC_Scroll");

	///////////////////////////////////////////////////
	CHILD* verticalSBChild = m_WindowCWidget->getChild("VScroll");
	wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
	idealRect.X = verticalSBChild->posOffsets.x;
	idealRect.Y = verticalSBChild->posOffsets.y;
	idealRect.Width = verticalSBChild->posOffsets.w; 
	idealRect.Height = verticalSBChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	vDestRect,
											m_WindowCWidget->widgetSize.width,
											m_WindowCWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											verticalSBChild->align.eHAlign,
											verticalSBChild->align.eVAlign
											);
	if(m_sbVertical == NULL) {
		hWnd = 
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
	}
	else {
		m_sbVertical->setPosition(vDestRect.X - m_iLeft, vDestRect.Y - m_iTop);
		m_sbVertical->setSize(vDestRect.Width, vDestRect.Height);
	}
	m_iMaxVScrollbarHeight = vDestRect.Height;
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	CHILD* horizontalSBChild = m_WindowCWidget->getChild("HScroll");
	wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
	idealRect.X = horizontalSBChild->posOffsets.x;
	idealRect.Y = horizontalSBChild->posOffsets.y;
	idealRect.Width = horizontalSBChild->posOffsets.w; 
	idealRect.Height = horizontalSBChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	hDestRect,
											m_WindowCWidget->widgetSize.width,
											m_WindowCWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											horizontalSBChild->align.eHAlign,
											horizontalSBChild->align.eVAlign
											);
	if(m_sbHorizontal == NULL) {
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
	}
	else {
		m_sbHorizontal->setPosition(hDestRect.X - m_iLeft, hDestRect.Y - m_iTop);
		m_sbHorizontal->setSize(hDestRect.Width, hDestRect.Height);
	}
	m_iMaxHScrollbarWidth = hDestRect.Width;

	m_minX = hDestRect.X;
	m_maxX = hDestRect.X + hDestRect.Width;
	m_minY = vDestRect.Y;
	m_maxY = vDestRect.Y + vDestRect.Height;
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	CHILD* btnChild = m_WindowCWidget->getChild("WindowResizeLeft");
	wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
	idealRect.X = btnChild->posOffsets.x;
	idealRect.Y = btnChild->posOffsets.y;
	idealRect.Width = btnChild->posOffsets.w; 
	idealRect.Height = btnChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	hDestRect,
											m_WindowCWidget->widgetSize.width,
											m_WindowCWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											btnChild->align.eHAlign,
											btnChild->align.eVAlign
											);
	if(m_ButtonWResizeLeft == NULL) {
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
		m_ButtonWResizeLeft->setAsIntegral(true);
	}
	else {
		m_ButtonWResizeLeft->setPosition(hDestRect.X - m_iLeft, hDestRect.Y - m_iTop);
		m_ButtonWResizeLeft->setSize(hDestRect.Width, hDestRect.Height);
	}
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	btnChild = m_WindowCWidget->getChild("WindowResizeRight");
	wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
	idealRect.X = btnChild->posOffsets.x;
	idealRect.Y = btnChild->posOffsets.y;
	idealRect.Width = btnChild->posOffsets.w; 
	idealRect.Height = btnChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	hDestRect,
											m_WindowCWidget->widgetSize.width,
											m_WindowCWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											btnChild->align.eHAlign,
											btnChild->align.eVAlign
											);
	if(m_ButtonWResizeRight == NULL) {
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
		m_ButtonWResizeRight->setAsIntegral(true);
	}
	else {
		m_ButtonWResizeRight->setPosition(hDestRect.X - m_iLeft, hDestRect.Y - m_iTop);
		m_ButtonWResizeRight->setSize(hDestRect.Width, hDestRect.Height);
	}
	///////////////////////////////////////////////////
	
	///////////////////////////////////////////////////
	switch(m_WndType) {
		case ID_TYPE_WND_C:
		{
		}
		break;
		case ID_TYPE_WND_CX:
		case ID_TYPE_WND_CMX:
		{
			CHILD* btnChild = 0;
			if(m_WndType == ID_TYPE_WND_CMX) {
				///////////////////////////////////////////////////
				btnChild = m_WindowCWidget->getChild("ButtonV");
				wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
				idealRect.X = btnChild->posOffsets.x;
				idealRect.Y = btnChild->posOffsets.y;
				idealRect.Width = btnChild->posOffsets.w; 
				idealRect.Height = btnChild->posOffsets.h;
				WWidgetManager::getDestinationRect(	hDestRect,
														m_WindowCWidget->widgetSize.width,
														m_WindowCWidget->widgetSize.height,
														&wndRect,
														&idealRect,
														btnChild->align.eHAlign,
														btnChild->align.eVAlign
														);
				if(m_ButtonMaximize == NULL) {
					hWnd = 
					CreateComponent(	"WButton", 
												"", 
												0, 
												hDestRect.X - m_iLeft,
												hDestRect.Y - m_iTop, 
												hDestRect.Width, 
												hDestRect.Height,
												this, 
												HMENU(110), 
												"ButtonV");
					m_ButtonMaximize = (WButton*)hWnd;
					m_ButtonMaximize->setPostRender(true);
					m_ButtonMaximize->setMovable(false);
					m_ButtonMaximize->setAsIntegral(true);
				}
				else {
					m_ButtonMaximize->setPosition(hDestRect.X - m_iLeft, hDestRect.Y - m_iTop);
					m_ButtonMaximize->setSize(hDestRect.Width, hDestRect.Height);
				}
				///////////////////////////////////////////////////
			}

			///////////////////////////////////////////////////
			btnChild = m_WindowCWidget->getChild("ButtonX");
				wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
				idealRect.X = btnChild->posOffsets.x;
				idealRect.Y = btnChild->posOffsets.y;
				idealRect.Width = btnChild->posOffsets.w; 
				idealRect.Height = btnChild->posOffsets.h;
			WWidgetManager::getDestinationRect(	hDestRect,
													m_WindowCWidget->widgetSize.width,
													m_WindowCWidget->widgetSize.height,
													&wndRect,
													&idealRect,
													btnChild->align.eHAlign,
													btnChild->align.eVAlign
													);
			if(m_ButtonClose == NULL) {
				hWnd = 
				CreateComponent(	"WButton", 
											"", 
											0, 
											hDestRect.X - m_iLeft,
											hDestRect.Y - m_iTop, 
											hDestRect.Width, 
											hDestRect.Height,
											this, 
											HMENU(110), 
											"ButtonX");
				m_ButtonClose = (WButton*)hWnd;
				m_ButtonClose->setPostRender(true);
				m_ButtonClose->setMovable(false);
				m_ButtonClose->setAsIntegral(true);
			}
			else {
				m_ButtonClose->setPosition(hDestRect.X - m_iLeft, hDestRect.Y - m_iTop);
				m_ButtonClose->setSize(hDestRect.Width, hDestRect.Height);
			}
			///////////////////////////////////////////////////
		}
		break;
	}
	///////////////////////////////////////////////////

	bool bHasClientArea = (m_WindowCWidget->clientAreas.size() > 0);
	if(bHasClientArea) {
		CLIENT_AREA* clientArea = m_WindowCWidget->getClientAreaAt(0);
		RectF destRect;
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = clientArea->posOffsets.x;
		idealRect.Y = clientArea->posOffsets.y;
		idealRect.Width = clientArea->posOffsets.w; 
		idealRect.Height = clientArea->posOffsets.h;
		WWidgetManager::getDestinationRect(	destRect,
												m_WindowCWidget->widgetSize.width,
												m_WindowCWidget->widgetSize.height,
												&wndRect,
												&idealRect,
												clientArea->align.eHAlign,
												clientArea->align.eVAlign
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

void WWindow::onUpdate(float deltaTimeMs) {

	m_iMaxWidthPixels = m_iMaxHeightPixels = 0;
	if(m_pChildren.size() > 0) {
		for(int i = 0; i < m_pChildren.size(); i++) {
			WComponent* comp = (WComponent*)m_pChildren[i];
			if(comp->isComponentAChild() && !comp->isIntegral()) {
				int iRight = comp->getOffsetX() + comp->getWidth();
				if(iRight > m_iMaxWidthPixels)
					m_iMaxWidthPixels = iRight;

				int iBottom = comp->getOffsetY() + comp->getHeight();
				if(iBottom > m_iMaxHeightPixels)
					m_iMaxHeightPixels = iBottom;
			}
		}

		setVScrollbarLength();
		setHScrollbarLength();
	}

	bool bVertSBVisibility = (m_iMaxHeightPixels > m_ClientRect.Height + WINDOW_TITLEBAR_HEIGHT);
	m_sbVertical->setVisible(bVertSBVisibility);
	if(!bVertSBVisibility) {
		m_ClientRect.Width = m_iClientRectW + m_sbVertical->getWidth() - WND_RIGHT_GUTTER;
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth + m_sbVertical->getWidth());
	}
	else {
		m_ClientRect.Width = m_iClientRectW - WND_RIGHT_GUTTER;
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth);
	}

	bool bHoriSBVisibility = (m_iMaxWidthPixels > m_ClientRect.Width);
	m_sbHorizontal->setVisible(bHoriSBVisibility);
	if(!bHoriSBVisibility) {
		m_ClientRect.Height = m_iClientRectH + m_sbHorizontal->getHeight() - WND_BOTTOM_GUTTER;
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight + m_sbHorizontal->getHeight());
	}
	else {
		m_ClientRect.Height = m_iClientRectH - WND_BOTTOM_GUTTER;
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight);
	}
}

void WWindow::setVScrollbarLength() {
	float _part = m_ClientRect.Height;
	float _total = m_iMaxHeightPixels - WINDOW_TITLEBAR_HEIGHT;

	if(_total > 0) {
		float _percentage = (_part / _total) * 100;

		if(_percentage <= 100) {
			m_sbVertical->setLength(_percentage);
		}
	}
}

void WWindow::setHScrollbarLength() {
	float _part = m_ClientRect.Width;
	float _total = m_iMaxWidthPixels;
	float _percentage = (_part / _total) * 100;

	if(_percentage <= 100)
		m_sbHorizontal->setLength(_percentage);
}

void WWindow::updateVBarPosition() {
	float _part = abs(m_iMainY);
	float _total = m_iMaxHeightPixels;
	float _percentage = (_part / _total) * 100;

	m_sbVertical->setCursorPositionInPercent(_percentage);
}

void WWindow::updateHBarPosition() {
	float _part = abs(m_iMainX);
	float _total = m_iMaxWidthPixels;
	float _percentage = (_part / _total) * 100;

	m_sbHorizontal->setCursorPositionInPercent(_percentage);
}

void WWindow::onRender() {
	WWidgetManager* renderer =  WWidgetManager::getInstance();

	RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
		
	CHILD* cWnd = 0;
	switch(m_WndType) {
		case ID_TYPE_WND_C:
		{
			cWnd = m_WindowCWidget->getChild("WindowC");
		}
		break;
		case ID_TYPE_WND_CX:
		{
			cWnd = m_WindowCWidget->getChild("WindowCSX");
		}
		break;
		case ID_TYPE_WND_CMX:
		{
			cWnd = m_WindowCWidget->getChild("WindowCSMX");
		}
		break;
	}

	renderer->renderChild(m_WindowCWidget, cWnd, &thisWndRect);
	//renderer->renderClientArea(m_WindowCWidget, 0, &thisWndRect);
}

void WWindow::resizeWidth(int iDiffWidth) {
	if(m_iResizingX == 1) {
		if(iDiffWidth < 0 && getWidth() < m_WindowCWidget->widgetSize.width)
			return;

		setWidth(getWidth() + iDiffWidth);
	}
	else {
		if(iDiffWidth > 0 && getWidth() < m_WindowCWidget->widgetSize.width)
			return;

		if((H_WND)this == WWidgetManager::getInstance()->GetWindow(0)) {
			setLeft(iDiffWidth);
		}
		else {
			setPositionX(getOffsetX() + iDiffWidth);
		}

		setWidth(getWidth() - iDiffWidth);
	}

	onResize(getWidth(), getHeight());
}

void WWindow::resizeHeight(int iDiffHeight) {
	if(iDiffHeight < 0 && getHeight() < m_WindowCWidget->widgetSize.height)
		return;

	setHeight(getHeight() + iDiffHeight);
	onResize(getWidth(), getHeight());
}

void WWindow::onMouseDownEx(int x, int y, int iButton) {

}

void WWindow::onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy) {

}

void WWindow::onMouseUpEx(int x, int y, int iButton) {
}

void WWindow::onMouseWheelEx(WPARAM wParam, LPARAM lParam){

}

bool WWindow::isPtInside(int x, int y) {
	// x,y in local coordinates!

	// Check title bar and related gap!
	//TODO

	if (x < m_iLeft+12 || x > m_iRight-14)
		return false;
	if (y < m_iTop+12 || y > m_iBottom-14)
		return false;

	return true;
}

void WWindow::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case MOUSE_DOWN:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_RESIZE_LEFT:
				{
					m_iResizingX = -1;
					WWidgetManager::getInstance()->setCursor(IDC__SIZENESW);
				}
				break;
				case ID_RESIZE_RIGHT:
				{
					m_iResizingX = 1;
					WWidgetManager::getInstance()->setCursor(IDC__SIZENWSE);
				}
				break;
				case ID_VERTICAL_SCROLLBAR:
					switch(lParam) {
						case BTN_SCROLLBAR_UP:
						{
							m_iMainY += 25;
							if(m_iMainY > 0)
								m_iMainY = 0;

							updateVBarPosition();
						}
						break;
						case BTN_SCROLLBAR_DOWN:
						{
							m_iMainY -= 25;
							if(abs(m_iMainY) > (m_iMaxHeightPixels - m_ClientRect.Height - WINDOW_TITLEBAR_HEIGHT))
								m_iMainY = -(m_iMaxHeightPixels - m_ClientRect.Height - WINDOW_TITLEBAR_HEIGHT);

							updateVBarPosition();
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

							updateHBarPosition();
						}
						break;
						case BTN_SCROLLBAR_RIGHT:
						{
							m_iMainX -= 25;
							if(abs(m_iMainX) > (m_iMaxWidthPixels - m_ClientRect.Width))
								m_iMainX = -(m_iMaxWidthPixels - m_ClientRect.Width);

							updateHBarPosition();
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
				case ID_RESIZE_LEFT:
				case ID_RESIZE_RIGHT:
				{
					m_iResizingX = 0;
				}
				break;
			}
		}
		break;
		case MOUSE_MOVE:
		{
			int code = (wParam>>16)&0xffff;
			int buttonID = (wParam&0xffff);

			DWORD* dwDiff = (DWORD*)(lParam);

			short dwDiffX = (*dwDiff >> 16) & 0xffff;
			short dwDiffY = (*dwDiff & 0xffff);
			switch(buttonID) {
				case ID_VERTICAL_SCROLLBAR:
				break;
				case ID_HORIZONTAL_SCROLLBAR:
				break;
				case ID_RESIZE_LEFT:
					WWidgetManager::getInstance()->setCursor(IDC__SIZENESW);

					resizeWidth(dwDiffX);
					resizeHeight(dwDiffY);
				break;
				case ID_RESIZE_RIGHT:
					WWidgetManager::getInstance()->setCursor(IDC__SIZENWSE);

					resizeWidth(dwDiffX);
					resizeHeight(dwDiffY);
				break;
			}
		}
		break;
		case MOUSE_HOVER:
		{
			int code = (wParam>>16)&0xffff;
			int buttonID = (wParam&0xffff);

			DWORD* dwDiff = (DWORD*)(lParam);

			short dwDiffX = (*dwDiff >> 16) & 0xffff;
			short dwDiffY = (*dwDiff & 0xffff);
			switch(buttonID) {
				case ID_VERTICAL_SCROLLBAR:
				break;
				case ID_HORIZONTAL_SCROLLBAR:
				break;
				case ID_RESIZE_LEFT:
					WWidgetManager::getInstance()->setCursor(IDC__SIZENESW);
				break;
				case ID_RESIZE_RIGHT:
					WWidgetManager::getInstance()->setCursor(IDC__SIZENWSE);
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
					float scrollMaterialHeight = (m_iMaxHeightPixels - m_ClientRect.Height - WINDOW_TITLEBAR_HEIGHT);
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

void WWindow::onResize(int width, int height) {
	setSize(width, height);
	onCreateEx((LPVOID)m_WndType);
}

LRESULT WWindow::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
	case WM__GETTEXTLENGTH:
		{
			return strlen(getWindowName());
		}
		break;
	case WM__GETTEXT:
		{
			return sprintf_s((char*)lParam, (size_t)wParam, "%s", getWindowName());
		}
		break;
	case WM__SETTEXT:
		{
			char* str = (char*)lParam;
			setWindowName(str);
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
			setPosition((dwPos & 0xffff), ((dwPos >> 16) & 0xffff));
			return WM__OKAY;
		}
		break;
	case WM__GETSIZE:
		{
			DWORD* dwSize = (DWORD*)lParam;
			*dwSize |= (getWidth() & 0xffff);
			*dwSize |= ((getHeight() & 0xffff) << 16);

			return WM__OKAY;
		}
		break;
	case WM__SETSIZE:
		{
			DWORD dwPos = (DWORD)lParam;
			onResize((dwPos & 0xffff), ((dwPos >> 16) & 0xffff));

			return WM__OKAY;
		}
		break;
	}
}
#endif