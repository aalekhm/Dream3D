#ifndef KEYBOARD_MANAGER_H
#define KEYBOARD_MANAGER_H

#define DEF_MAX_KEYS	256

class KeyboardManager {

	public:
		KeyboardManager();
		~KeyboardManager();

		static bool isKeyPressed(int iKeyID);
		void setKeyPressed(int iKeyID);
		void setKeyReleased(int iKeyID);
	private:
		static bool		m_bKeys[DEF_MAX_KEYS];	// Array Used For The Keyboard Routine
};

#endif