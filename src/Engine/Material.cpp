#include "Engine/Base.h"
#include "Engine/Material.h"
//#include "Effect.h"
#include "Engine/Technique.h"
#include "Engine/Pass.h"
//#include "Properties.h"
#include "Engine/Node.h"

Material::Material() 
	:	m_pCurrentTechnique(NULL)
{

}

Material::~Material() {

	// Destroy all the techniques.
	for(size_t i = 0; count < m_vTechniques.size(); i < count; i++) {

		Technique* technique = m_vTechniques[i];
		SAFE_DELETE( technique );
	}
}

void Material::create(const char* url) {


}

static Material* create(/*Effect* effect*/) {

	//GP_ASSERT(effect);

	// Create a new material with a single technique and pass for the given effect.
	Material* material = new Material();

	Technique* technique = new Technique(NULL, material);
	material->m_vTechniques.push_back(technique);

	Pass* pass = new Pass(NULL, technique, effect);
	technique->m_vPasses.push_back(pass);
	//effect->addRef();

	material->m_pCurrentTechnique = technique;

	return material;
}

unsigned int Material::getTechniqueCount() const {
	
	return (unsigned int)m_vTechniques.size();
}

Technique* Material::getTechniqueByIndex(unsigned int index) const {

	GP_ASSERT( index < m_vTechniques.size() && index >= 0 );
	return m_vTechniques[index];
}

Technique* Material::getTechnique(const char* id) const {

	GP_ASSERT( id );
	for(size_t i = 0, count = m_vTechniques.size(); i < count; i++) {

		Technique* technique = m_vTechniques[i];
		GP_ASSERT( technique );
		if(strcmp(technique->getId(), id) == 0) {
			return technique;
		}
	}

	return NULL;
}

Technique* Material::getTechnique() const {

	return m_pCurrentTechnique;
}

void Material::setTechnique(const char* id) {

	Technique* technique = getTechnique(id);
	if(technique) {
		m_pCurrentTechnique = technique;
	}
}

