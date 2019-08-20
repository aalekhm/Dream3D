/*
_____________________________________________________________________________________
________                                 ________ ________   
\______ \_______   ____ _____    _____   \_____  \\______ \  
 |    |  \_  __ \_/ __ \\__  \  /     \    _(__  < |    |  \ 
 |    `   \  | \/\  ___/ / __ \|  Y Y  \  /       \|    `   \
/_______  /__|    \___  >____  /__|_|  / /______  /_______  /
        \/            \/     \/      \/         \/        \/ 
_____________________________________________________________________________________
/*
                               (
                      .-'''-..' \                      _____           _____
            _______ .'       -   \                 ,ad8PPPP88b,     ,d88PPPP8ba,
          <<<<<<<< );__   ,,,_)   \               d8P"      "Y8b, ,d8P"      "Y8b
             <<<<<<<<< ) ;C  /     \             dP'           "8a8"           `Yd
               <<<<<< (.-'-.  )====_)_=======>   8(              "              )8
                 <<<<< \    '''''''   )          I8                             8I
                 ;  <<<     .......__/            Yb,                         ,dP
            .-'''         (         )              "8a,                     ,a8"
         .-'              ;.       /                 "8a,                 ,a8"
        /  .-'     .     =  .     /                    "Yba             adP"
    _-''\_/         '. .'    .   /                       `Y8a         a8P'
 .-'  )  ;\          '''.     . /                          `88,     ,88'
;   .''''  `.       '    ;     (                             "8b   d8"  Normand
O -'        .'''       .'                                     "8b d8"   Veilleux
          .'   .-'''''`      (__)))                            `888'
          'o-'           (  )mikey(__)))                         "
*/
#include "Dream3DTest.h"
#include "Engine/Model.h"
#include "Engine/Mesh.h"
#include "Engine/MeshPart.h"
#include "Engine/MeshBatch.h"
#include "Engine/SpriteBatch.h"
#include "Engine/VertexAttributeBinding.h"
#include "Engine/Texture.h"
#include "Engine/TGA.h"
#include "Engine/MeshObjLoader.h"
#include "Engine/FrameBuffer.h"

#include "Engine/Properties.h"
#include "Engine/MD5Model.h"
#include "Engine/MD5Animation.h"
#include "Engine/Material.h"
#include "Engine/Technique.h"
#include "Engine/MaterialParameter.h"

#include "Engine/Light.h"

#ifdef USE_YAGUI
#include "Engine/UI/WComponentFactory.h"
#endif

#define CARTOON_TGA			"data/cartoon.tga"
#define COLOURFUL_TGA		"data/ColorFul_2048x1300.tga"
#define CORE_UI				"data/core.tga"

struct SpotLight
{
	Node*		m_pNode;
	Vector3		m_vPosition;
	Vector3		m_vColour;
	float		m_fInnerAngleCos;
	float		m_fOuterAngleCos;
};

// Declare our game instance
Dream3DTest game;

void initMD5Models(Scene* pScene);
void initLights();
void drawTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3);
void drawGrid(int iSize, float fStep);
void drawLine(const Vector3& p1, const Vector3& p2);

Node* createHearthStoneCoolShaderModelNode(const char* pMaterial);
Node* magniQuadModelNode;
Node* medivhQuadModelNode;
Node* testShaderQuadModelNode;

Node* createTriangleModelNode();
Node* triangleModelNode;

Node* createTriangleStripModelNode(const char* pMaterial);
Node* triangleStripModelNode;

Node* createLineStripModelNode();
Node* lineStripModelNode;

Node* createLineModelNode();
Node* lineModelNode;

Node* createCubeModelIndexedNode(float size);
Node* cubeModelNode;

Node* createQuadIndexedNode();
Node* quadModelIndexedNode;

Node* createObjNode();
Node* objModelNode;

Node* createGrid(unsigned int iSize, float fStep = 1.0f);
Node* g_pGridNode;

Node* objMonkeyNode;

std::vector<MD5Model*>	v_pMD5Models;

SpriteBatch* createSpriteBatch();
SpriteBatch* gSpriteBatch;

Texture*  gTexture0;
Texture*  gTexture1;

FrameBuffer*	gFrameBuffer;
SpriteBatch*		gFBOSpriteBatch;

/////////////////////////////////////////////////////////////////////////////////////
Logger::Logger()
: m_pLogFile(NULL)
{
}

Logger* Logger::create(const char* pLogFileName) {

	GP_ASSERT( pLogFileName || strlen(pLogFileName) > 0 );

	Logger* pLogger = new Logger();
	pLogger->m_pLogFile = new RandomAccessFile();
	bool bCanWrite = pLogger->m_pLogFile->openForWrite(pLogFileName);
	if (bCanWrite) {
		return pLogger;
	}

	return NULL;
}

void Logger::log(unsigned int iIntVal) const {

	m_pLogFile->writeInt(iIntVal);
}

void Logger::log(float fFloatVal) const {

	char* pStr = new char[8];
	memset(pStr, 0, sizeof(char) * 8);
	sprintf(pStr, "%8f\n", fFloatVal);

	m_pLogFile->writeBytes(pStr);

	SAFE_DELETE( pStr );
}

void Logger::log(const Vector2& vVector2Val) const {

	char* pStr = new char[32];
	memset(pStr, 0, sizeof(char)* 32);
	sprintf(pStr, "%f, %f\n", vVector2Val.x, vVector2Val.y);

	m_pLogFile->writeBytes(pStr);

	SAFE_DELETE(pStr);
}

void Logger::log(const Vector3& vVector3Val) const {

	char* pStr = new char[32];
	memset(pStr, 0, sizeof(char)* 32);
	sprintf(pStr, "%f, %f, %f\n", vVector3Val.x, vVector3Val.y, vVector3Val.z);

	m_pLogFile->writeBytes(pStr);

	SAFE_DELETE(pStr);
}

Logger::~Logger() {

	if (m_pLogFile) {
		m_pLogFile->close();
		m_pLogFile = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////////////

#ifdef USE_YAGUI
Node* gCanvasCameraNode;

L_RESULT CALLBACK UICallback(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {
		case WN__CREATE:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {
					case 1103:
					{
						Rect YAGUIRect;
						H_WND hYAGUIWnd = GetWindowQ(0);
						int iRet = SendMessageQ(hYAGUIWnd, WM__GETRECT, (WPARAM)NULL, (LPARAM)&YAGUIRect);

						Rect boundsRect;
						iRet = SendMessageQ(hWnd, WM__GETRECT, (WPARAM)NULL, (LPARAM)&boundsRect);
						
						Camera* pCanvasCamera = Camera::createPerspective(boundsRect.X, boundsRect.Y, boundsRect.Width, boundsRect.Height, 45.0f, 0.01f, 10.0f);
						gCanvasCameraNode = Node::create("Canvas Camera");
						gCanvasCameraNode->setCamera(pCanvasCamera);

						return WM__OKAY;
					}
					break;
				}
			}
			break;
		case WN__PAINT:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {
					case 1103:
					{
						Rect boundsRect;
						int iRet = SendMessageQ(hWnd, WM__GETRECT, (WPARAM)NULL, (LPARAM)&boundsRect);
						
						//////////////////////////// 3D SCENE
						Camera* pCamera = gCanvasCameraNode->getCamera();
						pCamera->setCameraValues(boundsRect.X, boundsRect.Y, boundsRect.Width, boundsRect.Height, 45.0f, 0.01f, 10.0f);
						pCamera->setType(Camera::PERSPECTIVE);
						{
							glClearColor(0.0f, 0.0f, 0.0f, 1);
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
							
							drawTriangle(Vector3(0, 0.5f, -1.5f), Vector3(-0.5f, 0, -1.5f), Vector3(0.5f, 0, -1.5f));
						}
						//////////////////////////////////////////////////

						//////////////////////////// 2D SCENE
						pCamera->setType(Camera::ORTHOGRAPHIC);
						{
							//FillRect(&boundsRect, 255, 255, 255, 255);
							//DrawRect(&boundsRect, 255, 0,	0,	 255);

							SetColorQ(255, 0, 0, 255);
							DrawString("Hi There !!!", boundsRect.X + 50, boundsRect.Y + 10, 0);
							ResetColorQ();
						}
						//////////////////////////////////////////////////

						return WM__OKAY;
					}
					break;
				}
			}
			break;
		case WN__MOVE:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {
				}
			}
			break;
		case WN__SIZE:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {
				}
			}
			break;
		//WButton
		case WM_BTN_LBUTTONDOWN:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {

				}
			}
			break;
		case WM_BTN_MBUTTONDOWN:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {

				}
			}
			break;
		case WM_BTN_RBUTTONDOWN:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {

				}
			}
			break;
		case WM_BTN_LBUTTONUP:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {
				case 1101:
					{
						////////////// WM__GETTEXTLENGTH
						H_WND hwnd = GetWindowQ(1966);
						if(hwnd != NULL) {
							//int len = SendMessageQ(hwnd, WM__GETTEXTLENGTH, (WPARAM)NULL, (LPARAM)NULL);

							////////////////// WM__GETTEXT
							//char* str = new char[len + 1];
							//memset(str, 0, len + 1);

							//int iRet = SendMessageQ(hwnd, WM__GETTEXT, (WPARAM)(len + 1), (LPARAM)str);
							//printf("str = %s", str);

							//////////////////// WM__SETTEXT
							//str = "Static Text Changed...";
							//iRet = SendMessageQ(hwnd, WM__SETTEXT, (WPARAM)NULL, (LPARAM)str);
							//
							//Rect boundsRect;
							//H_WND hwndButton = GetWindowQ(121);
							//iRet = SendMessageQ(hwndButton, WM__GETRECT, (WPARAM)NULL, (LPARAM)&boundsRect);
							//iRet = SendMessageQ(hwndButton, BM__CLICK, (WPARAM)NULL, (LPARAM)&boundsRect);
							//iRet = SendMessageQ(hwndButton, BM__DISABLE, (WPARAM)NULL, (LPARAM)NULL);
							//iRet = SendMessageQ(hwndButton, BM__GET_STATE, (WPARAM)NULL, (LPARAM)NULL);
							//DWORD dwPos = 0;
							//iRet = SendMessageQ(hwndButton, WM__GETPOS, (WPARAM)NULL, (LPARAM)&dwPos);
							//boundsRect.X = (dwPos & 0xffff);
							//boundsRect.Y = (dwPos >> 16) & 0xffff;
							//boundsRect.X += 10;
							//boundsRect.Y += 10;
							//dwPos = 0;
							//dwPos |= (boundsRect.X & 0xffff);
							//dwPos |= ((boundsRect.Y & 0xffff) << 16);
							//iRet = SendMessageQ(hwndButton, WM__SETPOS, (WPARAM)NULL, (LPARAM)dwPos);

							//H_WND hwndCheckBox = GetWindowQ(1102);
							//iRet = SendMessageQ(hwndCheckBox, WM__GETRECT, (WPARAM)NULL, (LPARAM)&boundsRect);
							//iRet = SendMessageQ(hwndCheckBox, BM__CLICK, (WPARAM)NULL, (LPARAM)&boundsRect);
							//iRet = SendMessageQ(hwndCheckBox, BM__DISABLE, (WPARAM)NULL, (LPARAM)NULL);
							//iRet = SendMessageQ(hwndCheckBox, BM__GET_STATE, (WPARAM)NULL, (LPARAM)NULL);


							/////////WFRAME - WWINDOW//////
							//H_WND hwndWFrame = GetWindowQ(1212);
							//DWORD dwSize = 0;
							//iRet = SendMessageQ(hwndWFrame, WM__GETSIZE, (WPARAM)NULL, (LPARAM)&dwSize);
							//int width = (dwSize & 0xffff);
							//int height = (dwSize >> 16) & 0xffff;
							//width -= 200;
							//height += 200;
							//dwSize = 0;
							//dwSize |= (width & 0xffff);
							//dwSize |= ((height & 0xffff) << 16);
							//iRet = SendMessageQ(hwndWFrame, WM__SETSIZE, (WPARAM)NULL, (LPARAM)dwSize);

							///////COMBO BOX //////
							//int iListSize = SendMessageQ(hwnd, CBM__ADDSTRING, NULL, (LPARAM)"Appended ComboBox Item... !!!");
							//iListSize = SendMessageQ(hwnd, CBM__DELETESTRING, (WPARAM)2, (LPARAM)NULL);
							//int iIndex = SendMessageQ(hwnd, CBM__FINDSTRING, (WPARAM)-1, (LPARAM)"Inspy");
							//iIndex = SendMessageQ(hwnd, CBM__FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)"crisp");
							//iListSize = SendMessageQ(hwnd, CBM__GETCOUNT, (WPARAM)NULL, (LPARAM)NULL);
							//iIndex = SendMessageQ(hwnd, CBM__SETCURSEL, (WPARAM)3, (LPARAM)NULL);
							//iIndex = SendMessageQ(hwnd, CBM__GETCURSEL, (WPARAM)NULL, (LPARAM)NULL);

							//Rect listRect;
							//int iValue = SendMessageQ(hwnd, CB__GETDROPPEDCONTROLRECT, (WPARAM)NULL, (LPARAM)&listRect);
							//iValue = SendMessageQ(hwnd, CB__GETDROPPEDSTATE, (WPARAM)NULL, (LPARAM)NULL);
							//iValue = SendMessageQ(hwnd, CB__GETDROPPEDWIDTH, (WPARAM)NULL, (LPARAM)NULL);

							//DWORD dwRange = SendMessageQ(hwnd, CB__GETEDITSEL, (WPARAM)NULL, (LPARAM)NULL);
							//dwRange = 0
							//dwRange |= (1 & 0xffff);
							//dwRange |= ((3 & 0xffff) << 16);
							//iValue = SendMessageQ(hwnd, CB__SETEDITSEL, (WPARAM)NULL, (LPARAM)dwRange);
							//
							/////////
							//int iLength = SendMessageQ(hwnd, CB__GETLBTEXTLEN, (WPARAM)4, (LPARAM)NULL);

							//char* str = new char[iLength + 1];
							//memset(str, 0, iLength + 1);
							//iValue = SendMessageQ(hwnd, CB__GETLBTEXT, (WPARAM)4, (LPARAM)str);
							////////////////

							//iValue = SendMessageQ(hwnd, CB__SETITEMDATA, (WPARAM)1, (LPARAM)"Replaced Data !!!");

							//iValue = SendMessageQ(hwnd, CB__INSERTSTRING, (WPARAM)1, (LPARAM)"Inserted Data !!!");
							//iValue = SendMessageQ(hwnd, CB__SHOWDROPDOWN, (WPARAM)NULL, (LPARAM)NULL);

							//iIndex = SendMessageQ(hwnd, CB__SELECTSTRING, (WPARAM)-1, (LPARAM)"Insp");

							//iValue = SendMessageQ(hwnd, CB__RESETCONTENT, (WPARAM)NULL, (LPARAM)NULL);
							///////COMBO BOX //////

							///////TABBED PANE //////
							int iValue = SendMessageQ(hwnd, TAB__GETCURSEL, (WPARAM)NULL, (LPARAM)NULL);
							iValue = SendMessageQ(hwnd, TAB__SETCURSEL, (WPARAM)0, (LPARAM)NULL);
							iValue = SendMessageQ(hwnd, TAB__GETITEMCOUNT, (WPARAM)0, (LPARAM)NULL);

							TCIM_ tcim;
							iValue = SendMessageQ(hwnd, TAB__GETITEM, (WPARAM)0, (LPARAM)&tcim);
							if(iValue == WM__OKAY) {
								HWND hTabWnd = (HWND)tcim.lParam;
								iValue = SendMessageQ(hTabWnd, WM__GETTEXTLENGTH, (WPARAM)NULL, (LPARAM)NULL);
								char* str = new char[iValue + 1];
								memset(str, 0, iValue + 1);
								iValue = SendMessageQ(hTabWnd, WM__GETTEXT, (WPARAM)(iValue + 1), (LPARAM)str);
								bool b = true;
							}

							iValue = SendMessageQ(hwnd, TAB__DELETEITEM, (WPARAM)2, (LPARAM)NULL);

							char str[255];
							sprintf(str, "%s", "ADDED TAB !!!");
							tcim.lpzText = str;
							tcim.cchTextMax = strlen(str);
							iValue = SendMessageQ(hwnd, TAB__INSERTITEM, (WPARAM)1, (LPARAM)&tcim);
							if(iValue == WM__OKAY) {
								H_WND pTab = (H_WND)tcim.lParam;

								H_WND hWnd = 
									CreateComponentEx(	"WTextField", 
																"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
																WM_ANCHOR_TOPLEFT, 
																20,
																20, 
																200, 
																23,
																pTab, 
																HMENU(99), 
																NULL);
								((WTextField*)hWnd)->setComponentAsChild(true);
							}

							sprintf(str, "%s", "TAB RENAMED !!!");
							tcim.lpzText = str;
							tcim.cchTextMax = strlen(str);
							iValue = SendMessageQ(hwnd, TAB__SETITEMTITLE, (WPARAM)0, (LPARAM)&tcim);

							iValue = SendMessageQ(hwnd, WM__GETTEXT, (WPARAM)1, (LPARAM)&tcim);

							sprintf(str, "%s", "TAB SETTED !!!");
							tcim.lpzText = str;
							tcim.cchTextMax = strlen(str);
							iValue = SendMessageQ(hwnd, TAB__SETITEM, (WPARAM)2, (LPARAM)&tcim);
							if(iValue == WM__OKAY) {
								H_WND pTab = (H_WND)tcim.lParam;

								H_WND hWnd = 
									CreateComponentEx(	"WButton", 
																"Press Me !!!", 
																WM_ANCHOR_TOPLEFT, 
																20,
																20, 
																200, 
																23,
																pTab, 
																HMENU(909), 
																"Button");
							}

							///////TABBED PANE //////
							bool b = true;
						}
					}
					break;
				case 121:
					{
						bool b = true;
					}
					break;
				}
			}
			break;
		case WM_BTN_MBUTTONUP:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {

				}
			}
			break;
		case WM_BTN_RBUTTONUP:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {

				}
			}
			break;

			//WCheckBox
		case WM_CBX_CHECKED:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {

				}
			}
			break;
		case WM_CBX_UNCHECKED:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {

				}
			}
			break;

			//WTextField

			//WComboBox
		case WM_CBN_SELENDOK:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {
				case IDC_CB_FONT:
							WComboBox* pComboBox = (WComboBox*)hWnd;
							H_FONT hFont = CreateFontQ(pComboBox->getText(), 10, 96);
							if(hFont != NULL)
								SelectFontQ(hFont);
					break;
				}
			}
			break;
			//WTextBox
			//EM_GETLINE
			//EM_GETLINECOUNT
			//EM_GETLIMITTEXT
			//EM_LIMITTEXT
			//EM_GETPASSWORDCHAR
			//EM_SETPASSWORDCHAR
			//EM_GETRECT
			//EM_SETRECT
			//EM_GETSEL
			//EM_SETSEL
		}

	return 0;
}

void addDummyWindows_(H_WND hParent);
void addDummyWindows(H_WND hParent);
#endif

/////////////////////////////////////////
struct MeshBatchVertex
{
	Vector3 position;
	Vector3 color;
	float	s;
	float	t;

	MeshBatchVertex() { }

	MeshBatchVertex(const Vector3& position, const Vector3& color, const float s, const float t) : position(position), color(color), s(s), t(t) { }
};
Vector3 randomColor() {
	return Vector3(MATH_RANDOM_0_1(), MATH_RANDOM_0_1(), MATH_RANDOM_0_1());
}

MeshBatch* meshBatch;
MeshBatch* createMeshBatch();
std::vector<MeshBatchVertex> _meshBatchVertices;
unsigned short* _indices = NULL;
unsigned int _indicesCount = 0;
/////////////////////////////////////////

Dream3DTest::Dream3DTest() 
: m_pScene(NULL)
, m_pLogger(NULL)
{

}

Dream3DTest::~Dream3DTest() {
	//SAFE_DELETE( m_pScene );
	SAFE_DELETE( m_pLogger );
}

void Dream3DTest::initialize() {
	////////////////////// SCENE INITIALISATION
	m_pScene = Scene::create();

#ifdef USE_YAGUI
	//////////////////// YAGUI Related initializations !!!
	addUIListener(UICallback);
	addDummyWindows(GetWindowQ(0));
	////////////////////////////////////////////
#else
	Camera* pCamera = Camera::createPerspective(0, 0, getWidth(), getHeight(), 45.0f, 0.01f, 100.0f);
	Node* pCameraNode = Node::create("FirstPersonShooterCamera");
	pCameraNode->setCamera(pCamera);
	pCameraNode->setPosition(Vector3(0.0f, 0.5f, 1.0f));
	m_pScene->addNode(pCameraNode);

	m_pLogger = Logger::create("log.txt");
	GP_ASSERT( m_pLogger );
	//////////////////////////////////////////////

	//////////////////////////////////////////////
	glEnable(GL_DEPTH_TEST);

	magniQuadModelNode = createHearthStoneCoolShaderModelNode("data/box.material#magni");
	{
		magniQuadModelNode->setPosition(Vector3(0.0f, 0.0f, -3.0f));
		m_pScene->addNode(magniQuadModelNode);
	}

	medivhQuadModelNode = createHearthStoneCoolShaderModelNode("data/box.material#medivh");
	{
		medivhQuadModelNode->setPosition(Vector3(0.5f, 0.0f, -3.0f));
		m_pScene->addNode(medivhQuadModelNode);
	}

	testShaderQuadModelNode = createHearthStoneCoolShaderModelNode("data/box.material#testShader");
	{
		testShaderQuadModelNode->setPosition(Vector3(-0.5f, 0.0f, -3.0f));
		m_pScene->addNode(testShaderQuadModelNode);
	}

	g_pGridNode = createGrid(10, 0.25);
	{
		g_pGridNode->setPosition(Vector3(0.0f, 0.0f, 0.0f));
		m_pScene->addNode(g_pGridNode);
	}
	
	// Init Point Light
	{
		for (int i = 0; i < POINT_LIGHT_COUNT; i++)
		{
			m_pPointLight[i] = Light::createPointLight(Vector3(0.7f, 0.0f, 0.0f), 5.0f);

			Node* pLightNode = createCubeModelIndexedNode(0.1f);
			m_pPointLight[i]->setNode(pLightNode);
			m_pScene->addNode(pLightNode);
		}
		m_pPointLight[0]->getNode()->setPosition(Vector3(0.0, 0.0, 0.0));
	}

	// Init Spot Light
	{
		for (int i = 0; i < SPOT_LIGHT_COUNT; i++)
		{
			m_pSpotLight[i] = Light::createSpotLight(Vector3(0.7f, 0.0f, 0.0f), 2.0f, MATH_DEG_TO_RAD(5.0f), MATH_DEG_TO_RAD(10.0f));

			Node* pLightNode = createCubeModelIndexedNode(0.03f);
			m_pSpotLight[i]->setNode(pLightNode);
			m_pScene->addNode(pLightNode);
		}

		m_pSpotLight[0]->getNode()->setPosition(Vector3(0.0, 0.0, 3.0));
	}

	// Init Directional Light
	{
		for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++)
		{
			m_pDirectionalLight[i] = Light::createDirectionalLight(Vector3(0.0f, 0.0f, 0.4f));

			Node* pLightNode = createCubeModelIndexedNode(0.03f);
			m_pDirectionalLight[i]->setNode(pLightNode);
			m_pScene->addNode(pLightNode);
		}

		m_pDirectionalLight[0]->getNode()->rotateX(-45.0);
		m_pDirectionalLight[0]->getNode()->setPosition(Vector3(0.0, 0.0, 4.0));
	}

	/*
	triangleModelNode = createTriangleModelNode();
	{
		triangleModelNode->setPosition(Vector3(0.0f, 0.0f, 3.0f));
		m_pScene->addNode(triangleModelNode);
	}
	
	triangleStripModelNode = createTriangleStripModelNode("data/box.material#box");
	{
		triangleStripModelNode->setPosition(Vector3(-5.0f, 0.0f, 0.0f));
		m_pScene->addNode(triangleStripModelNode);
	}
	
	lineStripModelNode = createLineStripModelNode();
	{
		lineStripModelNode->setPosition(Vector3(0.0f, 0.0f, -5.0f));
		m_pScene->addNode(lineStripModelNode);
	}

	lineModelNode = createLineModelNode();
	{
		lineModelNode->setPosition(Vector3(0.0f, 0.0f, 5.0f));
		m_pScene->addNode(lineModelNode);
	}

	cubeModelNode = createCubeModelIndexedNode(0.25f);
	{
		cubeModelNode->rotateY(0.0f);
		cubeModelNode->setPosition(Vector3(0.0f, 0.0f, -1.0f));
		m_pScene->addNode(cubeModelNode);

		quadModelIndexedNode = createQuadIndexedNode();
		{
			quadModelIndexedNode->rotateY(45.0f);
			quadModelIndexedNode->setPosition(Vector3(0.0f, 0.0f, 0.0f));
		}

		cubeModelNode->addChild(quadModelIndexedNode);
	}

	objModelNode = createObjNode();
	{
		objModelNode->scale(0.1f);
		m_pScene->addNode(objModelNode);
	}
	//*/

	//meshBatch = createMeshBatch();
	//gSpriteBatch = createSpriteBatch();

	gTexture0 = Texture::create(COLOURFUL_TGA, false);
	gTexture1 = Texture::create(CORE_UI, false);

	{
		Properties* properties = Properties::create("data/box.material");//test.mat");
		if(properties != NULL)
			properties->print(0);

		//Properties* pNS = properties->getNamespace("scene", true);
		//const char* path = pNS->getString("path");
		//printf("path ============== %s\n", path);
		printf("__________________________________________________________________\n");

		Properties::printProperties(properties);

		Properties* pNS1 = NULL;
		pNS1 = properties->getNamespace("physics", true);
		if(pNS1 != NULL) {
			const char* spacename = pNS1->getNamespaceType();
			const char* id = pNS1->getID();
			printf("%s {\n", spacename);

			//Print all properties in this namespace.
			const char* name = pNS1->getNextProperty();
			const char* value = NULL;
			while(name != NULL) {

				value = pNS1->getString(name);
				printf("\t %s ==== %s\n", name, value);

				name = pNS1->getNextProperty();
			}
			printf("}");
		}
	}

	//initLights();
	loadSceneUsingAssimp(	"data/MD5Models/doom3/hellknight/hellknight.md5mesh",
							Vector3(0.0f,	0.0f,	-30.0f),
							0.005f,
							"data/box.material#testBumpMap");
	
	loadSceneUsingAssimp(	"data/OBJModels/lamp.obj",
							Vector3(0.0f,	0.0f,	30.0f),
							0.09f,
							"data/box.material#box");
	
	loadSceneUsingAssimp(	"data/OBJModels/lamp.obj",		
							Vector3(30.0f,	0.0f,	0.0f),
							0.09f,
							"data/box.material#box");

	loadSceneUsingAssimp(	"data/OBJModels/lamp.obj",
							Vector3(-30.0f, 0.0f,	0.0f),
							0.09f,
							"data/box.material#box");

#ifdef TEST_MD5_MODELS
	initMD5Models(m_pScene);
#endif
	//////////////////////////////////////////////
#endif

	gFrameBuffer = FrameBuffer::create("TEMP FBO_200x200", 512, 512);
	gFBOSpriteBatch = SpriteBatch::create(gFrameBuffer->getRenderTarget(0)->getTexture());
	GP_ASSERT( gFBOSpriteBatch );
}

struct AssimpVertexData {

	Vector3		m_vPosition;
	Vector3		m_vNormal;
	Vector3		m_vTangent;
	Vector3		m_vColour;

	float		m_fU, m_fV;
};

struct AssimpTextureData {
	
	unsigned int	m_iID;
	unsigned int	m_iType;
};

void Dream3DTest::processScene(aiMesh* pAIMesh, const aiScene* pAIScene, Vector3& vPosition, float fScale, const char* sMaterial) {

	std::vector<AssimpVertexData>		vVertexData;
	std::vector<unsigned int>	vIndices;
	std::vector<AssimpTextureData>	vTextures;

	for (unsigned i = 0; i < pAIMesh->mNumVertices; i++) {

		AssimpVertexData	tmpVertexData;
		Vector3		tmpVector3D;

		// Position
		tmpVector3D.x = pAIMesh->mVertices[i].x;
		tmpVector3D.y = pAIMesh->mVertices[i].y;
		tmpVector3D.z = pAIMesh->mVertices[i].z;
		tmpVertexData.m_vPosition = tmpVector3D;

		// Normals
		tmpVector3D.x = pAIMesh->mNormals[i].x;
		tmpVector3D.y = pAIMesh->mNormals[i].y;
		tmpVector3D.z = pAIMesh->mNormals[i].z;
		tmpVertexData.m_vNormal = tmpVector3D;

		// Tangents
		if (pAIMesh->mTangents) {

			tmpVector3D.x = pAIMesh->mTangents[i].x;
			tmpVector3D.y = pAIMesh->mTangents[i].y;
			tmpVector3D.z = pAIMesh->mTangents[i].z;
		}
		else {
			tmpVector3D.x = 1.0;
			tmpVector3D.y = tmpVector3D.z = 0.0;
		}
		tmpVertexData.m_vTangent = tmpVector3D;

		// Colours
		if (pAIMesh->mColors[0]) {

			tmpVector3D.x = pAIMesh->mColors[0][i].r;
			tmpVector3D.y = pAIMesh->mColors[0][i].g;
			tmpVector3D.z = pAIMesh->mColors[0][i].b;
		}
		else {
			tmpVector3D.x = 1.0;
			tmpVector3D.y = tmpVector3D.z = 0.0f;
		}
		tmpVertexData.m_vColour = tmpVector3D;

		// Textures
		if (pAIMesh->mTextureCoords[0]) {

			tmpVector3D.x = pAIMesh->mTextureCoords[0][i].x;
			tmpVector3D.y = pAIMesh->mTextureCoords[0][i].y;
		}
		else {
			tmpVector3D.x = tmpVector3D.y = tmpVector3D.z = 0.0f;
		}
		tmpVertexData.m_fU = tmpVector3D.x;
		tmpVertexData.m_fV = tmpVector3D.y;

		vVertexData.push_back(tmpVertexData);
	}

	for (int i = 0; i < pAIMesh->mNumFaces; i++) {

		aiFace pAIFace = pAIMesh->mFaces[i];
		for (int j = 0; j < pAIFace.mNumIndices; j++) { // 0..2, since we triangulated !!!

			vIndices.push_back(pAIFace.mIndices[j]);
		}
	}

	////////////////////////////////////////////////////////////
	unsigned int vertexCount = pAIMesh->mNumVertices;
	unsigned int indexCount = vIndices.size();
	VertexFormat::Element vertexElements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION,	VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::NORMAL,		VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::TANGENT,	VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::COLOR,		VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::TEXCOORD0,	VertexFormat::TWO)
	};
	unsigned int vertexElementCount = sizeof(vertexElements) / sizeof(VertexFormat::Element);

	Mesh* mesh = Mesh::createMesh(VertexFormat(vertexElements, vertexElementCount), vertexCount, false);
	if (mesh == NULL) {
		//GP_ERROR("Unable to create Mesh");
		return;
	}
	mesh->setPrimitiveType(Mesh::TRIANGLES);
	mesh->setVertexData((float*)vVertexData.data(), 0, vertexCount);

	MeshPart* meshPart = NULL;
	meshPart = mesh->addMeshPart(Mesh::TRIANGLES, Mesh::INDEX32, indexCount, false);
	if (meshPart != NULL)
		meshPart->setIndexData(vIndices.data(), 0, indexCount);

	Node* pNode = Node::create("Assimo_TestObj");
	{
		Model* pModel = Model::create(mesh);
		pNode->setModel(pModel);
		pModel->setMaterial(sMaterial, 0);

		Material* pMaterial = pModel->getMaterial(0);
		Technique* pTechnique = pMaterial->getTechnique();

		char sBuf[8];

		// Point Light
		for (int i = 0; i < POINT_LIGHT_COUNT; i++)
		{
			itoa(i, sBuf, 10);
			CCString cStr;

			cStr = "u_pointLightPosition[";	cStr += sBuf; cStr += "]";
			MaterialParameter* pPointLightPosition_MaterialParameter = pTechnique->getParameter(cStr.c_str());
			pPointLightPosition_MaterialParameter->bindValue(m_pPointLight[i]->getNode(), &Node::getTranslationWorld);

			cStr = "u_pointLightColour["; cStr += sBuf; cStr += "]";
			MaterialParameter* pPointLightColour_MaterialParameter = pTechnique->getParameter(cStr.c_str());
			pPointLightColour_MaterialParameter->setValue(m_pPointLight[i]->getLightColour());
		}

		// Spot Light
		for (int i = 0; i < SPOT_LIGHT_COUNT; i++)
		{
			itoa(i, sBuf, 10);
			CCString cStr;
			
			cStr = "u_spotLightPosition["; cStr += sBuf; cStr += "]";
			MaterialParameter* pSpotLightPosition_MaterialParameter = pTechnique->getParameter(cStr.c_str());
			pSpotLightPosition_MaterialParameter->bindValue(m_pSpotLight[i]->getNode(), &Node::getTranslationWorld);

			cStr = "u_spotLightDirection["; cStr += sBuf; cStr += "]";
			MaterialParameter* pSpotLightDirection_MaterialParameter = pTechnique->getParameter(cStr.c_str());
			pSpotLightDirection_MaterialParameter->bindValue(m_pSpotLight[i]->getNode(), &Node::getForwardVectorWorld);

			cStr = "u_spotLightColour["; cStr += sBuf; cStr += "]";
			MaterialParameter* pSpotLightColour_MaterialParameter = pTechnique->getParameter(cStr.c_str());
			pSpotLightColour_MaterialParameter->setValue(m_pSpotLight[i]->getLightColour());

			cStr = "u_spotLightRangeInverse["; cStr += sBuf; cStr += "]";
			MaterialParameter* pSpotLightRangeInverse_MaterialParameter = pTechnique->getParameter(cStr.c_str());
			pSpotLightRangeInverse_MaterialParameter->setValue(m_pSpotLight[i]->getRangeInverse());

			cStr = "u_spotLightInnerAngleCos["; cStr += sBuf; cStr += "]";
			MaterialParameter* pSpotLightInnerAngleCos_MaterialParameter = pTechnique->getParameter(cStr.c_str());
			pSpotLightInnerAngleCos_MaterialParameter->setValue(m_pSpotLight[i]->getInnerAngleCosine());

			cStr = "u_spotLightOuterAngleCos["; cStr += sBuf; cStr += "]";
			MaterialParameter* pSpotLightOuterAngleCos_MaterialParameter = pTechnique->getParameter(cStr.c_str());
			pSpotLightOuterAngleCos_MaterialParameter->setValue(m_pSpotLight[i]->getOuterAngleCosine());
		}

		// Directional Light
		{
			for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++)
			{
				itoa(i, sBuf, 10);
				CCString cStr;

				cStr = "u_directionalLightDirection["; cStr += sBuf; cStr += "]";
				MaterialParameter* pdirectionalLightDirection_MaterialParameter = pTechnique->getParameter(cStr.c_str());
				pdirectionalLightDirection_MaterialParameter->bindValue(m_pDirectionalLight[i]->getNode(), &Node::getForwardVectorWorld);

				cStr = "u_directionalLightColour["; cStr += sBuf; cStr += "]";
				MaterialParameter* pdirectionalLightColour_MaterialParameter = pTechnique->getParameter(cStr.c_str());
				pdirectionalLightColour_MaterialParameter->setValue(m_pDirectionalLight[i]->getLightColour());
			}
		}

		pNode->scale(fScale);
		pNode->setPosition(vPosition);
		m_pScene->addNode(pNode);

		objMonkeyNode = pNode;
	}
}

void Dream3DTest::recurseScene(aiNode* pAINode, const aiScene* pAIScene, Vector3& vPosition, float fScale, const char* sMaterial) {

	// Process
	for (unsigned int i = 0; i < pAINode->mNumMeshes; i++) {

		aiMesh* pMesh = pAIScene->mMeshes[pAINode->mMeshes[i]];
		processScene(pMesh, pAIScene, vPosition, fScale, sMaterial);
	}

	// Recursion
	for (unsigned int i = 0; i < pAINode->mNumChildren; i++) {

		recurseScene(pAINode->mChildren[i], pAIScene, vPosition, fScale, sMaterial);
	}
}

void Dream3DTest::loadSceneUsingAssimp(const char* pFilename, Vector3& vPosition, float fScale, const char* sMaterial)
{
	Assimp::Importer pAssimpImporter;

	/*
	// default pp steps
	unsigned int ppsteps =	aiProcess_CalcTangentSpace			| // calculate tangents and bitangents if possible
							aiProcess_JoinIdenticalVertices		| // join identical vertices/ optimize indexing
							aiProcess_ValidateDataStructure		| // perform a full validation of the loader's output
							aiProcess_ImproveCacheLocality		| // improve the cache locality of the output vertices
							aiProcess_RemoveRedundantMaterials	| // remove redundant materials
							aiProcess_FindDegenerates			| // remove degenerated polygons from the import
							aiProcess_FindInvalidData			| // detect invalid model data, such as invalid normal vectors
							aiProcess_GenUVCoords				| // convert spherical, cylindrical, box and planar mapping to proper UVs
							aiProcess_TransformUVCoords			| // preprocess UV transformations (scaling, translation ...)
							aiProcess_FindInstances				| // search for instanced meshes and remove them by references to one master
							aiProcess_LimitBoneWeights			| // limit bone weights to 4 per vertex
							aiProcess_OptimizeMeshes			| // join small meshes, if possible;
							aiProcess_SplitByBoneCount			| // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
							0;

	aiPropertyStore* props = aiCreatePropertyStore();
	aiSetImportPropertyFloat(props, AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 110.f);
	/*
	const aiScene* pAIScene = (aiScene*)aiImportFileExWithProperties(	pFilename,
																		////ppsteps |				 // default pp steps
																		aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
																		aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
																		aiProcess_Triangulate | // triangulate polygons with more than 3 edges
																		//aiProcess_ConvertToLeftHanded    | // convert everything to D3D left handed space
																		aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives
																		0,
																		//aiProcessPreset_TargetRealtime_MaxQuality,
																		NULL,
																		props);
	//*/
	const aiScene* pAIScene = pAssimpImporter.ReadFile(	pFilename,
														aiProcess_GenSmoothNormals
														| aiProcess_Triangulate
														| aiProcess_CalcTangentSpace
														| aiProcess_FlipUVs
													);

	bool bLoadSucess = (((pAIScene->mFlags | AI_SCENE_FLAGS_INCOMPLETE) == 0) || pAIScene->mRootNode != NULL);
	GP_ASSERT(bLoadSucess);
	if (bLoadSucess)
	{
		recurseScene(pAIScene->mRootNode, pAIScene, vPosition, fScale, sMaterial);
	}
}

#ifdef TEST_MD5_MODELS
void initMD5Models(Scene* pScene) {
	for(int i = 0; i < 1; i++) {
		MD5Model* pMD5Model = new MD5Model();

		//md5ModelNode = m_pMD5Model->loadModel("data/MD5Models/boblamp/bob_lamp_update/bob_lamp_update.md5mesh");
		//	m_pMD5Model->loadAnim("data/MD5Models/boblamp/bob_lamp_update/bob_lamp_update.md5anim");
		//	md5ModelNode->getModel()->setMaterial("data/MD5Models/boblamp/bob_lamp_update/bob_body.material", 0);
		//	md5ModelNode->getModel()->setMaterial("data/MD5Models/boblamp/bob_lamp_update/bob_head.material", 1);
		//	md5ModelNode->getModel()->setMaterial("data/MD5Models/boblamp/bob_lamp_update/bob_helmet.material", 2);
		//	md5ModelNode->getModel()->setMaterial("data/MD5Models/boblamp/bob_lamp_update/lantern.material", 3);
		//	md5ModelNode->getModel()->setMaterial("data/MD5Models/boblamp/bob_lamp_update/lantern_top.material", 4);
		//	md5ModelNode->getModel()->setMaterial("data/MD5Models/boblamp/bob_lamp_update/bob_body.material", 5);


		Node* pMd5ModelNode = pMD5Model->loadModel("data/MD5Models/doom3/hellknight/hellknight1.md5mesh");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/attack2.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/attack3.md5anim");
		////m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/chest.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/headpain.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/idle2.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/idle22.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/ik_pose.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/initial.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/leftslash.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/pain1.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/pain_luparm.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/pain_ruparm.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/range_attack2.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/roar1.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/stand.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/turret_attack.md5anim");
		pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/walk7.md5anim");
		//m_pMD5Model->loadAnim("data/MD5Models/doom3/hellknight/walk7_left.md5anim");
		pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/hellknight/hellknight.material#box", 0);
		pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/hellknight/gob2.material#box", 1);
		pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/hellknight/gob.material#box", 2);
		pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/hellknight/tongue.material#box", 3);

		//Node* pMd5ModelNode = pMD5Model->loadModel("data/MD5Models/doom3/cacodemon/cacodemon.md5mesh");
		//pMD5Model->loadAnim("data/MD5Models/doom3/cacodemon/walk.md5anim");
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cacodemon/cacodemon.material", 0);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cacodemon/cacoeye.material", 1);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cacodemon/cacodemon.material", 2);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cacodemon/cacodemon.material", 3);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cacodemon/cacodemon.material", 4);

		//Node* pMd5ModelNode = pMD5Model->loadModel("data/MD5Models/doom3/cyberdemon/cyberdemon.md5mesh");
		//pMD5Model->loadAnim("data/MD5Models/doom3/cyberdemon/walk3.md5anim");
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cyberdemon/cyberdemon.material", 0);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cyberdemon/rocketlauncher.material", 1);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cyberdemon/rocketlauncher.material", 2);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cyberdemon/rocketlauncher.material", 3);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cyberdemon/rocketlauncher.material", 4);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/cyberdemon/rocketlauncher.material", 5);

		//Node* pMd5ModelNode = pMD5Model->loadModel("data/MD5Models/doom3/vagary/vagary.md5mesh");
		//pMD5Model->loadAnim("data/MD5Models/doom3/vagary/walk3.md5anim");
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/vagary/vagary.material", 0);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/vagary/teeth.material", 1);
		//pMd5ModelNode->getModel()->setMaterial("data/MD5Models/doom3/vagary/sac.material", 2);

		//md5ModelNode = m_pMD5Model->loadModel("data/MD5Models/Player_BasicMovement/body.md5mesh");
		//m_pMD5Model->loadAnim("data/MD5Models/Player_BasicMovement/step.md5anim");
		//md5ModelNode->getModel()->setMaterial("data/MD5Models/Player_BasicMovement/player.material", 0);

		//md5ModelNode = m_pMD5Model->loadModel("data/MD5Models/zfat/zfat.md5mesh");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/attack2.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/attack3.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/attack_leftslap.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/barrel_Idle.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/barrel_Throw.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/bellypain.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/death1_pose.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/facepain.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/idle1.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/initial.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/leftarmpain.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/pipeattack1.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/pipeidle.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/pipesight.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/pipewalk4.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/rightarmpain.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/sight3.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/walk1.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/walk2.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/walk3.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/zfat/walk4.md5anim");
		//md5ModelNode->getModel()->setMaterial("data/MD5Models/zfat/zfat.material", 0);
		//md5ModelNode->getModel()->setMaterial("data/MD5Models/zfat/monkeywrench.material", 1);
		//md5ModelNode->getModel()->setMaterial("data/MD5Models/zfat/skeleton01.material", 3);

		//md5ModelNode = m_pMD5Model->loadModel("data/MD5Models/lostsoul/lostsoul.md5mesh");
		//	m_pMD5Model->loadAnim("data/MD5Models/lostsoul/attack1.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/lostsoul/attack2.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/lostsoul/charge.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/lostsoul/initial.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/lostsoul/pain1.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/lostsoul/pain2.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/lostsoul/sight.md5anim");
		//	m_pMD5Model->loadAnim("data/MD5Models/lostsoul/walk1.md5anim");
		//	md5ModelNode->getModel()->setMaterial("data/MD5Models/zfat/zfat.material", 0);
		//	md5ModelNode->getModel()->setMaterial("data/MD5Models/zfat/zfat.material", 1);
		//	md5ModelNode->getModel()->setMaterial("data/MD5Models/zfat/zfat.material", 2);

		if(pMd5ModelNode != NULL) {
			pMd5ModelNode->scale(0.005f);
			pMd5ModelNode->setPosition(Vector3(i * 100.0f + 200.0f, 0.0f, i * -10.0f - 200.0f));
			pMd5ModelNode->rotateX(-90.0f);
			pScene->addNode(pMd5ModelNode);

			v_pMD5Models.push_back(pMD5Model);
		}
	}
}
#endif

void initLights() {
	glEnable(GL_LIGHTING);

	// set up light colors (ambient, diffuse, specular)
	GLfloat lightKa[] = {1.0f, 0.0f, 0.0f, 1.0f};      // ambient light
	GLfloat lightKd[] = {.9f, .9f, .9f, 1.0f};      // diffuse light
	GLfloat lightKs[] = {1, 1, 1, 1};               // specular light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

	// position the light in eye space
	float lightPos[4] = {0, 1, 1, 0};               // directional light
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);                            // MUST enable each light source after configuration
}

void drawGrid(int iSize, float fStep) {
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	for(float i = (float)-iSize; i < iSize; i += fStep) {

		//Vertical
		glVertex3f(-i, 0.0f, (float)iSize);
		glVertex3f(-i, 0.0f, (float)-iSize);

		//Horizontal
		glVertex3f((float)iSize, 0.0f, i);
		glVertex3f((float)-iSize, 0.0f, i);
	}
	glEnd();
}

void drawTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3) {
	glBegin(GL_TRIANGLES);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(p1.x, p1.y, p1.z);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(p2.x, p2.y, p2.z);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(p3.x, p3.y, p3.z);
	glEnd();
}

void drawLine(const Vector3& p1, const Vector3& p2) {
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glBegin(GL_LINES);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
	glEnd();

	glPointSize(5);
	glBegin(GL_POINTS);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
	glEnd();
}

Node* createHearthStoneCoolShaderModelNode(const char* pMaterial)
{
	// Calculate the vertices of the quad.
	Vector3 p1(-0.25f,	0.5f,	0.0f);
	Vector3 p2(-0.25f,	0.0f,	0.0f);
	Vector3 p3(0.25f,	0.0f,	0.0f);
	Vector3 p4(0.25f,	0.5f,	0.0f);

	// Create 3 vertices. Each vertex has position (x, y, z), color (red, green, blue) & textureCoords(s, t)
	float vertices[] =
	{
		p1.x, p1.y, p1.z,		/*0.0f, 1.0f, 0.0f,*/		0.0f, 0.0f,
		p2.x, p2.y, p2.z,		/*1.0f, 0.0f, 0.0f,*/		0.0f, 1.0f,
		p3.x, p3.y, p3.z,		/*0.0f, 0.0f, 1.0f,*/		1.0f, 1.0f,

		p4.x, p4.y, p4.z,		/*0.0f, 0.0f, 1.0f,*/		1.0f, 0.0f,
		p1.x, p1.y, p1.z,		/*0.0f, 1.0f, 0.0f,*/		0.0f, 0.0f,
		p3.x, p3.y, p3.z,		/*0.0f, 0.0f, 1.0f,*/		1.0f, 1.0f
	};
	unsigned int vertexCount = 6;
	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		//VertexFormat::Element(VertexFormat::COLOR, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::TEXCOORD0, VertexFormat::TWO),
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, elementCount), vertexCount, false);
	if (mesh == NULL)
	{
		//GP_ERROR("Failed to create mesh.");
		printf("Failed to create mesh.");
		return NULL;
	}
	mesh->setPrimitiveType(Mesh::TRIANGLES);
	mesh->setVertexData(vertices, 0, vertexCount);

	Node* pNode = Node::create("QuadModel");
	{
		Model* pModel = Model::create(mesh);
		pNode->setModel(pModel);
		pModel->setMaterial(pMaterial);
	}

	return pNode;
}

Node* createTriangleModelNode()
{
	// Calculate the vertices of the triangle.
	Vector3 p1(-0.5f,	0.5f,	-1.0f);
	Vector3 p2(-1.0f,	0,		-1.0f);
	Vector3 p3(-0.5f,	0,		-1.0f);

	// Create 3 vertices. Each vertex has position (x, y, z), color (red, green, blue) & textureCoords(s, t)
	float vertices[] =
	{
		p1.x, p1.y, -0.5f,    0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		
		p2.x, p2.y, 0.0f,     1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		
		p3.x, p3.y, 0.0f,     0.0f, 0.0f, 1.0f,		1.0f, 1.0f,		
	};
	unsigned int vertexCount = 3;
	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::COLOR, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::TEXCOORD0, VertexFormat::TWO),
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, elementCount), vertexCount, false);
	if (mesh == NULL)
	{
		//GP_ERROR("Failed to create mesh.");
		printf("Failed to create mesh.");
		return NULL;
	}
	mesh->setPrimitiveType(Mesh::TRIANGLES);
	mesh->setVertexData(vertices, 0, vertexCount);

	Node* pNode = Node::create("TriangleModel");
	{
		Model* pModel = Model::create(mesh);
		pNode->setModel(pModel);
		pModel->setMaterial("data/box.material#box");
	}

	return pNode;
}

Node* createTriangleStripModelNode(const char* pMaterial) {
	// Calculate the vertices of the equilateral triangle.
	Vector3 p1(0.0f,	0.5f,	-1.0f);
	Vector3 p2(0.0f,	0.0f,	-1.0f);
	Vector3 p3(0.5f,	0.5f,	-1.0f);
	
	Vector3 p4(0.5f,	0.0f,	-1.0f);
	Vector3 p5(1.0f,	0.5f,	-1.0f);
	Vector3 p6(1.0f,	0.0f,	-1.0f);

	// Create 3 vertices. Each vertex has position (x, y, z) and color (red, green, blue)
	//float vertices[] =
	//{
	//	0.0f,	0.5f, -0.5f,    0.0f, 0.0f, 1.0f, // 0
	//	0.0f,	0.0f, -0.5f,    0.0f, 1.0f, 0.0f, // 1
	//	0.5f,	0.5f,  0.0f,    1.0f, 0.0f, 0.0f, // 2

	//	0.0f,	0.0f, -0.5f,    0.0f, 1.0f, 0.0f, // 1
	//	0.5f,	0.5f,  0.0f,    1.0f, 0.0f, 0.0f, // 2
	//	0.5f,	0.0f, -0.5f,    0.0f, 0.0f, 0.5f, // 3

	//	0.5f,	0.5f,  0.0f,    1.0f, 0.0f, 0.0f, // 2
	//	0.5f,	0.0f, -0.5f,    0.0f, 0.0f, 0.5f, // 3
	//	1.0f,	0.5f, -0.5f,    0.0f, 0.5f, 0.0f, // 4

	//	0.5f,	0.0f, -0.5f,    0.0f, 0.0f, 0.5f, // 3
	//	1.0f,	0.5f, -0.5f,    0.0f, 0.5f, 0.0f, // 4
	//	1.0f,	0.0f, 0.0f,     0.5f, 0.0f, 0.0f, // 5
	//};
	float vertices[] =
	{
		0.0f,	0.5f, -0.5f,    0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		0.0f,	0.0f, -0.5f,    0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		0.5f,	0.5f,  0.0f,    1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
								
		0.5f,	0.0f, -0.5f,    0.0f, 0.0f, 0.5f,	0.0f, 0.0f,
								
		1.0f,	0.5f, -0.5f,    0.0f, 0.5f, 0.0f,	0.0f, 1.0f,
								
		1.0f,	0.0f, 0.0f,     0.5f, 0.0f, 0.0f,	1.0f, 1.0f,
	};
	unsigned int vertexCount = 6;
	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::COLOR, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::TEXCOORD0, VertexFormat::TWO),
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, elementCount), vertexCount, false);
	if (mesh == NULL)
	{
		//GP_ERROR("Failed to create mesh.");
		printf("Failed to create mesh.");
		return NULL;
	}
	mesh->setPrimitiveType(Mesh::TRIANGLE_STRIP);
	mesh->setVertexData(vertices, 0, vertexCount);

	Node* pNode = Node::create("TriangleStripModel");
	{
		Model* pModel = Model::create(mesh);
		pNode->setModel(pModel);
		pModel->setMaterial("data/box.material#box");
	}

	return pNode;
}

Node* createLineStripModelNode() {
	float vertices[] =
	{
		0.0f,	0.25f,	-1.0f,		1.0f, 0.0f, 0.0f,
		0.0f,	0.0f,	-1.0f,		0.0f, 1.0f, 0.0f,
		0.25f,	0.25f,	-1.25f,		0.0f, 0.0f, 1.0f,
		0.5f,	0.0f,	-1.25f,		1.0f, 0.0f, 1.0f,
		0.5f,	0.25f,	-1.0f,		1.0f, 1.0f, 1.0f,
	};
	unsigned int vertexCount = 5;
	
	VertexFormat::Element elements[] = 
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::COLOR, VertexFormat::THREE)
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);

	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, elementCount), vertexCount, false);
	GL_ASSERT( mesh );

	mesh->setPrimitiveType(Mesh::LINE_STRIP);
	mesh->setVertexData(vertices, 0, vertexCount);

	Node* pNode = Node::create("LineStripModel");
	{
		Model* pModel = Model::create(mesh);
		pNode->setModel(pModel);
		pModel->setMaterial("data/box.material#box");
	}

	return pNode;

}

Node* createLineModelNode() {

	float vertices[] = 
	{
		1.0f,	0.25f,	-1.0f,		1.0f, 0.0f, 0.0f,
		1.0f,	0.0f,	-1.0f,		0.0f, 1.0f, 0.0f,
		1.25f,	0.25f,	-1.25f,		0.0f, 0.0f, 1.0f,
		1.5f,	0.0f,	-1.25f,		1.0f, 0.0f, 1.0f,
		1.5f,	0.25f,	-1.0f,		1.0f, 1.0f, 1.0f,
		1.5f,	0.5f,	-1.25f,		1.0f, 0.0f, 1.0f,
	};
	unsigned int vertexCount = 6;

	VertexFormat::Element elements[] = 
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::COLOR, VertexFormat::THREE)
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);

	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, elementCount), vertexCount, false);
	GL_ASSERT( mesh );
	mesh->setPrimitiveType(Mesh::LINES);
	mesh->setVertexData(vertices, 0, vertexCount);

	Node* pNode = Node::create("LineModel");
	{
		Model* pModel = Model::create(mesh);
		pNode->setModel(pModel);
		pModel->setMaterial("data/box.material#box");		
	}

	return pNode;
}

Node* createGrid(unsigned int iSize, float fStep) {

	unsigned int iLineCount = (iSize / fStep);
	unsigned int iLineCountTwice = 2 * iLineCount;
	unsigned int iVertexCount = 2 * 2 * iLineCountTwice;

	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::COLOR, VertexFormat::THREE)
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);

	float fStride = fStep;
	float* pVertices = new float[iVertexCount * 6];
	float fVertex = (float)iSize * -1.0f;
	for (int i = 0; i < iLineCountTwice; i++) {

		///////// Vertical
		pVertices[i * 24 + 0] = fVertex;
		pVertices[i * 24 + 1] = 0.0f;
		pVertices[i * 24 + 2] = (float)iSize * -1.0f;

		pVertices[i * 24 + 3] = 1.0f;
		pVertices[i * 24 + 4] = 1.0f;
		pVertices[i * 24 + 5] = 1.0f;

		pVertices[i * 24 + 6] = fVertex;
		pVertices[i * 24 + 7] = 0.0f;
		pVertices[i * 24 + 8] = (float)iSize * 1.0f;

		pVertices[i * 24 + 9] = 1.0f;
		pVertices[i * 24 + 10] = 1.0f;
		pVertices[i * 24 + 11] = 1.0f;

		///////// Horizontal
		pVertices[i * 24 + 12] = (float)iSize * -1.0f;
		pVertices[i * 24 + 13] = 0.0f;
		pVertices[i * 24 + 14] = fVertex;

		pVertices[i * 24 + 15] = 1.0f;
		pVertices[i * 24 + 16] = 1.0f;
		pVertices[i * 24 + 17] = 1.0f;

		pVertices[i * 24 + 18] = (float)iSize * 1.0f;
		pVertices[i * 24 + 19] = 0.0f;
		pVertices[i * 24 + 20] = fVertex;

		pVertices[i * 24 + 21] = 1.0f;
		pVertices[i * 24 + 22] = 1.0f;
		pVertices[i * 24 + 23] = 1.0f;

		fVertex += fStride;
	}
	
	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, elementCount), iVertexCount, false);
	GL_ASSERT(mesh);
	mesh->setPrimitiveType(Mesh::LINES);
	mesh->setVertexData(pVertices, 0, iVertexCount);

	Node* pNode = Node::create("GridModel");
	{
		Model* pModel = Model::create(mesh);
		pNode->setModel(pModel);
		pModel->setMaterial("data/box.material#box");
	}

	SAFE_DELETE_ARRAY( pVertices );

	return pNode;
}

Node* createCubeModelIndexedNode(float size = 1.0f) {
	float a1 = size * 0.5f;
	float a2 = size * 1.0f;
	float vertices[] =
	{
		//MeshPart 0
		-a1, -a1,  a1,    0.0,  0.0,  1.0,   1.0, 0.0, 0.0,	0.0, 0.0,
		 a1, -a1,  a1,    0.0,  0.0,  1.0,   0.0, 1.0, 0.0,	1.0, 0.0,
		-a1,  a1,  a1,    0.0,  0.0,  1.0,   0.0, 0.0, 1.0,	0.0, 1.0,
		 a1,  a1,  a1,    0.0,  0.0,  1.0,   1.0, 1.0, 0.0,	1.0, 1.0,

		-a1,  a1,  a1,    0.0,  1.0,  0.0,   0.0, 1.0, 1.0,	0.0, 0.0,
		 a1,  a1,  a1,    0.0,  1.0,  0.0,   1.0, 0.0, 1.0,	1.0, 0.0,
		-a1,  a1, -a1,    0.0,  1.0,  0.0,   0.0, 1.0, 0.0,	0.0, 1.0,
		 a1,  a1, -a1,    0.0,  1.0,  0.0,   1.0, 0.0, 0.0,	1.0, 1.0,

		-a1,  a1, -a1,    0.0,  0.0, -1.0,   0.0, 0.0, 1.0,	0.0, 0.0,
		 a1,  a1, -a1,    0.0,  0.0, -1.0,   0.0, 1.0, 1.0,	1.0, 0.0,
		-a1, -a1, -a1,    0.0,  0.0, -1.0,   1.0, 1.0, 0.0,	0.0, 1.0,
		 a1, -a1, -a1,    0.0,  0.0, -1.0,   0.0, 1.0, 1.0,	1.0, 1.0,

		-a1, -a1, -a1,    0.0, -1.0,  0.0,   1.0, 0.0, 1.0,	0.0, 0.0,
		 a1, -a1, -a1,    0.0, -1.0,  0.0,   1.0, 1.0, 0.0,	1.0, 0.0,
		-a1, -a1,  a1,    0.0, -1.0,  0.0,   0.0, 1.0, 0.0,	0.0, 1.0,
		 a1, -a1,  a1,    0.0, -1.0,  0.0,   1.0, 1.0, 0.0,	1.0, 1.0,

		 a1, -a1,  a1,    1.0,  0.0,  0.0,   1.0, 0.0, 1.0,	0.0, 0.0,
		 a1, -a1, -a1,    1.0,  0.0,  0.0,   1.0, 0.0, 1.0,	1.0, 0.0,
		 a1,  a1,  a1,    1.0,  0.0,  0.0,   0.0, 1.0, 1.0,	0.0, 1.0,
		 a1,  a1, -a1,    1.0,  0.0,  0.0,   1.0, 1.0, 1.0,	1.0, 1.0,

		-a1, -a1, -a1,   -1.0,  0.0,  0.0,   1.0, 1.0, 0.0,	0.0, 0.0,
		-a1, -a1,  a1,   -1.0,  0.0,  0.0,   1.0, 1.0, 0.0,	1.0, 0.0,
		-a1,  a1, -a1,   -1.0,  0.0,  0.0,   0.0, 1.0, 1.0,	0.0, 1.0,
		-a1,  a1,  a1,   -1.0,  0.0,  0.0,   1.0, 1.0, 0.0,	1.0, 1.0,
											 							
		//MeshPart 1						 							
		-a2, -a2,  a2,		0.0,  0.0,  1.0,   1.0, 0.0, 0.0,	0.0, 0.0,
		a2, -a2,  a2,		0.0,  0.0,  1.0,   0.0, 1.0, 0.0,	1.0, 0.0,
		-a2,  a2,  a2,		0.0,  0.0,  1.0,   0.0, 0.0, 1.0,	0.0, 1.0,
		a2,  a2,  a2,		0.0,  0.0,  1.0,   1.0, 1.0, 0.0,	1.0, 1.0,
		-a2,  a2,  a2,		0.0,  1.0,  0.0,   0.0, 1.0, 1.0,	0.0, 0.0,
		a2,  a2,  a2,		0.0,  1.0,  0.0,   1.0, 0.0, 1.0,	1.0, 0.0,
		-a2,  a2, -a2,		0.0,  1.0,  0.0,   0.0, 1.0, 0.0,	0.0, 1.0,
		a2,  a2, -a2,		0.0,  1.0,  0.0,   1.0, 0.0, 0.0,	1.0, 1.0,
		-a2,  a2, -a2,		0.0,  0.0, -1.0,   0.0, 0.0, 1.0,	0.0, 0.0,
		a2,  a2, -a2,		0.0,  0.0, -1.0,   0.0, 1.0, 1.0,	1.0, 0.0,
		-a2, -a2, -a2,		0.0,  0.0, -1.0,   1.0, 1.0, 0.0,	0.0, 1.0,
		a2, -a2, -a2,		0.0,  0.0, -1.0,   0.0, 1.0, 1.0,	1.0, 1.0,
		-a2, -a2, -a2,		0.0, -1.0,  0.0,   1.0, 0.0, 1.0,	0.0, 0.0,
		a2, -a2, -a2,		0.0, -1.0,  0.0,   1.0, 1.0, 0.0,	1.0, 0.0,
		-a2, -a2,  a2,		0.0, -1.0,  0.0,   0.0, 1.0, 0.0,	0.0, 1.0,
		a2, -a2,  a2,		0.0, -1.0,  0.0,   1.0, 1.0, 0.0,	1.0, 1.0,
		a2, -a2,  a2,		1.0,  0.0,  0.0,   1.0, 0.0, 1.0,	0.0, 0.0,
		a2, -a2, -a2,		1.0,  0.0,  0.0,   1.0, 0.0, 1.0,	1.0, 0.0,
		a2,  a2,  a2,		1.0,  0.0,  0.0,   0.0, 1.0, 1.0,	0.0, 1.0,
		a2,  a2, -a2,		1.0,  0.0,  0.0,   1.0, 1.0, 1.0,	1.0, 1.0,
		-a2, -a2, -a2,		-1.0,  0.0,  0.0,   1.0, 1.0, 0.0,	0.0, 0.0,
		-a2, -a2,  a2,		-1.0,  0.0,  0.0,   1.0, 1.0, 0.0,	1.0, 0.0,
		-a2,  a2, -a2,		-1.0,  0.0,  0.0,   0.0, 1.0, 1.0,	0.0, 1.0,
		-a2,  a2,  a2,		-1.0,  0.0,  0.0,   1.0, 1.0, 0.0,	1.0, 1.0 
	};
	
	short indicesPart0[] = 
	{
		//MeshPart 0
		0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 18, 17, 19, 20, 21, 22, 22, 21, 23
	};

	short indicesPart1[] = 
	{
		//MeshPart 1
		24+0, 24+1, 24+2, 24+2, 24+1, 24+3, 24+4, 24+5, 24+6, 24+6, 24+5, 24+7, 24+8, 24+9, 24+10, 24+10, 24+9, 24+11, 24+12, 24+13, 24+14, 24+14, 24+13, 24+15, 24+16, 24+17, 24+18, 24+18, 24+17, 24+19, 24+20, 24+21, 24+22, 24+22, 24+21, 24+23
	};
	
	unsigned int vertexCount = 48;
	unsigned int indexCount = sizeof(indicesPart0) / sizeof(short);
	VertexFormat::Element vertexElements[] = 
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::NORMAL, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::COLOR, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::TEXCOORD0, VertexFormat::TWO)
	};
	unsigned int vertexElementCount = sizeof(vertexElements) / sizeof(VertexFormat::Element);

	Mesh* mesh = Mesh::createMesh(VertexFormat(vertexElements, vertexElementCount), vertexCount, false);
	if(mesh == NULL) {
		//GP_ERROR("Unable to create Mesh");
		return NULL;
	}
	mesh->setPrimitiveType(Mesh::TRIANGLES);
	mesh->setVertexData(vertices, 0, vertexCount);

	MeshPart* meshPart = NULL; 
	meshPart = mesh->addMeshPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, false);
	if( meshPart != NULL)
		meshPart->setIndexData(indicesPart0, 0, indexCount);

	//meshPart = mesh->addMeshPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, false);
	//if( meshPart != NULL)
	//	meshPart->setIndexData(indicesPart1, 0, indexCount);

	Node* pNode = Node::create("CubeIndexedModel");
	{
		Model* pModel = Model::create(mesh);
		pNode->setModel(pModel);
		pModel->setMaterial("data/box.material#box", 0);
		//pModel->setMaterial("data/box.material#box1", 1);
	}

	return pNode;
}

Node* createQuadIndexedNode() {
	float vertices[] =
	{
		-0.5f, 0.5f, 0.0f,		0.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.0f,		0.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 1.0f,		1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 0.0f,		1.0f, 1.0f, 1.0f
	};

	short indices[] = 
	{
		0, 1, 3, 3, 1, 2
	};

	unsigned int vertexCount = 4;
	unsigned int indexCount = sizeof(indices) / sizeof(short);
	VertexFormat::Element elements[] = 
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::TEXCOORD0, VertexFormat::TWO),
		VertexFormat::Element(VertexFormat::COLOR, VertexFormat::THREE)
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);

	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, elementCount), vertexCount, false);
	if(mesh == NULL) {
		//GP_ERROR("Unable to create Mesh");
		return NULL;
	}
	mesh->setPrimitiveType(Mesh::TRIANGLES);
	mesh->setVertexData(vertices, 0, vertexCount);

	MeshPart* meshPart = mesh->addMeshPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, false);
	meshPart->setIndexData(indices, 0, indexCount);

	Node* pNode = Node::create("QuadIndexedModel");
	{
		Model* pModel = Model::create(mesh);
		pNode->setModel(pModel);
		pModel->setMaterial("data/box.material#box");
	}

	return pNode;
}

Node* createObjNode() {

	Node* pNode = NULL;
	MeshObjLoader* objLoader = new MeshObjLoader();
	if(objLoader->loadObject("data/OBJModels/foot.obj")) {

		pNode = Node::create("ObjModel");
		{
			Model* objModel = objLoader->createModel();
			pNode->setModel(objModel);
			objModel->setMaterial("data/box.material#box1");
		}
	}

	return pNode;
}

MeshBatch* createMeshBatch() {
	VertexFormat::Element elements[] = 
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::COLOR, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::TEXCOORD0, VertexFormat::TWO)
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
	VertexFormat* vf = new VertexFormat(elements, elementCount);
	MeshBatch* mb = MeshBatch::create(*vf, Mesh::TRIANGLES, "data/box.material#box", true);
	
	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0, 0.5f, -0.5f), randomColor(), 0.0f, 0.0f));
	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(-0.5f, -0.5f, -0.5f), randomColor(), 0.0f, 1.0f));
	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0.5f, -0.5f, -0.5f), randomColor(), 1.0f, 1.0f));

	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0, 0.5f, -1.5f), randomColor(), 0.0f, 0.0f));
	//_meshBatchVertices.push_back(MeshBatchVertex(Vector3(-0.5f, -0.5f, -0.5f), randomColor(), 0.0f, 1.0f));
	//_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0.5f, -0.5f, -0.5f), randomColor(), 1.0f, 1.0f));

	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0, 0.5f, 1.5f), randomColor(), 0.0f, 0.0f));
	//_meshBatchVertices.push_back(MeshBatchVertex(Vector3(-0.5f, -0.5f, -0.5f), randomColor(), 0.0f, 1.0f));
	//_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0.5f, -0.5f, -0.5f), randomColor(), 1.0f, 1.0f));

	unsigned int triangleCount = 3;
	unsigned int indexCount = triangleCount * 3;
	unsigned short indices[] = {0, 1, 2, 3, 1, 2, 4, 1, 2};
	_indices = new unsigned short[indexCount];
	memcpy(_indices, indices, sizeof(unsigned short) * indexCount);

	_indicesCount = indexCount;
	
	mb->setTexture(CARTOON_TGA);

	return mb;
}

SpriteBatch* createSpriteBatch() {
	SpriteBatch* pSpriteBatch = SpriteBatch::create(CARTOON_TGA);
	GP_ASSERT( pSpriteBatch );

	return pSpriteBatch;
}

static float rAngle = 0.0f;
const float ROTATION_PER_SECOND = 2;//360deg/sec = 360/1000;;
bool bIncreasing = true;

void Dream3DTest::moveLight()
{
	Node* pNode = m_pPointLight[0]->getNode();
	if (pNode != NULL)
	{
		Vector3 vPos = pNode->getPosition();
		{
			int iSign = 1;
			if (bIncreasing)
			{
				iSign = 1;
				bIncreasing = !(vPos.x > 3.0);
			}
			else {
				iSign = -1;
				bIncreasing = (vPos.x < -3.0);
			}

			pNode->translateLeft(iSign * 0.01f);
		}
	}
}

void Dream3DTest::render3D(float deltaTimeMs) {
	////////////////////////////////////////////////////////////////
	moveLight();
	////////////////////////////////////////////////////////////////


	Model* pModel = NULL;
	{
		double t = getTimer()->getElapsedTimeInSec();

		pModel = magniQuadModelNode->getModel();
		{
			pModel->getMaterial()->getParameter("u_time")->setValue((float)t);
		}

		pModel = medivhQuadModelNode->getModel();
		{
			pModel->getMaterial()->getParameter("u_time")->setValue((float)t);
		}
		
		pModel = testShaderQuadModelNode->getModel();
		{
			pModel->getMaterial()->getParameter("u_time")->setValue((float)t);
		}
	}

	m_pScene->render();

	/*
	glLoadMatrixf(pCamera->getNode()->getViewMatrix().getTranspose());
	// Draw all of the triangles as one mesh batch.
	meshBatch->start();
	meshBatch->add(&_meshBatchVertices[0], (unsigned int)_meshBatchVertices.size(), _indices, _indicesCount);
	meshBatch->stop();
	meshBatch->render();
	//*/
	////////////////////////////////////////////////////////////////
}

void Dream3DTest::render2D(float deltaTimeMs) {
	Camera* pCamera = m_pScene->getActiveCamera();
	pCamera->setType(Camera::ORTHOGRAPHIC);

	SpriteBatch * pSpriteBatch = gFBOSpriteBatch;//gSpriteBatch;//gFBOSpriteBatch;

	pSpriteBatch->setClip(0, 0, getWidth(), getHeight());
	pSpriteBatch->start();
		pSpriteBatch->setClip(0, 0, 512, 512);
		Vector4 src(0, 0, pSpriteBatch->getTexture()->getWidth(), pSpriteBatch->getTexture()->getHeight());
		//pSpriteBatch->draw(Rectangle(0, 0, 200.0f, 200.0f), Rectangle(0, 0, 900.0f, 695.0f), Vector4::fromColor(0xFFFFFF80));
		pSpriteBatch->draw(Vector4(0, 0, 200.0f, 200.0f), src, Vector4::fromColor(0xFFFFFF80));
		pSpriteBatch->setClip(0, 0, getWidth(), getHeight());

		pSpriteBatch->draw(Vector4(200, 0, 64, 64), src, Vector4::fromColor(0xF68B28FF));
		pSpriteBatch->draw(Vector4(264, 0, 64, 64), src, Vector4::fromColor(0xDA2128FF));
		pSpriteBatch->draw(Vector4(328, 0, 64, 64), src, Vector4::fromColor(0xE21B52FF));

		pSpriteBatch->draw(Vector3(200, 64, 0), src, Vector2(100, 100), Vector4::fromColor(0xFFFFFFFF));

		/////// Rotation
		pSpriteBatch->draw(Vector3(300, 64, 0), src, Vector2(100, 100), Vector4(1, 1, 1, 1), Vector2(0.5f, 0.5f), MATH_DEG_TO_RAD(rAngle));
		pSpriteBatch->draw(Vector3(400, 64, 0), src, Vector2(128, 128), Vector4(1, 1, 1, 1), Vector2(0.5f, 0.5f), MATH_DEG_TO_RAD(135));
	pSpriteBatch->stop();

	rAngle += deltaTimeMs*ROTATION_PER_SECOND;
	if (rAngle >= 360.0f) rAngle = 0.0f;
}

void Dream3DTest::update(float deltaTimeMs) {
	if(isKeyPressed(VK_ESCAPE))
		exit();

#ifndef USE_YAGUI
	m_pScene->getActiveCamera()->update(deltaTimeMs);
#ifdef TEST_MD5_MODELS
	for (int i = 0; i < 1; i++) {
		MD5Model* pMD5Model = (MD5Model*)v_pMD5Models[i];
		pMD5Model->update(deltaTimeMs);
	}
#endif
	
	// Log Camera pos
	Camera* pCamera = m_pScene->getActiveCamera();
	if (pCamera != nullptr)
	{
		m_pLogger->log(pCamera->getNode()->getPosition());
	}
#endif
}


void Dream3DTest::render(float deltaTimeMs) {
#ifdef USE_YAGUI
	gFrameBuffer->bind();
		glClearColor(0.1f, 0.1f, 0.1f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
		m_pScene->render();
	gFrameBuffer->bindDefault();

	//render2D(deltaTimeMs);
#else
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

	render3D(deltaTimeMs);
#endif
}

void Dream3DTest::keyPressedEx(unsigned int iVirtualKeycode, unsigned short ch) {

}

void Dream3DTest::keyReleasedEx(unsigned int iVirtualKeycode, unsigned short ch) {

}

void Dream3DTest::onMouseDownEx(int mCode, int x, int y) {

	if( (mCode & MK_LBUTTON) != 0 ) {
		
		//Camera* pCamera = m_pScene->getActiveCamera();
		//pCamera->pickRay(Rectangle_(0, 0, getWidth(), getHeight()), x, y);
	}
}

void Dream3DTest::onMouseMoveEx(int mCode, int x, int y) {

}

void Dream3DTest::onMouseUpEx(int mCode, int x, int y){

}

void Dream3DTest::onMouseWheelEx(WPARAM wParam, LPARAM lParam) {

}

#ifdef USE_YAGUI
void addDummyWindows(H_WND hParent) {
	unsigned int iYPos = 0;
	unsigned int iXPos = 20;

	//NEW_OBJECT("WStatic");
	WComponentFactory* factory = WComponentFactory::Get();
	H_WND hWnd = NULL;

	hWnd = 
	CreateComponentEx(	"WStatic", 
								"Static Text... !!!", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								40, 
								189, 
								25,
								hParent, 
								HMENU(1234), 
								(LPVOID)255);

	hWnd = 
	CreateComponentEx(	"WButton", 
								"Simple Button...", 
								WM_ALIGN_WRT_PARENT_TOPLEFT | WM_ANCHOR_TOPLEFT, 
								iXPos,
								90, 
								189, 
								25,
								hParent, 
								HMENU(121), 
								"Button");

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WSprite", 
									"Simple Sprite...", 
									WM_ALIGN_WRT_PARENT_TOPLEFT | WM_ANCHOR_TOPLEFT, 
									iXPos + 150,
									iYPos, 
									100, 
									100,
									hParent, 
									NULL, 
									"Exclamation");
	{
		hWnd = 
		CreateComponentEx(	"WSprite", 
										"Simple Sprite inside Sprite...", 
										WM_ALIGN_WRT_PARENT_TOPLEFT | WM_ANCHOR_TOPLEFT, 
										10,
										10, 
										50, 
										50,
										hWnd, 
										NULL, 
										"Exclamation");
	}

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WCheckbox", 
								"CheckBox text", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								100, 
								25,
								hParent, 
								HMENU(1102), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WCanvas", 
								"WCanvas window", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								100, 
								100,
								hParent, 
								HMENU(1103), 
								NULL);
		{
			H_WND hBtn = 
			CreateComponentEx(	"WButton", 
										"Simple Button", 
										WM_ANCHOR_TOPLEFT, 
										10,
										10, 
										50, 
										25,
										hWnd, 
										HMENU(1291), 
										"Button");
		}
	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WTextField", 
								"_______I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								200, 
								23,
								hParent, 
								HMENU(1099), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WTextField", 
								"sprintf(m_pText, \"%s%c%s\", leftHalfSubstr, iKey, rightHalfSubstr); g->SetClip(RectF(m_pParent->getLeft(), m_pParent->getTop(), m_pParent->getWidth(), m_pParent->getHeight()));", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								220, 
								23,
								hParent, 
								HMENU(98), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WTextField", 
								"**sprintf(m_pText, \"%s%c%s\", leftHalfSubstr, iKey, rightHalfSubstr); g->SetClip(RectF(m_pParent->getLeft(), m_pParent->getTop(), m_pParent->getWidth(), m_pParent->getHeight()));", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								220, 
								23,
								hParent, 
								HMENU(98), 
								NULL);
/*
	WScrollbar* sb = new WScrollbar();
	sb->create(hParent, 20, 150, 250, 15, 0, 120);
	addComponent(sb);

	WScrollbar* sb1 = new WScrollbar();
	sb1->create(hParent, 30, 170, 15, 180, 1, 1211);
	addComponent(sb1);
*/
	CCString sText = "New Delhi, July 26 (IANS) Hours after saying he did not wish to implicate Prime Minister Manmohan Singh or anyone else in the 2G spectrum allotment case, former telecom minister A. Raja Tuesday asked why the matter had not been referred to a ministerial panel and also wanted Home Minister P. Chidambaram to take the witness stand.\n\
Main kisi ko phasana nahi chahta tha (I had no intention of framing anybody),' Raja's lawyer Sushil Kumar said on his behalf when the names of Manmohan Singh and Chidambaram cropped up in a special Central Bureau of Investigation (CBI) court.\n\
I am just defending myself -- not accusing anything or anybody,' he said, a day after stroking a political storm by dragging the prime minister into the controversy. 'They (the media) cannot put words into my mouth. Ask them to report truthfully, or go out of this court,' he added.\n\
But the home minister must come in the court from either of the sides and be a witness in the case. When all decisions were known to the home minister, he should appear as a witness in the case,' Kumar told the special court presided over by Judge O.P. Saini.\n\
Just a few hours later, after recess, he stepped up his attack on the prime minister and wondered why a group of ministers (GoM) was not set up if any inconsistency was found on the way the spectrum allocation matter was handled.\n\
A lawyer by traininRaja himself took over from his counsel at one point.\n\
The prime minister is superior to me. He could have constituted a GoM. But he ignored a GoM. Is this a conspiracy?' Raja's counsel asked, wanting the then solicitor general and now attorney general Goolam. E. Vahanvati, too, in the witness box, while terming the judicial custody of his client since Feb 2 illegal.\n\
The counsel was continuing with the arguments the previous day that as finance minister in 2008 Chidambaram had taken the decision to permit the promoters of two telecom firms to sell stakes with full knowledge of the prime minister.\n\
While this was not denied subsequently by Chidambaram or present Communications Minister Kapil Sibal, both sought to say that the equity sale was by way of issuing fresh shares and not divestment by promoters, permitted under the policy that existed then.\n\
The Congress even launched a counter-attack Tuesday and said Raja had also dragged former prime minister Atal Bihari Vajpayee's name in the case and that the government of the Bharatiya Janata Party (BJP)-led coalition at that time was equally culpable.\n\
If the BJP decides to make a song and dance about one part of Raja's statement, then the other part of his statement squarely indicts Atal Bihari Vajpayee also,' Congress spokesperson Manish Tewari said.\n\
The official probe agency has said that Raja's decision as telecom minister in 2008 to issue radio spectrum to companies at a mere Rs.1,659 crore ($350 million) for a pan-India operation had caused the exchequer losses worth thousands of crores of rupees.\n\
Nine new telecom companies were issued the radio frequency airwaves, a scarce national resource, to operate second generation (2G) mobile phone services in the country. As many as 122 circle-wise licences were issued.\n\
The probe agency questioned the manner in which allocations were made that even resulted in a windfall for some.\n\
A new player Swan Telecom had bought licences for 13 circles with the necessary spectrum for $340 million but managed to sell a 45 percent stake in the company to UAE's Etisalat for $900 million. This swelled its valuation to $2 billion without a single subscriber.\n\
Similarly, another new player, Unitech, paid $365 million as licence fee but sold a 60 percent stake to Norway's Telenor for $1.36 billion, taking its valuation to nearly $2 billion, again without a single subscriber.\n\
\n\
The MBR can only represent four partitions. A technique called \"extended\" partitioning is used to allow more than four, and often times it is used when there are more than two partitions. All we're going to say about extended partitions is that they appear in this table just like a normal partition, and their first sector has another partition table that describes the partitions within its space. But for the sake of simply getting some code to work, we're going to not worry about extended partitions (and repartition and reformat any drive that has them....) The most common scenario is only one partition using the whole drive, with partitions 2, 3 and 4 blank.";	

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WConsoleLog", 
								sText.c_str(), 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								397, 
								163,
								hParent, 
								HMENU(198), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WTextBox", 
								sText.c_str(), 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								319, 
								165,
								hParent, 
								HMENU(197), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WTextBox", 
								sText.c_str(), 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								600, 
								163,
								hParent, 
								HMENU(197), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WTabbedPane", 
								"",
								WM_ANCHOR_TOPLEFT,
								iXPos,
								iYPos, 
								400, 
								360,
								hParent, 
								HMENU(1966), 
								NULL);
		((WTabbedPane*)hWnd)->setSelectedTab(1);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WListBox", 
								"", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								400, 
								140,
								hParent, 
								HMENU(1001), 
								NULL);
	{
		char* ss = new char[255];
		memset(ss, 0, 255);

		LISTBOX_ITEM* item;
		for(int i = 0; i < 20; i++) {
			sprintf(ss, "item %d", i);

			item = new LISTBOX_ITEM();
			item->itemLabel = ss;

			((WListBox*)hWnd)->addItem(item);
		}
		sprintf(ss, "itemmmmmmmmmmmmmmm ieee");
		item = new LISTBOX_ITEM();
		item->itemLabel = ss;
		((WListBox*)hWnd)->addItem(item);
		sprintf(ss, "The FAT filesystems are designed to handle bad sectors aalekh.");
		item = new LISTBOX_ITEM();
		item->itemLabel = ss;
		((WListBox*)hWnd)->addItem(item);
		((WListBox*)hWnd)->removeItemAt(10);
		((WListBox*)hWnd)->setSelectedIndex(5);

		((WListBox*)hWnd)->setComponentAsChild(true);

		delete[] ss;
	}

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WComboBox", 
								"", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								250, 
								100,
								hParent, 
								HMENU(IDC_CB_FONT), 
								NULL);
	{
		((WComboBox*)hWnd)->addItem("Rosemary_DroidSans");
		((WComboBox*)hWnd)->addItem("Crisp");
		((WComboBox*)hWnd)->addItem("Consolas");
		((WComboBox*)hWnd)->addItem("Walkway_Black");
		((WComboBox*)hWnd)->addItem("Rosemary Roman");
		((WComboBox*)hWnd)->addItem("Kingdom_Hearts_Font");
		((WComboBox*)hWnd)->addItem("Inspyratta");
		((WComboBox*)hWnd)->addItem("DroidSansMono");
		((WComboBox*)hWnd)->addItem("diagoth");
		((WComboBox*)hWnd)->addItem("DejaVuSans");
		((WComboBox*)hWnd)->addItem("Comic Sans MS");
		((WComboBox*)hWnd)->addItem("ROSEMARY_DROIDSANS-BOLD");
		((WComboBox*)hWnd)->setSelectedIndex(0);
	}

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WTextBox", 
								sText.c_str(), 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								800, 
								500,
								hParent, 
								HMENU(1111), 
								NULL);
	((WTextBox*)hWnd)->showLineNumbers(true);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WTree", 
								"Title2", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								450, 
								450,
								hParent, 
								HMENU(1001), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponentEx(	"WTable", 
								"Table", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								450, 
								450,
								hParent, 
								HMENU(1002), 
								NULL);
		((WTable*)hWnd)->addColumn("Name");
		((WTable*)hWnd)->addColumn("Date Modified");
		((WTable*)hWnd)->addColumn("Type");
		((WTable*)hWnd)->addColumn("Size");
		((WTable*)hWnd)->addColumn("Date Created");
		((WTable*)hWnd)->addColumn("Authors");
		((WTable*)hWnd)->addColumn("Tags");

		TableRowData* trd = NULL;
		TableCellData* tcd = NULL;
		for(int ii = 0; ii < 15; ii++) {
			trd = new TableRowData();
			{
				tcd = new TableCellData("1.txt");	trd->addCellData(tcd);
				tcd = new TableCellData("10 June 2012");	trd->addCellData(tcd);
				tcd = new TableCellData("Type 0");	trd->addCellData(tcd);
				tcd = new TableCellData("12345");	trd->addCellData(tcd);
				tcd = new TableCellData("10 June 2012");	trd->addCellData(tcd);
				tcd = new TableCellData("Aalekh Maldikar");	trd->addCellData(tcd);
				tcd = new TableCellData("RW");	trd->addCellData(tcd);
			}
			((WTable*)hWnd)->addRow(trd);
		}

		for(int ii = 0; ii < 15; ii++) {
			trd = new TableRowData();
			{
				tcd = new TableCellData("2.txt");	trd->addCellData(tcd);
				tcd = new TableCellData("12 June 2012");	trd->addCellData(tcd);
				tcd = new TableCellData("Type 1");	trd->addCellData(tcd);
				tcd = new TableCellData("54321");	trd->addCellData(tcd);
				tcd = new TableCellData("12 June 2012");	trd->addCellData(tcd);
				tcd = new TableCellData("Rashmi Maldikar");	trd->addCellData(tcd);
				tcd = new TableCellData("RW");	trd->addCellData(tcd);
			}
			((WTable*)hWnd)->addRow(trd);
		}

	iXPos = 500;
	iYPos = 40;
	/////////////////////////////////////////////////
	H_WND hWnd0 = 
	CreateComponentEx(	"WWindow", 
								"Title", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								500, 
								650,
								hParent, 
								HMENU(1212), 
								(LPVOID)ID_TYPE_WND_C);
	((WWindow*)hWnd0)->setVisible(true);
	{
			iXPos = 20;
			iYPos = 40;
			hWnd = 
			CreateComponentEx(	"WButton", 
										"Simple Button !!!", 
										WM_ANCHOR_TOPLEFT, 
										iXPos,
										iYPos, 
										125, 
										25,
										hWnd0, 
										HMENU(1101), 
										"Button");
				((WButton*)hWnd)->setComponentAsChild(true);
				H_WND hButton = hWnd;
			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
			hWnd = 
			CreateComponentEx(	"WCheckbox", 
										"CheckBox text111", 
										WM_ANCHOR_TOPLEFT, 
										iXPos,
										iYPos, 
										100, 
										25,
										hWnd0, 
										HMENU(1190), 
										NULL);
			((WComponent*)hWnd)->setAlignmentParent((WComponent*)hButton);
			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
				hWnd = 
				CreateComponentEx(	"WTextField", 
											"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
											WM_ALIGN_WRT_PARENT_CENTERRIGHT | WM_ANCHOR_TOPCENTER,
											iXPos,
											iYPos, 
											200, 
											23,
											hWnd0, 
											HMENU(99), 
											NULL);
				((WTextField*)hWnd)->setComponentAsChild(true);

			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
				hWnd = 
				CreateComponentEx(	"WTextBox", 
											sText.c_str(), 
											WM_ANCHOR_TOPLEFT, 
											iXPos,
											iYPos, 
											250, 
											163,
											hWnd0, 
											HMENU(99), 
											NULL);
				((WTextBox*)hWnd)->showLineNumbers(true);

			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
				hWnd = 
				CreateComponentEx(	"WTextBox", 
											sText.c_str(), 
											WM_ANCHOR_TOPLEFT, 
											iXPos,
											iYPos, 
											260, 
											165,
											hWnd0, 
											HMENU(199), 
											NULL);

			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
				hWnd = 
				CreateComponentEx(	"WComboBox", 
											"", 
											WM_ANCHOR_TOPLEFT, 
											iXPos,
											iYPos, 
											250, 
											100,
											hWnd0, 
											HMENU(299), 
											NULL);
				((WComboBox*)hWnd)->addDefaultTestItems();

			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
			{
				hWnd = 
				CreateComponentEx(	"WListBox", 
											"", 
											WM_ANCHOR_TOPLEFT, 
											iXPos,
											iYPos, 
											400, 
											140,
											hWnd0, 
											HMENU(1001), 
											(LPVOID)!true);
				char* ss1 = new char[255];
				memset(ss1, 0, 255);

				LISTBOX_ITEM* item;
				for(int i = 0; i < 20; i++) {
					sprintf(ss1, "item %d", i);

					item = new LISTBOX_ITEM();
					item->itemLabel = ss1;

					((WListBox*)hWnd)->addItem(item);
				}
				sprintf(ss1, "itemmmmmmmmmmmmmmm ieee");
				item = new LISTBOX_ITEM();
				item->itemLabel = ss1;
				((WListBox*)hWnd)->addItem(item);
				sprintf(ss1, "The FAT filesystems are designed to handle bad sectors aalekh.");
				item = new LISTBOX_ITEM();
				item->itemLabel = ss1;
				((WListBox*)hWnd)->addItem(item);
				((WListBox*)hWnd)->removeItemAt(10);
				((WListBox*)hWnd)->setSelectedIndex(5);

				((WListBox*)hWnd)->setComponentAsChild(true);

				delete[] ss1;
			}

		H_WND hWndW0 = 
		CreateComponentEx(	"WWindow", 
									"Title1", 
									WM_ANCHOR_TOPLEFT, 
									300,
									40, 
									300, 
									550,
									hWnd0, 
									HMENU(1001), 
									(LPVOID)ID_TYPE_WND_CMX);
			{

				iXPos = 20;
				iYPos = 40;
					hWnd = 
					CreateComponentEx(	"WTextField", 
												"00 I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
												WM_ANCHOR_TOPLEFT, 
												iXPos,
												iYPos, 
												220, 
												23,
												hWndW0, 
												HMENU(99), 
												NULL);
					((WTextField*)hWnd)->setComponentAsChild(true);

				iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
					hWnd = 
					CreateComponentEx(	"WComboBox", 
												"", 
												WM_ANCHOR_TOPLEFT, 
												iXPos,
												iYPos, 
												200, 
												100,
												hWndW0, 
												HMENU(299), 
												NULL);
					((WComboBox*)hWnd)->addDefaultTestItems();

				iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
				{
					H_WND hWind1 = 
					CreateComponentEx(	"WWindow", 
												"Title1", 
												WM_ANCHOR_TOPLEFT, 
												iXPos,
												iYPos, 
												250, 
												400,
												hWndW0, 
												HMENU(1001), 
												(LPVOID)ID_TYPE_WND_CMX);

					iXPos = 20;
					iYPos = 40;
					hWnd = 
					CreateComponentEx(	"WTextBox", 
												sText.c_str(), 
												WM_ANCHOR_TOPLEFT, 
												iXPos,
												iYPos, 
												200, 
												163,
												hWind1, 
												HMENU(199), 
												NULL);
						((WTextBox*)hWnd)->showLineNumbers(true);

					iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
					hWnd = 
					CreateComponentEx(	"WWindow", 
												"Title1", 
												WM_ANCHOR_TOPLEFT, 
												iXPos,
												iYPos, 
												200, 
												100,
												hWind1, 
												HMENU(1001), 
												(LPVOID)ID_TYPE_WND_C);
				}
			}
	}

	iXPos = ((WComponent*)hWnd0)->getOffsetX() + ((WComponent*)hWnd0)->getWidth() + 10;
	iYPos = 40;
	/////////////////////////////////////////////////
	H_WND wind0 = 
	CreateComponentEx(	"WWindow", 
								"Title", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								500, 
								650,
								hParent, 
								HMENU(1011), 
								(LPVOID)ID_TYPE_WND_C);
	((WWindow*)wind0)->setVisible(true);
	{
		/////////////////////////////////////////////////
		iXPos = 20;
		iYPos = 40;
		hWnd = 
		CreateComponentEx(	"WButton", 
									"SSSSimple Button", 
									WM_ANCHOR_TOPLEFT, 
									iXPos,
									iYPos, 
									125, 
									25,
									wind0, 
									HMENU(111), 
									"Button");
		((WButton*)hWnd)->setComponentAsChild(true);

		iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
		hWnd = 
		CreateComponentEx(	"WCheckbox", 
									"CheckBox text", 
									WM_ANCHOR_TOPLEFT, 
									iXPos,
									iYPos, 
									100, 
									25,
									wind0, 
									HMENU(111), 
									NULL);

		iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
		hWnd = 
		CreateComponentEx(	"WTextField", 
										"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
										WM_ANCHOR_TOPLEFT, 
										iXPos,
										iYPos, 
										200, 
										23,
										wind0, 
										HMENU(99), 
										NULL);
		((WTextField*)hWnd)->setComponentAsChild(true);

		iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
		WTextBox* tbW1 = new WTextBox();
			hWnd = 
			CreateComponentEx(	"WTextBox", 
												sText.c_str(), 
												WM_ANCHOR_TOPLEFT, 
												iXPos,
												iYPos, 
												250, 
												163,
												wind0, 
												HMENU(199), 
												NULL);
		((WTextBox*)hWnd)->showLineNumbers(true);

		iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
			hWnd = 
			CreateComponentEx(	"WTextBox", 
										sText.c_str(), 
										WM_ANCHOR_TOPLEFT, 
										iXPos,
										iYPos, 
										260, 
										165,
										wind0, 
										HMENU(199), 
										NULL);

		iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
			hWnd = 
			CreateComponentEx(	"WComboBox", 
																"", 
																WM_ANCHOR_TOPLEFT, 
																iXPos,
																iYPos, 
																250, 
																100,
																wind0, 
																HMENU(299), 
																NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

		iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
		{
			hWnd = 
			CreateComponentEx(	"WListBox", 
										"", 
										WM_ANCHOR_TOPLEFT, 
										iXPos,
										iYPos, 
										400, 
										140,
										wind0, 
										HMENU(1001), 
										(LPVOID)!true);
			char* ss1 = new char[255];
			memset(ss1, 0, 255);

			LISTBOX_ITEM* item;
			for(int i = 0; i < 20; i++) {
				sprintf(ss1, "item %d", i);

				item = new LISTBOX_ITEM();
				item->itemLabel = ss1;

				((WListBox*)hWnd)->addItem(item);
			}
			sprintf(ss1, "itemmmmmmmmmmmmmmm ieee");
			item = new LISTBOX_ITEM();
			item->itemLabel = ss1;
			((WListBox*)hWnd)->addItem(item);
			sprintf(ss1, "The FAT filesystems are designed to handle bad sectors aalekh.");
			item = new LISTBOX_ITEM();
			item->itemLabel = ss1;
			((WListBox*)hWnd)->addItem(item);
			((WListBox*)hWnd)->removeItemAt(10);
			((WListBox*)hWnd)->setSelectedIndex(5);

			((WListBox*)hWnd)->setComponentAsChild(true);

			delete[] ss1;
		}

		H_WND windW0 = 
			CreateComponentEx(	"WWindow", 
										"Title1", 
										WM_ANCHOR_TOPLEFT, 
										300,
										40, 
										300, 
										550,
										wind0, 
										HMENU(1001), 
										(LPVOID)ID_TYPE_WND_CMX);

			iXPos = 20;
			iYPos = 40;
			hWnd = 
			CreateComponentEx(	"WTextField", 
												"00 I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
												WM_ANCHOR_TOPLEFT, 
												iXPos,
												iYPos, 
												220, 
												23,
												windW0, 
												HMENU(99), 
												NULL);
			((WTextField*)hWnd)->setComponentAsChild(true);

			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
			hWnd = 
			CreateComponentEx(	"WComboBox", 
																"", 
																WM_ANCHOR_TOPLEFT, 
																iXPos,
																iYPos, 
																200, 
																100,
																windW0, 
																HMENU(299), 
																NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
			H_WND hwind1 = 
			CreateComponentEx(	"WWindow", 
										"Title1", 
										WM_ANCHOR_TOPLEFT, 
										iXPos,
										iYPos, 
										250, 
										400,
										windW0, 
										HMENU(1001), 
										(LPVOID)ID_TYPE_WND_CMX);
			
			hWnd = 
			CreateComponentEx(	"WWindow", 
										"Title1", 
										WM_ANCHOR_TOPLEFT, 
										iXPos,
										iYPos, 
										250, 
										400,
										windW0, 
										HMENU(1001), 
										(LPVOID)ID_TYPE_WND_CMX);

				iXPos = 20;
				iYPos = 40;
				hWnd = 
				CreateComponentEx(	"WTextBox", 
												sText.c_str(), 
												WM_ANCHOR_TOPLEFT, 
												iXPos,
												iYPos, 
												200, 
												163,
												hwind1, 
												HMENU(199), 
												NULL);
				((WTextBox*)hWnd)->showLineNumbers(true);

				iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
				hWnd = 
				CreateComponentEx(	"WWindow", 
												"Title1", 
												WM_ANCHOR_TOPLEFT, 
												iXPos,
												iYPos, 
												200, 
												100,
												hwind1, 
												HMENU(1001), 
												(LPVOID)ID_TYPE_WND_C);
		/////////////////////////////////////////////////
	/////////////////////////////////////////////////
	}

	iXPos = ((WComponent*)wind0)->getOffsetX() + ((WComponent*)wind0)->getWidth() + 10;
	iYPos = 40;
	H_WND hwind1 = 
	CreateComponentEx(	"WWindow", 
									"Title1", 
									WM_ANCHOR_TOPLEFT, 
									iXPos,
									iYPos, 
									200, 
									550,
									hParent, 
									HMENU(1001), 
									(LPVOID)ID_TYPE_WND_CX);
	{
		iXPos = 20;
		iYPos = 40;
		hWnd = 
		CreateComponentEx(	"WTextField", 
												"100 I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
												WM_ANCHOR_TOPLEFT, 
												iXPos,
												iYPos, 
												150, 
												23,
												hwind1, 
												HMENU(99), 
												NULL);
			((WTextField*)hWnd)->setComponentAsChild(true);

		iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
		hWnd = 
		CreateComponentEx(	"WComboBox", 
																"", 
																WM_ANCHOR_TOPLEFT, 
																iXPos,
																iYPos, 
																250, 
																100,
																hwind1, 
																HMENU(299), 
																NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();
	}

	iXPos = ((WComponent*)wind0)->getOffsetX();
	iYPos = ((WComponent*)wind0)->getOffsetY() + ((WComponent*)wind0)->getHeight() + 5;
	H_WND wind2 = 
	CreateComponentEx(	"WFrame", 
											"Title2", 
											WM_ANCHOR_TOPLEFT, 
											iXPos,
											iYPos, 
											550, 
											650,
											hParent, 
											HMENU(1113), 
											(LPVOID)true);
		((WFrame*)wind2)->setBorderVisibility(true);
		{
			hWnd = 
			CreateComponentEx(	"WButton", 
										"Simple Button", 
										WM_ANCHOR_TOPLEFT, 
										20,
										40, 
										125, 
										25,
										wind2, 
										HMENU(111), 
										"Button");
			((WButton*)hWnd)->setComponentAsChild(true);

			hWnd = 
			CreateComponentEx(	"WCheckbox", 
														"CheckBox text", 
														WM_ANCHOR_TOPLEFT, 
														20,
														70, 
														100, 
														25,
														wind2, 
														HMENU(111), 
														NULL);

			hWnd = 
			CreateComponentEx(	"WTextField", 
										"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
										WM_ANCHOR_TOPLEFT, 
										20,
										100, 
										200, 
										23,
										wind2, 
										HMENU(99), 
										NULL);
			((WTextField*)hWnd)->setComponentAsChild(true);

			hWnd = 
			CreateComponentEx(	"WTextBox", 
												sText.c_str(), 
												WM_ANCHOR_TOPLEFT, 
												20,
												130, 
												250, 
												163,
												wind2, 
												HMENU(199), 
												NULL);
			((WTextBox*)hWnd)->showLineNumbers(true);

			hWnd = 
			CreateComponentEx(	"WTextBox", 
											sText.c_str(), 
											WM_ANCHOR_TOPLEFT, 
											20,
											300, 
											260, 
											165,
											wind2, 
											HMENU(199), 
											NULL);

			hWnd = 
			CreateComponentEx(	"WComboBox", 
											"", 
											WM_ANCHOR_TOPLEFT, 
											20,
											470, 
											250, 
											100,
											wind2, 
											HMENU(299), 
											NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

			hWnd = 
			CreateComponentEx(	"WListBox", 
												"", 
												WM_ANCHOR_TOPLEFT, 
												20,
												560, 
												400, 
												140,
												wind2, 
												HMENU(1001), 
												(LPVOID)!true);
			{
				char* ss1 = new char[255];
				memset(ss1, 0, 255);

				LISTBOX_ITEM* item;
				for(int i = 0; i < 20; i++) {
					sprintf(ss1, "item %d", i);

					item = new LISTBOX_ITEM();
					item->itemLabel = ss1;

					((WListBox*)hWnd)->addItem(item);
				}
				sprintf(ss1, "itemmmmmmmmmmmmmmm ieee");
				item = new LISTBOX_ITEM();
				item->itemLabel = ss1;
				((WListBox*)hWnd)->addItem(item);
				sprintf(ss1, "The FAT filesystems are designed to handle bad sectors aalekh.");
				item = new LISTBOX_ITEM();
				item->itemLabel = ss1;
				((WListBox*)hWnd)->addItem(item);
				((WListBox*)hWnd)->removeItemAt(10);
				((WListBox*)hWnd)->setSelectedIndex(5);

				((WListBox*)hWnd)->setComponentAsChild(true);

				delete[] ss1;
			}

			hWnd = 
			CreateComponentEx(	"WComboBox", 
												"", 
												WM_ANCHOR_TOPLEFT, 
												20,
												720, 
												250, 
												100,
												wind2, 
												HMENU(299), 
												NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

			H_WND wind1 = 
			CreateComponentEx(	"WWindow", 
										"Title1", 
										WM_ANCHOR_TOPLEFT, 
										300,
										40, 
										200, 
										550,
										wind2, 
										HMENU(1001), 
										(LPVOID)ID_TYPE_WND_CX);
			{
				hWnd = 
				CreateComponentEx(	"WTextField", 
											"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
											WM_ANCHOR_TOPLEFT, 
											20,
											40, 
											150, 
											23,
											wind1, 
											HMENU(99), 
											NULL);
				((WTextField*)hWnd)->setComponentAsChild(true);

				hWnd = 
				CreateComponentEx(	"WComboBox", 
											"", 
											WM_ANCHOR_TOPLEFT, 
											20,
											70, 
											250, 
											100,
											wind1, 
											HMENU(299), 
											NULL);
				((WComboBox*)hWnd)->addDefaultTestItems();
			}

			hWnd = 
			CreateComponentEx(	"WTree", 
										"Title2", 
										WM_ANCHOR_TOPLEFT, 
										50,
										140, 
										450, 
										450,
										wind2, 
										HMENU(1001), 
										NULL);

			hWnd = 
			CreateComponentEx(	"WTable", 
										"Table", 
										WM_ANCHOR_TOPLEFT, 
										50,
										140, 
										450, 
										450,
										wind2, 
										HMENU(1003), 
										NULL);
			{
					((WTable*)hWnd)->addColumn("Name");
					((WTable*)hWnd)->addColumn("Date Modified");
					((WTable*)hWnd)->addColumn("Type");
					((WTable*)hWnd)->addColumn("Size");
					((WTable*)hWnd)->addColumn("Date Created");
					((WTable*)hWnd)->addColumn("Authors");
					((WTable*)hWnd)->addColumn("Tags");

					TableRowData* trd = new TableRowData();
					TableCellData* tcd = NULL;
					{
						tcd = new TableCellData("1.txt");	trd->addCellData(tcd);
						tcd = new TableCellData("10 June 2012");	trd->addCellData(tcd);
						tcd = new TableCellData("Type 0");	trd->addCellData(tcd);
						tcd = new TableCellData("12345");	trd->addCellData(tcd);
						tcd = new TableCellData("10 June 2012");	trd->addCellData(tcd);
						tcd = new TableCellData("Aalekh Maldikar");	trd->addCellData(tcd);
						tcd = new TableCellData("RW");	trd->addCellData(tcd);
					}
					((WTable*)hWnd)->addRow(trd);
					
					trd = new TableRowData();
					{
						tcd = new TableCellData("2.txt");	trd->addCellData(tcd);
						tcd = new TableCellData("12 June 2012");	trd->addCellData(tcd);
						tcd = new TableCellData("Type 1");	trd->addCellData(tcd);
						tcd = new TableCellData("54321");	trd->addCellData(tcd);
						tcd = new TableCellData("12 June 2012");	trd->addCellData(tcd);
						tcd = new TableCellData("Rashmi Maldikar");	trd->addCellData(tcd);
						tcd = new TableCellData("RW");	trd->addCellData(tcd);
					}
					((WTable*)hWnd)->addRow(trd);
			}

			hWnd = 
			CreateComponentEx(	"WInspector", 
										"Title1", 
										WM_ANCHOR_TOPLEFT, 
										120,
										40, 
										350, 
										250,
										wind2, 
										HMENU(1013), 
										(LPVOID)true);
			{
				((WInspector*)hWnd)->addTab();
				((WInspector*)hWnd)->addTab();
				((WInspector*)hWnd)->addTab();
			}
		}

	iXPos = ((WComponent*)wind2)->getOffsetX();
	iYPos = ((WComponent*)wind2)->getOffsetY() + ((WComponent*)wind2)->getHeight() + 10;
	wind2 = 
	CreateComponentEx(	"WFrame", 
								"Title2", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								550, 
								650,
								hParent, 
								HMENU(1112), 
								(LPVOID)true);
	{
		((WFrame*)wind2)->setBorderVisibility(true);
		{
			hWnd = 
			CreateComponentEx(	"WButton", 
										"Simple Button", 
										WM_ANCHOR_TOPLEFT, 
										20,
										40, 
										125, 
										25,
										wind2, 
										HMENU(111), 
										"Button");
			((WButton*)hWnd)->setComponentAsChild(true);

			hWnd = 
			CreateComponentEx(	"WCheckbox", 
										"CheckBox text", 
										WM_ANCHOR_TOPLEFT, 
										20,
										70, 
										100, 
										25,
										wind2, 
										HMENU(111), 
										NULL);

			hWnd = 
			CreateComponentEx(	"WTextField", 
										"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
										WM_ANCHOR_TOPLEFT, 
										20,
										100, 
										200, 
										23,
										wind2, 
										HMENU(99), 
										NULL);
			((WTextField*)hWnd)->setComponentAsChild(true);

			hWnd = 
			CreateComponentEx(	"WTextBox", 
											sText.c_str(), 
											WM_ANCHOR_TOPLEFT, 
											20,
											130, 
											260, 
											163,
											wind2, 
											HMENU(199), 
											NULL);
		((WTextBox*)hWnd)->showLineNumbers(true);

			hWnd = 
			CreateComponentEx(	"WTextBox", 
												sText.c_str(), 
												WM_ANCHOR_TOPLEFT, 
												20,
												300, 
												260, 
												165,
												wind2, 
												HMENU(199), 
												NULL);

			hWnd = 
			CreateComponentEx(	"WComboBox", 
															"", 
															WM_ANCHOR_TOPLEFT, 
															20,
															470, 
															250, 
															100,
															wind2, 
															HMENU(299), 
															NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

			hWnd = 
			CreateComponentEx(	"WListBox", 
													"", 
													WM_ANCHOR_TOPLEFT, 
													20,
													560, 
													400, 
													140,
													wind2, 
													HMENU(1001), 
													(LPVOID)!true);
			{
				char* ss1 = new char[255];
				memset(ss1, 0, 255);

				LISTBOX_ITEM* item;
				for(int i = 0; i < 20; i++) {
					sprintf(ss1, "item %d", i);

					item = new LISTBOX_ITEM();
					item->itemLabel = ss1;

					((WListBox*)hWnd)->addItem(item);
				}
				sprintf(ss1, "itemmmmmmmmmmmmmmm ieee");
				item = new LISTBOX_ITEM();
				item->itemLabel = ss1;
				((WListBox*)hWnd)->addItem(item);
				sprintf(ss1, "The FAT filesystems are designed to handle bad sectors aalekh.");
				item = new LISTBOX_ITEM();
				item->itemLabel = ss1;
				((WListBox*)hWnd)->addItem(item);
				((WListBox*)hWnd)->removeItemAt(10);
				((WListBox*)hWnd)->setSelectedIndex(5);

				((WListBox*)hWnd)->setComponentAsChild(true);

				delete[] ss1;
			}

			hWnd = 
			CreateComponentEx(	"WComboBox", 
																"", 
																WM_ANCHOR_TOPLEFT, 
																20,
																720, 
																250, 
																100,
																wind2, 
																HMENU(299), 
																NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

			H_WND wind1 = 
			CreateComponentEx(	"WWindow", 
												"Title1", 
												WM_ANCHOR_TOPLEFT, 
												300,
												40, 
												200, 
												550,
												wind2, 
												HMENU(1001), 
												(LPVOID)ID_TYPE_WND_CX);
			{
				hWnd = 
				CreateComponentEx(	"WTextField", 
																"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
																WM_ANCHOR_TOPLEFT, 
																20,
																40, 
																150, 
																23,
																wind1, 
																HMENU(99), 
																NULL);
				((WTextField*)hWnd)->setComponentAsChild(true);

				hWnd = 
				CreateComponentEx(	"WComboBox", 
																"", 
																WM_ANCHOR_TOPLEFT, 
																20,
																70, 
																250, 
																100,
																wind1, 
																HMENU(299), 
																NULL);
				((WComboBox*)hWnd)->addDefaultTestItems();
			}

				hWnd = 
				CreateComponentEx(	"WTree", 
											"Title2", 
											WM_ANCHOR_TOPLEFT, 
											50,
											140, 
											450, 
											450,
											wind2, 
											HMENU(1001), 
											NULL);

				hWnd = 
				CreateComponentEx(	"WTable", 
											"Table", 
											WM_ANCHOR_TOPLEFT, 
											50,
											140, 
											450, 
											450,
											wind2, 
											HMENU(1004), 
											NULL);
			{
					((WTable*)hWnd)->addColumn("Name");
					((WTable*)hWnd)->addColumn("Date Modified");
					((WTable*)hWnd)->addColumn("Type");
					((WTable*)hWnd)->addColumn("Size");
					((WTable*)hWnd)->addColumn("Date Created");
					((WTable*)hWnd)->addColumn("Authors");
					((WTable*)hWnd)->addColumn("Tags");

					TableRowData* trd = NULL;
					TableCellData* tcd = NULL;
					for(int ii = 0; ii < 15; ii++) {
						trd = new TableRowData();
						{
							tcd = new TableCellData("1.txt");	trd->addCellData(tcd);
							tcd = new TableCellData("10 June 2012");	trd->addCellData(tcd);
							tcd = new TableCellData("Type 0");	trd->addCellData(tcd);
							tcd = new TableCellData("12345");	trd->addCellData(tcd);
							tcd = new TableCellData("10 June 2012");	trd->addCellData(tcd);
							tcd = new TableCellData("Aalekh Maldikar");	trd->addCellData(tcd);
							tcd = new TableCellData("RW");	trd->addCellData(tcd);
						}
						((WTable*)hWnd)->addRow(trd);
					}
					
					for(int ii = 0; ii < 15; ii++) {
						trd = new TableRowData();
						{
							tcd = new TableCellData("2.txt");	trd->addCellData(tcd);
							tcd = new TableCellData("12 June 2012");	trd->addCellData(tcd);
							tcd = new TableCellData("Type 1");	trd->addCellData(tcd);
							tcd = new TableCellData("54321");	trd->addCellData(tcd);
							tcd = new TableCellData("12 June 2012");	trd->addCellData(tcd);
							tcd = new TableCellData("Rashmi Maldikar");	trd->addCellData(tcd);
							tcd = new TableCellData("RW");	trd->addCellData(tcd);
						}
						((WTable*)hWnd)->addRow(trd);
					}
			}

			hWnd = 
			CreateComponentEx(	"WInspector", 
													"Title1", 
													WM_ANCHOR_TOPLEFT, 
													120,
													40, 
													350, 
													250,
													wind2, 
													HMENU(1013), 
													(LPVOID)true);
			{
				((WInspector*)hWnd)->addTab();
				((WInspector*)hWnd)->addTab();
				((WInspector*)hWnd)->addTab();
			}
		}
	}
}

void addDummyWindows_(H_WND hParent) {
	unsigned int iYPos = 0;
	unsigned int iXPos = 20;

	//NEW_OBJECT("WStatic");
	WComponentFactory* factory = WComponentFactory::Get();
	H_WND hWnd = NULL;

	hWnd = 
		CreateComponentEx(	"WGraph", 
									"WGraph... !!!", 
									WM_ANCHOR_TOPLEFT,
									iXPos,
									40, 
									250, 
									250,
									hParent, 
									HMENU(1234), 
									&GraphScale(1.0f, 1.0f));
			WGraph* pGraph = (WGraph*)hWnd;
			pGraph->setBorderVisibility(true);
			pGraph->addDot(0.3f, 0.5f);
			pGraph->addDot(0.6f, 0.9f);
			pGraph->addDot(1.1f, 1.2f);
	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 10;

	iXPos += ((WComponent*)hWnd)->getOffsetX() + ((WComponent*)hWnd)->getWidth() + 10;
	hWnd = 
	CreateComponentEx(	"WCanvas", 
								"WCanvas window", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								250, 
								250,
								hParent, 
								HMENU(1103), 
								NULL);


	H_WND hContainer = 
	CreateComponentEx(	"WContainer",
								"Simple Container", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								100, 
								100, 
								100,
								hParent, 
								HMENU(1243), 
								NULL);

	H_WND hButton = 
	CreateComponentEx(	"WButton",
								"Simple Button", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								20, 
								189, 
								25,
								hContainer,//hParent, 
								HMENU(121), 
								"Button");

	iYPos = ((WComponent*)hButton)->getOffsetY() + ((WComponent*)hButton)->getHeight() + 10;
	hWnd = 
	CreateComponentEx(	"WTextField", 
								"**sprintf(m_pText, \"%s%c%s\", leftHalfSubstr, iKey, rightHalfSubstr); g->SetClip(RectF(m_pParent->getLeft(), m_pParent->getTop(), m_pParent->getWidth(), m_pParent->getHeight()));", 
								WM_ANCHOR_TOPLEFT, 
								iXPos,
								iYPos, 
								220, 
								23,
								hContainer,//hParent, 
								NULL, 
								NULL);
	((WComponent*)hWnd)->setAlignmentParent((WComponent*)hButton);
	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 10;
	//hWnd = 
	//CreateComponentEx(	"WWindow", 
	//							"Title", 
	//							WM_ANCHOR_TOPCENTER, 
	//							iXPos,
	//							iYPos, 
	//							500, 
	//							650,
	//							hParent, 
	//							HMENU(1212), 
	//							(LPVOID)ID_TYPE_WND_C);
}
#endif