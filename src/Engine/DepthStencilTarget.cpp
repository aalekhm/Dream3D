#include "Engine/DepthStencilTarget.h"

static std::vector<DepthStencilTarget*> __vDepthStencilTargets;

DepthStencilTarget::DepthStencilTarget(const char* id, Format eFormat, unsigned int iWidth, unsigned int iHeight)
	: m_sId(id ? id : "")
	, m_eFormat(eFormat)
	, m_iWidth(iWidth)
	, m_iHeight(iHeight)
	, m_RenderBufferHandle(0)
{
}

DepthStencilTarget::~DepthStencilTarget() {

	// Destroy GL resources.
	if(m_RenderBufferHandle) {
		GL_ASSERT( glDeleteTextures(1, &m_RenderBufferHandle) );
	}

	// Remove from vector.
	std::vector<DepthStencilTarget*>::iterator itr = std::find(__vDepthStencilTargets.begin(), __vDepthStencilTargets.end(), this);
	if(itr != __vDepthStencilTargets.end()) {
		__vDepthStencilTargets.erase(itr);
	}
}

DepthStencilTarget* DepthStencilTarget::create(const char* id, Format eFormat, unsigned int iWidth, unsigned int iHeight) {

	// Create the depth stencil target.
	DepthStencilTarget* pDepthStencilTarget =  new DepthStencilTarget(id, eFormat, iWidth, iHeight);

	// Create a render buffer for this new depth stencil target
	GL_ASSERT( glGenRenderbuffers( 1, &pDepthStencilTarget->m_RenderBufferHandle ) );
	GL_ASSERT( glBindRenderbuffer( GL_RENDERBUFFER, pDepthStencilTarget->m_RenderBufferHandle ) );
	GL_ASSERT( glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, iWidth, iHeight ));

	// Add it to the cache
	__vDepthStencilTargets.push_back(pDepthStencilTarget);

	return pDepthStencilTarget;
}

DepthStencilTarget* DepthStencilTarget::getDepthStencilTarget(const char* id) {

	GP_ASSERT(id);

	// Search the vector for a matching ID.
	std::vector<DepthStencilTarget*>::iterator itr;
	for(itr = __vDepthStencilTargets.begin(); itr != __vDepthStencilTargets.end(); itr++) {

		DepthStencilTarget* pDst = *itr;
		GP_ASSERT( pDst );
		if(strcmp(id, pDst->getId()) == 0) {
			return pDst;
		}
	}

	return NULL;
}

const char* DepthStencilTarget::getId() const {
	return m_sId.c_str();
}

unsigned int DepthStencilTarget::getWidth() const {
	return m_iWidth;
}

unsigned int DepthStencilTarget::getHeight() const {
	return m_iHeight;
}

DepthStencilTarget::Format DepthStencilTarget::getFormat() const {
	return m_eFormat;
}

