#include "Engine/Model.h"
#include "Engine/Material.h"
#include "Engine/Mesh.h"
#include "Engine/MeshPart.h"
#include "Engine/VertexAttributeBinding.h"
#include "Engine/Texture.h"
#include "Engine/Node.h"
#include "Engine/Technique.h"

Model::Model(Mesh* pMesh) 
:	m_pMesh(pMesh),
	//m_pTexture(NULL),
	m_pVertexAttributeBinding(NULL),
	m_pMaterial(NULL),
	m_pPartMaterials(NULL),
	m_pNode(NULL)
{
	GP_ASSERT( pMesh );
	m_iPartCount = pMesh->getMeshPartCount();

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
	if(getMeshPartCount() == 0) {// No mesh parts (i.e index buffers).
		if(m_pMaterial) {

			Technique* pTechnique = m_pMaterial->getTechnique();
			GP_ASSERT( pTechnique );

			unsigned int iPassCount = pTechnique->getPassCount();
			for(int i = 0; i < iPassCount; i++) {

				Pass* pPass = pTechnique->getPassByIndex(i);
				GP_ASSERT( pPass );

				pPass->bind();
				GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );

				if(	bWireframe 
					&& 
					(m_pMesh->getPrimitiveType() == Mesh::TRIANGLES || m_pMesh->getPrimitiveType() == Mesh::TRIANGLE_STRIP)
				) {
						unsigned int vertexCount = m_pMesh->getVertexCount();
						for(unsigned int i = 0; i < vertexCount; i += 3) {
							GL_ASSERT( glDrawArrays(GL_LINE_LOOP, i, 3) );
						}
				}
				else 
				{
					GL_ASSERT( glDrawArrays(m_pMesh->getPrimitiveType(), 0, m_pMesh->getVertexCount()) );
				}
				
				pPass->unbind();
			}
		}
	}
	else {
		for(int i = 0; i < getMeshPartCount(); i++) {
			MeshPart* meshPart = m_pMesh->getMeshPart(i);
			GP_ASSERT( meshPart );

			// Get the material for this mesh part.
			Material* pMaterial = getMaterial(i);
			if(pMaterial) {

				Technique* pTechnique = pMaterial->getTechnique();
				GP_ASSERT( pTechnique );

				unsigned int iPassCount = pTechnique->getPassCount();
				for(int i = 0; i < iPassCount; i++) {

					Pass* pPass = pTechnique->getPassByIndex(i);
					GP_ASSERT( pPass );
					
					pPass->bind();
					GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshPart->getIndexBuffer()) );

					if(bWireframe && (m_pMesh->getPrimitiveType() == Mesh::TRIANGLES || m_pMesh->getPrimitiveType() == Mesh::TRIANGLE_STRIP)) {
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

					pPass->unbind();
				}
			}
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

void Model::setNode(Node* node) {
	if(node != m_pNode) {
		m_pNode = node;
	}
}

void Model::setMaterialNodeBinding(Material* pMaterial) {

	GP_ASSERT( pMaterial );
	if(m_pNode) {
		pMaterial->setNodeBinding(m_pNode);

		unsigned int iTechniqueCount = pMaterial->getTechniqueCount();
		for (unsigned int i = 0; i < iTechniqueCount; ++i) {

			Technique* pTechnique = pMaterial->getTechniqueByIndex(i);
			GP_ASSERT( pTechnique );

			pTechnique->setNodeBinding(m_pNode);
			unsigned int iPassCount = pTechnique->getPassCount();
			for (unsigned int j = 0; j < iPassCount; ++j) {
				Pass* pPass = pTechnique->getPassByIndex(j);
				GP_ASSERT( pPass );
				pPass->setNodeBinding(m_pNode);
			}
		}
	}
}

void Model::setMaterial(Material* pMaterial, int iPartIndex) {

	GP_ASSERT( iPartIndex == -1 || (iPartIndex >= 0 && iPartIndex < (int)getMeshPartCount()) );

	Material* pOldMaterial = NULL;

	if(iPartIndex == -1) {
		
		pOldMaterial = pMaterial;

		//Set the new shared material
		if(pMaterial) {
			m_pMaterial = pMaterial;
			//m_pMaterial->addRef();
		}
	}
	else
	if(iPartIndex >= 0 && iPartIndex < (int)getMeshPartCount()) {

		// Release existing part material and part binding.
		if(m_pPartMaterials) {
			pOldMaterial = m_pPartMaterials[iPartIndex];
		}
		else {
			
			// Allocate part arrays for the first time.
			if(m_pPartMaterials == NULL) {
				m_pPartMaterials = new Material*[m_iPartCount];
				memset(m_pPartMaterials, 0, sizeof(Material*) * m_iPartCount);
			}
		}

		// Set new part material.
		if(pMaterial) {
			m_pPartMaterials[iPartIndex] = pMaterial;
			//pMaterial->addRef();
		}
		
		// Release existing material and binding.
		if(pOldMaterial) {

			for(unsigned int i = 0, iTCount = pOldMaterial->getTechniqueCount(); i < iTCount; i++) {
				
				Technique* pTechnique = pOldMaterial->getTechniqueByIndex(i);
				GP_ASSERT(pTechnique);
				for(unsigned int j = 0, iPCount = pTechnique->getPassCount(); j < iPCount; j++) {
					Pass* pPass = pTechnique->getPassByIndex(j);
					GP_ASSERT( pPass );
					pPass->setVertexAttributeBinding(NULL);
				}
			}
		}

		SAFE_DELETE( pOldMaterial );
	}

	if(pMaterial) {

		// Hookup vertex attribute bindings for all passes in the new material.
		for(unsigned int i = 0, iTCount = pMaterial->getTechniqueCount(); i < iTCount; i++) {

			Technique* pTechnique = pMaterial->getTechniqueByIndex(i);
			GP_ASSERT(pTechnique);
			for(unsigned int j = 0, iPCount = pTechnique->getPassCount(); j < iPCount; j++) {
				Pass* pPass = pTechnique->getPassByIndex(j);
				GP_ASSERT( pPass );
				VertexAttributeBinding* pVertexAttributeBinding = VertexAttributeBinding::create(m_pMesh);
				pPass->setVertexAttributeBinding(pVertexAttributeBinding);
				//SAFE_RELEASE(pVertexAttributeBinding);
			}
		}
	}

	// Apply node binding for the new material.
	if (m_pNode)
	{
		setMaterialNodeBinding(pMaterial);
	}
}

Material* Model::setMaterial(const char* sMaterialPath, int iPartIndex) {

	// Try to create a Material from the specified material file.	
	Material* pMaterial = Material::create(sMaterialPath);
	if(pMaterial == NULL) {

		GP_ERROR("Failed to create material for model.");
		return NULL;
	}

	// Assign the material to us
	setMaterial(pMaterial, iPartIndex);

	// Release the material since we now have a reference to it
	//SAFE_DELETE( pMaterial );

	return pMaterial;
}

Material* Model::getMaterial(int iPartIndex) {

	GP_ASSERT( iPartIndex == -1	|| (iPartIndex >= 0 && iPartIndex < (int)getMeshPartCount()) );

	Material* pMaterial = NULL;

	if(iPartIndex >= 0 && iPartIndex < (int)getMeshPartCount()) {

		// Look up explicitly specified part material.
		if(m_pPartMaterials) {
			pMaterial = m_pPartMaterials[iPartIndex];
		}

		if(pMaterial == NULL) {
			// Return the shared material.
			pMaterial = m_pMaterial;
		}
	}

	return pMaterial;
}

Model::~Model() {

	SAFE_DELETE( m_pTexture );
	SAFE_DELETE( m_pVertexAttributeBinding );
	SAFE_DELETE( m_pMesh );
}
