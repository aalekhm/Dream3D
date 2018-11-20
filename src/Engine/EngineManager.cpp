#include "Engine/Image.h"
#include "Engine/Texture.h"
#include "Engine/EngineManager.h"

EngineManager::EngineManager() {
	m_pKeyboardManager = new KeyboardManager();
	m_pMouseManager = new MouseManager();
	m_pTimer = new Timer();

	Texture* tex = Texture::create("data/core.tga");
}

void EngineManager::initTimer() {
	m_pTimer->initTimer();
}

bool EngineManager::isKeyPressed(int iKeyID) {
	return m_pKeyboardManager->isKeyPressed(iKeyID);
}

void EngineManager::setKeyPressed(int iKeyID) {
	m_pKeyboardManager->setKeyPressed(iKeyID);
}

void EngineManager::setKeyReleased(int iKeyID) {
	m_pKeyboardManager->setKeyReleased(iKeyID);
}

void EngineManager::update() {
	m_pTimer->startFrame();

	render();

	m_pTimer->endFrame();
}

void EngineManager::render() {

}

void EngineManager::setLMouseStatus(bool bPressed, int mouseX, int mouseY) {
	m_pMouseManager->setLMouseStatus(bPressed, mouseX, mouseY);
}

void EngineManager::setRMouseStatus(bool bPressed, int mouseX, int mouseY) {
	m_pMouseManager->setRMouseStatus(bPressed, mouseX, mouseY);
}

void EngineManager::setMouseMove(WPARAM keyState, int mouseX, int mouseY) {
	m_pMouseManager->setMouseMove(keyState, mouseX, mouseY);
}

EngineManager::~EngineManager() {

}