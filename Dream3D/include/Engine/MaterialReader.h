#ifndef _MaterialReader_h_
#define _MaterialReader_h_

#include "Common/GrammerUtils.h"

class Properties;

class MaterialReader {
	public:
		MaterialReader();

		void addKeywords() {
			GrammerUtils::strTok->vSeperators.push_back("<code>");
			GrammerUtils::strTok->vSeperators.push_back("</code>");
		}

		bool def();
		bool namespace_body();
		bool namespace_name();
		bool parent_namespace_name();
		bool name();
		bool name_value_pair__or__namespace_body();
		bool namespace_type();
		bool name_value_pair();
		bool value();

		Properties* read(const char* sFile);
		
		void resetVariables() {
			m_sNamespace = "";
			m_sID = "";
			m_sParentID = "";
		}

		Properties*	m_pRootNamespace;
		Properties*	m_pCurNamespace;

		std::string m_sNamespace;
		std::string m_sID;
		std::string m_sParentID;

		std::string m_sPropertyName;
		std::string m_sPropertyValue;
};
#endif
