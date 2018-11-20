#include "Engine/MaterialReader.h"
#include "Engine/Properties.h"
#include "Common/GrammerUtils.h"

MaterialReader::MaterialReader()
:	m_pRootNamespace(NULL),
	m_pCurNamespace(NULL),
	m_sNamespace(""),
	m_sID(""),
	m_sParentID(""),
	m_sPropertyName(""),
	m_sPropertyValue("")
{
	GrammerUtils::init();
}

Properties* MaterialReader::read(const char* sFile) {

	//////////////// THIS PIECE OF CODE WILL REMAIN COMMON FOR ALL //////////////////
	if(GrammerUtils::read(sFile)) {
		addKeywords();
		GrammerUtils::getNextToken();
		def();
	}
	/////////////////////////////////////////////////////////////////////////////////

	return m_pRootNamespace;
}


bool MaterialReader::def() {
	while(true) {
		if(namespace_type()) {
			if(!namespace_body())
				return false;
		}

		else
			break;
	}

	return true;

}

bool MaterialReader::namespace_body() {
	resetVariables ();
	m_sNamespace =GrammerUtils ::prevToken ->getValue ();
	if(!namespace_name()) {
	}
	else {
		m_sID =GrammerUtils ::prevToken ->getValue ();
		if(!GrammerUtils::match(':', OPTIONAL)) {

		}

		else {

			if(!parent_namespace_name())
				return false;
			m_sParentID =GrammerUtils ::prevToken ->getValue ();
		}

	}

	if(!GrammerUtils::match('{', MANDATORY))
		return false;
	Properties *pNamespace =new Properties (m_sNamespace .c_str (),m_sID .c_str (),m_sParentID .c_str ());
	if (m_pRootNamespace ==NULL ){m_pCurNamespace =m_pRootNamespace =pNamespace ;
	}else {m_pCurNamespace ->addNamespace (pNamespace );
	m_pCurNamespace =pNamespace ;
	}while(true) {
		if(name()) {
			m_sPropertyName =GrammerUtils ::prevToken ->getValue ();
			if(!name_value_pair__or__namespace_body())
				return false;
		}

		else
			break;
	}

	if(!GrammerUtils::match('}', MANDATORY))
		return false;

	m_pCurNamespace->rewind();
	m_pCurNamespace =m_pCurNamespace ->getParent ();
	return true;

}

bool MaterialReader::namespace_type() {
	if(!GrammerUtils::match("_WORD", MANDATORY))
		return false;
	return true;

}

bool MaterialReader::namespace_name() {
	if(GrammerUtils::match("_WORD", OPTIONAL)) {
		return true;
	}
	else
		if(GrammerUtils::match("_INTEGER", OPTIONAL)) {
			return true;
		}
		else
			return false;

	return true;

}

bool MaterialReader::parent_namespace_name() {
	if(GrammerUtils::match("_WORD", OPTIONAL)) {
		return true;
	}
	else
		if(GrammerUtils::match("_INTEGER", OPTIONAL)) {
			return true;
		}
		else
			return false;

	return true;

}

bool MaterialReader::name_value_pair__or__namespace_body() {
	if(name_value_pair()) {
		return true;
	}
	else
		if(namespace_body()) {
			return true;
		}
		else
			return false;

	return true;

}

bool MaterialReader::name_value_pair() {
	if(!GrammerUtils::match('=', MANDATORY))
		return false;
	if(!value())
		return false;
	m_sPropertyValue =GrammerUtils ::prevToken ->getValue ();
	m_pCurNamespace ->addProperty (m_sPropertyName .c_str (),m_sPropertyValue .c_str ());
	m_sPropertyName ="";
	m_sPropertyValue ="";
	return true;

}

bool MaterialReader::name() {
	if(!GrammerUtils::match("_WORD", MANDATORY))
		return false;
	return true;

}

bool MaterialReader::value() {
	if(GrammerUtils::match("QUOTEDSTRING", OPTIONAL)) {
		return true;
	}
	else
		if(GrammerUtils::match("_INTEGER", OPTIONAL)) {
			return true;
		}
		else
			if(GrammerUtils::match("_WORD", OPTIONAL)) {
				return true;
			}
			else
				if(GrammerUtils::match("_FLOAT", OPTIONAL)) {
					return true;
				}
				else
					return false;

	return true;

}

