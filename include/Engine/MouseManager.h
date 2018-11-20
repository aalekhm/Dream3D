#ifndef MOUSE_MANAGER_H
#define MOUSE_MANAGER_H

class MouseManager {

	public:
		MouseManager();
		~MouseManager();

		void	setLMouseStatus(bool bPressed, int mouseX, int mouseY);
		void	setRMouseStatus(bool bPressed, int mouseX, int mouseY);
		void	setMouseMove(unsigned int keyState, int mouseX, int mouseY);
	private:
		bool	m_bLMouseDown;
		bool	m_bRMouseDown;

		int		m_iMouseX;
		int		m_iMouseY;
};

#endif MOUSE_MANAGER_H