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

// Declare our game instance
Dream3DTest game;

void drawTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3);
void drawGrid(int iSize, float fStep);
void drawLine(const Vector3& p1, const Vector3& p2);

Model* createTriangleModel();
Model* triangleModel;

Model* createTriangleStripModel();
Model* triangleStripModel;

Model* createLineStripModel();
Model* lineStripModel;

Model* createLineModel();
Model* lineModel;

Model* createCubeModelIndexed(float size);
Model* cubeModel;

SpriteBatch* createSpriteBatch();
SpriteBatch* spriteBatch;

GLuint texID[2];
GLuint textureWH[2][2];

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
/////////////////////////////////////////

bool loadTexture(char* sTexName, GLuint &texid, GLuint* texWH) {
	TGAImg Img;        // Image loader

	// Load our Texture
	if(Img.Load(sTexName) != IMG_OK)
		return false;

	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid); // Set our Tex handle as current

	// Specify filtering and edge actions
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	// Create the texture
	if(Img.GetBPP() == 24) {
		glTexImage2D(GL_TEXTURE_2D, 0, 3, Img.GetWidth(), Img.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, Img.GetImg());
	}
	else 
	if(Img.GetBPP() == 32){
		glTexImage2D(GL_TEXTURE_2D, 0, 4, Img.GetWidth(), Img.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Img.GetImg());
	}
	else
		return false;

	texWH[0] = Img.GetWidth();
	texWH[1] = Img.GetHeight();
	glDisable(GL_TEXTURE_2D);

	return true;
}

Dream3DTest::Dream3DTest() {

}

Dream3DTest::~Dream3DTest() {
	
}

void Dream3DTest::initialize() {
	Camera* pCamera = Camera::createPerspective(0, 0, getWidth(), getHeight(), 45.0f, 0.01f, 10.0f);
	m_pCameraNode = Node::create("FirstPersonShooterCamera");
	m_pCameraNode->setCamera(pCamera);

	glEnable(GL_DEPTH_TEST);

	triangleModel = createTriangleModel();
	triangleStripModel = createTriangleStripModel();
	
	lineStripModel = createLineStripModel();

	lineModel = createLineModel();

	cubeModel = createCubeModelIndexed(0.25f);

	meshBatch = createMeshBatch();
	spriteBatch = createSpriteBatch();

	loadTexture("data/ColorFul_2048x1300.tga", texID[0], textureWH[0]);
	loadTexture("data/core.tga", texID[1], textureWH[1]);
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

Model* createTriangleModel()
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
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::COLOR, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
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
	pModel->setTexture("data/ColorFul_2048x1300.tga");

	return pModel;
}

Model* createTriangleStripModel() {
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
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::COLOR, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
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
	pModel->setTexture("data/cartoon.tga");

	return pModel;
}

Model* createLineStripModel() {
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
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::COLOR, 3)
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);

	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, elementCount), vertexCount, false);
	GL_ASSERT( mesh );

	mesh->setPrimitiveType(Mesh::LINE_STRIP);
	mesh->setVertexData(vertices, 0, vertexCount);

	Model* pModel = Model::create(mesh);
	return pModel;
}

Model* createLineModel() {

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
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::COLOR, 3)
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);

	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, elementCount), vertexCount, false);
	GL_ASSERT( mesh );
	mesh->setPrimitiveType(Mesh::LINES);
	mesh->setVertexData(vertices, 0, vertexCount);

	Model* pModel = Model::create(mesh);

	return pModel;
}

Model* createCubeModelIndexed(float size = 1.0f) {
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
		VertexFormat::Element(VertexFormat::POSITION, 3),
		//VertexFormat::Element(VertexFormat::NORMAL, 3),
		VertexFormat::Element(VertexFormat::COLOR, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
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
	pModel->setTexture("data/cartoon.tga");

	return pModel;
}

MeshBatch* createMeshBatch() {
	VertexFormat::Element elements[] = 
	{
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::COLOR, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
	};
	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
	VertexFormat* vf = new VertexFormat(elements, elementCount);
	MeshBatch* mb = MeshBatch::create(*vf, Mesh::TRIANGLES, false);
	
	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0, 0.5f, -0.5f), randomColor(), 0.0f, 0.0f));
	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(-0.5f, -0.5f, -0.5f), randomColor(), 0.0f, 1.0f));
	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0.5f, -0.5f, -0.5f), randomColor(), 1.0f, 1.0f));

	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0, 0.5f, -1.5f), randomColor(), 0.0f, 0.0f));
	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(-0.5f, -0.5f, -0.5f), randomColor(), 0.0f, 1.0f));
	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0.5f, -0.5f, -0.5f), randomColor(), 1.0f, 1.0f));

	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0, 0.5f, 1.5f), randomColor(), 0.0f, 0.0f));
	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(-0.5f, -0.5f, -0.5f), randomColor(), 0.0f, 1.0f));
	_meshBatchVertices.push_back(MeshBatchVertex(Vector3(0.5f, -0.5f, -0.5f), randomColor(), 1.0f, 1.0f));

	mb->setTexture("data/cartoon.tga");

	return mb;
}

SpriteBatch* createSpriteBatch() {
	SpriteBatch* spriteBatch = SpriteBatch::create("data/cartoon.tga");
	GP_ASSERT( spriteBatch );

	return spriteBatch;
}

void Dream3DTest::update(float deltaTimeMs) {
	if(isKeyPressed(VK_ESCAPE))
		exit();

	m_pCameraNode->getCamera()->update(deltaTimeMs);
}

static float rAngle = 0.0f;
const float ROTATION_PER_SECOND = 0.25*0.36f;//360deg/sec = 360/1000;

void Dream3DTest::render(float deltaTimeMs) {
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

	m_pCameraNode->getCamera()->setType(Camera::PERSPECTIVE);

	//No Transformations to the World(grid) & triangle hence load the 'view matrix'.
	glLoadMatrixf(m_pCameraNode->getWorldMatrix().getTranspose());
	drawGrid(10, 0.5);
	drawTriangle(Vector3(0, 0.5f, -1.5f), Vector3(-0.5f, 0, -1.5f), Vector3(0.5f, 0, -1.5f));
	
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texID[0]);
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
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	////////////////////////////////////////////////////////////////
	triangleModel->draw(!true);

	triangleStripModel->draw(!true);

	lineStripModel->draw();

	lineModel->draw();

	cubeModel->draw(!true);
	
	// Draw all of the triangles as one mesh batch.
	meshBatch->start();
	meshBatch->add(&_meshBatchVertices[0], (unsigned int)_meshBatchVertices.size());
	meshBatch->stop();
	meshBatch->draw();
	////////////////////////////////////////////////////////////////

	m_pCameraNode->getCamera()->setType(Camera::ORTHOGRAPHIC);

	Vector4 src(0, 0, 900.0f, 695.0f);
	spriteBatch->setClip(0, 0, getWidth(), getHeight());
	spriteBatch->start();
		spriteBatch->setClip(0, 0, 100, 100);
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


















