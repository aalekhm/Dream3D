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
		Properties (const char* sNamespace, const char* sID, const char* sParentID );
		void 			addNamespace(Properties* prop);
		
		void			addProperty(const char* sName, const char* sValue);
		void			setParent(Properties* prop);
		Properties*		getParent();

		void			print(int iLevel);

		const char*		getNamespace();
		const char*		getID();
		bool			exists(const char* name) const;

		~Properties();
	private:
		Properties();
		const char*		returnTabs(unsigned int iLevel);

		CCString								m_sNamespace;
		CCString								m_sID;
		CCString								m_sParentID;
		
		Properties*								m_pParent;

		std::map<std::string, std::string, strless>		m_mProperies;
		std::vector<Properties*>						m_vNamespaces;
};

#endif