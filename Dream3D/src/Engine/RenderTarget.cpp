#include "Engine/RenderTarget.h"

static std::vector<RenderTarget*> __vRenderTargets;

RenderTarget::RenderTarget(const char* id)
	: m_sId(id ? id : "")
	, m_pTexture(NULL)
{

}

RenderTarget::~RenderTarget() {

	//SAFE_RELEASE( m_pTexture );
	SAFE_DELETE( m_pTexture );

	// Remove ourself from the cache.
	std::vector<RenderTarget*>::iterator itr = std::find(__vRenderTargets.begin(), __vRenderTargets.end(), this);
	if(itr != __vRenderTargets.end()) {
		__vRenderTargets.erase(itr);
	}
}

RenderTarget* RenderTarget::create(const char* id, unsigned int iWidth, unsigned int iHeight) {

	// Create a new texture with the given width.
	Texture* ptexture = Texture::create(Texture::RGBA, iWidth, iHeight, NULL, false);
	if(ptexture == NULL) {
		GP_ERROR("Failed to create texture for render target.");
		return NULL;
	}

	RenderTarget* pRenderTarget = create(id, ptexture);
	//pTexture->release();

	return pRenderTarget;
}

RenderTarget* RenderTarget::create(const char* id, Texture* pTexture) {

	RenderTarget* pRenderTarget = new RenderTarget(id);
	pRenderTarget->m_pTexture = pTexture;
	//pRenderTarget->m_pTexture->addRef();

	__vRenderTargets.push_back(pRenderTarget);

	return pRenderTarget;
}

RenderTarget* RenderTarget::getRenderTarget(const char* id) {

	GP_ASSERT( id );

	// Search the vector for a matching ID.
	std::vector<RenderTarget*>::iterator itr;
	for(itr = __vRenderTargets.begin(); itr != __vRenderTargets.end(); ++itr) {

		RenderTarget* pDst = *itr;
		GP_ASSERT( pDst );

		if(strcmp(id, pDst->getId()) == 0) {

			return pDst;
		}
	}

	return NULL;
}

const char* RenderTarget::getId() const {

	return m_sId.c_str();
}

Texture* RenderTarget::getTexture() const {

	return m_pTexture;
}

unsigned int RenderTarget::getWidth() const {

	return m_pTexture->getWidth();
}

unsigned int RenderTarget::getHeight() const {

	return m_pTexture->getHeight();
}
