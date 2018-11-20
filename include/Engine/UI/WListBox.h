#ifndef WLISTBOX_H
#define WLISTBOX_H

#include "Engine/UI/WContainer.h"
#include "Engine/UI/WScrollbar.h"

struct WListBox : public WContainer {
	
	public:
		WListBox();
		~WListBox();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);
		
		void					addItem(LISTBOX_ITEM* item);
		void					insertItemAt(int pos, LISTBOX_ITEM* item);
		void					removeItemAt(int pos);
		int					getSelectedIndex();
		void					setSelectedIndex(int pos);
		const char*		getSelectedString();
		const char*		getStringAt(int pos);
		int					setStringAt(int iIndex, const char*);
		void					setProportionalToParent(bool bValue);
		int					getSize();
		void					clearList();

		virtual void			onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void			onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch);
	private:
		enum ListBoxState {
			NORMAL,
			READONLY
		};
		
		enum ID_SCROLLBARS {
			ID_VERTICAL_SCROLLBAR = 0,
			ID_HORIZONTAL_SCROLLBAR
		};
	
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

		virtual void		onMessage(UINT msg, WPARAM wParam, LPARAM lParam);
		
		void				setVScrollbarLength();
		void				setHScrollbarLength();
		void				updateVBarPosition();
		void				updateHBarPosition();
		void				updateScrollBarVisibility();
		void				setReadOnly(bool bRd);
		bool				getReadOnly();
		void				calculateMaxLineWidth();
		void				updateStretch();
		
		WScrollbar*							m_sbHorizontal;
		int										m_iHorizontalBarHeight;
		WScrollbar*							m_sbVertical;
		int										m_iVerticalBarHeight;
		ListBoxState							mState;
		WIDGET*								m_ListBoxWidget;
		std::vector<LISTBOX_ITEM*>	m_Data;
		int										m_MaxLineWidth;

		void			drawStringFont(int x, int y, int anchor);
		void			getCaretPos(int x, int y);
		void			updateMains();
		void			setCaretDrawPosition();
		void			setTBLineNoSpace();
		
		int				CURSOR_LINE_NO;
		int				PREV_CURSOR_LINE_NO;
		int				COLUMN_NO;
		//int				SEL_CURSOR_LINE_NO;
		//int				SEL_COLUMN_NO;
		int				LINES_PER_PAGE;
		int				CHARS_PER_PAGE;
		int				TB_LINE_NO_SPACE;
		
		bool				m_IsVScrolling;
		bool				m_IsHScrolling;

		bool				m_bStretchProportionalToParent;

		int				m_CaretPosX;
		int				m_CaretPosY;

		int				m_mainX;
		int				m_minX;
		int				m_maxX;

		int				m_mainY;
		int				m_minY;
		int				m_maxY;

		bool				HAS_LINE_NO;
		unsigned int	LINE_HEIGHT;
};

#endif