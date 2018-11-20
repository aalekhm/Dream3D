/*
███████████████ ▒		███████████████░   	  ███████████████		 ██████████
	█████   ██████ 		  ███████████████     ████████			    ████	████
	█████░   █████ 		░  █████  ░ █████     ██				   ████		█████
	░█████ ░  ██████	 ░ █████████████·░	  ████████			   ████	  ██  ████
	░█████   ▒██████	   ████████████░░     ██████			  ████	███	  ████
	░░▒█████   ███████	   █████   █████·░    ██				  ██████	   ████
	███████████████ 	░ ███████ ░██████     ███████			 ████		   ████
	███████████████ ▒ 	███████████ ███████   ████████████		████		    ████
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

#include "Engine/MaterialReader.h"
#include "Engine/Properties.h"

#include "Engine/UI/WComponentFactory.h"

#define CARTOON_TGA		"data/cartoon.tga"
#define COLOURFUL_TGA	"data/ColorFul_2048x1300.tga"
#define CORE_UI				"data/core.tga"

// Declare our game instance
Dream3DTest game;

void initLights();
void drawTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3);
void drawGrid(int iSize, float fStep);
void drawLine(const Vector3& p1, const Vector3& p2);

Node* createTriangleModelNode();
Node* triangleModelNode;

Node* createTriangleStripModelNode();
Node* triangleStripModelNode;

Node* createLineStripModelNode();
Node* lineStripModelNode;

Node* createLineModelNode();
Node* lineModelNode;

Node* createCubeModelIndexedNode(float size);
Node* cubeModelNode;

Node* createQuadIndexedNode();
Node* quadModelNode;

Node* createObjNode();
Node* objModelNode;

SpriteBatch* createSpriteBatch();
SpriteBatch* spriteBatch;

Texture*  gTexture0;
Texture*  gTexture1;

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
						pCamera->setType(Camera::PERSPECTIVE);
						{
							glClear(GL_DEPTH_BUFFER_BIT);
							glClearColor(0.1f, 0.1f, 0.1f, 1);
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
							glDisable(GL_SCISSOR_TEST);
							
							pCamera->getNode()->getViewMatrix().getTranspose();
							drawTriangle(Vector3(0, 0.5f, -1.5f), Vector3(-0.5f, 0, -1.5f), Vector3(0.5f, 0, -1.5f));
						}
						//////////////////////////////////////////////////

						//////////////////////////// 2D SCENE
						pCamera->setType(Camera::ORTHOGRAPHIC);
						{
							//FillRect(&boundsRect, 255.0f, 255.0f, 255.0f, 255.0f);
							DrawRect(&boundsRect, 0.0f, 0.0f, 0.0f, 255.0f);

							SetColorQ(255, 0, 0, 255);
							DrawString("Hi There !!!", boundsRect.X + 10, boundsRect.Y + 10, 0);
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
					case 1103:
					{
						Rect boundsRect;
						int iRet = SendMessageQ(hWnd, WM__GETRECT, (WPARAM)NULL, (LPARAM)&boundsRect);

						Camera* pCamera = gCanvasCameraNode->getCamera();
						pCamera->setCameraValues(boundsRect.X, boundsRect.Y, boundsRect.Width, boundsRect.Height, 45.0f, 0.01f, 10.0f);
					}
					break;
				}
			}
			break;
		case WN__SIZE:
			{
				int WINDOW_ID = wParam;
				switch(WINDOW_ID) {
				case 1103:
					{
						Rect boundsRect;
						int iRet = SendMessageQ(hWnd, WM__GETRECT, (WPARAM)NULL, (LPARAM)&boundsRect);

						Camera* pCamera = gCanvasCameraNode->getCamera();
						pCamera->setCameraValues(boundsRect.X, boundsRect.Y, boundsRect.Width, boundsRect.Height, 45.0f, 0.01f, 10.0f);
					}
					break;
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
									CreateComponent(	"WTextField", 
									"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
									0, 
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
									CreateComponent(	"WButton", 
									"Press Me !!!", 
									0, 
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

void addDummyWindows(H_WND hParent);

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

Dream3DTest::Dream3DTest() {

}

Dream3DTest::~Dream3DTest() {
	//SAFE_DELETE( m_pScene );
}

void Dream3DTest::initialize() {
	////////////////////// SCENE INITIALISATION
	m_pScene = Scene::create();

	Camera* pCamera = Camera::createPerspective(0, 0, getWidth(), getHeight(), 45.0f, 0.01f, 10.0f);
	Node* pCameraNode = Node::create("FirstPersonShooterCamera");
	pCameraNode->setCamera(pCamera);
	m_pScene->addNode(pCameraNode);
	//////////////////////////////////////////////

	//////////////////////////////////////////////
	//glEnable(GL_DEPTH_TEST);

	triangleModelNode = createTriangleModelNode();
	m_pScene->addNode(triangleModelNode);

	triangleStripModelNode = createTriangleStripModelNode();
	m_pScene->addNode(triangleStripModelNode);
	
	lineStripModelNode = createLineStripModelNode();
	m_pScene->addNode(lineStripModelNode);

	lineModelNode = createLineModelNode();
	m_pScene->addNode(lineModelNode);

	quadModelNode = createQuadIndexedNode();
	
	cubeModelNode = createCubeModelIndexedNode(0.25f);
	cubeModelNode->rotateY(0.0f);
	cubeModelNode->setPosition(Vector3(2.0f, 0.0f, -1.0f));
	m_pScene->addNode(cubeModelNode);
		quadModelNode->setPosition(Vector3(-1.0f, 0.0f, 0.0f));
		quadModelNode->rotateY(45.0f);
	cubeModelNode->addChild(quadModelNode);

	objModelNode = createObjNode();
	objModelNode->scale(0.1f);
	m_pScene->addNode(objModelNode);

	meshBatch = createMeshBatch();
	spriteBatch = createSpriteBatch();

	gTexture0 = Texture::create(COLOURFUL_TGA, false);
	gTexture1 = Texture::create(CORE_UI, false);

	MaterialReader* matReader = new MaterialReader();
	Properties* prop = matReader->read("data/test.mat");
	if(prop != NULL)
		prop->print(0);

	//initLights();
	//////////////////////////////////////////////

	//////////////////// YAGUI Related initializations !!!
	addUIListener(UICallback);
	addDummyWindows(GetWindowQ(0));
	////////////////////////////////////////////
}

void initLights() {
	glEnable(GL_LIGHTING);

	// set up light colors (ambient, diffuse, specular)
	GLfloat lightKa[] = {1.0f, 1.0f, 1.0f, 1.0f};      // ambient light
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

Node* createTriangleModelNode()
{
	// Calculate the vertices of the triangle.
	Vector3 p1(-0.5f,	0.5f,	-1.0f);
	Vector3 p2(-1.0f,	0,		-1.0f);
	Vector3 p3(-0.5f,	0,		-1.0f);

	// Create 3 vertices. Each vertex has position (x, y, z) and color (red, green, blue)
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

	Model* pModel = Model::create(mesh);
	pModel->setTexture(COLOURFUL_TGA);

	Node* pNode = Node::create("TriangleModel");
	pNode->setModel(pModel);

	return pNode;
}

Node* createTriangleStripModelNode() {
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

	Model* pModel = Model::create(mesh);
	pModel->setTexture(CARTOON_TGA);

	Node* pNode = Node::create("TriangleStripModel");
	pNode->setModel(pModel);

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

	Model* pModel = Model::create(mesh);

	Node* pNode = Node::create("LineStripModel");
	pNode->setModel(pModel);

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

	Model* pModel = Model::create(mesh);

	Node* pNode = Node::create("LineModel");
	pNode->setModel(pModel);

	return pNode;
}

Node* createCubeModelIndexedNode(float size = 1.0f) {
	float a = size * 0.5f;
	float vertices[] =
	{
		-a, -a,  a,   /* 0.0,  0.0,  1.0,*/   1.0, 0.0, 0.0,	0.0, 0.0,
		 a, -a,  a,   /* 0.0,  0.0,  1.0,*/   0.0, 1.0, 0.0,	1.0, 0.0,
		-a,  a,  a,   /* 0.0,  0.0,  1.0,*/   0.0, 0.0, 1.0,	0.0, 1.0,
		 a,  a,  a,   /* 0.0,  0.0,  1.0,*/   1.0, 1.0, 0.0,	1.0, 1.0,
		-a,  a,  a,   /* 0.0,  1.0,  0.0,*/   0.0, 1.0, 1.0,	0.0, 0.0,
		 a,  a,  a,   /* 0.0,  1.0,  0.0,*/   1.0, 0.0, 1.0,	1.0, 0.0,
		-a,  a, -a,   /* 0.0,  1.0,  0.0,*/   0.0, 1.0, 0.0,	0.0, 1.0,
		 a,  a, -a,   /* 0.0,  1.0,  0.0,*/   1.0, 0.0, 0.0,	1.0, 1.0,
		-a,  a, -a,   /* 0.0,  0.0, -1.0,*/   0.0, 0.0, 1.0,	0.0, 0.0,
		 a,  a, -a,   /* 0.0,  0.0, -1.0,*/   0.0, 1.0, 1.0,	1.0, 0.0,
		-a, -a, -a,   /* 0.0,  0.0, -1.0,*/   1.0, 1.0, 0.0,	0.0, 1.0,
		 a, -a, -a,   /* 0.0,  0.0, -1.0,*/   0.0, 1.0, 1.0,	1.0, 1.0,
		-a, -a, -a,   /* 0.0, -1.0,  0.0,*/   1.0, 0.0, 1.0,	0.0, 0.0,
		 a, -a, -a,   /* 0.0, -1.0,  0.0,*/   1.0, 1.0, 0.0,	1.0, 0.0,
		-a, -a,  a,   /* 0.0, -1.0,  0.0,*/   0.0, 1.0, 0.0,	0.0, 1.0,
		 a, -a,  a,   /* 0.0, -1.0,  0.0,*/   1.0, 1.0, 0.0,	1.0, 1.0,
		 a, -a,  a,   /* 1.0,  0.0,  0.0,*/   1.0, 0.0, 1.0,	0.0, 0.0,
		 a, -a, -a,   /* 1.0,  0.0,  0.0,*/   1.0, 0.0, 1.0,	1.0, 0.0,
		 a,  a,  a,   /* 1.0,  0.0,  0.0,*/   0.0, 1.0, 1.0,	0.0, 1.0,
		 a,  a, -a,   /* 1.0,  0.0,  0.0,*/   1.0, 1.0, 1.0,	1.0, 1.0,
		-a, -a, -a,   /*-1.0,  0.0,  0.0,*/   1.0, 1.0, 0.0,	0.0, 0.0,
		-a, -a,  a,   /*-1.0,  0.0,  0.0,*/   1.0, 1.0, 0.0,	1.0, 0.0,
		-a,  a, -a,   /*-1.0,  0.0,  0.0,*/   0.0, 1.0, 1.0,	0.0, 1.0,
		-a,  a,  a,   /*-1.0,  0.0,  0.0,*/   1.0, 1.0, 0.0,	1.0, 1.0 
	};
	
	short indices[] = 
	{
		0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 18, 17, 19, 20, 21, 22, 22, 21, 23
	};
	
	unsigned int vertexCount = 24;
	unsigned int indexCount = sizeof(indices) / sizeof(short);
	VertexFormat::Element elements[] = 
	{
		VertexFormat::Element(VertexFormat::POSITION, VertexFormat::THREE),
		//VertexFormat::Element(VertexFormat::NORMAL, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::COLOR, VertexFormat::THREE),
		VertexFormat::Element(VertexFormat::TEXCOORD0, VertexFormat::TWO)
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

	Model* pModel = Model::create(mesh);
	pModel->setTexture(CARTOON_TGA);

	Node* pNode = Node::create("CubeIndexedModel");
	pNode->setModel(pModel);

	return pNode;
}

Node* createQuadIndexedNode() {
	float vertices[] =
	{
		-0.5f, 0.5f, 0.0f,		0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,		0.0f, 1.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 0.0f
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
		VertexFormat::Element(VertexFormat::TEXCOORD0, VertexFormat::TWO)
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

	Model* pModel = Model::create(mesh);
	pModel->setTexture(CARTOON_TGA);

	Node* pNode = Node::create("QuadIndexedModel");
	pNode->setModel(pModel);

	return pNode;
}

Node* createObjNode() {

	MeshObjLoader* objLoader = new MeshObjLoader();
	objLoader->loadObject("data/foot.obj");

	Model* objModel = objLoader->createModel();
	objModel->setTexture(CARTOON_TGA);

	Node* pNode = Node::create("ObjModel");
	pNode->setModel(objModel);

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
	MeshBatch* mb = MeshBatch::create(*vf, Mesh::TRIANGLES, true);
	
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
	SpriteBatch* spriteBatch = SpriteBatch::create(CARTOON_TGA);
	GP_ASSERT( spriteBatch );

	return spriteBatch;
}

static float rAngle = 0.0f;
const float ROTATION_PER_SECOND = 0.25*0.36f;//360deg/sec = 360/1000;

void Dream3DTest::render3D(float deltaTimeMs) {
	Camera* pCamera = m_pScene->getActiveCamera();
	pCamera->setType(Camera::PERSPECTIVE);

	//No Transformations to the World(grid) & triangle hence load the 'view matrix'.
	glLoadMatrixf(pCamera->getNode()->getViewMatrix().getTranspose());
	drawGrid(10, 0.5);
	drawTriangle(Vector3(0, 0.5f, -1.5f), Vector3(-0.5f, 0, -1.5f), Vector3(0.5f, 0, -1.5f));

	glColor3f(1.0f, 1.0f, 1.0f);

	gTexture0->bind();
	glBegin(GL_QUADS);
		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(0.0f, 0.5f, 0.0f);

		glTexCoord2d(0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);

		glTexCoord2d(1.0f, 1.0f);
		glVertex3f(0.5f, 0.0f, 0.0f);

		glTexCoord2d(1.0f, 0.0f);
		glVertex3f(0.5f, 0.5f, 0.0f);
	glEnd();
	gTexture0->unbind();
	////////////////////////////////////////////////////////////////
	cubeModelNode->translateForward(0.0001f * deltaTimeMs);
	quadModelNode->rotateY(0.001f * rAngle);
	m_pScene->render();

	glLoadMatrixf(pCamera->getNode()->getViewMatrix().getTranspose());
	// Draw all of the triangles as one mesh batch.
	meshBatch->start();
	meshBatch->add(&_meshBatchVertices[0], (unsigned int)_meshBatchVertices.size(), _indices, _indicesCount);
	meshBatch->stop();
	meshBatch->render();
	////////////////////////////////////////////////////////////////
}

void Dream3DTest::render2D(float deltaTimeMs) {
	Camera* pCamera = m_pScene->getActiveCamera();
	pCamera->setType(Camera::ORTHOGRAPHIC);

	spriteBatch->setClip(0, 0, getWidth(), getHeight());
	spriteBatch->start();
		spriteBatch->setClip(0, 0, 100, 100);
		Vector4 src(0, 0, spriteBatch->getTexture()->getWidth(), spriteBatch->getTexture()->getHeight());
		//spriteBatch->draw(Rectangle(0, 0, 200.0f, 200.0f), Rectangle(0, 0, 900.0f, 695.0f), Vector4::fromColor(0xFFFFFF80));
		spriteBatch->draw(Vector4(0, 0, 200.0f, 200.0f), src, Vector4::fromColor(0xFFFFFF80));
		spriteBatch->setClip(0, 0, getWidth(), getHeight());

		spriteBatch->draw(Vector4(200, 0, 64, 64), src, Vector4::fromColor(0xF68B28FF));
		spriteBatch->draw(Vector4(264, 0, 64, 64), src, Vector4::fromColor(0xDA2128FF));
		spriteBatch->draw(Vector4(328, 0, 64, 64), src, Vector4::fromColor(0xE21B52FF));

		spriteBatch->draw(Vector3(200, 64, 0), src, Vector2(100, 100), Vector4::fromColor(0xFFFFFFFF));

		/////// Rotation
		spriteBatch->draw(Vector3(300, 64, 0), src, Vector2(100, 100), Vector4(1, 1, 1, 1), Vector2(0.5f, 0.5f), MATH_DEG_TO_RAD(rAngle));
		spriteBatch->draw(Vector3(400, 64, 0), src, Vector2(128, 128), Vector4(1, 1, 1, 1), Vector2(0.5f, 0.5f), MATH_DEG_TO_RAD(135));
	spriteBatch->stop();

	rAngle += deltaTimeMs*ROTATION_PER_SECOND;
	if(rAngle >= 360.0f) rAngle = 0.0f;
}

void Dream3DTest::update(float deltaTimeMs) {
	if(isKeyPressed(VK_ESCAPE))
		exit();

	//m_pScene->getActiveCamera()->update(deltaTimeMs);
}

void Dream3DTest::render(float deltaTimeMs) {
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

	render3D(deltaTimeMs);
	render2D(deltaTimeMs);
}

void addDummyWindows(H_WND hParent) {
	unsigned int iYPos = 0;
	unsigned int iXPos = 20;

	//NEW_OBJECT("WStatic");
	WComponentFactory* factory = WComponentFactory::Get();
	H_WND hWnd = NULL;

	hWnd = 
	CreateComponent(	"WStatic", 
								"Static Text... !!!", 
								0, 
								iXPos,
								40, 
								189, 
								25,
								hParent, 
								HMENU(1234), 
								(LPVOID)255);

	hWnd = 
	CreateComponent(	"WButton", 
								"Simple Button", 
								0, 
								iXPos,
								90, 
								189, 
								25,
								hParent, 
								HMENU(121), 
								"Button");

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponent(	"WCheckbox", 
								"CheckBox text", 
								0, 
								iXPos,
								iYPos, 
								100, 
								25,
								hParent, 
								HMENU(1102), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponent(	"WCanvas", 
								"WCanvas window", 
								0, 
								iXPos,
								iYPos, 
								100, 
								100,
								hParent, 
								HMENU(1103), 
								NULL);
	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponent(	"WTextField", 
								"_______I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
								0, 
								iXPos,
								iYPos, 
								200, 
								23,
								hParent, 
								HMENU(1099), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponent(	"WTextField", 
								"sprintf(m_pText, \"%s%c%s\", leftHalfSubstr, iKey, rightHalfSubstr); g->SetClip(RectF(m_pParent->getLeft(), m_pParent->getTop(), m_pParent->getWidth(), m_pParent->getHeight()));", 
								0, 
								iXPos,
								iYPos, 
								220, 
								23,
								hParent, 
								HMENU(98), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponent(	"WTextField", 
								"**sprintf(m_pText, \"%s%c%s\", leftHalfSubstr, iKey, rightHalfSubstr); g->SetClip(RectF(m_pParent->getLeft(), m_pParent->getTop(), m_pParent->getWidth(), m_pParent->getHeight()));", 
								0, 
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
	CreateComponent(	"WConsoleLog", 
								sText.c_str(), 
								0, 
								iXPos,
								iYPos, 
								397, 
								163,
								hParent, 
								HMENU(198), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponent(	"WTextBox", 
								sText.c_str(), 
								0, 
								iXPos,
								iYPos, 
								319, 
								165,
								hParent, 
								HMENU(197), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponent(	"WTextBox", 
								sText.c_str(), 
								0, 
								iXPos,
								iYPos, 
								600, 
								163,
								hParent, 
								HMENU(197), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponent(	"WTabbedPane", 
								"",
								0,
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
	CreateComponent(	"WListBox", 
								"", 
								0, 
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
	CreateComponent(	"WComboBox", 
								"", 
								0, 
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
	CreateComponent(	"WTextBox", 
								sText.c_str(), 
								0, 
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
	CreateComponent(	"WTree", 
								"Title2", 
								0, 
								iXPos,
								iYPos, 
								450, 
								450,
								hParent, 
								HMENU(1001), 
								NULL);

	iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
	hWnd = 
	CreateComponent(	"WTable", 
								"Table", 
								0, 
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
	CreateComponent(	"WWindow", 
								"Title", 
								0, 
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
			CreateComponent(	"WButton", 
										"Simple Button !!!", 
										0, 
										iXPos,
										iYPos, 
										125, 
										25,
										hWnd0, 
										HMENU(1101), 
										"Button");
				((WButton*)hWnd)->setComponentAsChild(true);

			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
			hWnd = 
			CreateComponent(	"WCheckbox", 
										"CheckBox text", 
										0, 
										iXPos,
										iYPos, 
										100, 
										25,
										hWnd0, 
										HMENU(111), 
										NULL);

			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
				hWnd = 
				CreateComponent(	"WTextField", 
											"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
											0, 
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
				CreateComponent(	"WTextBox", 
											sText.c_str(), 
											0, 
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
				CreateComponent(	"WTextBox", 
											sText.c_str(), 
											0, 
											iXPos,
											iYPos, 
											260, 
											165,
											hWnd0, 
											HMENU(199), 
											NULL);

			iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
				hWnd = 
				CreateComponent(	"WComboBox", 
											"", 
											0, 
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
				CreateComponent(	"WListBox", 
											"", 
											0, 
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
		CreateComponent(	"WWindow", 
									"Title1", 
									0, 
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
					CreateComponent(	"WTextField", 
												"00 I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
												0, 
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
					CreateComponent(	"WComboBox", 
												"", 
												0, 
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
					CreateComponent(	"WWindow", 
												"Title1", 
												0, 
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
					CreateComponent(	"WTextBox", 
												sText.c_str(), 
												0, 
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
					CreateComponent(	"WWindow", 
												"Title1", 
												0, 
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
	CreateComponent(	"WWindow", 
								"Title", 
								0, 
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
		CreateComponent(	"WButton", 
									"SSSSimple Button", 
									0, 
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
		CreateComponent(	"WCheckbox", 
									"CheckBox text", 
									0, 
									iXPos,
									iYPos, 
									100, 
									25,
									wind0, 
									HMENU(111), 
									NULL);

		iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
		hWnd = 
		CreateComponent(	"WTextField", 
										"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
										0, 
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
			CreateComponent(	"WTextBox", 
												sText.c_str(), 
												0, 
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
			CreateComponent(	"WTextBox", 
										sText.c_str(), 
										0, 
										iXPos,
										iYPos, 
										260, 
										165,
										wind0, 
										HMENU(199), 
										NULL);

		iYPos = ((WComponent*)hWnd)->getOffsetY() + ((WComponent*)hWnd)->getHeight() + 5;
			hWnd = 
			CreateComponent(	"WComboBox", 
																"", 
																0, 
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
			CreateComponent(	"WListBox", 
										"", 
										0, 
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
			CreateComponent(	"WWindow", 
										"Title1", 
										0, 
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
			CreateComponent(	"WTextField", 
												"00 I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
												0, 
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
			CreateComponent(	"WComboBox", 
																"", 
																0, 
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
			CreateComponent(	"WWindow", 
										"Title1", 
										0, 
										iXPos,
										iYPos, 
										250, 
										400,
										windW0, 
										HMENU(1001), 
										(LPVOID)ID_TYPE_WND_CMX);
			
			hWnd = 
			CreateComponent(	"WWindow", 
										"Title1", 
										0, 
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
				CreateComponent(	"WTextBox", 
												sText.c_str(), 
												0, 
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
				CreateComponent(	"WWindow", 
												"Title1", 
												0, 
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
	CreateComponent(	"WWindow", 
									"Title1", 
									0, 
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
		CreateComponent(	"WTextField", 
												"100 I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
												0, 
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
		CreateComponent(	"WComboBox", 
																"", 
																0, 
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
	CreateComponent(	"WFrame", 
											"Title2", 
											0, 
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
			CreateComponent(	"WButton", 
										"Simple Button", 
										0, 
										20,
										40, 
										125, 
										25,
										wind2, 
										HMENU(111), 
										"Button");
			((WButton*)hWnd)->setComponentAsChild(true);

			hWnd = 
			CreateComponent(	"WCheckbox", 
														"CheckBox text", 
														0, 
														20,
														70, 
														100, 
														25,
														wind2, 
														HMENU(111), 
														NULL);

			hWnd = 
			CreateComponent(	"WTextField", 
										"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
										0, 
										20,
										100, 
										200, 
										23,
										wind2, 
										HMENU(99), 
										NULL);
			((WTextField*)hWnd)->setComponentAsChild(true);

			hWnd = 
			CreateComponent(	"WTextBox", 
												sText.c_str(), 
												0, 
												20,
												130, 
												250, 
												163,
												wind2, 
												HMENU(199), 
												NULL);
			((WTextBox*)hWnd)->showLineNumbers(true);

			hWnd = 
			CreateComponent(	"WTextBox", 
											sText.c_str(), 
											0, 
											20,
											300, 
											260, 
											165,
											wind2, 
											HMENU(199), 
											NULL);

			hWnd = 
			CreateComponent(	"WComboBox", 
											"", 
											0, 
											20,
											470, 
											250, 
											100,
											wind2, 
											HMENU(299), 
											NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

			hWnd = 
			CreateComponent(	"WListBox", 
												"", 
												0, 
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
			CreateComponent(	"WComboBox", 
												"", 
												0, 
												20,
												720, 
												250, 
												100,
												wind2, 
												HMENU(299), 
												NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

			H_WND wind1 = 
			CreateComponent(	"WWindow", 
										"Title1", 
										0, 
										300,
										40, 
										200, 
										550,
										wind2, 
										HMENU(1001), 
										(LPVOID)ID_TYPE_WND_CX);
			{
				hWnd = 
				CreateComponent(	"WTextField", 
											"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
											0, 
											20,
											40, 
											150, 
											23,
											wind1, 
											HMENU(99), 
											NULL);
				((WTextField*)hWnd)->setComponentAsChild(true);

				hWnd = 
				CreateComponent(	"WComboBox", 
											"", 
											0, 
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
			CreateComponent(	"WTree", 
										"Title2", 
										0, 
										50,
										140, 
										450, 
										450,
										wind2, 
										HMENU(1001), 
										NULL);

			hWnd = 
			CreateComponent(	"WTable", 
										"Table", 
										0, 
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
			CreateComponent(	"WInspector", 
										"Title1", 
										0, 
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
	CreateComponent(	"WFrame", 
								"Title2", 
								0, 
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
			CreateComponent(	"WButton", 
										"Simple Button", 
										0, 
										20,
										40, 
										125, 
										25,
										wind2, 
										HMENU(111), 
										"Button");
			((WButton*)hWnd)->setComponentAsChild(true);

			hWnd = 
			CreateComponent(	"WCheckbox", 
										"CheckBox text", 
										0, 
										20,
										70, 
										100, 
										25,
										wind2, 
										HMENU(111), 
										NULL);

			hWnd = 
			CreateComponent(	"WTextField", 
										"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
										0, 
										20,
										100, 
										200, 
										23,
										wind2, 
										HMENU(99), 
										NULL);
			((WTextField*)hWnd)->setComponentAsChild(true);

			hWnd = 
			CreateComponent(	"WTextBox", 
											sText.c_str(), 
											0, 
											20,
											130, 
											260, 
											163,
											wind2, 
											HMENU(199), 
											NULL);
		((WTextBox*)hWnd)->showLineNumbers(true);

			hWnd = 
			CreateComponent(	"WTextBox", 
												sText.c_str(), 
												0, 
												20,
												300, 
												260, 
												165,
												wind2, 
												HMENU(199), 
												NULL);

			hWnd = 
			CreateComponent(	"WComboBox", 
															"", 
															0, 
															20,
															470, 
															250, 
															100,
															wind2, 
															HMENU(299), 
															NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

			hWnd = 
			CreateComponent(	"WListBox", 
													"", 
													0, 
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
			CreateComponent(	"WComboBox", 
																"", 
																0, 
																20,
																720, 
																250, 
																100,
																wind2, 
																HMENU(299), 
																NULL);
			((WComboBox*)hWnd)->addDefaultTestItems();

			H_WND wind1 = 
			CreateComponent(	"WWindow", 
												"Title1", 
												0, 
												300,
												40, 
												200, 
												550,
												wind2, 
												HMENU(1001), 
												(LPVOID)ID_TYPE_WND_CX);
			{
				hWnd = 
				CreateComponent(	"WTextField", 
																"I'm just messing around with thread hooks. I've got one now that displays the clipboard, if it is in CF_TEXT format, whenever the user pastes in my application. The problem I've run into is that it can get the clipboard data fine if I've copied it from another application, but if I copy it from my own, it pastes just fine on the screen, but when I retrieve the clipboard data, its garbled. Heres the code.", 
																0, 
																20,
																40, 
																150, 
																23,
																wind1, 
																HMENU(99), 
																NULL);
				((WTextField*)hWnd)->setComponentAsChild(true);

				hWnd = 
				CreateComponent(	"WComboBox", 
																"", 
																0, 
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
				CreateComponent(	"WTree", 
					"Title2", 
					0, 
					50,
					140, 
					450, 
					450,
					wind2, 
					HMENU(1001), 
					NULL);

				hWnd = 
				CreateComponent(	"WTable", 
											"Table", 
											0, 
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
			CreateComponent(	"WInspector", 
													"Title1", 
													0, 
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