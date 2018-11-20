#ifdef USE_YAGUI
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
		bool			m_bResizing;
		bool			m_bMoving;
		bool			m_bResizable;
		bool			m_bLButtonDown;

	public:
		WInspector();
		virtual ~WInspector();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		void setResizable(bool bResizable)	{ m_bResizable = bResizable; }
		void setScrollbars(bool bHasScroll)	{ m_bHasScrollBars = bHasScroll; }
		void setBorderVisibility(bool bHasBorder)	{ m_bShowBorder = bHasBorder; }
		
		virtual void onCreateEx(LPVOID lpVoid);
		virtual void onUpdate(float deltaTimeMs);
		virtual void onRender();

		virtual void onMouseDownEx(int x, int y, int iButton);
		virtual void onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy);
		virtual void onMouseUpEx(int x, int y, int iButton);
		virtual void onMouseWheelEx(WPARAM wParam, LPARAM lParam);
		virtual void onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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
#endif