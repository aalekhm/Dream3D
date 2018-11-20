#include "Engine/Pass.h"
#include "Engine/Effect.h"

Pass::Pass(const char* id, Technique* pTechnique)
	:	m_strID(id ? id : ""),
		m_pTechnique(pTechnique),
		m_pEffect(NULL),
		m_pVertexAttributeBinding(NULL),
		m_pSampler(NULL)
{
	RenderState::m_pParent = (RenderState*)pTechnique;
}

Pass::Pass(const Pass& copy) {

}

Pass::~Pass() {

	//SAFE_DELETE( m_pEffect );
	SAFE_DELETE( m_pVertexAttributeBinding );
}

bool Pass::initialize(const char* vshPath, const char* fshPath, const char* defines) {

	GP_ASSERT(vshPath);
	GP_ASSERT(fshPath);

	SAFE_DELETE(m_pEffect);
	SAFE_DELETE(m_pVertexAttributeBinding);

	// Create/Load Effect
	m_pEffect = Effect::createFromFile(vshPath, fshPath, defines);
	if (m_pEffect == NULL)
	{
		GP_ERROR("Failed to create Effect for Pass.");
		return false;
	}

	return true;
}

const char* Pass::getId() {

	return m_strID.c_str();
}

void Pass::setVertexAttributeBinding(VertexAttributeBinding* pBinding) {
	
	SAFE_DELETE( m_pVertexAttributeBinding );

	if(pBinding) {
		m_pVertexAttributeBinding = pBinding;
		//pBinding->addRef();
	}
}

VertexAttributeBinding*	Pass::getVertexAttributeBinding() {

	return m_pVertexAttributeBinding;
}

void Pass::setSampler(Texture::Sampler* pSampler) {
	
	SAFE_DELETE( m_pSampler );

	if(pSampler) {
		m_pSampler = pSampler;
		//pSampler->addRef();
	}
}

Texture::Sampler* Pass::getSampler() const {
	return m_pSampler;
}

Effect*	Pass::getEffect() const {
	return m_pEffect;
}

void Pass::bind() {

	GP_ASSERT( m_pEffect );

	// Bind our effect.
	m_pEffect->bind();
	
	// Bind our render state
	RenderState::bind(this);

	// If we have a vertex attribute binding, bind it
	if(m_pVertexAttributeBinding) {
		m_pVertexAttributeBinding->bind();
	}
}

void Pass::unbind() {

	// If we have a vertex attribute binding, unbind it
	if(m_pVertexAttributeBinding) {
		m_pVertexAttributeBinding->unbind();
	}
}