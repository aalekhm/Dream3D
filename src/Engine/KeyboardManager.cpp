#include "Engine\KeyboardManager.h"

KeyboardManager::KeyboardManager() {
	for(int i = 0; i < DEF_MAX_KEYS; i++)
		m_bKeys[i] = false;
}

bool KeyboardManager::isKeyPressed(int iKeyID) {
	if(iKeyID < DEF_MAX_KEYS) {
		return m_bKeys[iKeyID];
	}

	return false;
}

void KeyboardManager::setKeyPressed(int iKeyID) {
	if(iKeyID < DEF_MAX_KEYS) {
		m_bKeys[iKeyID] = true;
	}
}

void KeyboardManager::setKeyReleased(int iKeyID) {
	if(iKeyID < DEF_MAX_KEYS) {
		m_bKeys[iKeyID] = false;
	}
}

KeyboardManager::~KeyboardManager() {

}