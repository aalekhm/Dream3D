
#ifdef USE_YAGUI

#include "Engine/EngineManager.h"
#include "Engine/UI/WWidgetManager.h"
#include "Engine/UI/WComponentFactory.h"
#include "Engine/TGA.h"

#define CORE_TEXTURE_UI		"data/core.tga"
#define CORE_FONT_UI				"Rosemary_DroidSans"

//////////////////////////////////////////////////////////////////////////////////////////////
WWidgetManager*		WWidgetManager::m_pInstance = NULL;
YAGUICallback			WWidgetManager::m_lpfnWWndProc = NULL;
std::vector<WIDGET*>	WWidgetManager::m_pWidgets;
RectF						WWidgetManager::m_ClipRect = RectF(0, 0, 0, 0);
int							WWidgetManager::m_SpriteCount;
int							WWidgetManager::m_FrameCount;
int							WWidgetManager::m_iRenderColor;
char*						WWidgetManager::m_clipboardTextData;
int							WWidgetManager::m_iCurrentTextureID;
Texture*					WWidgetManager::m_pCurrentTexture = NULL;
Texture*					WWidgetManager::m_pTextureCoreUI = NULL;
H_FONT						WWidgetManager::m_pCurrentFont = NULL;

unsigned int				WWidgetManager::CHARACTER_WIDTH;
unsigned int				WWidgetManager::CHARACTER_HEIGHT;
Glyph						WWidgetManager::m_GlyphArray[ASCII_END_INDEX - ASCII_START_INDEX];

HCURSOR					WWidgetManager::m_hCurArrow;
HCURSOR					WWidgetManager::m_hCurIBeam;
HCURSOR					WWidgetManager::m_hCurCross;
HCURSOR					WWidgetManager::m_hCurSizeWE;
HCURSOR					WWidgetManager::m_hCurSizeNESW;
HCURSOR					WWidgetManager::m_hCurSizeNWSE;

VertexT2F_C4UB_V3F*			WWidgetManager::m_VB;
//VertexT2F_V3F*			WWidgetManager::m_VB;
//VertexT2F_C4F_N3F_V3F*		WWidgetManager::m_VB;
//VertexT2F_C3F_V3F*			WWidgetManager::m_VB;
//////////////////////////////////////////////////////////////////////////////////////////////

WWidgetManager::WWidgetManager() {
	WComponentFactory::Get()->Register("WStatic", &WStatic::Create);
	WComponentFactory::Get()->Register("WButton", &WButton::Create);
	WComponentFactory::Get()->Register("WCheckbox", &WCheckbox::Create);
	WComponentFactory::Get()->Register("WTextField", &WTextField::Create);
	WComponentFactory::Get()->Register("WScrollbar", &WScrollbar::Create);
	WComponentFactory::Get()->Register("WTextBox", &WTextBox::Create);
	WComponentFactory::Get()->Register("WListBox", &WListBox::Create);
	WComponentFactory::Get()->Register("WComboBox", &WComboBox::Create);
	WComponentFactory::Get()->Register("WConsoleLog", &WConsoleLog::Create);
	WComponentFactory::Get()->Register("WWindow", &WWindow::Create);
	WComponentFactory::Get()->Register("WTabbedPane", &WTabbedPane::Create);
	WComponentFactory::Get()->Register("WFrame", &WFrame::Create);
	WComponentFactory::Get()->Register("WTree", &WTree::Create);
	WComponentFactory::Get()->Register("WTable", &WTable::Create);
	WComponentFactory::Get()->Register("WDummy", &WDummy::Create);
	WComponentFactory::Get()->Register("WCanvas", &WCanvas::Create);
	WComponentFactory::Get()->Register("WInspectorTab", &WInspectorTab::Create);
	WComponentFactory::Get()->Register("WInspector", &WInspector::Create);

	if(!loadMainWindowDefaults())
		MessageBox(NULL, "Error", "Error loading WWidgetManager!!!", MB_OK);

	m_pBaseWindow =
	CreateComponent(	"WWindow", 
								"Title", 
								0, 
								110,
								0, 
								1024, 
								768,
								NULL, 
								NULL, 
								(LPVOID)ID_TYPE_WND_CX);

	resetColor();

	m_hCurArrow = LoadCursor(NULL, IDC_ARROW);
	m_hCurIBeam = LoadCursor(NULL, IDC_IBEAM);
	m_hCurCross = LoadCursor(NULL, IDC_CROSS);
	m_hCurSizeWE = LoadCursor(NULL, IDC_SIZEWE);
	m_hCurSizeNESW = LoadCursor(NULL, IDC_SIZENESW);
	m_hCurSizeNWSE = LoadCursor(NULL, IDC_SIZENWSE);
}

WWidgetManager* WWidgetManager::getInstance()	 {
	if (m_pInstance == NULL) 
		m_pInstance = new WWidgetManager();

	return m_pInstance;
}

bool WWidgetManager::loadMainWindowDefaults() {
	if(!readMap("data/coreInfo.txt"))
		return FALSE;

	Texture* pTexture = Texture::createEx(CORE_TEXTURE_UI, false);
	if(pTexture == NULL)
		return FALSE;
	m_pTextureCoreUI = pTexture;

	H_FONT hFont = loadFont(CORE_FONT_UI, 10, 96);
	if(hFont == NULL)
		return FALSE;
	m_pCurrentFont = hFont;

	m_ColorWhiteUV.u = 675;
	m_ColorWhiteUV.v = 65;

	m_VB = new VertexT2F_C4UB_V3F[SPR_MAX * 4];
	//m_VB = new VertexT2F_V3F[SPR_MAX * 4];
	//m_VB = new VertexT2F_C4F_N3F_V3F[SPR_MAX * 4];
	//m_VB = new VertexT2F_C3F_V3F[SPR_MAX * 4];
}

bool WWidgetManager::selectFont(H_FONT hFont) {
	if(hFont == NULL)
		return false;

	m_pCurrentFont = hFont;
}

H_FONT WWidgetManager::loadFont(const char* sFontFile, unsigned int iFontSize, unsigned int iFontDPI) {
	//Rosemary_DroidSans
	//Crisp
	//Consolas
	//Walkway_Black
	//Rosemary Roman
	//Kingdom_Hearts_Font
	//Inspyratta
	//DroidSansMono
	//diagoth
	//DejaVuSans
	//Comic Sans MS
	//ROSEMARY_DROIDSANS-BOLD

	char str[255] = "";
	memset(str, 0, 255);
	sprintf(str, "data/%s.ttf", sFontFile, iFontSize, iFontDPI);

	TTFFontEncoder* ttfEncoder = new TTFFontEncoder();
	ttfEncoder->encode(str, iFontSize, iFontDPI, true);

	sprintf(str, "data/%s_%d_%dDPI", sFontFile, iFontSize, iFontDPI);
	CCString sFontFileName = str;
	sFontFileName += ".tga";

	Texture* pTexture = Texture::createEx(sFontFileName.c_str(), false);
	if(pTexture == NULL)
		return FALSE;

	///////////////////////////////////////////////////
	sFontFileName.replace("tga", "dat");
	RandomAccessFile* rafIn = new RandomAccessFile();
	bool bCanRead = rafIn->openForRead(sFontFileName.c_str());

	if(bCanRead) {
		CHARACTER_WIDTH = rafIn->readInt();
		CHARACTER_HEIGHT = rafIn->readInt();
		int iSize = rafIn->readInt();
		for(int i = 0; i < iSize; i++) {
			m_GlyphArray[i].read(rafIn);
		}
	}
	m_GlyphArray[0].width = CHARACTER_WIDTH;
	rafIn->close();
	///////////////////////////////////////////////////

	return (H_FONT)pTexture;
}

void WWidgetManager::setGLStates() {
	Camera* pCamera = EngineManager::getInstance()->getUICamera();
	pCamera->forceType(Camera::ORTHOGRAPHIC);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	setClip(((WContainer*)m_pBaseWindow)->getLeft(), ((WContainer*)m_pBaseWindow)->getTop(), ((WContainer*)m_pBaseWindow)->getWidth(), ((WContainer*)m_pBaseWindow)->getHeight());
}

void WWidgetManager::clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WWidgetManager::update(float deltaTimeMs) {
	if(m_pBaseWindow) {
		///////////////////////////////////////
		setGLStates();
		///////////////////////////////////////

		///////////////////////////////////////
		((WContainer*)m_pBaseWindow)->frameUpdate();
		((WContainer*)m_pBaseWindow)->frameRender();

		char str[255];
		sprintf(str, "FPS : %d", EngineManager::getInstance()->getFPS());
		drawStringFont(str, ((WContainer*)m_pBaseWindow)->getLeft() + 10, ((WContainer*)m_pBaseWindow)->getTop() + 20, 0);

		flush();
		m_FrameCount++;
		///////////////////////////////////////
	}
}

void WWidgetManager::flush() {
	if(m_pCurrentTexture != NULL)
	{
		m_pCurrentTexture->bind();

		//glInterleavedArrays( GL_T2F_V3F, 0, m_VB);
		//glInterleavedArrays( GL_T2F_V3F, 0, m_VB);
		glInterleavedArrays( GL_T2F_C4UB_V3F, 0, m_VB);
		//glInterleavedArrays( GL_T2F_C3F_V3F, 0, m_VB);
		glDrawArrays( GL_QUADS, 0, m_SpriteCount);

		// Disable vertex arrays (Note: glInterleavedArrays implicitly called
		// glEnableClientState for vertex, texture coord and color arrays)
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );

		glDisable(GL_TEXTURE_2D);

		m_SpriteCount = 0;
	}
}

void WWidgetManager::keyPressed(unsigned int iVirtualKeycode, unsigned short ch) {
	if(m_pBaseWindow)
		((WContainer*)m_pBaseWindow)->onKeyBDown(iVirtualKeycode, ch);
}

void WWidgetManager::keyReleased(unsigned int iVirtualKeycode, unsigned short ch) {
	if(m_pBaseWindow)
		((WContainer*)m_pBaseWindow)->onKeyBUp(iVirtualKeycode, ch);
}

void WWidgetManager::onMouseDown(int mCode, int x, int y) {
	((WContainer*)m_pBaseWindow)->onMouseDown(x, y, mCode);
}

void WWidgetManager::onMouseHover(int mCode, int x, int y) {
	if(m_pBaseWindow) {
		((WContainer*)m_pBaseWindow)->onMouseHover(mCode, x, y, lastMouseX, lastMouseY);

		lastMouseX = x;
		lastMouseY = y;
	}
}

void WWidgetManager::onMouseMove(int mCode, int x, int y) {
	if(m_pBaseWindow) {
		((WContainer*)m_pBaseWindow)->onMouseMove(mCode, x, y, lastMouseX, lastMouseY);

		lastMouseX = x;
		lastMouseY = y;
	}
}

void WWidgetManager::onMouseUp(int mCode, int x, int y) {
	if(m_pBaseWindow)
		((WContainer*)m_pBaseWindow)->onMouseUp(x, y, mCode);
}

void WWidgetManager::onMouseRDown(int mCode, int x, int y) {
}

void WWidgetManager::onMouseRUp(int mCode, int x, int y) {
}

void WWidgetManager::onMouseWheel(WPARAM wParam, LPARAM lParam) {
	if(m_pBaseWindow)
		((WContainer*)m_pBaseWindow)->onMouseWheel(wParam, lParam);
}

void WWidgetManager::GetClipBounds(RectF* reclaimRect) {
	reclaimRect->X = WWidgetManager::m_ClipRect.X;
	reclaimRect->Y = WWidgetManager::m_ClipRect.Y;
	reclaimRect->Width = WWidgetManager::m_ClipRect.Width;
	reclaimRect->Height = WWidgetManager::m_ClipRect.Height;
}

void WWidgetManager::setClip(int x, int y, int w, int h ) {
	WWidgetManager::m_ClipRect.X = x;
	WWidgetManager::m_ClipRect.Y = y;
	WWidgetManager::m_ClipRect.Width = w;
	WWidgetManager::m_ClipRect.Height = h;
}

bool WWidgetManager::readMap(char* filePathAndName) {
	/////////////////// LOADING GAME FONTS
	RandomAccessFile* rafIn = new RandomAccessFile();
	bool bCanRead = rafIn->openForRead(filePathAndName);

	if(!bCanRead)
		return false;

	CCString singleLine;

	int iWidgetCount = 0;
	int READ_STATE	 = 0;

	while(rafIn->isEOF() == 0) {

		singleLine = (char*)rafIn->readLine();
		singleLine.trim();

		if(CCString::startsWith(singleLine.c_str(), "WIDGET")) {

			WIDGET*			widget = new WIDGET();
			BASESKIN*		baseSkin = NULL;
			CHILD*			child = NULL;
			CLIENT_AREA*	clientArea = NULL;
			SIMPLE_TEXT*	simpleText = NULL;

			sscanf(singleLine.c_str(), "WIDGET %s {\n", &widget->widgetName);

			while(true) {

				singleLine = (char*)rafIn->readLine();
				singleLine.trim();

				if(READ_STATE == STATE_READ_WIDGET && CCString::startsWith(singleLine.c_str(), "}")) {
					m_pWidgets.push_back(widget);
					break;
				}
				else
				if(CCString::startsWith(singleLine.c_str(), "BaseSkin")) {
					baseSkin = new BASESKIN();
					READ_STATE = STATE_READ_BASESKIN;
				}
				else
				if(CCString::startsWith(singleLine.c_str(), "Child")) {
					child = new CHILD();
					READ_STATE = STATE_READ_CHILD;
				}
				else
				if(CCString::startsWith(singleLine.c_str(), "ClientArea")) {
					clientArea = new CLIENT_AREA();
					READ_STATE = STATE_READ_CLIENTAREA;
				}
				else
				if(CCString::startsWith(singleLine.c_str(), "SimpleText")) {
					simpleText = new SIMPLE_TEXT();
					READ_STATE = STATE_READ_SIMPLE_TEXT;
				}

				switch(READ_STATE) {
					case STATE_READ_WIDGET:
						if(CCString::startsWith(singleLine.c_str(), "size")) {
							sscanf(singleLine.c_str(), "size \"%f %f\"\n", &widget->widgetSize.width, &widget->widgetSize.height);
						}
					break;
					case STATE_READ_CHILD:
						if(CCString::startsWith(singleLine.c_str(), "}")) {
							widget->childWindows.push_back(child);
							READ_STATE = STATE_READ_WIDGET;
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "skin")) {
							sscanf(	singleLine.c_str(), "skin %s\n", &child->sName);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "size")) {
							sscanf(	singleLine.c_str(), "size \"%f %f\"\n", &child->parentSize.width, &child->parentSize.height);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "posOffset")) {
							sscanf(	singleLine.c_str(), "posOffset \"%f %f %f %f\"\n", &child->posOffsets.x, &child->posOffsets.y, &child->posOffsets.w, &child->posOffsets.h);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "skinOffset")) {
							sscanf(	singleLine.c_str(), "skinOffset \"%f %f %f %f\"\n", &child->skinOffsets.x, &child->skinOffsets.y, &child->skinOffsets.w, &child->skinOffsets.h);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "align")) {
							ALIGN* align = new ALIGN();
							char hAlign[255];
							char vAlign[255];
							sscanf(	singleLine.c_str(), "align %s %s\n", &hAlign, &vAlign);

							child->align.iHAlign = getWidgetAlignInt(hAlign);
							child->align.iVAlign = getWidgetAlignInt(vAlign);
						}
					break;
					case STATE_READ_CLIENTAREA:
						if(CCString::startsWith(singleLine.c_str(), "}")) {
							widget->clientAreas.push_back(clientArea);
							READ_STATE = STATE_READ_WIDGET;
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "skin")) {
							sscanf(	singleLine.c_str(), "skin %s\n", &clientArea->sName);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "size")) {
							sscanf(	singleLine.c_str(), "size \"%f %f\"\n", &clientArea->parentSize.width, &clientArea->parentSize.height);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "posOffset")) {
							sscanf(	singleLine.c_str(), "posOffset \"%f %f %f %f\"\n", &clientArea->posOffsets.x, &clientArea->posOffsets.y, &clientArea->posOffsets.w, &clientArea->posOffsets.h);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "skinOffset")) {
							sscanf(	singleLine.c_str(), "skinOffset \"%f %f %f %f\"\n", &clientArea->skinOffsets.x, &clientArea->skinOffsets.y, &clientArea->skinOffsets.w, &clientArea->skinOffsets.h);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "align")) {
							ALIGN* align = new ALIGN();
							char hAlign[255];
							char vAlign[255];
							sscanf(	singleLine.c_str(), "align %s %s\n", &hAlign, &vAlign);

							clientArea->align.iHAlign = getWidgetAlignInt(hAlign);
							clientArea->align.iVAlign = getWidgetAlignInt(vAlign);
						}
					break;
					case STATE_READ_SIMPLE_TEXT:
						if(CCString::startsWith(singleLine.c_str(), "}")) {
							widget->strings.push_back(simpleText);
							READ_STATE = STATE_READ_WIDGET;
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "string")) {
							sscanf(	singleLine.c_str(), "string %s\n", &simpleText->string);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "posOffset")) {
							sscanf(	singleLine.c_str(), "posOffset \"%f %f %f %f\"\n", &simpleText->posOffsets.x, &simpleText->posOffsets.y, &simpleText->posOffsets.w, &simpleText->posOffsets.h);
						}
						//else
						//if(CCString::startsWith(singleLine.c_str(), "font")) {
						//	sscanf(	singleLine.c_str(), "font %s\n", &simpleText->font);
						//}
						else
						if(CCString::startsWith(singleLine.c_str(), "align")) {
							ALIGN* align = new ALIGN();
							char hAlign[255];
							char vAlign[255];
							sscanf(	singleLine.c_str(), "align %s %s\n", &hAlign, &vAlign);

							simpleText->align.iHAlign = getTextAlignInt(hAlign);
							simpleText->align.iVAlign = getTextAlignInt(vAlign);
						}
					break;
					case STATE_READ_BASESKIN:
						if(CCString::startsWith(singleLine.c_str(), "}")) {
							widget->skins.push_back(baseSkin);
							READ_STATE = STATE_READ_WIDGET;
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "size")) {
							sscanf(	singleLine.c_str(), "size \"%f %f\"\n", &baseSkin->parentSize.width, &baseSkin->parentSize.height);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "posOffset")) {
							sscanf(	singleLine.c_str(), "posOffset \"%f %f %f %f\"\n", &baseSkin->posOffsets.x, &baseSkin->posOffsets.y, &baseSkin->posOffsets.w, &baseSkin->posOffsets.h);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "skinOffset")) {
							sscanf(	singleLine.c_str(), "skinOffset \"%f %f %f %f\"\n", &baseSkin->skinOffsets.x, &baseSkin->skinOffsets.y, &baseSkin->skinOffsets.w, &baseSkin->skinOffsets.h);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "align")) {
							ALIGN* align = new ALIGN();
							char hAlign[255];
							char vAlign[255];
							sscanf(	singleLine.c_str(), "align %s %s\n", &hAlign, &vAlign);

							baseSkin->align.iHAlign = getWidgetAlignInt(hAlign);
							baseSkin->align.iVAlign = getWidgetAlignInt(vAlign);
						}
						else
						if(CCString::startsWith(singleLine.c_str(), "name")) {
							sscanf(	singleLine.c_str(), "name %s\n", &baseSkin->name);
						}
					break;
				}
			}
		}
	}

	/*
	for(int i = 0; i < m_pWidgets.size(); i++) {
		WIDGET* widget = (WIDGET*)m_pWidgets[i];
		
		printf("widgetName = %s\n",widget->widgetName);
		for(int i = 0; i < widget->childWindows.size(); i++) {
			CHILD* child = (CHILD*)widget->childWindows[i];
			printf("\tchildName = %s\n", child->sName);
			printf("\t\tchild PosOffsets = %f %f %f %f\n", child->posOffsets.x, child->posOffsets.y, child->posOffsets.w, child->posOffsets.h);
			printf("\t\tskin Offsets = %f %f %f %f\n", child->skinOffsets.x, child->skinOffsets.y, child->skinOffsets.w, child->skinOffsets.h);
		}
		for(int i = 0; i < widget->skins.size(); i++) {
			BASESKIN* baseskin = (BASESKIN*)widget->skins[i];
			printf("\tskinName = %s\n", baseSkin->name);
			printf("\t\tskin PosOffsets = %f %f %f %f\n", baseSkin->posOffsets.x, baseSkin->posOffsets.y, baseSkin->posOffsets.w, baseSkin->posOffsets.h);
			printf("\t\tskin Offsets = %f %f %f %f\n", baseSkin->skinOffsets.x, baseSkin->skinOffsets.y, baseSkin->skinOffsets.w, baseSkin->skinOffsets.h);
		}
	}
	*/

	return true;
}

int WWidgetManager::getWidgetAlignInt(char* sAlign) {
	if(strcmp(sAlign, "Left") == 0)
		return LEFT;
	else
	if(strcmp(sAlign, "Top") == 0)
		return TOP;
	else
	if(strcmp(sAlign, "Right") == 0)
		return RIGHT;
	else
	if(strcmp(sAlign, "Bottom") == 0)
		return BOTTOM;
	else
	if(strcmp(sAlign, "HStretch") == 0)
		return HSTRETCH;
	else
	if(strcmp(sAlign, "VStretch") == 0)
		return VSTRETCH;
	else
	if(strcmp(sAlign, "VCenter") == 0)
		return VCENTER;
	else
	if(strcmp(sAlign, "RELATIVE_X") == 0)
		return RELATIVE_X;
	else
	if(strcmp(sAlign, "RELATIVE_Y") == 0)
		return RELATIVE_Y;

	return LEFT;
}

int WWidgetManager::getTextAlignInt(char* sAlign) {
	if(strcmp(sAlign, "Left") == 0)
		return TXT_LEFT;
	else
	if(strcmp(sAlign, "Top") == 0)
		return TXT_TOP;
	else
	if(strcmp(sAlign, "Right") == 0)
		return TXT_RIGHT;
	else
	if(strcmp(sAlign, "Bottom") == 0)
		return TXT_BOTTOM;
	else
	if(strcmp(sAlign, "VCenter") == 0)
		return TXT_VCENTER;
	else
	if(strcmp(sAlign, "HCenter") == 0)
		return TXT_HCENTER;

	return TXT_LEFT;
}

void WWidgetManager::renderSkin(WIDGET* widget, BASESKIN* skinPtr, RectF* wndRect) {
	RectF skinRect;
	RectF idealRect;

	float parentW = widget->widgetSize.width;
	float parentH = widget->widgetSize.height;
	int hAlign = 0, vAlign = 0;

	skinRect.X = skinPtr->skinOffsets.x;
	skinRect.Y = skinPtr->skinOffsets.y;
	skinRect.Width = skinPtr->skinOffsets.w;
	skinRect.Height = skinPtr->skinOffsets.h;

	idealRect.X = skinPtr->posOffsets.x;
	idealRect.Y = skinPtr->posOffsets.y;
	idealRect.Width = skinPtr->posOffsets.w;
	idealRect.Height = skinPtr->posOffsets.h;

	hAlign = skinPtr->align.iHAlign;
	vAlign = skinPtr->align.iVAlign;

	RectF destRect;
	getDestinationRect(	destRect,
						parentW,
						parentH,
						wndRect,
						&idealRect, 
						hAlign, 
						vAlign
					);
	
	drawQuadU(m_pTextureCoreUI, destRect.X, destRect.Y, destRect.Width, destRect.Height, skinRect.X, skinRect.Y, skinRect.Width, skinRect.Height);
}

void WWidgetManager::renderChild(WIDGET* widget, CHILD* childPtr, RectF* wndRect) {
	RectF idealRect;

	float parentW = widget->widgetSize.width;
	float parentH = widget->widgetSize.height;
	int hAlign = 0, vAlign = 0;

	idealRect.X = childPtr->posOffsets.x;
	idealRect.Y = childPtr->posOffsets.y;
	idealRect.Width = childPtr->posOffsets.w;
	idealRect.Height = childPtr->posOffsets.h;

	hAlign = childPtr->align.iHAlign;
	vAlign = childPtr->align.iVAlign;

	RectF destRect;
	getDestinationRect(	destRect,
						parentW,
						parentH,
						wndRect,
						&idealRect, 
						hAlign, 
						vAlign
					);

	renderWidget(childPtr->sName, &destRect );
}

void WWidgetManager::renderChild(WIDGET* widget, CHILD* childPtr, RectF* wndRect, float x, float y) {
	RectF idealRect;

	float parentW = widget->widgetSize.width;
	float parentH = widget->widgetSize.height;
	int hAlign = 0, vAlign = 0;

	idealRect.X = childPtr->posOffsets.x;
	idealRect.Y = childPtr->posOffsets.y;
	idealRect.Width = childPtr->posOffsets.w;
	idealRect.Height = childPtr->posOffsets.h;

	hAlign = childPtr->align.iHAlign;
	vAlign = childPtr->align.iVAlign;

	RectF destRect;
	getDestinationRect(	destRect,
						parentW,
						parentH,
						wndRect,
						&idealRect, 
						hAlign, 
						vAlign
						);
	
	destRect.X = x;
	destRect.Y = y;

	renderWidget(childPtr->sName, &destRect );
}

void WWidgetManager::renderClientArea(WIDGET* widget, int i, RectF* wndRect) {
	float parentW = widget->widgetSize.width;
	float parentH = widget->widgetSize.height;

	RectF skinRect;
	RectF idealRect;
	RectF destRect;

	int hAlign = 0, vAlign = 0;

	if(widget->clientAreas.size() > 0 && i < widget->clientAreas.size()) {

			idealRect.X = widget->clientAreas[i]->posOffsets.x;
			idealRect.Y = widget->clientAreas[i]->posOffsets.y;
			idealRect.Width = widget->clientAreas[i]->posOffsets.w;
			idealRect.Height = widget->clientAreas[i]->posOffsets.h;

			hAlign = widget->clientAreas[i]->align.iHAlign;
			vAlign = widget->clientAreas[i]->align.iVAlign;

			getDestinationRect(	destRect,
								parentW,
								parentH,
								wndRect,
								&idealRect, 
								hAlign, 
								vAlign
								);
			Rect clientRect(destRect.X, destRect.Y, destRect.Width, destRect.Height);
			WWidgetManager::getInstance()->fillRect(255.0f, 255.0f, 0.0f, 255.0f, &clientRect);
	}
}

void WWidgetManager::renderWidget(const char* sWidgetName, RectF* wndRect) {
	int widgetId = getWidgetID(sWidgetName);
	WIDGET* widget = (WIDGET*)m_pWidgets[widgetId];
	
	if(widget->skins.size() > 0) {
		for(int j = 0; j < widget->skins.size(); j++) {
			renderSkin(widget, widget->skins[j], wndRect);
		}
	}
}

void WWidgetManager::getDestinationRect(RectF& destRect, float parentW, float parentH, RectF* wndRect, RectF* idealRect, int hAlign, int vAlign) {
	
	destRect.X = wndRect->X + idealRect->X;
	destRect.Y = wndRect->Y + idealRect->Y;
	destRect.Width = wndRect->Width;
	destRect.Height = wndRect->Height;

	switch(hAlign) {
		case LEFT:
			destRect.Width = idealRect->Width;
			break;
		case RIGHT:
			destRect.X = wndRect->X + wndRect->Width - (parentW - idealRect->X);
			destRect.Width = idealRect->Width;
			break;
		case HSTRETCH:
			{
				int wW = wndRect->Width - (parentW - (idealRect->X + idealRect->Width));
				destRect.Width = wW - idealRect->X;
			}
			break;
	}

	switch(vAlign) {
		case TOP:
			destRect.Height = idealRect->Height;
			break;
		case BOTTOM:
			destRect.Y = wndRect->Y + wndRect->Height - (parentH - idealRect->Y);
			destRect.Height = idealRect->Height;
			break;
		case VSTRETCH:
			{
				float hH = wndRect->Height - (parentH - (idealRect->Y + idealRect->Height));
				destRect.Height = hH - idealRect->Y;
			}
			break;
		case VCENTER:
			destRect.Y = (wndRect->Y + wndRect->Height) - ((parentH - idealRect->Y)/2);
			destRect.Height = idealRect->Height;
			break;
	}
}

WIDGET* WWidgetManager::getWidget(const char* sWidgetName) {
	for(int i = 0; i < m_pWidgets.size(); i++) {
		WIDGET* widget = (WIDGET*)m_pWidgets[i];
		if(strcmp(sWidgetName, widget->widgetName) == 0)
			return widget;
	}

	return NULL;
}

int WWidgetManager::getWidgetID(const char* sWidgetName) {
	for(int i = 0; i < m_pWidgets.size(); i++) {
		WIDGET* widget = (WIDGET*)m_pWidgets[i];
		if(strcmp(sWidgetName, widget->widgetName) == 0)
			return i;
	}

	return -1;
}

float WWidgetManager::getWidgetWidth(const char* sWidgetName) {
	WIDGET* widget = getWidget(sWidgetName);
	float width = 0.0f;

	if(widget) {
		width = widget->widgetSize.width;
	}

	return width;
}

float WWidgetManager::getWidgetHeight(const char* sWidgetName) {
	WIDGET* widget = getWidget(sWidgetName);
	float height = 0.0f;

	if(widget) {
		height = widget->widgetSize.height;
	}

	return height;
}

void WWidgetManager::drawQuadU(	Texture* pTexture, 
													float posX, float posY, float posW, float posH,
													float texX, float texY, float texW, float texH
) {
	if(pTexture != m_pCurrentTexture)
		WWidgetManager::getInstance()->flush();

	m_pCurrentTexture = pTexture;

	GLuint TEX_WIDTH = m_pCurrentTexture->getWidth();
	GLuint TEX_HEIGHT = m_pCurrentTexture->getHeight();
	GLfloat tX = texX/TEX_WIDTH, tY = texY/TEX_HEIGHT, tW = texW/TEX_WIDTH, tH = texH/TEX_HEIGHT;

	RectF drawRect(posX, posY, posW, posH);
	RectF posIntersectRect(0, 0, posW, posH);
	RectF::Intersect(posIntersectRect, m_ClipRect, drawRect);

	if(posIntersectRect.Width > 0 && posIntersectRect.Height > 0) {
		////////// If IntersectRect width is less than texture width
		if(posIntersectRect.Width < texW) {
			texW = posIntersectRect.Width;
			tW = texW/TEX_WIDTH;

			////////// If posX < clipRectX, shift texture X equal to the diff in X
			if(posIntersectRect.X > posX) {
				texX += (posIntersectRect.X - posX);
				tX = texX/TEX_WIDTH;
			}
		}

		////////// If IntersectRect height is less than texture height
		if(posIntersectRect.Height < texH) {
			texH = posIntersectRect.Height;
			tH = texH/TEX_HEIGHT;
			
			////////// If posY < clipRectY, shift texture Y equal to the diff in Y
			if(posIntersectRect.Y > posY) {
				texY += (posIntersectRect.Y - posY);
				tY = texY/TEX_HEIGHT;
			}
		}

		float zZ = 0.0f;

		//using VertexT2F_C4UB_V3F
		{	m_VB[m_SpriteCount+0].tu = tX;	m_VB[m_SpriteCount+0].tv = tY;
			m_VB[m_SpriteCount+0].color = m_iRenderColor;
			m_VB[m_SpriteCount+0].vx = posIntersectRect.X;	m_VB[m_SpriteCount+0].vy = posIntersectRect.Y;	m_VB[m_SpriteCount+0].vz = zZ;
		}
		{	m_VB[m_SpriteCount+1].tu = tX;		m_VB[m_SpriteCount+1].tv = tY+tH;	
			m_VB[m_SpriteCount+1].color = m_iRenderColor;
			m_VB[m_SpriteCount+1].vx = posIntersectRect.X;	m_VB[m_SpriteCount+1].vy = posIntersectRect.Y+posIntersectRect.Height;	m_VB[m_SpriteCount+1].vz = zZ;
		}
		{	m_VB[m_SpriteCount+2].tu = tX+tW;	m_VB[m_SpriteCount+2].tv = tY+tH;	
			m_VB[m_SpriteCount+2].color = m_iRenderColor;
			m_VB[m_SpriteCount+2].vx = posIntersectRect.X+posIntersectRect.Width;	m_VB[m_SpriteCount+2].vy = posIntersectRect.Y+posIntersectRect.Height;	m_VB[m_SpriteCount+2].vz = zZ;
		}
		{	m_VB[m_SpriteCount+3].tu = tX+tW;	m_VB[m_SpriteCount+3].tv = tY;		
			m_VB[m_SpriteCount+3].color = m_iRenderColor;
			m_VB[m_SpriteCount+3].vx = posIntersectRect.X+posIntersectRect.Width;	m_VB[m_SpriteCount+3].vy = posIntersectRect.Y;							m_VB[m_SpriteCount+3].vz = zZ;
		}
		
		m_SpriteCount += 4;
	}
}

int WWidgetManager::getStringWidthTillPos(const char* cStr, int iPos) {

	int iRetWidth = 0;
	if(cStr != NULL && iPos <= strlen(cStr)) {
		for(int i = 0; i < iPos; i++) {
			iRetWidth += getCharWidth(cStr[i]);
		}
	}

	return iRetWidth;
}

int WWidgetManager::getGlyphU(int c) {
	if(c >= ASCII_START_INDEX && c <= ASCII_END_INDEX) {
		return m_GlyphArray[c-32].uvCoords[0];
	}

	return -1;
}

int WWidgetManager::getGlyphV(int c) {
	if(c >= ASCII_START_INDEX && c <= ASCII_END_INDEX) {
		return m_GlyphArray[c-32].uvCoords[1];
	}

	return -1;
}

void WWidgetManager::drawStringFont(const char* cStr, int x, int y, int anchor) {
	int strWidth = getStringWidthTillPos(cStr, strlen(cStr));

	int X = 0, Y = 0;

	int HLEFT = 0;
	int HCENTER = 1;
	int HRIGHT = 2;

	int xX = x;
	int yY = y;

	if(anchor == HCENTER)
		xX = xX - (strWidth/2);
	else
	if(anchor == HRIGHT)
		xX = xX - strWidth;

	for(int i = 0 ; i < strlen(cStr); i++) {
		char c = cStr[i];

		if(c == '\r') {
			continue;
		}

		int CHAR_WIDTH = getCharWidth(c);

		if(c > ' ') {
			X = getGlyphU(c);
			Y = getGlyphV(c);

			//WWidgetManager::setColor(0xff000000);//ABGR
			WWidgetManager::drawFont(xX, yY, CHAR_WIDTH, WWidgetManager::CHARACTER_HEIGHT, X, Y);
			//WWidgetManager::resetColor();

			xX += CHAR_WIDTH;
		}
		else
		if(c == ' ' || c == '\t') {
			xX += CHAR_WIDTH;
		}
		else
		if(c == '\n') {
			xX = x;
			yY += WWidgetManager::CHARACTER_HEIGHT + 3;
		}
	}
}

int WWidgetManager::getCharWidth(int c) {

	if(c >= 32 && c <= 126) {
		unsigned int width = m_GlyphArray[c - 32].width;

		if(c == ' ' || c == '\t')
			width >>= 2;
		return width + 1;
	}

	return m_GlyphArray[0].width;
}

void WWidgetManager::fillRect(Color* color, Rect* rect) {
	fillRect(color->r, color->g, color->b, color->a, rect);
}

void WWidgetManager::fillRect(float r, float g, float b, float a, Rect* rect) {
#ifdef USE_GL_10
	glPushAttrib( GL_CURRENT_BIT );
	
	glColor4f(r/255.0, g/255.0, b/255.0, a/255.0);

	float x = (float)rect->X;
	float y = (float)rect->Y;
	float w = (float)rect->Width;
	float h = (float)rect->Height;

	glBegin(GL_QUADS);						// Draw A Quad
		glVertex3f(x,	y,		0);			// Top Left
		glVertex3f(x,	y+h,	0);			// Top Right
		glVertex3f(x+w, y+h,	0);			// Bottom Right
		glVertex3f(x+w, y,		0);			// Bottom Left
	glEnd();								// Done Drawing The Quad

	glPopAttrib();
#endif
	setColor( ((int)a << 24) | ((int)b << 16) | ((int)g << 8) | (int)r );//ABGR
	drawQuadU(m_pTextureCoreUI, rect->X, rect->Y, rect->Width, rect->Height, m_ColorWhiteUV.u, m_ColorWhiteUV.v, 1, 1);
	resetColor();
}

void WWidgetManager::drawRect(float r, float g, float b, float a, Rect* rect, int iStrokeWidth) {
	drawHorizontalLine(r, g, b, a, rect->X, rect->Y, rect->Width, iStrokeWidth);
	drawVerticalLine(r, g, b, a, rect->X + rect->Width - iStrokeWidth, rect->Y, rect->Height, iStrokeWidth);
	drawHorizontalLine(r, g, b, a, rect->X, rect->Y + rect->Height - iStrokeWidth, rect->Width, iStrokeWidth);
	drawVerticalLine(r, g, b, a, rect->X, rect->Y, rect->Height, iStrokeWidth);
}

void WWidgetManager::drawHorizontalLine(float r, float g, float b, float a, int x, int y, int width, int iStrokeWidth) {
	
	setColor( ((int)a << 24) | ((int)b << 16) | ((int)g << 8) | (int)r );//ABGR
	drawQuadU(m_pTextureCoreUI, x, y, width, iStrokeWidth, m_ColorWhiteUV.u, m_ColorWhiteUV.v, 1, 1);
	resetColor();
}

void WWidgetManager::drawVerticalLine(float r, float g, float b, float a, int x, int y, int height, int iStrokeWidth) {

	setColor( ((int)a << 24) | ((int)b << 16) | ((int)g << 8) | (int)r );//ABGR
	drawQuadU(m_pTextureCoreUI, x, y, iStrokeWidth, height, m_ColorWhiteUV.u, m_ColorWhiteUV.v, 1, 1);
	resetColor();
}

void WWidgetManager::drawVerticalLine(float x1, float y1, float x2, float y2) {
	drawQuadU(m_pTextureCoreUI, x1, y1, 1, (y2 - y1), 675, 50, 1, 1);
}

int WWidgetManager::getNextWhitespacePos(const char* str, int curPos, bool isLeft) {

	int len = 0;
	if(isLeft) {
		for(int i = curPos; i > 0; i--) {

			char ch = str[i];
			if(ch == '\r')
				continue;
			if(ch == ' ' || ch == '\t' || ch == '\n')
				return i;
		}

		return 0;
	}
	else {
		if(curPos == strlen(str))
			return strlen(str);

		for(int i = curPos; i < strlen(str); i++) {

			char ch = str[i];
			if(ch == '\r')
				continue;
			if(ch == ' ' || ch == '\t' || ch == '\n')
				return i;
		}

		return strlen(str)-1;
	}
}

void WWidgetManager::drawFont(int xX, int yY, int charW, int charH, int tX, int tY) {
	drawQuadU((Texture*)m_pCurrentFont, xX, yY, charW, charH, tX, tY, charW, charH);
}

void WWidgetManager::setCallback(YAGUICallback wndProc) {
	if(wndProc != NULL)
		m_lpfnWWndProc = wndProc;
}

L_RESULT WWidgetManager::onEvent(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if(m_lpfnWWndProc != NULL)
		return m_lpfnWWndProc(hWnd, msg, wParam, lParam);
}

H_WND WWidgetManager::GetWindow(int ID_WINDOW) {
	if(ID_WINDOW == 0)
		return (H_WND)m_pBaseWindow;
	else
		return ((WContainer*)m_pBaseWindow)->GetWindow(ID_WINDOW);
}

H_WND WWidgetManager::FindWindowQ(LPCSTR lpClassName, LPCSTR lpWindowName) {
	return ((WContainer*)m_pBaseWindow)->FindWindowQ(lpClassName, lpWindowName);
}

void WWidgetManager::setCursor(int MOUSE_IDC) {

	switch(MOUSE_IDC) {
		case  IDC__ARROW:
		{
			::SetCursor(m_hCurArrow);
		}
		break;
		case  IDC__IBEAM:
		{
			::SetCursor(m_hCurIBeam);
		}
		break;
		case  IDC__CROSS:
		{
			::SetCursor(m_hCurCross);
		}
		break;
		case  IDC__SIZEWE:
		{
			::SetCursor(m_hCurSizeWE);
		}
		break;
		case  IDC__SIZENESW:
		{
			::SetCursor(m_hCurSizeNESW);
		}
		break;
		case  IDC__SIZENWSE:
		{
			::SetCursor(m_hCurSizeNWSE);
		}
		break;
	}
}

WWidgetManager::~WWidgetManager() {

}

#endif