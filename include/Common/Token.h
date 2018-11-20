
#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stdio.h>
#include <string.h>
#include <vector>

extern "C" {
	#define END_OF_FILE 0
#define SAFE_DELETE(p) { if(p) { delete p; p = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] p; p = NULL; } }

	enum TokenKind {
		UNKNOWN,
		_WORD,
		_INTEGER,
		_FLOAT,
		QUOTEDSTRING,
		WHITESPACE,
		SYMBOL,
		CHARACTER,
		_NON_TERMINAL,
		_SPECIAL_SEPERATOR,
		_KEYWORD,
		_EOL,
		_EOF,
		LINE,
		STATEMENT,
		COMMENT_SINGLE_LINE,
		COMMENT_MULTI_LINE,
		ANY
	};

	class _declspec(dllexport) Token {

		private:
			int			m_iLine;
			int			m_iColumn;
			char*		m_Value;
			TokenKind	m_Kind;
		public:
			Token(TokenKind kind, char* value, int line, int column);
			Token();
			~Token();
			int getLine();
			int getColumn();
			const char* getValue();
			TokenKind getKind();
	};
}
#endif