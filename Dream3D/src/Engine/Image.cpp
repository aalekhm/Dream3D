#include "Engine/Image.h"

Image::Image() {
	// Unused
}

Image* Image::createImage(const char* sTexWithPath) {
	TGAImg tgaImg;

	// Load our Texture
	if(tgaImg.Load((char*)sTexWithPath) != IMG_OK)
		return false;
	
	Image* image = new Image();
	image->m_iWidth = tgaImg.GetWidth();
	image->m_iHeight = tgaImg.GetHeight();

	unsigned int size = 0;
	switch (tgaImg.GetBPP())
	{
		case 32:
			image->m_Format = Image::RGBA;
			size = image->m_iWidth * image->m_iHeight * 4;
		break;
		case 24:
			image->m_Format = Image::RGB;
			size = image->m_iWidth * image->m_iHeight * 3;
		break;
	}

	image->m_pPixelData = new unsigned char[size];
	_memccpy(image->m_pPixelData, tgaImg.GetImg(), sizeof(unsigned char), size);

	return image;
}

unsigned char* Image::getPixelData() const {
	return m_pPixelData;
}

Image::Format Image::getFormat()  const {
	return m_Format;
}
unsigned int Image::getWidth()  const {
	return m_iWidth;
}

unsigned int Image::getHeight()  const {
	return m_iHeight;
}

Image::~Image() {
	SAFE_DELETE_ARRAY(m_pPixelData);
}