#include "Engine/UI/WComponentFactory.h"

WComponentFactory::WComponentFactory() {

}

void WComponentFactory::Register(const std::string& lpClassName, CreateFn pfnCreate) {

	std::map<std::string, CreateFn>::iterator itr = m_WComponentMap.find(lpClassName);
	if( itr == m_WComponentMap.end() )
		m_WComponentMap[lpClassName] = pfnCreate;
}
