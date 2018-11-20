#ifndef MESHBATCH_H
#define MESHBATCH_H

#include "Engine/Base.h"
#include "Engine/Mesh.h"

class VertexFormat;
class VertexAttributeBinding;
class Mesh;
class Texture;
class Material;

class MeshBatch {

	public:
		static MeshBatch*		create(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, const char* materialUrl, bool bIndexed, unsigned int iInitialCapacity = 1024, unsigned int iGrowSize = 1024);
		static MeshBatch*		create(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, Material* material, bool bIndexed, unsigned int iInitialCapacity = 1024, unsigned int iGrowSize = 1024);

		unsigned int			getCapacity() const;
		void					setCapacity(unsigned int iCapacity);
		const VertexFormat&		getVertexFormat() const;

		template <class T>
		void					add(T* vertices, unsigned int vertexCount, unsigned short* pIndices = NULL, unsigned int iIndexCount = 0);

		void					start();
		void					stop();
		void					render();

		Material*				getMaterial() const;
		void					setTexture(const char* path, bool generateMipmaps = false);
		void					setTexture(Texture* pTexture, bool generateMipmaps = false);
		void					bindTexture();
		void					unbindTexture();
		Texture*				getTexture();

		~MeshBatch();
	private:
		MeshBatch(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, Material* pMaterial, bool bIndexed, unsigned int iInitialCapacity, unsigned int iGrowSize);
		
		bool					resize(unsigned int iCapacity);

		void					updateVertexAttributeBinding();
		void					setVertexAttributeBinding(VertexAttributeBinding* vaBinding);
		VertexAttributeBinding*	m_pVertexAttributeBinding;

		const VertexFormat&		m_VertexFormat;
		Mesh::PrimitiveType		m_PrimitiveType;

		Material*				m_pMaterial;
		
		bool					m_bIndexed;
		unsigned int			m_iCapacity;
		unsigned int			m_iGrowSize;

		unsigned int			m_iVertexCapacity;
		unsigned int			m_iIndexCapacity;

		unsigned int			m_iVertexCount;
		unsigned int			m_iIndexCount;

		unsigned char*			m_pVertices;
		unsigned char*			m_pVerticesPtr;

		unsigned short*			m_pIndices;
		unsigned short*			m_pIndicesPtr;

		Texture*				m_pTexture;
};

template<class T>
inline void MeshBatch::add(T* vertices, unsigned int vertexCount, unsigned short* pIndices, unsigned int iIndexCount) {
	GP_ASSERT(vertices);
	GP_ASSERT(sizeof(T) == m_VertexFormat.getVertexSize());

	unsigned int newVertexCount = m_iVertexCount + vertexCount;
	unsigned int newIndexCount = m_iIndexCount + iIndexCount;
	if (m_PrimitiveType == Mesh::TRIANGLE_STRIP && m_iVertexCount > 0)
		newIndexCount += 2; // need an extra 2 indices for connecting strips with degenerate triangles

	// Do we need to grow the batch?
	while (newVertexCount > m_iVertexCapacity || (m_bIndexed && newIndexCount > m_iIndexCapacity))
	{
		if (m_iGrowSize == 0)
			return; // growing disabled, just clip batch
		if (!resize(m_iCapacity + m_iGrowSize))
			return; // failed to grow
	}

	// Copy vertex data.
	GP_ASSERT(m_pVerticesPtr);
	unsigned int vBytes = vertexCount * m_VertexFormat.getVertexSize();
	memcpy(m_pVerticesPtr, vertices, vBytes);

	// Copy index data.
	if (m_bIndexed)
	{
		GP_ASSERT(pIndices);
		GP_ASSERT(m_pIndicesPtr);

		if (m_iVertexCount == 0)
		{
			// Simply copy values directly into the start of the index array.
			memcpy(m_pIndicesPtr, pIndices, iIndexCount * sizeof(unsigned short));
		}
		else
		{
			if (m_PrimitiveType == Mesh::TRIANGLE_STRIP)
			{
				// Create a degenerate triangle to connect separate triangle strips
				// by duplicating the previous and next vertices.
				m_pIndicesPtr[0] = *(m_pIndicesPtr-1);
				m_pIndicesPtr[1] = m_iVertexCount;
				m_pIndicesPtr += 2;
			}

			// Loop through all indices and insert them, with their values offset by
			// 'vertexCount' so that they are relative to the first newly inserted vertex.
			for (unsigned int i = 0; i < iIndexCount; ++i)
			{
				m_pIndicesPtr[i] = pIndices[i] + m_iVertexCount;
			}
		}
		m_pIndicesPtr += iIndexCount;
		m_iIndexCount = newIndexCount;
	}

	m_pVerticesPtr += vBytes;
	m_iVertexCount = newVertexCount;
}

#endif