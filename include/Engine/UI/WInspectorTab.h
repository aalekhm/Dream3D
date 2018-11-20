
#ifndef WINSPECTORTAB_H
#define WINSPECTORTAB_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WButton.h"

#define INSPECTOR_TITLEBAR_HEIGHT	23
#define INSPECTOR_BOTTOM_DECORATION	5

#define	STATE_INSPECTOR_OPEN		1
#define	STATE_INSPECTOR_CLOSING		2
#define	STATE_INSPECTOR_CLOSED		3
#define	STATE_INSPECTOR_OPENING		4

struct WInspectorTab : public WContainer {
	private:
		WComponent*		m_pParent;
		std::string		m_title;

		bool			m_bLButtonDown;
		WIDGET*			m_InspectorTabCWidget;

		WButton*		m_ButtonExpand;
		WButton*		m_ButtonCollapse;

		int				m_iMaxWidthPixels;
		int				m_iMaxHeightPixels;

		int				m_iState;
		int				m_iRate;

		virtual void onCreate();
		virtual void onUpdate();
		virtual void onRender();

		virtual void onMouseDownEx(int x, int y, int iButton);
		virtual void onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy);
		virtual void onMouseUpEx(int x, int y, int iButton);
		virtual void onMouseWheelEx(WPARAM wParam, LPARAM lParam);
		virtual void onMessage(UINT msg, WPARAM wParam, LPARAM lParam);
		virtual bool isPtInside(int x, int y);
		
		void		 setState(int iState);
	public:
		WInspectorTab();
		virtual ~WInspectorTab();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		H_WND	createWindow(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);
		//void		create(WComponent* parent, int left, int top, int width, int height, const std::string &title);

		const char* getTitle() { return m_title.c_str(); }
};

#endif