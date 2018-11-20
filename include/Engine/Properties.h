#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <map>
#include <vector>
#include "Common/CCString.h"
#include "Common/Matrices.h"

#define SAFE_DELETE(x) if(x) { delete x; x = NULL; }

//std::map expects a functor to compare the keys.
class strless {
public:
	bool operator() (const std::string & first, const std::string & second ) const  {
		return strcmp(first.c_str(), second.c_str()) < 0;
	}
};

class Properties {

	public:
		enum Type
		{
			NONE,
			STRING,
			NUMBER,
			VECTOR2,
			VECTOR3,
			VECTOR4,
			MATRIX
		};

		static Properties*									create(const char* url);
		Properties (const char* sNamespace, const char* sID, const char* sParentID );
		void 												addNamespace(Properties* prop);
		
		void												addProperty(const char* sName, const char* sValue);
		void												setParent(Properties* prop);
		Properties*											getParent();

		void												print(int iLevel);

		Properties*											getNextNamespace();
		Properties*											getNamespace(const char* id, bool bSearchNames = false);
		const char*											getNamespaceName();
		const char*											getNamespaceType();
		const char*											getID();
		const char*											getString(const char* name) const;

		bool												exists(const char* name) const;
		static void											printProperties(Properties* properties);
		void												rewind();

		const char*											getNextProperty(char** value = NULL);
		Properties::Type									getType(const char* sName) const;

		float												getFloat(const char* name = NULL) const;
		bool												getVector2(const char* name, Vector2* out) const;
		bool												getVector3(const char* name, Vector3* out) const;
		bool												getVector4(const char* name, Vector4* out) const;
		bool												getMatrix(const char* name, Matrix4* out) const;
		//bool												getColor(const char* name, Vector3* out) const;
		//bool												getColor(const char* name, Vector4* out) const;

															~Properties();
	private:
															Properties();

		static bool											parseVector2(const char* pStr, Vector2* pOutVector2);
		static bool											parseVector3(const char* pStr, Vector3* pOutVector3);
		static bool											parseVector4(const char* pStr, Vector4* pOutVector4);
		static bool											parseMatrix(const char* pStr, Matrix4* pOutMatrix4);

		const char*											returnTabs(unsigned int iLevel);
		


		CCString											m_sNamespace;
		CCString											m_sID;
		CCString											m_sParentID;
		
		Properties*											m_pParent;

		std::map<std::string, std::string>					m_mProperties;
		std::map<std::string, std::string>::const_iterator	m_mPropertiesItr;

		std::vector<Properties*>							m_vNamespaces;
		std::vector<Properties*>::const_iterator			m_vNamespacesItr;
};

#endif