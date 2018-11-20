#ifndef MOUSE_MANAGER_H
#define MOUSE_MANAGER_H

class MouseManager {

	public:
		MouseManager();
		~MouseManager();

		void	setLMouseStatus(bool bPressed, int mouseX, int mouseY);
		void	setMMouseStatus(bool bPressed, int mouseX, int mouseY);
		void	setRMouseStatus(bool bPressed, int mouseX, int mouseY);
		void	setMouseMove(unsigned int keyState, int mouseX, int mouseY);

		int		getMousePosX();
		int		getMousePosY();
	private:
		bool	m_bLMouseDown;
		bool	m_bMMouseDown;
		bool	m_bRMouseDown;

		int		m_iMouseX;
		int		m_iMouseY;

		unsigned int	m_iVirtualKeyCode;
};

#endif MOUSE_MANAGER_H