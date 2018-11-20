#include "Engine\MouseManager.h"

MouseManager::MouseManager() {

}

void MouseManager::setLMouseStatus(bool bPressed, int mouseX, int mouseY) {
	m_bLMouseDown = bPressed;

	m_iMouseX = mouseX;
	m_iMouseY = mouseY;
}

void MouseManager::setRMouseStatus(bool bPressed, int mouseX, int mouseY) {
	m_bRMouseDown = bPressed;

	m_iMouseX = mouseX;
	m_iMouseY = mouseY;
}

void MouseManager::setMouseMove(unsigned int keyState, int mouseX, int mouseY) {
	m_iMouseX = mouseX;
	m_iMouseY = mouseY;
}

MouseManager::~MouseManager() {

}