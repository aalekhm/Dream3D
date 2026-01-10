#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Engine/Base.h"
#include "Engine/RenderTarget.h"
#include "Engine/DepthStencilTarget.h"

/**
 * Defines a video output off all graphics buffer containing a complete frame of data.
 * This consists of a RenderTarget and DepthStencilTarget holding the color, depth and
 * stencil data in the rendering frame. 
 * 
 * to change the default Game framebuffer call Game::setFrameBuffer(myFrameBuffer);
 * To restore back to the default call Game::setFrameBuffer(NULL).
 * This is useful for rendering shadows and other post-processing effects.
 */
class FrameBuffer {

	public:
		/**
		 * Creates a new, empty FrameBuffer object.
		 *
		 * The new FrameBuffer does not have any render targets or a depth/stencil target and these
		 * must be added before it can be used. The FrameBuffer is added to the list of available
		 * FrameBuffers.
		 *
		 * @param id The ID of the new FrameBuffer. Uniqueness is recommended but not enforced.
		 *
		 * @return A newly created FrameBuffer.
		 * @script{create}
		 */
		static FrameBuffer* create(const char* id);

		/**
		 * Creates a new FrameBuffer with a single RenderTarget of the specified width and height,
		 * and adds the FrameBuffer to the list of available FrameBuffers.
		 *
		 * If width and height are non-zero a default RenderTarget of type RGBA will be created
		 * and added to the FrameBuffer, with the same ID. The ID of the render target can be
		 * changed later via RenderTarget::setId(const char*).
		 *
		 * You can additionally add a DepthStencilTarget using FrameBuffer::setDepthStencilTarget.
		 *
		 * @param id The ID of the new FrameBuffer. Uniqueness is recommended but not enforced.
		 * @param width The width of the RenderTarget to be created and attached.
		 * @param height The height of the RenderTarget to be created and attached.
		 *
		 * @return A newly created FrameBuffer.
		 */
		static FrameBuffer* create(const char* id, unsigned int iWidth, unsigned int iHeight);

		static void initialize();

		/**
		 * Get a named FrameBuffer from its ID.
		 *
		 * @param id The ID of the FrameBuffer to search for.
		 *
		 * @return The FrameBuffer with the specified ID, or NULL if one was not found.
		 */
		static FrameBuffer* getFrameBuffer(const char* id);

		/**
		 * Get the ID of this FrameBuffer.
		 *
		 * @return The ID of this FrameBuffer.
		 */
		const char* getId() const;

		/**
		 * Gets the width of the frame buffer.
		 *
		 * @return The width of the frame buffer.
		 */
		unsigned int getWidth() const;

		/**
		 * Gets the height of the frame buffer.
		 *
		 * @return The height of the frame buffer.
		 */
		unsigned int getHeight() const;

		/**
		 * Get the number of color attachments available on the current hardware.
		 *
		 * @return The number of color attachments available on the current hardware.
		 */
		static unsigned int getMaxRenderTargets();

		/**
		 * Set a RenderTarget on this FrameBuffer's color attachment at the specified index.
		 *
		 * @param target The RenderTarget to set.
		 * @param index The index of the color attachment to set.
		 */
		void setRenderTarget(RenderTarget* pRenderTarget, unsigned int iIndex = 0);

		/**
		 * Get the RenderTarget attached to the FrameBuffer's color attachment at the specified index.
		 *
		 * @param index The index of the color attachment to retrieve a RenderTarget from.
		 *
		 * @return The RenderTarget attached at the specified index.
		 */
		RenderTarget* getRenderTarget(unsigned int iIndex = 0) const;

		/**
		 * Set this FrameBuffer's DepthStencilTarget.
		 *
		 * @param target The DepthStencilTarget to set on this FrameBuffer.
		 */
		void setDepthStencilTarget(DepthStencilTarget* pDepthStencilTarget);

		/**
		 * Get this FrameBuffer's DepthStencilTarget.
		 *
		 * @return This FrameBuffer's DepthStencilTarget.
		 */
		DepthStencilTarget* getDepthStencilTarget() const;

		/**
		 * Binds this FrameBuffer for off-screen rendering.
		 */
		void bind();

		/**
		 * Binds the default FrameBuffer for rendering to the display.
		 */
		void bindDefault();
	private:
		/**
		 * Constructor.
		 */
		FrameBuffer(const char* id, unsigned int iWidth, unsigned int iHeight);

		/**
		 * Destructor.
		 */
		~FrameBuffer();

		/**
		 * Hidden copy assignment operator.
		 */
		FrameBuffer& operator=(FrameBuffer&);

		static bool isPowerOfTwo(unsigned int iValue);

		std::string					m_sId;
		unsigned int				m_iWidth;
		unsigned int				m_iHeight;
		FrameBufferHandle	m_pHandle;
		RenderTarget**			m_ppRenderTargets;
		DepthStencilTarget*	m_pDepthStencilTarget;
};

#endif