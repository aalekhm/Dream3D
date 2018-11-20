#include "Engine/UI/TTFFontEncoder.h"
#include <cmath>
#include "Common/RandomAccessFile.h"
#include "Common/CCString.h"

TTFFontEncoder::TTFFontEncoder() {

}

TTFFontEncoder::~TTFFontEncoder() {
	
}

int TTFFontEncoder::encode(const char* ttfFileName, unsigned int iFontSize, unsigned int iFontDPI, bool bFontpreview) {
	
	Glyph glyphArray[ASCII_END_INDEX - ASCII_START_INDEX];
	Glyph_ glyphArrayMy[ASCII_END_INDEX - ASCII_START_INDEX];
///////////////////////////////
//1. Initialize Freetype Library
///////////////////////////////
	FT_Library ftLibrary;
	FT_Error ftError = FT_Init_FreeType(&ftLibrary);
	if(ftError) {
		printf("FT_Init_FreeType error: %d\n", ftError);
		return -1;
	}

///////////////////////////////
//2. Initialize Font Face
///////////////////////////////
	FT_Face ftFace;
	ftError = FT_New_Face(ftLibrary, ttfFileName, 0, &ftFace);
	if(ftError) {
		printf("FT_New_Face error: %d\n", ftError);
		return -1;
	}

///////////////////////////////
//3. Set the Char Size
///////////////////////////////
	ftError = FT_Set_Char_Size(	
								ftFace,				// handle to the face object
								0,					// char_width in 1/64th of points.
								iFontSize * 64,		// char_height in 1/64th of points.
								iFontDPI,			// horizontal device resolution (defaults to 72 dpi if resolution (0, 0)).
								iFontDPI			// vertical device resolution.
						);
	if(ftError) {
		printf("FT_Set_Char_Size error: %d\n", ftError);
		return -1;
	}

///////////////////////////////
//4. Point to the 'slot' that contains the actual 'glyph bitmap'
///////////////////////////////
	FT_GlyphSlot ftGlyphSlot = ftFace->glyph;

///////////////////////////////
//5. Iterate through the ASCII_START_INDEX to ASCII_END_INDEX to 
//	 find the maximum Width & Height a glyph can occupy
///////////////////////////////
	int iMaxGlyphWidth = 0;
	int iMaxGlyphHeight = 0;
	int iTotalGlypHeight = 0; // Stores the total number of rows required to all glyphs.

	// Find the 'Max Height' that the 'Glyph' image can occupy.
	for(unsigned char ascii = ASCII_START_INDEX; ascii < ASCII_END_INDEX; ascii++) {

		// Load the glyph image in the 'slot' (erases previously loaded one).
		ftError = FT_Load_Char(ftFace, ascii, FT_LOAD_RENDER);
		if(ftError) {
			printf("FT_Load_Char error: %d\n", ftError);
		}

		int bitmapWidth = ftGlyphSlot->bitmap.pitch;
		int bitmapHeight = ftGlyphSlot->bitmap.rows;

		iMaxGlyphWidth = (bitmapWidth > iMaxGlyphWidth) ? bitmapWidth : iMaxGlyphWidth;
		iMaxGlyphHeight = (bitmapHeight > iMaxGlyphHeight) ? bitmapHeight : iMaxGlyphHeight;

		if(ftGlyphSlot->bitmap.rows > ftGlyphSlot->bitmap_top) {
			bitmapHeight += (ftGlyphSlot->bitmap.rows - ftGlyphSlot->bitmap_top);
		}

		iTotalGlypHeight = (bitmapHeight > iTotalGlypHeight) ? bitmapHeight : iTotalGlypHeight;
	}

	// Include padding in the iTotalGlypHeight
	iTotalGlypHeight += GLYPH_PADDING;

	printf("iTotalGlypHeight = %d\n", iTotalGlypHeight);
	printf("iMaxGlyphHeight = %d\n", iMaxGlyphHeight);


///////////////////////////////
//6. We have to create a 'Squared texture' size that would fit all the require font glyphs.
///////////////////////////////
	int iPenX = 0;
	int iPenY = 0;
	int iRow = 0;

	double powerOf2 = 2;
	unsigned int iImageWidth = 0;
	unsigned int iImageHeight = 0;
	bool bTextureSizeFound = false;

	int iAdvance = 0;
	int i;

	while(!bTextureSizeFound) {

		iImageWidth = (unsigned int)pow(2.0, powerOf2);
		iImageHeight = (unsigned int)pow(2.0, powerOf2);

		iPenX = iPenY = iRow = 0;

		// Find out the squared texture size that would fit all the require font glyphs.
		i = 0;
		for(unsigned char ascii = ASCII_START_INDEX; ascii < ASCII_END_INDEX; ascii++) {

			// Load the glyph image in the 'slot' (erases previously loaded one).
			ftError = FT_Load_Char(ftFace, ascii, FT_LOAD_RENDER);
			if(ftError) {
				printf("FT_Load_Char error: %d\n", ftError);
			}
//////////////////////////////// Test
/* Equivalent to 
	FT_Load_Char()
		--> FT_Get_Char_Index()
		--> FT_Load_Glyph(if 'load_flags' == FT_LOAD_RENDER)
			--> FT_Render_Glyph()
*/
//FT_UInt glyph_index;
///* retrieve glyph index from character code */ 
//glyph_index = FT_Get_Char_Index( face, ascii );
///* load glyph image into the slot (erase previous one) */ 
//error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT ); 
//if ( error ) continue; /* ignore errors */
///* convert to an anti-aliased bitmap */ 
//error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ); 
//if ( error ) continue;
//////////////////////////////// Test

			// Glyph image.
			int iGlyphWidth = ftGlyphSlot->bitmap.pitch;
			int iGlyphHeight = ftGlyphSlot->bitmap.rows;

			iAdvance = iMaxGlyphWidth + GLYPH_PADDING;

			// If we reach the end of the image wrap aroud to the next row.
			if((iPenX + iAdvance) > (int)iImageWidth) {
				iPenX = 0;
				iRow++;
				iPenY = iRow * iTotalGlypHeight;

				if((iPenY + iTotalGlypHeight) > (int)iImageHeight) {
					powerOf2++;
					break;
				}
			}

			// iPenY should include the glyph offsets.
			iPenY += (iMaxGlyphHeight - iGlyphHeight) + (iGlyphHeight - ftGlyphSlot->bitmap_top);

			// Set the pen position for the next glyph
			iPenX += iAdvance;// Move X to next glyph position

			// Move Y back to the top of the row.
			iPenY = iRow * iTotalGlypHeight;

			if(ascii == (ASCII_END_INDEX - 1)) {
				bTextureSizeFound = true;
			}

			i++;
		}
	}

///////////////////////////////
//7. Try further to find a tighter texture size.
///////////////////////////////
	powerOf2 = 1;
	while(true) {
		if((iPenY + iTotalGlypHeight) >= pow(2.0, powerOf2)) {
			powerOf2++;
		}
		else {
			iImageHeight = (int)pow(2.0, powerOf2);
			break;
		}
	}

///////////////////////////////
//8. Allocate temporary image buffer to draw the glyphs into.
///////////////////////////////
	//unsigned char* imageBuffer = (unsigned char*)malloc(iImageWidth * iImageHeight);
	unsigned char* imageBuffer = new unsigned char[iImageWidth * iImageHeight];
	memset(imageBuffer, 0, iImageWidth * iImageHeight);
	imageBuffer[0] = '1';
	iPenX = 0;
	iPenY = 0;
	iRow = 0;
	i = 0;
	for(unsigned char ascii = ASCII_START_INDEX; ascii < ASCII_END_INDEX; ascii++) {

		// Load the glyph image in the 'slot' (erases previously loaded one).
		ftError = FT_Load_Char(ftFace, ascii, FT_LOAD_RENDER);
		if(ftError) {
			printf("FT_Load_Char error: %d\n", ftError);
		}

		// Glyph image.
		unsigned char* glyphBuffer =  ftGlyphSlot->bitmap.buffer;
		int iGlyphWidth = ftGlyphSlot->bitmap.pitch;
		int iGlyphHeight = ftGlyphSlot->bitmap.rows;

		iAdvance = iMaxGlyphWidth + GLYPH_PADDING;

		// If we reach the end of the image wrap aroud to the next row.
		if((iPenX + iAdvance) > (int)iImageWidth) {
			iPenX = 0;
			iRow++;
			iPenY = iRow * iTotalGlypHeight;

			if((iPenY + iTotalGlypHeight) > (int)iImageHeight) {
				free(imageBuffer);
				printf("Image size exceeded !!!\n");
				return -1;
			}
		}

		// iPenY should include the glyph offsets.
		iPenY += (iMaxGlyphHeight - iGlyphHeight) + (iGlyphHeight - ftGlyphSlot->bitmap_top);

		// Draw the glyph to the bitmap with a one pixel padding.
		renderBitmapToMemory(imageBuffer, iPenX, iPenY, iImageWidth, glyphBuffer, iGlyphWidth, iGlyphHeight);

		// Move Y back to the top of the row.
		iPenY = iRow * iTotalGlypHeight;
	
		//////////////////////
		glyphArray[ascii - ASCII_START_INDEX].index = ascii;
		glyphArray[ascii - ASCII_START_INDEX].width = iGlyphWidth;
		glyphArray[ascii - ASCII_START_INDEX].height = iGlyphHeight;
		// Generate UV coords.
		glyphArray[i].uvCoords[0] = iPenX;//(float)iPenX / (float)iImageWidth;
		glyphArray[i].uvCoords[1] = iPenY;//(float)iPenY / (float)iImageHeight;
		glyphArray[i].uvCoords[2] = iPenX + iGlyphWidth;//(float)(iPenX + iAdvance - GLYPH_PADDING) / (float)iImageWidth;
		glyphArray[i].uvCoords[3] = iPenY + iGlyphHeight;//(float)(iPenY + iTotalGlypHeight) / (float)iImageHeight;

		//glyphArrayMy[ascii - ASCII_START_INDEX].code = ascii;  // unicode value
		//glyphArrayMy[ascii - ASCII_START_INDEX].index = ftGlyphSlot->; // freetype glyph index
		//glyphArrayMy[ascii - ASCII_START_INDEX].width = ftGlyphSlot->bitmap.pitch;  // width of the glyph in pixels
		//glyphArrayMy[ascii - ASCII_START_INDEX].height = ftGlyphSlot->bitmap.rows; // height of the glyph in pixels
		//glyphArrayMy[ascii - ASCII_START_INDEX].advanceX = ftGlyphSlot->advance.x; // number of pixels to advance on x axis
		//glyphArrayMy[ascii - ASCII_START_INDEX].advanceY = ftGlyphSlot->advance.y; // number of pixels to advance on x axis
		//glyphArrayMy[ascii - ASCII_START_INDEX].bearingX = ftGlyphSlot->bitmap_left; // x offset of top-left corner from x axis
		//glyphArrayMy[ascii - ASCII_START_INDEX].bearingY = ftGlyphSlot->bitmap_top; // y offset of top-left corner from y axis
		//glyphArrayMy[ascii - ASCII_START_INDEX].textureX; // x pixel coord of the bitmap's bottom-left corner
		//glyphArrayMy[ascii - ASCII_START_INDEX].textureY;// y pixel coord of the bitmap's bottom-left corner
		//////////////////////

		// Set the pen position for the next glyph
		iPenX += iAdvance; // Move X to next glyph position
		i++;
	}

///////////////////////////////
//9. Write out font map to an image( .pgm format).
///////////////////////////////
	if (bFontpreview) {
		exportToFile(ttfFileName, iFontSize, iFontDPI, iImageWidth, iImageHeight, imageBuffer);
		exportData(ttfFileName, iFontSize, iFontDPI, glyphArray, ASCII_END_INDEX - ASCII_START_INDEX, iAdvance, iTotalGlypHeight);
	}

///////////////////////////////
//10. Cleanup resources.
///////////////////////////////
	free(imageBuffer);

	FT_Done_Face(ftFace);
	FT_Done_FreeType(ftLibrary);
}

void TTFFontEncoder::exportData(const char* ttfFileName, int iFontSize, int iFontDPI, Glyph* glyphArray, int iSize, int iMaxGlyphWidth, int iMaxGlyphHeight) {

	char* bytes = new char[255];
	CCString sFileNameNoExt = ttfFileName;
	sFileNameNoExt = CCString::substr(&sFileNameNoExt, 0, sFileNameNoExt.lastIndexOf('.'));
	sprintf(bytes, "_%d_%dDPI", iFontSize, iFontDPI);
	sFileNameNoExt += bytes;
	sFileNameNoExt += ".dat";

	RandomAccessFile* rafOut = new RandomAccessFile();
	bool bCanWrite = rafOut->openForWrite(sFileNameNoExt.c_str());
	if(bCanWrite) {
		rafOut->writeInt(iMaxGlyphWidth);
		rafOut->writeInt(iMaxGlyphHeight);
		rafOut->writeInt(iSize);
		for(int i = 0; i < iSize; i++) {
			glyphArray[i].write(rafOut);
		}
	}

	rafOut->close();
}

void TTFFontEncoder::renderBitmapToMemory(unsigned char* dstBitmap, int x, int y, int dstWidth, unsigned char* srcBitmap, int srcWidth, int srcHeight) {
	// offset dst bitmap by x,y.
	dstBitmap +=  (x + (y * dstWidth));

	for (int i = 0; i < srcHeight; ++i)
	{
		memcpy(dstBitmap, (const void*)srcBitmap, srcWidth);
		srcBitmap += srcWidth;
		dstBitmap += dstWidth;
	}	
}

void TTFFontEncoder::exportToFile(const char* ttfFileName, int iFontSize, int iFontDPI, int iImageWidth, int iImageHeight, unsigned char* imageBuffer) {

	exportToPGM(ttfFileName, iFontSize, iFontDPI, iImageWidth, iImageHeight, imageBuffer);
	exportToTGA(ttfFileName, iFontSize, iFontDPI, iImageWidth, iImageHeight, imageBuffer);
}

void	TTFFontEncoder::exportToPGM(const char* ttfFileName, int iFontSize, int iFontDPI, int iImageWidth, int iImageHeight, unsigned char* imageBuffer) {

	///////////////////////////////
	// PGM - Portable Greymap Format (P5 ==> Portable graymap with 'Binary' encoding
	///////////////////////////////
	// Magic	Number	Type			Encoding
	// P1			Portable bitmap		ASCII
	// P2			Portable graymap	ASCII
	// P3			Portable pixmap		ASCII
	// P4			Portable bitmap		Binary
	// P5			Portable graymap	Binary
	// P6			Portable pixmap		Binary
	///////////////////////////////
	// P5 %d(Width) %d(Height) %d(numbers of grey between black and white)
	// ..........
	// .....
	// ... Image data
	///////////////////////////////

	char* bytes = new char[255];
	CCString sFileNameNoExt = ttfFileName;
	sFileNameNoExt = CCString::substr(&sFileNameNoExt, 0, sFileNameNoExt.lastIndexOf('.'));
	sprintf(bytes, "_%d_%dDPI", iFontSize, iFontDPI);
	sFileNameNoExt += bytes;
	sFileNameNoExt += ".pgm";

	RandomAccessFile* rafOut = new RandomAccessFile();
	bool bCanWrite = rafOut->openForWrite(sFileNameNoExt.c_str());
	if(bCanWrite) {
		sprintf(bytes, "P5 %u %u 255\n", iImageWidth, iImageHeight);
		rafOut->writeBytes(bytes);
		rafOut->write((char *)imageBuffer, 0, iImageWidth * iImageHeight);
	}

	rafOut->close();
}

void	TTFFontEncoder::exportToTGA(const char* ttfFileName, int iFontSize, int iFontDPI, int iImageWidth, int iImageHeight, unsigned char* imageBuffer) {

	char* bytes = new char[255];
	CCString sFileNameNoExt = ttfFileName;
	sFileNameNoExt = CCString::substr(&sFileNameNoExt, 0, sFileNameNoExt.lastIndexOf('.'));
	sprintf(bytes, "_%d_%dDPI", iFontSize, iFontDPI);
	sFileNameNoExt += bytes;
	sFileNameNoExt += ".tga";

	RandomAccessFile* rafOut = new RandomAccessFile();
	bool bCanWrite = rafOut->openForWrite(sFileNameNoExt.c_str());
	if(bCanWrite) {
		rafOut->writeByte(0);													/*	Image ID length - 1 byte */
																							/*
																								0 - 255 The number of bytes that the image ID field consists of. 
																								The image ID field can contain any information, but it is common for it to 
																								contain the date and time the image was created or a serial number.
																								As of version 2.0 of the TGA spec, the date and time the image was created is catered for in the extension area.
																							*/
		rafOut->writeByte(0);													/* Color map type - 1 byte */
																							/*
																								0 if image file contains no color map
																								1 if present
																								2-127 reserved by Truevision
																								128-255 available for developer use
																							*/
		rafOut->writeByte(2);													/* Image type - 1 byte */
																							/*
																								0 no image data is present
																								1 uncompressed color-mapped image
																								2 uncompressed true-color image
																								3 uncompressed black-and-white (grayscale) image
																								9 run-length encoded color-mapped image
																								10 run-length encoded true-color image
																								11 run-length encoded black-and-white (grayscale) image
																							*/
		rafOut->writeByte(0);		rafOut->writeByte(0);			/* First entry index (2 bytes): offset into the color map table */
		rafOut->writeByte(0);		rafOut->writeByte(0);			/* Color map length (2 bytes): number of entries */
		rafOut->writeByte(0);													/* Color map entry size (1 byte): number of bits per pixel */
		rafOut->writeByte(0);		rafOut->writeByte(0);			/* X origin - 2 bytes*/
		rafOut->writeByte(0);		rafOut->writeByte(0);			/* y origin - 2 bytes*/
		rafOut->writeByte((iImageWidth & 0x00FF));					/* Image width - 2 bytes, width in pixels*/
		rafOut->writeByte((iImageWidth & 0xFF00) / 256);			
		rafOut->writeByte((iImageHeight & 0x00FF));					/* Image height - 2 bytes, height in pixels*/
		rafOut->writeByte((iImageHeight & 0xFF00) / 256);
		rafOut->writeByte(32);													/* Pixel depth - 1 byte, bits per pixel (bpp) */
		rafOut->writeByte(0);													/* Image descriptor (1 byte): bits 3-0 give the alpha channel depth, bits 5-4 give direction */

		int iSize = iImageWidth * iImageHeight * 4;
		char* imgData = new char[iSize];
		memset(imgData, 0, iSize);

		char* imgPtr = imgData;
		for(int y = iImageHeight-1; y >= 0; y--) {
			for(int x = 0; x < iImageWidth; x++) {

				*imgPtr = 255;	imgPtr++;	// Blue
				*imgPtr = 255;	imgPtr++;	// Green
				*imgPtr = 255;	imgPtr++;	// Red

				char alpha = imageBuffer[y * iImageWidth + x];
				*imgPtr = alpha;	imgPtr++;	// Alpha
			}
		}

		rafOut->write((char *)imgData, 0, iSize);
	}

	rafOut->close();
}
