#include "ENGINE/Base.h"
#include "ENGINE/Image.h"
#include "ENGINE/Texture.h"

static std::vector<Texture*> __textureCache;
static GLuint __currentTextureID;

Texture::Texture() :	m_tHandle(0),
						m_Format(UNKNOWN),
						m_iWidth(0),
						m_iHeight(0),
						m_bMipmapped(false),
						m_bCached(false),
						m_bCompressed(false)
{
}

Texture::~Texture() {
	if(m_tHandle) {
		GL_ASSERT( glDeleteTextures(1, &m_tHandle) );
		m_tHandle = 0;
	}

	// Remove ourself from the texture cache.
	if(m_bCached) {
		std::vector<Texture*>::iterator itr = std::find(__textureCache.begin(), __textureCache.end(), this);
		if(itr != __textureCache.end()) {
			__textureCache.erase(itr);
		}
	}
}
		
Texture* Texture::create(const char* path, bool generateMipmaps) {
	GP_ASSERT(path);

	// Search texture cache first.
	for(size_t i = 0; i < __textureCache.size(); i++) {
		Texture* t = __textureCache[i];
		if(t->m_sPath == path) {
			// If 'generateMipmaps' is true, call Texture::generateMipamps() to force the 
			// texture to generate its mipmap chain if it hasn't already done so.
			if (generateMipmaps) {
				t->generateMipmaps();
			}
		}

		// Found a match.
		return t;
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
					Image* image = Image::createImage(path);
					if (image)
						texture = create(image, generateMipmaps);
					SAFE_DELETE(image);//SAFE_RELEASE(image);
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
	
	GP_ASSERT(image);

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

Texture* Texture::create(Format format, unsigned int width, unsigned int height, unsigned char* data, bool generateMipmaps) {
	// Create and load the texture.
	GLuint textureID;
	GL_ASSERT( glGenTextures(1, &textureID) );
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, textureID) );
	GL_ASSERT( glPixelStorei(GL_UNPACK_ALIGNMENT, 1) );
	GL_ASSERT( glTexImage2D(GL_TEXTURE_2D, 0, GLenum(format), width, height, 0, GLenum(format), GL_UNSIGNED_BYTE, data));

	// Set initial minification filter based on whether or not mipmaping was enabled.
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR) );

	Texture* texture = new Texture();
	texture->m_tHandle = textureID;
	texture->m_Format = format;
	texture->m_iWidth = width;
	texture->m_iHeight = height;
	if(generateMipmaps) {
		texture->generateMipmaps();
	}

	// Restore the texture id
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, __currentTextureID) );

	return texture;
}

Texture* Texture::create(TextureHandle handle, int width, int height, Format format) {
	GP_ASSERT(handle);

	Texture* texture = new Texture();
	texture->m_tHandle = handle;
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
	
}

bool Texture::isMipmapped() const {
	return m_bMipmapped;
}

bool Texture::isCompressed() const {
	return m_bCompressed;
}

GLuint Texture::getHandle() const {
	return m_tHandle;
}

void Texture::setWrapMode(Wrap wrapS, Wrap wrapT)
{
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, m_tHandle) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)wrapS) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)wrapT) );
}

void Texture::setFilterMode(Filter minificationFilter, Filter magnificationFilter)
{
	GL_ASSERT( glBindTexture(GL_TEXTURE_2D, m_tHandle) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)minificationFilter) );
	GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)magnificationFilter) );
}
