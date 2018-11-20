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
		virtual void	onUpdate();
		virtual void	onRender();
		virtual void	onMouseDown(int x, int y, int iButton);
		virtual void	onMouseUp(int x, int y, int iButton);
		virtual void	onMouseEnter(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseHover(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseLeave(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseMove(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseWheel(WPARAM wParam, LPARAM lParam);
		virtual void	onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		static unsigned int CHECKBOX_TEXT_HEIGHT;
};

#endif