#ifndef MESH_H
#define MESH_H

#include "Engine/Base.h"
#include "Engine/VertexFormat.h"

class MeshPart;
class VertexAttributeBinding;
class Texture;

class Mesh {
	public:
		enum PrimitiveType {
			TRIANGLES = GL_TRIANGLES,
			TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
			LINES = GL_LINES,
			LINE_STRIP = GL_LINE_STRIP,
			POINTS = GL_POINTS
		};

		enum IndexFormat {
			INDEX8 = GL_UNSIGNED_BYTE,
			INDEX16 = GL_UNSIGNED_SHORT,
			INDEX32 = GL_UNSIGNED_INT
		};
		
		virtual ~Mesh();
		static Mesh*			createMesh(const VertexFormat& vertexFormat, unsigned int vertexCount, bool bDynamic);

		const VertexFormat&		getVertexFormat() const;
		unsigned int			getVertexCount() const;
		unsigned int			getVertexSize() const;
		VBOHandle				getVertexBuffer() const;
		bool					isDynamic() const;

		PrimitiveType			getPrimitiveType() const;
		void					setPrimitiveType(Mesh::PrimitiveType type);
		void					setVertexData(const float* vertexData, unsigned int vertexStart, unsigned int vertexCount);

		MeshPart*				addMeshPart(Mesh::PrimitiveType primitiveType, Mesh::IndexFormat indexFormat, unsigned int indexCount, bool isDynamic = false);
		unsigned int			getMeshPartCount() const;
		MeshPart*				getMeshPart(unsigned int index);
	private:
		Mesh(const VertexFormat& vertexFormat);
		Mesh(const Mesh& copy);

		const VertexFormat	m_VertexFormat;
		unsigned int		m_iVertexCount;
		VBOHandle			m_hVBO;
		PrimitiveType		m_PrimitiveType;
		bool				m_bDynamic;

		MeshPart**			m_ppMeshParts;
		unsigned int		m_iPartCount;
};

#endif