#ifndef VERTEXATTRIBUTEBINDING_H
#define VERTEXATTRIBUTEBINDING_H

#include "Engine/Base.h"
#include "Engine/VertexFormat.h"
#include "Engine/Mesh.h"

class Effect;
class VertexAttributeBinding {
	
	public:
		static VertexAttributeBinding*	create(Mesh* mesh, Effect* pEffect);
		static VertexAttributeBinding*	create(const VertexFormat& vertexFormat, void* vertexPointer, Effect* pEffect);

		void							bind();
		void							unbind();

		virtual ~VertexAttributeBinding();
	private:
		class VertexAttribute {
			public:
				bool			m_bEnabled;
				int				m_iSize;
				GLenum			m_Type;
				GLboolean		m_bNormalized;
				unsigned int	m_iStride;
				void*			m_pPointer;
		};

		VertexAttributeBinding();
		static VertexAttributeBinding*	create(Mesh* mesh, const VertexFormat& vertexFormat, void* vertexPointer, Effect* pEffect);
		void setVertexAttributeBinding(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer);

#ifdef USE_VAO
		VAOHandle			m_hVAO;
#endif
		VertexAttribute*	m_pAttributes;
		Mesh*				m_pMesh;
		Effect*				m_pEffect;
};

#endif