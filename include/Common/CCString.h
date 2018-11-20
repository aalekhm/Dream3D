#ifndef DEF_CCSTRING_H
#define DEF_CCSTRING_H

#include <string.h>
#include <stdio.h>

#define SAFE_DELETE(p) { if(p) { delete p; p = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] p; p = NULL; } }

#define CS_MIN_CHAR_SIZE	255

extern "C" {
	struct _declspec(dllexport) CCString {

	public:
		CCString();
		CCString(const char*);
		~CCString();

		char*				c_str();
		int					length();
		int					size();
		void				operator+=(const char*);
		void				operator+=(CCString*);
		void				operator+=(CCString);
		void				operator+=(char);

		void				operator=(const char*);
		void				operator=(CCString*);
		void				operator=(CCString);
		char&				operator [] (int index) { return m_pCString[index]; } //non-const
		const char&			operator [] (int index) const { return m_pCString[index]; } //const

		const char*			toLower();
		static const char*	toLower(char* inStr);

		const char*			toUpper();
		static const char*	toUpper(char* inStr);

		static bool			isAlphabet(char c);
		static bool			isLowerCase(char c);
		static bool			isUpperCase(char c);

		int					charAt(int);
		static int			charAt(const char* inStr, int index);

		int					indexOf(char ch, int fromIndex = 0);
		static int			indexOf(const char* inStr, char ch, int fromIndex = 0);

		int					indexOf(const char* searchStr, int fromIndex = 0);
		static int			indexOf(const char* inStr, const char* searchStr, int fromIndex = 0);
		
		int					lastIndexOf(char ch);
		static int			lastIndexOf(const char* inStr, char ch);

		int					lastIndexOf(char ch, int fromPos);
		static int			lastIndexOf(const char* inStr, char ch, int fromIndex);

		int					lastIndexOf(const char* searchStr);
		int					lastIndexOf(const char* searchStr, int fromIndex);
		static int			lastIndexOf(const char* inStr, const char* searchStr, int fromIndex);

		static bool			isWhiteSpace(char ch);
		void				lTrim();
		static char*		lTrim(char* text);
		void				rTrim();
		static char*		rTrim(char* text);
		void				trim();
		static char*		trim(char* text);

		char*				substr_c_str(int iBeginIndex);
		static char*		substr_c_str(char* inStr, int iBeginIndex);
		char*				substr_c_str(int iBeginIndex, int iEndIndex);
		static char*		substr_c_str(char* inStr, int iBeginIndex, int iEndIndex);

		CCString*			substr(int iBeginIndex);
		static CCString*	substr(CCString* inStr, int iBeginIndex);
		CCString*			substr(int iBeginIndex, int iEndIndex);
		static CCString*	substr(CCString* inStr, int iBeginIndex, int iEndIndex);

		void				replace(char oldChar, char newChar);
		static char*		replace(char* inStr, char oldChar, char newChar);
		void				replace(char* oldStr, char* newStr);
		static char*		replace(char* inStr, char* oldStr, char* newStr);

		bool				startsWith(const char* startStr);
		static bool			startsWith(const char* str, const char* startStr);//, bool isCaseSensitive);
		bool				endsWith(const char* endStr);
		static bool			endsWith(const char* str, const char* endStr);
		
		void				stripChars(char* stripChars);
		static char*		stripChars(char* text, char* stripChars);
	private:
		char*				m_pCString;
	};
}
#endif