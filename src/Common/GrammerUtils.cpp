#include "Common/GrammerUtils.h"

Token* GrammerUtils::token = NULL;
Token* GrammerUtils::prevToken = NULL;

RandomAccessFile* GrammerUtils::rafIn = NULL;
StringTokenizer* GrammerUtils::strTok = NULL;
int GrammerUtils::iLineNo = 0;
char GrammerUtils::CHAR_TOKEN;
bool GrammerUtils::bMandatory;

void GrammerUtils::init() {
	if(strTok == NULL) {
		strTok = new StringTokenizer("Default");

		strTok->m_SymbolChars = "!@#$%^&*()_-+={}[]|\;':<>,.?/~`\"";

		strTok->setTokenCallback(&onTokenCallback);
		strTok->skipWhiteSpaces(true);
		strTok->ignoreNonTerminals(true);
	}
}

bool GrammerUtils::read(const char* sFile) {
	if(rafIn != NULL) {
		rafIn->close();
		rafIn = NULL;
	}
	
	rafIn = new RandomAccessFile();
	bool bCanRead = rafIn->openForRead(sFile);

	if(bCanRead)
		return true;

	return false;
}

Token* GrammerUtils::getNextToken() {
	//if(token != NULL) {
	//	prevToken = token;
	//	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ prevToken = %s\n", prevToken->getValue());
	//}

	if(	token == NULL 
		|| 
		token->getKind() == _EOF 
		|| 
		token->getKind() == _EOL 
		|| 
		token->getKind() == COMMENT_SINGLE_LINE
		|| 
		token->getKind() == COMMENT_MULTI_LINE
		) {
			if(rafIn->isEOF() == 0) { 
				delete token;
				CCString sLine = (char*)rafIn->readLine();
				//printf("Sline = %s\n", sLine);
				iLineNo++;
				sLine.trim();
				strTok->setData(sLine.c_str());
			}
			else {
				return NULL;
			}
	}

	token = strTok->getNextToken();

	if(	token == NULL 
		|| 
		token->getKind() == _EOF 
		|| 
		token->getKind() == _EOL 
		|| 
		token->getKind() == COMMENT_SINGLE_LINE
		|| 
		token->getKind() == COMMENT_MULTI_LINE
	) {
		getNextToken();
	}
	//printf("token ==== %d %s\n", token->getKind(), token->getValue());
	return token;
}

bool GrammerUtils::isWhiteSpace() {
	
	if(	CHAR_TOKEN == ' '
		||
		CHAR_TOKEN == '\t'
		||
		CHAR_TOKEN == '\r'
		||
		CHAR_TOKEN == '\n'
	) 
		return true;

	return false;
}

bool GrammerUtils::isEqual(const char* str1, const char* str2) {
	//printf("%s %s******************** %s *** %s\n", str1, str2, StringUtil::toUpper((char*)str1), StringUtil::toUpper((char*)str2));
	if(strcmp(str1, str2) == 0)
		return true;

	return false;
}

bool GrammerUtils::isOfKindToken(const char* sToken) {
//printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %s\n", sToken);
	if(	isEqual(sToken , "UNKNOWN")
		||
		isEqual(sToken , "_WORD")
		||
		isEqual(sToken , "_INTEGER")
		||
		isEqual(sToken , "_FLOAT")
		||
		isEqual(sToken , "QUOTEDSTRING")
		||
		isEqual(sToken , "WHITESPACE")
		||
		isEqual(sToken , "SYMBOL")
		||
		isEqual(sToken , "CHARACTER")
		||
		isEqual(sToken , "_NON_TERMINAL")
		||
		isEqual(sToken , "_SPECIAL_SEPERATOR")
		||
		isEqual(sToken , "_KEYWORD")
		||
		isEqual(sToken , "_EOL")
		||
		isEqual(sToken , "_EOF")
		||
		isEqual(sToken , "LINE")
		||
		isEqual(sToken , "STATEMENT")
		||
		isEqual(sToken , "COMMENT_SINGLE_LINE")
		||
		isEqual(sToken , "COMMENT_MULTI_LINE")
		||
		isEqual(sToken , "ANY")
	) {
		return true;
	}

	return false;
}

bool GrammerUtils::match(const char* str, int TYPE) {
	bool bIsAMatch = false;
	
	if(	(	token->getKind() == _INTEGER && isEqual("_INTEGER", str) )
		||
		(	token->getKind() == _FLOAT && isEqual("_FLOAT", str) )
	) {
//printf("************\n");
		bIsAMatch = true;
	}
	
	if(token->getKind() == _SPECIAL_SEPERATOR) {
//printf("************00000\n");
		bIsAMatch = isEqual(token->getValue(), str);
	}
	else
	if(	token->getKind() == _WORD
		||
		token->getKind() == QUOTEDSTRING
	) {
//printf("************1111\n");
		if(!isOfKindToken(str)) {

			bIsAMatch = isEqual(token->getValue(), str);
//printf("&&&&&&&&&&&&&&&&&&&&&&&&&& %s &&& %s %d\n", token->getValue(), str, bIsAMatch);
		}
		else {
			bIsAMatch = true;
		}
	}

	if(bIsAMatch) {
		printf("%d %s == %s\n", token->getKind(), str, token->getValue());

		if(token != NULL) {
			prevToken = token;
			printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ prevToken = %s\n", prevToken->getValue());
		}

		getNextToken();
		printf("		Next token = %s\n", token->getValue());
	}
	else {
		if(TYPE == MANDATORY)
			printf("000000000 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX %d != %d OR %d ............ found - %s, wanted - %s\n", token->getKind(), _WORD, QUOTEDSTRING, token->getValue(), str);
	}

	return bIsAMatch;
}

bool GrammerUtils::match(char ch, int TYPE) {

	if(	(	token->getKind() == CHARACTER
			||
			token->getKind() == SYMBOL
		)
		&&
		ch == token->getValue()[0]
	) {
		printf("%d Found, %c ====== wanted, %c\n", token->getKind(), token->getValue()[0], ch);

		if(token != NULL) {
			prevToken = token;
			printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ prevToken = %s\n", prevToken->getValue());
		}

		getNextToken();
		printf("		Next token = %s\n", token->getValue());
		return true;
	}
	else {
		if(TYPE == MANDATORY)
			printf("1111111111 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX %d != %d OR %d OR %c != %c\n", token->getKind(), CHARACTER, SYMBOL, ch, token->getValue()[0]);

		return false;
	}
}

void GrammerUtils::onTokenCallback(int iKind, char* sValue, int iLine, int iColumn) {

}
