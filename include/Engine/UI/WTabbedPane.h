#ifndef WTABBEDPANE_H
#define WTABBEDPANE_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WFrame.h"
#include "Engine/UI/WButton.h"

#define TAB_WIDTH		113
#define TAB_TITLE_START_X 5
#define TAB_TITLE_START_Y 2
#define TAB_START_X		4

struct WTabbedPane : public WContainer {
	private:
		enum ID_SCROLLTABS {
			ID_BUTTON_SCROLL_TAB_LEFT = 0,
			ID_BUTTON_SCROLL_TAB_RIGHT
		};

		virtual void	onCreateEx(LPVOID lpVoid);
		virtual void	onUpdate();
		virtual void	onRender();
		virtual void	onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		virtual void	onMouseDownEx(int x, int y, int iButton);

		WContainer*		m_pParent;
		WIDGET*			m_TabbedWidget;
		WButton*			m_ButtonScrollLeft;
		WButton*			m_ButtonScrollRight;

		std::vector<WFrame*>	m_vTabFrames;
		int				m_TabCount;
		int				m_SelectedTab;
		int				m_StartTabId;
		int				m_MaxTabsOnScreen;

		int				m_minX;
		int				m_maxX;
		int				m_minY;
	public:
		WTabbedPane();
		virtual ~WTabbedPane();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		H_WND			addTab(const std::string& sTitle);
		WFrame*		addTab1(const std::string& sTitle);
		int				deleteTab(int iIndex);
		int				setSelectedTab(int iSel);
		int				getSelectedTab();
		WFrame*		getTab(int iIndex);
		WFrame*		getTab(const char* sTitle);
		H_WND			setTab(int iIndex, const std::string& sTitle);
		H_WND			insertTab(int iIndex, const std::string& sTitle);

		unsigned int	TAB_HEIGHT;
};

#endif