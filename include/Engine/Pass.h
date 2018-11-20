#ifndef PASS_H
#define PASS_H

#include "Common/CCString.h"
#include "Engine/RenderState.h"
#include "Engine/VertexAttributeBinding.h"

class Pass {
	
	public:
		friend class Technique;
		friend class Material;
		friend class RenderState;

		~Pass();

		const char*				getId() const;
		void					setVertexAttributeBinding(VertexAttributeBinding* pBinding);
		VertexAttributeBinding*	getVertexAttributeBinding();
		void					bind();
		void					unbind();
	private:
		Pass(const char* id, Technique* pTechnique/*, Effect* pEffect*/);
		Pass(const Pass& copy);

		static Pass* create(const char* id, Technique* pTechnique/*, const char* vshPath, const char* fshPath, const char* defines*/);

		CCString				m_strID;
		Technique*				m_pTechnique;
		//Effect*				m_pEffect;
		VertexAttributeBinding*	m_pVertexAttributeBinding;
};

#endif