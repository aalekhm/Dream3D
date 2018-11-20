#ifndef DEPTHSTENCILTARGET_H
#define DEPTHSTENCILTARGET_H

#include "Engine/Base.h"
#include "Engine/Texture.h"

/**
 * Defines a container for depth and stencil targets in a frame buffer object.
 */
class DepthStencilTarget {

	friend class FrameBuffer;
	
	public:
		
		// Defines the accepted formats for DepthStencilTargets.
		enum Format {
			DEPTH,					// A target with depth data.
			DEPTH_STENCIL		// A target with depth data and stencil data.
		};

		/**
		 * Create a DepthStencilTarget and add it to the list of available DepthStencilTargets.
		 *
		 * @param id The ID of the new DepthStencilTarget.  Uniqueness is recommended but not enforced.
		 * @param format The format of the new DepthStencilTarget.
		 * @param width Width of the new DepthStencilTarget.
		 * @param height Height of the new DepthStencilTarget.
		 *
		 * @return A newly created DepthStencilTarget.
		 */
		static DepthStencilTarget* create(const char* id, Format eFormat, unsigned int iWidth, unsigned int iHeight);
			
		/**
		 * Get a named DepthStencilTarget from its ID.
		 *
		 * @param id The ID of the DepthStencilTarget to search for.
		 *
		 * @return The DepthStencilTarget with the specified ID, or NULL if one was not found.
		 */
		static DepthStencilTarget* getDepthStencilTarget(const char* id);

		/**
		 * Get the ID of this DepthStencilTarget.
		 *
		 * @return The ID of this DepthStencilTarget.
		 */
		const char* getId() const;

		/**
		 * Returns the format of the DepthStencilTarget.
		 *
		 * @return The format.
		 */
		Format getFormat() const;

		/**
		 * Returns the width of the DepthStencilTarget.
		 *
		 * @return The width.
		 */
		unsigned int getWidth() const;

		/**
		* Returns the height of the DepthStencilTarget.
		*
		* @return The height.
		*/
		unsigned int getHeight() const;

	private:
		/**
		 * Constructor.
		 */
		DepthStencilTarget(const char* id, Format eFormat, unsigned int iWidth, unsigned int iHeight);

		/**
		* Destructor.
		*/
		~DepthStencilTarget();

		/**
		 * Hidden copy assignment operator.
		 */
		DepthStencilTarget& operator=(DepthStencilTarget&);

		std::string					m_sId;
		Format						m_eFormat;
		RenderBufferHandle	m_RenderBufferHandle;
		unsigned int				m_iWidth;
		unsigned int				m_iHeight;
};

#endif