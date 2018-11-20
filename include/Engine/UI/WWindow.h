
#ifndef WWINDOW_H
#define WWINDOW_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WScrollbar.h"
#include "Engine/UI/WButton.h"
//#include <string.h>

#define WINDOW_TITLEBAR_HEIGHT	31

struct WWindow : public WContainer {
	private:
		std::string		m_title;

		bool			m_bResizable;

	public:
		WWindow();
		virtual ~WWindow();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);
	
		H_WND			createWindow(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);
		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);
		//void create(WComponent* parent, int left, int top, int width, int height, int iType, const std::string &title);

		void setResizable(bool bResizable)	{ m_bResizable = bResizable; }
		const char* getTitle() { return m_title.c_str(); }
		
		void				onResize(int width, int height);
		void				resizeWidth(int iDiffWidth);
		void				resizeHeight(int iDiffHeight);

		virtual void onCreate();
		virtual void onUpdate();
		virtual void onRender();

		virtual void onMouseDownEx(int x, int y, int iButton);
		virtual void onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy);
		virtual void onMouseUpEx(int x, int y, int iButton);
		virtual void onMouseWheelEx(WPARAM wParam, LPARAM lParam);

		virtual void onMessage(UINT msg, WPARAM wParam, LPARAM lParam);
		virtual bool isPtInside(int x, int y);

		void updateVBarPosition();
		void updateHBarPosition();
		void setVScrollbarLength();
		void setHScrollbarLength();
		
		WIDGET*			m_WindowCWidget;
		WScrollbar*		m_sbVertical;
		WScrollbar*		m_sbHorizontal;

		WButton*		m_ButtonClose;
		WButton*		m_ButtonMaximize;
		WButton*		m_ButtonWResizeLeft;
		WButton*		m_ButtonWResizeRight;

		int				m_minX;
		int				m_maxX;
		int				m_minY;
		int				m_maxY;

		int				m_iMaxWidthPixels;
		int				m_iMaxHeightPixels;

		int				m_iMaxHScrollbarWidth;
		int				m_iMaxVScrollbarHeight;

		int				m_WndType;
		
		int				m_iResizingX;
};

#endif