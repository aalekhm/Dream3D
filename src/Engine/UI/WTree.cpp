#include "Engine/UI/WTree.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/UI/WWidgetManager.h"

CHILD*	WTree::m_wRoot;
CHILD*	WTree::m_wFolderClosed;
CHILD*	WTree::m_wFolderOpen;
CHILD*	WTree::m_wFolderOpenBranch;
CHILD*	WTree::m_wFolderFirstOpenBranch;
CHILD*	WTree::m_wFolderClosedBranch;
CHILD*	WTree::m_wFolderFirstClosedBranch;
CHILD*	WTree::m_wFolderFirstChildBranch;
CHILD*	WTree::m_wFolderChildBranch;
CHILD*	WTree::m_wFolderChildLineBranch;
CHILD*	WTree::m_wFolderHalfLineBranch;
CHILD*	WTree::m_wChild1;
CHILD*	WTree::m_wChild2;

WTree::WTree() :
	m_bResizing(false),
	m_bMoving(false),
	m_bResizable(true),
	m_bLButtonDown(false),

	m_pTextField(NULL),
	m_bIsEditing(false),
	m_pEditItem(NULL)
{
	setIsContainer(true);
	m_iMainX = m_iMainY = 0;
}

WTree::~WTree() {

}

H_WND WTree::Create(		const char* lpClassName, 
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
	WTree* pWTree = new WTree();
	return pWTree->createWindow(lpClassName, lpWindowName, dwStyle, x, y, width, height, hwndParent, hMenu, lpVoid);
}

H_WND WTree::createWindow(	const char* lpClassName, 
											const char* lpWindowName, 
											DWORD dwStyle, 
											int x, 
											int y, 
											int width, 
											int height, 
											H_WND hwndParent, 
											HMENU hMenu, 
											LPVOID lpParam
) {	
		sprintf(m_pClassName, "%s", lpClassName);

		m_pParent = (WComponent*)hwndParent;

		m_iOffsetX = x;
		m_iOffsetY = y;

		if(m_pParent) {
			m_iLeft = m_pParent->getLeft() + m_iOffsetX + m_iMainX;
			m_iTop = m_pParent->getTop() + m_iOffsetY + m_iMainY;
		}

		m_iRight = m_iLeft + width;
		m_iBottom = m_iTop + height;

		m_title = lpWindowName;
		m_TreeWidget = WWidgetManager::getWidget("Tree");

		LEAF_SPACING_Y = WWidgetManager::CHARACTER_HEIGHT + (LEAF_TEXT_Y << 1);

		// custom initialization
		onCreate();

		m_pRoot = new TREEITEM();
		m_pRoot->setParent(NULL);
		m_pRoot->m_bIsLeaf = false;
		m_pRoot->m_iDepth = 0;
		m_pRoot->setName("Root...");

		m_pCopyItem = NULL;
		m_iListCounter = 0;
		m_vLeafList.insert(m_vLeafList.begin() + m_iListCounter, m_pRoot);
		m_iListCounter++;

		H_WND hWnd = 
		CreateComponent(	"WTextField", 
									"_______", 
									0, 
									0,
									0, 
									100, 
									23,
									this, 
									HMENU(111), 
									NULL);
		m_pTextField = (WTextField*)hWnd;
		m_pTextField->setVisible(false);

		addDefaultList();

		populateLeafList(m_pRoot);
		calculateDeepestLeaf();
		setPositionInTree();

		//printLeafList();
		
		setComponentID((int)hMenu);
		setComponentAsChild(true);
		((WContainer*)m_pParent)->addComponent(this);

		return this;
}

//void WTree::create(WComponent* parent, int x, int y, int w, int h, const std::string &title) {
//	setComponentAsChild(true);
//
//	m_pParent = parent;
//
//	m_iOffsetX = x;
//	m_iOffsetY = y;
//
//	if(m_pParent) {
//		m_iLeft = m_pParent->getLeft() + m_iOffsetX + m_iMainX;
//		m_iTop = m_pParent->getTop() + m_iOffsetY + m_iMainY;
//	}
//
//	m_iRight = m_iLeft + w;
//	m_iBottom = m_iTop + h;
//
//	m_title = title;
//	m_TreeWidget = WWidgetManager::getWidget("Tree");
//
//	LEAF_SPACING_Y = WWidgetManager::CHARACTER_HEIGHT + (LEAF_TEXT_Y << 1);
//
//	// custom initialization
//	onCreate();
//
//	m_pRoot = new ITEM();
//		m_pRoot->setParent(NULL);
//		m_pRoot->m_bIsLeaf = false;
//		m_pRoot->m_iDepth = 0;
//		m_pRoot->setName("Root...");
//
//		addDefaultList();
//
//	m_pCopyItem = NULL;
//	m_iListCounter = 0;
//	m_vLeafList.insert(m_vLeafList.begin() + m_iListCounter, m_pRoot);
//	m_iListCounter++;
//	populateLeafList(m_pRoot);
//	calculateDeepestLeaf();
//	setPositionInTree();
//
//	//printLeafList();
//}

void WTree::populateLeafList(TREEITEM* iLeaf) {
	
	if(iLeaf->m_bIsOpen) {
		for(int i = 0; i < iLeaf->m_vChildrens.size(); i++) {
			TREEITEM* it = (TREEITEM*)iLeaf->m_vChildrens[i];
			if(it->m_bIsLeaf) {
				m_vLeafList.insert(m_vLeafList.begin() + m_iListCounter, it);
				m_iListCounter++;
			}
			else {
				m_vLeafList.insert(m_vLeafList.begin() + m_iListCounter, it);
				m_iListCounter++;

				populateLeafList(it);
			}
		}
	}
}

void WTree::printLeafList() {
	for(int i = 0; i < m_vLeafList.size(); i++) {
		TREEITEM* item = (TREEITEM*)m_vLeafList[i];

		if(item->m_bIsLeaf)
			printf("Folder ==== %s\n", item->m_sName);
		else
			printf("Leaf ==== %s\n", item->m_sName);
	}
}

void WTree::calculateDeepestLeaf() {
	int iDeepestLeafValue = 0;
	int iStringWidthInPixel = 0;
	for(int i = 0; i < m_vLeafList.size(); i++) {
		TREEITEM* item = (TREEITEM*)m_vLeafList[i];
		
		int iLen = getStringWidthInPixels(item);
		if(iLen >= iStringWidthInPixel) {
			iStringWidthInPixel = iLen;
		}
	}
	
	m_iMaxWidthPixels = iStringWidthInPixel;
}

int WTree::getStringWidthInPixels(TREEITEM* item) {
	if(item != NULL)
		return LEAF_ROOT_X + ((item->m_iDepth + 1)*LEAF_SPACING_X) + LEAF_NAME_TEXT_X + WWidgetManager::getStringWidthTillPos(item->getName(), strlen(item->getName()));

	return 0;
}

void WTree::setPositionInTree() {
	int iPos = 0;

	for(int i = 0; i < m_vLeafList.size(); i++, iPos++) {
		TREEITEM* item = (TREEITEM*)m_vLeafList[i];
		item->setPosInTree(iPos);
	}
}

void WTree::onCreate() {

	H_WND hWnd = NULL;

	RectF vDestRect;
	RectF hDestRect;
	RectF wndRect;
	RectF idealRect;

	///////////////////////////////////////////////////
	CHILD* verticalSBChild = m_TreeWidget->getChild("VScroll");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = verticalSBChild->posOffsets.x;
		idealRect.Y = verticalSBChild->posOffsets.y;
		idealRect.Width = verticalSBChild->posOffsets.w; 
		idealRect.Height = verticalSBChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	vDestRect,
											m_TreeWidget->widgetSize.width,
											m_TreeWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											verticalSBChild->align.iHAlign,
											verticalSBChild->align.iVAlign
											);
	hWnd = 
	CreateComponent(	"WScrollbar", 
								"", 
								0, 
								vDestRect.X - m_iLeft, 
								vDestRect.Y - m_iTop,
								vDestRect.Width, 
								vDestRect.Height,
								this, 
								HMENU(ID_VERTICAL_SCROLLBAR), 
								(LPVOID)1);
	m_sbVertical = (WScrollbar*)hWnd;
	m_sbVertical->hasBG(true);
	m_sbVertical->setPostRender(true);
	m_iMaxVScrollbarHeight = vDestRect.Height;
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	CHILD* horizontalSBChild = m_TreeWidget->getChild("HScroll");
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = horizontalSBChild->posOffsets.x;
		idealRect.Y = horizontalSBChild->posOffsets.y;
		idealRect.Width = horizontalSBChild->posOffsets.w; 
		idealRect.Height = horizontalSBChild->posOffsets.h;
	WWidgetManager::getDestinationRect(	hDestRect,
											m_TreeWidget->widgetSize.width,
											m_TreeWidget->widgetSize.height,
											&wndRect,
											&idealRect,
											horizontalSBChild->align.iHAlign,
											horizontalSBChild->align.iVAlign
											);
	hWnd = 
	CreateComponent(	"WScrollbar", 
								"", 
								0, 
								hDestRect.X - m_iLeft, 
								hDestRect.Y - m_iTop,
								hDestRect.Width, 
								hDestRect.Height,
								this, 
								HMENU(ID_HORIZONTAL_SCROLLBAR), 
								(LPVOID)0);
	m_sbHorizontal = (WScrollbar*)hWnd;
	m_sbHorizontal->hasBG(true);
	m_sbHorizontal->setPostRender(true);
	m_iMaxHScrollbarWidth = hDestRect.Width;
	///////////////////////////////////////////////////

	bool bHasClientArea = (m_TreeWidget->clientAreas.size() > 0);
	if(bHasClientArea) {
		CLIENT_AREA* clientArea = m_TreeWidget->getClientAreaAt(0);
		RectF destRect;
		wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight();
		idealRect.X = clientArea->posOffsets.x;
		idealRect.Y = clientArea->posOffsets.y;
		idealRect.Width = clientArea->posOffsets.w; 
		idealRect.Height = clientArea->posOffsets.h;
		WWidgetManager::getDestinationRect(	destRect,
												m_TreeWidget->widgetSize.width,
												m_TreeWidget->widgetSize.height,
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

	m_IsVScrolling = m_IsHScrolling = false;
	m_minX = getLeft() + m_ClientRect.X;
	m_maxX = m_minX + m_ClientRect.Width;
	m_minY = getTop() + m_ClientRect.Y;
	m_maxY = m_minY + m_ClientRect.Height;

	m_wRoot = m_TreeWidget->getChild("Tree_Root");
	m_wFolderClosed = m_TreeWidget->getChild("Tree_FolderClosed");
	m_wFolderOpen = m_TreeWidget->getChild("Tree_FolderOpen");
	m_wFolderOpenBranch = m_TreeWidget->getChild("Tree_FolderOpenBranch");
	m_wFolderFirstOpenBranch = m_TreeWidget->getChild("Tree_FolderFirstOpenBranch");
	m_wFolderClosedBranch = m_TreeWidget->getChild("Tree_FolderClosedBranch");
	m_wFolderFirstClosedBranch = m_TreeWidget->getChild("Tree_FolderFirstClosedBranch");
	m_wFolderFirstChildBranch = m_TreeWidget->getChild("Tree_FirstChildBranch");
	m_wFolderChildBranch = m_TreeWidget->getChild("Tree_ChildBranch");
	m_wFolderChildLineBranch = m_TreeWidget->getChild("Tree_ChildLineBranch");
	m_wFolderHalfLineBranch = m_TreeWidget->getChild("Tree_FolderHalfLineBranch");
	m_wChild1 = m_TreeWidget->getChild("Tree_Child1");
	m_wChild2 = m_TreeWidget->getChild("Tree_Child2");

	///////////////////////////////////////////
}

void WTree::onUpdate() {
	updateComponentPosition();

	if(!m_IsVScrolling)
		updateVBarPosition();
	if(!m_IsHScrolling)
		updateHBarPosition();

	m_iMaxHeightPixels = (m_vLeafList.size() + 1)*LEAF_SPACING_Y;

	setVScrollbarLength();
	setHScrollbarLength();

	bool bVertSBVisibility = (m_iMaxHeightPixels > m_ClientRect.Height);
	m_sbVertical->setDisable(bVertSBVisibility);
	if(!bVertSBVisibility) {
		m_ClientRect.Width = m_iClientRectW + m_sbVertical->getWidth();
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth + m_sbVertical->getWidth());
	}
	else {
		m_ClientRect.Width = m_iClientRectW;
		m_sbHorizontal->setWidth(m_iMaxHScrollbarWidth);
	}

	bool bHoriSBVisibility = (m_iMaxWidthPixels > m_ClientRect.Width);
	m_sbHorizontal->setDisable(bHoriSBVisibility);
	if(!bHoriSBVisibility) {
		m_ClientRect.Height = m_iClientRectH + m_sbHorizontal->getHeight();
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight + m_sbHorizontal->getHeight());
	}
	else {
		m_ClientRect.Height = m_iClientRectH;
		m_sbVertical->setHeight(m_iMaxVScrollbarHeight);
	}
}

void WTree::setVScrollbarLength() {
	float _part = m_ClientRect.Height;
	float _total = m_iMaxHeightPixels;

	if(_total > 0) {
		float _percentage = (_part / _total) * 100;

		if(_percentage <= 100) {
			m_sbVertical->setLength(_percentage);
		}
	}
}

void WTree::setHScrollbarLength() {
	float _part = m_ClientRect.Width;
	float _total = m_iMaxWidthPixels;
	float _percentage = (_part / _total) * 100;

	if(_percentage <= 100)
		m_sbHorizontal->setLength(_percentage);
}

void WTree::updateVBarPosition() {
	float _part = abs(m_iMainY);
	float _total = m_iMaxHeightPixels;
	float _percentage = (_part / _total) * 100;

	m_sbVertical->setCursorPositionInPercent(_percentage);
}

void WTree::updateHBarPosition() {
	float _part = abs(m_iMainX);
	float _total = m_iMaxWidthPixels;
	float _percentage = (_part / _total) * 100;

	m_sbHorizontal->setCursorPositionInPercent(_percentage);
}

void WTree::updateMains() {
	//////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////

	int yPosTop = m_minY + m_iMainY + ( LEAF_ROOT_Y + m_iSelection*LEAF_SPACING_Y);
	if(yPosTop < m_minY) {
		m_iMainY = -( m_iSelection*LEAF_SPACING_Y);
	}

	int yPosBottom = m_minY + m_iMainY + (LEAF_ROOT_Y + m_iSelection*LEAF_SPACING_Y + LEAF_SPACING_Y);
	if(yPosBottom >= m_maxY) {
		m_iMainY -= ((yPosBottom - m_maxY) + LEAF_SPACING_Y);
		m_iMainY -= (m_iMainY % LEAF_SPACING_Y);
	}

	if(m_iMainY > 0)
		m_iMainY = 0;
	//////////////////////////////////////////////////////////////
}

void WTree::onRender() {
	WWidgetManager* pRenderer =  WWidgetManager::getInstance();

	RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
		
	CHILD* cWnd = m_TreeWidget->getChild("TextArea");
	pRenderer->renderChild(m_TreeWidget, cWnd, &thisWndRect);
	//pRenderer->renderClientArea(m_TreeWidget, 0, &thisWndRect);

	///////////////////////////////////////////////////
	{
		RectF reclaimRect;
		WWidgetManager::GetClipBounds(&reclaimRect);

		RectF clipRect(	getLeft() + m_ClientRect.X,
								getTop() + m_ClientRect.Y,
								m_ClientRect.Width,
								m_ClientRect.Height);
		RectF::Intersect(clipRect, reclaimRect, clipRect);
		WWidgetManager::setClip(clipRect.X, clipRect.Y, clipRect.Width, clipRect.Height);

		renderTree();
		WWidgetManager::setClip(reclaimRect.X, reclaimRect.Y, reclaimRect.Width, reclaimRect.Height);
	}
	///////////////////////////////////////////////////
}

void WTree::startEditing(TREEITEM* item) {
	m_bIsEditing = true;

	item->setEditing(true);
	m_pEditItem = item;

	int itemNameLength = strlen(item->getName());
	m_pTextField->getFocus();
	m_pTextField->setVisible(true);
	m_pTextField->setText(item->getName());

	int iWidth = WWidgetManager::getStringWidthTillPos(item->getName(), itemNameLength);
	m_pTextField->setWidth(iWidth + LEAF_SPACING_X - LEAF_NAME_TEXT_X);

	DWORD dwRange = 0;
	dwRange |= (0 & 0xffff);
	dwRange |= ((itemNameLength & 0xffff) << 16);
	m_pTextField->setSelection(dwRange);
}

void WTree::stopEditing() {
	m_bIsEditing = false;
	m_pEditItem->setEditing(false);
	m_pEditItem->setName(m_pTextField->getText());
	m_pEditItem = NULL;
	m_pTextField->setVisible(false);

	calculateDeepestLeaf();
}

void WTree::repositionEditField() {

	TREEITEM* item = m_vLeafList[m_iSelection];

	int leafX = LEAF_ROOT_X + item->m_iDepth*LEAF_SPACING_X + LEAF_SPACING_X;
	int leafY = LEAF_ROOT_Y + m_iSelection*LEAF_SPACING_Y + LEAF_TEXT_Y;

	m_pTextField->setPosition(leafX, leafY);
}

void WTree::onMouseDownEx(int x, int y, int iButton) {
	m_bLButtonDown = true;
	m_IsVScrolling = m_IsHScrolling = false;

	int itemIdx = (y - getTop() - m_iMainY - LEAF_ROOT_Y) / LEAF_SPACING_Y;
	int maxTreeSize = m_vLeafList.size();

	if(itemIdx < maxTreeSize) {
		if(m_iSelection != itemIdx) {
			m_iSelection = itemIdx;

			if(m_bIsEditing) {
				stopEditing();
			}

			repositionEditField();
		}
		else {
			startEditing(m_vLeafList[itemIdx]);
			//if(!collapseNode(m_iSelection))
			//	expandNode(m_iSelection);
		}
	}
}

void WTree::onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy) {
	if(m_IsVScrolling || m_IsHScrolling)
		return;

	if (isMovable() && m_bLButtonDown) {

		if (x-prevx!=0 || y-prevy!=0)
			m_bMoving = true;

		// Drag window around:
		int diffX = (x - prevx);
		int diffY = (y - prevy);

		setPosition(getOffsetX() + diffX, getOffsetY() + diffY);
	}
}

void WTree::onMouseUpEx(int x, int y, int iButton) {
	// stop dragging or
	// ..stop resizing
	m_bResizing = false;
	m_bMoving = false;
	m_bLButtonDown = false;
}

void WTree::onMouseWheelEx(WPARAM wParam, LPARAM lParam){
	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	if(zDelta < 0) {
		m_iMainY -= LEAF_SPACING_Y;
		if(abs(m_iMainY) > (m_iMaxHeightPixels - m_ClientRect.Height))
			m_iMainY = -(m_iMaxHeightPixels - m_ClientRect.Height);
	}
	else {
		m_iMainY += LEAF_SPACING_Y;
		if(m_iMainY > 0)
			m_iMainY = 0;
	}
}

void WTree::onMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case MOUSE_DOWN:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_VERTICAL_SCROLLBAR:
					switch(lParam) {
						case BTN_SCROLLBAR_UP:
						{
							m_iMainY += LEAF_SPACING_Y;
							if(m_iMainY > 0)
								m_iMainY = 0;
						}
						break;
						case BTN_SCROLLBAR_DOWN:
						{
							m_iMainY -= LEAF_SPACING_Y;
							if(abs(m_iMainY) > (m_iMaxHeightPixels - m_ClientRect.Height))
								m_iMainY = -(m_iMaxHeightPixels - m_ClientRect.Height);
						}
						break;
					}
					break;
				case ID_HORIZONTAL_SCROLLBAR:
					switch(lParam) {
						case BTN_SCROLLBAR_LEFT:
						{
							m_iMainX += 25;
							if(m_iMainX > 0)
								m_iMainX = 0;

							m_IsHScrolling = true;
						}
						break;
						case BTN_SCROLLBAR_RIGHT:
						{
							m_iMainX -= 25;
							if(abs(m_iMainX) > (m_iMaxWidthPixels - m_ClientRect.Width))
								m_iMainX = -(m_iMaxWidthPixels - m_ClientRect.Width);

							m_IsHScrolling = true;
						}
						break;
				}
				break;
			}
		}
		break;
		case MOUSE_UP:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_VERTICAL_SCROLLBAR:
				break;
				case ID_HORIZONTAL_SCROLLBAR:
				break;
			}
		}
		break;
		case MOUSE_MOVE:
		{
			int buttonID = wParam;
			switch(wParam) {
				case ID_VERTICAL_SCROLLBAR:
				break;
				case ID_HORIZONTAL_SCROLLBAR:
				break;
			}
		}
		break;
		case SCROLLER_POS_ON_DRAG:
		{
			int cursorPosInPercentage = (int)lParam;
			switch(wParam) {
				case ID_VERTICAL_SCROLLBAR:
				{
					float scrollMaterialHeight = (m_iMaxHeightPixels - m_ClientRect.Height);
					int mainYValue = (cursorPosInPercentage*scrollMaterialHeight)/100;
					m_iMainY = -mainYValue;

					m_IsVScrolling = true;
				}
				break;
				case ID_HORIZONTAL_SCROLLBAR:
				{
					float scrollMaterialWidth = (m_iMaxWidthPixels - m_ClientRect.Width);
					int mainXValue = (cursorPosInPercentage*scrollMaterialWidth)/100;
					m_iMainX = -mainXValue;

					m_IsHScrolling = true;
				}
				break;
			}
		}
		break;
	}
}

void WTree::renderTree() {
	WWidgetManager* pRenderer = WWidgetManager::getInstance();

	int iLeafYCount = 0;
	RectF dummyRect(0, 0, 0, 0);

	int leafX = 0, leafY = 0;
	int leafStartX = getLeft() + m_iMainX + LEAF_ROOT_X;
	int leafStartY = getTop() + m_iMainY + LEAF_ROOT_Y;
	int maxRenderY = getTop() + m_ClientRect.Y + m_ClientRect.Height;

	RectF thisWndRect(getLeft(), getTop(), getWidth(), getHeight());
	leafY = leafStartY + m_iSelection*LEAF_SPACING_Y - 2;
	
	int iStartNode = abs(m_iMainY)/LEAF_SPACING_Y;
	int maxLinesPerPage = m_iClientRectH/LEAF_SPACING_Y;
	for(int i = iStartNode, iLeafYCount = iStartNode; i < m_vLeafList.size(); i++, iLeafYCount++) {
		if(i > iStartNode + maxLinesPerPage)
			break;

		TREEITEM* it = (TREEITEM*)m_vLeafList[i];

		leafX = leafStartX + it->m_iDepth*LEAF_SPACING_X;
		leafY = leafStartY + iLeafYCount*LEAF_SPACING_Y + LEAF_TEXT_Y;

		if(leafY > maxRenderY)
			break;

		///////////////////////////// Render the leaves
		CHILD* leafImg = 0;
		if(it->m_bIsLeaf) { // leaves
			leafImg = m_wChild1;
		}
		else {
			if(it->m_iDepth == 0) { // Is a Root
				iLeafYCount = 0;
				leafImg = m_wRoot;
			}
			else { // Other Folders
				leafImg = (it->m_bIsOpen)?m_wFolderOpen:m_wFolderClosed;
			}
		}

		// 'Center Align' the icon with the ROW.
		int iLeafIconPosY = leafStartY + iLeafYCount*LEAF_SPACING_Y + (LEAF_SPACING_Y - leafImg->posOffsets.h)/2;
		pRenderer->renderChild(	m_TreeWidget,
												leafImg,
												&dummyRect,
												leafX, 
												iLeafIconPosY);

		////////////// SELECTION
		if(m_iSelection == i) {
			Rect fRect(	leafX - LEAF_SELECTION_ADD_X, 
								leafY, 
								LEAF_SPACING_X + LEAF_NAME_TEXT_X + WWidgetManager::getStringWidthTillPos(it->getName(), strlen(it->getName())) + (LEAF_SELECTION_ADD_X << 1), 
								m_pTextField->getHeight() - LEAF_TEXT_Y);
			WWidgetManager::getInstance()->fillRect(0.0f, 0.0f, 255.0f, 32.0f, &fRect);
			WWidgetManager::getInstance()->drawRect(0.0f, 0.0f, 0.0f, 128.0f, &fRect, 1);
		}
		/////////////////////////////////////////

		if(it->isEditing()) {
			m_pTextField->setPosition(leafX + LEAF_SPACING_X - getLeft() - m_iMainX, leafY - getTop() - m_iMainY);
		}
		else {
			int ileafY = getTop() + m_iMainY + LEAF_ROOT_Y + iLeafYCount*LEAF_SPACING_Y + (LEAF_TEXT_Y << 1);
			pRenderer->drawStringFont(it->m_sName, leafX + LEAF_SPACING_X + LEAF_NAME_TEXT_X, ileafY, 0);
		}

		if(it->m_iDepth == 0) continue;//For Root-Node
		/////////////////////////////////////////////////////////////////////
		///////////////////////////// Render the branches
		if(it->m_bIsLeaf) {
			leafImg = (it->m_bIsFirstChild)?m_wFolderFirstChildBranch:m_wFolderChildBranch;
		}
		else {
			if(it->m_bIsFirstChild) {
				leafImg = (it->m_bIsOpen)?m_wFolderFirstOpenBranch:m_wFolderFirstClosedBranch;
			}
			else {
				leafImg = (it->m_bIsOpen)?m_wFolderOpenBranch:m_wFolderClosedBranch;
			}
		}
		
		//Render the actual Node
		pRenderer->renderChild(	m_TreeWidget,
												leafImg,
												&dummyRect,
												leafX, 
												iLeafIconPosY);

		if(!it->m_bIsLeaf && it->m_bIsOpen) {
			int openLeaves = getExpandedChildCount(it);
			
			if(openLeaves > 0) {//Render the Lines.
				pRenderer->renderChild(	m_TreeWidget,
														m_wFolderHalfLineBranch,
														&dummyRect,
														leafX,
														leafY);

				for(int j = 1; j < openLeaves + 1; j++) {
					pRenderer->renderChild(	m_TreeWidget,
												m_wFolderChildLineBranch,
												&dummyRect,
												leafX,
												leafY + j*LEAF_SPACING_Y);
				}
			}
		}
		//*/
		/////////////////////////////////////////////////////////////////////
	}
	
	if(!true) {
		Rect fRect(thisWndRect.X, leafY, thisWndRect.Width, LEAF_SPACING_Y);

		fRect.X = thisWndRect.X;
		fRect.Y = thisWndRect.Y;
		fRect.Width = thisWndRect.Width;
		fRect.Height = 40;

		WWidgetManager::getInstance()->fillRect(255.0f, 255.0f, 255.0f, 255.0f, &fRect);
		char* str = new char[255];
		memset(str, 0, 255);
		sprintf(str, "sn=%d/%d, %d", iStartNode, iStartNode + maxLinesPerPage, m_vLeafList.size());
		pRenderer->drawStringFont(str, thisWndRect.X+5, thisWndRect.Y+5, 0);
		
		delete[] str;
	}
}

int WTree::getExpandedChildCount(TREEITEM* item) {
	int retVal = 0;
	
	if(!item->m_bIsLeaf) {
		for(int i = 0; i < item->m_vChildrens.size(); i++) {
			TREEITEM* childItem = (TREEITEM*)item->m_vChildrens[i];

			if(childItem->m_bIsLeaf)
				retVal++;
			else {
				if(childItem->m_bIsOpen)
					retVal += getExpandedChildCount(childItem) + 1;
				else
					retVal++;
			}
		}
	}

	return retVal;
}

void WTree::onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch) {
	
	if(m_bIsEditing) {
		if(iVirtualKeycode == VK_RETURN) {
			stopEditing();
		}
		else
			m_pTextField->onKeyBDown(iVirtualKeycode, ch);

		return;
	}

	switch(iVirtualKeycode) {
		case VK_F2:
		{
			startEditing(m_vLeafList[m_iSelection]);
		}
		break;
		case VK_HOME:
		{
			m_iSelection = 0;
			updateMains();
			repositionEditField();
		}
		break;
		case VK_END:
		{
			m_iSelection = m_vLeafList.size() - 1;
			updateMains();
			repositionEditField();
		}
		break;
		case VK_PRIOR:
		{
			int maxLinesPerPage = m_iClientRectH/LEAF_SPACING_Y;
			m_iSelection -= maxLinesPerPage;
			if(m_iSelection < 0) m_iSelection = 0;

			updateMains();
			repositionEditField();
		}
		break;
		case VK_NEXT:
		{
			int maxLinesPerPage = m_iClientRectH/LEAF_SPACING_Y;
			m_iSelection += maxLinesPerPage;
			int maxTreeSize = m_vLeafList.size();
			if(m_iSelection >= maxTreeSize) m_iSelection = maxTreeSize-1;

			updateMains();
			repositionEditField();
		}
		break;
		case VK_UP:
		{
			m_iSelection--;
			if(m_iSelection < 0) m_iSelection = 0;
			
			updateMains();
			repositionEditField();
		}
		break;
		case VK_DOWN:
		{
			m_iSelection++;
			int maxTreeSize = m_vLeafList.size();
			if(m_iSelection >= maxTreeSize) m_iSelection = maxTreeSize-1;
			
			updateMains();
			repositionEditField();
		}
		break;
		case VK_LEFT://Collapse Node
		{
			TREEITEM* item = (TREEITEM*)m_vLeafList[m_iSelection];
			bool bSetSelection = false;
			if(item->m_bIsLeaf)
				bSetSelection = true;
			else {
				if(item->m_bIsOpen)
					collapseNode(m_iSelection);
				else
					bSetSelection = true;
			}

			if(bSetSelection) {
				TREEITEM* parentNode = item->getParent();
				if(parentNode == NULL)
					m_iSelection = 0;
				else {
					m_iSelection = parentNode->getPosInTree();
				}
			}

			updateMains();
		}
		break;
		case VK_RIGHT://Expand Node
		{
			expandNode(m_iSelection);
		}
		break;
		case VK_DELETE:
		{
			TREEITEM* item = (TREEITEM*)m_vLeafList[m_iSelection];
			deleteNode(item);
		}
		break;
		case 'X':
		{
			TREEITEM* item = (TREEITEM*)m_vLeafList[m_iSelection];
			//if(m_pCopyItem != NULL)//TODO
			//	m_pCopyItem->deleteLeaf(false);
			
			m_pCopyItem = new TREEITEM(*item);
			deleteNode(item);
		}
		break;
		case 'C':
		{
			if(GetAsyncKeyState(VK_CONTROL)) {
				TREEITEM* item = (TREEITEM*)m_vLeafList[m_iSelection];
				//if(m_pCopyItem != NULL)//TODO
				//	m_pCopyItem->deleteLeaf(false);

				m_pCopyItem = new TREEITEM(*item);

			}
		}
		break;
		case 'V':
		{
			if(GetAsyncKeyState(VK_CONTROL)) {
				if(m_pCopyItem != NULL) {
					m_pCopyItem = new TREEITEM(*m_pCopyItem);

					TREEITEM* selItem = ((TREEITEM*)m_vLeafList[m_iSelection]);
					TREEITEM* selItemParent = selItem->getParent();

					if(selItem->m_bIsLeaf) {
						collapseNode(selItemParent->getPosInTree());

						m_pCopyItem->m_pParent = selItemParent;
						selItemParent->addLeaf(m_pCopyItem);
					}
					else {
						collapseNode(selItem->getPosInTree());

						m_pCopyItem->m_pParent = selItem;
						selItem->addLeaf(m_pCopyItem);
					}

					expandNode(m_pCopyItem->m_pParent->getPosInTree());
					m_iSelection = m_pCopyItem->getPosInTree();
					updateMains();
				}
			}
		}
		break;
	}
}

void WTree::deleteNode(TREEITEM* item) {
	if(!item->m_bIsLeaf) {//If its a ParentNode(& open), collapse(close) it first & then delete it.
		collapseNode(m_iSelection);
	}

	m_vLeafList.erase(m_vLeafList.begin() + m_iSelection);
	item->deleteLeaf(true);
}

void WTree::expandNode(int itemIdx) {
	TREEITEM* item = (TREEITEM*)m_vLeafList[itemIdx];
	if(!item->m_bIsLeaf) {
		if(!item->m_bIsOpen) {
			item->m_bIsOpen = true;

			m_iListCounter = itemIdx + 1;
			populateLeafList(item);
		}
	}
	
	calculateDeepestLeaf();
	setPositionInTree();
}

bool WTree::collapseNode(int itemIdx) {
	TREEITEM* item = (TREEITEM*)m_vLeafList[itemIdx];

	if(!item->m_bIsLeaf) {
		if(item->m_bIsOpen) {
			int openLeavesCount = getExpandedChildCount(item);
			m_vLeafList.erase(m_vLeafList.begin() + itemIdx + 1, m_vLeafList.begin() + itemIdx + 1 + openLeavesCount);

			item->m_bIsOpen = false;
			
			calculateDeepestLeaf();
			setPositionInTree();

			return true;
		}
	}

	return false;
}

void WTree::addDefaultList() {
	/////////////////////
	TREEITEM* it = new TREEITEM();
	it->setParent(m_pRoot);
	it->m_bIsLeaf = true;
	it->setName("Leaf 0");
	m_pRoot->addLeaf(it);

	it = new TREEITEM();
	it->setParent(m_pRoot);
	it->m_bIsLeaf = true;
	it->setName("Leaf 1");
	m_pRoot->addLeaf(it);

	it = new TREEITEM();
	it->setParent(m_pRoot);
	it->m_bIsLeaf = false;
	it->setName("Leaf 2");
	m_pRoot->addLeaf(it);
	{
		TREEITEM* itc = 0;

		itc = new TREEITEM();
		itc->setParent(it);
		itc->m_bIsLeaf = false;
		itc->setName("Leaf 4");
		itc->m_bIsOpen = false;
		it->addLeaf(itc);
		{
			TREEITEM* itc1 = new TREEITEM();
			itc1->setParent(itc);
			itc1->m_bIsLeaf = true;
			itc1->setName("Leaf 41");
			itc->addLeaf(itc1);

			itc1 = new TREEITEM();
			itc1->setParent(itc);
			itc1->m_bIsLeaf = true;
			itc1->setName("Leaf 42");
			itc->addLeaf(itc1);

			itc1 = new TREEITEM();
			itc1->setParent(itc);
			itc1->m_bIsLeaf = true;
			itc1->setName("Leaf 43");
			itc->addLeaf(itc1);

			itc1 = new TREEITEM();
			itc1->setParent(itc);
			itc1->m_bIsLeaf = true;
			itc1->setName("Leaf 44");
			itc->addLeaf(itc1);
		}

		itc = new TREEITEM();
		itc->setParent(it);
		itc->m_bIsLeaf = true;
		itc->setName("Leaf 30");
		it->addLeaf(itc);

		itc = new TREEITEM();
		itc->setParent(it);
		itc->m_bIsLeaf = true;
		itc->setName("Leaf 3");
		it->addLeaf(itc);

		itc = new TREEITEM();
		itc->setParent(it);
		itc->m_bIsLeaf = true;
		itc->setName("Leaf 311");
		it->addLeaf(itc);

		itc = new TREEITEM();
		itc->setParent(it);
		itc->m_bIsLeaf = true;
		itc->setName("Leaf 5");
		it->addLeaf(itc);

		itc = new TREEITEM();
		itc->setParent(it);
		itc->m_bIsLeaf = false;
		itc->setName("Leaf 90");
		it->addLeaf(itc);
		{
			TREEITEM* itc1 = 0;

			itc1 = new TREEITEM();
			itc1->setParent(itc);
			itc1->m_bIsLeaf = false;
			itc1->setName("Leaf 900");
			itc->addLeaf(itc1);
			{
				TREEITEM* itc2 = new TREEITEM();
				itc2->setParent(itc1);
				itc2->m_bIsLeaf = true;
				itc2->setName("Leaf Leaf Leaf 9001");
				itc1->addLeaf(itc2);

				itc2 = new TREEITEM();
				itc2->setParent(itc1);
				itc2->m_bIsLeaf = true;
				itc2->setName("Leaf Leaf Leaf 9002");
				itc1->addLeaf(itc2);

				itc2 = new TREEITEM();
				itc2->setParent(itc1);
				itc2->m_bIsLeaf = false;
				itc2->setName("Leaf 9003--------");
				itc1->addLeaf(itc2);
				{
					TREEITEM* itc3 = new TREEITEM();
					itc3->setParent(itc2);
					itc3->m_bIsLeaf = true;
					itc3->setName("Leaf Leaf Leaf 900111111");
					itc2->addLeaf(itc3);

					itc3 = new TREEITEM();
					itc3->setParent(itc2);
					itc3->m_bIsLeaf = true;
					itc3->setName("Leaf Leaf Leaf 90021111111AA A.");
					itc2->addLeaf(itc3);

					itc3 = new TREEITEM();
					itc3->setParent(itc2);
					itc3->m_bIsLeaf = true;
					itc3->setName("Leaf 900311111");
					itc2->addLeaf(itc3);
				}
			}

			itc1 = new TREEITEM();
			itc1->setParent(itc);
			itc1->m_bIsLeaf = true;
			itc1->setName("Leaf 901");
			itc->addLeaf(itc1);

			itc1 = new TREEITEM();
			itc1->setParent(itc);
			itc1->m_bIsLeaf = true;
			itc1->setName("Leaf 902");
			itc->addLeaf(itc1);
		}

		itc = new TREEITEM();
		itc->setParent(it);
		itc->m_bIsLeaf = true;
		itc->setName("Leaf 6");
		it->addLeaf(itc);

		itc = new TREEITEM();
		itc->setParent(it);
		itc->m_bIsLeaf = true;
		itc->setName("Leaf 7");
		it->addLeaf(itc);

		itc = new TREEITEM();
		itc->setParent(it);
		itc->m_bIsLeaf = true;
		itc->setName("Leaf 8");
		it->addLeaf(itc);

		TREEITEM* itc2 = new TREEITEM();
		itc2->setParent(it);
		itc2->m_bIsLeaf = false;
		itc2->setName("Leaf 8999999");
		it->addLeaf(itc2);
		{
			TREEITEM* itc1 = 0;

			itc1 = new TREEITEM();
			itc1->setParent(itc2);
			itc1->m_bIsLeaf = false;
			itc1->setName("Leaf 900");
			itc2->addLeaf(itc1);
			{
				TREEITEM* itc2 = new TREEITEM();
				itc2->setParent(itc1);
				itc2->m_bIsLeaf = true;
				itc2->setName("Leaf Leaf Leaf 9001");
				itc1->addLeaf(itc2);

				itc2 = new TREEITEM();
				itc2->setParent(itc1);
				itc2->m_bIsLeaf = true;
				itc2->setName("Leaf Leaf Leaf 9002 Leaf Leaf Leaf 90021111111AA A");
				itc1->addLeaf(itc2);

				itc2 = new TREEITEM();
				itc2->setParent(itc1);
				itc2->m_bIsLeaf = true;
				itc2->setName("Leaf 9003");
				itc1->addLeaf(itc2);
			}

			itc1 = new TREEITEM();
			itc1->setParent(itc2);
			itc1->m_bIsLeaf = true;
			itc1->setName("Leaf 901");
			itc2->addLeaf(itc1);

			itc1 = new TREEITEM();
			itc1->setParent(itc2);
			itc1->m_bIsLeaf = true;
			itc1->setName("Leaf 902");
			itc2->addLeaf(itc1);
		}

		itc = new TREEITEM();
		itc->setParent(it);
		itc->m_bIsLeaf = true;
		itc->setName("Leaf 9");
		it->addLeaf(itc);
	}
	it = new TREEITEM();
	it->setParent(m_pRoot);
	it->m_bIsLeaf = true;
	it->setName("Leaf 10");
	m_pRoot->addLeaf(it);
	/////////////////////
}

LRESULT WTree::OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case TVM__GETCOUNT:
		{
			return m_vLeafList.size();
		}
		break;
		case TVM__SELECTITEM:
		{
			return WM__OKAY;
		}
		break;
	}

	return WM__ERR;
}