#include "Engine/Properties.h"

Properties::Properties()
	:	m_sNamespace(""),
		m_sID(""),
		m_sParentID(""),
		
		m_pParent(NULL)
{
}

Properties::Properties(const char* sNamespace, const char* sID, const char* sParentID ) 
:	m_sNamespace(sNamespace),
	m_sID(sID),
	m_sParentID(sParentID),
	
	m_pParent(NULL)
{

}

Properties::~Properties() {
	
	for(size_t i = 0; i < m_vNamespaces.size(); i++) {
		SAFE_DELETE( m_vNamespaces[i] );
	}
}

void Properties::addNamespace(Properties* prop) {

	if(prop != NULL) {
		m_vNamespaces.push_back(prop);
		prop->setParent(this);
	}
}

void Properties::setParent(Properties* parent) {

	m_pParent = parent;
}

Properties*	Properties::getParent() {
	
	return m_pParent;
}

void Properties::addProperty(const char* sName, const char* sValue) {
	printf("Adding property ==> %s = %s\n", sName, sValue);
	m_mProperies[sName] = sValue;
}

void Properties::print(int iLevel) {

	printf("%s%s ", returnTabs(iLevel), m_sNamespace.c_str());
	if(m_sID.length() > 0)
		printf("%s ", m_sID.c_str());
	if(m_sParentID.length() > 0)
		printf(": %s", m_sParentID.c_str());

	printf("\n%s{\n", returnTabs(iLevel));
	std::map<std::string, std::string, strless>::iterator itr = m_mProperies.begin();
	for(; itr != m_mProperies.end(); itr++) {
		printf("%s%s = %s\n", returnTabs(iLevel+1), (itr->first).c_str(), (itr->second).c_str());
	}

	unsigned int iMaxNamespaces = m_vNamespaces.size();
	for(int i = 0; i < iMaxNamespaces; i++) {
			Properties* pr = (Properties*)m_vNamespaces[i];
			if(pr != NULL)
				pr->print(iLevel + 1);
	}

	printf("%s}\n", returnTabs(iLevel));
}

const char* Properties::returnTabs(unsigned int iLevel) {
	CCString sTabs = "";
	for(int i = 0; i < iLevel+1; i++) {
		sTabs += "\t";
	}

	return sTabs.c_str();
}

const char*	Properties::getNamespace() {
	return m_sNamespace.c_str();
}

const char*	Properties::getID() {
	return m_sID.c_str();
}

bool Properties::exists(const char* name) const {
	if(name != NULL)
		return m_mProperies.find(name) != m_mProperies.end();

	return false;
}
