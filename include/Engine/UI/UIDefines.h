
#ifndef DEFINES_H
#define DEFINES_H

#include <windows.h>
#include <string.h>
#include <vector>
#include "Common/CCString.h"
#include "Common/RandomAccessFile.h"
#include "Engine/UI/TTFFontEncoder.h"

#define RENDER_GL
#define SAFE_DELETE(x) if(x){ delete x; x = NULL; }
#define SAFE_DELETE_ARRAY(x) if(x){ delete[] x; x = NULL; }

#define STATE_READ_WIDGET		0
#define	STATE_READ_CHILD		1
#define	STATE_READ_BASESKIN		2
#define	STATE_READ_CLIENTAREA	3
#define	STATE_READ_SIMPLE_TEXT	4

#define u8 unsigned char
#define s8 char
#define u16 unsigned short int
#define s16 short int
#define u32 unsigned int
#define s32 int
#define s64 long long
#define u64 unsigned long long

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#define REAL_EPSILON        1.192092896e-07F        /* FLT_EPSILON */

typedef float REAL;
struct SizeF
{
public:
    SizeF()
    {
        Width = Height = 0.0f;
    }

    SizeF(IN const SizeF& size)
    {
        Width = size.Width;
        Height = size.Height;
    }

    SizeF(IN REAL width,
          IN REAL height)
    {
        Width = width;
        Height = height;
    }

    SizeF operator+(IN const SizeF& sz) const
    {
        return SizeF(Width + sz.Width,
                     Height + sz.Height);
    }

    SizeF operator-(IN const SizeF& sz) const
    {
        return SizeF(Width - sz.Width,
                     Height - sz.Height);
    }

public:

    REAL Width;
    REAL Height;
};

struct Size
{
public:
    Size()
    {
        Width = Height = 0;
    }

    Size(IN const Size& size)
    {
        Width = size.Width;
        Height = size.Height;
    }

    Size(IN INT width,
         IN INT height)
    {
        Width = width;
        Height = height;
    }

    Size operator+(IN const Size& sz) const
    {
        return Size(Width + sz.Width,
                    Height + sz.Height);
    }

    Size operator-(IN const Size& sz) const
    {
        return Size(Width - sz.Width,
                    Height - sz.Height);
    }

    BOOL Equals(IN const Size& sz) const
    {
        return (Width == sz.Width) && (Height == sz.Height);
    }

    BOOL Empty() const
    {
        return (Width == 0 && Height == 0);
    }

public:

    INT Width;
    INT Height;
};

struct PointF
{
public:
   PointF()
   {
       X = Y = 0.0f;
   }

   PointF(IN const PointF &point)
   {
       X = point.X;
       Y = point.Y;
   }

   PointF(IN const SizeF &size)
   {
       X = size.Width;
       Y = size.Height;
   }

   PointF(IN REAL x,
          IN REAL y)
   {
       X = x;
       Y = y;
   }

   PointF operator+(IN const PointF& point) const
   {
       return PointF(X + point.X,
                     Y + point.Y);
   }

   PointF operator-(IN const PointF& point) const
   {
       return PointF(X - point.X,
                     Y - point.Y);
   }
public:

    REAL X;
    REAL Y;
};

typedef PointF GraphScale;

struct Point
{
public:
   Point()
   {
       X = Y = 0;
   }

   Point(IN const Point &point)
   {
       X = point.X;
       Y = point.Y;
   }

   Point(IN const Size &size)
   {
       X = size.Width;
       Y = size.Height;
   }

   Point(IN INT x,
         IN INT y)
   {
       X = x;
       Y = y;
   }

   Point operator+(IN const Point& point) const
   {
       return Point(X + point.X,
                    Y + point.Y);
   }

   Point operator-(IN const Point& point) const
   {
       return Point(X - point.X,
                    Y - point.Y);
   }

   BOOL Equals(IN const Point& point)
   {
       return (X == point.X) && (Y == point.Y);
   }

public:

    INT X;
    INT Y;
};

struct RectF
{
public:

    RectF()
    {
        X = Y = Width = Height = 0.0f;
    }

    RectF(IN REAL x,
          IN REAL y,
          IN REAL width,
          IN REAL height)
    {
        X = x;
        Y = y;
        Width = width;
        Height = height;
    }

    RectF(IN const PointF& location,
          IN const SizeF& size)
    {
        X = location.X;
        Y = location.Y;
        Width = size.Width;
        Height = size.Height;
    }

    RectF* Clone() const
    {
        return new RectF(X, Y, Width, Height);
    }

    void GetSize(OUT SizeF* size) const
    {
        size->Width = Width;
        size->Height = Height;
    }

    void GetBounds(OUT RectF* rect) const
    {
        rect->X = X;
        rect->Y = Y;
        rect->Width = Width;
        rect->Height = Height;
    }

    REAL GetLeft() const
    {
        return X;
    }

    REAL GetTop() const
    {
        return Y;
    }

    REAL GetRight() const
    {
        return X+Width;
    }

    REAL GetBottom() const
    {
        return Y+Height;
    }

    BOOL Intersect(IN const RectF& rect)
    {
        return Intersect(*this, *this, rect);
    }

    static BOOL Intersect(OUT RectF& c,
                          IN const RectF& a,
                          IN const RectF& b)
    {
        REAL right = min(a.GetRight(), b.GetRight());
        REAL bottom = min(a.GetBottom(), b.GetBottom());
        REAL left = max(a.GetLeft(), b.GetLeft());
        REAL top = max(a.GetTop(), b.GetTop());

        c.X = left;
        c.Y = top;
        c.Width = right - left;
        c.Height = bottom - top;
        return !c.IsEmptyArea();
    }

	static BOOL Union(OUT RectF& c,
		IN const RectF& a,
		IN const RectF& b)
	{
		REAL right = max(a.GetRight(), b.GetRight());
		REAL bottom = max(a.GetBottom(), b.GetBottom());
		REAL left = min(a.GetLeft(), b.GetLeft());
		REAL top = min(a.GetTop(), b.GetTop());

		c.X = left;
		c.Y = top;
		c.Width = right - left;
		c.Height = bottom - top;
		return !c.IsEmptyArea();
	}
    
	BOOL IsEmptyArea() const
    {
        return (Width <= REAL_EPSILON) || (Height <= REAL_EPSILON);
    }

	void Offset(IN const PointF& point)
    {
        Offset(point.X, point.Y);
    }

    void Offset(IN REAL dx,
                IN REAL dy)
    {
        X += dx;
        Y += dy;
    }

	void Set(REAL xX, REAL yY, REAL wWidth, REAL hHeight)
	{
		X = xX;
		Y = yY;
		Width = wWidth;
		Height = Height;

	}
public:

    REAL X;
    REAL Y;
    REAL Width;
    REAL Height;
};

struct Rect {
public:

	Rect() {
        X = Y = Width = Height = 0;
    }

    Rect(IN INT x,
         IN INT y,
         IN INT width,
         IN INT height
	) {
        X = x;
        Y = y;
        Width = width;
        Height = height;
    }

    Rect(IN const Point& location,
         IN const Size& size)
    {
        X = location.X;
        Y = location.Y;
        Width = size.Width;
        Height = size.Height;
    }

    Rect* Clone() const {
        return new Rect(X, Y, Width, Height);
    }

    VOID GetLocation(OUT Point* point) const {
        point->X = X;
        point->Y = Y;
    }

    VOID GetSize(OUT Size* size) const {
        size->Width = Width;
        size->Height = Height;
    }

    VOID GetBounds(OUT Rect* rect) const {
        rect->X = X;
        rect->Y = Y;
        rect->Width = Width;
        rect->Height = Height;
    }

    INT GetLeft() const {
        return X;
    }

    INT GetTop() const {
        return Y;
    }

    INT GetRight() const {
        return X+Width;
    }

    INT GetBottom() const {
        return Y+Height;
    }

    BOOL IsEmptyArea() const {
        return (Width <= 0) || (Height <= 0);
    }

    BOOL Equals(IN const Rect & rect) const {
        return X == rect.X &&
               Y == rect.Y &&
               Width == rect.Width &&
               Height == rect.Height;
    }

    BOOL Contains(IN INT x,
                  IN INT y) const
    {
        return x >= X && x < X+Width &&
               y >= Y && y < Y+Height;
    }

    BOOL Contains(IN const Point& pt) const {
        return Contains(pt.X, pt.Y);
    }

    BOOL Contains(IN Rect& rect) const {
        return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
               (Y <= rect.Y) && (rect.GetBottom() <= GetBottom());
    }

    VOID Inflate(IN INT dx,
                 IN INT dy)
    {
        X -= dx;
        Y -= dy;
        Width += 2*dx;
        Height += 2*dy;
    }

    VOID Inflate(IN const Point& point) {
        Inflate(point.X, point.Y);
    }

    BOOL Intersect(IN const Rect& rect) {
        return Intersect(*this, *this, rect);
    }

    static BOOL Intersect(OUT Rect& c,
                          IN const Rect& a,
                          IN const Rect& b)
    {
        INT right = min(a.GetRight(), b.GetRight());
        INT bottom = min(a.GetBottom(), b.GetBottom());
        INT left = max(a.GetLeft(), b.GetLeft());
        INT top = max(a.GetTop(), b.GetTop());

        c.X = left;
        c.Y = top;
        c.Width = right - left;
        c.Height = bottom - top;
        return !c.IsEmptyArea();
    }

    BOOL IntersectsWith(IN const Rect& rect) const {
        return (GetLeft() < rect.GetRight() &&
                GetTop() < rect.GetBottom() &&
                GetRight() > rect.GetLeft() &&
                GetBottom() > rect.GetTop());
    }

    static BOOL Union(OUT Rect& c,
                      IN const Rect& a,
                      IN const Rect& b)
    {
        INT right = max(a.GetRight(), b.GetRight());
        INT bottom = max(a.GetBottom(), b.GetBottom());
        INT left = min(a.GetLeft(), b.GetLeft());
        INT top = min(a.GetTop(), b.GetTop());

        c.X = left;
        c.Y = top;
        c.Width = right - left;
        c.Height = bottom - top;
        return !c.IsEmptyArea();
    }

    VOID Offset(IN const Point& point) {
        Offset(point.X, point.Y);
    }

    VOID Offset(IN INT dx,
                IN INT dy)
    {
        X += dx;
        Y += dy;
    }

public:

    INT X;
    INT Y;
    INT Width;
    INT Height;
};

struct Color {
	public:
		float r;
		float g;
		float b;
		float a;

		Color() {
			r = g = b = 0;
			a = 255;
		}

		Color(float rRed, float gGreen, float bBlue, float aAlpha) {
			r = rRed;
			g = gGreen;
			b = bBlue;
			a = aAlpha;
		}

		float getRed() { return r; }
		float getGreen() { return g; }
		float getBlue() { return b; }
		float getAlpha() { return a; }

		void setRed(float rRed) { r = rRed; }
		void setGreen(float gGreen) { g = gGreen; }
		void setBlue(float bBlue) { b = bBlue; }
		void setAlpha(float aAlpha) { a = aAlpha; }
};

struct Vector2_ {

	public:
		Vector2_();
		Vector2_(int x, int y) {
			this->x = x;
			this->y = y;
		}
		Vector2_(const Vector2_ &v) {
			this->x = v.x;
			this->y = v.y;
		}

		int x;
		int y;
};

struct OFFSETS {
	public:
		OFFSETS() {
			x = y = w = h = 0.0f;
		};

		float	x;
		float	y;
		float	w;
		float	h;
};

#define WM_ANCHOR_LEFT								0x0000L
#define WM_ANCHOR_TOP									0x0000L

#define WM_ANCHOR_CENTERX							0x0001L
#define WM_ANCHOR_RIGHT								0x0002L
#define WM_ANCHOR_HSTRETCH						0x0004L

#define WM_ANCHOR_CENTERY							0x0008L
#define WM_ANCHOR_BOTTOM							0x0010L
#define WM_ANCHOR_VSTRETCH						0x0020L

#define WM_CUSTOM											0x0040L

#define	WM_ALIGN_WRT_PARENT_LEFT				0x0080L
#define	WM_ALIGN_WRT_PARENT_CENTERX		0x0100L
#define	WM_ALIGN_WRT_PARENT_RIGHT				0x0200L
#define	WM_ALIGN_WRT_PARENT_TOP				0x0400L
#define	WM_ALIGN_WRT_PARENT_CENTERY		0x0800L
#define	WM_ALIGN_WRT_PARENT_BOTTOM			0x1000L

#define WM_ANCHOR_TOPLEFT									WM_ANCHOR_TOP | WM_ANCHOR_LEFT
#define WM_ANCHOR_TOPCENTER								WM_ANCHOR_TOP | WM_ANCHOR_CENTERX
#define WM_ANCHOR_TOPRIGHT									WM_ANCHOR_TOP | WM_ANCHOR_RIGHT

#define WM_ANCHOR_CENTERLEFT							WM_ANCHOR_CENTERY | WM_ANCHOR_LEFT
#define WM_ANCHOR_CENTER									WM_ANCHOR_CENTERY | WM_ANCHOR_CENTERX
#define WM_ANCHOR_CENTERRIGHT							WM_ANCHOR_CENTERY | WM_ANCHOR_RIGHT

#define WM_ANCHOR_BOTTOMLEFT								WM_ANCHOR_BOTTOM | WM_ANCHOR_LEFT
#define WM_ANCHOR_BOTTOMCENTER						WM_ANCHOR_BOTTOM | WM_ANCHOR_CENTERX
#define WM_ANCHOR_BOTTOMRIGHT							WM_ANCHOR_BOTTOM | WM_ANCHOR_RIGHT

#define WM_ANCHOR													8064

#define WM_ALIGN_WRT_PARENT_TOPLEFT					WM_ALIGN_WRT_PARENT_TOP | WM_ALIGN_WRT_PARENT_LEFT
#define WM_ALIGN_WRT_PARENT_TOPCENTER			WM_ALIGN_WRT_PARENT_TOP | WM_ALIGN_WRT_PARENT_CENTERX
#define WM_ALIGN_WRT_PARENT_TOPRIGHT				WM_ALIGN_WRT_PARENT_TOP | WM_ALIGN_WRT_PARENT_RIGHT

#define WM_ALIGN_WRT_PARENT_CENTERLEFT			WM_ALIGN_WRT_PARENT_CENTERY | WM_ALIGN_WRT_PARENT_LEFT
#define WM_ALIGN_WRT_PARENT_CENTER					WM_ALIGN_WRT_PARENT_CENTERY | WM_ALIGN_WRT_PARENT_CENTERX
#define WM_ALIGN_WRT_PARENT_CENTERRIGHT		WM_ALIGN_WRT_PARENT_CENTERY | WM_ALIGN_WRT_PARENT_RIGHT

#define WM_ALIGN_WRT_PARENT_BOTTOMLEFT			WM_ALIGN_WRT_PARENT_BOTTOM | WM_ALIGN_WRT_PARENT_LEFT
#define WM_ALIGN_WRT_PARENT_BOTTOMCENTER		WM_ALIGN_WRT_PARENT_BOTTOM | WM_ALIGN_WRT_PARENT_CENTERX
#define WM_ALIGN_WRT_PARENT_BOTTOMRIGHT			WM_ALIGN_WRT_PARENT_BOTTOM | WM_ALIGN_WRT_PARENT_RIGHT

#define	 WM_ALIGN_WRT_PARENT								63

enum H_ALIGN {
	H_LEFT = 0,
	H_CENTER,
	H_RIGHT,
	H_STRETCH
};

enum V_ALIGN {
	V_TOP = 0,
	V_CENTER,
	V_BOTTOM,
	V_STRETCH
};

struct ALIGN {
	public:
		ALIGN() {
			eHAlign = H_LEFT;
			eVAlign = V_TOP;
		};

		H_ALIGN		eHAlign;
		V_ALIGN		eVAlign;
};

struct WIDGETSIZE {
	public:
		WIDGETSIZE() {
			width = height = 0;
		};

		float		width;
		float		height;
};

struct CHILD {
	public:
		CHILD() {
		};

		char			sName[255];
		OFFSETS			posOffsets;
		OFFSETS			skinOffsets;
		ALIGN			align;
		WIDGETSIZE		parentSize;
};

struct CLIENT_AREA {
	public:
		CLIENT_AREA() {
		};

		char			sName[255];
		OFFSETS			posOffsets;
		OFFSETS			skinOffsets;
		ALIGN			align;
		WIDGETSIZE		parentSize;
};

struct BASESKIN {
	public:
		BASESKIN() {
		};

		OFFSETS			posOffsets;
		OFFSETS			skinOffsets;
		ALIGN			align;
		WIDGETSIZE		parentSize;
		char			name[255];
};

struct SIMPLE_TEXT {
	public:
		SIMPLE_TEXT() {
		}

		char			string[255];
		char			font[255];
		OFFSETS			posOffsets;
		ALIGN			align;
};

struct WIDGET {
	public:
		WIDGET() {
		};

		char						widgetName[255];
		std::vector<CHILD*>			childWindows;
		std::vector<BASESKIN*>		skins;
		std::vector<CLIENT_AREA*>	clientAreas;
		std::vector<SIMPLE_TEXT*>	strings;
		WIDGETSIZE 					widgetSize;

		BASESKIN* getSkin(const char* skinName) {
			for(int i = 0; i < (int)skins.size(); i++) {
				BASESKIN* skinPtr = (BASESKIN*)skins[i];
				if(strcmp(skinName, skinPtr->name) == 0)
					return skinPtr;
			}
			
			return NULL;
		}

		CHILD* getChild(const char* childName) {
			for(int i = 0; i < childWindows.size(); i++) {
				CHILD* childPtr = (CHILD*)childWindows[i];
				if(strcmp(childName, childPtr->sName) == 0) {
					return childPtr;
				}
			}

			return NULL;
		}

		CLIENT_AREA* getClientAreaAt(int i) {
			if(clientAreas.size() > 0) {
				return clientAreas[0];
			}

			return NULL;
		}
};

enum ENUM_TEXT_ALIGN {
	TXT_LEFT,
	TXT_HCENTER,
	TXT_RIGHT,
	TXT_TOP,
	TXT_VCENTER,
	TXT_BOTTOM
};

typedef struct LISTBOX_ITEM {
	std::string		itemLabel;
	//void*			itemData;
};

#define BTN_SCROLLBAR_UP		100
#define BTN_SCROLLBAR_DOWN		101
#define BTN_SCROLLBAR_LEFT		102
#define BTN_SCROLLBAR_RIGHT		103
#define BTN_SCROLLBAR_SCROLL	104
#define BTN_PLUS				105
#define BTN_MINUS				106

#define ID_RESIZE_LEFT			105
#define ID_RESIZE_RIGHT			106

enum ID_SCROLLBARS {
	ID_VERTICAL_SCROLLBAR = 0,
	ID_HORIZONTAL_SCROLLBAR
};

enum ID_WND_TYPE {
	ID_TYPE_WND_C = 0,
	ID_TYPE_WND_CX,
	ID_TYPE_WND_CMX
};

#define WINDOW_TITLE_HEIGHT		28
#define FONT_TEXTURE_START_X	701
#define FONT_TEXTURE_START_Y	113

#define SPR_MAX		16384
#define SCREEN_MAX	1
#define ALLSQR_MAX	(SCREEN_MAX + SPR_MAX)

struct SCoordFloat3
{
	float x;
	float y;
	float z;
};

struct SCoordFloat2
{
	float x;
	float y;
};

struct VertexT2F_V3F
{
	float tu, tv;
	float vx, vy, vz;
};

struct VertexT2F_C4F_N3F_V3F
{
	float tu, tv;
	float r, g, b, a;
	float nx, ny, nz;
	float vx, vy, vz;
};

struct VertexT2F_C4UB_V3F
{
	float tu, tv;
	u32 color;
	float vx, vy, vz;
};

struct VertexT2F_C3F_V3F
{
	float tu, tv;
	float r, g, b;
	float vx, vy, vz;
};

struct ColorUV
{
	u32 u, v;
};

#define					IDC_CB_FONT	100

//////////////////////////////////////////////////////////////////
#define ADD_VERTICAL_SCROLL(__widget__) \
{ \
	RectF wndRect; \
	RectF vDestRect; \
	RectF idealRect; \
	RectF hDestRect; \
	\
	CHILD* verticalSBChild = __widget__->getChild("VScroll"); \
	wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight(); \
	idealRect.X = verticalSBChild->posOffsets.x; \
	idealRect.Y = verticalSBChild->posOffsets.y; \
	idealRect.Width = verticalSBChild->posOffsets.w;  \
	idealRect.Height = verticalSBChild->posOffsets.h; \
	WWidgetManager::getDestinationRect(	vDestRect, \
															__widget__->widgetSize.width, \
															__widget__->widgetSize.height, \
															&wndRect, \
															&idealRect, \
															verticalSBChild->align.eHAlign, \
															verticalSBChild->align.eVAlign \
														); \
	if(m_sbVertical == NULL) { \
		hWnd =  \
				CreateComponent(	"WScrollbar",  \
				"",  \
				0,  \
				vDestRect.X - m_iLeft,  \
				vDestRect.Y - m_iTop, \
				vDestRect.Width,  \
				vDestRect.Height, \
				this,  \
				HMENU(ID_VERTICAL_SCROLLBAR),  \
				(LPVOID)1); \
		m_sbVertical = (WScrollbar*)hWnd; \
		m_sbVertical->hasBG(true); \
		m_sbVertical->setPostRender(true); \
	} \
	else { \
		m_sbVertical->setPosition(vDestRect.X - m_iLeft, vDestRect.Y - m_iTop); \
		m_sbVertical->setSize(vDestRect.Width, vDestRect.Height); \
	} \
	m_iMaxVScrollbarHeight = vDestRect.Height; \
} \
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#define ADD_HORIZONTAL_SCROLL(__widget__) \
{ \
	RectF wndRect; \
	RectF vDestRect; \
	RectF idealRect; \
	RectF hDestRect; \
	\
	CHILD* horizontalSBChild = __widget__->getChild("HScroll"); \
	wndRect.X = m_iLeft; wndRect.Y = m_iTop; wndRect.Width = getWidth(); wndRect.Height = getHeight(); \
	idealRect.X = horizontalSBChild->posOffsets.x; \
	idealRect.Y = horizontalSBChild->posOffsets.y; \
	idealRect.Width = horizontalSBChild->posOffsets.w;  \
	idealRect.Height = horizontalSBChild->posOffsets.h; \
	WWidgetManager::getDestinationRect(	hDestRect, \
															__widget__->widgetSize.width, \
															__widget__->widgetSize.height, \
															&wndRect, \
															&idealRect, \
															horizontalSBChild->align.eHAlign, \
															horizontalSBChild->align.eVAlign \
														); \
	if(m_sbHorizontal == NULL) { \
		hWnd =  \
			CreateComponent(	"WScrollbar",  \
										"",  \
										0,  \
										hDestRect.X - m_iLeft,  \
										hDestRect.Y - m_iTop, \
										hDestRect.Width,  \
										hDestRect.Height, \
										this,  \
										HMENU(ID_HORIZONTAL_SCROLLBAR),  \
										(LPVOID)0); \
		m_sbHorizontal = (WScrollbar*)hWnd; \
		m_sbHorizontal->hasBG(true); \
		m_sbHorizontal->setPostRender(true); \
	} \
	else { \
		m_sbHorizontal->setPosition(hDestRect.X - m_iLeft, hDestRect.Y - m_iTop); \
		m_sbHorizontal->setSize(hDestRect.Width, hDestRect.Height); \
	} \
	\
	m_iMaxHScrollbarWidth = hDestRect.Width; \
} \
//////////////////////////////////////////////////////////////////
#endif