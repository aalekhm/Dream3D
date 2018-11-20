#ifndef TEXTURE_H
#define TEXTURE_H

class Image;

class Texture {
	friend class Sampler;

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
			CLAMP = GL_CLAMP,
			CLAMP_TO_EGDE = GL_CLAMP_TO_EDGE
		};

		/**
		* Destructor.
		*/
		virtual ~Texture();

		static Texture* create(const char* path, bool generateMipmaps = false);
		static Texture* createEx(const char* path, bool generateMipmaps = false);
		static Texture* create(Image* image, bool generateMipmaps = false);
		static Texture* create(Format format, unsigned int width, unsigned int height, unsigned char* data, bool generateMipmaps = false);
		static Texture* create(GLuint handle, int width, int height, Format format = UNKNOWN);

		static Texture*	createTGA(const char* path, bool generateMipmaps);

		Format getFormat() const;
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		void generateMipmaps();
		bool isMipmapped() const;
		bool isCompressed() const;
		GLuint getHandle() const;

		void setWrapMode(Wrap wrapS, Wrap wrapT);
		void setFilterMode(Filter minificationFilter, Filter magnificationFilter);

		void bind();
		void unbind();
		GLuint			m_hTexture;

		/**
		 * Defines a texture sampler.
		 *
		 * A texture sampler is basically an instance of a texture that can be
		 * used to sample a texture from a material. In addition to the texture
		 * itself, a sampler stores per-instance texture state information, such
		 * as wrap and filter modes.
		 */
		class Sampler {
			friend class Texture;

			public:
				virtual ~Sampler();
				static Sampler* create(Texture* texture);
				static Sampler* create(const char* sPath, bool bGenerateMipmaps = false);
				void			setWrapMode(Wrap wrapS, Wrap wrapT);
				void			setFilterMode(Filter minificationFilter, Filter magnificationFilter);
				Texture*		getTexture() const;
				void			bind();
			private:
				Sampler(Texture* texture);

				Texture*	m_pTexture;
				Wrap		m_WrapS;
				Wrap		m_WrapT;
				Filter		m_minFilter;
				Filter		m_magFilter;
		};
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
		* Hidden copy assignment operator.
		*/
		Texture& operator=(const Texture&);
		
		std::string		m_sPath;
		
		Format			m_Format;
		unsigned int	m_iWidth;
		unsigned int	m_iHeight;
		bool			m_bMipmapped;
		bool			m_bCached;
		bool			m_bCompressed;
};

#endif