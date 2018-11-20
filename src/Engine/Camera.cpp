#include "Engine/Camera.h"

Camera::Camera()
	:	m_CameraPosition(),
		m_CameraAngle(),
		m_MatrixView()
{

}

Camera::~Camera() 
{

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

void Camera::update()
{
	// transform the camera (viewing matrix) from world space to eye space
	// Notice all values are negated, because we move the whole scene with the
	// inverse of camera transform    matrixView.identity();
	m_MatrixView.identity();

	translate(m_MatrixView, m_CameraPosition);

	m_MatrixView.rotateZ(-m_CameraAngle.z);    // roll
	m_MatrixView.rotateY(-m_CameraAngle.y);    // heading
	m_MatrixView.rotateX(-m_CameraAngle.x);    // pitch	
}

void Camera::translate(Matrix4 &mat, Vector3 camPos) {
	mat[3] = -camPos.x;
	mat[7] = -camPos.y;
	mat[11] = -camPos.z;
}
