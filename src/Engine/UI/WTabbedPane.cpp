#ifdef USE_YAGUI
#include "Engine/UI/WTabbedPane.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/UI/WWidgetManager.h"

#include "Engine/UI/WScrollbar.h"
#include "Engine/UI/WTextBox.h"
#include "Engine/UI/WListBox.h"
#include "Engine/UI/WComboBox.h"
#include "Engine/UI/WFrame.h"

WTabbedPane::WTabbedPane()
	:	m_pParent(NULL),
		m_SelectedTab(-1),
		m_TabCount(0),
		m_StartTabId(-1),
		m_MaxTabsOnScreen(0)
{
	TAB_HEIGHT = WWidgetManager::CHARACTER_HEIGHT + (TAB_TITLE_START_Y << 1);
}

WTabbedPane::~WTabbedPane() {

}

H_WND WTabbedPane::Create(		const char* lpClassName, 
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
	WTabbedPane* pWTabbedPane = new WTabbedPane();

	((WContainer*)pWTabbedPane)->Create(	lpClassName, 
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

	return pWTabbedPane;
}

void WTabbedPane::onCreateEx(LPVOID lpVoid) {
	
	H_WND hWnd = NULL;

	///////////////////////////////////////////////////
	RectF hDestRect;
	RectF wndRect;
	RectF idealRect;

	TAB_HEIGHT = WWidgetManager::CHARACTER_HEIGHT + (TAB_TITLE_START_Y << 1);
	m_TabbedWidget = WWidgetManager::getWidget("Tab");

	///////////////////////////////////////////////////
	CHILD* btnScrollLeft = m_TabbedWidget->getChild("ButtonLeft");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = btnScrollLeft->posOffsets.x;
		idealRect.Y = (TAB_HEIGHT - btnScrollLeft->posOffsets.h) / 2;
		idealRect.Width = btnScrollLeft->posOffsets.w; 
		idealRect.Height = btnScrollLeft->posOffsets.h;
	WWidgetManager::getDestinationRect(	hDestRect,
											m_TabbedWidget->widgetSize.width,
											m_TabbedWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											btnScrollLeft->align.eHAlign,
											btnScrollLeft->align.eVAlign
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
								HMENU(ID_BUTTON_SCROLL_TAB_LEFT), 
								"ButtonLeft");
	m_ButtonScrollLeft = (WButton*)hWnd;
	m_ButtonScrollLeft->setPostRender(true);
	///////////////////////////////////////////////////

	///////////////////////////////////////////////////
	CHILD* btnScrollRight = m_TabbedWidget->getChild("ButtonRight");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = btnScrollRight->posOffsets.x;
		idealRect.Y = (TAB_HEIGHT - btnScrollRight->posOffsets.h) / 2;
		idealRect.Width = btnScrollRight->posOffsets.w; 
		idealRect.Height = btnScrollRight->posOffsets.h;
	WWidgetManager::getDestinationRect(	hDestRect,
											m_TabbedWidget->widgetSize.width,
											m_TabbedWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											btnScrollRight->align.eHAlign,
											btnScrollRight->align.eVAlign
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
								HMENU(ID_BUTTON_SCROLL_TAB_RIGHT), 
								"ButtonRight");
	m_ButtonScrollRight = (WButton*)hWnd;
	m_ButtonScrollRight->setPostRender(true);
	///////////////////////////////////////////////////

	bool bHasClientArea = (m_TabbedWidget->clientAreas.size() > 0);
	if(bHasClientArea) {
		CLIENT_AREA* clientArea = m_TabbedWidget->getClientAreaAt(0);
		RectF destRect;
			wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
			idealRect.X = clientArea->posOffsets.x;
			idealRect.Y = TAB_HEIGHT;
			idealRect.Width = clientArea->posOffsets.w;
			idealRect.Height = clientArea->posOffsets.h - TAB_HEIGHT;
		WWidgetManager::getDestinationRect(	destRect,
												m_TabbedWidget->widgetSize.width,
												m_TabbedWidget->widgetSize.height,
												&wndRect,
												&idealRect,
												clientArea->align.eHAlign,
												clientArea->align.eVAlign
												);
		m_ClientRect.X = destRect.X - getLeft();
		m_ClientRect.Y = destRect.Y - getTop();
		m_ClientRect.Width = destRect.Width;
		m_ClientRect.Height = destRect.Height;
	}
	///////////////////////////////////////////

	m_minX = getLeft() + TAB_START_X;
	m_MaxTabsOnScreen = m_ButtonScrollLeft->getOffsetX() / TAB_WIDTH;
	
	char* sTitle = new char[255];
	memset(sTitle, 0, 255);
	for(int i = 0; i < 6; i++) {
		sprintf(sTitle, "W-%d\0", i);
		std::string sTitle(sTitle);
		addTab1(sTitle);
	}

	setSelectedTab(0);
}

void WTabbedPane::onUpdate() {

}

void WTabbedPane::onRender() {
	WWidgetManager* renderer =  WWidgetManager::getInstance();

	RectF tabWndRect(getLeft()+m_ClientRect.X, getTop() + m_ClientRect.Y - 21, m_ClientRect.Width, m_ClientRect.Height + 26);
	renderer->renderWidget("Tab_Body", &tabWndRect);
	
	CHILD* cTabSelected = m_TabbedWidget->getChild("TabPatchSelected");
	CHILD* cTabUnSelected = m_TabbedWidget->getChild("TabPatchUnSelected");

	int count = 0;
	RectF rTab(getLeft(), getTop(), TAB_WIDTH, TAB_HEIGHT);
	for(int i = m_StartTabId; i < m_TabCount; i++, count++) {
		if(count >= m_MaxTabsOnScreen)
			break;

		rTab.X = getLeft() + count*TAB_WIDTH;
		if(i == m_SelectedTab) {
			renderer->renderWidget("TabPatchSelected", &rTab);
		}
		else {
			renderer->renderWidget("TabPatchUnSelected", &rTab);
		}

		renderer->drawStringFont((char*)((WFrame*)m_vTabFrames[i])->getWindowName(), getLeft() + count*TAB_WIDTH + TAB_TITLE_START_X, getTop() + TAB_TITLE_START_Y, 0);
	}
}

void WTabbedPane::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case MOUSE_DOWN:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_BUTTON_SCROLL_TAB_LEFT:
				{
					m_StartTabId--;
					if(m_StartTabId < 0)
						m_StartTabId = 0;
				}
				break;
				case ID_BUTTON_SCROLL_TAB_RIGHT:
				{
					if(m_TabCount > m_MaxTabsOnScreen) {
						m_StartTabId++;
						if(m_StartTabId + m_MaxTabsOnScreen > m_TabCount)
							m_StartTabId = m_TabCount - m_MaxTabsOnScreen;
					}
				}
				break;
			}
		}
		break;
	}
}

void WTabbedPane::onMouseDownEx(int x, int y, int iButton) {
	
	if(	x < m_minX 
		|| 
		x > (m_ButtonScrollLeft->getLeft())
		||
		x > (m_minX + m_MaxTabsOnScreen * TAB_WIDTH)
		||
		y > (getTop() + TAB_HEIGHT)
	) {
			return;
	}

	int tabID = (x - m_minX) / TAB_WIDTH;
	setSelectedTab(m_StartTabId + tabID);
}

H_WND WTabbedPane::addTab(const std::string& sTitle) {

	H_WND hWnd = NULL;

	WFrame* wTab = NULL;
	hWnd = 
	CreateComponent(	"WFrame", 
								sTitle.c_str(), 
								0, 
								m_ClientRect.X,
								m_ClientRect.Y, 
								m_ClientRect.Width, 
								m_ClientRect.Height,
								this, 
								HMENU(1112), 
								(LPVOID)true);
	wTab = (WFrame*)hWnd;
	wTab->setComponentAsChild(false);
	wTab->setMovable(false);
	wTab->setBorderVisibility(false);

	m_vTabFrames.push_back(wTab);

	m_TabCount = m_vTabFrames.size();
	m_StartTabId = m_SelectedTab - m_MaxTabsOnScreen;
	if(m_StartTabId < 0)
		m_StartTabId = 0;
	setSelectedTab(m_TabCount - 1);

	return (H_WND)wTab;
}

WFrame* WTabbedPane::addTab1(const std::string& sTitle) {
	
	H_WND hWnd = NULL;

	WFrame* wTab = new WFrame();
	hWnd = 
	CreateComponent(	"WFrame", 
								sTitle.c_str(), 
								0, 
								m_ClientRect.X,
								m_ClientRect.Y, 
								m_ClientRect.Width, 
								m_ClientRect.Height,
								this, 
								HMENU(1112), 
								(LPVOID)true);
	wTab = (WFrame*)hWnd;
	wTab->setComponentAsChild(false);
	wTab->setMovable(false);
	wTab->setBorderVisibility(false);

	m_vTabFrames.push_back(wTab);

	m_TabCount = m_vTabFrames.size();
	m_StartTabId = m_SelectedTab - m_MaxTabsOnScreen;
	if(m_StartTabId < 0)
		m_StartTabId = 0;
	setSelectedTab(m_TabCount - 1);

	///////////////////////////////// TO REMOVE LATER
	{
		char* sText = "New Delhi, July 26 (IANS) Hours after saying he did not wish to implicate Prime Minister Manmohan Singh or anyone else in the 2G spectrum allotment case, former telecom minister A. Raja Tuesday asked why the matter had not been referred to a ministerial panel and also wanted Home Minister P. Chidambaram to take the witness stand.\n\
					  Main kisi ko phasana nahi chahta tha (I had no intention of framing anybody),' Raja's lawyer Sushil Kumar said on his behalf when the names of Manmohan Singh and Chidambaram cropped up in a special Central Bureau of Investigation (CBI) court.\n\
					  I am just defending myself -- not accusing anything or anybody,' he said, a day after stroking a political storm by dragging the prime minister into the controversy. 'They (the media) cannot put words into my mouth. Ask them to report truthfully, or go out of this court,' he added.\n\
					  But the home minister must come in the court from either of the sides and be a witness in the case. When all decisions were known to the home minister, he should appear as a witness in the case,' Kumar told the special court presided over by Judge O.P. Saini.\n\
					  Just a few hours later, after recess, he stepped up his attack on the prime minister and wondered why a group of ministers (GoM) was not set up if any inconsistency was found on the way the spectrum allocation matter was handled.\n\
					  A lawyer by training, Raja himself took over from his counsel at one point.\n\
					  The prime minister is superior to me. He could have constituted a GoM. But he ignored a GoM. Is this a conspiracy?' Raja's counsel asked, wanting the then solicitor general and now attorney general Goolam. E. Vahanvati, too, in the witness box, while terming the judicial custody of his client since Feb 2 illegal.\n\
					  The counsel was continuing with the arguments the previous day that as finance minister in 2008 Chidambaram had taken the decision to permit the promoters of two telecom firms to sell stakes with full knowledge of the prime minister.\n\
					  While this was not denied subsequently by Chidambaram or present Communications Minister Kapil Sibal, both sought to say that the equity sale was by way of issuing fresh shares and not divestment by promoters, permitted under the policy that existed then.\n\
					  The Congress even launched a counter-attack Tuesday and said Raja had also dragged former prime minister Atal Bihari Vajpayee's name in the case and that the government of the Bharatiya Janata Party (BJP)-led coalition at that time was equally culpable.\n\
					  If the BJP decides to make a song and dance about one part of Raja's statement, then the other part of his statement squarely indicts Atal Bihari Vajpayee also,' Congress spokesperson Manish Tewari said.\n\
					  The official probe agency has said that Raja's decision as telecom minister in 2008 to issue radio spectrum to companies at a mere Rs.1,659 crore ($350 million) for a pan-India operation had caused the exchequer losses worth thousands of crores of rupees.\n\
					  Nine new telecom companies were issued the radio frequency airwaves, a scarce national resource, to operate second generation (2G) mobile phone services in the country. As many as 122 circle-wise licences were issued.\n\
					  The probe agency questioned the manner in which allocations were made that even resulted in a windfall for some.\n\
					  A new player Swan Telecom had bought licences for 13 circles with the necessary spectrum for $340 million but managed to sell a 45 percent stake in the company to UAE's Etisalat for $900 million. This swelled its valuation to $2 billion without a single subscriber.\n\
					  Similarly, another new player, Unitech, paid $365 million as licence fee but sold a 60 percent stake to Norway's Telenor for $1.36 billion, taking its valuation to nearly $2 billion, again without a single subscriber.\n\
					  \n\
					  The MBR can only represent four partitions. A technique called \"extended\" partitioning is used to allow more than four, and often times it is used when there are more than two partitions. All we're going to say about extended partitions is that they appear in this table just like a normal partition, and their first sector has another partition table that describes the partitions within its space. But for the sake of simply getting some code to work, we're going to not worry about extended partitions (and repartition and reformat any drive that has them....) The most common scenario is only one partition using the whole drive, with partitions 2, 3 and 4 blank.";	

		if(m_TabCount == 1) {
			hWnd = 
			CreateComponent(	"WTextBox", 
												sText, 
												0, 
												5,
												5, 
												340, 
												163,
												wTab, 
												HMENU(199), 
												NULL);
			((WTextBox*)hWnd)->showLineNumbers(true);

			hWnd = 
			CreateComponent(	"WComboBox", 
										"", 
										0, 
										5,
										((WTextBox*)hWnd)->getOffsetY() + ((WTextBox*)hWnd)->getHeight() + 5, 
										240, 
										100,
										wTab, 
										HMENU(9998), 
										NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

			hWnd = 
			CreateComponent(	"WTextBox", 
										sText, 
										0, 
										5,
										((WComboBox*)hWnd)->getOffsetY() + ((WComboBox*)hWnd)->getHeight() + 5, 
										240, 
										163,
										wTab, 
										HMENU(199), 
										NULL);
			((WTextBox*)hWnd)->showLineNumbers(true);
		}
		else
		if(m_TabCount == 2) {
			hWnd = 
			CreateComponent(	"WComboBox", 
										"", 
										0, 
										5,
										5, 
										220, 
										100,
										wTab, 
										HMENU(9999), 
										NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();
		}
	}
	///////////////////////////////// TO REMOVE LATER

	return wTab;
}

int WTabbedPane::deleteTab(int iIndex) {
	
	if(iIndex >= 0 && iIndex < m_TabCount) {

		///////////////////////////
		std::vector<WFrame*>::iterator itr = m_vTabFrames.begin();
		WFrame* pTab = m_vTabFrames[iIndex];

		removeComponent(pTab);
		m_vTabFrames.erase(itr + iIndex);
		SAFE_DELETE(pTab);

		m_TabCount = m_vTabFrames.size();

		int iReturnValue = setSelectedTab( (iIndex >= m_vTabFrames.size()) ? iIndex-1 : iIndex);
		if(iReturnValue >= 0)
			m_StartTabId = iReturnValue;
		///////////////////////////

		return iIndex;
	}

	return WM__ERR;
}

H_WND WTabbedPane::setTab(int iIndex, const std::string& sTitle) {

	if(iIndex < 0 || iIndex > (m_TabCount - 1))
		return NULL;

	if(deleteTab(iIndex) >= 0) {
		return insertTab(iIndex, sTitle);
	}
	else
		return NULL;
}

H_WND WTabbedPane::insertTab(int iIndex, const std::string& sTitle) {
	if(iIndex < 0 || iIndex > (m_TabCount - 1))
		return NULL;

	H_WND hWnd = NULL;
	WFrame* wTab = NULL;
	hWnd = 
	CreateComponent(	"WFrame", 
								sTitle.c_str(), 
								0, 
								m_ClientRect.X,
								m_ClientRect.Y, 
								m_ClientRect.Width, 
								m_ClientRect.Height,
								this, 
								HMENU(1112), 
								(LPVOID)true);
	wTab = (WFrame*)hWnd;
	wTab->setComponentAsChild(false);
	wTab->setMovable(false);
	wTab->setBorderVisibility(false);

	std::vector<WFrame*>::iterator itrBegin = m_vTabFrames.begin();
	m_vTabFrames.insert(itrBegin + iIndex, wTab);

	m_TabCount = m_vTabFrames.size();
	m_StartTabId = m_SelectedTab - m_MaxTabsOnScreen;
	if(m_StartTabId < 0)
		m_StartTabId = 0;
	setSelectedTab(iIndex);

	return (H_WND)wTab;
}

WFrame* WTabbedPane::getTab(const char* sTitle) {

	WFrame* pTab = NULL;
	if(sTitle != NULL && strlen(sTitle) > 0) {
		CCString cTitle = sTitle;
		for(int i = 0; i < m_TabCount; i++) {
			pTab = (WFrame*)m_vTabFrames[i];
			CCString cTabTitle = pTab->getWindowName();
			if (strcmp(cTitle.toLower(), cTabTitle.toLower()) == 0) {
				return pTab;
			}
		}
	}

	return pTab;
}

WFrame* WTabbedPane::getTab(int iIndex) {

	WFrame* pTab = NULL;
	if(iIndex >= 0 && iIndex < m_TabCount) {
		pTab = m_vTabFrames[iIndex];
	}

	return pTab;
}

int WTabbedPane::getSelectedTab() {
	return m_SelectedTab;
}

int WTabbedPane::setSelectedTab(int iSel) {

	if(m_TabCount > 0 && ( iSel >= 0 && iSel < m_TabCount)) {
		m_SelectedTab = iSel;

		WFrame* wTab = 0;
		for(int i = 0; i < m_TabCount; i++) {
			wTab = (WFrame*)m_vTabFrames[i];
			wTab->setVisible(false);
		}

		wTab = (WFrame*)m_vTabFrames[iSel];
		wTab->setVisible(true);

		return m_SelectedTab;
	}

	return -1;
}

LRESULT WTabbedPane::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case WM__GETTEXT:
			{
				int iIndex = (int)wParam;
				TCIM_* tcim = (TCIM_*)lParam;
				if(iIndex>= 0 && iIndex < m_TabCount) {

					WFrame* pTab = getTab(iIndex);

					tcim->lpzText = (LPSTR)pTab->getWindowName();
					tcim->cchTextMax = strlen(tcim->lpzText);
					tcim->lParam = (LPARAM)NULL;
					
					return WM__OKAY;
				}
				else
					return WM__ERR;
			}
			break;
		case WM__GETTEXTLENGTH:
			{
				//return m_cbTextField->OnSendMessage(WM__GETTEXTLENGTH, wParam, lParam);
				return WM__OKAY;
			}
			break;
		case WM__SETTEXT:
			{
				int iValue = OnSendMessage(TAB__SETITEMTITLE, wParam, lParam);
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
		case TAB__GETCURSEL:
			{
				return getSelectedTab();
			}
			break;
		case TAB__SETCURSEL:
			{
				int iTabToSelect = (int)wParam;
				int iReturnValue = setSelectedTab(iTabToSelect);
				if(iReturnValue >= 0)
					m_StartTabId = iReturnValue;

				return iReturnValue;
			}
			break;
		case TAB__GETITEMCOUNT:
			{
				return m_TabCount;
			}
			break;
		case TAB__GETITEM:
			{
				int iTabindex = (int)wParam;
				TCIM_* tcim = (TCIM_*)lParam;

				if(iTabindex >= 0 && iTabindex < m_TabCount) {
					WFrame* pTab = getTab(iTabindex);
					if(pTab != NULL) {
						tcim->lParam = (LPARAM)pTab;
						return WM__OKAY;
					}
				}
				
				return WM__ERR;
			}
			break;
		case TAB__INSERTITEM:
			{
				int iIndex = (int)wParam;
				TCIM_* tcim = (TCIM_*)lParam;
				if(iIndex>= 0 && iIndex < m_TabCount) {
					
					H_WND hWnd = insertTab(iIndex, tcim->lpzText);
 
					if(hWnd != NULL) {
						tcim->lParam = (LPARAM)hWnd;

						return WM__OKAY;
					}
				}

				return WM__ERR;
			}
			break;
		case TAB__DELETEITEM:
			{
				int iIndex = (int)wParam;
				return deleteTab(iIndex);
			}
			break;
		case TAB__SETITEM:
			{
				int iIndex = (int)wParam;
				TCIM_* tcim = (TCIM_*)lParam;
				if(iIndex>= 0 && iIndex < m_TabCount) {

					H_WND hWnd = setTab(iIndex, tcim->lpzText);
					if(hWnd != NULL) {
						tcim->lParam = (LPARAM)hWnd;

						return WM__OKAY;
					}
				}

				return WM__ERR;
			}
			break;
		case TAB__SETITEMSIZE:
			{

			}
			break;
		case TAB__SETITEMTITLE:
			{
				int iIndex = (int)wParam;
				TCIM_* tcim = (TCIM_*)lParam;
				if(iIndex>= 0 && iIndex < m_TabCount) {
					
					WFrame* pTab = getTab(iIndex);
					pTab->setWindowName(tcim->lpzText);
					
					return WM__OKAY;
				}
				else
					return WM__ERR;
			}
			break;
		}
}
#endif