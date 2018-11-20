#ifdef USE_YAGUI
#ifndef WSPRITE_H
#define WSPRITE_H

#include "Engine/UI/WContainer.h"
#include "Engine/UI/widgetdef.h"

struct WSprite : public WContainer {

	private:
		enum ButtonState {
			NORMAL,
			HOVER,
			PUSHED,
			INACTIVE
		};
		
		ButtonState		m_State;

		char*			m_pSpriteWidget;

		virtual void		onCreateEx(LPVOID lpVoid);
		virtual void		onUpdate();
		virtual void		onRender();

		virtual void		onMouseDownEx(int x, int y, int iButton);
		virtual void		onMouseUpEx(int x, int y, int iButton);
		virtual void		onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void		onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void		onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void		onMouseMoveEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void		onMouseWheelEx(WPARAM wParam, LPARAM lParam);

		virtual void		onKeyBDownEx(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void		onKeyBUpEx(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void		onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		WSprite();
		~WSprite();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);
	
		LRESULT			OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void					setState(ButtonState bs) { m_State = bs; }
		ButtonState		getState() { return m_State; }
		void					deactivate();
		void					activate();
};

#endif
#endif