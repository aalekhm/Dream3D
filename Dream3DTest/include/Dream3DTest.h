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
		
		void drawTriangle();
		void drawGrid(int iSize, float fStep);
	private:
		Camera*		m_pCamera;
};

#endif


