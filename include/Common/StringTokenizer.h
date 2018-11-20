
#ifndef _STRINGTOKENIZER_H_
#define _STRINGTOKENIZER_H_

#include "Common/Token.h"
#include "Common/CCString.h"

#define SAFE_DELETE(p) { if(p) { delete p; p = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] p; p = NULL; } }

using namespace std;

extern "C" {
	class _declspec(dllexport) StringTokenizer {
		
		private:
			int		m_iPos,		m_SavePos;
			int		m_iLine,	m_SaveLine;
			int		m_iColumn,	m_SaveColumn;
			char*	m_Data;
			int		m_iDataLength;
			bool	bUseExplicitSymbols;
			bool	bSkipWhiteSpaces;
			bool	IGNORE_NONTERMINAL;
		public:
			char*			m_SymbolChars;
			vector<char*>	vSeperators;

		StringTokenizer(const char* data);
		StringTokenizer(const char* sFileNameWithPath, bool isFile);
		~StringTokenizer();
		
		char peek(int count);
		Token* getNextToken();
		Token* createToken(TokenKind tokKind, char* value);
		Token* createToken(TokenKind tokKind);
		char consume(int iHowMany);
		void initRead();
		void setData(const char* newData);
		Token* readWhiteSpace();//SPACE, \t, \r, \n
		Token* readNumber();//_INTEGER = 1234, _FLOAT = 12.556452
		Token* readString();//"A String"
		Token* readCharacter();//'c'
		Token* readNonTerminal();//<name>
		Token* readWord();//WORD
		Token* readSingleLineComment();
		Token* readMultiLineComment();
		Token* checkDefault(char ch);
		bool isSymbol(char c);
		bool isWhiteSpaces(char ch);
		bool isSpecialSeperator(char c);
		void setUseSymbolsOnlyAsTokens(bool bValue);
		void skipWhiteSpaces(bool bSkip);
		void clear();
		
		void setTokenCallback(void (__cdecl *callbackFuncAddr)(int,char *,int,int));
		void (*onTokenCallback)(int iKind, char* sValue, int iLine, int iColumn);

			void ignoreNonTerminals(bool bIgnore);
		//readWord
		//readLine
		//readNumber

	};
}
#endif