#include "Engine\MouseManager.h"

MouseManager::MouseManager() 
	:	m_bLMouseDown(false),
		m_bMMouseDown(false),
		m_bRMouseDown(false),
		m_iMouseX(0),
		m_iMouseY(0),
		m_iVirtualKeyCode(0)
{

}

void MouseManager::setLMouseStatus(bool bPressed, int mouseX, int mouseY) {
	m_bLMouseDown = bPressed;

	m_iMouseX = mouseX;
	m_iMouseY = mouseY;
}

void MouseManager::setMMouseStatus(bool bPressed, int mouseX, int mouseY) {
	m_bMMouseDown = bPressed;

	m_iMouseX = mouseX;
	m_iMouseY = mouseY;
}

void MouseManager::setRMouseStatus(bool bPressed, int mouseX, int mouseY) {
	m_bRMouseDown = bPressed;

	m_iMouseX = mouseX;
	m_iMouseY = mouseY;
}

void MouseManager::setMouseMove(unsigned int keyState, int mouseX, int mouseY) {
	m_iVirtualKeyCode = keyState;

	m_iMouseX = mouseX;
	m_iMouseY = mouseY;
}

int MouseManager::getMousePosX() {
	return m_iMouseX;
}

int MouseManager::getMousePosY() {
	return m_iMouseY;
}

MouseManager::~MouseManager() {

}