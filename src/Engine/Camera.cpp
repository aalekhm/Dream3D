#include "Engine/Camera.h"
#include "Engine/EngineManager.h"

#define ROTATE_SPEED 0.05f
#define WALK_SPEED 0.001f

// Camera dirty bits
#define CAMERA_DIRTY_VIEW 1
#define CAMERA_DIRTY_PROJ 2
#define CAMERA_DIRTY_VIEW_PROJ 4
#define CAMERA_DIRTY_INV_VIEW 8
#define CAMERA_DIRTY_INV_VIEW_PROJ 16
#define CAMERA_DIRTY_BOUNDS 32
#define CAMERA_DIRTY_ALL (CAMERA_DIRTY_VIEW | CAMERA_DIRTY_PROJ | CAMERA_DIRTY_VIEW_PROJ | CAMERA_DIRTY_INV_VIEW | CAMERA_DIRTY_INV_VIEW_PROJ | CAMERA_DIRTY_BOUNDS)

Camera::Camera(int x, int y, int w, int h, float iFieldOfView, float fNearPlane, float fFarPlane)
	:	m_iCameraType(PERSPECTIVE),
		m_iViewX(x),
		m_iViewY(y),
		m_iViewW(w),
		m_iViewH(h),
		m_fAspectRatio((float)(w)/h),
		m_fFieldOfView(iFieldOfView),
		m_fNearPlane(fNearPlane),
		m_fFarPlane(fFarPlane),

		m_iDirty(CAMERA_DIRTY_ALL),

		m_CameraPosition(),
		m_CameraAngle(),
		m_MatrixView(),
		m_MatrixProjection(),
		m_MatrixViewProjection()
{
	
}

Camera::Camera(int x, int y, int w, int h, float fNearPlane, float fFarPlane)
	:	m_iCameraType(ORTHOGRAPHIC),
		m_iViewX(x),
		m_iViewY(y),
		m_iViewW(w),
		m_iViewH(h),
		m_fAspectRatio((float)(w)/h),
		m_fNearPlane(fNearPlane),
		m_fFarPlane(fFarPlane),
		
		m_iDirty(CAMERA_DIRTY_ALL),

		m_CameraPosition(),
		m_CameraAngle(),
		m_MatrixView(),
		m_MatrixProjection(),
		m_MatrixViewProjection()
{
	m_iViewX = x;
	m_iViewY = y;
	m_iViewW = w;
	m_iViewH = h;
	m_fAspectRatio = (float)(w)/h;
	m_fNearPlane = fNearPlane;
	m_fFarPlane = fFarPlane;
}

Camera::~Camera() 
{

}

Camera* Camera::createPerspective(int x, int y, int w, int h, float iFieldOfView, float fNearPlane, float fFarPlane) {
	Camera* cam = new Camera(x, y, w, h, iFieldOfView, fNearPlane, fFarPlane);
	cam->setPerspective(x, y, w, h, iFieldOfView, fNearPlane, fFarPlane);

	return cam;
}

Camera* Camera::createOrthographic(int x, int y, int w, int h, float fNearPlane, float fFarPlane) {
	Camera* cam = new Camera(x, y, w, h, fNearPlane, fFarPlane);
	cam->setOrthographic(x, y, w, h, fNearPlane, fFarPlane);
	
	return cam;
}

Matrix4& Camera::getProjectionMatrix() {
	if(m_iDirty & CAMERA_DIRTY_PROJ) {
		
		if(m_iCameraType == PERSPECTIVE) {
			setPerspective(m_iViewX, m_iViewY, m_iViewW, m_iViewH, m_fFieldOfView, m_fNearPlane, m_fFarPlane);
		}
		else {
			setOrthographic(m_iViewX, m_iViewY, m_iViewW, m_iViewH, m_fNearPlane, m_fFarPlane);
		}

		m_iDirty &= ~CAMERA_DIRTY_PROJ;
	}

	return m_MatrixProjection;
}

Matrix4& Camera::getViewProjectionMatrix() {
	if (m_iDirty & CAMERA_DIRTY_VIEW_PROJ)
	{
		m_MatrixViewProjection = getProjectionMatrix() * getViewMatrix();
		m_iDirty &= ~CAMERA_DIRTY_VIEW_PROJ;
	}

	return m_MatrixViewProjection;
}

Matrix4& Camera::getViewMatrix() {
	if (m_iDirty & CAMERA_DIRTY_VIEW)
	{
		//if (_node)
		{
			// The view matrix is the inverse of our transform matrix.
			//_node->getWorldMatrix().invert(&_view);
		}
		//else
		{
			m_MatrixView.identity();
		}

		m_iDirty &= ~CAMERA_DIRTY_VIEW;
	}

	return m_MatrixView;
}

///////////////////////////////////////////////////////////////////////////////
// configure projection and viewport of sub window
///////////////////////////////////////////////////////////////////////////////
void Camera::setPerspective(int x, int y, int w, int h, float iFieldOfView, float fNearPlane, float fFarPlane) {
	// set viewport
	glViewport(x, y, w, h);

	// set perspective viewing frustum
	setFrustum(iFieldOfView, (float)(w)/h, fNearPlane, fFarPlane); // FOV, AspectRatio, NearClip, FarClip

	// copy projection matrix to OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_MatrixProjection.getTranspose());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
// set a symmetric perspective frustum with 4 params similar to gluPerspective
// (vertical field of view, aspect ratio, near, far)
///////////////////////////////////////////////////////////////////////////////
void Camera::setFrustum(float fovY, float aspectRatio, float front, float back) {
	float tangent = tanf(fovY/2 * DEG2RAD);   // tangent of half fovY
	float height = front * tangent;           // half height of near plane
	float width = height * aspectRatio;       // half width of near plane

	// params: left, right, bottom, top, near, far
	setPerspectiveFrustum(-width, width, -height, height, front, back);
}

///////////////////////////////////////////////////////////////////////////////
// set a perspective frustum with 6 params similar to glFrustum()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
void Camera::setPerspectiveFrustum(float l, float r, float b, float t, float n, float f) {
	m_MatrixProjection.identity();
	m_MatrixProjection[0]  =  2 * n / (r - l);
	m_MatrixProjection[2]  =  (r + l) / (r - l);
	m_MatrixProjection[5]  =  2 * n / (t - b);
	m_MatrixProjection[6]  =  (t + b) / (t - b);
	m_MatrixProjection[10] = -(f + n) / (f - n);
	m_MatrixProjection[11] = -(2 * f * n) / (f - n);
	m_MatrixProjection[14] = -1;
	m_MatrixProjection[15] =  0;
}

void Camera::setOrthographic(int x, int y, int w, int h, float iNearPlane, float iFarPlane) {
	setOrthogonalFrustum((float)x, (float)x+w, (float)y+h, (float)y, 0.0f, 1.0f);
	
	glMatrixMode(GL_PROJECTION);						// Setup and orthogonal, pixel-to-pixel projection matrix
	glLoadMatrixf(m_MatrixProjection.getTranspose());	// copy projection matrix to OpenGL

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
// set a orthographic frustum with 6 params similar to glOrtho()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
void Camera::setOrthogonalFrustum(float l, float r, float b, float t, float n, float f) {
	m_MatrixProjection.identity();
	m_MatrixProjection[0]  =  2 / (r - l);
	m_MatrixProjection[3]  =  -(r + l) / (r - l);
	m_MatrixProjection[5]  =  2 / (t - b);
	m_MatrixProjection[7]  =  -(t + b) / (t - b);
	m_MatrixProjection[10] = -2 / (f - n);
	m_MatrixProjection[11] = -(f + n) / (f - n);
}

void Camera::setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ) {
	float forward[4];
	float up[4];
	float left[4];
	float position[4];
	float invLength;

	// determine forward vector (direction reversed because it is camera)
	forward[0] = -(posX - targetX);    // x
	forward[1] = -(posY - targetY);    // y
	forward[2] = -(posZ - targetZ);    // z
	forward[3] = 0.0f;              // w

	// normalize it without w-component
	invLength = 1.0f / sqrtf(forward[0]*forward[0] + forward[1]*forward[1] + forward[2]*forward[2]);
	forward[0] *= invLength;
	forward[1] *= invLength;
	forward[2] *= invLength;

	// assume up direction is straight up
	up[0] = 0.0f;   // x
	up[1] = 1.0f;   // y
	up[2] = 0.0f;   // z
	up[3] = 0.0f;   // w

	// compute left vector with cross product
	left[0] = up[1]*forward[2] - up[2]*forward[1];  // x
	left[1] = up[2]*forward[0] - up[0]*forward[2];  // y
	left[2] = up[0]*forward[1] - up[1]*forward[0];  // z
	left[3] = 1.0f;                                 // w

	// re-compute orthogonal up vector
	up[0] = forward[1]*left[2] - forward[2]*left[1];    // x
	up[1] = forward[2]*left[0] - forward[0]*left[2];    // y
	up[2] = forward[0]*left[1] - forward[1]*left[0];    // z
	up[3] = 0.0f;                                       // w

	// camera position
	position[0] = -posX;
	position[1] = -posY;
	position[2] = -posZ;
	position[3] = 1.0f;

	// copy axis vectors to matrix
	m_MatrixView.identity();
	m_MatrixView.setColumn(0, left);
	m_MatrixView.setColumn(1, up);
	m_MatrixView.setColumn(2, forward);
	m_MatrixView.setRow(3, position);
}

void Camera::translate(Matrix4 &mat, Vector3 camPos) {
	mat[3] = -camPos.x;
	mat[7] = -camPos.y;
	mat[11] = -camPos.z;
}

void Camera::moveForward(float distance) {
	Vector3 viewVector = m_MatrixView.getColumn(2);
	viewVector *= distance;
	m_CameraPosition += viewVector;
}

void Camera::strafe(float distance) {
	Vector3 rightVector = m_MatrixView.getColumn(0);
	rightVector *= distance;
	m_CameraPosition += rightVector;
}

void Camera::moveUpward(float distance) {
	Vector3 upVector = m_MatrixView.getColumn(1);
	upVector *= distance;
	m_CameraPosition += upVector;
}

void Camera::handleMouse() {
	int screenCenterX = EngineManager::getInstance()->getWidth()>>1;
	int screenCenterY = EngineManager::getInstance()->getHeight()>>1;

	POINT mousePos;
	GetCursorPos(&mousePos);

	//Mouse not moved, hence return;
	if(mousePos.x == screenCenterX && mousePos.y == screenCenterY)
		return;

	SetCursorPos(screenCenterX, screenCenterY);

	//Move, Rotate as proportinal to the displacement
	float deltaX = (mousePos.x - screenCenterX)/5.0f;
	float deltaY = (mousePos.y - screenCenterY)/5.0f;

	bool yesRotateX = false;
	if(deltaY <= 0) {
		if(m_CameraAngle.x < 80) {
			yesRotateX = true;
		}
	}
	else 
	if(deltaY > 0) {
		if(m_CameraAngle.x > -80) {
			yesRotateX = true;
		}
	}

		if(yesRotateX) {
			m_CameraAngle.x -= deltaY;
		}

		m_CameraAngle.y -= deltaX;
}

void Camera::handleKeyboard(float deltaTimeMs) {
	if(EngineManager::isKeyPressed('Q') || EngineManager::isKeyPressed(VK_LEFT)) {
		m_CameraAngle.y += ROTATE_SPEED * deltaTimeMs;
		if(m_CameraAngle.y > 360.0f) m_CameraAngle.y = 0.0f;
	}
	else
	if(EngineManager::isKeyPressed('E') || EngineManager::isKeyPressed(VK_RIGHT)) {
		m_CameraAngle.y -= ROTATE_SPEED * deltaTimeMs;
		if(m_CameraAngle.y < 0.0f) m_CameraAngle.y = 360.0f;
	}

	if(EngineManager::isKeyPressed('W')) {
		moveForward(-WALK_SPEED * deltaTimeMs);
	}
	else
	if(EngineManager::isKeyPressed('S')) {
		moveForward(WALK_SPEED * deltaTimeMs);
	}

	if(EngineManager::isKeyPressed('A')) {
		strafe(-WALK_SPEED * deltaTimeMs);
	}
	else
	if(EngineManager::isKeyPressed('D')) {
		strafe(WALK_SPEED * deltaTimeMs);
	}

	if(EngineManager::isKeyPressed(VK_UP)) {
		if(m_CameraAngle.x < 80) {
			m_CameraAngle.x += ROTATE_SPEED * deltaTimeMs;
		}
	}
	else
	if(EngineManager::isKeyPressed(VK_DOWN)) {
		if(m_CameraAngle.x > -80) {
			m_CameraAngle.x -= ROTATE_SPEED * deltaTimeMs;
		}
	}

	if(EngineManager::isKeyPressed(VK_PRIOR)) {
		moveUpward(WALK_SPEED * deltaTimeMs);
	}
	else
	if(EngineManager::isKeyPressed(VK_NEXT)) {
		moveUpward(-WALK_SPEED * deltaTimeMs);
	}
}

void Camera::updateViewMatrix() {
	// transform the camera (viewing matrix) from world space to eye space
	// Notice all values are negated, because we move the whole scene with the
	// inverse of camera transform    matrixView.identity();
	m_MatrixView.identity();

	translate(m_MatrixView, m_CameraPosition);

	m_MatrixView.rotateZ(-m_CameraAngle.z);    // roll
	m_MatrixView.rotateY(-m_CameraAngle.y);    // heading
	m_MatrixView.rotateX(-m_CameraAngle.x);    // pitch	
}

void Camera::update(float deltaTimeMs) {
	handleKeyboard(deltaTimeMs);
	handleMouse();

	updateViewMatrix();
}
