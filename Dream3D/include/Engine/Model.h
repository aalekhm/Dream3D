#ifndef MODEL_H
#define MODEL_H

#include "Engine/Base.h"

class Mesh;
class MeshPart;
class VertexAttributeBinding;
class Texture;
class Node;
class Material;

class Model {

	public:
		virtual ~Model();

		static Model*	create(Mesh* pMesh);
		Mesh*			getMesh() const;
		unsigned int	getMeshPartCount() const;
		//Node*			getNode() const;
		void			draw(bool bWireFrame = false);

		void			setVertexAttributeBinding(VertexAttributeBinding* vaBinding);
		void			setTexture(const char* path, bool generateMipmaps = false);
		void			bindTexture();
		void			unbindTexture();

		void			setNode(Node* node);
		Material*	setMaterial(const char* sMaterialPath, int iPartIndex = -1);
		void			setMaterial(Material* pMaterial, int iPartIndex = -1);
		Material*	getMaterial(int iPartIndex = -1);
		void			setMaterialNodeBinding(Material* pMaterial);
	private:
		Model(Mesh* pMesh);
		
		Mesh*					m_pMesh;

		VertexAttributeBinding*	m_pVertexAttributeBinding;
		Texture*				m_pTexture;

		Node*					m_pNode;
		unsigned int			m_iPartCount;
		Material*				m_pMaterial;
		Material**				m_pPartMaterials;
};

#endif