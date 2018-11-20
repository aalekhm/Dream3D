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
									Pass(const char* id, Technique* pTechnique);
									~Pass();

		bool						initialize(const char* vshPath, const char* fshPath, const char* defines);

		const char*					getId();
		void						setVertexAttributeBinding(VertexAttributeBinding* pBinding);
		VertexAttributeBinding*		getVertexAttributeBinding();

		void						setSampler(Texture::Sampler* pSampler);
		Texture::Sampler*			getSampler() const;

		Effect*						getEffect() const;

		void						bind();
		void						unbind();

		Effect*						m_pEffect; //Check how this is been accessed directly in Gameplay even when its private !!!
	private:
									Pass(const Pass& copy);

		CCString					m_strID;
		Technique*					m_pTechnique;
		VertexAttributeBinding*		m_pVertexAttributeBinding;
		Texture::Sampler*			m_pSampler;
};

#endif