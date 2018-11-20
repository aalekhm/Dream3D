#include "Engine/UI/WTextBox.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/UI/WWidgetManager.h"
#include <algorithm>

using namespace std;

#define TB_LEFT_GUTTER		5
#define TB_TOP_GUTTER		5
#define TB_RIGHT_GUTTER		10

unsigned int WTextBox::LINE_HEIGHT;

WTextBox::WTextBox() {
	setIsContainer(true);
	m_iMainX = m_iMainY = 0;

	LINE_HEIGHT = WWidgetManager::CHARACTER_HEIGHT;
}

struct maxLineLength {
	bool operator() ( const std::string& str1, const std::string& str2 )
	{
		int s1 = str1.size();
		int s2 = str2.size();
		//printf("%d 0r %d\n", s1, s2);

		return (s1 < s2);
	}
};

H_WND WTextBox::Create(		const char* lpClassName, 
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
	WTextBox* pWTextBox = new WTextBox();
	return pWTextBox->createWindow(lpClassName, lpWindowName, dwStyle, x, y, width, height, hwndParent, hMenu, lpVoid);
}

H_WND WTextBox::createWindow(	const char* lpClassName, 
												const char* lpWindowName, 
												DWORD dwStyle, 
												int x, 
												int y, 
												int width, 
												int height, 
												H_WND hwndParent, 
												HMENU hMenu, 
												LPVOID lpParam
) {
	sprintf(m_pClassName, "%s", lpClassName);

	H_WND hWnd = NULL;

	m_pParent = (WContainer*)hwndParent;

	m_iOffsetX = x;
	m_iOffsetY = y;

	m_iLeft = m_pParent->getLeft() + m_iOffsetX;
	m_iTop = m_pParent->getTop() + m_iOffsetY;
	m_iRight = m_iLeft + width;
	m_iBottom = m_iTop + height;

	mState = NORMAL;
	m_CaretPosX = m_CaretPosY = 0;
	m_mainX = 0;
	m_mainY = 0;

	setText(lpWindowName);
	showLineNumbers(false);

	mState = NORMAL;

	m_IsVScrolling = m_IsHScrolling = false;
	SEL_CURSOR_LINE_NO = CURSOR_LINE_NO = 0;
	SEL_COLUMN_NO = COLUMN_NO = 0;
	m_bIsSelecting = false;

	dbStr = new char[255];
	memset(dbStr, 0, 255);

	///////////////////////////////////////////////////

	m_TextBoxWidget = WWidgetManager::getWidget("TextBox");

	CHILD* verticalSBChild = m_TextBoxWidget->getChild("VScroll");
	RectF destRect;
	RectF wndRect(m_iLeft, m_iTop, getWidth(), getHeight());
	RectF idealRect;

	idealRect.X = verticalSBChild->posOffsets.x;
	idealRect.Y = verticalSBChild->posOffsets.y;
	idealRect.Width = verticalSBChild->posOffsets.w; 
	idealRect.Height = verticalSBChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	destRect,
		m_TextBoxWidget->widgetSize.width,
		m_TextBoxWidget->widgetSize.height,
		&wndRect,
		&idealRect,
		verticalSBChild->align.iHAlign,
		verticalSBChild->align.iVAlign
		);
	hWnd = 
	CreateComponent(	"WScrollbar", 
								"", 
								0, 
								destRect.X - m_iLeft, 
								destRect.Y - m_iTop,
								destRect.Width, 
								destRect.Height,
								this, 
								HMENU(ID_VERTICAL_SCROLLBAR), 
								(LPVOID)1);
	m_sbVertical = (WScrollbar*)hWnd;
	m_sbVertical->hasBG(true);
	m_sbVertical->setPostRender(true);
	m_iMaxVScrollbarHeight = destRect.Height;

	///////////////////////////////////////////////////

	CHILD* horizontalSBChild = m_TextBoxWidget->getChild("HScroll");
	idealRect.X = horizontalSBChild->posOffsets.x;
	idealRect.Y = horizontalSBChild->posOffsets.y;
	idealRect.Width = horizontalSBChild->posOffsets.w; 
	idealRect.Height = horizontalSBChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	destRect,
		m_TextBoxWidget->widgetSize.width,
		m_TextBoxWidget->widgetSize.height,
		&wndRect,
		&idealRect,
		horizontalSBChild->align.iHAlign,
		horizontalSBChild->align.iVAlign
		);
	hWnd = 
	CreateComponent(	"WScrollbar", 
								"", 
								0, 
								destRect.X - m_iLeft, 
								destRect.Y - m_iTop,
								destRect.Width, 
								destRect.Height,
								this, 
								HMENU(ID_HORIZONTAL_SCROLLBAR), 
								(LPVOID)0);
	m_sbHorizontal = (WScrollbar*)hWnd;
	m_sbHorizontal->hasBG(true);
	m_sbHorizontal->setPostRender(true);
	m_iMaxHScrollbarWidth = destRect.Width;

	bool bHasClientArea = (m_TextBoxWidget->clientAreas.size() > 0);
	if(bHasClientArea) {
		CLIENT_AREA* clientArea = m_TextBoxWidget->getClientAreaAt(0);
		RectF destClientRect;
		idealRect.X = clientArea->posOffsets.x;
		idealRect.Y = clientArea->posOffsets.y;
		idealRect.Width = clientArea->posOffsets.w; 
		idealRect.Height = clientArea->posOffsets.h;
		WWidgetManager::getDestinationRect(	destClientRect,
			m_TextBoxWidget->widgetSize.width,
			m_TextBoxWidget->widgetSize.height,
			&wndRect,
			&idealRect,
			clientArea->align.iHAlign,
			clientArea->align.iVAlign
			);
		m_ClientRect.X = destClientRect.X - getLeft();
		m_ClientRect.Y = destClientRect.Y - getTop();
		m_ClientRect.Width = destClientRect.Width;
		m_ClientRect.Height = destClientRect.Height;

		m_iClientRectW = m_ClientRect.Width;
		m_iClientRectH = m_ClientRect.Height;
	}

	showLineNumbers(true);

	///////////////////////////////////////////////////
	updateScrollBarVisibility();
	updateMains();
	///////////////////////////////////////////////////

	setComponentID((int)hMenu);
	setComponentAsChild(true);
	((WContainer*)m_pParent)->addComponent(this);

	return this;
}

void WTextBox::calculateMaxLineWidth() {
	std::vector<std::string>::iterator pos = std::max_element(m_Lines.begin(), m_Lines.end(), maxLineLength());
	m_MaxLineWidth = m_Lines[pos - m_Lines.begin()].size();
}

void WTextBox::setText(const char* str) {
	
	m_Lines.clear();
	appendText(str);
}

void WTextBox::appendText(const char* str) {

	int startPos = 0, endPos = 0;
	std::string strBuff = str;

	while(true) {
		endPos = strBuff.find_first_of("\r\n", startPos);
		if(endPos < 0)
			break;
		m_Lines.push_back(strBuff.substr(startPos, endPos-startPos));

		if(strBuff[endPos] == '\r')
			endPos++;

		startPos = endPos+1;
	}
	if(startPos < strBuff.size())
		m_Lines.push_back(strBuff.substr(startPos, strBuff.size() - startPos));

	strBuff.~basic_string();
}

void WTextBox::setVScrollbarLength() {

	float _part = LINES_PER_PAGE*LINE_HEIGHT;
	float _total = m_Lines.size()*LINE_HEIGHT;
	float _percentage = (_part / _total) * 100;

	if(_percentage <= 100)
		m_sbVertical->setLength(_percentage);
}

void WTextBox::setHScrollbarLength() {

	float _part = CHARS_PER_PAGE*WWidgetManager::CHARACTER_WIDTH;
	float _total = m_MaxLineWidth*WWidgetManager::CHARACTER_WIDTH;
	float _percentage = (_part / _total) * 100;

	if(_percentage <= 100)
		m_sbHorizontal->setLength(_percentage);
}

void WTextBox::drawStringFont(int x, int y, int anchor) {
//printf("In drawStringFont\n");

	Rect rect;

	int lineNo = abs(m_mainY)/LINE_HEIGHT;
	if(lineNo > m_Lines.size()-1)
		return;

	int X = 0, Y = 0;
	
	int xX = x;
	int yY = y - m_mainY;

	int i = 0;
	while(true) {
		bool bDrawSelect = false;
		if(yY >= m_maxY)
			break;
		
		int CHAR_WIDTH = 0;
		char c = ' ';

		bool bSkipRestLine = false;

		if(m_Lines[lineNo].size() > 0) {
			c = m_Lines[lineNo].at(i);
			CHAR_WIDTH = WWidgetManager::getCharWidth(c);
			
			int iRightTop = (xX + CHAR_WIDTH);
			int iLeftTop = xX;
			if(	(iLeftTop < m_maxX && iLeftTop >= m_minX) || ( iRightTop < m_maxX && iRightTop >= m_minX )) {
				if(c > ' ') {
					X = WWidgetManager::m_GlyphArray[c-32].uvCoords[0];
					Y = WWidgetManager::m_GlyphArray[c-32].uvCoords[1];

					WWidgetManager::setColor(0xff000000);
					WWidgetManager::drawFont(xX, yY, CHAR_WIDTH, WWidgetManager::CHARACTER_HEIGHT, X, Y);
					WWidgetManager::resetColor();
//printf("%c", c);
					if(m_bIsSelecting)
						bDrawSelect = isUnderSelection(lineNo, i);
				}
				else
				if(c == ' ' || c == '\t') {
					if(m_bIsSelecting)
						bDrawSelect = isUnderSelection(lineNo, i);
				}

				if(bDrawSelect) {
					Rect rect(xX, yY, CHAR_WIDTH, LINE_HEIGHT);
					WWidgetManager::getInstance()->fillRect(128.0f, 128.0f, 128.0f, 128.0f, &rect);
				}
			}

			if(iLeftTop >= m_maxX || iRightTop >= m_maxX) {
				bSkipRestLine = true;
			}

			xX += CHAR_WIDTH;
		}

		i++;

		if(i >= m_Lines[lineNo].size() || bSkipRestLine) {
			bSkipRestLine = false;
			if(lineNo == CURSOR_LINE_NO) {//Current line Selection.
				rect.X = m_minX;
				rect.Y = yY;
				rect.Width = (m_maxX - m_minX) + CHAR_WIDTH;
				rect.Height = LINE_HEIGHT;
				
				WWidgetManager::getInstance()->fillRect(0.0f, 0.0f, 255.0f, 32.0f, &rect);
			}

			lineNo++;
			if(lineNo >= m_Lines.size())
				break;
			i = 0;
			yY += LINE_HEIGHT;
			xX = x;
//printf("\n");
		}
	}

//printf("Ex drawStringFont\n");
}

bool WTextBox::isUnderSelection(int lineNo, int column) {
	bool bRet = false;

	if(lineNo == CURSOR_LINE_NO && lineNo == SEL_CURSOR_LINE_NO) {
		if(COLUMN_NO > SEL_COLUMN_NO) {
			if(column >= SEL_COLUMN_NO && column < COLUMN_NO)
				bRet = true;
		}
		if(COLUMN_NO < SEL_COLUMN_NO) {
			if(column >= COLUMN_NO && column < SEL_COLUMN_NO)
				bRet = true;
		}
	}
	else
	if(CURSOR_LINE_NO > SEL_CURSOR_LINE_NO) {
		if(	lineNo == SEL_CURSOR_LINE_NO && column >= SEL_COLUMN_NO
			||
			lineNo == CURSOR_LINE_NO && column < COLUMN_NO
			||
			lineNo > SEL_CURSOR_LINE_NO && lineNo < CURSOR_LINE_NO
		) {
			bRet = true;
		}
	}
	else
	if(CURSOR_LINE_NO < SEL_CURSOR_LINE_NO) {
		if(	lineNo == CURSOR_LINE_NO && column >= COLUMN_NO
			||
			lineNo == SEL_CURSOR_LINE_NO && column < SEL_COLUMN_NO
			||
			lineNo > CURSOR_LINE_NO && lineNo < SEL_CURSOR_LINE_NO
		) {
			bRet = true;
		}
	}

	return bRet;
}

void WTextBox::getCaretPos(int x, int y) {
//printf("In getCaretPos\n");

	int lineNo = abs(y)/LINE_HEIGHT;
	if(lineNo >= m_Lines.size() || (CURSOR_LINE_NO == 0 && y < 0))
		return;

	CCString str = m_Lines[lineNo].c_str();

	int xX = 0;
	int yY = lineNo*LINE_HEIGHT;

	CURSOR_LINE_NO = lineNo;
	int i = 0;
	int caretPos = 0;

	if(str.length() == 0) {
		m_CaretPosX = xX;
		m_CaretPosY = yY;
		CURSOR_LINE_NO = lineNo;
		COLUMN_NO = i;

		return;
	}

	int CHAR_WIDTH = 0;
	while(true) {
		
		char c = str[i];
		CHAR_WIDTH = WWidgetManager::getCharWidth(c);
//printf("W getCaretPos, ==== %d <= %d <= %d\n", xX, x, xX+CHAR_WIDTH);
//printf("H getCaretPos, ==== %d <= %d <= %d\n", yY, y, yY+CHAR_HEIGHT);
		if(x >= xX && x <= xX + CHAR_WIDTH) {
			if(y >= yY && y < yY + LINE_HEIGHT) {
//printf("DDDDDDDDDDDDDDDD getCaretPos, ==== i = %d, %c\n", i, c);
				m_CaretPosX = xX;
				m_CaretPosY = yY;
				CURSOR_LINE_NO = lineNo;
				COLUMN_NO = i;

				return;
			}
		}

		xX += CHAR_WIDTH;
		i++;

		if(	i >= str.length()) {
			if(x >= xX) {
				if(y >= yY && y < yY+LINE_HEIGHT) {
					m_CaretPosX = xX;
					m_CaretPosY = yY;
					CURSOR_LINE_NO = lineNo;
					COLUMN_NO = i;

					return;
				}
			}
			
			break;
//printf("\n");
		}
	}

//printf("Ex getCaretPos\n");
	return;
}

void WTextBox::onUpdate() {

	updateComponentPosition();

	m_iMaxWidthPixels = m_iMaxHeightPixels = 0;
	
	setVScrollbarLength();
	setHScrollbarLength();

	updateScrollBarVisibility();

	if(!m_IsVScrolling)
		updateVBarPosition();
	if(!m_IsHScrolling)
		updateHBarPosition();
	
	/////////////// RE-ADJUST MAINS
	if(m_mainY > 0)
		m_mainY = 0;
	if(abs(m_mainY) > (m_Lines.size()*LINE_HEIGHT - LINES_PER_PAGE*LINE_HEIGHT))
		m_mainY = -(m_Lines.size()*LINE_HEIGHT - LINES_PER_PAGE*LINE_HEIGHT);

	if(m_mainX > 0)
		m_mainX = 0;
	if(m_MaxLineWidth >= CHARS_PER_PAGE) {
		if(abs(m_mainX) > (m_MaxLineWidth*WWidgetManager::CHARACTER_WIDTH - CHARS_PER_PAGE*WWidgetManager::CHARACTER_WIDTH))
			m_mainX = -(m_MaxLineWidth*WWidgetManager::CHARACTER_WIDTH - LINES_PER_PAGE*WWidgetManager::CHARACTER_WIDTH);
	}
	///////////////////////////////

	PREV_CURSOR_LINE_NO = CURSOR_LINE_NO;
}

void WTextBox::updateScrollBarVisibility() {
	m_minX = getLeft() + ((HAS_LINE_NO)?TB_LINE_NO_SPACE:0) + TB_LEFT_GUTTER;
	m_minY = getTop() + TB_TOP_GUTTER;

	/*
	///////// VERTICAL
	bool bVertSBVisibility = (m_Lines.size() > LINES_PER_PAGE);
	m_sbVertical->setVisible(bVertSBVisibility);
	if(!bVertSBVisibility) {
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth + m_sbVertical->getWidth());
		m_maxX = getRight() - TB_RIGHT_GUTTER;
	}
	else {
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth);
		m_maxX = getRight() - m_sbVertical->getWidth() - TB_RIGHT_GUTTER;
	}
	CHARS_PER_PAGE = (m_maxX-m_minX)/WWidgetManager::CHARACTER_HEIGHT;
	///////////////////////////////////////////////////

	///////// HORIZONTAL
	bool bHoriSBVisibility = (m_MaxLineWidth > CHARS_PER_PAGE);
	m_sbHorizontal->setVisible(bHoriSBVisibility);
	if(!bHoriSBVisibility) {
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight + m_sbHorizontal->getHeight());
		m_maxY = getBottom() - TB_LEFT_GUTTER;
	}
	else {
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight);
		m_maxY = getBottom() - m_sbHorizontal->getHeight() - TB_LEFT_GUTTER;
	}

	LINES_PER_PAGE = (m_maxY-m_minY)/LINE_HEIGHT;
	///////////////////////////////////////////////////
	//*/

	///*
	m_iMaxHeightPixels = m_Lines.size()*LINE_HEIGHT;
	m_iMaxWidthPixels = m_MaxLineWidth*WWidgetManager::CHARACTER_WIDTH;

	///////// VERTICAL
	bool bVertSBVisibility = (m_iMaxHeightPixels > m_ClientRect.Height);
	m_sbVertical->setVisible(bVertSBVisibility);
	if(!bVertSBVisibility) {
		m_ClientRect.Width = m_iClientRectW + m_sbVertical->getWidth();
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth + m_sbVertical->getWidth());
	}
	else {
		m_ClientRect.Width = m_iClientRectW;
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth);
	}
	if(!bVertSBVisibility)	m_maxX = getRight() - TB_RIGHT_GUTTER;
	else					m_maxX = getRight() - m_sbVertical->getWidth() - TB_RIGHT_GUTTER;
	CHARS_PER_PAGE = (m_maxX-m_minX)/WWidgetManager::CHARACTER_WIDTH;
	///////////////////////////////////////////////////

	///////// HORIZONTAL
	bool bHoriSBVisibility = (m_iMaxWidthPixels > m_ClientRect.Width);
	m_sbHorizontal->setVisible(bHoriSBVisibility);
	if(!bHoriSBVisibility) {
		m_ClientRect.Height = m_iClientRectH + m_sbHorizontal->getHeight();
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight + m_sbHorizontal->getHeight());
	}
	else {
		m_ClientRect.Height = m_iClientRectH;
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight);
	}
	if(!bHoriSBVisibility)	m_maxY = getBottom() - TB_LEFT_GUTTER;
	else					m_maxY = getBottom() - m_sbHorizontal->getHeight() - TB_LEFT_GUTTER;
	LINES_PER_PAGE = (m_maxY-m_minY)/LINE_HEIGHT;
	///////////////////////////////////////////////////
	//*/
}

void WTextBox::updateVBarPosition() {
	float _part = abs(m_mainY);
	float _total = m_Lines.size()*LINE_HEIGHT;
	float _percentage = (_part / _total) * 100;

	m_sbVertical->setCursorPositionInPercent(_percentage);
}

void WTextBox::updateHBarPosition() {
	float _part = abs(m_mainX);
	float _total = m_MaxLineWidth*WWidgetManager::CHARACTER_WIDTH;
	float _percentage = (_part / _total) * 100;

	m_sbHorizontal->setCursorPositionInPercent(_percentage);
}

void WTextBox::onRender() {
	setClip(getLeft(), getTop(), getWidth(), getHeight());

	WWidgetManager* renderer =  WWidgetManager::getInstance();
	RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
	
		CHILD* child = m_TextBoxWidget->getChild("TextArea");
		renderer->renderChild(m_TextBoxWidget, child, &thisWndRect);
		//renderer->renderClientArea(m_TextBoxWidget, 0, thisWndRect);
		/////////////////////////////

		if(mState == READONLY) {
			Rect readOnlyRect(thisWndRect.X, thisWndRect.Y, thisWndRect.Width, thisWndRect.Height);
			WWidgetManager::getInstance()->fillRect(128.0f, 128.0f, 128.0f, 128.0f, &readOnlyRect);
		}

		/////////////////////////////
		resetClip();

		setClip(m_minX, m_minY, m_maxX - m_minX, m_maxY - m_minY);
		drawStringFont(m_minX + m_mainX, m_minY + m_mainY, 0);
		resetClip();

		setClip(getLeft(), getTop(), getWidth(), getHeight());

		drawLineNumbers();
		/////////////////////////////

		if(hasKeyFocus()) {
			WWidgetManager::drawVerticalLine(	m_minX + m_mainX + m_CaretPosX, 
												m_minY + m_mainY + m_CaretPosY,
												m_minX + m_mainX + m_CaretPosX, 
												m_minY + m_mainY + m_CaretPosY + LINE_HEIGHT);
		}
		
		setSelection();

		resetClip();
}

void WTextBox::drawLineNumbers() {
	if(HAS_LINE_NO) {
		Rect rect(getLeft(), getTop(), TB_LINE_NO_SPACE, getHeight());
		WWidgetManager::getInstance()->fillRect(128.0f, 128.0f, 128.0f, 128.0f, &rect);

		int startLineNo = abs(m_mainY)/LINE_HEIGHT;
		int endLineNo = (m_maxY-m_minY)/LINE_HEIGHT;
		for(int i = 0; i <= endLineNo; i++) {

			if((startLineNo+i) > m_Lines.size()-1)
				break;

			memset(dbStr, 0, 255);
			sprintf(dbStr, "%d", (startLineNo+i));
			WWidgetManager::drawStringFont(dbStr, getLeft() + TB_LINE_NO_SPACE - TB_LEFT_GUTTER, m_minY+(i*LINE_HEIGHT), 2);
		}
	}
}

void WTextBox::setClip(int x, int y , int width, int height) {
	WWidgetManager::GetClipBounds(&m_reclaimRect);

	RectF clipRect(x, y, width, height);
	RectF::Intersect(clipRect, m_reclaimRect, clipRect);

	WWidgetManager::setClip(clipRect.X, clipRect.Y, clipRect.Width, clipRect.Height);
}

void WTextBox::resetClip() {
	WWidgetManager::setClip(m_reclaimRect.X, m_reclaimRect.Y, m_reclaimRect.Width, m_reclaimRect.Height);
}

void WTextBox::setSelection() {
	
	if(m_bIsSelecting) {
	}
	else {
		SEL_CURSOR_LINE_NO = CURSOR_LINE_NO;
		SEL_COLUMN_NO = COLUMN_NO;
	}
}

void WTextBox::onMouseDownEx(int x, int y, int iButton) {

	m_IsVScrolling = m_IsHScrolling = false;

	if(	x < m_minX || x > m_maxX
		||
		y < m_minY || y > m_maxY
	) {
		return;
	}

	WWidgetManager::setCursor(IDC__IBEAM);
	if(m_bIsSelecting && !GetAsyncKeyState(VK_SHIFT)) {
		m_bIsSelecting = false;
	}	

	getCaretPos(x-m_minX-m_mainX, y-m_minY-m_mainY);
	setCaretDrawPosition();
}

void WTextBox::onMouseUpEx(int x, int y, int iButton) {

	if(x < m_minX || x > m_maxX
		||
		y < m_minY || y > m_maxY
	) {
		return;
	}
		
	WWidgetManager::setCursor(IDC__IBEAM);
}

void WTextBox::onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY) {

}

void WTextBox::onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY) {
	if(x < m_minX || x > m_maxX
		||
		y < m_minY || y > m_maxY
	) {
		
	}
	else
		WWidgetManager::setCursor(IDC__IBEAM);
}

void WTextBox::onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY) {
	WWidgetManager::setCursor(IDC__ARROW);
}

void WTextBox::onMouseMoveEx(int mCode, int x, int y, int prevX, int prevY) {
		
	if(m_IsVScrolling || m_IsHScrolling)
		return;

	bool bHasHScrollBar = (m_MaxLineWidth > CHARS_PER_PAGE);

	if((x < m_minX || x > m_maxX
		 ||
		 y < m_minY || y > m_maxY
		)
		&&
		bHasHScrollBar
	) {
			return;
	}

	WWidgetManager::setCursor(IDC__IBEAM);

	if(mCode & MK_LBUTTON) {
		m_bIsSelecting = true;

		if(m_bIsSelecting) {
			getCaretPos(x-m_minX-m_mainX, y-m_minY-m_mainY);

			setCaretDrawPosition();
			updateMains();
		}
	}
	else
	if(mCode & MK_RBUTTON) {
		m_mainX += (x - prevX);
		if(m_mainX > 0)
			m_mainX = 0;
		
		m_mainY += (y - prevY);
		if(m_mainY > 0)
			m_mainY = 0;
	}
}

void WTextBox::onMouseWheelEx(WPARAM wParam, LPARAM lParam) {
//printf("WTextBox::onMouseWheel\n");

	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	if(zDelta < 0) {
		m_mainY -= LINE_HEIGHT;
		if(abs(m_mainY) > (m_Lines.size()*LINE_HEIGHT - LINES_PER_PAGE*LINE_HEIGHT))
			m_mainY = -(m_Lines.size()*LINE_HEIGHT - LINES_PER_PAGE*LINE_HEIGHT);
	}
	else {
		m_mainY += LINE_HEIGHT;
		if(m_mainY > 0)
			m_mainY = 0;
	}
}

bool WTextBox::isReadOnlyChar(char ch) {
	bool bRet = false;

	if(	(GetAsyncKeyState(VK_CONTROL) && (ch == 'C' || ch == 'c'))
		||
		ch == VK_PRIOR
		||
		ch == VK_NEXT
		||
		ch == VK_LEFT
		||
		ch == VK_RIGHT
		||
		ch == VK_UP
		||
		ch == VK_DOWN
		||
		ch == VK_HOME
		||
		ch == VK_END
	)
		bRet = true;

	return bRet;
}

void WTextBox::onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch) {
//printf("In onKeyBDown\n");
	
	if(mState == READONLY && !isReadOnlyChar(iVirtualKeycode))
		return;
	else
	if(GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_SHIFT) && iVirtualKeycode == 'L') {
		showLineNumbers(!HAS_LINE_NO);
	}

	m_IsVScrolling = m_IsHScrolling = false;

	CCString leftHalfSubstr;
	CCString rightHalfSubstr;
	
//char* ss = new char[255];
//sprintf(ss, "%d", iVirtualKeycode);
//MessageBox(NULL, ss, "AsdaS", MB_OK);

	switch(iVirtualKeycode) {
		case 'X':
		case 'C':
		{
			if(GetAsyncKeyState(VK_CONTROL)) {
				if(OpenClipboard(GetForegroundWindow())) {
					bool isCut = (iVirtualKeycode == 'X');

					CCString leftStrUnderSelection;
					CCString rightStrUnderSelection;
					CCString strUnderSelection;

					if(CURSOR_LINE_NO == SEL_CURSOR_LINE_NO) {
						CCString middleString;

						if(COLUMN_NO > SEL_COLUMN_NO) {
							leftHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(0, SEL_COLUMN_NO).c_str();
							rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();
							middleString = m_Lines[CURSOR_LINE_NO].substr(SEL_COLUMN_NO, (COLUMN_NO-SEL_COLUMN_NO)).c_str();
							
							if(isCut)
								COLUMN_NO = SEL_COLUMN_NO;
						}
						else {
							leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
							rightHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(SEL_COLUMN_NO).c_str();
							middleString = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO, (SEL_COLUMN_NO-COLUMN_NO)).c_str();
						}
						
						if(isCut) {
							m_Lines[CURSOR_LINE_NO] = "";
							m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
							m_Lines[CURSOR_LINE_NO] += rightHalfSubstr.c_str();
							m_bIsSelecting = false;
						}
						
						delete[] WWidgetManager::m_clipboardTextData;
						WWidgetManager::m_clipboardTextData = new char[middleString.length()+1];
						memset(WWidgetManager::m_clipboardTextData, 0, middleString.length()+1);
						sprintf(WWidgetManager::m_clipboardTextData, "%s", middleString.c_str());
						//SetClipboardData(CF_TEXT, WWidgetManager::m_clipboardTextData);
					}
					else
					if(CURSOR_LINE_NO > SEL_CURSOR_LINE_NO) {
						leftHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(0, SEL_COLUMN_NO).c_str();
						rightStrUnderSelection = m_Lines[SEL_CURSOR_LINE_NO].substr(SEL_COLUMN_NO).c_str();

						rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();
						leftStrUnderSelection = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
							
						strUnderSelection += rightStrUnderSelection.c_str();
						strUnderSelection += "\r\n";
						for(int ii = 0; ii < (CURSOR_LINE_NO - SEL_CURSOR_LINE_NO); ii++) {
							if(ii == (CURSOR_LINE_NO-SEL_CURSOR_LINE_NO)-1)
								continue;
							strUnderSelection += m_Lines[SEL_CURSOR_LINE_NO + 1 + ii].c_str();
							strUnderSelection += "\r\n";
						}
						strUnderSelection += leftStrUnderSelection.c_str();

						delete[] WWidgetManager::m_clipboardTextData;
						WWidgetManager::m_clipboardTextData = new char[strUnderSelection.length()];
						memset(WWidgetManager::m_clipboardTextData, 0, strUnderSelection.length());
						sprintf(WWidgetManager::m_clipboardTextData, "%s", strUnderSelection.c_str());
						//SetClipboardData(CF_TEXT, WWidgetManager::m_clipboardTextData);

						if(isCut) {
							for(int ii = 0; ii < (CURSOR_LINE_NO-SEL_CURSOR_LINE_NO); ii++) {
								m_Lines[SEL_CURSOR_LINE_NO + 1].~basic_string();
								m_Lines.erase(m_Lines.begin() + SEL_CURSOR_LINE_NO + 1);
							}

							m_Lines[SEL_CURSOR_LINE_NO] = "";
							m_Lines[SEL_CURSOR_LINE_NO] += leftHalfSubstr.c_str();
							m_Lines[SEL_CURSOR_LINE_NO] += rightHalfSubstr.c_str();

							m_bIsSelecting = false;
							CURSOR_LINE_NO = SEL_CURSOR_LINE_NO;
							COLUMN_NO = SEL_COLUMN_NO;
						}
					}
					else 
					if(CURSOR_LINE_NO < SEL_CURSOR_LINE_NO) {
						leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
						rightStrUnderSelection = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();

						rightHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(SEL_COLUMN_NO).c_str();
						leftStrUnderSelection = m_Lines[SEL_CURSOR_LINE_NO].substr(0, SEL_COLUMN_NO).c_str();
						
						strUnderSelection += rightStrUnderSelection.c_str();
						strUnderSelection += "\r\n";
						for(int ii = 1; ii < (SEL_CURSOR_LINE_NO-CURSOR_LINE_NO); ii++) {
							if(ii == (CURSOR_LINE_NO-SEL_CURSOR_LINE_NO)-1)
								continue;
							strUnderSelection += m_Lines[CURSOR_LINE_NO + ii].c_str();
							strUnderSelection += "\r\n";
						}
						strUnderSelection += leftStrUnderSelection.c_str();

						delete[] WWidgetManager::m_clipboardTextData;
						WWidgetManager::m_clipboardTextData = new char[strUnderSelection.length()+1];
						memset(WWidgetManager::m_clipboardTextData, 0, strUnderSelection.length()+1);
						sprintf(WWidgetManager::m_clipboardTextData, "%s", strUnderSelection.c_str());
						//SetClipboardData(CF_TEXT, WWidgetManager::m_clipboardTextData);
						
						if(isCut) {
							for(int ii = 0; ii < (SEL_CURSOR_LINE_NO-CURSOR_LINE_NO); ii++) {
								m_Lines[CURSOR_LINE_NO + 1].~basic_string();
								m_Lines.erase(m_Lines.begin() + CURSOR_LINE_NO + 1);
							}

							m_Lines[CURSOR_LINE_NO] = "";
							m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
							m_Lines[CURSOR_LINE_NO] += rightHalfSubstr.c_str();

							m_bIsSelecting = false;
						}
					}

					setCaretDrawPosition();
					updateMains();
				}
			}
		}
		break;
		case 'V':
		{
			if(GetAsyncKeyState(VK_CONTROL)) {
				//if(OpenClipboard(GetForegroundWindow())) 
				{
					if(m_bIsSelecting)
						deleteSelectedText();

					pasteText();
					
					setCaretDrawPosition();
					updateMains();

					return;
				}
			}
		}
		break;
		case VK_PRIOR:
		{
			CURSOR_LINE_NO -= LINES_PER_PAGE;
			if(CURSOR_LINE_NO < 0)
				CURSOR_LINE_NO = 0;

			CCString str = m_Lines[CURSOR_LINE_NO].c_str();
			if(COLUMN_NO > str.length())
				COLUMN_NO = str.length();

			setCaretDrawPosition();
			updateMains();

			return;
		}
		break;
		case VK_NEXT:
		{
			CURSOR_LINE_NO += LINES_PER_PAGE;
			if(CURSOR_LINE_NO >= m_Lines.size())
				CURSOR_LINE_NO = m_Lines.size()-1;

			CCString str = m_Lines[CURSOR_LINE_NO].c_str();
			if(COLUMN_NO > str.length())
				COLUMN_NO = str.length();

			setCaretDrawPosition();
			updateMains();
			return;
		}
		break;
		case VK_UP:
		{
			scrollVertically(-1);
			return;
		}
		break;
		case VK_DOWN:
		{
			scrollVertically(1);
			return;
		}
		break;
		case VK_LEFT:
		{
			bool bWasSelecting = m_bIsSelecting;
			m_bIsSelecting = GetAsyncKeyState(VK_SHIFT);
			
			CCString str = m_Lines[CURSOR_LINE_NO].c_str();
			/* Move Caret */
			if(GetAsyncKeyState(VK_CONTROL)) {
				if(COLUMN_NO > 0)
					COLUMN_NO = WWidgetManager::getNextWhitespacePos(str.c_str(), COLUMN_NO-1, true);
				else
					COLUMN_NO = -1;
			}
			else {
				COLUMN_NO--;
			}
			
			if(COLUMN_NO < 0) {
				CURSOR_LINE_NO--;
				bool bIsFirstLine = (CURSOR_LINE_NO < 0);
				if(bIsFirstLine) {
					CURSOR_LINE_NO = 0;
					COLUMN_NO = 0;
				}
				else {
					COLUMN_NO = m_Lines[CURSOR_LINE_NO].size();
				}
			}
			
			/* Set Caret Position & Update Viewport*/
			setCaretDrawPosition();
			updateMains();

			return;
		}
		break;
		case VK_RIGHT:
		{	
			int prevColumnNo = COLUMN_NO;
			bool bWasSelecting = m_bIsSelecting;
			m_bIsSelecting = GetAsyncKeyState(VK_SHIFT);
			
			CCString str = m_Lines[CURSOR_LINE_NO].c_str();
			/* Move Caret */
			if(GetAsyncKeyState(VK_CONTROL)) {
				if(COLUMN_NO < str.length()-1)
					COLUMN_NO = WWidgetManager::getNextWhitespacePos(str.c_str(), COLUMN_NO, false) + 1;
				else
					COLUMN_NO = str.length()+1;
			}
			else {
				COLUMN_NO++;
			}
			
			if(COLUMN_NO > str.length()) {
				COLUMN_NO = 0;
				CURSOR_LINE_NO++;
				if(CURSOR_LINE_NO >= m_Lines.size()) {
					CURSOR_LINE_NO = m_Lines.size()-1;
					COLUMN_NO = prevColumnNo;
				}
			}
			
			/* Set Caret Position & Update Viewport*/
			setCaretDrawPosition();
			updateMains();

			return;
		}
		break;
		case VK_HOME:
		{
			m_bIsSelecting = GetAsyncKeyState(VK_SHIFT);

			COLUMN_NO = 0;

			if(GetAsyncKeyState(VK_CONTROL))
				CURSOR_LINE_NO = 0;

			setCaretDrawPosition();
			updateMains();

			return;
		}
		break;
		case VK_END:
		{	
			bool bCtrl = GetAsyncKeyState(VK_CONTROL);
			m_bIsSelecting = GetAsyncKeyState(VK_SHIFT);
			
			CCString str = m_Lines[CURSOR_LINE_NO].c_str();
			COLUMN_NO = str.length();

			if(bCtrl) {
				CURSOR_LINE_NO = m_Lines.size()-1;
				CCString str = m_Lines[CURSOR_LINE_NO].c_str();
				COLUMN_NO = str.length();
			}

			setCaretDrawPosition();
			updateMains();

			return;
		}
		break;
		case VK_BACK:
		{
			bool bWasSelecting = m_bIsSelecting;
			if(m_bIsSelecting) {
				deleteSelectedText();
			}
			else {
				if(COLUMN_NO > 0) {
					leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO-1).c_str();
					rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();

					if(rightHalfSubstr.length() > 0) {
						m_Lines[CURSOR_LINE_NO] = "";
						m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
						m_Lines[CURSOR_LINE_NO] += rightHalfSubstr.c_str();
					}
					else {
						m_Lines[CURSOR_LINE_NO].assign(leftHalfSubstr.c_str());
					}

					COLUMN_NO--;
				}
				else {
					if(CURSOR_LINE_NO > 0) {
						rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();

						m_Lines[CURSOR_LINE_NO].~basic_string();
						m_Lines.erase(m_Lines.begin() + CURSOR_LINE_NO);

						CURSOR_LINE_NO--;
						COLUMN_NO = m_Lines[CURSOR_LINE_NO].size();
						m_Lines[CURSOR_LINE_NO].append(rightHalfSubstr.c_str());
					}
				}
			}
			
			setCaretDrawPosition();
			updateMains();
			
			return;
		}
		break;
		case VK_DELETE:
		{
			bool bWasSelecting = m_bIsSelecting;
			if(m_bIsSelecting) {
				deleteSelectedText();
			}
			else {
				if(COLUMN_NO < m_Lines[CURSOR_LINE_NO].size()) {
					leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
					rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO+1).c_str();

					if(rightHalfSubstr.length() > 0) {
						m_Lines[CURSOR_LINE_NO] = "";
						m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
						m_Lines[CURSOR_LINE_NO] += rightHalfSubstr.c_str();
					}
					else
						m_Lines[CURSOR_LINE_NO].assign(leftHalfSubstr.c_str());
				}
				else {
					if(CURSOR_LINE_NO < m_Lines.size()-1) {
						leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
						rightHalfSubstr = m_Lines[CURSOR_LINE_NO+1].c_str();
						leftHalfSubstr += rightHalfSubstr.c_str();
						m_Lines[CURSOR_LINE_NO].append(leftHalfSubstr.c_str());
						
						m_Lines[CURSOR_LINE_NO + 1].~basic_string();
						m_Lines.erase(m_Lines.begin() + CURSOR_LINE_NO + 1);
					}
				}
			}
			
			setCaretDrawPosition();
			updateMains();
			
			return;
		}
		break;
	}

	if( ch >= 32 && ch <= 126 ) {
		bool bWasSelecting = m_bIsSelecting;
		if(m_bIsSelecting) {
			if(CURSOR_LINE_NO == SEL_CURSOR_LINE_NO) {
				if(COLUMN_NO > SEL_COLUMN_NO) {
					leftHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(0, SEL_COLUMN_NO).c_str();
					rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();

					m_Lines[SEL_CURSOR_LINE_NO] = "";
					m_Lines[SEL_CURSOR_LINE_NO] += leftHalfSubstr.c_str();
					m_Lines[SEL_CURSOR_LINE_NO] += rightHalfSubstr.c_str();
					COLUMN_NO = SEL_COLUMN_NO;
				}
				else {
					leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
					rightHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(SEL_COLUMN_NO).c_str();

					m_Lines[SEL_CURSOR_LINE_NO] = "";
					m_Lines[SEL_CURSOR_LINE_NO] += leftHalfSubstr.c_str();
					m_Lines[SEL_CURSOR_LINE_NO] += rightHalfSubstr.c_str();
				}
				
				m_bIsSelecting = false;
			}
			else
			if(CURSOR_LINE_NO > SEL_CURSOR_LINE_NO) {
				leftHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(0, SEL_COLUMN_NO).c_str();
				rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();
				
				m_Lines[SEL_CURSOR_LINE_NO] = "";
				m_Lines[SEL_CURSOR_LINE_NO] += leftHalfSubstr.c_str();
				m_Lines[SEL_CURSOR_LINE_NO] += rightHalfSubstr.c_str();

				for(int ii = 0; ii < (CURSOR_LINE_NO-SEL_CURSOR_LINE_NO); ii++) {
					m_Lines[SEL_CURSOR_LINE_NO + 1].~basic_string();
					m_Lines.erase(m_Lines.begin() + SEL_CURSOR_LINE_NO + 1);
				}

				COLUMN_NO = SEL_COLUMN_NO;
				CURSOR_LINE_NO = SEL_CURSOR_LINE_NO;
				m_bIsSelecting = false;
			}
			else
			if(SEL_CURSOR_LINE_NO > CURSOR_LINE_NO) {
				leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
				rightHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(SEL_COLUMN_NO).c_str();
				
				m_Lines[CURSOR_LINE_NO] = "";
				m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
				m_Lines[CURSOR_LINE_NO] += rightHalfSubstr.c_str();
				
				for(int ii = 0; ii < (SEL_CURSOR_LINE_NO-CURSOR_LINE_NO); ii++) {
					m_Lines[CURSOR_LINE_NO + 1].~basic_string();
					m_Lines.erase(m_Lines.begin() + CURSOR_LINE_NO + 1);
				}

				m_bIsSelecting = false;
			}
		}
		else {
			leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
			rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();
		}

		if(rightHalfSubstr.length() > 0) {
			m_Lines[CURSOR_LINE_NO] = "";
			m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
			m_Lines[CURSOR_LINE_NO] += ch;
			m_Lines[CURSOR_LINE_NO] += rightHalfSubstr.c_str();
		}	
		else 
		if(leftHalfSubstr.length() > 0) {
			m_Lines[CURSOR_LINE_NO] = "";
			m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
			m_Lines[CURSOR_LINE_NO] += ch;
		}
		else {
			m_Lines[CURSOR_LINE_NO] = "";
			m_Lines[CURSOR_LINE_NO] += ch;
		}
			
		COLUMN_NO++;
		
		setCaretDrawPosition();
		updateMains();
	}
	else
	if(ch == 13) {
		if(m_bIsSelecting) {
			if(CURSOR_LINE_NO == SEL_CURSOR_LINE_NO) {
				if(COLUMN_NO > SEL_COLUMN_NO) {
					leftHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(0, SEL_COLUMN_NO).c_str();
					rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();
				}
				else {
					leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
					rightHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(SEL_COLUMN_NO).c_str();
				}
				
				m_Lines[CURSOR_LINE_NO] = "";
				m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
				
				m_Lines.insert(m_Lines.begin() + CURSOR_LINE_NO+1, rightHalfSubstr.c_str());
				COLUMN_NO = 0;
				CURSOR_LINE_NO++;
				m_bIsSelecting = false;
			}
			else
			if(CURSOR_LINE_NO > SEL_CURSOR_LINE_NO) {
				leftHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(0, SEL_COLUMN_NO).c_str();
				rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();
				
				m_Lines[SEL_CURSOR_LINE_NO] = "";
				m_Lines[SEL_CURSOR_LINE_NO] += leftHalfSubstr.c_str();

				for(int ii = 0; ii < (CURSOR_LINE_NO-SEL_CURSOR_LINE_NO); ii++) {
					m_Lines[SEL_CURSOR_LINE_NO + 1].~basic_string();
					m_Lines.erase(m_Lines.begin() + SEL_CURSOR_LINE_NO + 1);
				}

				m_Lines.insert(m_Lines.begin() + SEL_CURSOR_LINE_NO+1, rightHalfSubstr.c_str());

				COLUMN_NO = 0;
				CURSOR_LINE_NO = SEL_CURSOR_LINE_NO+1;
				m_bIsSelecting = false;
			}
			else
			if(SEL_CURSOR_LINE_NO > CURSOR_LINE_NO) {
				leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
				rightHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(SEL_COLUMN_NO).c_str();
				
				m_Lines[CURSOR_LINE_NO] = "";
				m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
				
				for(int ii = 0; ii < (SEL_CURSOR_LINE_NO-CURSOR_LINE_NO); ii++) {
					m_Lines[CURSOR_LINE_NO + 1].~basic_string();
					m_Lines.erase(m_Lines.begin() + CURSOR_LINE_NO + 1);
				}

				m_Lines.insert(m_Lines.begin() + CURSOR_LINE_NO+1, rightHalfSubstr.c_str());
				
				COLUMN_NO = 0;
				CURSOR_LINE_NO++;
				m_bIsSelecting = false;
			}
		}
		else {
			leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
			rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();
			
			if(rightHalfSubstr.length() > 0) {
				COLUMN_NO = 0;
				CURSOR_LINE_NO++;
				
				m_Lines[CURSOR_LINE_NO-1] = leftHalfSubstr.c_str();
				m_Lines.insert(m_Lines.begin() + CURSOR_LINE_NO, rightHalfSubstr.c_str());
			}
			else {	
				COLUMN_NO = 0;
				CURSOR_LINE_NO++;
				m_Lines.insert(m_Lines.begin() + CURSOR_LINE_NO, "");
			}
		}
		
		setCaretDrawPosition();
		updateMains();
	}


	if(!m_bIsSelecting) {
		SEL_CURSOR_LINE_NO = CURSOR_LINE_NO;
		SEL_COLUMN_NO = COLUMN_NO;
	}
//printf("Ex onKeyBDown\n");
}

void WTextBox::updateMains() {
//printf("In updateMains\n");
	//////////////////////////////////////////////////////////////

	int xVal = m_minX+m_CaretPosX+m_mainX;
	if(m_CaretPosX == 0) {
		m_mainX = 0;
	}
	else
	if(xVal > m_maxX) {
		m_mainX = -(m_CaretPosX-(m_maxX-m_minX) + 1);
	}

	if(xVal < m_minX) {
		m_mainX += CHARS_PER_PAGE*WWidgetManager::CHARACTER_WIDTH;
	}
	if(m_mainX > 0)
		m_mainX = 0;

	//////////////////////////////////////////////////////////////

	int yPosTop = m_minY+(m_CaretPosY)+m_mainY;
	if(yPosTop < m_minY) {
		//m_mainY = -(m_CaretPosY/LINE_HEIGHT)*LINE_HEIGHT;
		//Or
		m_mainY = -m_CaretPosY;
	}

	int yPosBottom = m_minY + (m_CaretPosY + LINE_HEIGHT) + m_mainY;
	if(yPosBottom >= m_maxY) {
		m_mainY -= ((yPosBottom - m_maxY) + LINE_HEIGHT);
		m_mainY -= (m_mainY % LINE_HEIGHT);
	}

	if(m_mainY > 0)
		m_mainY = 0;
	
	//////////////////////////////////////////////////////////////

	calculateMaxLineWidth();
	setTBLineNoSpace();
//printf("Ex updateMains\n");
}

void WTextBox::setCaretDrawPosition() {
//printf("In setCaretDrawPosition\n");
	int xX = 0;
	int yY = CURSOR_LINE_NO*LINE_HEIGHT;

	if(CURSOR_LINE_NO < m_Lines.size()) {
		yY = CURSOR_LINE_NO*LINE_HEIGHT;

		CCString str = m_Lines[CURSOR_LINE_NO].c_str();

		for(int i = 0; i < COLUMN_NO; i++) {

			char c = str[i];
			int CHAR_WIDTH = WWidgetManager::getCharWidth(c);

			xX += CHAR_WIDTH;
		}
	}

	m_CaretPosX = xX;
	m_CaretPosY = yY;

//printf("Ex setCaretDrawPosition\n");
}

void WTextBox::onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch) {

}

void WTextBox::onMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case MOUSE_DOWN:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_VERTICAL_SCROLLBAR:
					switch(lParam) {
						case BTN_SCROLLBAR_UP:
						{
							m_mainY += LINE_HEIGHT;
							if(m_mainY > 0)
								m_mainY = 0;
						}
						break;
						case BTN_SCROLLBAR_DOWN:
						{
							m_mainY -= LINE_HEIGHT;
							if(abs(m_mainY) > (m_Lines.size()*LINE_HEIGHT - LINES_PER_PAGE*LINE_HEIGHT))
								m_mainY = -(m_Lines.size()*LINE_HEIGHT - LINES_PER_PAGE*LINE_HEIGHT);
						}
						break;
					}
				break;
				case ID_HORIZONTAL_SCROLLBAR:
					switch(lParam) {
						case BTN_SCROLLBAR_LEFT:
						{
							m_mainX += WWidgetManager::CHARACTER_WIDTH;
							if(m_mainX > 0)
								m_mainX = 0;
						}
						break;
						case BTN_SCROLLBAR_RIGHT:
						{
							m_mainX -= WWidgetManager::CHARACTER_WIDTH;
							if(abs(m_mainX) > (m_MaxLineWidth*WWidgetManager::CHARACTER_WIDTH - CHARS_PER_PAGE*WWidgetManager::CHARACTER_WIDTH))
								m_mainX = -(m_MaxLineWidth*WWidgetManager::CHARACTER_WIDTH - CHARS_PER_PAGE*WWidgetManager::CHARACTER_WIDTH);
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
					float scrollMaterialHeight = m_Lines.size()*LINE_HEIGHT - LINES_PER_PAGE*LINE_HEIGHT;
					int mainYValue = (cursorPosInPercentage*scrollMaterialHeight)/100;

					m_mainY = -(mainYValue/LINE_HEIGHT)*LINE_HEIGHT;

					m_IsVScrolling = true;
				}
				break;
				case ID_HORIZONTAL_SCROLLBAR:
				{
					float scrollMaterialWidth = m_MaxLineWidth*WWidgetManager::CHARACTER_HEIGHT - CHARS_PER_PAGE*WWidgetManager::CHARACTER_HEIGHT;
					int mainXValue = (cursorPosInPercentage*scrollMaterialWidth)/100;

					m_mainX = -(mainXValue/LINE_HEIGHT)*LINE_HEIGHT;

					m_IsHScrolling = true;
				}
				break;
			}
		}
		break;
	}
}

void WTextBox::showLineNumbers(bool bShow) {
	HAS_LINE_NO = bShow;

	setTBLineNoSpace();
}

void WTextBox::setReadOnly(bool bRd) {
	mState = bRd?READONLY:NORMAL;

	m_sbVertical->setReadOnly(bRd);
	m_sbHorizontal->setReadOnly(bRd);
}

bool WTextBox::getReadOnly() {
	return (mState == READONLY);
}

void WTextBox::setTBLineNoSpace() {
	char* sLen = new char[16];
	memset(sLen, 0, 16);
	sprintf(sLen, "%d", m_Lines.size());
	TB_LINE_NO_SPACE = WWidgetManager::getStringWidthTillPos(sLen, strlen(sLen)) + (TB_LEFT_GUTTER << 1);

	m_minX = getLeft() + ((HAS_LINE_NO)?TB_LINE_NO_SPACE:0) + TB_LEFT_GUTTER;

	delete[] sLen;
}

void WTextBox::scrollVertically(int dir) {
	if(dir < 0) {
		CURSOR_LINE_NO--;
		if(CURSOR_LINE_NO < 0)
			CURSOR_LINE_NO = 0;
	}

	else
	if(dir > 0) {
		CURSOR_LINE_NO++;
		if(CURSOR_LINE_NO >= m_Lines.size())
			CURSOR_LINE_NO = m_Lines.size()-1;
	}
	
	CCString str = m_Lines[CURSOR_LINE_NO].c_str();
	if(COLUMN_NO > str.length())
		COLUMN_NO = str.length();

	setCaretDrawPosition();
	updateMains();
}

void WTextBox::deleteSelectedText() {
	CCString leftHalfSubstr;
	CCString rightHalfSubstr;

	if(CURSOR_LINE_NO == SEL_CURSOR_LINE_NO) {
		if(COLUMN_NO > SEL_COLUMN_NO) {
			leftHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(0, SEL_COLUMN_NO).c_str();
			rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();
			COLUMN_NO = SEL_COLUMN_NO;
		}
		else {
			leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
			rightHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(SEL_COLUMN_NO).c_str();
		}

		m_bIsSelecting = false;
		m_Lines[CURSOR_LINE_NO] = "";
		m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
		m_Lines[CURSOR_LINE_NO] += rightHalfSubstr.c_str();
	}
	else
	if(CURSOR_LINE_NO > SEL_CURSOR_LINE_NO) {
		leftHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(0, SEL_COLUMN_NO).c_str();
		rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();

		m_bIsSelecting = false;
		COLUMN_NO = SEL_COLUMN_NO;
		m_Lines[SEL_CURSOR_LINE_NO] = "";
		m_Lines[SEL_CURSOR_LINE_NO] += leftHalfSubstr.c_str();
		m_Lines[SEL_CURSOR_LINE_NO] += rightHalfSubstr.c_str();

		for(int ii = 0; ii < (CURSOR_LINE_NO-SEL_CURSOR_LINE_NO); ii++) {
			m_Lines[SEL_CURSOR_LINE_NO + 1].~basic_string();
			m_Lines.erase(m_Lines.begin() + SEL_CURSOR_LINE_NO + 1);
		}

		CURSOR_LINE_NO = SEL_CURSOR_LINE_NO;
	}
	else 
	if(CURSOR_LINE_NO < SEL_CURSOR_LINE_NO) {
		leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
		rightHalfSubstr = m_Lines[SEL_CURSOR_LINE_NO].substr(SEL_COLUMN_NO).c_str();

		m_bIsSelecting = false;
		m_Lines[CURSOR_LINE_NO] = "";
		m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();
		m_Lines[CURSOR_LINE_NO] += rightHalfSubstr.c_str();

		for(int ii = 0; ii < (SEL_CURSOR_LINE_NO-CURSOR_LINE_NO); ii++) {
			m_Lines[CURSOR_LINE_NO + 1].~basic_string();
			m_Lines.erase(m_Lines.begin() + CURSOR_LINE_NO + 1);
		}
	}
}

void WTextBox::pasteText() {
	//char* pIsClipboardString = (char*)GetClipboardData(CF_TEXT);
	char* pIsClipboardString = WWidgetManager::m_clipboardTextData;
	//MessageBox(NULL, pIsClipboardString, "pIsClipboardString", MB_OK);

	CCString leftHalfSubstr;
	CCString rightHalfSubstr;

	leftHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(0, COLUMN_NO).c_str();
	rightHalfSubstr = m_Lines[CURSOR_LINE_NO].substr(COLUMN_NO).c_str();

	m_Lines[CURSOR_LINE_NO] = "";
	m_Lines[CURSOR_LINE_NO] += leftHalfSubstr.c_str();

	//////////////////////////////////////////////////
	CCString sLine;
	int startPos = 0, endPos = 0;
	bool bFirstLine = true;
	std::string strBuff = pIsClipboardString;
	while(true) {
		endPos = strBuff.find_first_of("\r\n", startPos);
		if(endPos < 0)
			break;

		sLine = strBuff.substr(startPos, endPos-startPos).c_str();

		if(bFirstLine) {
			m_Lines[CURSOR_LINE_NO] += sLine.c_str();
			bFirstLine = false;
		}
		else {
			m_Lines.insert(m_Lines.begin() + CURSOR_LINE_NO, sLine.c_str());
		}

		if(strBuff[endPos] == '\r')
			endPos++;

		CURSOR_LINE_NO++;
		startPos = endPos+1;
	}
	//////////////////////////////////////////////////

	if(bFirstLine) {//Single-Line selection
		m_Lines[CURSOR_LINE_NO] += pIsClipboardString;
		COLUMN_NO += strlen(pIsClipboardString);
	}
	else {//Multi-Line selection
		if(startPos < strBuff.size())
			m_Lines.insert(m_Lines.begin() + (CURSOR_LINE_NO), strBuff.substr(startPos, strBuff.size() - startPos));

		COLUMN_NO = strBuff.size() - startPos;
	}

	if(rightHalfSubstr.length() > 0)
		m_Lines[CURSOR_LINE_NO] += rightHalfSubstr.c_str();
}

int WTextBox::getTextLength() {
	m_sText = "";
	for(size_t i = 0; i < m_Lines.size(); i++) {
		m_sText += m_Lines[i].c_str();
		if(i < m_Lines.size() - 1)
			m_sText += "\n";
	}

	return strlen(m_sText.c_str());
}

const char* WTextBox::getText() {

	m_sText = "";
	for(size_t i = 0; i < m_Lines.size(); i++) {
		m_sText += m_Lines[i].c_str();
		if(i < m_Lines.size() - 1)
			m_sText += "\n";
	}

	return m_sText.c_str();
}

LRESULT WTextBox::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case WM__GETTEXTLENGTH:
		{
			return getTextLength();
		}
		break;
		case WM__GETTEXT:
			{
				return sprintf_s((char*)lParam, (size_t)wParam, "%s", getText());
			}
			break;
		case WM__SETTEXT:
		{
			setText((char*)lParam);
			return 1;
		}
		break;
		case WM__APPENDTEXT:
		{
			appendText((char*)lParam);
			return 1;
		}
		break;
	}

	return -1;
}

WTextBox::~WTextBox() {
	int i = 0;
	while(m_Lines.size() > 0) {
		m_Lines[i].~basic_string();
		i++;
	}

	m_Lines.~vector();

	delete[] dbStr;
}
