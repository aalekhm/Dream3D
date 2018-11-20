#ifndef MODEL_H
#define MODEL_H

#include "Engine/Base.h"

class Mesh;
class MeshPart;
class VertexAttributeBinding;
class Texture;

class Model {

	public:
		static Model*	create(Mesh* pMesh);
		Mesh*			getMesh() const;
		unsigned int	getMeshPartCount() const;
		//Node*			getNode() const;
		void			draw(bool bWireFrame = false);

		void			setVertexAttributeBinding(VertexAttributeBinding* vaBinding);
		void			setTexture(const char* path, bool generateMipmaps = false);
		void			bindTexture();
		void			unbindTexture();
	private:
		Model(Mesh* pMesh);
		virtual ~Model();

		Mesh*					m_pMesh;

		VertexAttributeBinding*	m_pVertexAttributeBinding;
		Texture*				m_pTexture;

};

#endif