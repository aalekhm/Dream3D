#include "Engine/Technique.h"

Technique::Technique(const char* id, Material* material) 
	:	m_strID(id),
		m_pMaterial(material)
{
	RenderState::m_pParent = (RenderState*)material;
}

Technique::Technique(const Technique&) {

}

Technique::~Technique() {

	// Destroy all the passes.
	for(int i = 0; i < m_vPasses.size(); i++) {
		SAFE_DELETE( m_vPasses[i] );
	}
}

const char*	Technique::getId() {

	return m_strID.c_str();
}

unsigned int Technique::getPassCount() const {

	return (unsigned int)m_vPasses.size();
}

Pass* Technique::getPassByIndex(unsigned int index) const {

	GP_ASSERT( index < m_vPasses.size() && index >= 0);

	return m_vPasses[index];
}

Pass* Technique::getPass(const char* id) const {

	GP_ASSERT( id );

	for(size_t i = 0, count = m_vPasses.size(); i < count; i++) {

		Pass* pass = m_vPasses[i];
		GP_ASSERT( pass );
		if(strcmp(pass->getId(), id) == 0) {
			return pass;
		}
	}

	return NULL;
}

void Technique::setNodeBinding(Node* node) {

	RenderState::setNodeBinding(node);

	for (size_t i = 0, count = m_vPasses.size(); i < count; ++i) {

		m_vPasses[i]->setNodeBinding(node);
	}
}

