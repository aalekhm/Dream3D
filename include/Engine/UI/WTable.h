#ifndef WTABLE_H
#define WTABLE_H

#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WWidgetManager.h"
#include "Engine/UI/WScrollbar.h"

#define TABLE_HEADER_OFFSET_X	5
#define TABLE_HEADER_OFFSET_Y	1
#define TABLE_LEFT_GUTTER		5
#define TABLE_TOP_GUTTER		5

struct TableTitleData {
	public:
		TableTitleData(const std::string& str) {
			m_sTitle = str;
			setWidth(WWidgetManager::CHARACTER_WIDTH*(str.size() + 1) + TABLE_HEADER_OFFSET_X*2);
		}

		~TableTitleData() {
			m_sTitle.clear();
		}

		const char* getTitle() {
			return m_sTitle.c_str();
		}

		void setWidth(float width) {
			m_iWidth = width;
		}
		float getWidth() {
			return m_iWidth;
		}
	private:
		float		m_iWidth;
		std::string m_sTitle;
};

struct TableCellData {
	public:
		TableCellData(std::string sCellData) {
			m_sCellString = sCellData;
		}
		
		~TableCellData() {
			m_sCellString.clear();
		}

		const char* getValue() {
			return m_sCellString.c_str();
		}
		
	private:
		std::string		m_sCellString;
};

struct TableRowData {
	public:
		TableRowData() {
		}

		TableCellData* getCell(int i) {
			if(i < m_vRowData.size() && i >= 0)
				return m_vRowData[i];

			return NULL;
		}

		~TableRowData() {
			TableCellData* tcd = NULL;
			for(int i = 0; i < m_vRowData.size(); i++) {
				tcd = (TableCellData*)m_vRowData[i];
				delete tcd;
			}

			m_vRowData.clear();
		}

		void addCellData(TableCellData* tcd) {
			m_vRowData.push_back(tcd);
		}

	private:
		std::vector<TableCellData*> m_vRowData;
};

struct WTable : public WContainer {
	private:
		bool					m_bResizing;
		bool					m_bMoving;
		bool					m_bResizable;
		bool					m_bLButtonDown;
		int					m_iDraggableColumn;
		int					TB_LINE_NO_SPACE;
		char*				m_sLineNoStr;

		WIDGET*			m_TableWidget;
		WScrollbar*		m_sbVertical;
		WScrollbar*		m_sbHorizontal;

		WWidgetManager*		m_pRenderer;
		static CHILD*				m_pGreyTop;
		std::vector<TableTitleData*>	m_vTableTitleList;
		std::vector<TableRowData*>		m_vTableRowList;

		int					m_iSelection;
		void					renderTree();

		bool					m_IsVScrolling;
		bool					m_IsHScrolling;
		void					updateMains();
		void					renderTableHeader();
		void					drawLineNo();
	public:
		WTable();
		~WTable();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		void setResizable(bool bResizable)	{ m_bResizable = bResizable; }
		void setScrollbars(bool bHasScroll)	{ m_bHasScrollBars = bHasScroll; }
		void setBorderVisibility(bool bHasBorder)	{ m_bShowBorder = bHasBorder; }
		
		virtual void onCreateEx(LPVOID lpVoid);
		virtual void onUpdate();
		virtual void onRender();

		virtual void onMouseDownEx(int x, int y, int iButton);
		virtual void onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy);
		virtual void onMouseUpEx(int x, int y, int iButton);
		virtual void onMouseWheelEx(WPARAM wParam, LPARAM lParam);
		virtual void onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		virtual void onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch);

		void updateVBarPosition();
		void updateHBarPosition();
		void setVScrollbarLength();
		void setHScrollbarLength();

		void addColumn(const std::string& str) {
			TableTitleData* td = new TableTitleData(str);
			m_vTableTitleList.push_back(td);
		}

		int getTableColumnLength() { return m_vTableTitleList.size(); }

		void addRow(TableRowData* trd) { m_vTableRowList.push_back(trd); }

		void renderRows() {
			TableRowData* trd = NULL;
			int tableY = getTop() + TABLE_ROW_HEADER_HEIGHT + TABLE_TOP_GUTTER;
			for(int i = abs(m_iMainY)/LINE_HEIGHT; i < m_vTableRowList.size(); i++, tableY+= LINE_HEIGHT) {
				trd = (TableRowData*)m_vTableRowList[i];
				renderRow(trd, tableY);
			}
		}

		void renderRow(TableRowData* trd, int rowY) {
			RectF dummyRect(	getLeft() + TB_LINE_NO_SPACE + m_ClientRect.X + m_iMainX, 
								rowY, 
								m_ClientRect.Width, 
								TABLE_ROW_HEADER_HEIGHT);

			for(int i = 0; i < m_vTableTitleList.size(); i++) {
				TableTitleData* td = (TableTitleData*)m_vTableTitleList[i];
				TableCellData* tcd = (TableCellData*)trd->getCell(i);

				dummyRect.Width = td->getWidth();
				m_pRenderer->drawStringFont(tcd->getValue(), dummyRect.X + TABLE_HEADER_OFFSET_X, dummyRect.Y, 0);
				dummyRect.X += dummyRect.Width - 1;
			}
		}

		int				m_minX;
		int				m_maxX;
		int				m_minY;
		int				m_maxY;

		bool			m_bHasScrollBars;
		bool			m_bShowBorder;

		int				m_iMaxWidthPixels;
		int				m_iMaxHeightPixels;

		int				m_iMaxHScrollbarWidth;
		int				m_iMaxVScrollbarHeight;

		unsigned int	LINE_HEIGHT;
		unsigned int	TABLE_ROW_HEADER_HEIGHT;
};

#endif