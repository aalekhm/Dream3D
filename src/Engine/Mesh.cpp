#include "Engine/Mesh.h"
#include "Engine/MeshPart.h"
#include "Engine/VertexAttributeBinding.h"
#include "Engine/Texture.h"

Mesh::Mesh(const VertexFormat& vertexFormat) 
	:	m_VertexFormat(vertexFormat),
		m_iVertexCount(0),
		m_hVBO(0),
		m_PrimitiveType(TRIANGLES),
		m_bDynamic(false),
		m_ppMeshParts(NULL),
		m_iPartCount(0)
{

}

Mesh::~Mesh() {

	if(m_hVBO) {
		GL_ASSERT( glDeleteBuffers(1, &m_hVBO) );
		m_hVBO = 0;
	}
}

Mesh* Mesh::createMesh(const VertexFormat& vertexFormat, unsigned int vertexCount, bool bDynamic) {
	GLuint vbo;
	GL_ASSERT( glGenBuffers(1, &vbo) );
	GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, vbo) );
	GL_ASSERT( glBufferData(GL_ARRAY_BUFFER, vertexFormat.getVertexSize()*vertexCount, NULL, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW ));
	GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0) );

	Mesh* mesh = new Mesh(vertexFormat);
	mesh->m_iVertexCount = vertexCount;
	mesh->m_hVBO = vbo;
	mesh->m_bDynamic = bDynamic;

	return mesh;
}

const VertexFormat& Mesh::getVertexFormat() const {
	return m_VertexFormat;
}

unsigned int Mesh::getVertexCount() const {
	return m_iVertexCount;
}

unsigned int Mesh::getVertexSize() const {
	return m_VertexFormat.getVertexSize();
}

VBOHandle Mesh::getVertexBuffer() const {
	return m_hVBO;
}

bool Mesh::isDynamic() const {
	return m_bDynamic;
}

Mesh::PrimitiveType Mesh::getPrimitiveType() const {
	return m_PrimitiveType;
}

void Mesh::setPrimitiveType(Mesh::PrimitiveType type) {
	m_PrimitiveType = type;
}

void Mesh::setVertexData(const float* vertexData, unsigned int vertexStart, unsigned int vertexCount) {

	GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, m_hVBO) );
	if(vertexStart == 0 && vertexCount == 0) {
		GL_ASSERT( glBufferData(GL_ARRAY_BUFFER, m_VertexFormat.getVertexSize() * m_iVertexCount, vertexData, m_bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );
	}
	else {
		if(vertexCount == 0) {
			vertexCount = m_iVertexCount - vertexStart;
		}

		GL_ASSERT( glBufferSubData(GL_ARRAY_BUFFER, vertexStart * m_VertexFormat.getVertexSize(), vertexCount * m_VertexFormat.getVertexSize(), vertexData) );
	}
	GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0) );
}

MeshPart* Mesh::addMeshPart(PrimitiveType primitiveType, IndexFormat indexFormat, unsigned int indexCount, bool isDynamic) {

	MeshPart* meshPart = MeshPart::create(this, m_iPartCount, primitiveType, indexFormat, indexCount, isDynamic);

	if(meshPart) {
		// Increase size of mesh part array and copy old subsets into it.
		MeshPart** oldMeshParts = m_ppMeshParts;
		m_ppMeshParts = new MeshPart*[m_iPartCount + 1];

		for(unsigned int i = 0; i < m_iPartCount; i++) {
			m_ppMeshParts[i] = oldMeshParts[i];
		}

		// Add new part to array.
		m_ppMeshParts[m_iPartCount++] = meshPart;

		// Delete old part array.
		SAFE_DELETE_ARRAY(oldMeshParts);
	}

	return meshPart;
}

unsigned int Mesh::getMeshPartCount() const {
	return m_iPartCount;
}

MeshPart* Mesh::getMeshPart(unsigned int index) {
	GP_ASSERT( m_ppMeshParts );
	return m_ppMeshParts[index];
}
