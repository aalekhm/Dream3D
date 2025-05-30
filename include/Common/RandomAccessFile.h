/////////////////////////////////////////////////////////////////////////////////////////
//Usage: RandomAccessFile in scripting
//	1. Path for the file to read is saved in:
//		%fileName = c:\windows\system.dll;
//	2. Create a new object:
//		%fileObj = new RandomAccessFile();
//	3. Open the file for READ, WRITE, APPEND using:
//		IMP -	File is opened in different modes since a READ_ONLY file cannot be opened in RW/Update mode ie "r+" _
//				and hence gives an error.
//		%fileObj.openForRead(%fileName);
//		%fileObj.openForWrite(%fileName);
//		%fileObj.openForAppend(%fileName);
//	4. File reading methods:
//		a1. readByte() 	- Reads a byte from the file stream.
//		b2. readChar() 	- Reads a character from the file stream.
//		c3. readBoolean() - Reads a byte from the file stream, the value is either 0 or greater than zero.
//		d4. readShort() 	- Reads a short value from the file stream.
//		e5. readInt() 	- Reads a integer value from the file stream.
//		f6. readLong() 	- Reads a long value from the file stream.
//		g7. readLine() 	- Reads a series of character till it finds "Line feed" or "Carriage return" (CrLf).
//	5. File writing methods:
//		a8. writeByte(byteValue < 255) 	- Writes a byte to the file stream.
//		b9. writeChar('A') 		- Writes a character to the file stream.
//		c10. writeBoolean("true") 		- Writes a byte to the file stream, the value is either 0 or greater than zero.
//		d11. writeShort(shortValue < 65535) 	- Writes a short value to the file stream.
//		e12. writeInt(intValue < 4294967295)	- Writes a integer value to the file stream.
//		f13. writeLong(intValue < 18446744073709551615) 	- Writes a long value to the file stream.
//		g14. writeLine("This is a sequence of characters") - Writes a String.
//	6. Some useful methods:
//		a15. isEOF() - Checks for EOF(End of File), returns "true" or "false".
//		b16. getFileDescriptor() - returns the file descriptor, an integer value associated with the fileObjPointer.
//		c17. length() - returns the length of the file in bytes.
//		d18. skipBytes(10) - skips the number of bytes specified.
//		e19. seek(1024) - Positions the filepointer to the position specified from the beginning of the file.
//		f20. setLength(2048) - sets the file to be of the specified length.
//		g21. getFilePointer() - returns the current position of the file pointer in the File Stream.
//		h22. close() - closes the file stream, the object can be of no use after this.
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef _RandomAccessFile_H_
#define _RandomAccessFile_H_

#include <stdio.h>
#include <io.h>
#include <sys\stat.h>
#include <string.h>
#include "CCString.h"
#include <string>

#define SAFE_DELETE(p) { if(p) { delete p; p = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] p; p = NULL; } }

//*/
extern "C" {
	class _declspec(dllexport) RandomAccessFile {
			FILE*					m_pFileObj;
			unsigned long long		m_lFilePointer;
			CCString				m_sFileName;
			CCString				m_sStringValue;
		public:
			RandomAccessFile();
			~RandomAccessFile();
			
			void					close();
						
			bool					readBoolean();
			const char*				readBooleanS();

			char					readChar();
			const char*				readCharS();

			int						readByte();
			const char*				readByteS();

			int						readShort();
			const char*				readShortS(); 
			
			unsigned long			readInt();
			const char*				readIntS(); 
			
			unsigned long long		readLong();
			const char*				readLongS();
			
			const char*				readLine();
			const char*				getFilePointer();		
			
			int						read(char* buf);
			int						read(char* buf, int offset, int len);

			static std::string		readAll(const char* pFilename);
			static void				writeAll(const char* pFilename, const char* sWriteString);

			void					write(char* buf);
			void					write(char* buf, int offset, int len);

			void					writeByte(const char *);		//ok
			void					writeByte(int);				
			void					writeShort(const char *);
			void					writeShort(int);
			void					writeBoolean(const char *);
			void					writeBoolean(int);
			void					writeChar(const char*);
			void					writeLine(const char*);
			void					writeIntS(const char*);
			void					writeInt(unsigned long);
			void					writeLong(const char*);
			void					writeLong(unsigned long long);
			void					writeChars(const char*);
			void					writeBytes(const char*);
			void					skipBytes(unsigned long long numOfBytesToSkip);	//ok
			void					seek(unsigned long long pos);					//ok
			const char*				lengthS();										//ok
			unsigned long long		length();										//ok
			long					getFileLength();								//ok
			unsigned long long		getFilePointerLong();
			const char*				getFileDescriptor();							//ok
			int						getFD();										//ok
			void					setLength(const char*);							//ok
			int						isEOF();										//ok
			const char*				isEOFS();										//ok
			int						touch(const char*);
			
			//File is opened in different modes since a READ_ONLY file cannot be opened in RW/Update mode ie "r+"
			bool					openForRead(const char *);
			bool					openForWrite(const char*);
			const char*				openForAppend(const char*);
			bool					openForRW(const char*);


			void					renameFile(const char*);
			static void				deleteFile(const char*);

			FILE*					getRawFileObj() { return m_pFileObj; }
	};

	//__int8 nSmall;      // Declares 8-bit integer
	//__int16 nMedium;    // Declares 16-bit integer
	//__int32 nLarge;     // Declares 32-bit integer
	//__int64 nHuge;      // Declares 64-bit integer
}
#endif