#include "Engine/Pass.h"


Pass::Pass(const char* id, Technique* pTechnique/*, Effect* pEffect*/) 
	:	m_strID(id ? id : ""),
		m_pTechnique(pTechnique),
		m_pVertexAttributeBinding(NULL)
{
	RenderState::m_pParent = m_pTechnique;
}

Pass::Pass(const Pass& copy) {

}

Pass::~Pass() {

	//SAFE_DELETE( m_pEffect );
	SAFE_DELETE( m_pVertexAttributeBinding );
}

Pass* Pass::create(const char* id, Technique* pTechnique/*, const char* vshPath, const char* fshPath, const char* defines*/) {

	//// Attempt to create/load the effect.
	//Effect* effect = Effect::createFromFile(vshPath, fshPath, defines);
	//if (effect == NULL) {
	//	GP_ERROR("Failed to create effect for pass.");
	//	return NULL;
	//}

	// Return the new pass.
	return new Pass(id, pTechnique/*, effect*/);
}

const char*	Pass::getId() const {

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

void Pass::bind() {

	//GP_ASSERT(_effect);

	//// Bind our effect.
	//_effect->bind();
	
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