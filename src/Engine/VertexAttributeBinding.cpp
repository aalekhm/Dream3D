#include "Engine/VertexAttributeBinding.h"

static GLuint __maxVertexAttributes = 0;
static std::vector<VertexAttributeBinding*>	__vertexAttributeBindingCache;

VertexAttributeBinding::VertexAttributeBinding()
	:	m_hVAO(0),
		m_pAttributes(NULL),
		m_pMesh(NULL)
{

}

VertexAttributeBinding::~VertexAttributeBinding() {

	// Delete from the vertex attribute binding cache.
	std::vector<VertexAttributeBinding*>::iterator itr = std::find(__vertexAttributeBindingCache.begin(), __vertexAttributeBindingCache.end(), this);
	if(itr != __vertexAttributeBindingCache.end()) {
		__vertexAttributeBindingCache.erase(itr);
	}

	SAFE_DELETE(m_pMesh);//SAFE_RELEASE(m_pMesh);
	SAFE_DELETE_ARRAY(m_pAttributes);

	if(m_hVAO) {
		GL_ASSERT( glDeleteVertexArrays(1, &m_hVAO) );
		m_hVAO = 0;
	}
}

VertexAttributeBinding*	VertexAttributeBinding::create(Mesh* mesh/*, Effect* effect*/) {

	GP_ASSERT( mesh );

	//Search for an existing Vertex Attribute Binding that can be used
	VertexAttributeBinding* b;
	for(int i = 0, count = __vertexAttributeBindingCache.size(); i < count; i++) {
		b = __vertexAttributeBindingCache[i];
		GP_ASSERT( b );
		if(b->m_pMesh == mesh /*&& b->m_pEffect == effect*/) {
			
			//Found a match
			return b;
		}
	}

	b = create(mesh, mesh->getVertexFormat(), 0/*, effect*/);

	//Add the new Vertex Attribute Cache to the Cache
	if(b) {
		__vertexAttributeBindingCache.push_back(b);
	}

	return b;
}

VertexAttributeBinding*	VertexAttributeBinding::create(const VertexFormat& vertexFormat, void* vertexPointer/*, Effect* effect*/) {
	return create(NULL, vertexFormat, vertexPointer);
}

VertexAttributeBinding*	VertexAttributeBinding::create(Mesh* mesh, const VertexFormat& vertexFormat, void* vertexPointer/*, Effect* effect*/) {

	//GP_ASSERT( effect );

	//One time initialization
	if(__maxVertexAttributes == 0) {
		GLint temp;
		GL_ASSERT( glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &temp) );

		__maxVertexAttributes = temp;
		if(__maxVertexAttributes <= 0) {
			//GP_ERROR("The maximum number of vertex attributes supported by OpenGL on the current device is 0 or less.");
			return NULL;
		}
	}

	// Create a new VertexAttributeBinding.
	VertexAttributeBinding* b = new VertexAttributeBinding();

#ifdef USE_VAO
	if(mesh && glGenVertexArrays) {
		GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0) );
		GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
		
		// Use hardware VAOs.
		GL_ASSERT( glGenVertexArrays(1, &b->m_hVAO) );
		if(b->m_hVAO == 0) {
			//GP_ERROR("Failed to create VAO handle.");
			SAFE_DELETE(b);
			return NULL;
		}

		// Bind the new VAO
		GL_ASSERT( glBindVertexArray(b->m_hVAO) );

		// Bind the Mesh VBO so that out glVertexAttribPointer calls use it.
		GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexBuffer()) );
	}
	else
#endif
	{
		// Construct a software representation of a VAO.
		VertexAttribute* vertexAttribs = new VertexAttribute[__maxVertexAttributes];
		for(unsigned int i = 0; i < __maxVertexAttributes; i++) {
			//Set GL defauts
			vertexAttribs[i].m_bEnabled = false;
			vertexAttribs[i].m_iSize = 4;
			vertexAttribs[i].m_Type = GL_FLOAT;
			vertexAttribs[i].m_bNormalized = false;
			vertexAttribs[i].m_iStride = 0;
			vertexAttribs[i].m_pPointer = 0;
		}

		b->m_pAttributes = vertexAttribs;
	}

	if(mesh) {
		b->m_pMesh = mesh;

		// Bind the Mesh VBO so that out glVertexAttribPointer calls use it.
		GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexBuffer()) );
	}

	//b->_effect = effect;

	// Call setVertexAttribPointer for each vertex element.
	unsigned int offset = 0;
	for(int i = 0, count = vertexFormat.getElementCount(); i < count; i++) {

		const VertexFormat::Element& e = vertexFormat.getElement(i);
		::VertexAttribute attrib;

		switch(e.type) {
			case VertexFormat::POSITION:
				attrib = gl_Vertex;
			break;
			case VertexFormat::NORMAL:
				attrib = gl_Normal;
			break;
			case VertexFormat::COLOR:
				attrib = gl_Color;
			break;
			case VertexFormat::TANGENT:
			break;
			case VertexFormat::BINORMAL:
			break;
			case VertexFormat::BLENDWEIGHTS:
			break;
			case VertexFormat::BLENDINDICES:
			break;
			case VertexFormat::TEXCOORD0:
				attrib = gl_TexCoord;
			break;
			case VertexFormat::TEXCOORD1:
			case VertexFormat::TEXCOORD2:
			case VertexFormat::TEXCOORD3:
			case VertexFormat::TEXCOORD4:
			case VertexFormat::TEXCOORD5:
			case VertexFormat::TEXCOORD6:
			case VertexFormat::TEXCOORD7:
			break;
		}

		if( attrib == -1) {

		}
		else {
			void* pointer = vertexPointer ? (void*)((unsigned char*)vertexPointer + offset) : (void*)offset;
			b->setVertexAttributeBinding(attrib, (GLint)e.size, GL_FLOAT, false, (GLsizei)vertexFormat.getVertexSize(), pointer);
		}

		offset += e.size * sizeof(float);
	}

	if(mesh) {
		GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0) );
	}

	if(b->m_hVAO) {
		GL_ASSERT( glBindVertexArray(0) );
	}

	return b;
}

void VertexAttributeBinding::setVertexAttributeBinding(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer) {

	GP_ASSERT( index < (GLuint)__maxVertexAttributes);

	if(m_hVAO) {
		//Hardware mode
		GL_ASSERT( glVertexAttribPointer(index, size, type, normalized, stride, pointer) );
		GL_ASSERT( glEnableVertexAttribArray(index) );
	}
	else {
		//Software mode
		GP_ASSERT(m_pAttributes);
		m_pAttributes[index].m_bEnabled = true;
		m_pAttributes[index].m_iSize = size;
		m_pAttributes[index].m_Type = type;
		m_pAttributes[index].m_bNormalized = normalized;
		m_pAttributes[index].m_iStride = stride;
		m_pAttributes[index].m_pPointer = pointer;
	}
}

void VertexAttributeBinding::bind() {
	if(m_hVAO) {
		//Hardware mode
		GL_ASSERT( glBindVertexArray(m_hVAO) );
	}
	else {
		//Software mode
		if(m_pMesh) {
			GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->getVertexBuffer()) );
		}
		else {
			GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0) );
		}

#ifdef USE_VERTEX_POINTERS
		if(m_pMesh) {
			unsigned int offset = 0;
			const VertexFormat& vertexFormat = m_pMesh->getVertexFormat();
			for(int i = 0, count = vertexFormat.getElementCount(); i < count; i++) {
				const VertexFormat::Element& element = vertexFormat.getElement(i);

				switch(element.type) {
					case VertexFormat::POSITION:
						GL_ASSERT( glEnableClientState(GL_VERTEX_ARRAY) );
						GL_ASSERT( glVertexPointer(element.size, GL_FLOAT, vertexFormat.getVertexSize(), (void*)(sizeof(float) * offset)) );
						break;
					case VertexFormat::NORMAL:
						break;
					case VertexFormat::COLOR:
						GL_ASSERT( glEnableClientState(GL_COLOR_ARRAY) );
						GL_ASSERT( glColorPointer(element.size, GL_FLOAT, vertexFormat.getVertexSize(), (void*)(sizeof(float) * offset)) );
						break;
					case VertexFormat::TANGENT:
						break;
					case VertexFormat::BINORMAL:
						break;
					case VertexFormat::BLENDWEIGHTS:
						break;
					case VertexFormat::BLENDINDICES:
						break;
					case VertexFormat::TEXCOORD0:
						GL_ASSERT( glEnableClientState(GL_TEXTURE_COORD_ARRAY) );
						GL_ASSERT( glTexCoordPointer(element.size, GL_FLOAT, vertexFormat.getVertexSize(), (void*)(sizeof(float) * offset)) );
						break;
					case VertexFormat::TEXCOORD1:
						break;
					case VertexFormat::TEXCOORD2:
						break;
					case VertexFormat::TEXCOORD3:
						break;
					case VertexFormat::TEXCOORD4:
						break;
					case VertexFormat::TEXCOORD5:
						break;
					case VertexFormat::TEXCOORD6:
						break;
					case VertexFormat::TEXCOORD7:
						break;
				}

				offset += element.size;
			}
		}
		else {
			GL_ASSERT( m_pAttributes );
			for(unsigned int i = 0; i < __maxVertexAttributes; i++) {
				VertexAttribute& a = m_pAttributes[i];
				if(a.m_bEnabled) {
					GL_ASSERT( glVertexAttribPointer(i, a.m_iSize, a.m_Type, a.m_bNormalized, a.m_iStride, a.m_pPointer) );
					GL_ASSERT( glEnableVertexAttribArray(i) );
				}
			}
		}
#else
		GL_ASSERT( m_pAttributes );
		for(unsigned int i = 0; i < __maxVertexAttributes; i++) {
			VertexAttribute& a = m_pAttributes[i];
			if(a.m_bEnabled) {
				GL_ASSERT( glVertexAttribPointer(i, a.m_iSize, a.m_Type, a.m_bNormalized, a.m_iStride, a.m_pPointer) );
				GL_ASSERT( glEnableVertexAttribArray(i) );
			}
		}
#endif
	}
}

void VertexAttributeBinding::unbind() {
	if(m_hVAO) {
		//Hardware mode
		GL_ASSERT( glBindVertexArray(0) );
	}
	else {
		//Software mode
		if(m_pMesh) {
			GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0) );
		}

#ifdef USE_VERTEX_POINTERS
		if(m_pMesh) {
			const VertexFormat& vertexFormat = m_pMesh->getVertexFormat();
			for(int i = 0, count = vertexFormat.getElementCount(); i < count; i++) {
				const VertexFormat::Element& element = vertexFormat.getElement(i);

				switch(element.type) {
					case VertexFormat::POSITION:
						GL_ASSERT( glDisableClientState(GL_VERTEX_ARRAY) );
						break;
					case VertexFormat::NORMAL:
						break;
					case VertexFormat::COLOR:
						GL_ASSERT( glDisableClientState(GL_COLOR_ARRAY) );
						break;
					case VertexFormat::TANGENT:
						break;
					case VertexFormat::BINORMAL:
						break;
					case VertexFormat::BLENDWEIGHTS:
						break;
					case VertexFormat::BLENDINDICES:
						break;
					case VertexFormat::TEXCOORD0:
						GL_ASSERT( glDisableClientState(GL_TEXTURE_COORD_ARRAY) );
						break;
					case VertexFormat::TEXCOORD1:
						break;
					case VertexFormat::TEXCOORD2:
						break;
					case VertexFormat::TEXCOORD3:
						break;
					case VertexFormat::TEXCOORD4:
						break;
					case VertexFormat::TEXCOORD5:
						break;
					case VertexFormat::TEXCOORD6:
						break;
					case VertexFormat::TEXCOORD7:
						break;
				}
			}
		}
		else {
			GL_ASSERT( m_pAttributes );
			for(unsigned int i = 0; i < __maxVertexAttributes; i++) {
				if(m_pAttributes[i].m_bEnabled) {
					GL_ASSERT( glDisableVertexAttribArray(i) );
				}
			}
		}
#else
		GL_ASSERT( m_pAttributes );
		for(unsigned int i = 0; i < __maxVertexAttributes; i++) {
			if(m_pAttributes[i].m_bEnabled) {
				GL_ASSERT( glDisableVertexAttribArray(i) );
			}
		}
#endif
	}
}
