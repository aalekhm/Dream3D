#ifndef WSTATIC_H
#define WSTATIC_H

#include "Engine/UI/WComponent.h"
#include "Engine/UI/widgetdef.h"

struct WStatic : public WComponent {

	private:
		enum StaticState {
			NORMAL,
			INACTIVE
		};
		
		char*				m_pText;
		StaticState		m_State;
		
		virtual void	 onCreateEx(LPVOID lpVoid);
		virtual void	frameRender();
		virtual void	onUpdate();
		virtual void	onRender();
		virtual void	onMouseDown(int x, int y, int iButton);
		virtual void	onMouseUp(int x, int y, int iButton);
		virtual void	onMouseMove(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseWheel(WPARAM wParam, LPARAM lParam);
		virtual void	onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		WStatic();
		~WStatic();
		
		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void			setText(const char* sText) { sprintf(m_pText, "%s", sText); }
		void			setState(StaticState bs) { m_State = bs; }
		StaticState		getState() { return m_State; }
		void			deactivate();
		void			activate();
};

#endif