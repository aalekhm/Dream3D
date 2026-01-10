#include "Engine/MeshPart.h"

MeshPart::MeshPart() 
	: m_pMesh(NULL),
	  m_iMeshIndex(0),
	  m_PrimitiveType(Mesh::TRIANGLES),
	  m_iIndexCount(0),
	  m_hIBO(0),
	  m_bDynamic(false)
{

}

MeshPart::~MeshPart() {
	if(m_hIBO) {
		GL_ASSERT( glDeleteBuffers(1, &m_hIBO) );
		m_hIBO = 0;
	}
}

MeshPart* MeshPart::create(	Mesh* mesh, 
							unsigned int meshIndex, 
							Mesh::PrimitiveType primitiveType, 
							Mesh::IndexFormat indexFormat, 
							unsigned int indexCount, 
							bool isDynamic
) {
	// Create a VBO for our index buffer.
	GLuint vbo;
	GL_ASSERT( glGenBuffers(1, &vbo) );
	GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo) );

	unsigned int indexSize = 0;
	switch(indexFormat) {
		case Mesh::INDEX8:
			indexSize = 1;
		break;
		case Mesh::INDEX16:
			indexSize = 2;
		break;
		case Mesh::INDEX32:
			indexSize = 4;
		break;
		default:
			//GP_ERROR("Unsupported index format (%d).", indexFormat");
			glDeleteBuffers(1, &vbo);
			return NULL;
	}

	GL_ASSERT( glBufferData(GL_ELEMENT_ARRAY_BUFFER,  indexSize * indexCount, NULL, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );

	MeshPart* meshPart = new MeshPart();
	meshPart->m_pMesh = mesh;
	meshPart->m_iMeshIndex = meshIndex;
	meshPart->m_PrimitiveType = primitiveType;
	meshPart->m_IndexFormat = indexFormat;
	meshPart->m_iIndexCount = indexCount;
	meshPart->m_hIBO = vbo;
	meshPart->m_bDynamic = isDynamic;

	return meshPart;
}

unsigned int MeshPart::getMeshIndex() const {
	return m_iMeshIndex;
}

Mesh::PrimitiveType	MeshPart::getPrimitiveType() const {
	return m_PrimitiveType;
}

unsigned int MeshPart::getIndexCount() const {
	return m_iIndexCount;
}

Mesh::IndexFormat MeshPart::getIndexFormat() const {
	return m_IndexFormat;
}

IBOHandle MeshPart::getIndexBuffer() const {
	return m_hIBO;
}

bool MeshPart::isDynamic() const {
	return m_bDynamic;
}

void MeshPart::setIndexData(void* indexData, unsigned int indexStart, unsigned int indexCount) {
	
	GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO) );

	unsigned int indexSize = 0;
	switch(m_IndexFormat) {
	case Mesh::INDEX8:
		indexSize = 1;
		break;
	case Mesh::INDEX16:
		indexSize = 2;
		break;
	case Mesh::INDEX32:
		indexSize = 4;
		break;
	default:
		//GP_ERROR("Unsupported index format (%d).", indexFormat");
		return;
	}
	
	if(indexStart == 0 && indexCount == 0) {
		GL_ASSERT( glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * m_iIndexCount, indexData, m_bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );
	}
	else {
		if(indexCount == 0) {
			indexCount = m_iIndexCount - indexStart;
		}

		GL_ASSERT( glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexStart *  indexSize, indexCount * indexSize, indexData) );
	}
}

GLvoid* MeshPart::getMapBuffer() {

	GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO) );
	GLvoid* pIBOMapBuffer = NULL;
	pIBOMapBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

	return pIBOMapBuffer;
}

void MeshPart::unmapBuffer() {

	GL_ASSERT( glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER ) );
}
