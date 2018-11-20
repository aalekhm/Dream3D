#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H

#include "Engine/KeyboardManager.h"
#include "Engine/MouseManager.h"
#include "Engine/Timer.h"
#include "Engine/Base.h"
#include "Engine/UI/WWidgetManager.h"

class EngineManager {

	friend class Node;
	friend class Camera;

	public:
		enum State {
			UNINITIALIZED,
			RUNNING,
			PAUSED
		};

		EngineManager();
		~EngineManager();
		
		static EngineManager* getInstance();
		MouseManager*	getMouseManager() const;

		void					startup(HWND pHWnd);
		void					shutdown();
		void					exit();
		State				getState();

		void					setViewport(int w, int h);
		void					initTimer();
		Timer*				getTimer();
		HWND				getWindowHandle();

		static bool			isKeyPressed(int iKeyID);
		void					keyPressed(unsigned int iVirtualKeycode, unsigned short ch);
		void					keyReleased(unsigned int iVirtualKeycode, unsigned short ch);
	
		void					onMouseDown(int mCode, int x, int y);
		void					onMouseMove(int mCode, int x, int y);
		void					onMouseUp(int mCode, int x, int y);
		void					onMouseWheel(WPARAM wParam, LPARAM lParam);

		void					setLMouseStatus(bool bPressed, int mouseX, int mouseY);
		void					setMMouseStatus(bool bPressed, int mouseX, int mouseY);
		void					setRMouseStatus(bool bPressed, int mouseX, int mouseY);
		void					setMouseMove(WPARAM keyState, int mouseX, int mouseY);

		unsigned int		getWidth();
		unsigned int		getHeight();
		
		void					frame();
		void					updateFPS();
		unsigned int		getFPS();

		virtual void			initialize() = 0;
		virtual void			update(float elapsedTime) = 0;
		virtual void			render(float elapsedTime) = 0;

		void					addUIListener(YAGUICallback callbackProc);
		Camera*			getUICamera();
	private:
		static EngineManager*	m_pEngineManager;
		Timer*							m_pTimer;
		KeyboardManager*			m_pKeyboardManager;
		MouseManager*				m_pMouseManager;
		WWidgetManager*			m_pWidgetManager;
		HWND							m_pHWnd;
		Node*							m_pYAGUICameraNode;

		int								m_iViewportW;
		int								m_iViewportH;

		State							m_iState;
		bool								m_bInitialized;

		unsigned int					m_iFrameCount;
		unsigned int					m_iFrameRate;
		double							m_dLastElapsedFPSTimeMs;
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