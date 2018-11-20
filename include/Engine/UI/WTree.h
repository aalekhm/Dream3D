#ifdef USE_YAGUI
#ifndef WTREE_H
#define WTREE_H

#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WScrollbar.h"
#include "Engine/UI/WTextField.h"

#define LEAF_SPACING_X					20
#define LEAF_TEXT_Y						2
#define LEAF_ROOT_X						10
#define LEAF_ROOT_Y						10
#define LEAF_NAME_TEXT_X				5
#define LEAF_SELECTION_ADD_X		5

struct WTree : public WContainer {
	private:
		std::string			m_title;

		bool					m_bResizing;
		bool					m_bMoving;
		bool					m_bResizable;
		bool					m_bLButtonDown;

		WIDGET*			m_TreeWidget;
		WScrollbar*		m_sbVertical;
		WScrollbar*		m_sbHorizontal;
		WTextField*		m_pTextField;
		
		enum LEAF_TYPE {
			LEAF_TYPE_ROOT = 0,
			LEAF_TYPE_FOLDER_CLOSED,
			LEAF_TYPE_FOLDER_OPEN,
			LEAF_TYPE_FOLDER_CLOSED_BRANCH,
			LEAF_TYPE_FOLDER_OPEN_BRANCH,
			LEAF_TYPE_LEAF_FIRST_CHILD_BRANCH,
			LEAF_TYPE_LEAF_CHILD_BRANCH,
			LEAF_TYPE_CHILD1,
			LEAF_TYPE_CHILD2
		};

		static CHILD*	m_wRoot;
		static CHILD*	m_wFolderClosed;
		static CHILD*	m_wFolderOpen;
		static CHILD*	m_wFolderClosedBranch;
		static CHILD*	m_wFolderOpenBranch;
		static CHILD*	m_wFolderFirstClosedBranch;
		static CHILD*	m_wFolderFirstOpenBranch;
		static CHILD*	m_wFolderFirstChildBranch;
		static CHILD*	m_wFolderChildBranch;
		static CHILD*	m_wFolderChildLineBranch;
		static CHILD*	m_wFolderHalfLineBranch;
		static CHILD*	m_wChild1;
		static CHILD*	m_wChild2;

		int				m_ileafY;

struct TREEITEM {
	public:
		TREEITEM*					m_pParent;
		bool								m_bIsLeaf;
		bool								m_bIsOpen;
		bool								m_bIsFirstChild;
		short								m_iDepth;
		char*							m_sName;
		int								m_iPositionInTree;
		std::vector<TREEITEM*>	m_vChildrens;
		bool								m_bIsEditing;

		TREEITEM() 
			:	m_bIsOpen(true),
				m_bIsLeaf(false),
				m_bIsEditing(false),
				m_bIsFirstChild(0),
				m_iDepth(0),
				m_iPositionInTree(0),
				m_pParent(NULL)
		{
			m_sName = new char[255];
			memset(m_sName, 0, 255);
		}

		TREEITEM(TREEITEM* item) {
			copy(item);
		}

		TREEITEM(const TREEITEM& item) {
			copy((TREEITEM*)&item);
		}

		TREEITEM& operator=(const TREEITEM& item)
		{
			copy((TREEITEM*)&item);

			return *this;
		}

		void copy(TREEITEM* cloneMe) {
			m_bIsLeaf = cloneMe->m_bIsLeaf;
			m_bIsOpen = cloneMe->m_bIsOpen;
			m_bIsFirstChild = cloneMe->m_bIsFirstChild;
			m_iDepth = cloneMe->m_iDepth;
			m_iPositionInTree = cloneMe->m_iPositionInTree;
			m_vChildrens = std::vector<TREEITEM*>(cloneMe->m_vChildrens.size());
			m_pParent = cloneMe->m_pParent;

			m_sName = new char[255];
			memset(m_sName, 0, 255);
			setName(cloneMe->m_sName);
			
			if(!cloneMe->m_bIsLeaf) {
				//http://stackoverflow.com/questions/8865632/c-how-to-correctly-copy-stdvectorclass-in-copy-constructor
				for (int i = 0; i < cloneMe->m_vChildrens.size(); ++i) {
					m_vChildrens[i] = new TREEITEM(*cloneMe->m_vChildrens[i]);

					TREEITEM* thisItem = (TREEITEM*)m_vChildrens[i];
					thisItem->m_pParent = this;
				}
			}
		}

		TREEITEM* getParent() {
			return m_pParent;
		}

		void setParent(TREEITEM* parent) {
			m_pParent = parent;
		}

		~TREEITEM() {
			if(m_sName)
				delete[] m_sName;
			m_vChildrens.clear();
		}

		void setPosInTree(int pos) {
			m_iPositionInTree = pos;
		}

		const char* getName() {
			return m_sName;
		}

		void setName(const char* str) {
			memset(m_sName, 0, 255);
			sprintf(m_sName, "%s\0", str);
		}

		bool isEditing() {
			return m_bIsEditing;
		}

		void setEditing(bool bEdit) {
			m_bIsEditing = bEdit;
		}

		int getPosInTree() {
			return m_iPositionInTree;
		}

		void addLeaf(TREEITEM* item) {
			if(item) {
				if(m_vChildrens.size() <= 0)
					item->m_bIsFirstChild = true;

				m_vChildrens.push_back(item);
				
				item->m_iDepth = m_iDepth + 1;
				adjustDepthOfChilderns();
			}
		}

		void adjustDepthOfChilderns() {
			for(int i = 0; i < m_vChildrens.size(); i++) {
				TREEITEM* item = (TREEITEM*)m_vChildrens[i];
				item->m_iDepth = m_iDepth + 1;
				if(!item->m_bIsLeaf)
					item->adjustDepthOfChilderns();
			}
		}

		void deleteLeaf(bool bDeleteFromParent) {
			TREEITEM* item = NULL;
			int posToDelete = -1;
			if(bDeleteFromParent) {
				for(int i = 0; i < m_pParent->m_vChildrens.size(); i++) {
					item = (TREEITEM*)m_pParent->m_vChildrens[i];
					if(item == this) {
						posToDelete = i;
						break;
					}
				}
			}
			
			if(m_bIsLeaf) {
				if(bDeleteFromParent)
					m_pParent->m_vChildrens.erase(m_pParent->m_vChildrens.begin() + posToDelete);
				delete this;
			}
			else {
				for(int i = 0; i < m_vChildrens.size(); i++) {
					item = (TREEITEM*)m_vChildrens[i];
					item->deleteLeaf(true);
				}

				if(bDeleteFromParent)
					m_pParent->m_vChildrens.erase(m_pParent->m_vChildrens.begin() + posToDelete);
				delete this;
			}
		}

		int getDepth() {
			
		}
		/*
		void printChildrens() {
			RectF destRect(0, 0, 0, 0);

			if(m_iDepth == 0)
				WTree::m_ileafY = 0;

			WTree::m_pRenderer->renderChild(	WTree::m_TreeWidget, 
												(m_iDepth == 0)?WTree::m_wRoot:WTree::m_wFolderClosed,
												&destRect, 
												m_pTree->getLeft() + m_iDepth*25, 
												m_pTree->getTop() + WTree::m_ileafY*20);
			
			for(int i = 0; i < m_vChildrens.size(); i++) {
				ITEM* it = (ITEM*)m_vChildrens[i];
				
				WTree::m_ileafY++;

				if(!it->m_bIsLeaf)
					it->printChildrens();
				else {
					WTree::m_pRenderer->renderChild(	WTree::m_TreeWidget, 
														WTree::m_wChild1, 
														&destRect, 
														m_pTree->getLeft() + it->m_iDepth*25, 
														m_pTree->getTop() + WTree::m_ileafY*20);
				}
			}
		}
		*/
};

		TREEITEM*					m_pCopyItem;
		TREEITEM*					pparent;

		TREEITEM*					m_pRoot;
		std::vector<TREEITEM*>	m_vLeafList;
		int								m_iSelection;
		int								m_iListCounter;

		bool								m_bIsEditing;
		TREEITEM*					m_pEditItem;

		void					populateLeafList(TREEITEM* iLeaf);
		void					printLeafList();
		void					calculateDeepestLeaf();
		int					getExpandedChildCount(TREEITEM* item);
		void					renderTree();
		void					expandNode(int itemIdx);
		bool					collapseNode(int itemIdx);
		void					deleteNode(TREEITEM* item);
		void					setPositionInTree();
		int					getStringWidthInPixels(TREEITEM* item);

		void					startEditing(TREEITEM* item);
		void					stopEditing();
		void					repositionEditField();

		bool					m_IsVScrolling;
		bool					m_IsHScrolling;
		void					updateMains();
	public:
		WTree();
		virtual ~WTree();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void setResizable(bool bResizable)	{ m_bResizable = bResizable; }
		const char* getTitle() { return m_title.c_str(); }
		
		virtual void onCreateEx(LPVOID lpVoid);
		virtual void onUpdate(float deltaTimeMs);
		virtual void onRender();

		virtual void onMouseDownEx(int x, int y, int iButton);
		virtual void onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy);
		virtual void onMouseUpEx(int x, int y, int iButton);
		virtual void onMouseWheelEx(WPARAM wParam, LPARAM lParam);
		virtual void onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		virtual void onKeyBDownEx(unsigned int iVirtualKeycode, unsigned short ch);

		void updateVBarPosition();
		void updateHBarPosition();
		void setVScrollbarLength();
		void setHScrollbarLength();
		
		void addDefaultList();

		int				m_minX;
		int				m_maxX;
		int				m_minY;
		int				m_maxY;

		int				m_iMaxWidthPixels;
		int				m_iMaxHeightPixels;

		int				m_iMaxHScrollbarWidth;
		int				m_iMaxVScrollbarHeight;

		unsigned int	LEAF_SPACING_Y;
};

#endif
#endif