#ifdef USE_YAGUI
#include "Engine/UI/WGraph.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/UI/WWidgetManager.h"

#define WGRAPH_LEFT_GUTTER 5
#define WGRAPH_TOP_GUTTER 5
#define WGRAPH_RIGHT_GUTTER 12
#define WGRAPH_BOTTOM_GUTTER 12
#define WGRAPH_1CM 50
#define WGRAPH_1MM (WGRAPH_1CM / 10)
#define WGRAPH_DOT_WIDTH 4
#define WGRAPH_DOT_HALF_WIDTH (WGRAPH_DOT_WIDTH >> 1)
#define WGRAPH_DOT_HEIGHT 4
#define WGRAPH_DOT_HALF_HEIGHT (WGRAPH_DOT_HEIGHT >> 1)

WGraph::WGraph() :
	m_bResizable(true),

	m_ButtonWResizeLeft(NULL),
	m_ButtonWResizeRight(NULL),

	m_GraphScale(1.0f, 1.0f),

	m_iResizingX(0),
	m_iMainX(0),
	m_iMainY(0),

	m_bCanPan(false),
	m_pSelectedGraphDot(NULL),
	m_iSelectedPos(-1)
{
}

WGraph::~WGraph() {

}

H_WND WGraph::Create(		const char* lpClassName, 
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
	WGraph* pWGraph = new WGraph();

	((WContainer*)pWGraph)->Create(	lpClassName, 
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

	return pWGraph;
}

void WGraph::onCreateEx(LPVOID lpVoid) {

	H_WND hWnd = NULL;

	RectF vDestRect;
	RectF hDestRect;

	RectF wndRect;
	RectF idealRect;

	m_pGraphWidget = WWidgetManager::getWidget("GraphWidget");
	m_GraphScale = *((GraphScale*)lpVoid);
	m_GraphScale.X /= 10.0f;
	m_GraphScale.Y /= 10.0f;
	///////////////////////////////////////////////////
		CHILD* btnChild = m_pGraphWidget->getChild("WindowResizeLeft");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = btnChild->posOffsets.x;
		idealRect.Y = btnChild->posOffsets.y;
		idealRect.Width = btnChild->posOffsets.w; 
		idealRect.Height = btnChild->posOffsets.h;
		WWidgetManager::getDestinationRect(	hDestRect,
												m_pGraphWidget->widgetSize.width,
												m_pGraphWidget->widgetSize.height,
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
		btnChild = m_pGraphWidget->getChild("WindowResizeRight");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = btnChild->posOffsets.x;
		idealRect.Y = btnChild->posOffsets.y;
		idealRect.Width = btnChild->posOffsets.w; 
		idealRect.Height = btnChild->posOffsets.h;
		WWidgetManager::getDestinationRect(	hDestRect,
												m_pGraphWidget->widgetSize.width,
												m_pGraphWidget->widgetSize.height,
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

	bool bHasClientArea = (m_pGraphWidget->clientAreas.size() > 0);
	if(bHasClientArea) {
		CLIENT_AREA* clientArea = m_pGraphWidget->getClientAreaAt(0);
		RectF destRect;
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = clientArea->posOffsets.x;
		idealRect.Y = clientArea->posOffsets.y;
		idealRect.Width = clientArea->posOffsets.w;
		idealRect.Height = clientArea->posOffsets.h;
		WWidgetManager::getDestinationRect(	destRect,
												m_pGraphWidget->widgetSize.width,
												m_pGraphWidget->widgetSize.height,
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

	m_iMainX = (getWidth() >> 1);
	m_iMainY = (getHeight() >> 1);
}

void WGraph::onUpdate(float deltaTimeMs) {

}

void WGraph::onRender() {
	if(getBorderVisibility()) {
		WWidgetManager* renderer =  WWidgetManager::getInstance();

		RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
			
		CHILD* cWnd = 0;
		cWnd = m_pGraphWidget->getChild("TextArea");

		renderer->renderChild(m_pGraphWidget, cWnd, &thisWndRect);
		//renderer->renderClientArea(m_DummyWidget, 0, &thisWndRect);

		renderGraph();
	}
}

void WGraph::renderGraph()
{
	RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
	Rect outerRect(thisWndRect.X + WGRAPH_LEFT_GUTTER, thisWndRect.Y + WGRAPH_TOP_GUTTER, thisWndRect.Width - WGRAPH_RIGHT_GUTTER, thisWndRect.Height - WGRAPH_BOTTOM_GUTTER);

	WWidgetManager::SET_CLIP(outerRect.X, outerRect.Y, outerRect.Width, outerRect.Height);

	WWidgetManager* pRenderer = WWidgetManager::getInstance();
	pRenderer->fillRect(255.0f, 255.0f, 255.0f, 255.0f, &outerRect);

	float fLeftX = outerRect.X;
	float fRightX = outerRect.X + outerRect.Width;
	float fTopY = outerRect.Y;
	float fBottomY = thisWndRect.Y + outerRect.Height;

	int _1MM = WGRAPH_1CM / 10;
	int lineX, lineY = 0;
	if(m_iMainX <= 0)	lineX = fLeftX + (m_iMainX % WGRAPH_1CM);
	else						lineX = fLeftX + (m_iMainX % WGRAPH_1CM) - WGRAPH_1CM;
	if(m_iMainY <= 0)	lineY = fTopY + (m_iMainY % WGRAPH_1CM);
	else						lineY = fTopY + (m_iMainY % WGRAPH_1CM) - WGRAPH_1CM;
	bool bBreakLoop = false;
	while(true) {

		if(lineY > fTopY && lineY < fBottomY)
			pRenderer->drawHorizontalLine(0.0f, 255.0f, 0.0f, 255.0f, fLeftX, lineY, outerRect.Width, 1);

		for(int i = 1; i < 10; i++) {
			int yY = lineY + (i * _1MM);
			if(yY < fTopY) 
				continue;

			bBreakLoop = (yY > fBottomY);
			if(bBreakLoop) 
				break;

			pRenderer->drawHorizontalLine(0.0f, 255.0f, 0.0f, 100.0f, fLeftX, yY, outerRect.Width, 1);
		}

		if(bBreakLoop) 
			break;

		lineY += WGRAPH_1CM;
	}

	bBreakLoop = false;
	while(true) {

		if(lineX > fLeftX && lineX < fRightX)
			pRenderer->drawVerticalLine(0.0f, 255.0f, 0.0f, 255.0f, lineX, fTopY, outerRect.Height, 1);

		for(int i = 1; i < 10; i++) {

			int xX = lineX + (i * _1MM);
			if(xX < fLeftX) 
				continue;

			bBreakLoop = (xX > fRightX);
			if(bBreakLoop) 
				break;
			pRenderer->drawVerticalLine(0.0f, 255.0f, 0.0f, 100.0f, xX, fTopY, outerRect.Height, 1);
		}

		if(bBreakLoop) 
			break;
		lineX += WGRAPH_1CM;
	}

	//Render 'X-axis'
	if(m_iMainY > 0 && m_iMainY < outerRect.Height)
		pRenderer->drawHorizontalLine(255.0f, 0.0f, 0.0f, 255.0f, fLeftX, fTopY + m_iMainY, outerRect.Width, 1);

	//Render 'Y-axis'
	if(m_iMainX > 0 && m_iMainX < outerRect.Width)
		pRenderer->drawVerticalLine(255.0f, 0.0f, 0.0f, 255.0f, fLeftX + m_iMainX, fTopY, outerRect.Height, 1);

	//////////////////////////////////////////////
	for(GraphDots::iterator itr = m_vDots.begin(); itr != m_vDots.end(); itr++)
	{
		GraphDot* pGraphDot = *itr;
		if(pGraphDot != NULL)
		{
			float xDot = (pGraphDot->m_Pos.X / m_GraphScale.X) * WGRAPH_1MM;
			float yDot = (pGraphDot->m_Pos.Y / m_GraphScale.Y) * WGRAPH_1MM;

			float renderX = (int)(fLeftX + m_iMainX + xDot - WGRAPH_DOT_HALF_WIDTH);
			float renderY = (int)(fTopY + m_iMainY - yDot - WGRAPH_DOT_HALF_HEIGHT);

			Rect dotRect(renderX, renderY, WGRAPH_DOT_WIDTH, WGRAPH_DOT_HEIGHT);
			pRenderer->fillRect(255.0f, 0.0f, 0.0f, 255.0f, &dotRect);
			if(pGraphDot->m_bIsSelected) {
				Rect borderRect(renderX - WGRAPH_DOT_HALF_WIDTH, renderY - WGRAPH_DOT_HALF_HEIGHT, WGRAPH_DOT_WIDTH << 1, WGRAPH_DOT_HEIGHT << 1);
				pRenderer->drawRect(0.0f, 0.0f, 0.0f, 255.0f, &borderRect, 1);

				Rect posRect;
				char sPosStr[255];
				memset(sPosStr, 0, 255);
				sprintf_s(sPosStr, "( %f, %f )", pGraphDot->m_Pos.X, pGraphDot->m_Pos.Y);
				posRect.X = renderX + 10;
				posRect.Y = renderY - WWidgetManager::CHARACTER_HEIGHT;
				posRect.Width = pRenderer->getStringWidthTillPos(sPosStr, strlen(sPosStr)) + 10;
				posRect.Height = WWidgetManager::CHARACTER_HEIGHT;
				pRenderer->fillRect(255.0f, 255.0f, 255.0f, 255.0f, &posRect);
				WWidgetManager::setColor(0xffff0000);//ABGR
				WWidgetManager::getInstance()->drawStringFont(sPosStr, posRect.X + (posRect.Width >> 1), posRect.Y, 1);
				pRenderer->drawRect(0.0f, 0.0f, 0.0f, 255.0f, &posRect, 1);
			}
		}
	}
	//////////////////////////////////////////////

	renderLegend();

	pRenderer->drawRect(0.0f, 0.0f, 0.0f, 255.0f, &outerRect);

	WWidgetManager::RESET_CLIP();
}

void WGraph::renderLegend()
{
	RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
	Rect outerRect(thisWndRect.X + WGRAPH_LEFT_GUTTER, thisWndRect.Y + WGRAPH_TOP_GUTTER, thisWndRect.Width - WGRAPH_RIGHT_GUTTER, thisWndRect.Height - WGRAPH_BOTTOM_GUTTER);

	byte iLineHeight = 15;
	char dbStr[255];
	memset(dbStr, 0, 255);
	WWidgetManager::setColor(0xff000000);//ABGR

	sprintf(dbStr, "On 'X-axis', 1 cm = %f", m_GraphScale.X);
	WWidgetManager::getInstance()->drawStringFont(dbStr, outerRect.X + outerRect.Width - 175, outerRect.Y + 10, 0);

	sprintf(dbStr, "On 'Y-axis', 1 cm = %f", m_GraphScale.Y);
	WWidgetManager::getInstance()->drawStringFont(dbStr, outerRect.X + outerRect.Width - 175, outerRect.Y + 10 + (iLineHeight << 0), 0);

	sprintf(dbStr, "X = %f, Y = %f", m_fPosX, m_fPosY);
	WWidgetManager::getInstance()->drawStringFont(dbStr, outerRect.X + outerRect.Width - 175, outerRect.Y + 10 + (iLineHeight << 1), 0);
}

void WGraph::resizeWidth(int iDiffWidth) {
	if(m_iResizingX == 1) {
		if(iDiffWidth < 0 && getWidth() < m_pGraphWidget->widgetSize.width)
			return;

		setWidth(getWidth() + iDiffWidth);
	}
	else {
		if(iDiffWidth > 0 && getWidth() < m_pGraphWidget->widgetSize.width)
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

void WGraph::resizeHeight(int iDiffHeight) {
	if(iDiffHeight < 0 && getHeight() < m_pGraphWidget->widgetSize.height)
		return;

	setHeight(getHeight() + iDiffHeight);
	onResize(getWidth(), getHeight());
}

void WGraph::onMouseDownEx(int x, int y, int iButton) {
	
}

void WGraph::onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy) {

	// Drag graph around:
	int diffX = (x - prevx);
	int diffY = (y - prevy);

	if(m_bCanPan && (mCode & MK_LBUTTON) != 0) {
		m_iMainX += diffX;
		m_iMainY += diffY;
	}
}

void WGraph::onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY) 
{
	getPositionInGraph(x, y, m_fPosX, m_fPosY);
}

void WGraph::onMouseUpEx(int x, int y, int iButton) {

	float fDotX = 0.0f, fDotY = 0.0f;

	if((iButton & MK_LBUTTON) != 0)
	{
		getPositionInGraph(x, y, fDotX, fDotY);
		unsigned short _iCount = 0;
		for(GraphDots::iterator itr = m_vDots.begin(); itr != m_vDots.end(); itr++, _iCount++)
		{
			GraphDot* pGraphDot = *itr;
			if(		(	pGraphDot->m_Pos.X > fDotX - m_GraphScale.X
						&&
						pGraphDot->m_Pos.X < fDotX + m_GraphScale.X
					)
					&& 
					(	pGraphDot->m_Pos.Y > fDotY - m_GraphScale.Y
						&&
						pGraphDot->m_Pos.Y < fDotY + m_GraphScale.Y
					)
			) {	
				if(m_pSelectedGraphDot != NULL)
					m_pSelectedGraphDot->m_bIsSelected = false;
				m_pSelectedGraphDot = pGraphDot;
				m_pSelectedGraphDot->m_bIsSelected = true;
				m_iSelectedPos = _iCount;
			}
		}
	}
	else
	if((iButton & MK_RBUTTON) != 0)
	{
		float fDotX = 0.0f, fDotY = 0.0f;
		getPositionInGraph(x, y, fDotX, fDotY);

		addDot(fDotX, fDotY);
	}
}

void WGraph::onMouseWheelEx(WPARAM wParam, LPARAM lParam) {

}

void WGraph::getPositionInGraph(int iInputMouseX, int iInputMouseY, float& fOutX, float& fOutY)
{
	float fPosX = ((iInputMouseX - (getLeft() + WGRAPH_LEFT_GUTTER)) - m_iMainX);
	float fPosY = (m_iMainY - (iInputMouseY - (getTop() + WGRAPH_TOP_GUTTER)));

	int _1UnitOnX = (int)(fPosX / WGRAPH_1MM);
	int _1UnitOnY = (int)(fPosY / WGRAPH_1MM);

	fOutX = (_1UnitOnX * m_GraphScale.X);
	fOutY = (_1UnitOnY * m_GraphScale.Y);
}

bool WGraph::isPtInside(int x, int y) {
	// x,y in local coordinates!

	// Check title bar and related gap!
	//TODO

	if (x < m_iLeft || x > m_iRight)
		return false;
	if (y < m_iTop || y > m_iBottom)
		return false;

	return true;
}

void WGraph::onKeyBDownEx(unsigned int iVirtualKeycode, unsigned short ch) {
	if(iVirtualKeycode == VK_SPACE)
		m_bCanPan = true;
	else
	if(iVirtualKeycode == VK_DELETE)
	{
		if(m_pSelectedGraphDot != NULL)
		{
			deleteGraphDotFromList(m_pSelectedGraphDot);
		}
	}
	else
	if(iVirtualKeycode == VK_LEFT || iVirtualKeycode == VK_RIGHT)
	{
		if(GetAsyncKeyState(VK_SHIFT))
		{
			unsigned short _iSize = m_vDots.size();
			if(_iSize > 0 && m_pSelectedGraphDot != NULL)
			{
				if(iVirtualKeycode == VK_LEFT)
				{
					--m_iSelectedPos;
					if(m_iSelectedPos < 0)
						m_iSelectedPos = _iSize - 1;
				}
				else 
				{
					++m_iSelectedPos;
					if(m_iSelectedPos >= _iSize)
						m_iSelectedPos = 0;
				}

				GraphDot* pPrevGraphDot = m_pSelectedGraphDot;
				pPrevGraphDot->m_bIsSelected = false;
				m_pSelectedGraphDot = m_vDots.at(m_iSelectedPos);
				m_pSelectedGraphDot->m_bIsSelected = true;
			}
		}
		else 
		{
			if(m_pSelectedGraphDot != NULL)
			{
				signed char iMovement = (iVirtualKeycode == VK_LEFT) ? -1 : 1;
				m_pSelectedGraphDot->m_Pos.X += iMovement * m_GraphScale.X;
			}
		}
	}
	if(iVirtualKeycode == VK_UP || iVirtualKeycode == VK_DOWN)
	{
		if(m_pSelectedGraphDot != NULL)
		{
			signed char iMovement = (iVirtualKeycode == VK_UP) ? 1 : -1;
			m_pSelectedGraphDot->m_Pos.Y += iMovement * m_GraphScale.Y;
		}
	}
}

void WGraph::onKeyBUpEx(unsigned int iVirtualKeycode, unsigned short ch) {
	if(iVirtualKeycode == VK_SPACE)
		m_bCanPan = false;
}

void WGraph::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

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
			}
		}
		break;
		case MOUSE_UP:
		{
			int buttonID = wParam;
			switch(wParam) {
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
				case ID_RESIZE_LEFT:
					WWidgetManager::getInstance()->setCursor(IDC__SIZENESW);
				break;
				case ID_RESIZE_RIGHT:
					WWidgetManager::getInstance()->setCursor(IDC__SIZENWSE);
				break;
			}
		}
		break;
	}
}

void WGraph::onResize(int width, int height) {
	setSize(width, height);

	GraphScale stGraphScale(m_GraphScale.X * 10, m_GraphScale.Y * 10);
	onCreateEx((void*)&stGraphScale);
}

void WGraph::clear()
{
	for(GraphDots::iterator itr = m_vDots.begin(); itr != m_vDots.end(); itr++)
	{
		GraphDot* pGraphDot = *itr;
		if(pGraphDot != NULL) {
			SAFE_DELETE(pGraphDot);
		}
	}

	m_vDots.clear();
}

void WGraph::deleteGraphDotFromList(const GraphDot* pGraphDotPtr)
{
	if(pGraphDotPtr != NULL)
	{
		if(m_vDots.size() > 0) {
			GraphDots::iterator itr = std::find(m_vDots.begin(), m_vDots.end(), pGraphDotPtr);
			if(itr != m_vDots.end()) {
				GraphDot* pGraphDot = *itr;
				
				m_vDots.erase(itr);
				SAFE_DELETE(pGraphDot);

				m_pSelectedGraphDot = NULL;
				m_iSelectedPos = -1;
			}
		}
	}
}

LRESULT WGraph::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

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