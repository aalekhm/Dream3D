#ifndef MESHPART_H
#define MESHPART_H

#include "Engine/Base.h"
#include "Engine/Mesh.h"

class Mesh;

// Defines a part of a mesh describing the way the
// mesh's vertices are connected together.
class MeshPart {

	public:
		~MeshPart();

		unsigned int				getMeshIndex() const;
		Mesh::PrimitiveType		getPrimitiveType() const;
		unsigned int				getIndexCount() const;
		Mesh::IndexFormat		getIndexFormat() const;
		IBOHandle					getIndexBuffer() const;
		bool							isDynamic() const;
		void							setIndexData(void* indexData, unsigned int indexStart, unsigned int indexCount);

		static MeshPart*		create(Mesh* mesh, unsigned int meshIndex, Mesh::PrimitiveType primitiveType, Mesh::IndexFormat indexFormat, unsigned int indexCount, bool isDynamic = false);

		GLvoid*						getMapBuffer();
		void							unmapBuffer();
	private:
		MeshPart();
		
		Mesh*					m_pMesh;
		unsigned int			m_iMeshIndex;
		Mesh::PrimitiveType		m_PrimitiveType;
		Mesh::IndexFormat		m_IndexFormat;
		unsigned int			m_iIndexCount;
		IBOHandle				m_hIBO;
		bool					m_bDynamic;
};

#endif