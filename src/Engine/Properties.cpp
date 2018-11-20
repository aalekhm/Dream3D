#include "Engine/Properties.h"
#include "Engine/MaterialReader.h"
#include <string>

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

Properties* Properties::create(const char* url) {

	if(url != NULL && strlen(url) == 0) {
		return NULL;
	}

	MaterialReader* matReader = new MaterialReader();
	Properties* pProp = matReader->read(url);

	return pProp;
}

void Properties::rewind() {

	m_vNamespacesItr = m_vNamespaces.end();
	m_mPropertiesItr = m_mProperties.end();
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
	m_mProperties[sName] = sValue;
}

void Properties::print(int iLevel) {

	printf("%s%s ", returnTabs(iLevel), m_sNamespace.c_str());
	if(m_sID.length() > 0)
		printf("%s ", m_sID.c_str());
	if(m_sParentID.length() > 0)
		printf(": %s", m_sParentID.c_str());

	printf("\n%s{\n", returnTabs(iLevel));
	std::map<std::string, std::string>::const_iterator itr = m_mProperties.begin();
	for(; itr != m_mProperties.end(); itr++) {
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

const char* Properties::getNextProperty(char** value) {

	if(m_mPropertiesItr == m_mProperties.end() ) {
		m_mPropertiesItr = m_mProperties.begin();
	}
	else {
		++m_mPropertiesItr;
	}

	if(m_mPropertiesItr != m_mProperties.end()) {

		const std::string& name = m_mPropertiesItr->first;
		if(!name.empty()) {

			if(value) {
				strcpy(*value, m_mPropertiesItr->second.c_str());
			}

			return name.c_str();
		}
	}

	return NULL;
}

Properties* Properties::getNextNamespace() {

	if(m_vNamespacesItr == m_vNamespaces.end() ) {
		m_vNamespacesItr = m_vNamespaces.begin();
	}
	else {
		++m_vNamespacesItr;
	}

	if(m_vNamespacesItr != m_vNamespaces.end()) {
		Properties* pNS = *m_vNamespacesItr;
		return pNS;
	}

	return NULL;
}

Properties* Properties::getNamespace(const char* id, bool bSearchNames) const {
	
	if(id == NULL)
		return NULL;

	Properties* ret = NULL;
	std::vector<Properties*>::const_iterator itr;

	for(itr = m_vNamespaces.begin(); itr < m_vNamespaces.end(); itr++) {

		ret = *itr;
		if( strcmp( (bSearchNames ? ret->getNamespace(): ret->getID()), id) == 0) {
			return ret;
		}

		ret = ret->getNamespace(id, bSearchNames);
		if(ret != NULL) {
			return ret;
		}
	}

	return ret;
}

const char* Properties::getNamespace() {
	return m_sNamespace.c_str();
}

const char* Properties::getID() {
	return m_sID.c_str();
}

const char* Properties::getString(const char* name) const {

	if(name) {

		std::map<std::string, std::string>::const_iterator itr = m_mProperties.find(name);
		if(itr != m_mProperties.end()) {
			return itr->second.c_str();
		}
	}
	else {
		if(m_mPropertiesItr != m_mProperties.end()) {
			return m_mPropertiesItr->second.c_str();
		}
	}

	return NULL;
}

bool Properties::exists(const char* name) const {
	if(name != NULL)
		return m_mProperties.find(name) != m_mProperties.end();

	return false;
}

void Properties::printProperties(Properties* properties) {

	// Print the name and ID of the current namespace.
	const char* spacename = properties->getNamespace();
	const char* id = properties->getID();
	//GP_WARN("Namespace: %s  ID: %s\n{", spacename, id);
	printf("%s %s {\n", spacename, id);

	// Print all properties in this namespace.
	const char* name = properties->getNextProperty();
	const char* _value = NULL;
	while (name != NULL)
	{
		_value = properties->getString(name);
		//GP_WARN("%s = %s", name, value);
		printf("\t %s = %s\n", name, _value);

		name = properties->getNextProperty();
	}
	//GP_WARN("}\n");
	printf("}");

	// Print the properties of every namespace within this one.
	Properties* space = properties->getNextNamespace();
	while (space != NULL)
	{
		printProperties(space);
		space = properties->getNextNamespace();
	}
}
