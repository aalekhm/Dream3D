
#ifndef WSLIDER_H
#define WSLIDER_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"

struct WSlider : public WContainer {
	private:
	public:
		WSlider();
		virtual ~WSlider();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		virtual void onCreateEx(LPVOID lpVoid);
		virtual void onUpdate();
		virtual void onRender();

		virtual void onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void onMouseDownEx(int x, int y, int iButton);
		virtual void onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy);
		virtual void onMouseUpEx(int x, int y, int iButton);
		virtual void onMouseWheelEx(WPARAM wParam, LPARAM lParam);

		virtual void onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		WIDGET*		m_DummyWidget;

		int				m_minX;
		int				m_maxX;
		int				m_minY;
		int				m_maxY;
};

#endif