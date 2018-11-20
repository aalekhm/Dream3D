#ifndef _GrammerUtils_H_
#define _GrammerUtils_H_

#include "Common/RandomAccessFile.h"
#include "Common/StringTokenizer.h"
#include "Common/Token.h"

#define OPTIONAL	0
#define MANDATORY	1

class GrammerUtils
{
	public:
		static Token*	token;
		static Token*	getNextToken();
		static Token*	prevToken;

		static char		getNextTokenMy();
		static void		skipWhiteSpaces();

		static bool		match(const char* str, int TYPE);
		static bool		match(char ch, int TYPE);

		static bool		isWhiteSpace();
		static bool		isEqual(const char* str1, const char* str2);
		static bool		isOfKindToken(const char* sToken);

		static void		init();
		static bool		read(const char* sFile);
		static void		onTokenCallback(int iKind, char* sValue, int iLine, int iColumn);

		static StringTokenizer*		strTok;
		static int					iLineNo;
	private:
		static char					CHAR_TOKEN;
		static bool					bMandatory;

		static RandomAccessFile*	rafIn;
};
#endif