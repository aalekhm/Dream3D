#include "Dream3DTest.h"
#include "Engine/Mesh.h"

#define ROTATE_SPEED 0.05f
#define WALK_SPEED 0.001f

// Declare our game instance
Dream3DTest game;

Mesh* createTriangleMesh1();
Mesh* triangleMesh1;
Mesh* createTriangleMesh2();
Mesh* triangleMesh2;
Dream3DTest::Dream3DTest() {

}

Dream3DTest::~Dream3DTest() {
	
}

void Dream3DTest::initialize() {
	m_pCamera = Camera::createPerspective(0, 0, getWidth(), getHeight(), 45.0f, 0.1f, 10.0f);

	glEnable(GL_DEPTH_TEST);

	triangleMesh1 = createTriangleMesh1();
	triangleMesh2 = createTriangleMesh2();
}

void Dream3DTest::update(float deltaTimeMs) {
	if(isKeyPressed(VK_ESCAPE))
		exit();

	m_pCamera->update(deltaTimeMs);
}

void Dream3DTest::drawGrid(int iSize, float fStep) {
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

void Dream3DTest::drawTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3) {
	glBegin(GL_TRIANGLES);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(p1.x, p1.y, p1.z);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(p2.x, p2.y, p2.z);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(p3.x, p3.y, p3.z);
	glEnd();
}

void Dream3DTest::drawLine(const Vector3& p1, const Vector3& p2) {
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

Mesh* createTriangleMesh1()
{
	// Calculate the vertices of the equilateral triangle.
	Vector3 p1(0,		0.5f,	-1.0f);
	Vector3 p2(-0.5f,	0,		-1.0f);
	Vector3 p3(0.5f,	0,		-1.0f);

	// Create 3 vertices. Each vertex has position (x, y, z) and color (red, green, blue)
	float vertices[] =
	{
		p1.x, p1.y, -0.5f,    0.0f, 1.0f, 0.0f,
		p2.x, p2.y, 0.0f,     1.0f, 0.0f, 0.0f,
		p3.x, p3.y, 0.0f,     0.0f, 0.0f, 1.0f,
	};
	unsigned int vertexCount = 3;
	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::COLOR, 3)
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
	return mesh;
}

Mesh* createTriangleMesh2()
{
	// Calculate the vertices of the equilateral triangle.
	Vector3 p1(0,		0.5f,	-1.0f);
	Vector3 p2(-0.5f,	0,		-1.0f);
	Vector3 p3(0.5f,	0,		-1.0f);

	// Create 3 vertices. Each vertex has position (x, y, z) and color (red, green, blue)
	float vertices[] =
	{
		p1.x, p1.y, 0.0f,     0.0f, 0.0f, 1.0f,
		p2.x, p2.y, 0.0f,     0.0f, 1.0f, 0.0f,
		p3.x, p3.y, 0.0f,     1.0f, 0.0f, 0.0f,
	};
	unsigned int vertexCount = 3;
	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::COLOR, 3)
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
	return mesh;
}

void Dream3DTest::render(float deltaTimeMs) {
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

	//No Transformations to the World(grid) & triangle hence load the 'view matrix'.
	glLoadMatrixf(m_pCamera->getViewMatrix().getTranspose());
	drawGrid(10, 0.5);
	drawTriangle(Vector3(0, 0.5f, -1.0f), Vector3(-0.5f, 0, -1.0f), Vector3(0.5f, 0, -1.0f));

	////////////////////////////////////////////////////////////////
	// Software mode - No VAO's
	GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, triangleMesh1->getVertexBuffer()) );
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)triangleMesh1->getVertexSize(), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, (GLsizei)triangleMesh1->getVertexSize(), (GLvoid*)(3*sizeof(float)));
	glEnableVertexAttribArray(3);
	glDrawArrays(triangleMesh1->getPrimitiveType(), 0, triangleMesh1->getVertexCount());
	GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0) );
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(3);
	
	////////////////////////////////////////////////////////////////
	// Hardware mode - With VAO's
	glBindVertexArray(triangleMesh2->getVertexArrayBuffer());
	glDrawArrays(triangleMesh2->getPrimitiveType(), 0, triangleMesh2->getVertexCount());
	glBindVertexArray(0);
}


















