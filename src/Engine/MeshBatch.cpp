#include "Engine/MeshBatch.h"
#include "Engine/VertexAttributeBinding.h"
#include "Engine/Texture.h"
#include "Engine/Material.h"
#include "Engine/Technique.h"

MeshBatch::MeshBatch(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, Material* pMaterial, bool bIndexed, unsigned int iInitialCapacity, unsigned int iGrowSize) 
	: m_VertexFormat(vertexFormat),
	  m_PrimitiveType(primitiveType),
	  m_pMaterial(pMaterial),
	  m_bIndexed(bIndexed),
	  m_iCapacity(0),
	  m_iGrowSize(iGrowSize),
	  m_iVertexCapacity(0),
	  m_iIndexCapacity(0),
	  m_iVertexCount(0),
	  m_iIndexCount(0),
	  m_pVertices(NULL),
	  m_pVerticesPtr(NULL),
	  m_pIndices(NULL),
	  m_pIndicesPtr(NULL),

	  m_pVertexAttributeBinding(NULL),
	  m_pTexture(NULL)
{
	resize(iInitialCapacity);
}

MeshBatch::~MeshBatch() {

	SAFE_DELETE_ARRAY(m_pVertices);
	SAFE_DELETE_ARRAY(m_pIndices);
}

MeshBatch* MeshBatch::create(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, const char* materialUrl, bool bIndexed, unsigned int iInitialCapacity, unsigned int iGrowSize) {

	Material* pMaterial = Material::create(materialUrl);
	if (pMaterial == NULL) {

		GP_ERROR("Failed to create material for mesh batch from file %d", materialUrl);
		return NULL;
	}

	MeshBatch* meshBatch = new MeshBatch(vertexFormat, primitiveType, pMaterial, bIndexed, iInitialCapacity, iGrowSize);
	return meshBatch;
}

MeshBatch* MeshBatch::create(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, Material* pMaterial,  bool bIndexed, unsigned int iInitialCapacity, unsigned int iGrowSize) {

	MeshBatch* meshBatch = new MeshBatch(vertexFormat, primitiveType, pMaterial, bIndexed, iInitialCapacity, iGrowSize);
	return meshBatch;
}

const VertexFormat& MeshBatch::getVertexFormat() const {
	return m_VertexFormat;
}

unsigned int MeshBatch::getCapacity() const {
	return m_iCapacity;
}

void MeshBatch::setCapacity(unsigned int iCapacity) {
	resize(iCapacity);
}

bool MeshBatch::resize(unsigned int iCapacity) {

	if(iCapacity <= 0) {
		//GP_ERROR("Invalid capacity");
		return false;
	}

	if(iCapacity == m_iCapacity)
		return true;

	// Store old batch data.
	unsigned char* oldVertices = m_pVertices;
	unsigned short* oldIndices = m_pIndices;

	unsigned int vertexCapacity = 0;
	switch(m_PrimitiveType) {
		case Mesh::LINES:
			vertexCapacity = iCapacity * 2;
		break;
		case Mesh::LINE_STRIP:
			vertexCapacity = iCapacity + 1;
		break;
		case Mesh::TRIANGLES:
			vertexCapacity = iCapacity * 3;
		break;
		case Mesh::TRIANGLE_STRIP:
			vertexCapacity = iCapacity + 2;
		break;
		case Mesh::POINTS:
			vertexCapacity = iCapacity;
		break;
		default:
			//GP_ERROR("Invalid Primitive Type (%d).", m_PrimitiveType);
			return false;
	}

	// We have no way of knowing how many vertices will be stored in the batch
	// (we only know how many indices will be stored). Assume the worst case
	// for now, which is the same number of vertices as indices.
	unsigned int indexCapacity = vertexCapacity;
	if(m_bIndexed && indexCapacity > USHRT_MAX) {
		//GP_ERROR("Index capacity is greater than the maximum unsigned short value (%d > %d).", indexCapacity, USHRT_MAX);
		return false;
	}

	// Allocate new data and reset pointers.
	unsigned int iVSize = vertexCapacity * m_VertexFormat.getVertexSize();
	m_pVertices = new unsigned char[iVSize];

	if(m_bIndexed) {
		m_pIndices = new unsigned short[indexCapacity];
	}

	// Copy old data back in
	if(oldVertices) {
		memcpy(m_pVertices, oldVertices, std::min(vertexCapacity, m_iVertexCapacity) * m_VertexFormat.getVertexSize());
		SAFE_DELETE_ARRAY(oldVertices);
	}

	if(oldIndices) {
		memcpy(m_pIndices, oldIndices, std::min(indexCapacity, m_iIndexCapacity));
		SAFE_DELETE_ARRAY(oldIndices);
	}

	// Assign new capacities
	m_iCapacity = iCapacity;
	m_iVertexCapacity = vertexCapacity;
	m_iIndexCapacity = indexCapacity;

	// Update our vertex attribute bindings now that our client array pointers have changed
	updateVertexAttributeBinding();

	return true;
}

void MeshBatch::updateVertexAttributeBinding() {

	GP_ASSERT( m_pMaterial );

	// Update our vertex attribute bindings.
	for (unsigned int i = 0, iTechniqueCount = m_pMaterial->getTechniqueCount(); i < iTechniqueCount; i++) {

		Technique* pTechnique = m_pMaterial->getTechniqueByIndex(i);
		GP_ASSERT( pTechnique );

		for (unsigned int j = 0, iPassCount = pTechnique->getPassCount(); j < iPassCount; j++) {

			Pass* pPass = pTechnique->getPassByIndex(j);
			GP_ASSERT( pPass );

			VertexAttributeBinding* pVAB = VertexAttributeBinding::create(m_VertexFormat, m_pVertices, pPass->getEffect());
			pPass->setVertexAttributeBinding(pVAB);
		}
	}
}

void MeshBatch::setVertexAttributeBinding(VertexAttributeBinding* vaBinding) {
	GL_ASSERT( vaBinding );
	m_pVertexAttributeBinding = vaBinding;
}

Material* MeshBatch::getMaterial() const {

	return m_pMaterial;
}

void MeshBatch::setTexture(const char* path, bool generateMipmaps) {
	GP_ASSERT( path );

	Texture* texture = Texture::createEx(path, generateMipmaps);
	setTexture(texture);
}

void MeshBatch::setTexture(Texture* pTexture, bool generateMipmaps) {
	GP_ASSERT( pTexture );

	m_pTexture = pTexture;
}

Texture* MeshBatch::getTexture() {
	GP_ASSERT( m_pTexture );

	if(m_pTexture)
		return m_pTexture;

	return NULL;
}

void MeshBatch::bindTexture() {
	if(m_pTexture != NULL) {
		m_pTexture->bind();
	}
}

void MeshBatch::unbindTexture() {
	if(m_pTexture != NULL) {
		m_pTexture->unbind();
	}
}

void MeshBatch::start() {
	m_iVertexCount = 0;
	m_iIndexCount = 0;
	m_pVerticesPtr = m_pVertices;
	m_pIndicesPtr = m_pIndices;
}

void MeshBatch::stop() {

}

void MeshBatch::render() {
	
	if(m_iVertexCount == 0 || (m_bIndexed && m_iIndexCount == 0))
		return;// Nothing to draw

	// Not using VBOs, so unbind the element array buffer.
	// ARRAY_BUFFER will be unbound automatically during pass->bind().
	GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 ) );

	if (m_bIndexed)
		GP_ASSERT(m_pIndices);

	// Bind the material
	Technique* pTechnique = m_pMaterial->getTechnique();
	GP_ASSERT( pTechnique );

	unsigned int iPassCount = pTechnique->getPassCount();
	for (unsigned int i = 0; i < iPassCount; i++) {

		Pass* pPass = pTechnique->getPassByIndex(i);
		GP_ASSERT( pPass );

		pPass->bind();

		if(m_bIndexed) {
			GL_ASSERT( glDrawElements(m_PrimitiveType, m_iIndexCount, GL_UNSIGNED_SHORT, (GLvoid*)m_pIndices) );
		}
		else {
			GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
			GL_ASSERT( glDrawArrays(m_PrimitiveType, 0, m_iVertexCount) );
		}

		pPass->unbind();
	}
}
