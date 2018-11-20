
#ifndef WCOMPONENT_H
#define WCOMPONENT_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"

struct WComponent {
	
	public:
		enum WND_MSG {
			MOUSE_DOWN,
			MOUSE_UP,
			MOUSE_MOVE,
			MOUSE_HOVER,
			KEY_DOWN,
			KEY_UP,
			SCROLLER_POS_ON_DRAG,
			WIDTH_CHANGED,
			HEIGHT_CHANGED
		};

		WComponent();
		virtual ~WComponent();
		
		virtual	H_WND	Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam, bool bIsContainer = false, bool bIsChild = false);

		virtual LRESULT	OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);
		const char*		getClassName();
		const char*		getWindowName();
		void					setWindowName(const char* sWindowName);

		int			getLeft()	{ return m_iLeft; }
		int			getTop()	{ return m_iTop; }
		int			getRight()	{ return m_iRight; }
		int			getBottom() { return m_iBottom; }
		int			getWidth()	{ return m_iRight - m_iLeft; }
		int			getHeight()	{ return m_iBottom - m_iTop; }
		int			getOffsetX() { return m_iOffsetX; }
		int			getOffsetY() { return m_iOffsetY; }

		void		setPosition(int newOffsetX, int newOffsetY);
		void		setPositionX(int x) { setPosition(x, getOffsetY()); }
		void		setLeft(int x);
		void		setTop(int y);
		void		setPositionY(int y) { setPosition(getOffsetX(), y); }
		void		setSize(int w, int h);
		void		setWidth(int w) { 
						setSize(w, getHeight()); 
						onMessage((H_WND)this, WIDTH_CHANGED, getWidth(), w);
					}
		void		setHeight(int h) { 
						setSize(getWidth(), h); 
						onMessage((H_WND)this, HEIGHT_CHANGED, getHeight(), h);
					}

		void		setFixedZOrder(bool bFixed)	{ m_bFixedOrder = bFixed; }

		bool		hasFixedZOrder()	{ return m_bFixedOrder; }
		bool		isAlwaysOnTop()		{ return m_bAlwaysOnTop; }
		bool		isVisible()			{ return m_bVisible; }
		bool		hasMouseFocus()		{ return m_bHasMouseFocus; }
		bool		hasKeyFocus()		{ return m_bHasKeyBFocus; }
		bool		isActive()			{ return m_bActive; }
		bool		isEnabled()			{ return m_bEnabled; }
		
		bool		isContainer()		{ return m_bIsContainer; }
		void		setIsContainer(bool bCont){ m_bIsContainer = bCont; }

		void		setMovable(bool bMovable) { m_bMovable = bMovable; }
		bool		isMovable() { return m_bMovable; }

		void		setAsIntegral(bool bIntegralChild) { m_bIntegralChild = bIntegralChild; }
		bool		isIntegral() { return m_bIntegralChild; }
		
		void		updateComponentPosition();
		bool		isComponentAChild() { return m_bIsChild; }
		void		setComponentAsChild(bool bChild){ m_bIsChild = bChild; }
				
		bool		isPostRender()		{ return m_IsPostRender; }
		void		setPostRender(bool bPostRender){ m_IsPostRender = bPostRender; }

		void		getFocus();
		void		releaseFocus();
		void		getMouseFocus();
		void		getKeyBFocus();
		void		releaseMouseFocus();
		void		releaseKeyBFocus();

		WComponent*		getParent()	{ return m_pParent; }
		void		setParent(WComponent* pParent)	{ m_pParent = pParent; }
		void		setAlwaysOnTop(bool bOnTop)		{ m_bAlwaysOnTop = bOnTop; }
		void		setVisible(bool bVisible)		{ m_bVisible = bVisible; }

		void		setComponentID(int id) { m_iComponentID = id; }
		int		getComponentID() { return m_iComponentID; }

		virtual void	frameUpdate();
		virtual void	frameRender();

		// event handlers. Override these methods to define how your
		// component behaves
		virtual void		onCreateEx(LPVOID lpVoid) {};
		virtual void		updateScroll() {};
		virtual void		onUpdate() {};
		virtual void		onRender();
		virtual void		onMouseDown(int x, int y, int iButton);
		virtual void		onMouseUp(int x, int y, int iButton);
		virtual void		onMouseEnter(int mCode, int x, int y, int prevX, int prevY);
		virtual void		onMouseHover(int mCode, int x, int y, int prevX, int prevY);
		virtual void		onMouseMove(int mCode, int x, int y, int prevX, int prevY);
		virtual void		onMouseLeave(int mCode, int x, int y, int prevX, int prevY);
		virtual void		onMouseWheel(WPARAM wParam, LPARAM lParam);
		virtual void		onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void		onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void		onActivate()	{ m_bActive = true; }
		virtual void		onDeactivate()	{ m_bActive = false; }
		virtual void		onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		virtual bool	isPointInside(int x, int y);

		// Coordinate transformation
		//Vector2	worldToLocal(const Vector2& v) const;
		//Vector2 parentToLocal(const Vector2& v) const;
		//Vector2 localToWorld(const Vector2& v) const;
		//Vector2 localToParent(const Vector2& v) const;
		
		//helpers
		static void drawComponent();
		int			m_iLeft, m_iTop, m_iRight, m_iBottom;
		int			m_iOffsetX, m_iOffsetY;
		int			m_iMainX, m_iMainY;
	protected:
		WComponent*	m_pParent;
		char*			m_pClassName;
		char*			m_pWindowName;
		
		bool		m_bAlwaysOnTop;
		bool		m_bVisible;
		bool		m_bHasMouseFocus;
		bool		m_bHasKeyBFocus;
		bool		m_bActive;
		bool		m_bEnabled;
		bool		m_bFixedOrder;
		bool		m_bIsContainer;
		bool		m_bIsChild;
		bool		m_IsPostRender;
		bool		m_bMovable;
		bool		m_bIntegralChild;

		int		m_iComponentID;

};

#endif