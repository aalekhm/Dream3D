#include "Dream3DTest.h"

#define ROTATE_SPEED 0.05f
#define WALK_SPEED 0.001f

// Declare our game instance
Dream3DTest game;

Dream3DTest::Dream3DTest() {

}

Dream3DTest::~Dream3DTest() {
	
}

void Dream3DTest::initialize() {
	m_pCamera = Camera::createPerspective(0, 0, getWidth(), getHeight(), 45.0f, 0.1f, 10.0f);

	glEnable(GL_DEPTH_TEST);
}

void Dream3DTest::update(float deltaTimeMs) {
	if(isKeyPressed(VK_ESCAPE))
		exit();

	m_pCamera->update(deltaTimeMs);
}

void Dream3DTest::drawTriangle() {
	glBegin(GL_TRIANGLES);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(0, 0.5f, -1.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, 0, -1.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0, -1.0f);
	glEnd();
}

void Dream3DTest::drawGrid(int iSize, float fStep) {
	glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		for(float i = -iSize; i < iSize; i += fStep) {
			
			//Vertical
			glVertex3f(-i, 0.0f, iSize);
			glVertex3f(-i, 0.0f, -iSize);

			//Horizontal
			glVertex3f(iSize, 0.0f, i);
			glVertex3f(-iSize, 0.0f, i);
		}
	glEnd();
}

void Dream3DTest::render(float deltaTimeMs) {
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

	//No Transformations to the World(grid) & triangle hence load the 'view matrix'.
	glLoadMatrixf(m_pCamera->getViewMatrix().getTranspose());
	drawGrid(10, 0.5);
	drawTriangle();
}

