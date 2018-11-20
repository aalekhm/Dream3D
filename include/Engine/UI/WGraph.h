#ifdef USE_YAGUI
#ifndef WGRAPH_H
#define WGRAPH_H

#include "Engine/UI/UIDefines.h"
#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WScrollbar.h"
#include "Engine/UI/WButton.h"

struct GraphDot
{
	GraphDot(PointF pPos) {
		GraphDot(pPos.X, pPos.Y);
	}

	GraphDot(float xPos, float yPos) {
		m_Pos.X = xPos;
		m_Pos.Y = yPos;

		m_bIsSelected = false;
	}

	PointF	m_Pos;
	bool		m_bIsSelected;
};
typedef std::vector<GraphDot*> GraphDots;

struct WGraph : public WContainer {
	private:
		bool			m_bResizable;

	public:
		WGraph();
		virtual ~WGraph();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void				setResizable(bool bResizable)	{ m_bResizable = bResizable; }
		void				setBorderVisibility(bool bHasBorder)	{ m_bShowBorder = bHasBorder; }
		bool				getBorderVisibility()					{ return m_bShowBorder; }

		void				setScale(float xAxis, float yAxis) { if(xAxis > 0) m_GraphScale.X = xAxis; if(yAxis > 0) m_GraphScale.Y = yAxis; }
		void				addDot(PointF m_Pos) { addDot(m_Pos.X, m_Pos.Y); }
		void				addDot(float xPos, float yPos) { GraphDot* pGraphDot = new GraphDot(xPos, yPos); m_vDots.push_back(pGraphDot); }
		void				clear();
	protected:
		void				onResize(int width, int height);
		void				resizeWidth(int iDiffWidth);
		void				resizeHeight(int iDiffHeight);

		virtual void onCreateEx(LPVOID lpVoid);
		virtual void onUpdate();
		virtual void onRender();

		virtual void onMouseDownEx(int x, int y, int iButton);
		virtual void onMouseMoveEx(int mCode, int x, int y, int prevx, int prevy);
		virtual void onMouseUpEx(int x, int y, int iButton);
		virtual void onMouseWheelEx(WPARAM wParam, LPARAM lParam);
		virtual void onMouseHoverEx(int mCode, int x, int y, int prevX, int prevY);

		virtual void	onKeyBDownEx(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onKeyBUpEx(unsigned int iVirtualKeycode, unsigned short ch);

		virtual void onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		virtual bool isPtInside(int x, int y);
	
		void			getPositionInGraph(int iInputX, int iInputY, float& fOutX, float& fOutY);
		void			deleteGraphDotFromList(const GraphDot* pGraphDotPtr);
	private:
		void				renderGraph();
		void				renderLegend();

		WIDGET*		m_pGraphWidget;
		WButton*		m_ButtonWResizeLeft;
		WButton*		m_ButtonWResizeRight;
		
		int				m_iMainX;
		int				m_iMainY;

		int				m_iMinX;
		int				m_iMinY;

		int				m_iMaxX;
		int				m_iMaxY;

		GraphScale	m_GraphScale;
		float				m_fPosX;
		float				m_fPosY;

		bool				m_bShowBorder;
		int				m_iResizingX;

		bool				m_bCanPan;

		GraphDots		m_vDots;
		GraphDot*		m_pSelectedGraphDot;
		short				m_iSelectedPos;
};

#endif
#endif