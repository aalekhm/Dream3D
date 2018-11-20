#ifndef DREAM3DTEST_H
#define DREAM3DTEST_H

#include "Engine/EngineManager.h"
#include "Engine/Camera.h"
#include "Engine/Node.h"

class Dream3DTest : EngineManager
{
	public:
		Dream3DTest();
		virtual ~Dream3DTest();
		
		void initialize();
		void update(float elapsedTime);
		void render(float elapsedTime);
	private:
		Node*		m_pCameraNode;
};

#endif


