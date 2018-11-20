#include "Engine/Base.h"
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

Properties* Properties::getNamespace(const char* id, bool bSearchNames /*= false*/) {
	
	if(id == NULL)
		return NULL;

	rewind();
	Properties* pRetProperties = NULL;
	Properties* pProperties = getNextNamespace();
	
	while (pProperties != NULL)
	{
		if (strcmp((bSearchNames ? pProperties->getNamespaceType() : pProperties->getID()), id) == 0) {
			pRetProperties = pProperties;
			break;
		}

		pProperties = getNextNamespace();
	}

	return pRetProperties;
}

const char* Properties::getNamespaceType() {
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

static bool isStringNumeric(const char* str) {

	GP_ASSERT(str);

	char ch = '\0';

	// The first character may be '-'
	if (*str == '-') {
		str++;
	}

	// The first character after the sign must be a digit
	if (!isdigit(*str)) {
		return false;
	}

	str++;

	// All remaining characters must be digits, with a single decimal (.) permitted
	unsigned int decimalPos = 0;
	while (*str) {

		if (!isdigit(*str)) {
			if (*str == '.' && decimalPos == 0) {
				// Max of 1 decimal allowed
				decimalPos++;
			}
			else {
				return false;
			}
		}

		str++;
	}

	return true;
}

Properties::Type Properties::getType(const char* sName) const {

	const char* sValue = getString(sName);
	if (!sValue) {
		return Properties::NONE;
	}

	// Parse the value to determine the format
	unsigned int iCommaCount = 0;
	char* sValuePtr = (char*)sValue;
	while( sValuePtr = strchr(sValuePtr, ',') ) {
		iCommaCount++;
		sValuePtr++;
	}

	switch (iCommaCount) {

		case 0:
			return isStringNumeric(sValue) ? Properties::NUMBER : Properties::STRING;
		case 1:
			return Properties::VECTOR2;
		case 2:
			return Properties::VECTOR3;
		case 3:
			return Properties::VECTOR4;
		case 15:
			return Properties::MATRIX;
		default:
			return Properties::STRING;
	}
}

bool Properties::exists(const char* name) const {
	if(name != NULL)
		return m_mProperties.find(name) != m_mProperties.end();

	return false;
}

bool Properties::parseVector2(const char* pStr, Vector2* pOutVector2) {

	if (pStr) {

		float x, y;
		if (sscanf(pStr, "%f, %f", &x, &y)) {

			if (pOutVector2) {
				pOutVector2->set(x, y);
			}

			return true;
		}
		else
		{
			GP_ERROR("Error attempting to parse property as a two-dimensional vector: %s", pStr);
		}
	}

	if (pOutVector2)
		pOutVector2->set(0.0f, 0.0f);

	return false;
}

bool Properties::parseVector3(const char* pStr, Vector3* pOutVector3) {

	if (pStr) {

		float x, y, z;
		if (sscanf(pStr, "%f, %f, %f", &x, &y, &z)) {

			if (pOutVector3) {
				pOutVector3->set(x, y, z);
			}

			return true;
		}
		else
		{
			GP_ERROR("Error attempting to parse property as a three-dimensional vector: %s", pStr);
		}
	}

	if (pOutVector3)
		pOutVector3->set(0.0f, 0.0f, 0.0f);

	return false;
}

bool Properties::parseVector4(const char* pStr, Vector4* pOutVector4) {

	if (pStr) {

		float x, y, z, w;
		if (sscanf(pStr, "%f, %f, %f, %f", &x, &y, &z, &w)) {

			if (pOutVector4) {
				pOutVector4->set(x, y, z, w);
			}

			return true;
		}
		else
		{
			GP_ERROR("Error attempting to parse property as a three-dimensional vector: %s", pStr);
		}
	}

	if (pOutVector4)
		pOutVector4->set(0.0f, 0.0f, 0.0f, 0.0f);

	return false;
}

bool Properties::parseMatrix(const char* pStr, Matrix4* pOutMatrix4) {

	if (pStr) {

		float m[16];
		int scanned;
		scanned = sscanf(	pStr, 
							"%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f",
							&m[0], &m[1], &m[2], &m[3], &m[4], &m[5], &m[6], &m[7],
							&m[8], &m[9], &m[10], &m[11], &m[12], &m[13], &m[14], &m[15]
						);
		if (scanned == EOF) {
			GP_ERROR("Error attempting to parse property '%s' as a matrix.", name);
			pOutMatrix4->setIdentity();
			return false;
		}

		pOutMatrix4->set(m);
		return true;
	}

	pOutMatrix4->setIdentity();
	return false;
}

float Properties::getFloat(const char* name) const {

	const char* sValueString = getString(name);
	if (sValueString) {

		float fValue;
		int scanned;
		scanned = sscanf(sValueString, "%f", &fValue);
		if (scanned == EOF) {

			GP_ERROR("Error attempting to parse property '%s' as a float.", name);
			return 0.0f;
		}

		return fValue;
	}

	return 0.0f;
}

bool Properties::getVector2(const char* name, Vector2* out) const {
	return parseVector2(getString(name), out);
}

bool Properties::getVector3(const char* name, Vector3* out) const {
	return parseVector3(getString(name), out);
}

bool Properties::getVector4(const char* name, Vector4* out) const {
	return parseVector4(getString(name), out);
}

bool Properties::getMatrix(const char* name, Matrix4* out) const {
	return parseMatrix(getString(name), out);
}

//bool Properties::getColor(const char* name, Vector3* out) const {
//
//}
//
//bool Properties::getColor(const char* name, Vector4* out) const {
//
//}
	
void Properties::printProperties(Properties* properties) {

	// Print the name and ID of the current namespace.
	const char* spacename = properties->getNamespaceType();
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
