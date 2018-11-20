#ifndef IMAGE_H
#define IMAGE_H

#include "Engine/Base.h"
#include "Engine/TGA.h"

class Image {

	public:
		enum Format
		{
			RGB,
			RGBA
		};
		
		~Image();
		static Image*	createImage(const char* sTexWithPath);
		unsigned char*	getPixelData() const;
		Format			getFormat() const;
		unsigned int	getWidth() const;
		unsigned int	getHeight() const;
		
	private:
		Image();
		
		Image& operator=(const Image&);
		unsigned char*	m_pPixelData;
		Format			m_Format;
		unsigned int	m_iWidth;
		unsigned int	m_iHeight;
};

#endif