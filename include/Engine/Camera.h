#ifndef CAMERA_H
#define CAMERA_H

#include "Common/Vectors.h"
#include "Common/Matrices.h"

class Camera {

	public:
		Camera();
		~Camera();

		void		setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
		void		update();
		void		translate(Matrix4 &mat, Vector3 camPos);
	private:
		Vector3		m_CameraPosition;
		Vector3		m_CameraAngle;

		Matrix4		m_MatrixView;
};

#endif