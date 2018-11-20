#ifdef USE_YAGUI
#ifndef WTEXTBOX_H
#define WTEXTBOX_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WScrollbar.h"

struct WTextBox : public WContainer {
	
	public:
		WTextBox();
		~WTextBox();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);
		
		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);
		//void			create(WComponent* parent, int x, int y, int w, int h, const char* sText);
		void			showLineNumbers(bool bShow);
		bool			getReadOnly();
		void			setReadOnly(bool bRd);
	private:

		enum TextBoxState {
			NORMAL,
			READONLY
		};
		
		enum ID_SCROLLBARS {
			ID_VERTICAL_SCROLLBAR = 0,
			ID_HORIZONTAL_SCROLLBAR
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

		virtual void	onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		void			setClip(int x, int y , int width, int height);
		void			resetClip();
		RectF			m_reclaimRect;

		void			drawLineNumbers();
		void			setSelection();
		bool			isUnderSelection(int lineNo, int column);
		void			setText(const char* str);
		void			appendText(const char* str);
		int			getTextLength();
		const char*	getText();
		void			setVScrollbarLength();
		void			setHScrollbarLength();
		void			updateVBarPosition();
		void			updateHBarPosition();
		void			updateScrollBarVisibility();
		bool			isReadOnlyChar(char ch);
		void			calculateMaxLineWidth();
		void			pasteText();
		void			deleteSelectedText();

		WScrollbar*					m_sbHorizontal;
		int								m_iHorizontalBarHeight;
		WScrollbar*					m_sbVertical;
		int								m_iVerticalBarHeight;
		TextBoxState					mState;
		WIDGET*						m_TextBoxWidget;
		std::vector<std::string>	m_Lines;
		CCString						m_sText;

		void			drawStringFont(int x, int y, int anchor);
		void			getCaretPos(int x, int y);
		void			updateMains();
		void			setCaretDrawPosition();
		void			setTBLineNoSpace();
		void			scrollVertically(int dir);

		static unsigned int		LINE_HEIGHT;

		int				CURSOR_LINE_NO;
		int				PREV_CURSOR_LINE_NO;
		int				COLUMN_NO;
		int				SEL_CURSOR_LINE_NO;
		int				SEL_COLUMN_NO;
		int				LINES_PER_PAGE;
		int				TB_LINE_NO_SPACE;
		
		bool				m_IsVScrolling;
		bool				m_IsHScrolling;

		int				m_CaretPosX;
		int				m_CaretPosY;

		int				m_mainX;
		int				m_minX;
		int				m_maxX;

		int				m_mainY;
		int				m_minY;
		int				m_maxY;

		bool				m_bIsSelecting;	

		char*			dbStr;
		bool				HAS_LINE_NO;

		int				m_iMaxWidthPixels;
		int				m_iMaxHeightPixels;

		int				m_iMaxHScrollbarWidth;
		int				m_iMaxVScrollbarHeight;
};

#endif
#endif