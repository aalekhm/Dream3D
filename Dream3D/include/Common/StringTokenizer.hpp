
#pragma once

#include "Common/RandomAccessFile.hpp"
#include "Common/Token.hpp"
#include "Engine/Base.h"
#include <windows.h>

using namespace std;

extern "C" 
{
	class _declspec(dllexport) StringTokenizer 
	{
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

			void (*onTokenCallback)(int iKind, const char* sValue, int iLine, int iColumn);

			StringTokenizer(const char* data) 
			{
				this->clear();
	
				m_iDataLength = strlen(data);
				m_Data = new char[m_iDataLength+1];
				memset(m_Data, 0, m_iDataLength+1);
				strncpy(m_Data, data, m_iDataLength+1);
				m_Data[m_iDataLength] = '\0';	
			}

			StringTokenizer(const char* sFileNameWithPath, bool isFile) 
			{
				this->clear();

				RandomAccessFile* rafIn = new RandomAccessFile();
				bool goAhead = rafIn->openForRW(sFileNameWithPath);

				if(goAhead) 
				{
					m_iDataLength = rafIn->getFileLength();
					m_Data = new char[m_iDataLength+1];
					::memset(m_Data, 1, m_iDataLength);
					m_Data[m_iDataLength] = '\0';

					int bytesRead = rafIn->read(m_Data, 0, m_iDataLength);
					m_Data[bytesRead] = '\0';
					rafIn->close();
				}
			}

			char peek(int count) 
			{
				if(m_iPos + count >= m_iDataLength)
					return END_OF_FILE;
				else
					return m_Data[m_iPos + count];
			}

			Token* getNextToken() 
			{
				initRead();
				while(true) 
				{
					char ch = peek(0);
					if(bUseExplicitSymbols) 
					{
						if(ch == END_OF_FILE) 
						{
							return createToken(_EOF, "");
						}
						else
						if(isSymbol(ch)) 
						{
							createToken(SYMBOL);
							consume(1);
				
							initRead();
						}
						else
							consume(1);
					}
					else 
					{
						if(isSpecialSeperator(ch))
							return createToken(_SPECIAL_SEPERATOR);
						else 
						{
							switch(ch) 
							{
								case END_OF_FILE:
									return createToken(_EOF, "");
								break;
								case ' ':
								case '\t':
									if(!bSkipWhiteSpaces)
										return readWhiteSpace();
									else
										consume(1);
								break;
								case '0':
								case '1':
								case '2':
								case '3':
								case '4':
								case '5':
								case '6':
								case '7':
								case '8':
								case '9':
									return readNumber();
								break;
								case '\r':
									initRead();
									consume(1);//Skip

									if(peek(0) == '\n')
										consume(1);

									m_iLine++;
									m_iColumn = 1;

									return createToken(_EOL);
								break;
								case '\n':
									initRead();
									m_iLine++;
									m_iColumn = 1;

									return createToken(_EOL);
								break;
								case '"':
									return readString();
								break;
								case '\'':
									return readCharacter();
								break;
								/*
								case ':':
									if(peek(1) == ':' && peek(2) == '=') {
										initRead();

										consume(1);
										consume(1);
										consume(1);
										return createToken(_GRAMMER_ASSIGNMENT);
									}
									else
										return checkDefault(ch);
								break;
								*/
								default:
									//if(!isSpecialSeperator(ch)) {
										return checkDefault(ch);
									//}
									//else {
										//printf("asdasdasdasdasdasdasd _SPECIAL_SEPERATOR\n");
									//	return createToken(_SPECIAL_SEPERATOR);
									//}
								break;
							}
						}
					}
				}
			}

			char consume(int iHowMany) 
			{
				char ret;
				m_iPos += iHowMany;	
				m_iColumn += iHowMany;

				ret = m_Data[m_iPos];

				return ret;
			}

			Token* createToken(TokenKind tokKind, char* value) 
			{
				return new Token(tokKind, value, m_iLine, m_iColumn);
			}

			Token* createToken(TokenKind tokKind) 
			{
				std::string sData = m_Data;
				std::string sTokData = sData.substr(m_SavePos, m_iPos - m_SavePos);

				onTokenCallback(tokKind, sTokData.c_str(), m_SaveLine, m_SaveColumn);
 				return new Token(tokKind, sTokData.c_str(), m_iLine, m_iColumn);
			}

			void setUseSymbolsOnlyAsTokens(bool bValue) 
			{
				bUseExplicitSymbols = bValue;
			}

			void setData(const char* newData) 
			{
				if(m_Data) 
				{
					m_iPos = m_SavePos = m_iDataLength = 0;
					m_iLine = m_SaveLine = m_iColumn = m_SaveColumn = 1;

					SAFE_DELETE_ARRAY(m_Data);
		
					m_iDataLength = strlen(newData);
					m_Data = new char[m_iDataLength+1];
					::memset(m_Data, 1, m_iDataLength+1);
					strncpy(m_Data, newData, m_iDataLength+1);
					m_Data[m_iDataLength] = '\0';
				}
			}

			void clear() 
			{
				m_iPos = m_SavePos = m_iDataLength = 0;
				m_iLine = m_SaveLine = m_iColumn = m_SaveColumn = 1;

				m_SymbolChars = "=+-/,.*~!@#$%^&(){}[]:;<>?|\\";

				bUseExplicitSymbols = false;
				IGNORE_NONTERMINAL = false;
			}

			Token* readNumber() 
			{
				initRead();
				bool bHasDot = false;

				consume(1);//Read the 1st Digit
				while(true) 
				{
					char ch = peek(0);
					if(::isdigit(ch))
						consume(1);
					else 
					if(ch == '.' && !bHasDot) 
					{
						bHasDot = true;
						consume(1);
					}
					else
						break;
				}
	
				return createToken(bHasDot?_FLOAT:_INTEGER);
			}

			Token* readWhiteSpace()
			{
				initRead();
				consume(1);

				while(true) 
				{
					char ch = peek(0);

					if(isWhiteSpaces(ch))
						consume(1);
					else
						break;
				}

				return createToken(WHITESPACE);
			}

			Token* readString() 
			{
				consume(1);//Read the 1st Digit
				initRead();

				while(true) 
				{
					char ch = peek(0);

					if(ch == '\0')
						break;
					else
					if(ch == '\r') 
					{
						consume(1);
						if(peek(0) == '\n')
							consume(1);

						m_iLine++;
						m_iColumn = 1;
					}
					else
					if(ch == '\n') 
					{
						consume(1);
						m_iLine++;
						m_iColumn = 1;
					}
					else
					if(ch == '"') 
					{
						break;
					}
					else
						consume(1);
				}
	
				Token* tokQuotedString = createToken(QUOTEDSTRING);
				consume(1);
	
				return tokQuotedString;
			}

			Token* readCharacter() 
			{
				consume(1);//Read the 1st Digit
				initRead();

				char ch = peek(0);
				consume(1);
	
				Token* tokCharacter = createToken(CHARACTER);

				ch = peek(0);
				if(ch != '\'') 
				{
					printf("Error in char\n");
				}
	
				consume(1);
	
				return tokCharacter;
			}

			Token* readNonTerminal() 
			{
				consume(1);//Read the 1st Digit
				initRead();
	
				while(true) 
				{
					char ch = peek(0);

					if(ch != '>')
						consume(1);
					else
						break;
				}

				Token* tokNonTerminal = createToken(_NON_TERMINAL);
				consume(1);
	
				return tokNonTerminal;
			}

			Token* readWord() 
			{
				initRead();
				consume(1);

				while(true) 
				{
					char ch = peek(0);

					if(isAlphabet(ch) || ch == '_' || isdigit(ch))
						consume(1);
					else
						break;
				}
	
				return createToken(_WORD);
			}

			Token* readSingleLineComment() 
			{
				initRead();
				consume(1);
	
				while(true) 
				{
					char ch = peek(0);
					if(ch == '\r' || ch == '\n' || ch == 0)
						break;
					else
						consume(1);
				}

				return createToken(COMMENT_SINGLE_LINE);
			}

			Token* readMultiLineComment() 
			{
				initRead();
				consume(1);
	
				while(true) 
				{
					char ch = peek(0);
					if(ch == '*' && peek(1) == '/') 
					{
						consume(2);//consume '/'
						break;
					}
					else
						consume(1);
				}

				return createToken(COMMENT_MULTI_LINE);
			}

			Token* checkDefault(char ch) 
			{
				if(isAlphabet(ch) || ch == '_')
					return readWord();
				else
				if(ch == '/' && peek(1) == '/') 
				{	//Single line comment			
					return readSingleLineComment();
				}
				else
				if(ch == '/' && peek(1) == '*') 
				{	//Multi line comment			
					return readMultiLineComment();
				}
				else
				if(ch == '-' && ::isdigit(peek(1))) 
				{	//Negative number
					return readNumber();
				}
				else
				if(ch == '<' && !IGNORE_NONTERMINAL) 
				{
					return readNonTerminal();
				}
				else
				if(isSymbol(ch)) 
				{
					initRead();
					consume(1);

					return createToken(SYMBOL);
				}
				else 
				{
					initRead();
					consume(1);
					return createToken(UNKNOWN);
				}
			}

			bool isSymbol(char c) 
			{
				for(int i = 0; i < strlen(m_SymbolChars); i++) 
				{
					if(c == m_SymbolChars[i])
						return true;
				}

				return false;
			}

			bool isSpecialSeperator(char ch) 
			{
				bool bReturn = false;
				int iSize = vSeperators.size();

				for(int i = 0; i < iSize; i++) 
				{
					char* sSeperator = vSeperators[i];
					int iLenCount = strlen(sSeperator);
					int iLength = iLenCount;

					char c;
					int iPeekCount = 0;
					while(iLenCount > 0) 
					{
						c = peek(iPeekCount);
						if(c != sSeperator[iPeekCount])
							break;

						iLenCount--;
						iPeekCount++;
					}
		
					c = peek(iPeekCount);//next char

					if(	iLenCount > 0
						||
						(isAlphabet(c) || ::isdigit(c) || c == '_' || c == '-')
					)
						continue;
					else 
					{
						bReturn = true;
						initRead();
						consume(iLength);
						break;
					}
				}

				return bReturn;
			}

			void setTokenCallback(void (__cdecl *callbackFuncAddr)(int, const char *, int, int)) 
			{
				onTokenCallback = callbackFuncAddr;
			}

			void initRead() 
			{
				m_SavePos = m_iPos;
				m_SaveLine = m_iLine;
				m_SaveColumn = m_iColumn;
			}

			~StringTokenizer() 
			{
				this->clear();
				SAFE_DELETE_ARRAY(m_SymbolChars);
				SAFE_DELETE_ARRAY(m_Data);
			}

			void skipWhiteSpaces(bool bSkip) 
			{
				bSkipWhiteSpaces = bSkip;
			}

			bool isWhiteSpaces(char ch) 
			{
				return (ch == '\t' || ch == ' ');
			}

			void ignoreNonTerminals(bool bIgnore) 
			{
				IGNORE_NONTERMINAL = bIgnore;
			}
	};
}