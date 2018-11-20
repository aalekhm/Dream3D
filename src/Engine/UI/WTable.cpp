#ifdef USE_YAGUI
#include "Engine/UI/WTable.h"
#include "Engine/UI/WComponentFactory.h"

CHILD*			WTable::m_pGreyTop;

WTable::WTable() :
	m_bResizing(false),
	m_bMoving(false),
	m_bResizable(true),
	m_bLButtonDown(false),
	m_iDraggableColumn(-1)
{
	setIsContainer(true);
	m_iMainX = m_iMainY = 0;
}

WTable::~WTable() {
	for(int i = 0; i < m_vTableTitleList.size(); i++) {
		TableTitleData* td = (TableTitleData*)m_vTableTitleList[i];
		delete td;
	}
	m_vTableTitleList.clear();

	for(int i = 0; i < m_vTableRowList.size(); i++) {
		TableRowData* trd = (TableRowData*)m_vTableRowList[i];
		delete trd;
	}
	m_vTableRowList.clear();
}

H_WND WTable::Create(		const char* lpClassName, 
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
	WTable* pWTable = new WTable();

	((WContainer*)pWTable)->Create(	lpClassName, 
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

	return pWTable;
}

void WTable::onCreateEx(LPVOID lpVoid) {
	
	H_WND hWnd = NULL;

	RectF vDestRect;
	RectF hDestRect;
	RectF wndRect;
	RectF idealRect;

	m_TableWidget = WWidgetManager::getWidget("Table");
	m_sLineNoStr = new char[255];

	LINE_HEIGHT = WWidgetManager::CHARACTER_HEIGHT;
	TABLE_ROW_HEADER_HEIGHT = WWidgetManager::CHARACTER_HEIGHT + (TABLE_HEADER_OFFSET_Y << 1);
	
	m_pRenderer =  WWidgetManager::getInstance();

	///////////////////////////////////////////////////
	CHILD* verticalSBChild = m_TableWidget->getChild("VScroll");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = verticalSBChild->posOffsets.x;
		idealRect.Y = verticalSBChild->posOffsets.y;
		idealRect.Width = verticalSBChild->posOffsets.w; 
		idealRect.Height = verticalSBChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	vDestRect,
											m_TableWidget->widgetSize.width,
											m_TableWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											verticalSBChild->align.eHAlign,
											verticalSBChild->align.eVAlign
											);
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
	m_iMaxVScrollbarHeight = vDestRect.Height;
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	CHILD* horizontalSBChild = m_TableWidget->getChild("HScroll");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = horizontalSBChild->posOffsets.x;
		idealRect.Y = horizontalSBChild->posOffsets.y;
		idealRect.Width = horizontalSBChild->posOffsets.w; 
		idealRect.Height = horizontalSBChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	hDestRect,
											m_TableWidget->widgetSize.width,
											m_TableWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											horizontalSBChild->align.eHAlign,
											horizontalSBChild->align.eVAlign
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
	m_iMaxHScrollbarWidth = hDestRect.Width;
	///////////////////////////////////////////////////

	bool bHasClientArea = (m_TableWidget->clientAreas.size() > 0);
	if(bHasClientArea) {
		CLIENT_AREA* clientArea = m_TableWidget->getClientAreaAt(0);
		RectF destRect;
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = clientArea->posOffsets.x;
		idealRect.Y = clientArea->posOffsets.y;
		idealRect.Width = clientArea->posOffsets.w; 
		idealRect.Height = clientArea->posOffsets.h;
		WWidgetManager::getDestinationRect(	destRect,
												m_TableWidget->widgetSize.width,
												m_TableWidget->widgetSize.height,
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

	m_IsVScrolling = m_IsHScrolling = false;
	m_minX = getLeft() + m_ClientRect.X;
	m_maxX = m_minX + m_ClientRect.Width;
	m_minY = getTop() + m_ClientRect.Y;
	m_maxY = m_minY + m_ClientRect.Height;

	m_pGreyTop = m_TableWidget->getChild("GreyTop");
	///////////////////////////////////////////
}

void WTable::onUpdate(float deltaTimeMs) {

	if(!m_IsVScrolling)
		updateVBarPosition();
	if(!m_IsHScrolling)
		updateHBarPosition();

	/////////////////////////////////////////////////////////////////////////
	{
		char* sLen = new char[255];
		memset(sLen, 0, 255);
		sprintf(sLen, "%d", 10);
		TB_LINE_NO_SPACE = WWidgetManager::getStringWidthTillPos(sLen, strlen(sLen)) + (TABLE_LEFT_GUTTER << 1);
		delete[] sLen; 
	}
	m_iMaxWidthPixels = TB_LINE_NO_SPACE;
	for(int i = 0; i < m_vTableTitleList.size(); i++) {
		TableTitleData* td = (TableTitleData*)m_vTableTitleList[i];
		m_iMaxWidthPixels += td->getWidth();
	}

	m_iMaxHeightPixels = m_vTableRowList.size() * LINE_HEIGHT;
	/////////////////////////////////////////////////////////////////////////
	
	setVScrollbarLength();
	setHScrollbarLength();

	bool bVertSBVisibility = (m_iMaxHeightPixels > m_ClientRect.Height);
	m_sbVertical->setDisable(bVertSBVisibility);
	if(!bVertSBVisibility) {
		m_ClientRect.Width = m_iClientRectW + m_sbVertical->getWidth();
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth + m_sbVertical->getWidth());
	}
	else {
		m_ClientRect.Width = m_iClientRectW;
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth);
	}

	bool bHoriSBVisibility = (m_iMaxWidthPixels > m_ClientRect.Width);
	m_sbHorizontal->setDisable(bHoriSBVisibility);
	if(!bHoriSBVisibility) {
		m_ClientRect.Height = m_iClientRectH + m_sbHorizontal->getHeight();
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight + m_sbHorizontal->getHeight());
	}
	else {
		m_ClientRect.Height = m_iClientRectH;
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight);
	}
}

void WTable::setVScrollbarLength() {
	float _part = m_ClientRect.Height;
	float _total = m_iMaxHeightPixels;

	if(_total > 0) {
		float _percentage = (_part / _total) * 100;

		if(_percentage <= 100) {
			m_sbVertical->setLength(_percentage);
		}
	}
}

void WTable::setHScrollbarLength() {
	float _part = m_ClientRect.Width;
	float _total = m_iMaxWidthPixels;
	float _percentage = (_part / _total) * 100;

	if(_percentage <= 100)
		m_sbHorizontal->setLength(_percentage);
}

void WTable::updateVBarPosition() {
	float _part = abs(m_iMainY);
	float _total = m_iMaxHeightPixels;
	float _percentage = (_part / _total) * 100;

	m_sbVertical->setCursorPositionInPercent(_percentage);
}

void WTable::updateHBarPosition() {
	float _part = abs(m_iMainX);
	float _total = m_iMaxWidthPixels;
	float _percentage = (_part / _total) * 100;

	m_sbHorizontal->setCursorPositionInPercent(_percentage);
}

void WTable::updateMains() {
	//////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////
/*
	int yPosTop = m_minY + m_iMainY + ( TABLE_ROW_HEADER_HEIGHT + m_iSelection*TABLE_ROW_HEADER_HEIGHT);
	if(yPosTop < m_minY) {
		m_iMainY = -( m_iSelection*TABLE_ROW_HEADER_HEIGHT);
	}

	int yPosBottom = m_minY + m_iMainY + (TABLE_ROW_HEADER_HEIGHT + m_iSelection*TABLE_ROW_HEADER_HEIGHT + TABLE_ROW_HEADER_HEIGHT);
	if(yPosBottom >= m_maxY) {
		m_iMainY -= ((yPosBottom - m_maxY) + TABLE_ROW_HEADER_HEIGHT);
		m_iMainY -= (m_iMainY % TABLE_ROW_HEADER_HEIGHT);
	}

	if(m_iMainY > 0)
		m_iMainY = 0;
*/
	//////////////////////////////////////////////////////////////
}

void WTable::onRender() {
	RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
		
	CHILD* cWnd = m_TableWidget->getChild("TextArea");
	m_pRenderer->renderChild(m_TableWidget, cWnd, &thisWndRect);
	//m_pRenderer->renderClientArea(m_TableWidget, 0, &thisWndRect);

	///////////////////////////////////////////////////
	{
		drawLineNo();

		RectF reclaimRect;
		WWidgetManager::GetClipBounds(&reclaimRect);
		
		RectF clipRect(	getLeft() + TB_LINE_NO_SPACE + m_ClientRect.X,
								getTop() + m_ClientRect.Y,
								m_ClientRect.Width - TB_LINE_NO_SPACE,
								m_ClientRect.Height);
		RectF::Intersect(clipRect, reclaimRect, clipRect);
		WWidgetManager::setClip(clipRect.X, clipRect.Y, clipRect.Width, clipRect.Height);
			
			renderTableHeader();
			renderRows();
		WWidgetManager::setClip(reclaimRect.X, reclaimRect.Y, reclaimRect.Width, reclaimRect.Height);

//char* ss = new char[255];
//sprintf(ss, "%d-- %f, %d, %f, DragItm = %d", m_iMainY, m_ClientRect.Height, LINE_HEIGHT, ((m_ClientRect.Height)/LINE_HEIGHT), m_iDraggableColumn);
//WWidgetManager::setColor(0xff000000);//ABGR
//WWidgetManager::drawStringFont(	ss, 
//									getLeft() + 10, 
//									getTop() + 10, 
//									2);
//WWidgetManager::resetColor();
//delete[] ss;
	}
	///////////////////////////////////////////////////
}

void WTable::drawLineNo() {
	Rect rect(getLeft() + m_ClientRect.X, getTop() + m_ClientRect.Y, TB_LINE_NO_SPACE, m_ClientRect.Height);
	WWidgetManager::getInstance()->fillRect(128.0f, 128.0f, 128.0f, 128.0f, &rect);

	int startLineNo = abs(m_iMainY)/LINE_HEIGHT;
	float endLineNo = (m_ClientRect.Height - TABLE_ROW_HEADER_HEIGHT)/LINE_HEIGHT;
	for(int i = 0; i < endLineNo; i++, startLineNo++) {
		if(i <= m_vTableRowList.size()) {
			memset(m_sLineNoStr, 0, 255);
			sprintf(m_sLineNoStr, "%d", startLineNo);
			WWidgetManager::setColor(0xff000000);//ABGR
			WWidgetManager::drawStringFont(	m_sLineNoStr, 
												getLeft() + TB_LINE_NO_SPACE, 
												getTop() + TABLE_ROW_HEADER_HEIGHT + TABLE_TOP_GUTTER + (i*LINE_HEIGHT), 
												2);
			WWidgetManager::resetColor();
		}
		else
			break;
	}
}

void WTable::renderTableHeader() {
	RectF dummyRect(	getLeft() + TB_LINE_NO_SPACE + m_ClientRect.X + m_iMainX, 
								getTop() + m_ClientRect.Y, 
								m_ClientRect.Width, 
								TABLE_ROW_HEADER_HEIGHT);

	for(int i = 0; i < m_vTableTitleList.size(); i++) {
		
		TableTitleData* td = (TableTitleData*)m_vTableTitleList[i];
		dummyRect.Width = td->getWidth();
		m_pRenderer->renderChild(	m_TableWidget,
									m_pGreyTop,
									&dummyRect);

		m_pRenderer->drawStringFont(td->getTitle(), dummyRect.X + TABLE_HEADER_OFFSET_X, dummyRect.Y + TABLE_HEADER_OFFSET_Y, 0);
		dummyRect.X += dummyRect.Width - 1;
	}
}

void WTable::onMouseDownEx(int x, int y, int iButton) {
	m_bLButtonDown = true;
	m_IsVScrolling = m_IsHScrolling = false;
}

void WTable::onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy) {
	if((y > getTop() + TABLE_ROW_HEADER_HEIGHT) && m_iDraggableColumn < 0) {
		if (isMovable() && m_bLButtonDown) {
			if (x-prevx!=0 || y-prevy!=0)
				m_bMoving = true;

			// Drag window around:
			int diffX = (x - prevx);
			int diffY = (y - prevy);

			setPosition(getOffsetX() + diffX, getOffsetY() + diffY);
		}
	}
	else {
		if(m_bLButtonDown && m_iDraggableColumn < 0) {
			int xx = getLeft() + TB_LINE_NO_SPACE + m_ClientRect.X + m_iMainX;
			for(int i = 0; i < m_vTableTitleList.size(); i++) {
				TableTitleData* td = (TableTitleData*)m_vTableTitleList[i];
				xx += td->getWidth() - 1;
				
				if(x <= xx+5 && x >= xx-5) {
					m_iDraggableColumn = i;
					break;
				}
			}
		}

		if(m_iDraggableColumn >= 0) {
			int diffX = (x - prevx);
			TableTitleData* td = (TableTitleData*)m_vTableTitleList[m_iDraggableColumn];
			int w = td->getWidth();
			m_vTableTitleList[m_iDraggableColumn]->setWidth(w + diffX);
		}
	}
}

void WTable::onMouseUpEx(int x, int y, int iButton) {
	// stop dragging or
	// ..stop resizing
	m_bResizing = false;
	m_bMoving = false;
	m_bLButtonDown = false;

	m_iDraggableColumn = -1;
}

void WTable::onMouseWheelEx(WPARAM wParam, LPARAM lParam){
	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	if(zDelta < 0) {
		m_iMainY -= TABLE_ROW_HEADER_HEIGHT;
		if(abs(m_iMainY) > (m_iMaxHeightPixels - m_ClientRect.Height))
			m_iMainY = -(m_iMaxHeightPixels - m_ClientRect.Height);
	}
	else {
		m_iMainY += TABLE_ROW_HEADER_HEIGHT;
		if(m_iMainY > 0)
			m_iMainY = 0;
	}
}

void WTable::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case MOUSE_DOWN:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_VERTICAL_SCROLLBAR:
					switch(lParam) {
						case BTN_SCROLLBAR_UP:
						{
							m_iMainY += TABLE_ROW_HEADER_HEIGHT;
							if(m_iMainY > 0)
								m_iMainY = 0;
						}
						break;
						case BTN_SCROLLBAR_DOWN:
						{
							m_iMainY -= TABLE_ROW_HEADER_HEIGHT;
							if(abs(m_iMainY) > (m_iMaxHeightPixels - m_ClientRect.Height))
								m_iMainY = -(m_iMaxHeightPixels - m_ClientRect.Height);
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

							m_IsHScrolling = true;
						}
						break;
						case BTN_SCROLLBAR_RIGHT:
						{
							m_iMainX -= 25;
							if(abs(m_iMainX) > (m_iMaxWidthPixels - m_ClientRect.Width))
								m_iMainX = -(m_iMaxWidthPixels - m_ClientRect.Width);

							m_IsHScrolling = true;
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
			int buttonID = wParam;
			switch(wParam) {
				case ID_VERTICAL_SCROLLBAR:
				break;
				case ID_HORIZONTAL_SCROLLBAR:
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

					m_IsVScrolling = true;
				}
				break;
				case ID_HORIZONTAL_SCROLLBAR:
				{
					float scrollMaterialWidth = (m_iMaxWidthPixels - m_ClientRect.Width);
					int mainXValue = (cursorPosInPercentage*scrollMaterialWidth)/100;
					m_iMainX = -mainXValue;

					m_IsHScrolling = true;
				}
				break;
			}
		}
		break;
	}
}

void WTable::onKeyBDownEx(unsigned int iVirtualKeycode, unsigned short ch) {
	
	switch(iVirtualKeycode) {
		case VK_HOME:
		{
			m_iSelection = 0;
			updateMains();
		}
		break;
		case VK_END:
		{
			m_iSelection = m_vTableRowList.size() - 1;
			updateMains();
		}
		break;
		case VK_PRIOR:
		{
			int maxLinesPerPage = m_iClientRectH/TABLE_ROW_HEADER_HEIGHT;
			m_iSelection -= maxLinesPerPage;
			if(m_iSelection < 0) m_iSelection = 0;

			updateMains();
		}
		break;
		case VK_NEXT:
		{
			int maxLinesPerPage = m_iClientRectH/TABLE_ROW_HEADER_HEIGHT;
			m_iSelection += maxLinesPerPage;
			int maxTreeSize = m_vTableRowList.size();
			if(m_iSelection >= maxTreeSize) m_iSelection = maxTreeSize-1;

			updateMains();
		}
		break;
		case VK_UP:
		{
			m_iSelection--;
			if(m_iSelection < 0) m_iSelection = 0;
			
			updateMains();
		}
		break;
		case VK_DOWN:
		{
			m_iSelection++;
			int maxTableSize = m_vTableRowList.size();
			if(m_iSelection >= maxTableSize) m_iSelection = maxTableSize-1;
			
			updateMains();
		}
		break;
		case VK_LEFT://Collapse Node
		{
			updateMains();
		}
		break;
		case VK_RIGHT://Expand Node
		{
		}
		break;
		case VK_DELETE:
		{
		}
		break;
		case 'X':
		{
		}
		break;
		case 'C':
		{
			if(GetAsyncKeyState(VK_CONTROL)) {
			}
		}
		break;
		case 'V':
		{
			if(GetAsyncKeyState(VK_CONTROL)) {
			}
		}
		break;
	}
}
#endif