
#ifndef WCONTAINER_H
#define WCONTAINER_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WComponent.h"

struct WContainer : public WComponent {
	
	public:
		WContainer();
		virtual ~WContainer();

		virtual	H_WND	Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam, bool bIsContainer = false, bool bIsChild = false);

		void					addComponent(WComponent* pC);
		void					removeComponent(WComponent* pC);
		int					getNumChildren() {	return (int)m_pChildren.size(); }
		WComponent*		getChild(int pos) { return m_pChildren[pos]; }

		void					addBaseSkinChild(CHILD* pC);
		void					removeBaseSkinChild(CHILD* pC);

		// overridables
		virtual		void		onCreateEx(LPVOID lpVoid) { };
		virtual		void		onMouseDownEx(int x, int y, int iButton) { };
		virtual		void		onMouseUpEx(int x, int y, int iButton) { };
		virtual		void		onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY) { };
		virtual		void		onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY) { };
		virtual		void		onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY) { };
		virtual		void		onMouseMoveEx(int mCode, int x, int y, int prevX, int prevY);
		virtual		void		onMouseWheelEx(WPARAM wParam, LPARAM lParam) { }
		virtual		void		onKeyBDownEx(unsigned int iVirtualKeycode, unsigned short ch) { };
		virtual		void		onKeyBUpEx(unsigned int iVirtualKeycode, unsigned short ch) { };
	
		virtual		void		frameUpdate();
		virtual		void		frameRender();
		bool						canUpdateOrRender();

		virtual		void		onMouseDown(int x, int y, int iButton);
		virtual		void		onMouseUp(int x, int y, int iButton);
		virtual		void		onMouseEnter(int mCode, int x, int y, int prevX, int prevY);
		virtual		void		onMouseHover(int mCode, int x, int y, int prevX, int prevY);
		virtual		void		onMouseLeave(int mCode, int x, int y, int prevX, int prevY);
		virtual		void		onMouseMove(int mCode, int x, int y, int prevX, int prevY);
		virtual		void		onMouseWheel(WPARAM wParam, LPARAM lParam);
		virtual		void		onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch);
		virtual		void		onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch);

		virtual		void		onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		H_WND					GetWindow(UINT ID_WINDOW);
		H_WND					FindWindowQ(LPCSTR lpClassName, LPCSTR lpWindowName);
		int						getIndex(const WComponent* pComp);

	protected:

		void				bringChildToFront(int id);
		void				postRender();
		virtual void		postRenderEx() { }

		std::vector<WComponent*>	m_pChildren;
		WComponent*						m_pActiveComponent;

		std::vector<CHILD*>			m_pBaseSkinChilds;

		RectF								m_ClientRect;
		REAL									m_iClientRectW;
		REAL									m_iClientRectH;
};

#endif
