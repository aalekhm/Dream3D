#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include "Common/CCString.h"
#include "Engine/Pass.h"

class Material;

class Technique : public RenderState {

	friend class Material;
	friend class Pass;
	friend class RenderState;

	public:
		const char*		getId();
		unsigned int	getPassCount() const;
		Pass*			getPassByIndex(unsigned int index) const;
		Pass*			getPass(const char* id) const;
		void			setNodeBinding(Node* node);
	private:
		Technique(const char* id, Material* material);
		Technique(const Technique&);
		~Technique();

		Technique& operator=(const Technique&);

		CCString			m_strID;
		Material*			m_pMaterial;
		std::vector<Pass*>	m_vPasses;
};

#endif