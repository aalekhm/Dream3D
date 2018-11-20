#ifdef USE_YAGUI
#ifndef WBUTTON_H
#define WBUTTON_H

#include "Engine/UI/WComponent.h"
#include "Engine/UI/widgetdef.h"

struct WButton : public WComponent {

	private:
		enum ButtonState {
			NORMAL,
			HOVER,
			PUSHED,
			INACTIVE
		};
		
		char*				m_pTitle;
		ButtonState		m_State;

		char*			m_pButtonStateNameNormal;
		char*			m_pButtonStateNameHighlighted;
		char*			m_pButtonStateNamePushed;
		char*			m_pButtonStateNameDisabled;

		virtual void		onCreateEx(LPVOID lpVoid);
		virtual void		frameRender();
		virtual void		onUpdate(float deltaTimeMs);
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
		WButton();
		~WButton();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);
	
		LRESULT			OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void					setTitle(const char* sTitle) { sprintf(m_pTitle, "%s", sTitle); }
		void					setState(ButtonState bs) { m_State = bs; }
		ButtonState		getState() { return m_State; }
		void					deactivate();
		void					activate();

		static unsigned int BUTTON_TEXT_HEIGHT;
};

#endif
#endif