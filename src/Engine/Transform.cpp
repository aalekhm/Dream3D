#include "Engine/Transform.h"

Transform::Transform() 
	:	m_vScale(Vector3::one()),
		m_iDirty(0)
{

}

void Transform::setDirty(unsigned int iDirtyBits) {
	m_iDirty |= iDirtyBits;
}

void Transform::scale(float fScale) {
	m_vScale *= fScale;
	setDirty(DIRTY_SCALE);
}

void Transform::scale(float sx, float sy, float sz) {
	m_vScale.x *= sx;
	m_vScale.y *= sy;
	m_vScale.z *= sz;

	setDirty(DIRTY_SCALE);
}

void Transform::scale(const Vector3& v) {
	m_vScale.x *= v.x;
	m_vScale.y *= v.y;
	m_vScale.z *= v.z;

	setDirty(DIRTY_SCALE);
}

void Transform::scaleX(float sx) {
	m_vScale.x *= sx;
	setDirty(DIRTY_SCALE);
}

void Transform::scaleY(float sy) {
	m_vScale.y *= sy;
	setDirty(DIRTY_SCALE);
}

void Transform::scaleZ(float sz) {
	m_vScale.z *= sz;
	setDirty(DIRTY_SCALE);
}

void Transform::translate(float tx, float ty, float tz) {
	Vector3 tVector(tx, ty, tz);
	m_vTranslation += tVector;
	setDirty(DIRTY_TRANSLATION);
}

void Transform::translate(const Vector3& v) {
	m_vTranslation += v;
	setDirty(DIRTY_TRANSLATION);
}

void Transform::translateForward(float amount) {

	// Force the current transform matrix to be updated.
	getMatrix();

	Vector3 viewVector = m_Matrix.getColumn(2);
	viewVector.normalize();
	viewVector *= amount;
	translate(viewVector);
}

void Transform::translateLeft(float amount) {

	// Force the current transform matrix to be updated.
	getMatrix();

	Vector3 rightVector = m_Matrix.getColumn(0);
	rightVector.normalize();
	rightVector *= amount;
	translate(rightVector);
}

void Transform::translateUp(float amount) {

	// Force the current transform matrix to be updated.
	getMatrix();

	Vector3 upVector = m_Matrix.getColumn(1);
	upVector.normalize();
	upVector*= amount;
	translate(upVector);
}

void Transform::rotate(float angle, const Vector3& axis) {
	m_Matrix.rotate(angle, axis);
	setDirty(DIRTY_ROTATION);
}

void Transform::rotate(float angle, float x, float y, float z) {
	m_Matrix.rotate(angle, x, y, z);
	setDirty(DIRTY_ROTATION);
}

void Transform::rotate(const Vector3& vAngles) {
	rotateX(vAngles.x);
	rotateY(vAngles.y);
	rotateZ(vAngles.z);
	setDirty(DIRTY_ROTATION);
}

void Transform::rotateX(float angle) {
	m_vRotation.x += angle;
	setDirty(DIRTY_ROTATION);
}
							
void Transform::rotateY(float angle) {
	m_vRotation.y += angle;
	setDirty(DIRTY_ROTATION);
}

void Transform::rotateZ(float angle) {
	m_vRotation.z += angle;
	setDirty(DIRTY_ROTATION);
}

float Transform::getRotateX() {
	return m_vRotation.x;
}

float Transform::getRotateY() {
	return m_vRotation.y;
}

float Transform::getRotateZ() {
	return m_vRotation.z;
}

Matrix4& Transform::getMatrix() {
	if(m_iDirty) {
		m_Matrix.identity();

		bool bHasTranslation = !m_vTranslation.isZero();
		bool bHasScaling = !m_vScale.isOne();
		bool bHasRotation = !m_vRotation.isZero();

		if(bHasTranslation || (m_iDirty & DIRTY_TRANSLATION) == DIRTY_TRANSLATION) {
			m_Matrix.translate(-m_vTranslation);

			if(bHasRotation || (m_iDirty & DIRTY_ROTATION) == DIRTY_ROTATION) {
				m_Matrix.rotate(-m_vRotation);
			}

			if(bHasScaling || (m_iDirty & DIRTY_SCALE) == DIRTY_SCALE) {
				m_Matrix.scale(m_vScale);
			}
		}
		else
		if(bHasRotation || (m_iDirty & DIRTY_ROTATION) == DIRTY_ROTATION) {
			m_Matrix.rotate(-m_vRotation);

			if(bHasScaling || (m_iDirty & DIRTY_SCALE) == DIRTY_SCALE) {
				m_Matrix.scale(m_vScale);
			}
		}
		else
		if(bHasScaling || (m_iDirty & DIRTY_SCALE) == DIRTY_SCALE) {
			m_Matrix.scale(m_vScale);
		}

		m_iDirty &= ~DIRTY_TRANSLATION & ~DIRTY_ROTATION & ~DIRTY_SCALE;
	}

	return m_Matrix;
}

void Transform::setIdentity() {
	m_Matrix.identity();
}

void Transform::setAxisX(const Vector3& vLeft) {
	m_Matrix.setColumn(0, vLeft);
}

void Transform::setAxisY(const Vector3& vUp) {
	m_Matrix.setColumn(1, vUp);
}

void Transform::setAxisZ(const Vector3& vForward) {
	m_Matrix.setColumn(2, vForward);
}

void Transform::setPosition(const Vector3& vPosition) {
	m_Matrix.setRow(3, vPosition);
}

Transform::~Transform() {

}
