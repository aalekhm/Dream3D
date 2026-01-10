#include "Engine/FrameBuffer.h"

static unsigned int							__iMaxRenderTargets = 0;
static std::vector<FrameBuffer*>		__vFrameBuffers;
static FrameBufferHandle				__pDefaultHandle = 0;
static FrameBufferHandle				__pCurrentHandle = 0;

FrameBuffer::FrameBuffer(const char* id, unsigned int iWidth, unsigned int iHeight) 
	: m_sId(id ? id : "")
	,  m_iWidth(iWidth)
	, m_iHeight(iHeight)
	, m_pHandle(0)
	, m_ppRenderTargets(NULL)
	, m_pDepthStencilTarget(NULL)
{
}

FrameBuffer::~FrameBuffer() {
	if (m_ppRenderTargets) {
		for (unsigned int i = 0; i < __iMaxRenderTargets; ++i) {
			if (m_ppRenderTargets[i]) {
				//SAFE_RELEASE(_renderTargets[i]);
				SAFE_DELETE( m_ppRenderTargets[i] );
			}
		}

		SAFE_DELETE_ARRAY(m_ppRenderTargets);
	}

	if (m_pDepthStencilTarget) {
		//SAFE_RELEASE(m_pDepthStencilTarget);
		SAFE_DELETE(m_pDepthStencilTarget);
	}

	// Release GL resource.
	if (m_pHandle) {
		GL_ASSERT( glDeleteFramebuffers(1, &m_pHandle) );
	}

	// Remove self from vector.
	std::vector<FrameBuffer*>::iterator itr = std::find(__vFrameBuffers.begin(), __vFrameBuffers.end(), this);
	if (itr != __vFrameBuffers.end()) {
		__vFrameBuffers.erase(itr);
	}
}

void FrameBuffer::initialize() {

	// Query the current/initial FBO handle and store is as out 'default' frame buffer.
	// On many platforms this will simply be the zero (0) handle, but this is not always the case.
	GLint fbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
	__pDefaultHandle = (FrameBufferHandle)fbo;
	__pCurrentHandle = __pDefaultHandle;

	// Query the max supported color attachments. This glGet operation is not supported
	// on GL ES 2.x, so if the define does not exist, assume a value of 1.
#ifdef GL_MAX_COLOR_ATTACHMENTS
	GLint iVal;
	GL_ASSERT( glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &iVal) );
	__iMaxRenderTargets = (unsigned int)std::max(1, iVal);
#else
	__iMaxRenderTargets = 1;
#endif
}

FrameBuffer* FrameBuffer::create(const char* id) {
	return create(id, 0, 0);
}

FrameBuffer* FrameBuffer::create(const char* id, unsigned int iWidth, unsigned int iHeight) {

	// Call getMaxRenderTargets() to force __maxRenderTargets to be set
	getMaxRenderTargets();

	RenderTarget* pRenderTarget = NULL;
	if(iWidth > 0 && iHeight > 0) {
		if( ! isPowerOfTwo(iWidth) || ! isPowerOfTwo(iHeight) ) {
			GP_ERROR( "Failed to create render target for frame buffer. Width and Height must be a power of 2." );
			return NULL;
		}

		// Create a default RenderTarget with same ID.
		pRenderTarget = RenderTarget::create(id, iWidth, iHeight);
		if(pRenderTarget == NULL) {
			GP_ERROR( "Failed to create render target for frame buffer." );
			return NULL;
		}
	}

	// Create the frame buffer
	GLuint iHandle = 0;
	GL_ASSERT( glGenFramebuffers(1, &iHandle) );
	FrameBuffer* pFrameBuffer = new FrameBuffer(id, iWidth, iHeight);
	pFrameBuffer->m_pHandle = iHandle;

	// Create the render target array for the new frame buffer
	pFrameBuffer->m_ppRenderTargets = new RenderTarget*[__iMaxRenderTargets];
	memset(pFrameBuffer->m_ppRenderTargets, 0, sizeof(RenderTarget*) * __iMaxRenderTargets);

	if(pRenderTarget) {
		pFrameBuffer->setRenderTarget(pRenderTarget, 0);
		//SAFE_RELEASE(pRenderTarget);
	}

	__vFrameBuffers.push_back(pFrameBuffer);
	return pFrameBuffer;
}

FrameBuffer* FrameBuffer::getFrameBuffer(const char* id) {

	GP_ASSERT( id );

	// Search the vector for a matching ID.
	std::vector<FrameBuffer*>::const_iterator itr;
	for (itr = __vFrameBuffers.begin(); itr < __vFrameBuffers.end(); itr++) {

		FrameBuffer* pFB = *itr;
		GP_ASSERT( pFB );
		if(strcmp(id, pFB->getId()) == 0) {
			return pFB;
		}
	}

	return NULL;
}

const char* FrameBuffer::getId() const {
	return m_sId.c_str();
}

unsigned int FrameBuffer::getWidth() const {
	return m_iWidth;
}

unsigned int FrameBuffer::getHeight() const {
	return m_iHeight;
}

unsigned int FrameBuffer::getMaxRenderTargets() {
	return __iMaxRenderTargets;
}

void FrameBuffer::setRenderTarget(RenderTarget* pRenderTarget, unsigned int iIndex) {

	GP_ASSERT( iIndex < __iMaxRenderTargets && iIndex >= 0);
	GL_ASSERT( m_ppRenderTargets );

	if(m_ppRenderTargets[iIndex] == pRenderTarget) {
		// No change.
		return;
	}

	// Release our reference to the current RenderTarget at this index.
	//SAFE_RELEASE( m_ppRenderTargets[iIndex] );
	SAFE_DELETE( m_ppRenderTargets[iIndex] );

	m_ppRenderTargets[iIndex] = pRenderTarget;
	
	if(pRenderTarget) {
		GP_ASSERT( m_ppRenderTargets[iIndex]->getTexture() );

		// This FrameBuffer now references the RenderTarget.
		//pRenderTarget->addRef();

		// Now set this target as the color attachment corresponding to index.
		GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, m_pHandle) );
		GLenum iAttachment = GL_COLOR_ATTACHMENT0 + iIndex;
		GL_ASSERT( glFramebufferTexture2D(GL_FRAMEBUFFER, iAttachment, GL_TEXTURE_2D, m_ppRenderTargets[iIndex]->getTexture()->getHandle(), 0) );
		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(fboStatus != GL_FRAMEBUFFER_COMPLETE) {
			GP_ERROR( "Framebuffer status incomplete: 0x%x", fboStatus);
		}

		// Restore the FBO binding
		GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, __pCurrentHandle) );
	}
}

RenderTarget* FrameBuffer::getRenderTarget(unsigned int iIndex) const {

	GL_ASSERT( iIndex < __iMaxRenderTargets && iIndex >= 0 );
	GL_ASSERT( m_ppRenderTargets );

	if(iIndex < __iMaxRenderTargets && iIndex >= 0) {
		return m_ppRenderTargets[iIndex];
	}

	return NULL;
}

void FrameBuffer::setDepthStencilTarget(DepthStencilTarget* pDepthStencilTarget) {

	if(m_pDepthStencilTarget == pDepthStencilTarget) {
		return;
	}

	// Release our existing depth stencil target.
	//SAFE_RELEASE( m_pDepthStencilTarget );
	SAFE_DELETE( m_pDepthStencilTarget );

	m_pDepthStencilTarget = pDepthStencilTarget;

	if(pDepthStencilTarget) {

		// The FrameBuffer now owns this DepthStencilTarget.
		//pDepthStencilTarget->addRef();

		// Now set this target as the color attachment corresponding to index.
		GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, m_pHandle) );

		// Attach the render buffer to the framebuffer
		GL_ASSERT( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_pDepthStencilTarget->m_RenderBufferHandle) );
		if(pDepthStencilTarget->getFormat() == DepthStencilTarget::DEPTH_STENCIL) {
			GL_ASSERT( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL, GL_RENDERBUFFER, m_pDepthStencilTarget->m_RenderBufferHandle) );
		}

		// Check the framebuffer is good to go.
		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
			GP_ERROR("Framebuffer status incomplete: 0x%x", fboStatus);
		}

		// Restore the FBO binding
		GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, __pCurrentHandle) );
	}
}

DepthStencilTarget* FrameBuffer::getDepthStencilTarget() const {

	return m_pDepthStencilTarget;
}

void FrameBuffer::bind() {

	// Bind this FrameBuffer for rendering.
	GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, m_pHandle) );

	// Update the current FBO handle
	__pCurrentHandle = m_pHandle;
}

void FrameBuffer::bindDefault() {

	GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, __pDefaultHandle) );

	// Update the current FBO handle
	__pCurrentHandle = __pDefaultHandle;
}

bool FrameBuffer::isPowerOfTwo(unsigned int iValue) {

	return ( iValue != 0 ) && ( (iValue & (iValue - 1)) == 0 );
}


/*********************************************
* http://http.download.nvidia.com/developer/presentations/2005/GDC/OpenGL_Day/OpenGL_FrameBuffer_Object.pdf
*
In order of increasing performance:
	- Multiple FBOs
		create a separate FBO for each texture you want to render to
		switch using BindFramebuffer()
		can be 2x faster than wglMakeCurrent() in beta NVIDIA drivers
	- Single FBO, multiple texture attachments
		textures should have same format and dimensions
		use FramebufferTexture() to switch between textures
	- Single FBO, multiple texture attachments
		attach textures to different color attachments
		use glDrawBuffer() to switch rendering to different color attachments
*/////////////////////////////////////////////////////////////
