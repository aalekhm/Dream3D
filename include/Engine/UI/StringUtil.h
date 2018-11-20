
#ifndef _STRINGUTIL_H_
#define _STRINGUTIL_H_

#include <stdio.h>
#include <string.h>

#define EXPORT_DLL __declspec(dllexport)

namespace StringUtil {

	EXPORT_DLL const char*	strCat(const char*s1, const char*s2);

	EXPORT_DLL char*		trim(char* text);
	EXPORT_DLL char*		lTrim(char* text);
	EXPORT_DLL char*		rTrim(char* text);
	EXPORT_DLL bool			isWhiteSpace(char c);
	EXPORT_DLL const char*	skipWhiteSpaces(const char* str);
	EXPORT_DLL bool			startsWith(const char* str, const char* startStr);//, bool isCaseSensitive);
	EXPORT_DLL char*		stripChars(char* text, char* stripChars);
	EXPORT_DLL char* 		toLower(char* text);
	EXPORT_DLL char* 		toUpper(char* text);
	EXPORT_DLL bool			isUpperCase(char c);
	EXPORT_DLL bool			isLowerCase(char c);
	EXPORT_DLL bool			isAlphabet(char c);
	EXPORT_DLL bool			isAlphaNumeric(char c);

	EXPORT_DLL char* 	substr(char* inStr, int beginIndex);
	EXPORT_DLL char* 	substr(char* inStr, int beginIndex, int endIndex);
	EXPORT_DLL char* 	replace(char* inStr, char oldChar, char newChar);
	EXPORT_DLL char* 	replace(char* inStr, char* oldStr, char* newStr);
	EXPORT_DLL char		charAt(char* inStr, int index);
	EXPORT_DLL int		indexOf(char* inStr, char c);
	EXPORT_DLL int		indexOf(char* inStr, char c, int fromIndex);
	EXPORT_DLL int		indexOf(char* inStr, char* searchStr);
	EXPORT_DLL int		indexOf(char* inStr, char* searchStr, int fromIndex);
	EXPORT_DLL int		lastIndexOf(char* inStr, char ch);
	EXPORT_DLL int		lastIndexOf(char* inStr, char ch, int fromIndex);
	EXPORT_DLL int		lastIndexOf(char* inStr, char* searchStr);
}
#endif