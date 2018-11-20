#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H
#include <Windows.h>
#include "Engine/KeyboardManager.h"
#include "Engine/MouseManager.h"
#include "Engine/Timer.h"

class EngineManager {

	public:
		EngineManager();
		~EngineManager();
		
		void	initTimer();
		bool	isKeyPressed(int iKeyID);
		void	setKeyPressed(int iKeyID);
		void	setKeyReleased(int iKeyID);

		void	setLMouseStatus(bool bPressed, int mouseX, int mouseY);
		void	setRMouseStatus(bool bPressed, int mouseX, int mouseY);
		void	setMouseMove(WPARAM keyState, int mouseX, int mouseY);

		void	update();
		void	render();
	private:
		Timer*				m_pTimer;
		KeyboardManager*	m_pKeyboardManager;
		MouseManager*		m_pMouseManager;
};

#endif

/*
- clean the screen
- 3D:
	- enable lighting, z-test, etc
	- set active matrix mode to projection
	- load identity and establish a perspective projection
	- set active matrix mode back to modelview
	- draw everything 3D
- 2D:
	- disable lighting, z-test, etc
	- set active matrix mode to projection
	- load identity and establish an orthogonal projection
	- set active matrix mode back to modelview
	- draw everything 2D
-	swap buffers
*/