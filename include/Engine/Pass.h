#ifndef PASS_H
#define PASS_H

#include "Common/CCString.h"
#include "Engine/RenderState.h"
#include "Engine/VertexAttributeBinding.h"
#include "Engine/Texture.h"

class Technique;
class Material;
class RenderState;

class Pass : public RenderState {
	
	public:
		~Pass();

		static Pass* create(const char* id, Technique* pTechnique/*, const char* vshPath, const char* fshPath, const char* defines*/);

		const char*					getId();
		void								setVertexAttributeBinding(VertexAttributeBinding* pBinding);
		VertexAttributeBinding*	getVertexAttributeBinding();

		void								setSampler(Texture::Sampler* pSampler);
		Texture::Sampler*			getSampler();

		void								bind();
		void								unbind();
	private:
		Pass(const char* id, Technique* pTechnique/*, Effect* pEffect*/);
		Pass(const Pass& copy);

		CCString							m_strID;
		Technique*							m_pTechnique;
		//Effect*							m_pEffect;
		VertexAttributeBinding*		m_pVertexAttributeBinding;
		Texture::Sampler*				m_pSampler;

};

#endif