#ifndef WTEXTFIELD_H
#define WTEXTFIELD_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"

struct WTextField : public WContainer {
	
	public:
		WTextField();
		~WTextField();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void				setReadOnly(bool bRd);
		bool				getReadOnly();
		const char*	getText();
		void				setText(const char* str);
		virtual void		onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch);
		void				selectText();
		void				setBorderVisibility(bool bHasBorder)	{ m_bShowBorder = bHasBorder; }
		bool				getBorderVisibility()					{ return m_bShowBorder; }
		int				getLength();

		DWORD			getSelection();
		void				setSelection(DWORD dwRange);
		static unsigned int LINE_HEIGHT;
	private:
		enum TextBoxState {
			NORMAL,
			READONLY
		};
	
		virtual void	onCreateEx(LPVOID lpVoid);
		virtual void	onUpdate();
		virtual void	onRender();

		virtual void	onMouseDownEx(int x, int y, int iButton);
		virtual void	onMouseUpEx(int x, int y, int iButton);
		virtual void	onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseMoveEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseWheelEx(WPARAM wParam, LPARAM lParam);

		virtual void	onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch);

		virtual void	onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void			setClip(int x, int y , int width, int height);
		void			resetClip();

		void			drawSelection();
		bool			isUnderSelection(int column);
		bool			isReadOnlyChar(char ch);
		
		void			drawStringFont(int x, int y, int anchor);
		void			getCaretPos(int x, int y);
		void			updateMains();
		void			setCaretDrawPosition();
		void			pasteText();
		void			deleteSelectedText();
		
		TextBoxState					mState;
		WIDGET*						m_TextBoxWidget;
		std::vector<std::string>	m_Lines;
		bool								m_bShowBorder;

		int				COLUMN_NO;
		int				SEL_COLUMN_NO;
		
		int				m_CaretPosX;
		int				m_CaretPosY;

		int				m_mainX;
		int				m_minX;
		int				m_maxX;

		int				m_mainY;
		int				m_minY;
		int				m_maxY;

		bool			m_bIsSelecting;	

		char*			m_clipData;
		RectF			m_reclaimRect;
};

#endif