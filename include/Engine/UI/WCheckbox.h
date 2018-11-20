#ifdef USE_YAGUI
#ifndef WCHECKBOX_H
#define WCHECKBOX_H

#include "Engine/UI/WComponent.h"
#include "Engine/UI/widgetdef.h"

struct WCheckbox : public WComponent {

	private:
		enum CheckButtonState {
			NORMAL,
			HIGHLIGHTED,
			PUSHED,
			INACTIVE
		};
		
		char*					m_pTitle;
		CheckButtonState	m_State;
		bool						m_bChecked;

		char*			m_pCBCheckedStateNameNormal;
		char*			m_pCBCheckedStateNameHighlighted;
		char*			m_pCBCheckedStateNamePushed;
		char*			m_pCBCheckedStateNameDisabled;

		char*			m_pCBUnCheckedStateNameNormal;
		char*			m_pCBUnCheckedStateNameHighlighted;
		char*			m_pCBUnCheckedStateNamePushed;
		char*			m_pCBUnCheckedStateNameDisabled;
	public:
		WCheckbox();
		~WCheckbox();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);
		
		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void			setTitle(const char* sTitle) { sprintf(m_pTitle, "%s", sTitle); }
		void			setChecked(bool bCheck) { m_bChecked = bCheck; }
		bool			isChecked() { return m_bChecked; }

		void			activate();
		void			deactivate();
		
		virtual void	onCreateEx(LPVOID lpVoid);
		virtual void	onUpdate(float deltaTimeMs);
		virtual void	onRender();
		virtual void	onMouseDownEx(int x, int y, int iButton);
		virtual void	onMouseUpEx(int x, int y, int iButton);
		virtual void	onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseMoveEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseWheelEx(WPARAM wParam, LPARAM lParam);
		virtual void	onKeyBDownEx(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onKeyBUpEx(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		static unsigned int CHECKBOX_TEXT_HEIGHT;
};

#endif
#endif