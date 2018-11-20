#include "ENGINE/Base.h"
#include "ENGINE/Image.h"
#include "ENGINE/Texture.h"

static std::vector<Texture*> __textureCache;
static GLuint __currentTextureID;

Texture::Texture() :	m_hTexture(0),
						m_Format(UNKNOWN),
						m_iWidth(0),
						m_iHeight(0),
						m_bMipmapped(false),
						m_bCached(false),
						m_bCompressed(false)
{
}

Texture::~Texture() {
	if(m_hTexture) {
		GL_ASSERT( glDeleteTextures(1, &m_hTexture) );
		m_hTexture = 0;
	}

	// Remove ourself from the texture cache.
	if(m_bCached) {
		if(__textureCache.size() > 0) {
			std::vector<Texture*>::iterator itr = std::find(__textureCache.begin(), __textureCache.end(), this);		
			if(itr != __textureCache.end()) {
				__textureCache.erase(itr);
			}
		}
	}
}

Texture* Texture::createEx(const char* path, bool generateMipmaps) {
	GP_ASSERT( path );

	// Search texture cache first.
	for(size_t i = 0; i < __textureCache.size(); i++) {
		Texture* t = __textureCache[i];
		if(strcmp(t->m_sPath.c_str(), path) == 0) {
			// If 'generateMipmaps' is true, call Texture::generateMipamps() to force the 
			// texture to generate its mipmap chain if it hasn't already done so.
			if (generateMipmaps) {
				t->generateMipmaps();
			}

			// Found a match.
			return t;
		}
	}

	Texture* texture = NULL;

	// Filter loading based on file extension.
	const char* ext = strrchr(path, '.');
	if(ext) {
		switch(strlen(ext)) {
		case 4:
			if (tolower(ext[1]) == 'p' && tolower(ext[2]) == 'n' && tolower(ext[3]) == 'g') {
			}
			else 
			if (tolower(ext[1]) == 't' && tolower(ext[2]) == 'g' && tolower(ext[3]) == 'a') {
				texture = createTGA(path, generateMipmaps);
			}
			else 
			if (tolower(ext[1]) == 'p' && tolower(ext[2]) == 'v' && tolower(ext[3]) == 'r') {
				// PowerVR Compressed Texture RGBA.
				//texture = createCompressedPVRTC(path);
			}
			else 
			if (tolower(ext[1]) == 'd' && tolower(ext[2]) == 'd' && tolower(ext[3]) == 's') {
				// DDS file format (DXT/S3TC) compressed textures
				//texture = createCompressedDDS(path);
			}
			break;
		}
	}

	if(texture) {
		texture->m_sPath = path;
		texture->m_bCached = true;

		// Add to texture cache.
		__textureCache.push_back(texture);

		return texture;
	}

	//GP_ERROR("Failed to load texture from file '%s'.", path);
	return NULL;
}

Texture* Texture::createTGA(const char* path, bool generateMipmaps) {

	// Image loader
	TGAImg Img;
	Texture* texture = NULL;

	// Load our Texture
	if(Img.Load((char*)path) != IMG_OK)
		return texture;

	// Create and load the texture.
	GLuint textureID;
	GL_ASSERT( glGenTextures(1, &textureID) );
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, textureID) ); // Set our Tex handle as current

	// Specify filtering and edge actions
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) );

	Texture::Format format = Texture::UNKNOWN;
	
	if(Img.GetBPP() == 24)
		format = Texture::RGB;
	else 
	if(Img.GetBPP() == 32)
		format = Texture::RGBA;
	else
		return texture;

	// Create the texture
	GL_ASSERT( glTexImage2D(GL_TEXTURE_2D, 0, (GLuint)format, Img.GetWidth(), Img.GetHeight(), 0, (GLuint)format, GL_UNSIGNED_BYTE, Img.GetImg()) );
	
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, 0) );
	GL_ASSERT( glDisable(GL_TEXTURE_2D) );

	texture = new Texture();
	texture->m_hTexture = textureID;
	texture->m_Format = format;
	texture->m_iWidth = Img.GetWidth();
	texture->m_iHeight = Img.GetHeight();
	texture->m_eType = TEXTURE_2D;
	if(generateMipmaps) {
		texture->generateMipmaps();
	}

	return texture;
}

Texture* Texture::create(const char* path, bool generateMipmaps) {
	GP_ASSERT( path );

	// Search texture cache first.
	for(size_t i = 0; i < __textureCache.size(); i++) {
		Texture* t = __textureCache[i];
		if(strcmp(t->m_sPath.c_str(), path) == 0) {
			// If 'generateMipmaps' is true, call Texture::generateMipamps() to force the 
			// texture to generate its mipmap chain if it hasn't already done so.
			if (generateMipmaps) {
				t->generateMipmaps();
			}

			// Found a match.
			return t;
		}
	}

	Texture* texture = NULL;
	
	// Filter loading based on file extension.
	const char* ext = strrchr(path, '.');
	if(ext) {
		switch(strlen(ext)) {
			case 4:
				if (	//(tolower(ext[1]) == 'p' && tolower(ext[2]) == 'n' && tolower(ext[3]) == 'g')
						//||
						(tolower(ext[1]) == 't' && tolower(ext[2]) == 'g' && tolower(ext[3]) == 'a')
				) {
					//Image* image = Image::createImage(path);
					//if (image)
					//	texture = create(image, generateMipmaps);
					//SAFE_DELETE(image);//SAFE_RELEASE(image);

					texture = createTGA(path, generateMipmaps);
				}
				else if (tolower(ext[1]) == 'p' && tolower(ext[2]) == 'v' && tolower(ext[3]) == 'r') {
					// PowerVR Compressed Texture RGBA.
					//texture = createCompressedPVRTC(path);
				}
				else if (tolower(ext[1]) == 'd' && tolower(ext[2]) == 'd' && tolower(ext[3]) == 's') {
					// DDS file format (DXT/S3TC) compressed textures
					//texture = createCompressedDDS(path);
				}
			break;
		}
	}

	if(texture) {
		texture->m_sPath = path;
		texture->m_bCached = true;

		// Add to texture cache.
		__textureCache.push_back(texture);

		return texture;
	}

	//GP_ERROR("Failed to load texture from file '%s'.", path);
	return NULL;
}

Texture* Texture::create(Image* image, bool generateMipmaps) {
	
	GP_ASSERT( image );

	switch(image->getFormat()) {
		case Image::RGB:
			return create(Texture::RGB, image->getWidth(), image->getHeight(), image->getPixelData(), generateMipmaps);
		break;
		case Image::RGBA:
			return create(Texture::RGBA, image->getWidth(), image->getHeight(), image->getPixelData(), generateMipmaps);
		break;
		default:
			//GP_ERROR("Unsupported image format (%d).", image->getFormat());
			return NULL;
	}
}

Texture* Texture::create(Format format, unsigned int width, unsigned int height, unsigned char* data, bool generateMipmaps, Type type) {
	// Create and load the texture.
	GLuint textureID;

	// OpenGL keeps a track of loaded textures by numbering them: the first one you load is 1, second is 2, ...and so on.
	GL_ASSERT( glGenTextures(1, &textureID) );

	// Binding the texture to GL_TEXTURE_2D is like telling OpenGL that the texture with this ID is now the current 2D texture in use
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, textureID) );

	// Specify filtering and edge actions
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S, GL_CLAMP) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_T, GL_CLAMP) );

	//GL_ASSERT( glPixelStorei(GL_UNPACK_ALIGNMENT, 1) );

	// This call will actually load the image data into OpenGL and your video card's memory. 
	// The texture is always loaded into the current texture you have selected with the last glBindTexture call
	// It asks for the width, height, type of image (determines the format of the data you are giving to it) and the pointer to the actual data
	GL_ASSERT( glTexImage2D(GL_TEXTURE_2D, 0, GLenum(format), width, height, 0, GLenum(format), GL_UNSIGNED_BYTE, data));

	// Specify filtering and edge actions
	// Set initial minification filter based on whether or not mipmaping was enabled.
	//GL_ASSERT( glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, generateMipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR) );
	GL_ASSERT( glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR) );

	Texture* texture = new Texture();
	texture->m_hTexture = textureID;
	texture->m_Format = format;
	texture->m_iWidth = width;
	texture->m_iHeight = height;
	texture->m_eType = type;
	if(generateMipmaps) {
		texture->generateMipmaps();
	}

	// Restore the texture id
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, __currentTextureID) );
	GL_ASSERT( glDisable(GL_TEXTURE_2D) );

	return texture;
}

Texture* Texture::create(TextureHandle handle, int width, int height, Format format) {
	GP_ASSERT( handle );

	Texture* texture = new Texture();
	texture->m_hTexture = handle;
	texture->m_Format = format;
	texture->m_iWidth = width;
	texture->m_iHeight = height;
	
	return texture;
}

Texture::Format Texture::getFormat() const {
	return m_Format;
}

unsigned int Texture::getWidth() const {
	return m_iWidth;
}

unsigned int Texture::getHeight() const {
	return m_iHeight;
}

void Texture::generateMipmaps() {
	if(!m_bMipmapped) {
		GL_ASSERT( glBindTexture(GL_TEXTURE_2D, m_hTexture) );
		GL_ASSERT( glGenerateMipmap(GL_TEXTURE_2D) );

		m_bMipmapped = true;
	}
}

bool Texture::isMipmapped() const {
	return m_bMipmapped;
}

bool Texture::isCompressed() const {
	return m_bCompressed;
}

GLuint Texture::getHandle() const {
	return m_hTexture;
}

Texture::Type Texture::getType() const {

	return m_eType;
}

void Texture::setWrapMode(Wrap wrapS, Wrap wrapT)
{
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, m_hTexture) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)wrapS) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)wrapT) );
}

void Texture::setFilterMode(Filter minificationFilter, Filter magnificationFilter)
{
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, m_hTexture) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)minificationFilter) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)magnificationFilter) );
}

void Texture::bind() {
	GP_ASSERT( m_hTexture );

	GL_ASSERT( glEnable(GL_TEXTURE_2D) );
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, m_hTexture) );
}

void Texture::unbind() {
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, 0) );
	GL_ASSERT( glDisable(GL_TEXTURE_2D) );
}

Texture::Sampler::Sampler(Texture* pTexture) 
	:	m_pTexture(pTexture),
		m_WrapS(Texture::REPEAT),
		m_WrapT(Texture::REPEAT),
		m_magFilter(Texture::LINEAR)
{
	GP_ASSERT( pTexture );
	m_minFilter = pTexture->isMipmapped() ? Texture::NEAREST_MIPMAP_LINEAR : Texture::LINEAR;
}

Texture::Sampler::~Sampler() {
	SAFE_DELETE( m_pTexture );
}

Texture::Sampler* Texture::Sampler::create(Texture* pTexture) {

	GP_ASSERT( pTexture );
	return new Sampler(pTexture);
}

Texture::Sampler* Texture::Sampler::create(const char* sPath, bool bGenerateMipmaps) {

	Texture* pTexture = Texture::create(sPath, bGenerateMipmaps);
	return pTexture ? new Sampler(pTexture) : NULL;
}

void Texture::Sampler::setWrapMode(Wrap wrapS, Wrap wrapT) {
	m_WrapS = wrapS;
	m_WrapT = wrapT;
}

void Texture::Sampler::setFilterMode(Filter minificationFilter, Filter magnificationFilter) {
	m_minFilter = minificationFilter;
	m_magFilter = magnificationFilter;
}

Texture* Texture::Sampler::getTexture() const {
	return m_pTexture;
}

void Texture::Sampler::bind() {

	GP_ASSERT( m_pTexture );

	m_pTexture->bind();
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)m_WrapS) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)m_WrapT) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)m_minFilter) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)m_magFilter) );
}

void Texture::Sampler::unbind() {

	GP_ASSERT( m_pTexture );

	m_pTexture->unbind();
}