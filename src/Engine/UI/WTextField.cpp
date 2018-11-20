#include "Engine/UI/WTextField.h"
#include "Engine/UI/WWidgetManager.h"
#include <algorithm>

using namespace std;

#define TB_LEFT_GUTTER		5
#define TB_TOP_GUTTER		3
#define TB_RIGHT_GUTTER		10
#define	 TB_TOP_GUTTER				2

unsigned int WTextField::LINE_HEIGHT;

WTextField::WTextField()
	: m_clipData(NULL)
{

}

H_WND WTextField::Create(		const char* lpClassName, 
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
	WTextField* pWTextField = new WTextField();
	return pWTextField->createWindow(lpClassName, lpWindowName, dwStyle, x, y, width, height, hwndParent, hMenu, lpVoid);
}

H_WND WTextField::createWindow(	const char* lpClassName, 
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

	LINE_HEIGHT = WWidgetManager::CHARACTER_HEIGHT + (TB_TOP_GUTTER << 1);
	setText(lpWindowName);

	m_pParent = (WContainer*)hwndParent;
	m_HwndID = (int)hMenu;
	
	m_iOffsetX = x;
	m_iOffsetY = y;

	m_iLeft = m_pParent->getLeft() + m_iOffsetX + m_pParent->m_iMainX;
	m_iTop = m_pParent->getTop() + m_iOffsetY + m_pParent->m_iMainY;
	m_iRight = m_iLeft + width;
	m_iBottom = m_iTop + LINE_HEIGHT;

	mState = NORMAL;
	m_CaretPosX = m_CaretPosY = 0;
	m_mainX = 0;
	m_mainY = 0;

	m_bIsSelecting = false;
	setBorderVisibility(true);

	m_TextBoxWidget = WWidgetManager::getWidget("TextBox");
	SEL_COLUMN_NO = COLUMN_NO = 0;
	mState = NORMAL;

	m_minX = getLeft() + TB_LEFT_GUTTER;
	m_minY = getTop() + TB_TOP_GUTTER;
	m_maxX = getRight() - TB_RIGHT_GUTTER;
	m_maxY = getBottom();

	updateMains();

	setComponentID((int)hMenu);
	setComponentAsChild(true);
	((WContainer*)m_pParent)->addComponent(this);

	return this;
}

//void WTextField::create(WComponent* parent, int x, int y, int w, int h, const char* sText, int TEXTFIELD_ID) {
//	LINE_HEIGHT = WWidgetManager::CHARACTER_HEIGHT + (TB_TOP_GUTTER << 1);
//	setText(sText);
//	
//	m_pParent = (WContainer*)parent;
//	m_HwndID = TEXTFIELD_ID;
//	setID(TEXTFIELD_ID);
//
//	m_iOffsetX = x;
//	m_iOffsetY = y;
//
//	m_iLeft = m_pParent->getLeft() + m_iOffsetX + m_pParent->m_iMainX;
//	m_iTop = m_pParent->getTop() + m_iOffsetY + m_pParent->m_iMainY;
//	m_iRight = m_iLeft + w;
//	m_iBottom = m_iTop + LINE_HEIGHT;
//
//	mState = NORMAL;
//	m_CaretPosX = m_CaretPosY = 0;
//	m_mainX = 0;
//	m_mainY = 0;
//	
//	m_bIsSelecting = false;
//	setBorderVisibility(true);
//	
//	m_TextBoxWidget = WWidgetManager::getWidget("TextBox");
//	SEL_COLUMN_NO = COLUMN_NO = 0;
//	mState = NORMAL;
//	
//	m_minX = getLeft() + TB_LEFT_GUTTER;
//	m_minY = getTop() + TB_TOP_GUTTER;
//	m_maxX = getRight() - TB_RIGHT_GUTTER;
//	m_maxY = getBottom();
//
//	updateMains();
//}

const char*	WTextField::getText() {
	return m_Lines[0].c_str();
}

int WTextField::getLength() {
	return strlen(m_Lines[0].c_str());
}

void WTextField::setText(const char* str) {
	
	int startPos = 0, endPos = 0;
	std::string strBuff = str;
	
	endPos = strBuff.find_first_of("\r\n", startPos);
	if(endPos < 0)
		endPos = strBuff.size();
	
	if(m_Lines.size() > 0) {
		std::vector<std::string>::iterator itr = m_Lines.begin();
		m_Lines.erase(itr + 0);
	}

	m_Lines.push_back(strBuff.substr(startPos, endPos-startPos));
}

void WTextField::drawStringFont(int x, int y, int anchor) {
//printf("In drawStringFont\n");

	int X = 0, Y = 0;
	
	int xX = x;
	int yY = y - m_mainY;

	CCString str = m_Lines[0].c_str();

	int i = 0;
	while(true) {
		bool bDrawSelect = false;
		if(yY >= m_maxY)
			break;
		
		int CHAR_WIDTH = 0;
		char c = ' ';

		if(m_Lines[0].size() > 0) {
			c = m_Lines[0].at(i);
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

					if(m_bIsSelecting)
						bDrawSelect = isUnderSelection(i);
				}
				else
				if(c == ' ' || c == '\t') {
					if(m_bIsSelecting)
						bDrawSelect = isUnderSelection(i);
				}

				if(bDrawSelect) {
					Rect rect(xX, m_minY, CHAR_WIDTH, WWidgetManager::CHARACTER_HEIGHT - TB_TOP_GUTTER);
					WWidgetManager::getInstance()->fillRect(128.0f, 128.0f, 128.0f, 128.0f, &rect);
				}
			}

			xX += CHAR_WIDTH;
		}

		i++;

		if(i >= str.length()) {
			break;
//printf("\n");
		}
	}

//printf("Ex drawStringFont\n");
}

bool WTextField::isUnderSelection(int column) {
	bool bRet = false;

	if(COLUMN_NO > SEL_COLUMN_NO) {
		if(column >= SEL_COLUMN_NO && column < COLUMN_NO)
			bRet = true;
	}
	if(COLUMN_NO < SEL_COLUMN_NO) {
		if(column >= COLUMN_NO && column < SEL_COLUMN_NO)
			bRet = true;
	}

	return bRet;
}

void WTextField::getCaretPos(int x, int y) {
//printf("In getCaretPos\n");

	int lineNo = 0;//abs(y)/LINE_HEIGHT;

	CCString str = m_Lines[lineNo].c_str();

	int xX = 0;
	int yY = lineNo*LINE_HEIGHT;

	int i = 0;
	int caretPos = 0;

	if(str.length() == 0) {
		m_CaretPosX = xX;
		m_CaretPosY = yY;
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
			//if(y >= yY && y < yY+LINE_HEIGHT) 
			{
//printf("DDDDDDDDDDDDDDDD getCaretPos, ==== i = %d, %c\n", i, c);
				m_CaretPosX = xX;
				m_CaretPosY = yY;
				COLUMN_NO = i;

				return;
			}
		}

		xX += CHAR_WIDTH;
		i++;

		if(	i >= str.length()) {
			if(x >= xX) {
				//if(y >= yY && y < yY+LINE_HEIGHT) 
				{
					m_CaretPosX = xX;
					m_CaretPosY = yY;
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

void WTextField::onUpdate() {
	updateComponentPosition();

	m_minX = getLeft() + TB_LEFT_GUTTER;
	m_minY = getTop() + TB_TOP_GUTTER;
	m_maxX = getRight() - TB_RIGHT_GUTTER;
	m_maxY = getBottom() - TB_TOP_GUTTER;
}

void WTextField::onRender() {
	//setClip(getLeft(), getTop(), getWidth(), getHeight());

	WWidgetManager* renderer =  WWidgetManager::getInstance();
	RectF thisWndRect(getLeft(), getTop(), getWidth(), LINE_HEIGHT);

	if(getBorderVisibility()) {
		CHILD* child = m_TextBoxWidget->getChild("TextArea");
		renderer->renderChild(m_TextBoxWidget, child, &thisWndRect);
	}
	/////////////////////////////
	if(mState == READONLY) {
		Rect rect(getLeft(), getTop(), getWidth(), getHeight());
		WWidgetManager::getInstance()->fillRect(128.0f, 128.0f, 128.0f, 128.0f, &rect);
	}
	/////////////////////////////

	/////////////////////////////
	//resetClip();

	setClip(m_minX, m_minY, m_maxX - m_minX, LINE_HEIGHT);
	drawStringFont(m_minX + m_mainX, m_minY + m_mainY, 0);
	resetClip();
	/////////////////////////////

	setClip(getLeft(), getTop(), getWidth(), getHeight());
	if(hasKeyFocus()) {
		WWidgetManager::drawVerticalLine(	m_minX + m_mainX + m_CaretPosX, 
																m_minY,
																m_minX + m_mainX + m_CaretPosX, 
																m_minY + WWidgetManager::CHARACTER_HEIGHT - TB_TOP_GUTTER);
	}	
	drawSelection();
	resetClip();
}

void WTextField::setClip(int x, int y , int width, int height) {
	WWidgetManager::GetClipBounds(&m_reclaimRect);

	RectF clipRect(x, y, width, height);
	RectF::Intersect(clipRect, m_reclaimRect, clipRect);

	WWidgetManager::setClip(clipRect.X, clipRect.Y, clipRect.Width, clipRect.Height);
}

void WTextField::resetClip() {
	WWidgetManager::setClip(m_reclaimRect.X, m_reclaimRect.Y, m_reclaimRect.Width, m_reclaimRect.Height);
}

void WTextField::drawSelection() {
	
	if(m_bIsSelecting) {
	}
	else {
		SEL_COLUMN_NO = COLUMN_NO;
	}
}

DWORD WTextField::getSelection() {

	DWORD dwRange = 0;
	if(m_bIsSelecting) {
		if(SEL_COLUMN_NO > COLUMN_NO ) {
			dwRange |= (COLUMN_NO & 0xffff);
			dwRange |= ((SEL_COLUMN_NO & 0xffff) << 16);
		}
		else
		if(COLUMN_NO > SEL_COLUMN_NO) {
			dwRange |= (SEL_COLUMN_NO & 0xffff);
			dwRange |= ((COLUMN_NO & 0xffff) << 16);
		}
	}

	return dwRange;
}

void WTextField::setSelection(DWORD dwRange) {

	short leftSel = (dwRange & 0xffff);
	short rightSel = ((dwRange >> 16) & 0xffff);

	if(leftSel < 0 || leftSel > getLength() || rightSel < 0 || rightSel > getLength()) {
		m_bIsSelecting = false;
		return;
	}
	
	m_bIsSelecting = true;
	COLUMN_NO = leftSel;
	SEL_COLUMN_NO = rightSel;

	setCaretDrawPosition();
	updateMains();
}

void WTextField::onMouseDownEx(int x, int y, int iButton) {
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

void WTextField::onMouseUpEx(int x, int y, int iButton) {
	WWidgetManager::setCursor(IDC__IBEAM);
}

void WTextField::onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY) {
	WWidgetManager::setCursor(IDC__IBEAM);
}

void WTextField::onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY) {
	WWidgetManager::setCursor(IDC__IBEAM);
}

void WTextField::onMouseMoveEx(int mCode, int x, int y, int prevX, int prevY) {

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

void WTextField::onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY) {
	WWidgetManager::setCursor(IDC__ARROW);
}

void WTextField::onMouseWheelEx(WPARAM wParam, LPARAM lParam) {
	
}

bool WTextField::isReadOnlyChar(char ch) {
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

void WTextField::onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch) {
//printf("In onKeyBDown\n");
	
	if(mState == READONLY && !isReadOnlyChar(iVirtualKeycode))
		return;

	std::string leftHalfSubstr("");
	std::string rightHalfSubstr("");
	
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

					std::string leftStrUnderSelection = "";
					std::string rightStrUnderSelection = "";
					std::string strUnderSelection = "";

					{
						std::string middleString = "";

						if(COLUMN_NO > SEL_COLUMN_NO) {
							leftHalfSubstr = m_Lines[0].substr(0, SEL_COLUMN_NO);
							rightHalfSubstr = m_Lines[0].substr(COLUMN_NO);
							middleString = m_Lines[0].substr(SEL_COLUMN_NO, (COLUMN_NO-SEL_COLUMN_NO));
							
							if(isCut)
								COLUMN_NO = SEL_COLUMN_NO;
						}
						else {
							leftHalfSubstr = m_Lines[0].substr(0, COLUMN_NO);
							rightHalfSubstr = m_Lines[0].substr(SEL_COLUMN_NO);
							middleString = m_Lines[0].substr(COLUMN_NO, (SEL_COLUMN_NO-COLUMN_NO));
						}
						
						if(isCut) {
							m_Lines[0] = "";
							m_Lines[0] += leftHalfSubstr;
							m_Lines[0] += rightHalfSubstr;
							m_bIsSelecting = false;
						}
						
						delete[] WWidgetManager::m_clipboardTextData;
						WWidgetManager::m_clipboardTextData = new char[middleString.size()+1];
						memset(WWidgetManager::m_clipboardTextData, 0, middleString.size()+1);
						sprintf(WWidgetManager::m_clipboardTextData, "%s", middleString.c_str());
						//SetClipboardData(CF_TEXT, WWidgetManager::m_clipboardTextData);
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
				if(OpenClipboard(GetForegroundWindow())) {
					if(m_bIsSelecting)
						deleteSelectedText();

					pasteText();
					
					setCaretDrawPosition();
					updateMains();
				}
			}
		}
		break;
		case VK_LEFT:
		{
			bool bWasSelecting = m_bIsSelecting;
			m_bIsSelecting = GetAsyncKeyState(VK_SHIFT);
			
			std::string str = m_Lines[0];
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
			
			if(COLUMN_NO < 0)
				COLUMN_NO = 0;
			
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
			
			std::string str = m_Lines[0];
			/* Move Caret */
			if(GetAsyncKeyState(VK_CONTROL)) {
				if(COLUMN_NO < str.size()-1)
					COLUMN_NO = WWidgetManager::getNextWhitespacePos(str.c_str(), COLUMN_NO, false) + 1;
				else
					COLUMN_NO = str.size()+1;
			}
			else {
				COLUMN_NO++;
			}
			
			if(COLUMN_NO > str.size())
				COLUMN_NO = prevColumnNo;
			
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

			setCaretDrawPosition();
			updateMains();

			return;
		}
		break;
		case VK_END:
		{	
			bool bCtrl = GetAsyncKeyState(VK_CONTROL);
			m_bIsSelecting = GetAsyncKeyState(VK_SHIFT);
			
			std::string str = m_Lines[0];
			COLUMN_NO = str.size();

			if(bCtrl) {
				std::string str = m_Lines[0];
				COLUMN_NO = str.size();
			}

			setCaretDrawPosition();
			updateMains();

			return;
		}
		break;
		case VK_BACK:
		{
			bool bWasSelecting = m_bIsSelecting;
			if(m_bIsSelecting)
				deleteSelectedText();
			else {
				if(COLUMN_NO > 0) {
					leftHalfSubstr = m_Lines[0].substr(0, COLUMN_NO-1);
					rightHalfSubstr = m_Lines[0].substr(COLUMN_NO);

					if(rightHalfSubstr.size() > 0) {
						m_Lines[0] = "";
						m_Lines[0] += leftHalfSubstr;
						m_Lines[0] += rightHalfSubstr;
					}
					else {
						m_Lines[0].assign(leftHalfSubstr);
					}

					COLUMN_NO--;
				}
			}
			
			setCaretDrawPosition();
			updateMains();
		}
		break;
		case VK_DELETE:
		{
			bool bWasSelecting = m_bIsSelecting;
			if(m_bIsSelecting)
				deleteSelectedText();
			else {
				if(COLUMN_NO < m_Lines[0].size()) {
					leftHalfSubstr = m_Lines[0].substr(0, COLUMN_NO);
					rightHalfSubstr = m_Lines[0].substr(COLUMN_NO+1);

					if(rightHalfSubstr.size() > 0) {
						m_Lines[0] = "";
						m_Lines[0] += leftHalfSubstr;
						m_Lines[0] += rightHalfSubstr;
					}
					else
						m_Lines[0].assign(leftHalfSubstr);
				}
			}
			
			setCaretDrawPosition();
			updateMains();
		}
		break;
	}

	if( ch >= 32 && ch <= 126 ) {
		bool bWasSelecting = m_bIsSelecting;
		if(m_bIsSelecting) {
			{
				if(COLUMN_NO > SEL_COLUMN_NO) {
					leftHalfSubstr = m_Lines[0].substr(0, SEL_COLUMN_NO);
					rightHalfSubstr = m_Lines[0].substr(COLUMN_NO);

					m_Lines[0] = "";
					m_Lines[0] += leftHalfSubstr;
					m_Lines[0] += rightHalfSubstr;
					COLUMN_NO = SEL_COLUMN_NO;
				}
				else {
					leftHalfSubstr = m_Lines[0].substr(0, COLUMN_NO);
					rightHalfSubstr = m_Lines[0].substr(SEL_COLUMN_NO);

					m_Lines[0] = "";
					m_Lines[0] += leftHalfSubstr;
					m_Lines[0] += rightHalfSubstr;
				}
				
				m_bIsSelecting = false;
			}
		}
		else {
			leftHalfSubstr = m_Lines[0].substr(0, COLUMN_NO);
			rightHalfSubstr = m_Lines[0].substr(COLUMN_NO);
		}

		if(rightHalfSubstr.size() > 0) {
			m_Lines[0] = "";
			m_Lines[0] += leftHalfSubstr;
			m_Lines[0] += ch;
			m_Lines[0] += rightHalfSubstr;
		}	
		else 
		if(leftHalfSubstr.size() > 0) {
			m_Lines[0] = "";
			m_Lines[0] += leftHalfSubstr;
			m_Lines[0] += ch;
		}
		else {
			m_Lines[0] = "";
			m_Lines[0] += ch;
		}
			
		COLUMN_NO++;
		
		setCaretDrawPosition();
		updateMains();
	}
	else
	if(ch == 13) {
	}

	if(!m_bIsSelecting) {
		SEL_COLUMN_NO = COLUMN_NO;
	}

	if(m_pParent)
		m_pParent->onMessage(KEY_DOWN, this->m_HwndID, (iVirtualKeycode<<32) | ch);

//printf("Ex onKeyBDown\n");
}

void WTextField::updateMains() {
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
		m_mainX = -m_CaretPosX;
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

//printf("Ex updateMains\n");
}

void WTextField::setCaretDrawPosition() {
//printf("In setCaretDrawPosition\n");
	int xX = 0;
	int yY = 0;
	std::string str = m_Lines[0];

	for(int i = 0; i < COLUMN_NO; i++) {

		char c = str[i];
		int CHAR_WIDTH = WWidgetManager::getCharWidth(c);

		xX += CHAR_WIDTH;
	}

	m_CaretPosX = xX;
	m_CaretPosY = yY;

//printf("Ex setCaretDrawPosition\n");
}

void WTextField::onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch) {

}

void WTextField::onMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
}

void WTextField::setReadOnly(bool bRd) {
	mState = bRd?READONLY:NORMAL;
}

bool WTextField::getReadOnly() {
	return (mState == READONLY);
}

void WTextField::selectText() {
	m_bIsSelecting = true;
	SEL_COLUMN_NO = 0;
	COLUMN_NO = m_Lines[0].size();
}

void WTextField::pasteText() {
	//char* pIsClipboardString = (char*)GetClipboardData(CF_TEXT);
	char* pIsClipboardString = WWidgetManager::m_clipboardTextData;
	//MessageBox(NULL, pIsClipboardString, "pIsClipboardString", MB_OK);

	std::string leftHalfSubstr("");
	std::string rightHalfSubstr("");

	leftHalfSubstr = m_Lines[0].substr(0, COLUMN_NO);
	rightHalfSubstr = m_Lines[0].substr(COLUMN_NO);

	m_Lines[0] = "";
	m_Lines[0] += leftHalfSubstr;

	//////////////////////////////////////////////////
	std::string sLine = "";
	int startPos = 0, endPos = 0;
	bool bFirstLine = true;
	std::string strBuff = pIsClipboardString;
	{
		endPos = strBuff.find_first_of("\r\n", startPos);
		if(endPos < 0)
			endPos = strBuff.size();

		sLine = strBuff.substr(startPos, endPos-startPos);

		if(bFirstLine) {
			m_Lines[0] += sLine;
			bFirstLine = false;
		}
		else {
			m_Lines.insert(m_Lines.begin(), sLine);
		}
	}
	//////////////////////////////////////////////////

	if(bFirstLine) {//Single-Line selection
		m_Lines[0] += pIsClipboardString;
		COLUMN_NO += strlen(pIsClipboardString);
	}
	else {//Multi-Line selection
		if(startPos < strBuff.size())
			m_Lines.insert(m_Lines.begin(), strBuff.substr(startPos, strBuff.size() - startPos));

		COLUMN_NO = strBuff.size() - startPos;
	}

	if(rightHalfSubstr.size() > 0)
		m_Lines[0] += rightHalfSubstr;
}

void WTextField::deleteSelectedText() {
	std::string leftHalfSubstr("");
	std::string rightHalfSubstr("");

	if(COLUMN_NO > SEL_COLUMN_NO) {
		leftHalfSubstr = m_Lines[0].substr(0, SEL_COLUMN_NO);
		rightHalfSubstr = m_Lines[0].substr(COLUMN_NO);
		COLUMN_NO = SEL_COLUMN_NO;
	}
	else {
		leftHalfSubstr = m_Lines[0].substr(0, COLUMN_NO);
		rightHalfSubstr = m_Lines[0].substr(SEL_COLUMN_NO);
	}

	m_bIsSelecting = false;
	m_Lines[0] = "";
	m_Lines[0] += leftHalfSubstr;
	m_Lines[0] += rightHalfSubstr;
}

LRESULT WTextField::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {

	case WM__GETTEXTLENGTH:
		{
			return strlen(m_Lines[0].c_str());
		}
		break;
	case WM__GETTEXT:
		{
			int iSize = (int)wParam;
			char* str = (char*)lParam;

			if(iSize < 0 || iSize > getLength() + 1)
				return -1;

			return sprintf_s(str, (size_t)iSize, "%s", m_Lines[0].c_str());
		}
		break;
	case WM__SETTEXT:
		{
			char* str = (char*)lParam;
			if(str == NULL)
				return -1;

			m_Lines[0] = str;
			return 1;
		}
		break;
	}

	return -1;
}

WTextField::~WTextField() {
	int i = 0;

	std::vector<std::string>::iterator itrBegin = m_Lines.begin();
	while(m_Lines.size() > 0) {
		m_Lines[i].~basic_string();
		m_Lines.erase(itrBegin + i);
		i++;
	}

	m_Lines.~vector();
	SAFE_DELETE(m_clipData);
}