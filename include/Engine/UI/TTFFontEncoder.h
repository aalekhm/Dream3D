#ifndef TTFFONTENCODER_H
#define TTFFONTENCODER_H

#include "Common/RandomAccessFile.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#define ASCII_START_INDEX	32
#define ASCII_END_INDEX		127
#define GLYPH_PADDING		4

class Glyph {

	private:
	public:
		unsigned int	index;
		unsigned int	width;
		unsigned int	height;
		unsigned int	uvCoords[4];

		void write(RandomAccessFile* rafOut) {
			rafOut->writeInt(index);
			rafOut->writeInt(width);
			rafOut->writeInt(height);

			rafOut->writeInt(uvCoords[0]);
			rafOut->writeInt(uvCoords[1]);
			rafOut->writeInt(uvCoords[2]);
			rafOut->writeInt(uvCoords[3]);
		}

		void read(RandomAccessFile* rafIn) {
			index = rafIn->readInt();
			width = rafIn->readInt();
			height = rafIn->readInt();

			uvCoords[0] = rafIn->readInt();
			uvCoords[1] = rafIn->readInt();
			uvCoords[2] = rafIn->readInt();
			uvCoords[3] = rafIn->readInt();
		}
};

struct Glyph_
{
	unsigned short	code;  // unicode value
	unsigned short	index; // freetype glyph index

	unsigned char		width;  // width of the glyph in pixels
	unsigned char		height; // height of the glyph in pixels

	unsigned char		advanceX; // number of pixels to advance on x axis
	unsigned char		advanceY; // number of pixels to advance on y axis
	
	unsigned char		bearingX; // x offset of top-left corner from x axis
	unsigned char		bearingY; // y offset of top-left corner from y axis

	unsigned char		textureX; // x pixel coord of the bitmap's bottom-left corner
	unsigned short	textureY;// y pixel coord of the bitmap's bottom-left corner
};

class TTFFontEncoder {

	public:
		TTFFontEncoder();
		~TTFFontEncoder();

		int		encode(const char* ttfFileName, unsigned int iFontSize, unsigned int iFontDPI, bool bFontpreview = false);
		void	renderBitmapToMemory(unsigned char* dstBitmap, int x, int y, int dstWidth, unsigned char* srcBitmap, int srcWidth, int srcHeight);
		void	exportToFile(const char* ttfFileName, int iFontSize, int iFontDPI, int iImageWidth, int iImageHeight, unsigned char* imageBuffer);
		void	exportToPGM(const char* ttfFileName, int iFontSize, int iFontDPI, int iImageWidth, int iImageHeight, unsigned char* imageBuffer);
		void	exportToTGA(const char* ttfFileName, int iFontSize, int iFontDPI, int iImageWidth, int iImageHeight, unsigned char* imageBuffer);
		void	exportData(const char* ttfFileName, int iFontSize, int iFontDPI, Glyph* glyphArray, int iSize, int iMaxGlyphWidth, int iMaxGlyphHeight);
	protected:

	private:
};

#endif