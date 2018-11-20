
#ifndef WINSPECTOR_H
#define WINSPECTOR_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WScrollbar.h"
#include "Engine/UI/WInspectorTab.h"

#define LEFT_MARGIN_WIDTH	4
#define RIGHT_MARGIN_WIDTH	6
#define TOP_MARGIN_WIDTH	4
#define INSPECTOR_HEIGHT	27
#define INSPECTOR_SCROLL_WIDTH 15

struct WInspector : public WContainer {
	private:
		std::string		m_title;

		bool			m_bResizing;
		bool			m_bMoving;
		bool			m_bResizable;
		bool			m_bLButtonDown;

	public:
		WInspector();
		virtual ~WInspector();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		H_WND		createWindow(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);
		//void create(WComponent* parent, int left, int top, int width, int height, const std::string &title, bool bHasScrollBars);

		void setResizable(bool bResizable)	{ m_bResizable = bResizable; }
		void setScrollbars(bool bHasScroll)	{ m_bHasScrollBars = bHasScroll; }
		void setBorderVisibility(bool bHasBorder)	{ m_bShowBorder = bHasBorder; }
		const char* getTitle() { return m_title.c_str(); }
		
		virtual void onCreate();
		virtual void onUpdate();
		virtual void onRender();

		virtual void onMouseDownEx(int x, int y, int iButton);
		virtual void onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy);
		virtual void onMouseUpEx(int x, int y, int iButton);
		virtual void onMouseWheelEx(WPARAM wParam, LPARAM lParam);
		virtual void onMessage(UINT msg, WPARAM wParam, LPARAM lParam);
		virtual bool isPtInside(int x, int y);
		
		void updateVBarCursorPosition();
		void updateHBarCursorPosition();
		//void updateScrollBarPosition();
		//void updateHScrollBarPosition();
		//void updateVScrollBarPosition();
		void setVScrollbarLength();
		void setHScrollbarLength();

		void addTab();
		
		WIDGET*			m_FrameWidget;
		WScrollbar*		m_sbVertical;
		WScrollbar*		m_sbHorizontal;

		WButton*		m_ButtonWResizeLeft;
		WButton*		m_ButtonWResizeRight;
		
		int				m_iInspTabCount;
		std::vector<WInspectorTab*>	m_vInspectorTabs;

		bool			m_bHasScrollBars;
		bool			m_bShowBorder;

		int				m_iMaxWidthPixels;
		int				m_iMaxHeightPixels;

		int				m_iMaxInspectorTabWidth;
};

#endif