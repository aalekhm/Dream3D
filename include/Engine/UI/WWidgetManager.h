#ifndef WWIDGETMANAGER_H
#define WWIDGETMANAGER_H

#include "Engine/UI/widgetdef.h"
#include "Engine/UI/UIDefines.h"

#include <gl\gl.h>						// Header File For The OpenGL32 Library
#include <gl\glu.h>						// Header File For The GLu32 Library

#define TEXTURE_CORE		0
#define TEXTURE_FONT_ROSEMARY_16		1

#define MAX_WIDGETS			80

typedef void (__stdcall* YAGUICallback)(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct WWidgetManager {

	public:
		~WWidgetManager();
		static WWidgetManager* getInstance();
		
		static YAGUICallback		m_lpfnWWndProc;
		H_WND							m_pBaseWindow;

		static void			setCallback(YAGUICallback wndProc);
		H_WND				createWindow(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);
		H_WND				GetWindow(int ID_WINDOW);
		H_WND				FindWindowQ(LPCSTR lpClassName, LPCSTR lpWindowName);

		void					update(float deltaTimeMs);

		void					keyPressed(unsigned int iVirtualKeycode, unsigned short ch);
		void					keyReleased(unsigned int iVirtualKeycode, unsigned short ch);
		void					onMouseDown(int mCode, int x, int y);
		void					onMouseUp(int mCode, int x, int y);
		void					onMouseRDown(int mCode, int x, int y);
		void					onMouseRUp(int mCode, int x, int y);
		void					onMouseHover(int mCode, int x, int y);
		void					onMouseMove(int mCode, int x, int y);
		void					onMouseWheel(WPARAM wParam, LPARAM lParam);

		void					setGLStates();
		void					flush();
		static void			drawFont(int x, int y, int charW, int charH, int tX, int tY);
		static int			getGlyphU(int c);
		static int			getGlyphV(int c);

		static void			renderWidget(const char* sWidgetName, RectF* WndRect);
		static void			renderSkin(WIDGET* widget, BASESKIN* skinPtr, RectF* wndRect);
		static void			renderChild(WIDGET* widget, CHILD* childPtr, RectF* wndRect);
		static void			renderChild(WIDGET* widget, CHILD* childPtr, RectF* wndRect, float x, float y);
		static void			renderClientArea(WIDGET* widget, int i, RectF* wndRect);

		static WIDGET*	getWidget(const char* sWidgetName);
		static int			getWidgetID(const char* sWidgetName);
		static float		getWidgetWidth(const char* sWidgetName);
		static float		getWidgetHeight(const char* sWidgetName);
		static void			getDestinationRect(RectF& destRect, float parentW, float parentH, RectF* wndRect, RectF* idealRect, int hAlign, int vAlign);
		bool					loadMainWindowDefaults();
		static void			setClip(int x, int y, int w, int h );
		static void			GetClipBounds(RectF* reclaimRect);
		
		static void			drawStringFont(const char* cStr, int x, int y, int anchor);
		static int			getCharWidth(int c);
		static int			getStringWidthTillPos(const char* cStr, int iPos);
		void					fillRect(Color* color, Rect* rect);
		void					fillRect(float r, float g, float b, float a, Rect* rect);
		void					drawRect(float r, float g, float b, float a, Rect* rect, int iStrokeWidth = 1);
		void					drawHorizontalLine(float r, float g, float b, float a, int x, int y, int width, int iStrokeWidth);
		void					drawVerticalLine(float r, float g, float b, float a, int x, int y, int height, int iStrokeWidth);
		static void			drawVerticalLine(float x1, float y1, float x2, float y2);
		static int			getNextWhitespacePos(const char* str, int curPos, bool isLeft);

		static void			clearScreen();
		static void			setCursor(int MOUSE_IDC);

		void					drawTGA(int texID, int x, int y);
		static void			glSelectTexture(int id);
		static bool			loadTexture(char* sTexName, GLuint &texid, GLuint* texWH);

		static void			setupOrthogonalProjection();
		static void			drawQuadU(	int TEXTURE_ID, 
													float posX, float posY, float posW, float posH,
													float texX, float texY, float texW, float texH);
		static void			drawQuad(	GLfloat xTexLT,	GLfloat yTexLT,		GLfloat xTexLB,		GLfloat yTexLB,
												GLfloat xTexRB,	GLfloat yTexRB,		GLfloat xTexRT,		GLfloat yTexRT,
												GLfloat xQuadLT,	GLfloat yQuadLT,	GLfloat xQuadLB,	GLfloat yQuadLB,
												GLfloat xQuadRB,	GLfloat yQuadRB,	GLfloat xQuadRT,	GLfloat yQuadRT);

		static void			drawQuadU(	GLfloat xTexLT,	GLfloat yTexLT,		GLfloat xTexLB,		GLfloat yTexLB,
													GLfloat xTexRB,	GLfloat yTexRB,		GLfloat xTexRT,		GLfloat yTexRT,
													GLfloat xQuadLT,	GLfloat yQuadLT,	GLfloat xQuadLB,	GLfloat yQuadLB,
													GLfloat xQuadRB,	GLfloat yQuadRB,	GLfloat xQuadRT,	GLfloat yQuadRT);
		static void			setColor(unsigned int iColor) { m_iRenderColor = iColor; }
		static void			resetColor() { m_iRenderColor = 0xffffffff; }

		static bool			loadFont(const char* sFontFileName, unsigned int iFontSize, unsigned int iFontDPI);

		static void			onEvent(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static H_WND		getWindow(int wndID);

		struct STGA
		{
			STGA()
			{
					data = (unsigned char*)0;
					width = 0;
					height = 0;
					byteCount = 0;
			}

			~STGA() { delete[] data; data = 0; }

			void destroy() { delete[] data; data = 0; }

			int width;
			int height;
			unsigned char byteCount;
			unsigned char* data;
		};

		static char*					m_clipboardTextData;

		static WWidgetManager*	m_pInstance;
		static int						m_SpriteCount;
		static int						m_FrameCount;
		static int						m_iRenderColor;
		
		static VertexT2F_C4UB_V3F*	m_VB;
		//static VertexT2F_V3F*	m_VB;
		//static VertexT2F_C4F_N3F_V3F*	m_VB;
		//static VertexT2F_C3F_V3F*	m_VB;

		static unsigned int		CHARACTER_WIDTH;
		static unsigned int		CHARACTER_HEIGHT;
		static Glyph m_GlyphArray[];

	private:
		WWidgetManager();
		bool					readMap(char* filePathAndName);
		int					getWidgetAlignInt(char* sAlign);
		int					getTextAlignInt(char* sAlign);

		static std::vector<WIDGET*>		m_pWidgets;

		static RectF								m_ClipRect;
		static int									m_iCurrentTextureID;

		ColorUV										m_ColorWhiteUV;

		static unsigned int						m_iFontSize;
		static unsigned int						m_iFontDPI;
		static CCString							m_sFontName;

		int											lastMouseX;
		int											lastMouseY;

		static HCURSOR							m_hCurArrow;
		static HCURSOR							m_hCurIBeam;
		static HCURSOR							m_hCurCross;
		static HCURSOR							m_hCurSizeWE;
		static HCURSOR							m_hCurSizeNESW;
		static HCURSOR							m_hCurSizeNWSE;

		static GLuint								m_texture[3];				// Storage For Our Font Texture
		static GLuint								m_textureWH[3][3];			// W/H Storage For Our Font Texture

};

#endif