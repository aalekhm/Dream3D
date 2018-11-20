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
		m_iPartCount(0),
		m_pTexture(NULL),

		m_pVertexAttributeBinding(NULL)
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

///////////////////////////////////// TO BE PUT IN 'MODEL' /////////////////////////////////////
void Mesh::setVertexAttributeBinding(VertexAttributeBinding* vaBinding) {
	GL_ASSERT( vaBinding );
	m_pVertexAttributeBinding = vaBinding;
}

void Mesh::draw(bool bWireframe) {
#ifdef USE_VERTEX_POINTERS
	m_pVertexAttributeBinding->bind();
	bindTexture();

	if(m_iPartCount == 0) {
		GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
		GL_ASSERT( glDrawArrays(getPrimitiveType(), 0, getVertexCount()) );
	}
	else {
		for(unsigned int i = 0; i < getMeshPartCount(); i++) {
			MeshPart* meshPart = getMeshPart(i);
			GP_ASSERT( meshPart );

			GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshPart->getIndexBuffer()) );
			GL_ASSERT( glDrawElements(meshPart->getPrimitiveType(), meshPart->getIndexCount(), meshPart->getIndexFormat(), 0) );
			GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
		}
	}

	m_pVertexAttributeBinding->unbind();
	unbindTexture();
#else
	if(m_iPartCount == 0) {
		bindTexture();
		m_pVertexAttributeBinding->bind();
		
		GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
		

		if(bWireframe && (getPrimitiveType() == Mesh::TRIANGLES || getPrimitiveType() == Mesh::TRIANGLE_STRIP)) {
			unsigned int vertexCount = getVertexCount();
			for(unsigned int i = 0; i < vertexCount; i += 3) {
				GL_ASSERT( glDrawArrays(GL_LINE_LOOP, i, 3) );
			}
		}
		else {
			GL_ASSERT( glDrawArrays(getPrimitiveType(), 0, getVertexCount()) );
		}

		m_pVertexAttributeBinding->unbind();
		unbindTexture();
	}
	else {
		for(int i = 0; i < getMeshPartCount(); i++) {
			MeshPart* meshPart = getMeshPart(i);
			GP_ASSERT( meshPart );

			bindTexture();
			m_pVertexAttributeBinding->bind();
			
			GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshPart->getIndexBuffer()) );

			if(bWireframe && (getPrimitiveType() == Mesh::TRIANGLES || getPrimitiveType() == Mesh::TRIANGLE_STRIP)) {
				unsigned int indexCount = meshPart->getIndexCount();
				unsigned int indexSize = 0;
				switch(meshPart->getIndexFormat()) {
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
					//GP_ERROR("Unsupported index format (%d).", part->getIndexFormat());
					continue;
				}

				for (unsigned int k = 0; k < indexCount; k += 3) {
					GL_ASSERT( glDrawElements(GL_LINE_LOOP, 3, meshPart->getIndexFormat(), ((const GLvoid*)(k*indexSize))) );
				}
			}
			else {
				GL_ASSERT( glDrawElements(meshPart->getPrimitiveType(), meshPart->getIndexCount(), meshPart->getIndexFormat(), 0) );
			}

			GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
			m_pVertexAttributeBinding->unbind();
			unbindTexture();
		}
	}
#endif
}
///////////////////////////////////// TO BE PUT IN 'MODEL' /////////////////////////////////////

void Mesh::setTexture(const char* path, bool generateMipmaps) {
	GP_ASSERT( path );

	Texture* texture = Texture::createEx(path, generateMipmaps);
	GP_ASSERT( path );

	m_pTexture = texture;
}

void Mesh::bindTexture() {
	if(m_pTexture != NULL) {
		m_pTexture->bind();
	}
}

void Mesh::unbindTexture() {
	if(m_pTexture != NULL) {
		m_pTexture->unbind();
	}
}
