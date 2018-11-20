#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Base.h"
#include "Common/Vectors.h"
#include "Common/Matrices.h"

class Transform {

	public:
		enum TransformDirtyBits {
			DIRTY_SCALE = 0x01,
			DIRTY_TRANSLATION = 0x02,
			DIRTY_ROTATION = 0x04,
			DIRTY_NOTIFY = 0x08
		};

		Transform();
		virtual ~Transform();

		void	scale(float fScale);
		void	scale(float sx, float sy, float sz);
		void	scale(const Vector3& v);
		void	scaleX(float sx);
		void	scaleY(float sy);
		void	scaleZ(float sz);

		void	translate(float tx, float ty, float tz);
		void	translate(const Vector3& v);
		void	translateForward(float amount);
		void	translateLeft(float amount);
		void	translateUp(float amount);

		void    rotate(float angle, const Vector3& axis);		// rotate angle(degree) along the given axix
		void    rotate(float angle, float x, float y, float z);
		void    rotate(const Vector3& vAngles);					// rotate on X,Y & Z-axis with angle in the Vector3
		void    rotateX(float angle);							// rotate on X-axis with degree
		void    rotateY(float angle);							// rotate on Y-axis with degree
		void    rotateZ(float angle);							// rotate on Z-axis with degree
		float   getRotateX();
		float   getRotateY();
		float   getRotateZ();

		void	setIdentity();
		void	setAxisX(const Vector3& vLeft);
		void	setAxisY(const Vector3& vUp);
		void	setAxisZ(const Vector3& vForward);
		void	setPosition(const Vector3& vPosition);

		void		setDirty(unsigned int bits);
		Matrix4&	getMatrix();
	private:
		Matrix4		m_Matrix;
		Vector3		m_vScale;
		Vector3		m_vTranslation;
		Vector3		m_vRotation;

		unsigned int	m_iDirty;
};

#endif