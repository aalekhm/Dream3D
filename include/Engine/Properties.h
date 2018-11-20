#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "Common/CCString.h"
#include <map>
#include <vector>

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
		static Properties* create(const char* url);
		Properties (const char* sNamespace, const char* sID, const char* sParentID );
		void 			addNamespace(Properties* prop);
		
		void			addProperty(const char* sName, const char* sValue);
		void			setParent(Properties* prop);
		Properties*		getParent();

		void			print(int iLevel);

		Properties*			getNextNamespace();
		Properties*			getNamespace(const char* id, bool bSearchNames = false) const;
		const char*		getNamespace();
		const char*		getNextProperty(char** value = NULL);
		const char*		getID();
		const char*		getString(const char* name) const;

		bool					exists(const char* name) const;
		static void			printProperties(Properties* properties);
		void					rewind();

		~Properties();
	private:
		Properties();
		const char*		returnTabs(unsigned int iLevel);

		CCString								m_sNamespace;
		CCString								m_sID;
		CCString								m_sParentID;
		
		Properties*								m_pParent;

		std::map<std::string, std::string>							m_mProperties;
		std::map<std::string, std::string>::const_iterator	m_mPropertiesItr;

		std::vector<Properties*>							m_vNamespaces;
		std::vector<Properties*>::const_iterator	m_vNamespacesItr;
};

#endif