#include "Engine/Model.h"
#include "Engine/Mesh.h"
#include "Engine/MeshPart.h"
#include "Engine/VertexAttributeBinding.h"
#include "Engine/Texture.h"

Model::Model(Mesh* pMesh) 
:	m_pMesh(pMesh),
	m_pTexture(NULL),
	m_pVertexAttributeBinding(NULL)
{
	VertexAttributeBinding* vaBinding = VertexAttributeBinding::create(pMesh);
	
	GP_ASSERT( vaBinding );
	m_pVertexAttributeBinding = vaBinding;
}

Model* Model::create(Mesh* pMesh) {
	GP_ASSERT( pMesh );

	return new Model(pMesh);
}

Mesh* Model::getMesh() const {
	GP_ASSERT( m_pMesh );
	return m_pMesh;
}

unsigned int Model::getMeshPartCount() const {
	GP_ASSERT( m_pMesh );
	return m_pMesh->getMeshPartCount();
}

//Node* Model::getNode() const {
//
//}

void Model::setVertexAttributeBinding(VertexAttributeBinding* vaBinding) {
	GP_ASSERT( vaBinding );
	m_pVertexAttributeBinding = vaBinding;
}

void Model::draw(bool bWireframe) {
#ifdef USE_VERTEX_POINTERS
	m_pVertexAttributeBinding->bind();
	bindTexture();

	if(getMeshPartCount() == 0) {
		GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
		GL_ASSERT( glDrawArrays(m_pMesh->getPrimitiveType(), 0, m_pMesh->getVertexCount()) );
	}
	else {
		for(unsigned int i = 0; i < getMeshPartCount(); i++) {
			MeshPart* meshPart = m_pMesh->getMeshPart(i);
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
			MeshPart* meshPart = m_pMesh->getMeshPart(i);
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

void Model::setTexture(const char* path, bool generateMipmaps) {
	GP_ASSERT( path );

	Texture* texture = Texture::createEx(path, generateMipmaps);
	GP_ASSERT( path );

	m_pTexture = texture;
}

void Model::bindTexture() {
	if(m_pTexture != NULL) {
		m_pTexture->bind();
	}
}

void Model::unbindTexture() {
	if(m_pTexture != NULL) {
		m_pTexture->unbind();
	}
}

Model::~Model() {

	SAFE_DELETE( m_pTexture );
	SAFE_DELETE( m_pVertexAttributeBinding );
	SAFE_DELETE( m_pMesh );
}