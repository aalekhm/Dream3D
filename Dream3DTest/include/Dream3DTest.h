#ifndef DREAM3DTEST_H
#define DREAM3DTEST_H

#include "Engine/EngineManager.h"

class Dream3DTest : EngineManager
{
	public:
		Dream3DTest();
		virtual ~Dream3DTest();
		
		void initialize();
		void update(float elapsedTime);
		void render(float elapsedTime);
		
		void drawTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3);
		void drawGrid(int iSize, float fStep);
		void drawLine(const Vector3& p1, const Vector3& p2);
	private:
		Camera*		m_pCamera;
};

#endif


