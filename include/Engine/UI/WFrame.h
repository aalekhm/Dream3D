#ifdef USE_YAGUI
#ifndef WFrame_H
#define WFrame_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WScrollbar.h"
#include "Engine/UI/WButton.h"

struct WFrame : public WContainer {
	private:
		std::string		m_title;

		bool			m_bResizable;

	public:
		WFrame();
		virtual ~WFrame();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void setResizable(bool bResizable)	{ m_bResizable = bResizable; }
		void setScrollbars(bool bHasScroll)	{ m_bHasScrollBars = bHasScroll; }
		void setBorderVisibility(bool bHasBorder)	{ m_bShowBorder = bHasBorder; }
		bool getBorderVisibility()					{ return m_bShowBorder; }
		
		void				onResize(int width, int height);
		void				resizeWidth(int iDiffWidth);
		void				resizeHeight(int iDiffHeight);

		virtual void onCreateEx(LPVOID lpVoid);
		virtual void onUpdate(float deltaTimeMs);
		virtual void onRender();

		virtual void onMouseDownEx(int x, int y, int iButton);
		virtual void onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy);
		virtual void onMouseUpEx(int x, int y, int iButton);
		virtual void onMouseWheelEx(WPARAM wParam, LPARAM lParam);

		virtual void onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		virtual bool isPtInside(int x, int y);
		
		void updateVBarPosition();
		void updateHBarPosition();
		void setVScrollbarLength();
		void setHScrollbarLength();
		
		WIDGET*			m_FrameWidget;
		WScrollbar*		m_sbVertical;
		WScrollbar*		m_sbHorizontal;

		WButton*		m_ButtonWResizeLeft;
		WButton*		m_ButtonWResizeRight;

		int				m_minX;
		int				m_maxX;
		int				m_minY;
		int				m_maxY;

		bool				m_bHasScrollBars;
		bool				m_bShowBorder;
		int				m_iMaxWidthPixels;
		int				m_iMaxHeightPixels;

		int				m_iMaxHScrollbarWidth;
		int				m_iMaxVScrollbarHeight;

		int				m_iResizingX;
};

#endif
#endif