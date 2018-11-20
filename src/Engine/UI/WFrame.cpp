#include "Engine/UI/WFrame.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/UI/WWidgetManager.h"

#define FR_RIGHT_GUTTER 3
#define FR_BOTTOM_GUTTER 3
#define FR_RESIZE_GAP 5

WFrame::WFrame() :
	m_bResizable(true),

	m_sbVertical(NULL),
	m_sbHorizontal(NULL),

	m_ButtonWResizeLeft(NULL),
	m_ButtonWResizeRight(NULL),

	m_iResizingX(0)
{
}

WFrame::~WFrame() {

}

H_WND WFrame::Create(		const char* lpClassName, 
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
	WFrame* pWFrame = new WFrame();

	((WContainer*)pWFrame)->Create(	lpClassName, 
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

	return pWFrame;
}

// custom initialization
void WFrame::onCreateEx(LPVOID lpVoid) {

	H_WND hWnd = NULL;
	
	RectF vDestRect;
	RectF hDestRect;
	RectF wndRect;
	RectF idealRect;

	m_bHasScrollBars = (bool)lpVoid;
	m_FrameWidget = WWidgetManager::getWidget("Frame_Scroll");
	///////////////////////////////////////////////////
	{
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

void WFrame::onUpdate() {

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

	bool bVertSBVisibility = (m_iMaxHeightPixels > m_ClientRect.Height);
	m_sbVertical->setVisible(bVertSBVisibility);
	if(!bVertSBVisibility) {
		m_ClientRect.Width = m_iClientRectW + m_sbVertical->getWidth() - FR_RIGHT_GUTTER;
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth + m_sbVertical->getWidth());
	}
	else {
		m_ClientRect.Width = m_iClientRectW - FR_RIGHT_GUTTER;
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth);
	}

	bool bHoriSBVisibility = (m_iMaxWidthPixels > m_ClientRect.Width);
	m_sbHorizontal->setVisible(bHoriSBVisibility);
	if(!bHoriSBVisibility) {
		m_ClientRect.Height = m_iClientRectH + m_sbHorizontal->getHeight() - FR_BOTTOM_GUTTER;
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight + m_sbHorizontal->getHeight());
	}
	else {
		m_ClientRect.Height = m_iClientRectH - FR_BOTTOM_GUTTER;
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight);
	}
}
void WFrame::setVScrollbarLength() {
	float _part = m_ClientRect.Height;
	float _total = m_iMaxHeightPixels;

	if(_total > 0) {
		float _percentage = (_part / _total) * 100;

		if(_percentage <= 100) {
			m_sbVertical->setLength(_percentage);
		}
	}
}

void WFrame::setHScrollbarLength() {
	float _part = m_ClientRect.Width;
	float _total = m_iMaxWidthPixels;
	float _percentage = (_part / _total) * 100;

	if(_percentage <= 100)
		m_sbHorizontal->setLength(_percentage);
}

void WFrame::updateVBarPosition() {
	float _part = abs(m_iMainY);
	float _total = m_iMaxHeightPixels;
	float _percentage = (_part / _total) * 100;

	m_sbVertical->setCursorPositionInPercent(_percentage);
}

void WFrame::updateHBarPosition() {
	float _part = abs(m_iMainX);
	float _total = m_iMaxWidthPixels;
	float _percentage = (_part / _total) * 100;

	m_sbHorizontal->setCursorPositionInPercent(_percentage);
}


void WFrame::onRender() {
	if(getBorderVisibility()) {
		WWidgetManager* renderer =  WWidgetManager::getInstance();

		RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
			
		CHILD* cWnd = 0;
		cWnd = m_FrameWidget->getChild("FrameWindow");

		renderer->renderChild(m_FrameWidget, cWnd, &thisWndRect);
		//renderer->renderClientArea(m_FrameWidget, 0, &thisWndRect);
	}
}

void WFrame::resizeWidth(int iDiffWidth) {
	if(m_iResizingX == 1) {
		if(iDiffWidth < 0 && getWidth() < m_FrameWidget->widgetSize.width)
			return;
			
		setWidth(getWidth() + iDiffWidth);
	}
	else {
		if(iDiffWidth > 0 && getWidth() < m_FrameWidget->widgetSize.width)
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

void WFrame::resizeHeight(int iDiffHeight) {
	if(iDiffHeight < 0 && getHeight() < m_FrameWidget->widgetSize.height)
		return;
		
	setHeight(getHeight() + iDiffHeight);
	onResize(getWidth(), getHeight());
}

void WFrame::onMouseDownEx(int x, int y, int iButton) {

}

void WFrame::onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy) {
	// If click was on the title bar, drag window
	// If resizing, resize window

	// Drag window around:
	int diffX = (x - prevx);
	int diffY = (y - prevy);

	if((mCode & MK_LBUTTON) != 0) {
		if(m_pParent != NULL) {
			setPosition(getOffsetX() + diffX, getOffsetY() + diffY);
		}
		else 
		if((H_WND)this == WWidgetManager::getInstance()->GetWindow(0)) {
			setLeft(diffX);
			setTop(diffY);
		}
	}
}

void WFrame::onMouseUpEx(int x, int y, int iButton) {

}

void WFrame::onMouseWheelEx(WPARAM wParam, LPARAM lParam) {

}

bool WFrame::isPtInside(int x, int y) {
	// x,y in local coordinates!

	// Check title bar and related gap!
	//TODO

	if (x < m_iLeft || x > m_iRight)
		return false;
	if (y < m_iTop || y > m_iBottom)
		return false;

	return true;
}

void WFrame::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

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
							if(abs(m_iMainY) > (m_iMaxHeightPixels - m_ClientRect.Height))
								m_iMainY = -(m_iMaxHeightPixels - m_ClientRect.Height);

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

void WFrame::onResize(int width, int height) {
	setSize(width, height);
	onCreateEx((LPVOID)m_bHasScrollBars);
}

LRESULT WFrame::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

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