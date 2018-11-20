
#include "Engine/UI/WSlider.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/UI/WWidgetManager.h"

WSlider::WSlider() {
}

WSlider::~WSlider() {

}

H_WND WSlider::Create(		const char* lpClassName, 
											const char* lpWindowName, 
											DWORD dwStyle, 
											int x, 
											int y, 
											int width, 
											int height, 
											H_WND hwndParent, 
											HMENU hMenu, 
											LPVOID lpVoid
) {
	WSlider* pWSlider = new WSlider();

	((WContainer*)pWSlider)->Create(	lpClassName, 
														lpWindowName, 
														dwStyle, 
														x, 
														y, 
														width, 
														height, 
														hwndParent, 
														hMenu, 
														lpVoid,
														true, 
														true);

	return pWSlider;
}

void WSlider::onCreateEx(LPVOID lpVoid) {

	H_WND hWnd = NULL;

	RectF vDestRect;
	RectF hDestRect;

	RectF wndRect;
	RectF idealRect;

	m_DummyWidget = WWidgetManager::getWidget("WSlider");

	///////////////////////////////////////////////////
	bool bHasClientArea = (m_DummyWidget->clientAreas.size() > 0);
	if(bHasClientArea) {
		CLIENT_AREA* clientArea = m_DummyWidget->getClientAreaAt(0);
		RectF destRect;
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = clientArea->posOffsets.x;
		idealRect.Y = clientArea->posOffsets.y;
		idealRect.Width = clientArea->posOffsets.w;
		idealRect.Height = clientArea->posOffsets.h;
		WWidgetManager::getDestinationRect(	destRect,
												m_DummyWidget->widgetSize.width,
												m_DummyWidget->widgetSize.height,
												&wndRect,
												&idealRect,
												clientArea->align.iHAlign,
												clientArea->align.iVAlign
												);
		m_ClientRect.X = destRect.X - getLeft();
		m_ClientRect.Y = destRect.Y - getTop();
		m_ClientRect.Width = destRect.Width;
		m_ClientRect.Height = destRect.Height;

		m_iClientRectW = m_ClientRect.Width;
		m_iClientRectH = m_ClientRect.Height;
	}
	///////////////////////////////////////////
}

void WSlider::onUpdate() {

}

void WSlider::onRender() {
	WWidgetManager* renderer =  WWidgetManager::getInstance();

	RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
			
	CHILD* cWnd = 0;
	cWnd = m_DummyWidget->getChild("TextArea");

	renderer->renderChild(m_DummyWidget, cWnd, &thisWndRect);
	//renderer->renderClientArea(m_DummyWidget, 0, &thisWndRect);
}

void WSlider::onMouseEnterEx(int mCode, int x, int y, int prevX, int prevY) {

}

void WSlider::onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY) {

}

void WSlider::onMouseLeaveEx(int mCode, int x, int y, int prevX, int prevY) {

}

void WSlider::onMouseDownEx(int x, int y, int iButton) {

}

void WSlider::onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy) {

}

void WSlider::onMouseUpEx(int x, int y, int iButton) {

}

void WSlider::onMouseWheelEx(WPARAM wParam, LPARAM lParam) {

}

void WSlider::onMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case MOUSE_DOWN:
		{
		}
		break;
		case MOUSE_UP:
		{
		}
		break;
		case MOUSE_MOVE:
		{
		}
		break;
		case MOUSE_HOVER:
		{
		}
		break;
	}
}

LRESULT WSlider::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case WM__GETTEXTLENGTH:
			{
				return strlen(getWindowName());
			}
			break;
		case WM__GETTEXT:
			{
				return sprintf_s((char*)lParam, (size_t)wParam, "%s", getWindowName());
			}
			break;
		case WM__SETTEXT:
			{
				char* str = (char*)lParam;
				setWindowName(str);
				return WM__OKAY;
			}
			break;
		case WM__GETRECT:
			{
				Rect* listRect = (Rect*)lParam;
				listRect->X = getLeft();
				listRect->Y = getTop();
				listRect->Width = getWidth();
				listRect->Height = getHeight();

				return WM__OKAY;
			}
			break;
		case WM__GETPOS:
			{
				DWORD* dwPos = (DWORD*)lParam;
				*dwPos |= (getLeft() & 0xffff);
				*dwPos |= ((getTop() & 0xffff) << 16);

				return WM__OKAY;
			}
			break;	
		case WM__SETPOS:
			{
				DWORD dwPos = (DWORD)lParam;
				setPosition((dwPos & 0xffff), ((dwPos >> 16) & 0xffff));
				return WM__OKAY;
			}
			break;
		case WM__GETSIZE:
			{
				DWORD* dwSize = (DWORD*)lParam;
				*dwSize |= (getWidth() & 0xffff);
				*dwSize |= ((getHeight() & 0xffff) << 16);

				return WM__OKAY;
			}
			break;
		case WM__SETSIZE:
			{
				DWORD dwSize = (DWORD)lParam;
				return WM__OKAY;
			}
			break;
	}
}