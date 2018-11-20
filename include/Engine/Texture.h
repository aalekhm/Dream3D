#ifndef TEXTURE_H
#define TEXTURE_H

class Texture {

	public:
		/**
		 * Defines the set of supported texture formats.
		 */
		enum Format
		{
			UNKNOWN = 0,
			RGB     = GL_RGB,
			RGBA    = GL_RGBA,
			ALPHA   = GL_ALPHA
		};

		/**
		 * Defines the set of supported texture filters.
		 */
		enum Filter
		{
			NEAREST = GL_NEAREST,
			LINEAR = GL_LINEAR,
			NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
			LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
			NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
			LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
		};

		/**
		 * Defines the set of supported texture wrapping modes.
		 */
		enum Wrap
		{
			REPEAT = GL_REPEAT,
			CLAMP = GL_CLAMP
		};

		static Texture* create(const char* path, bool generateMipmaps = false);
		static Texture* create(Image* image, bool generateMipmaps = false);
		static Texture* create(Format format, unsigned int width, unsigned int height, unsigned char* data, bool generateMipmaps = false);
		static Texture* create(GLuint handle, int width, int height, Format format = UNKNOWN);
		Format getFormat() const;
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		void generateMipmaps();
		bool isMipmapped() const;
		bool isCompressed() const;
		GLuint getHandle() const;

		void setWrapMode(Wrap wrapS, Wrap wrapT);
		void setFilterMode(Filter minificationFilter, Filter magnificationFilter);

	private:
		/**
		* Constructor.
		*/
		Texture();

		/**
		* Copy constructor.
		*/
		//Texture(const &Texture copy);

		/**
		* Destructor.
		*/
		virtual ~Texture();

		/**
		* Hidden copy assignment operator.
		*/
		Texture& operator=(const Texture&);
		
		std::string		m_sPath;
		GLuint			m_tHandle;
		Format			m_Format;
		unsigned int	m_iWidth;
		unsigned int	m_iHeight;
		bool			m_bMipmapped;
		bool			m_bCached;
		bool			m_bCompressed;
};

#endif