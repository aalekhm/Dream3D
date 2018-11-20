
#include "Engine/Base.h"
#include "Engine/MaterialParameter.h"

MaterialParameter::MaterialParameter(const char* sName) 
	:	m_sName(sName ? sName : ""),
		m_Type(MaterialParameter::NONE),
		m_iCount(1)
{

}

MaterialParameter::~MaterialParameter() {

}

const char* MaterialParameter::getName() {
	return m_sName.c_str();
}

Texture::Sampler* MaterialParameter::getSampler() const {
	if(m_Type == MaterialParameter::SAMPLER)
		return const_cast<Texture::Sampler*>(m_Value.samplerPtrValue);

	return NULL;
}

Texture::Sampler* MaterialParameter::setValue(const char* sTexturePath, bool bGenerateMipmaps) {
	
	if(sTexturePath) {
		Texture::Sampler* pSampler = Texture::Sampler::create(sTexturePath, bGenerateMipmaps);
		if(pSampler) {
			m_Value.samplerPtrValue = pSampler;
			m_Type = MaterialParameter::SAMPLER;
		}

		return pSampler;
	}

	return NULL;
}