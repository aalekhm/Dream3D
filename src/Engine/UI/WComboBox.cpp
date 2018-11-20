#ifdef USE_YAGUI
#include "Engine/UI/WComboBox.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/UI/WWidgetManager.h"
#include "Engine/UI/WScrollbar.h"
#include <algorithm>

using namespace std;

#define TB_LEFT_GUTTER		5
#define TB_TOP_GUTTER		5
#define TB_RIGHT_GUTTER		10
//#define LINE_HEIGHT			15
#define TB_CURSOR_TOP		2
#define TB_CURSOR_BOTTOM	12
#define TB_TEXT_Y				2
#define	 TB_BOTTOM_SHADOW_HEIGHT		3

WComboBox::WComboBox() 
	:	m_cbTextField(NULL),
		m_cbButton(NULL),
		m_cbListBox(NULL)
{

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

H_WND WComboBox::Create(		const char* lpClassName, 
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
	WComboBox* pWComboBox = new WComboBox();

	((WContainer*)pWComboBox)->Create(	lpClassName, 
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

	return pWComboBox;
}

void WComboBox::onCreateEx(LPVOID lpVoid) {

	H_WND hWnd = NULL;

	m_ComboBoxWidget = WWidgetManager::getWidget("ComboBox");
	TB_HEIGHT = WWidgetManager::CHARACTER_HEIGHT + (TB_TEXT_Y << 1);
	m_minX = getLeft() + TB_LEFT_GUTTER;
	m_minY = getTop() + TB_TOP_GUTTER;

	RectF wndRect;
	RectF idealRect;

	CHILD* cbTextField = m_ComboBoxWidget->getChild("TextField");
	RectF vDestRect;
	wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
	idealRect.X = cbTextField->posOffsets.x;
	idealRect.Y = cbTextField->posOffsets.y;
	idealRect.Width = cbTextField->posOffsets.w; 
	idealRect.Height = cbTextField->posOffsets.h;
	WWidgetManager::getDestinationRect(	vDestRect,
															m_ComboBoxWidget->widgetSize.width, 
															m_ComboBoxWidget->widgetSize.height,
															&wndRect,
															&idealRect,
															cbTextField->align.eHAlign,
															cbTextField->align.eVAlign
															);
	hWnd = 
	CreateComponent(	"WTextField", 
								"Sample text", 
								0, 
								cbTextField->posOffsets.x,
								cbTextField->posOffsets.y, 
								vDestRect.Width, 
								vDestRect.Height,
								this, 
								HMENU(ID_TEXTFIELD), 
								NULL);
	m_cbTextField = ((WTextField*)hWnd);
	m_cbTextField->setReadOnly(!true);
	m_cbTextField->setBorderVisibility(false);

	CHILD* cbButton = m_ComboBoxWidget->getChild("ButtonDown");
	wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
	idealRect.X = cbButton->posOffsets.x;
	idealRect.Y = (WTextField::LINE_HEIGHT >> 1) - (cbButton->posOffsets.h  / 2);//Button centered aligned vertically.
	idealRect.Width = cbButton->posOffsets.w; 
	idealRect.Height = cbButton->posOffsets.h;
	WWidgetManager::getDestinationRect(	vDestRect,
															m_ComboBoxWidget->widgetSize.width, m_ComboBoxWidget->widgetSize.height,
															&wndRect,
															&idealRect,
															cbButton->align.eHAlign,
															cbButton->align.eVAlign
														);
	hWnd = 
	CreateComponent(	"WButton", 
								"", 
								0, 
								vDestRect.X - m_iLeft,
								vDestRect.Y - m_iTop, 
								vDestRect.Width, 
								vDestRect.Height,
								this, 
								HMENU(ID_BUTTON), 
								"ButtonDown");
	m_cbButton = (WButton*)hWnd;

	CHILD* cbListBox = m_ComboBoxWidget->getChild("ListBox");
	wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
	idealRect.X = cbListBox->posOffsets.x;
	idealRect.Y = WTextField::LINE_HEIGHT - TB_BOTTOM_SHADOW_HEIGHT;//ListBox is aligned at the bottom of the TextField.
	idealRect.Width = cbListBox->posOffsets.w; 
	idealRect.Height = cbListBox->posOffsets.h;
	WWidgetManager::getDestinationRect(	vDestRect,
															m_ComboBoxWidget->widgetSize.width, m_ComboBoxWidget->widgetSize.height,
															&wndRect,
															&idealRect,
															cbListBox->align.eHAlign,
															cbListBox->align.eVAlign
															);
	hWnd = 
	CreateComponent(	"WListBox", 
								"", 
								0, 
								vDestRect.X - m_iLeft, 
								vDestRect.Y - m_iTop,
								vDestRect.Width, 
								(getHeight() - TB_HEIGHT),
								this, 
								HMENU(ID_LISTBOX), 
								(LPVOID)true);
	m_cbListBox = (WListBox*)hWnd;
	m_cbListBox->setVisible(false);
}

void WComboBox::onUpdate() {

	if(!isActive()) {
		if(m_cbListBox->isVisible())
			m_cbListBox->setVisible(false);
	}

	if(m_cbListBox->isVisible())
		setHeight(TB_HEIGHT + m_cbListBox->getHeight());
	else
		setHeight(TB_HEIGHT);
}

void WComboBox::onRender() {
	WWidgetManager* renderer =  WWidgetManager::getInstance();

	RectF thisWndRect(getLeft(), getTop(), getWidth(), TB_HEIGHT);

	CHILD* child = m_ComboBoxWidget->getChild("TextArea");
	renderer->renderChild(m_ComboBoxWidget, child, &thisWndRect);
	//renderer->renderClientArea(m_ComboBoxWidget, 0, thisWndRect);
}

void WComboBox::onMouseWheelEx(WPARAM wParam, LPARAM lParam) {
	if(m_cbListBox->isVisible())
		m_cbListBox->onMouseWheel(wParam, lParam);
}

void WComboBox::onKeyBDownEx(unsigned int iVirtualKeycode, unsigned short ch) {
	if((getSize() <= 0))
		return;

	if(	iVirtualKeycode == VK_RETURN
		||
		iVirtualKeycode == VK_ESCAPE
	){
		m_cbListBox->setVisible(false);
		setSelectedIndex(m_cbListBox->getSelectedIndex());
		WWidgetManager::onEvent(this, WM_CBN_SELENDOK, getComponentID(), 0);
		return;
	}
	
	if(m_cbListBox->isVisible()) {
		if(iVirtualKeycode == VK_UP || iVirtualKeycode == VK_DOWN || iVirtualKeycode == VK_NEXT || iVirtualKeycode == VK_PRIOR) {
			m_cbListBox->onKeyBDown(iVirtualKeycode, ch);

			m_CurrentIndex = m_cbListBox->getSelectedIndex();
			m_cbTextField->setText(m_cbListBox->getSelectedString());
			
			m_cbTextField->releaseFocus();
			m_cbTextField->selectText();
		}
		else {
			m_cbTextField->getFocus();
			m_cbTextField->onKeyBDown(iVirtualKeycode, ch);
		}
	}
	else {
		if(!m_cbListBox->isVisible())
			m_cbListBox->setVisible(true);

		m_cbTextField->getFocus();
		m_cbTextField->onKeyBDown(iVirtualKeycode, ch);
	}
}

void WComboBox::onKeyBUpEx(unsigned int iVirtualKeycode, unsigned short ch) {
	if((getSize() <= 0))
		return;

	if(m_cbListBox->isVisible())
		m_cbListBox->onKeyBUp(iVirtualKeycode, ch);
}

void WComboBox::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case MOUSE_DOWN:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_BUTTON:
				{
					bool isListBoxVisible = m_cbListBox->isVisible();
					m_cbListBox->setVisible(!isListBoxVisible);

					if(!isListBoxVisible) {
						m_cbListBox->setSelectedIndex(m_CurrentIndex);
					}
				}
				break;
				case ID_LISTBOX:
				{
				}
				break;
			}
		}
		break;
		case MOUSE_UP:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_LISTBOX:
				{
					m_cbListBox->setVisible(!m_cbListBox->isVisible());

					m_CurrentIndex = m_cbListBox->getSelectedIndex();
					m_cbTextField->setText(m_cbListBox->getSelectedString());
					m_cbTextField->selectText();

					WWidgetManager::onEvent(this, WM_CBN_SELENDOK, getComponentID(), 0);
				}
				break;
			}
		}
			break;
		case MOUSE_MOVE:
		{
			int buttonID = wParam;
			switch(wParam) {
			}
		}
		break;
		case KEY_DOWN:
		{
			int buttonID = wParam;
			unsigned int iVirtualKeycode = (lParam>>32)&0xffffffff;
			unsigned int ch = (lParam&0xffffffff);
			switch(wParam) {
				case ID_TEXTFIELD:
				{
					int idToShow = findString(m_cbTextField->getText());
					if(idToShow < 0)
						idToShow = 0;

					m_cbListBox->setSelectedIndex(idToShow);
				}
				break;
			}
		}
		break;
	}
}

void WComboBox::setReadOnly(bool bRd) {
	mState = bRd?READONLY:NORMAL;
}

bool WComboBox::getReadOnly() {
	return (mState == READONLY);
}

void WComboBox::addItem(LISTBOX_ITEM* item) {
	m_cbListBox->addItem(item);
}

void WComboBox::insertItemAt(int pos, LISTBOX_ITEM* item) {
	m_cbListBox->insertItemAt(pos, item);
}

void WComboBox::removeItemAt(int pos) {
	m_cbListBox->removeItemAt(pos);
}

int WComboBox::getSelectedIndex() {
	return m_cbListBox->getSelectedIndex();
}

void WComboBox::setSelectedIndex(int pos) {
	m_cbListBox->setSelectedIndex(pos);

	m_CurrentIndex = m_cbListBox->getSelectedIndex();
	m_cbTextField->setText(m_cbListBox->getSelectedString());
	m_cbTextField->selectText();
	m_cbTextField->releaseFocus();
}

const char*	WComboBox::getSelectedString() {
	return m_cbListBox->getSelectedString();
}

const char*	WComboBox::getStringAt(int pos) {
	return m_cbListBox->getStringAt(pos);
}

int WComboBox::getSize() {
	return m_cbListBox->getSize();
}

void WComboBox::addItem(const char* sItem) {
	
	if(sItem != NULL) {
		LISTBOX_ITEM* item = new LISTBOX_ITEM();
		item->itemLabel = sItem;

		m_cbListBox->addItem(item);
	}
}

int WComboBox::findString(const char* strToFind) {
	
	std::string str = strToFind;
	int totalItems = getSize();
	int idToShow = -1;

	while(1) {
		for(int i = 0; i < totalItems; i++) {
			if(CCString::startsWith(getStringAt(i), str.c_str())) {
				idToShow = i;
				break;
			}
		}

		if(idToShow >= 0 || str.size() <= 0)
			break;
		else {
			str = str.substr(0, str.size()-1);
		}
	}

	return idToShow;
}

int WComboBox::findStringExact(const char* strToFind) {

	std::string str = strToFind;
	int totalItems = getSize();
	int idToShow = -1;

	CCString str1 = str.c_str();
	for(int i = 0; i < totalItems; i++) {
		CCString str2 = getStringAt(i);
		if(strcmp(str1.toLower(), str2.toLower()) == 0) {
			idToShow = i;
			break;
		}
	}

	return idToShow;
}

void WComboBox::addDefaultTestItems() {

	char* ss = new char[255];
	memset(ss, 0, 255);

	LISTBOX_ITEM* item;
	for(int i = 0; i < 5; i++) {
		sprintf(ss, "%c item %c", (65+i), (65+i));
		addItem(ss);
	}

	sprintf(ss, "c:\_projects\myyagui_20_7_combobox_memorymanaged. warning C4715: 'WListBox::getSize' : not all control paths return a value.");
	addItem(ss);
	sprintf(ss, "Finished generating code");
	addItem(ss);
	sprintf(ss, "Embedding manifest...");
	addItem(ss);
	sprintf(ss, "Generating code");
	addItem(ss);
	sprintf(ss, "MyYAGui - 0 error(s), 51 warning(s)");
	addItem(ss);
	sprintf(ss, "Linking...");
	addItem(ss);
	sprintf(ss, "Compiling...");
	addItem(ss);
	sprintf(ss, "Comqiling...");
	addItem(ss);
	sprintf(ss, "WComboBox.cpp");
	addItem(ss);

	setSelectedIndex(10);

	SAFE_DELETE_ARRAY(ss);
}

const char* WComboBox::getText() {
	return m_cbTextField->getText();
}

int WComboBox::getStringLengthAtPos(int iIndex) {
	if(iIndex < 0 || iIndex > m_cbListBox->getSize() - 1)
		return CB__ERR;

	char* str = (char*)getStringAt(iIndex);

	return strlen(str);
}

LRESULT WComboBox::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case WM__GETTEXT:
			{
				return m_cbTextField->OnSendMessage(WM__GETTEXT, wParam, lParam);
			}
			break;
		case WM__GETTEXTLENGTH:
			{
				return m_cbTextField->OnSendMessage(WM__GETTEXTLENGTH, wParam, lParam);
			}
			break;
		case WM__SETTEXT:
			{
				int iValue = m_cbTextField->OnSendMessage(WM__SETTEXT, wParam, lParam);
				m_cbTextField->selectText();

				return iValue;
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
		case CBM__ADDSTRING:
			{
				char* str = (char*)lParam;
				if(str == NULL)
					return CB_ERR;

				addItem(str);

				//Since out ComboBox doesn't support sorting(as of now)...
				//The return value is the zero-based index to the string in the list box of the combo box
				return m_cbListBox->getSize() - 1;
			}
			break;
		case CBM__DELETESTRING:
			{
				int iPos = (int)wParam;
				if(iPos >= 0 && iPos < m_cbListBox->getSize())
					removeItemAt(iPos);

				//The return value is a count of the strings remaining in the list.
				return m_cbListBox->getSize() - 1;
			}
			break;
		case CBM__FINDSTRING:
			{
				char* str = (char*)lParam;
				if(str == NULL)
					return CB_ERR;

				int iIndex = findString(str);
				return iIndex;
			}
			break;
		case CBM__FINDSTRINGEXACT:
			{
				char* str = (char*)lParam;
				if(str == NULL)
					return CB_ERR;

				int iIndex = findStringExact(str);
				return iIndex;
			}
			break;
		case CBM__GETCOUNT:
			{
				return m_cbListBox->getSize();
			}
			break;
		case CBM__GETCURSEL:
			{
				return getSelectedIndex();
			}
			break;
		case CBM__SETCURSEL:
			{
				int iIndex = (int)wParam;
				if(iIndex < 0 || iIndex > m_cbListBox->getSize() - 1) {
					m_cbTextField->setText("");
					return CB__ERR;
				}

				setSelectedIndex(iIndex);
				return iIndex;
			}
			break;
		case CB__GETDROPPEDCONTROLRECT:
			{
				Rect* listRect = (Rect*)lParam;
				listRect->X = m_cbListBox->getLeft();
				listRect->Y = m_cbListBox->getTop();
				listRect->Width = m_cbListBox->getWidth();
				listRect->Height = m_cbListBox->getHeight();

				return CB_OKAY;
			}
		case CB__GETDROPPEDSTATE:
			{
				return m_cbListBox->isVisible()?1:0;
			}
			break;
		case CB__GETDROPPEDWIDTH:
			{
				int iWidth = getWidth();
				if(iWidth > 0)
					return iWidth;
				return CB_ERR;
			}
			break;
		case CB__GETEDITSEL:
			{	
				return m_cbTextField->getSelection();
			}
			break;
		case CB__SETEDITSEL:
			{	
				m_cbTextField->setSelection((DWORD)lParam);
				return CB_OKAY;
			}
			break;
		case CB__GETLBTEXTLEN:
			{
				int iPos = (int)wParam;
				if(iPos < 0 || iPos > getSize() - 1)
					return CB_ERR;

				int iLength = getStringLengthAtPos((int)wParam);
				return iLength;
			}
			break;
		case CB__GETLBTEXT:
			{
				int iIndex = (int)wParam;
				char* str = (char*)lParam;
				if(iIndex < 0 || iIndex > m_cbListBox->getSize() - 1)
					return CB_ERR;

				memcpy(str, getStringAt(iIndex), getStringLengthAtPos(iIndex));
				return CB_OKAY;
			}
			break;
		case CB__INSERTSTRING:
			{
				int iIndex = (int)wParam;
				char* str = (char*)lParam;
				
				if(str == NULL)
					return CB__ERR;

				if(iIndex < 0)
					OnSendMessage(CBM__ADDSTRING, wParam, lParam);

				LISTBOX_ITEM* item = new LISTBOX_ITEM();
				item->itemLabel = str;
				insertItemAt(iIndex, item);

				return CB_OKAY;
			}
			break;
		case CB__LIMITTEXT:
			{

			}
			break;
		case CB__RESETCONTENT:
			{
				m_cbListBox->clearList();
				m_cbTextField->setText("");
				return CB_OKAY;
			}
			break;
		case CB__SELECTSTRING:
			{
				char* str = (char*)lParam;
				if(str == NULL)
					return CB_ERR;

				int iIndex = findString(str);

				if(iIndex < 0)
					return CB_ERR;

				setSelectedIndex(iIndex);

				return iIndex;
			}
			break;
		case CB__SETITEMDATA:
			{
				int iIndex = (int)wParam;
				char* str = (char*)lParam;

				if(iIndex < 0 || iIndex > getSize() - 1 || str == NULL)
					return CB_ERR;

				return m_cbListBox->setStringAt(iIndex, str);
			}
			break;
		case CB__SHOWDROPDOWN:
			{
				bool bShow = (bool)wParam;
				m_cbListBox->setVisible(bShow);

				return CB_OKAY;
			}
			break;
	}

	return CB__ERR;
}

WComboBox::~WComboBox() {
}

/*
 * Combo Box messages
 */
//#ifndef NOWINMESSAGES
//#define CB__DIR                      0x0145
//#define CB__GETITEMDATA              0x0150
//#define CB__SETITEMHEIGHT            0x0153
//#define CB__GETITEMHEIGHT            0x0154
//#define CB__SETEXTENDEDUI            0x0155
//#define CB__GETEXTENDEDUI            0x0156
//#define CB__SETLOCALE                0x0159
//#define CB__GETLOCALE                0x015A
//#if(WINVER >= 0x0400)
//#define CB__GETTOPINDEX              0x015b
//#define CB__SETTOPINDEX              0x015c
//#define CB__GETHORIZONTALEXTENT      0x015d
//#define CB__SETHORIZONTALEXTENT      0x015e
//#define CB__GETDROPPEDWIDTH          0x015f
//#define CB__SETDROPPEDWIDTH          0x0160
//#define CB__INITSTORAGE              0x0161
//#if(_WIN32_WCE >= 0x0400)
//#define CB__MULTIPLEADDSTRING        0x0163
//#endif
//#endif /* WINVER >= 0x0400 */
//
//#if(_WIN32_WINNT >= 0x0501)
//#define CB__GETCOMBOBOXINFO          0x0164
//#endif /* _WIN32_WINNT >= 0x0501 */
//
//#if(_WIN32_WINNT >= 0x0501)
//#define CB__MSGMAX                   0x0165
//#elif(_WIN32_WCE >= 0x0400)
//#define CB__MSGMAX                   0x0163
//#elif(WINVER >= 0x0400)
//#define CB__MSGMAX                   0x0162
//#else
//#define CB__MSGMAX                   0x015B
//#endif
//#endif  /* !NOWINMESSAGES */

#endif