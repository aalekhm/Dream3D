#include "Engine/Image.h"
#include "Engine/Texture.h"
#include "Engine/EngineManager.h"
#include "Engine/Camera.h"

EngineManager*	EngineManager::m_pEngineManager;

EngineManager::EngineManager() 
	:	m_pTimer(NULL),
		m_pKeyboardManager(NULL),
		m_pMouseManager(NULL),
		
		m_iViewportW(0),
		m_iViewportH(0),
		m_iState(UNINITIALIZED),
	 	m_bInitialized(false),

		m_iFrameCount(0),
		m_iFrameRate(0),
		m_dLastElapsedFPSTimeMs(0.0f)
{
	GP_ASSERT(m_pEngineManager == NULL);
	m_pEngineManager = this;
}

EngineManager* EngineManager::getInstance() {
	GP_ASSERT(m_pEngineManager);
	return m_pEngineManager;
}

void EngineManager::startup() {
	m_pKeyboardManager = new KeyboardManager();
	m_pMouseManager = new MouseManager();
	m_pTimer = new Timer();

	initTimer();

	m_iState = RUNNING;
}

void EngineManager::shutdown() {
	if(m_iState != UNINITIALIZED) {

		m_iState = UNINITIALIZED;
	}
}

void EngineManager::exit() {
	shutdown();
}

EngineManager::State EngineManager::getState() {
	return m_iState;
}

void EngineManager::setViewport(int w, int h) {
	m_iViewportW = w;
	m_iViewportH = h;
}

void EngineManager::initTimer() {
	m_pTimer->initTimer();
}

Timer*	EngineManager::getTimer() {
	return m_pTimer;
}

bool EngineManager::isKeyPressed(int iKeyID) {
	return KeyboardManager::isKeyPressed(iKeyID);
}

void EngineManager::setKeyPressed(int iKeyID) {
	m_pKeyboardManager->setKeyPressed(iKeyID);
}

void EngineManager::setKeyReleased(int iKeyID) {
	m_pKeyboardManager->setKeyReleased(iKeyID);
}

void EngineManager::frame() {
	if(!m_bInitialized) {
		initialize();
		m_bInitialized = true;
	}

	if(m_iState == RUNNING) {
		m_pTimer->startFrame();

		update((float)m_pTimer->getDeltaTimeMs());
		render((float)m_pTimer->getDeltaTimeMs());

		updateFPS();

		m_pTimer->endFrame();
	}
}

void EngineManager::setLMouseStatus(bool bPressed, int mouseX, int mouseY) {
	m_pMouseManager->setLMouseStatus(bPressed, mouseX, mouseY);
}

void EngineManager::setMMouseStatus(bool bPressed, int mouseX, int mouseY) {
	m_pMouseManager->setMMouseStatus(bPressed, mouseX, mouseY);
}

void EngineManager::setRMouseStatus(bool bPressed, int mouseX, int mouseY) {
	m_pMouseManager->setRMouseStatus(bPressed, mouseX, mouseY);
}

void EngineManager::setMouseMove(WPARAM keyState, int mouseX, int mouseY) {
	m_pMouseManager->setMouseMove(keyState, mouseX, mouseY);
}

MouseManager* EngineManager::getMouseManager() const {
	return m_pMouseManager;
}

unsigned int EngineManager::getWidth() {
	return m_iViewportW;
}

unsigned int EngineManager::getHeight() {
	return m_iViewportH;
}

void EngineManager::updateFPS() {
	++m_iFrameCount;
	if( (m_pTimer->getFrameElapsedTime() - m_dLastElapsedFPSTimeMs) >= 1000.0f) {
		m_iFrameRate = m_iFrameCount;
		m_iFrameCount = 0;
		m_dLastElapsedFPSTimeMs = m_pTimer->getElapsedTimeInMilliSec();
	}
}

unsigned int EngineManager::getFPS() {
	return m_iFrameRate;
}

EngineManager::~EngineManager() {

}