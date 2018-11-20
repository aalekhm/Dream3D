#ifndef WSCROLLBAR_H
#define WSCROLLBAR_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WButton.h"

struct WScrollbar : public WContainer {

	public:
		enum SCROLLBARSTATE {
			NORMAL,
			HIGHLIGHTED,
			PUSHED,
			INACTIVE
		};

		enum SB_ALIGN {
			HORIZONTAL,
			VERTICAL
		};

		WScrollbar();
		~WScrollbar();
		
		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		void			setLength(float pixelsPercentage);
		void			setCursorPositionInPercent(float pixelsPercentage);
		void			setDisable(bool bDisable);
		void			setReadOnly(bool bDisable);
		void			hasBG(bool bBG) { m_bDrawBG = bBG; }
	protected:
		virtual void	onCreateEx(LPVOID lpVoid);
		virtual void	frameUpdate();
		virtual void	frameRender();

		virtual void	onMouseDownEx(int x, int y, int iButton);
		virtual void	onMouseUpEx(int x, int y, int iButton);
		virtual void	onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseMoveEx(int mCode, int x, int y, int prevX, int prevY);
		virtual void	onMouseWheelEx(WPARAM wParam, LPARAM lParam);

		virtual void	onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onMessage(UINT msg, WPARAM wParam, LPARAM lParam);
		
		void			onUpdate();
		void			onRender();

		void			moveCursorBy(int pixels);

		SCROLLBARSTATE		m_State;
		SB_ALIGN			m_Align;
		WIDGET*				m_ScrollbarWidget;

		WButton*			m_ButtonLeft;
		WButton*			m_ButtonScrollTrack;
		WButton*			m_ButtonRight;

		int					m_curSliderPos;
		int					m_minSliderPos;
		int					m_maxSliderPos;
		int					m_sliderLen;		
		bool				m_bDrawBG;		
};

#endif