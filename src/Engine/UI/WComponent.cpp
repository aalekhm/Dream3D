#ifdef USE_YAGUI
#include "Engine/UI/WComponent.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WWidgetManager.h"
#include "Engine/UI/WComponentFactory.h"

RenderAlignmentData WComponent::m_StaticRenderAlignment;

WComponent::WComponent()	: m_iLeft(0)
											, m_iTop(0) 
											, m_iRight(0)
											, m_iBottom(0)

											, m_iOffsetX(0)
											, m_iOffsetY(0)

											, m_iMainX(0)
											, m_iMainY(0)

											, m_pParent(NULL)
											, m_pAlignmentParent(NULL)
											, m_bAlwaysOnTop(false)
											, m_bVisible(true)
											, m_bHasMouseFocus(false)
											, m_bHasKeyBFocus(false)
											, m_bActive(false)
											, m_bEnabled(true)
											, m_bFixedOrder(false)
											, m_bIsContainer(false)
											, m_bIsChild(false)
											, m_IsPostRender(false)
											, m_bMovable(true)
											, m_bIntegralChild(false)
											, m_iComponentID(-1)

											, m_pClassName(NULL)
											, m_pWindowName(NULL)

											, m_iAnchor(WM_ANCHOR_TOPLEFT)
											, m_iAlignWRTParent(WM_ALIGN_WRT_PARENT_TOPLEFT)

#ifdef HAS_CUSTOM_FEATURES
											, m_bIsCustom(false)
											, m_eCustomState(CUSTOM_STATE_NONE)
											, m_pMiscChildsWidget(NULL)
#endif
{
	m_pClassName = new char[255];
	memset(m_pClassName, 0, 255);

	m_pWindowName = new char[255];
	memset(m_pWindowName, 0, 255);
}

H_WND WComponent::Create(		const char* lpClassName, 
												const char* lpWindowName, 
												DWORD dwStyle, 
												int x, 
												int y, 
												int width, 
												int height, 
												H_WND hwndParent, 
												HMENU hMenu, 
												LPVOID lParam,
												bool bIsContainer, 
												bool bIsChild
) {
		m_pParent = (WComponent*)hwndParent;

		memset(m_pClassName, 0, 255);
		sprintf(m_pClassName, "%s", lpClassName);

		setWindowName(lpWindowName);

		m_iMainX = m_iMainY = 0;
		m_iLeft = m_iOffsetX = x;
		m_iTop = m_iOffsetY = y;
		m_iRight = m_iLeft + width;
		m_iBottom = m_iTop + height;
		
		m_bIsCustom = (dwStyle & WM_CUSTOM);
		m_iAnchor = (dwStyle & WM_ALIGN_WRT_PARENT);
		
		m_iAlignWRTParent = (dwStyle & WM_ANCHOR);
		if(m_iAlignWRTParent == 0)
			m_iAlignWRTParent = WM_ALIGN_WRT_PARENT_TOPLEFT;

		updateComponentPosition();

		setComponentID((int)hMenu);
		setIsContainer(bIsContainer);
		setComponentAsChild(bIsChild);

		if(m_pParent != NULL)
			((WContainer*)m_pParent)->addComponent(this);

		onCreateEx(lParam);

#ifdef HAS_CUSTOM_FEATURES
		createComponentCustomWidgets();
#endif
		WWidgetManager::onEvent((H_WND)this , WN__CREATE, getComponentID(), 0);

		return this;
}

const char* WComponent::getClassName() {
	return m_pClassName;
}

const char* WComponent::getWindowName() {
	return m_pWindowName;
}

void WComponent::setWindowName(const char* sWindowName) {
	if(sWindowName != NULL && strlen(sWindowName) > 0) {
		memset(m_pWindowName, 0, 255);
		sprintf(m_pWindowName, "%s", sWindowName);
	}
}

void WComponent::getFocus() {
	getFocusEx();

	getMouseFocus();
	getKeyBFocus();
}

void WComponent::releaseFocus() {
#ifdef HAS_CUSTOM_FEATURES
	m_eCustomState = CUSTOM_STATE_NONE;
	m_StaticRenderAlignment.m_pMe = NULL;
#endif

	releaseFocusEx();

	releaseMouseFocus();
	releaseKeyBFocus();
}

void WComponent::getMouseFocus() {
	m_bHasMouseFocus = true;
}

void WComponent::getKeyBFocus() {
	m_bHasKeyBFocus = true;
}

void WComponent::releaseMouseFocus() {
	m_bHasMouseFocus = false;
}

void WComponent::releaseKeyBFocus() {
	m_bHasKeyBFocus = false;
}

void WComponent::updateComponentPosition() {
	////////////////////////////////////////////////////////////
	u32 _width = getWidth();
	u32 _height = getHeight();

	WComponent* pParent = (m_pAlignmentParent == NULL) ? m_pParent : m_pAlignmentParent;

	if(pParent) {
		s32 _parentX = pParent->getLeft();
		s32 _parentY = pParent->getTop();
		s32 _parentW = pParent->getWidth();
		s32 _parentH = pParent->getHeight();

		/////////////////////////////////////
		{
			if(m_iAlignWRTParent & WM_ALIGN_WRT_PARENT_LEFT) {
				_parentX = pParent->getLeft();
			}
			else
			if(m_iAlignWRTParent & WM_ALIGN_WRT_PARENT_CENTERX) {
				_parentX = pParent->getLeft() + (_parentW >> 1);
			}
			else
			if(m_iAlignWRTParent & WM_ALIGN_WRT_PARENT_RIGHT) {
				_parentX = pParent->getLeft() + _parentW;
			}

			if(m_iAlignWRTParent & WM_ALIGN_WRT_PARENT_TOP) {
				_parentY = pParent->getTop();
			}
			else
			if(m_iAlignWRTParent & WM_ALIGN_WRT_PARENT_CENTERY) {
				_parentY = pParent->getTop() + (_parentH >> 1);
			}
			else
			if(m_iAlignWRTParent & WM_ALIGN_WRT_PARENT_BOTTOM) {
				_parentY = pParent->getTop() + _parentH;
			}
		}
		
		//////////////////////////////////////////////////

		if(m_iAnchor & WM_ANCHOR_LEFT) {
			m_iLeft = _parentX + m_iOffsetX;
		}
		else
		if(m_iAnchor & WM_ANCHOR_CENTERX) {
			m_iLeft = _parentX + m_iOffsetX - (_width >> 1);
		}
		else
		if(m_iAnchor & WM_ANCHOR_RIGHT) {
			m_iLeft = _parentX + m_iOffsetX - _width;
		}
		else {
			m_iLeft = _parentX + m_iOffsetX;
		}

		if(m_iAnchor & WM_ANCHOR_TOP) {
			m_iTop = _parentY + m_iOffsetY;
		}
		else
		if(m_iAnchor & WM_ANCHOR_CENTERY) {
			m_iTop = _parentY + m_iOffsetY - (_height >> 1);
		}
		else
		if(m_iAnchor & WM_ANCHOR_BOTTOM) {
			m_iTop = _parentY + m_iOffsetY - _height;
		}
		else {
			m_iTop = _parentY + m_iOffsetY;
		}

		if(isComponentAChild()) {
			m_iLeft += (isMovable() ? pParent->m_iMainX : 0);
			m_iTop += (isMovable() ? pParent->m_iMainY : 0);
		}
	}

	m_iRight = m_iLeft + _width;
	m_iBottom = m_iTop + _height;
}

u32 WComponent::getAnchorX() {
	u32 _returnX = getLeft();
	u32 _width = getWidth();

	if(m_pParent) {
		switch(m_iAnchor) {
			case WM_ANCHOR_TOPLEFT:
			case WM_ANCHOR_CENTERLEFT:
			case WM_ANCHOR_BOTTOMLEFT:
			{
				_returnX = getLeft();
			}
			break;
			case WM_ANCHOR_TOPCENTER:
			case WM_ANCHOR_CENTER:
			case WM_ANCHOR_BOTTOMCENTER:
			{
				_returnX = getLeft() + (_width >> 1);
			}
			break;
			case WM_ANCHOR_TOPRIGHT:
			case WM_ANCHOR_CENTERRIGHT:
			case WM_ANCHOR_BOTTOMRIGHT:
			{
				_returnX = getLeft() + _width;
			}
			break;
			default:
			{
				_returnX = getLeft();
			}
			break;
		}
	}

	return _returnX;
}

u32 WComponent::getAnchorY() {
	u32 _returnY = getTop();
	u32 _height = getHeight();

	if(m_pParent) {
		switch(m_iAnchor) {
		case WM_ANCHOR_TOPLEFT:
		case WM_ANCHOR_TOPCENTER:
		case WM_ANCHOR_TOPRIGHT:
			{
				_returnY = getTop();
			}
			break;
		case WM_ANCHOR_CENTERLEFT:
		case WM_ANCHOR_CENTER:
		case WM_ANCHOR_CENTERRIGHT:
			{
				_returnY = getTop() + (_height >> 1);
			}
			break;
		case WM_ANCHOR_BOTTOMLEFT:
		case WM_ANCHOR_BOTTOMCENTER:
		case WM_ANCHOR_BOTTOMRIGHT:
			{
				_returnY = getTop() + _height;
			}
			break;
		default:
			{
				_returnY = getTop();
			}
			break;
		}
	}

	return _returnY;
}

void WComponent::setLeft(int x) {
	m_iLeft += x; m_iRight += x;
}

void WComponent::setTop(int y) {
	m_iTop += y; m_iBottom += y;
}

void WComponent::setPosition(int newOffsetX, int newOffsetY) { 
	m_iOffsetX = newOffsetX;
	m_iOffsetY = newOffsetY;
	updateComponentPosition();

	WWidgetManager::onEvent((H_WND)this , WN__MOVE, getComponentID(), 0);
}

void WComponent::setSize(int w, int h) { 
	m_iRight = m_iLeft + w;
	m_iBottom = m_iTop + h; 

	WWidgetManager::onEvent((H_WND)this , WN__SIZE, getComponentID(), 0);
}

void WComponent::onRender() {

}

void WComponent::postRenderInternal() {
	if(m_bIsCustom) {

		WWidgetManager* pRenderer =  WWidgetManager::getInstance();
		Rect rRect(0, 0, 1, 1);
		char sStr[255];

		s32 _PosX = getLeft();
		s32 _PosY = getTop();
		u32 _Width = getWidth();
		u32 _WidthHalf = _Width >> 1;
		u32 _Height = getHeight();
		u32 _HeightHalf = _Height >> 1;
		u32 _WidthBy3 = _Width / 3;
		u32 _WidthBy3Half = _WidthBy3 >> 1;
		u32 _HeightBy3 = _Height / 3;
		u32 _HeightBy3Half = _HeightBy3 >> 1;
#ifdef HAS_CUSTOM_FEATURES
		if(m_eCustomState == CUSTOM_STATE_MOVE) {

			RectF renderRect(0, 0, 1, 1);
			WIDGET* pWidget = NULL;
			{
				rRect.X = _PosX; rRect.Y = _PosY, rRect.Width = _Width; rRect.Height = _Height;
				pRenderer->fillRect(64.0f, 64.0f, 64.0f, 200.0f, &rRect);

				pWidget = WWidgetManager::getWidget("ButtonLeft_Normal");
				renderRect.Width = pWidget->widgetSize.width; renderRect.Height = pWidget->widgetSize.height; 
				renderRect.X = _PosX + _WidthHalf - ((u32)pWidget->widgetSize.width << 1);  renderRect.Y = _PosY + _HeightHalf - ((u32)pWidget->widgetSize.height >> 1); 
				WWidgetManager::renderWidget("ButtonLeft_Normal", &renderRect);

				pWidget = WWidgetManager::getWidget("ButtonRight_Normal");
				renderRect.Width = pWidget->widgetSize.width; renderRect.Height = pWidget->widgetSize.height; 
				renderRect.X = _PosX + _WidthHalf + (u32)pWidget->widgetSize.width;  renderRect.Y = _PosY + _HeightHalf - ((u32)pWidget->widgetSize.height >> 1); 
				WWidgetManager::renderWidget("ButtonRight_Normal", &renderRect);

				pWidget = WWidgetManager::getWidget("ButtonUp_Normal");
				renderRect.Width = pWidget->widgetSize.width; renderRect.Height = pWidget->widgetSize.height; 
				renderRect.X = _PosX + _WidthHalf - ((u32)pWidget->widgetSize.width >> 1);  renderRect.Y = _PosY + _HeightHalf - ((u32)pWidget->widgetSize.height << 1); 
				WWidgetManager::renderWidget("ButtonUp_Normal", &renderRect);

				pWidget = WWidgetManager::getWidget("ButtonDown_Normal");
				renderRect.Width = pWidget->widgetSize.width; renderRect.Height = pWidget->widgetSize.height; 
				renderRect.X = _PosX + _WidthHalf - ((u32)pWidget->widgetSize.width >> 1);  renderRect.Y = _PosY + _HeightHalf + (u32)pWidget->widgetSize.height; 
				WWidgetManager::renderWidget("ButtonDown_Normal", &renderRect);
			}
		}
		else
		if(m_eCustomState == CUSTOM_STATE_SET_ANCHOR || m_eCustomState == CUSTOM_STATE_ALIGN_TO_PARENT) {
			///////////////////////////////////////////////////////////////
			rRect.X = _PosX; rRect.Y = _PosY; rRect.Width = _WidthBy3; rRect.Height = _HeightBy3;
			pRenderer->fillRect(255.0f, 0.0f, 0.0f, 255.0f, &rRect);

			rRect.X = _PosX + _WidthHalf - _WidthBy3Half; rRect.Y = _PosY; rRect.Width = _WidthBy3; rRect.Height = _HeightBy3;
			pRenderer->fillRect(0.0f, 255.0f, 0.0f, 255.0f, &rRect);

			rRect.X = _PosX + _Width - _WidthBy3; rRect.Y = _PosY; rRect.Width = _WidthBy3; rRect.Height = _HeightBy3;
			pRenderer->fillRect(0.0f, 0.0f, 255.0f, 255.0f, &rRect);
			///////////////////////////////////////////////////////////////
			rRect.X = _PosX; rRect.Y = _PosY + _HeightHalf - _HeightBy3Half; rRect.Width = _WidthBy3; rRect.Height = _HeightBy3;
			pRenderer->fillRect(128.0f, 128.0f, 255.0f, 255.0f, &rRect);

			rRect.X = _PosX + _WidthHalf - _WidthBy3Half; rRect.Y = _PosY + _HeightHalf - _HeightBy3Half; rRect.Width = _WidthBy3; rRect.Height = _HeightBy3;
			pRenderer->fillRect(255.0f, 128.0f, 128.0f, 255.0f, &rRect);

			rRect.X = _PosX + _Width - _WidthBy3; rRect.Y = _PosY + _HeightHalf - _HeightBy3Half; rRect.Width = _WidthBy3; rRect.Height = _HeightBy3;
			pRenderer->fillRect(128.0f, 255.0f, 128.0f, 255.0f, &rRect);
			///////////////////////////////////////////////////////////////
			rRect.X = _PosX; rRect.Y = _PosY + _Height - _HeightBy3; rRect.Width = _WidthBy3; rRect.Height = _HeightBy3;
			pRenderer->fillRect(255.0f, 255.0f, 0.0f, 255.0f, &rRect);

			rRect.X = _PosX + _WidthHalf - _WidthBy3Half; rRect.Y = _PosY + _Height - _HeightBy3; rRect.Width = _WidthBy3; rRect.Height = _HeightBy3;
			pRenderer->fillRect(0.0f, 255.0f, 255.0f, 255.0f, &rRect);

			rRect.X = _PosX + _Width - _WidthBy3; rRect.Y = _PosY + _Height - _HeightBy3; rRect.Width = _WidthBy3; rRect.Height = _HeightBy3;
			pRenderer->fillRect(255.0f, 0.0f, 255.0f, 255.0f, &rRect);
			///////////////////////////////////////////////////////////////
		}

		////////////////////// Draw Anchor //////////////////////
		Rect thisWndPos(getAnchorX() - 2, getAnchorY() - 2, 4, 4);

		pRenderer->fillRect(255.0f, 255.0f, 255.0f, 255.0f, &thisWndPos);

		thisWndPos.X += 1; 	thisWndPos.Y += 1; thisWndPos.Width >>= 1; thisWndPos.Height >>= 1;
		pRenderer->fillRect(0.0f, 0.0f, 0.0f, 255.0f, &thisWndPos);
		///////////////////////////////////////////////////////

		////////////////////// Show Anchor Position //////////////////////
		memset(sStr, 0, 255);
		sprintf_s(sStr, "( %d, %d )", getOffsetX(), getOffsetY());
		byte iLineHeight = 15;
		thisWndPos.X = getAnchorX(); thisWndPos.Y = getAnchorY(); thisWndPos.Width = WWidgetManager::getStringWidthTillPos(sStr, strlen(sStr)) + 10; thisWndPos.Height = (iLineHeight + 4);
		pRenderer->fillRect(0.0f, 0.0f, 0.0f, 255.0f, &thisWndPos);
		WWidgetManager::setColor(0xffffffff);//ABGR
		WWidgetManager::getInstance()->drawStringFont(sStr, getAnchorX() + 5, getAnchorY() - 2, 0);
		//////////////////////////////////////////////////////////////

		///////////////// Show Alignment wrt Parent //////////////////////
		if(m_eCustomState != CUSTOM_STATE_NONE) {
			if(m_pParent != NULL) {
				m_StaticRenderAlignment.m_pMe = this;
			}
		}

		if(m_StaticRenderAlignment.m_pMe != NULL) {
			WComponent* pParent = hasAlignmentParent() ? pParent = m_StaticRenderAlignment.m_pMe->getAlignmentParent() : m_StaticRenderAlignment.m_pMe->getParent();
			WORD			_iAlignWRTParent = m_StaticRenderAlignment.m_pMe->m_iAlignWRTParent;
			if(m_StaticRenderAlignment.m_pMe->getParent() == this) {
				thisWndPos.Y = getTop();
				thisWndPos.Width = 5;
				thisWndPos.Height = getHeight();
				if(_iAlignWRTParent & WM_ALIGN_WRT_PARENT_LEFT) {
					thisWndPos.X = getLeft();
				}
				else
				if(_iAlignWRTParent & WM_ALIGN_WRT_PARENT_CENTERX) {
					thisWndPos.X = getLeft() + (getWidth() >> 1);
				}
				else
				if(_iAlignWRTParent & WM_ALIGN_WRT_PARENT_RIGHT) {
					thisWndPos.X = getLeft() + getWidth() - thisWndPos.Width;
				}
				pRenderer->fillRect(0.0f, 0.0f, 0.0f, 255.0f, &thisWndPos);

				thisWndPos.X = getLeft();
				thisWndPos.Height = 5;
				thisWndPos.Width = getWidth();
				if(_iAlignWRTParent & WM_ALIGN_WRT_PARENT_TOP) {
					thisWndPos.Y = getTop();
				}
				else
				if(_iAlignWRTParent & WM_ALIGN_WRT_PARENT_CENTERY) {
					thisWndPos.Y = getTop() + (getHeight() >> 1);
				}
				else
				if(_iAlignWRTParent & WM_ALIGN_WRT_PARENT_BOTTOM) {
					thisWndPos.Y = getTop() + getHeight() - thisWndPos.Height;
				}
				pRenderer->fillRect(0.0f, 0.0f, 0.0f, 255.0f, &thisWndPos);
			}
		}
		//////////////////////////////////////////////////////////////
#endif
	}
}

void WComponent::onMouseDown(int x, int y, int iButton) {
	
#ifdef HAS_CUSTOM_FEATURES
	WWidgetManager::setCursor(IDC__ARROW);
	
	if(m_eCustomState == CUSTOM_STATE_SET_ANCHOR) {

		u32 _boxWidth = getWidth() / 3;
		u32 _boxHeight = getHeight() / 3;
		u32 _xBox = (x - getLeft()) / _boxWidth;
		u32 _yBox = (y - getTop()) / _boxHeight;
		u32 _iBox = _yBox * 3 + _xBox;

		if((iButton & MK_LBUTTON) != 0) {
			switch(_iBox) {
				case 0:
					m_iAnchor = WM_ANCHOR_TOPLEFT;
				break;
				case 1:
					m_iAnchor = WM_ANCHOR_TOPCENTER;
				break;
				case 2:
					m_iAnchor = WM_ANCHOR_TOPRIGHT;
				break;
				case 3:
					m_iAnchor = WM_ANCHOR_CENTERLEFT;
				break;
				case 4:
					m_iAnchor = WM_ANCHOR_CENTER;
				break;
				case 5:
					m_iAnchor = WM_ANCHOR_CENTERRIGHT;
				break;
				case 6:
					m_iAnchor = WM_ANCHOR_BOTTOMLEFT;
				break;
				case 7:
					m_iAnchor = WM_ANCHOR_BOTTOMCENTER;
				break;
				case 8:
					m_iAnchor = WM_ANCHOR_BOTTOMRIGHT;
				break;
			}
			
			updateComponentPosition();
		}
	}
	else
	if(m_eCustomState == CUSTOM_STATE_ALIGN_TO_PARENT) {
		u32 _boxWidth = getWidth() / 3;
		u32 _boxHeight = getHeight() / 3;
		u32 _xBox = (x - getLeft()) / _boxWidth;
		u32 _yBox = (y - getTop()) / _boxHeight;
		u32 _iBox = _yBox * 3 + _xBox;


		s32 _iParentX = 0;
		s32 _iParentY = 0;
		u32 _iParentWidth = 0;
		u32 _iParentHeight = 0;
		u32 _iParentWidthHalf = 0;
		u32 _iParentHeightHalf = 0;
		if(m_pParent != NULL) {
			_iParentX = m_pParent->getLeft();
			_iParentY = m_pParent->getTop();
			_iParentWidth = m_pParent->getWidth();
			_iParentWidthHalf = _iParentWidth >> 1;
			_iParentHeight = m_pParent->getHeight();
			_iParentHeightHalf = _iParentHeight >> 1;
		}
		
		if((iButton & MK_LBUTTON) != 0) {
			switch(_iBox) {
			case 0:
				m_iAlignWRTParent = WM_ALIGN_WRT_PARENT_TOPLEFT;
				m_iOffsetX = getAnchorX() - _iParentX;
				m_iOffsetY = getAnchorY() - _iParentY;
				break;
			case 1:
				m_iAlignWRTParent = WM_ALIGN_WRT_PARENT_TOPCENTER;
				m_iOffsetX = getAnchorX() - (_iParentX + _iParentWidthHalf);
				m_iOffsetY = getAnchorY() - _iParentY;
				break;
			case 2:
				m_iAlignWRTParent = WM_ALIGN_WRT_PARENT_TOPRIGHT;
				m_iOffsetX = getAnchorX() - (_iParentX + _iParentWidth);
				m_iOffsetY = getAnchorY() - _iParentY;
				break;
			case 3:
				m_iAlignWRTParent = WM_ALIGN_WRT_PARENT_CENTERLEFT;
				m_iOffsetX = getAnchorX() - _iParentX;
				m_iOffsetY = getAnchorY() - (_iParentY + _iParentHeightHalf);
				break;
			case 4:
				m_iAlignWRTParent = WM_ALIGN_WRT_PARENT_CENTER;
				m_iOffsetX = getAnchorX() - (_iParentX + _iParentWidthHalf);
				m_iOffsetY = getAnchorY() - (_iParentY + _iParentHeightHalf);
				break;
			case 5:
				m_iAlignWRTParent = WM_ALIGN_WRT_PARENT_CENTERRIGHT;
				m_iOffsetX = getAnchorX() - (_iParentX + _iParentWidth);
				m_iOffsetY = getAnchorY() - (_iParentY + _iParentHeightHalf);
				break;
			case 6:
				m_iAlignWRTParent = WM_ALIGN_WRT_PARENT_BOTTOMLEFT;
				m_iOffsetX = getAnchorX() - _iParentX;
				m_iOffsetY = getAnchorY() - (_iParentY + _iParentHeight);
				break;
			case 7:
				m_iAlignWRTParent = WM_ALIGN_WRT_PARENT_BOTTOMCENTER;
				m_iOffsetX = getAnchorX() - (_iParentX + _iParentWidthHalf);
				m_iOffsetY = getAnchorY() - (_iParentY + _iParentHeight);
				break;
			case 8:
				m_iAlignWRTParent = WM_ALIGN_WRT_PARENT_BOTTOMRIGHT;
				m_iOffsetX = getAnchorX() - (_iParentX + _iParentWidth);
				m_iOffsetY = getAnchorY() - (_iParentY + _iParentHeight);
				break;
			}

			updateComponentPosition();
		}
	}
	else
	if(m_eCustomState == CUSTOM_STATE_MOVE) {

	}

	if(m_eCustomState != CUSTOM_STATE_NONE) {
		WWidgetManager::setCursor(IDC__ARROW);
	}
	else
#endif
	{
		// Custom Handling
		onMouseDownEx(x, y, iButton);
	}
}

void WComponent::onMouseEnter(int mCode, int x, int y, int prevX, int prevY) {
	
	// Custom Handling
	onMouseEnterEx(mCode, x, y, prevX, prevY);
}

void WComponent::onMouseHover(int mCode, int x, int y, int prevX, int prevY) {

#ifdef HAS_CUSTOM_FEATURES
	if(m_eCustomState != CUSTOM_STATE_NONE) {
		WWidgetManager::setCursor(IDC__ARROW);
	}
	else
#endif
	{
		// Custom Handling
		onMouseHoverEx(mCode, x, y, prevX, prevY);
	}
}

void WComponent::onMouseMove(int mCode, int x, int y, int prevX, int prevY) {

#ifdef HAS_CUSTOM_FEATURES
	if(m_eCustomState == CUSTOM_STATE_MOVE) {
		// Drag window around:
		int diffX = (x - prevX);
		int diffY = (y - prevY);

		if((mCode & MK_LBUTTON) != 0) {
			if(m_pParent != NULL) {
				setPosition(getOffsetX() + diffX, getOffsetY() + diffY);
			}
			else 
			if((H_WND)this == WWidgetManager::getInstance()->GetWindow(0)) {
				setLeft(diffX);
				setTop(diffY);
			}
		}
	}

	if(m_eCustomState != CUSTOM_STATE_NONE) {
		WWidgetManager::setCursor(IDC__ARROW);
	}
	else
#endif
	{
		// Custom Handling
		onMouseMoveEx(mCode, x, y, prevX, prevY);
	}
}

void WComponent::onMouseLeave(int mCode, int x, int y, int prevX, int prevY) {

	// Custom Handling
	onMouseLeaveEx(mCode, x, y, prevX, prevY);
}

void WComponent::onMouseUp(int x, int y, int iButton) {

#ifdef HAS_CUSTOM_FEATURES
	if(m_eCustomState != CUSTOM_STATE_NONE) {
		WWidgetManager::setCursor(IDC__ARROW);
	}
	else
#endif
	{
		// Custom Handling
		onMouseUpEx(x, y, iButton);
	}
}

void WComponent::onMouseWheel(WPARAM wParam, LPARAM lParam) {

	// Custom Handling
	onMouseWheelEx(wParam, lParam);
}

void WComponent::onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch) {

#ifdef HAS_CUSTOM_FEATURES
	if(isActive()) {
		if(m_bIsCustom) {
			if(GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_LWIN) && GetAsyncKeyState('O')) {
				m_eCustomState = CUSTOM_STATE_ALIGN_TO_PARENT;
			}
			else
			if(GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_LWIN) && GetAsyncKeyState('A')) {
				m_eCustomState = CUSTOM_STATE_SET_ANCHOR;
			}
			else
			if(GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_LWIN)) {
				m_eCustomState = CUSTOM_STATE_MOVE;
			}

			if(m_eCustomState != CUSTOM_STATE_NONE)
				m_StaticRenderAlignment.m_pMe = this;
		}
	}

	if(m_eCustomState == CUSTOM_STATE_NONE)
#endif
	{
		// Custom Handling
		onKeyBDownEx(iVirtualKeycode, ch);
	}
}

void WComponent::onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch) {

#ifdef HAS_CUSTOM_FEATURES
	if(m_eCustomState == CUSTOM_STATE_MOVE) {

	}
	else
#endif
	{
		// Custom Handling
		onKeyBUpEx(iVirtualKeycode, ch);
	}
}

void WComponent::onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

}

bool WComponent::isPointInside(int x, int y) {
	if(x < m_iLeft || x > m_iRight || y < m_iTop || y > m_iBottom)
		return false;

	return true;
}

void WComponent::frameRender() {
	
	if(!isVisible())
		return;

	// Render the component
	onRender();

	// Custom Handling
	postRenderEx();

	postRenderInternal();
}

void WComponent::frameUpdate(float deltaTimeMs) {
	
	if(!isVisible())
		return;

	updateComponentPosition();

	// Update the component
	onUpdate(deltaTimeMs);
}

#ifdef HAS_CUSTOM_FEATURES
void WComponent::createComponentCustomWidgets() {
	m_pMiscChildsWidget = WWidgetManager::getWidget("MiscChildsWidget");
}
#endif

LRESULT WComponent::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	return -1;
}

WComponent::~WComponent() {

}
#endif