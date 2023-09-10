//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) Idealabs Interactive.
//-----------------------------------------------------------------------------

#include "Common/RandomAccessFile.h"
//#include "Util.h"
#include <stdlib.h>
#include <string.h>
#include <windows.h>

//default constructor
RandomAccessFile::RandomAccessFile() 
: m_pFileObj(NULL)
, m_lFilePointer(0)
, m_sFileName("")
, m_sStringValue("")
{
}

RandomAccessFile::~RandomAccessFile() {
	close();
}

bool RandomAccessFile::openForRead(const char *fileName) {//, const char *mode) {
	int fileStatus = _access(fileName, 4);
	if(fileStatus == -1) {
		printf("RandomAccessFile:: %s File/Directory does not exist!!!", fileName);
		return false;
	}

	printf("%s - opened for Read!!!\n", fileName);
	m_sFileName = fileName;

	fopen_s(&m_pFileObj, m_sFileName.c_str(), "rb");//Read As BINARY
	m_lFilePointer = 0;

	return true;
}

bool RandomAccessFile::openForWrite(const char *fileName) {
	if(_access(fileName, 0) == -1) {
		touch(fileName);
	}

	int fileStatus = _access(fileName, 2);
	if(fileStatus == -1) {
		printf("RandomAccessFile:: %s File/Directory does not exist!!!", fileName);
		return false;
	}

	m_sFileName = fileName;

	fopen_s(&m_pFileObj, m_sFileName.c_str(), "wb+");//Write as BINARY
	m_lFilePointer = 0;
	
	return true;
}

const char* RandomAccessFile::openForAppend(const char *fileName) {
	int fileStatus = _access(fileName, 6);
	if(fileStatus == -1) {
		printf("RandomAccessFile:: %s File/Directory does not exist!!!", fileName);
		return "-1";
	}

	m_sFileName = fileName;

	fopen_s(&m_pFileObj, m_sFileName.c_str(), "a+");
	m_lFilePointer = getFileLength();

	return "1";
}

bool RandomAccessFile::openForRW(const char *fileName) {//, const char *mode) {
	int fileStatus = _access(fileName, 6);
	if(fileStatus == -1) {
		printf("RandomAccessFile::open --> File/Directory does not exist!!!\n");
		return false;
	}

	m_sFileName = fileName;

	fopen_s(&m_pFileObj, m_sFileName.c_str(), "rb+");//Read As BINARY
	m_lFilePointer = 0;

	return true;
}

int RandomAccessFile::touch(const char* fileName) {
	int fileHandle = -1;
	fileHandle = _creat(fileName, _S_IREAD | _S_IWRITE);
	if(fileHandle == -1) {
		return -1;
	}

	printf("File created. %s\n", fileName);
	_close(fileHandle);
	return 1;
}

void RandomAccessFile::close() {
	if(m_pFileObj != NULL) {
		if(fclose(m_pFileObj) == 0) {
			m_pFileObj = 0;
			
			printf("File close successfull.\n");
		}
		else
			printf("Unable to close File.\n");
	}
}

bool RandomAccessFile::readBoolean() {
	return (readByte()>0)?true:false;
}

const char* RandomAccessFile::readBooleanS() {

	m_sStringValue = (readByte()>0) ? "true" : "false";
	return m_sStringValue.c_str();
}

int RandomAccessFile::readByte() {
	char byte0 = -1;

	fseek(m_pFileObj, m_lFilePointer, SEEK_SET);

	if(m_pFileObj != NULL && !isEOF()) {

		byte0 = getc(m_pFileObj);
		m_lFilePointer += 1;
	}

	return byte0&0xff;
}

char RandomAccessFile::readChar() {
	if (m_pFileObj != NULL) {
		unsigned char byte0;

		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);
		byte0 = fgetc(m_pFileObj);
		m_lFilePointer += 1;

		return byte0;
	}

	return NULL;
}

const char* RandomAccessFile::readCharS() {
	if (m_pFileObj != NULL) {
		unsigned char byte0;
		char* pBuffer = new char[sizeof(char)+1];
		memset(pBuffer, 0, sizeof(char)+1);
		pBuffer[sizeof(char)] = '\0';

		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);

		byte0 = fgetc(m_pFileObj);
		m_lFilePointer += 1;

		sprintf(pBuffer, "%c", byte0);
		pBuffer[sizeof(char)] = '\0';

		m_sStringValue = pBuffer;
		SAFE_DELETE_ARRAY(pBuffer);

		return m_sStringValue.c_str();
	}

	return NULL;
}

const char* RandomAccessFile::readByteS() {
	char* pBuffer = new char[sizeof(char)+1];
	memset(pBuffer, 0, sizeof(char)+1);
	pBuffer[sizeof(char)] = '\0';

	sprintf(pBuffer, "%d", readByte());
	pBuffer[sizeof(char)] = '\0';

	m_sStringValue = pBuffer;
	SAFE_DELETE_ARRAY(pBuffer);

	return m_sStringValue.c_str();
}

int RandomAccessFile::readShort() {
	if(m_pFileObj != NULL) {
		int int_16;

		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);

		int_16 = readByte();
		int_16 |= (readByte()<<8);

		return int_16;
	}

	return NULL;
}

const char* RandomAccessFile::readShortS() {
	if (m_pFileObj != NULL) {
		short LSB;
		char* pBuffer = new char[sizeof(short)+1];
		memset(pBuffer, 0, sizeof(short)+1);
		pBuffer[sizeof(short)] = '\0';

		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);

		sprintf(pBuffer, "%d", readShort());
		pBuffer[sizeof(short)] = '\0';

		m_sStringValue = pBuffer;
		SAFE_DELETE_ARRAY(pBuffer);

		return m_sStringValue.c_str();
	}

	return NULL;
}

unsigned long RandomAccessFile::readInt() {
	if(m_pFileObj != NULL) {
		unsigned long int_32;

		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);

		int_32 = readShort();
		int_32 |= readShort()<<16;

		return int_32;
	}

	return NULL;
}

const char* RandomAccessFile::readIntS() {
	if (m_pFileObj != NULL) {
		char* pBuffer = new char[sizeof(int)+1];
		memset(pBuffer, 0, sizeof(int)+1);
		pBuffer[sizeof(int)] = '\0';

		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);

		sprintf(pBuffer, "%lu", readInt());
		pBuffer[sizeof(int)] = '\0';

		m_sStringValue = pBuffer;
		SAFE_DELETE_ARRAY(pBuffer);

		return m_sStringValue.c_str();
	}

	return NULL;
}

unsigned long long RandomAccessFile::readLong() {
	if(m_pFileObj != NULL) {
		unsigned long long LSB, MSB, int_64;

		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);

		LSB  = readInt();
		MSB  = readInt();
		int_64 = MSB<<32;
		int_64 |= LSB;

		return int_64;
	}

	return -1;
}

const char* RandomAccessFile::readLongS() {
	if(m_pFileObj != NULL) {
		char* pBuffer = new char[sizeof(long long)+1];
		memset(pBuffer, 0, sizeof(long long)+1);
		pBuffer[sizeof(long long)] = '\0';

		sprintf(pBuffer, "%llu", readLong());
		pBuffer[sizeof(long long)] = '\0';

		m_sStringValue = pBuffer;
		SAFE_DELETE_ARRAY(pBuffer);

		return m_sStringValue.c_str();
	}

	return NULL;
}

std::string RandomAccessFile::readAll(const char* pFilename) {
	
	std::string sReturnBuffer = "";
	if (pFilename != NULL) 
	{
		RandomAccessFile* pRafIn = new RandomAccessFile();
		bool bCanRead = pRafIn->openForRead(pFilename);
		if (bCanRead)
		{
			FILE* pFILEObj = pRafIn->getRawFileObj();
			if (pFILEObj != NULL) {

				unsigned long long iFileSize = pRafIn->length();
				char* pBuffer = new char[iFileSize + 1];
				memset(pBuffer, 0, iFileSize + 1);

				int iBytesRead = fread(pBuffer, sizeof(char), iFileSize, pFILEObj);

				sReturnBuffer = pBuffer;
				SAFE_DELETE_ARRAY(pBuffer);
				
				pRafIn->close();
			}

			pRafIn = NULL;
		}
	}

	return sReturnBuffer;
}

void RandomAccessFile::writeAll(const char* pFilename, const char* sWriteString) {

	int iWriteLength = strlen(sWriteString);
	if (pFilename != NULL && iWriteLength > 0)
	{
		RandomAccessFile* pRafOut = new RandomAccessFile();
		bool bCanWrite = pRafOut->openForWrite(pFilename);
		if (bCanWrite)
		{
			FILE* pFILEObj = pRafOut->getRawFileObj();
			if (pFILEObj != NULL) {

				int iBytesWritten = fwrite(sWriteString, sizeof(char), iWriteLength, pFILEObj);
				pRafOut->close();
			}

			pRafOut = NULL;
		}
	}
}

void RandomAccessFile::writeByte(const char* byte0) {
	if(m_pFileObj != NULL) {
		writeByte(atoi(byte0));
	}
}

void RandomAccessFile::writeByte(int byte0) {
	if(m_pFileObj != NULL) {
		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);
		fputc(byte0, m_pFileObj);
		
		m_lFilePointer += 1;
	}
}

void RandomAccessFile::writeShort(const char* shortValue) {
	if(m_pFileObj != NULL) {
		writeShort(atoi(shortValue));
	}
}

void RandomAccessFile::writeShort(int shortValue) {
	if(m_pFileObj != NULL) {
		writeByte(shortValue&0xff);
		writeByte((shortValue>>8)&0xff);
	}
}

void RandomAccessFile::writeBoolean(const char* isTrue) {
	if(m_pFileObj != NULL) {
		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);

		int numValue = 0;
		if(strcmpi(isTrue, "1") == 0) {
			numValue = 1;
		}
		
		fputc(numValue, m_pFileObj);
		m_lFilePointer += 1;
	}
}

void RandomAccessFile::writeBoolean(int isTrue) {
	writeByte(isTrue>0?1:0);
}

void RandomAccessFile::writeChar(const char* charValue) {
	if(m_pFileObj != NULL) {
		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);
		
		char char0 = (char)charValue[0];
		
		fputc(char0, m_pFileObj);
		m_lFilePointer += 1;
	}
}

const char *RandomAccessFile::isEOFS() {
	if(m_pFileObj != NULL) {
		if(!feof(m_pFileObj)) {
			return "false";
		}
		else {
			return "true";
		}	
	}

	return "true";
}

int RandomAccessFile::isEOF() {
	if(m_pFileObj != NULL) {
		return feof(m_pFileObj)?1:0;
	}

	return 1;
}
/*
const char *RandomAccessFile::readLine() {
	if(isEOF())
		return "";

	int REALLOC = 1;
	int MAX_READ = 255;

	char* line = (char*)::malloc(MAX_READ);//GlobalAlloc(0, MAX_READ);
	::memset(line, 1, MAX_READ);

	int prevFilePointer = filePointer;

	int bytesRead = read(line);
	int cursor = 0, count = 0;
		
	//MessageBox(NULL, getStringValue(bytesRead), "OOOOOOOOOOOOOOOOOOOOOOOOO", MB_OK);

	while(true) {
		if(line[cursor] == '\r') {
			line[cursor++] = 0;
			
			filePointer = prevFilePointer + cursor;
			if(line[cursor] == '\n') {
				filePointer++;
			}
			
			fseek(m_pFileObj, filePointer, SEEK_SET);

			break;
		}
		else {
			if(	count >= bytesRead
				&&
				bytesRead != MAX_READ
			) {
				line[cursor++] = 0;
				filePointer = prevFilePointer + cursor;
				break;
			}
		}
		

		cursor++;
		count++;

		if(cursor%255 == 0) {
			REALLOC++;
//MessageBox(NULL, getStringValue(REALLOC*255), MyStrCat("REALLOC ", (char*)getStringValue(cursor)), MB_OK);
			line = new char[REALLOC*MAX_READ+1];//reallocMemory(line, REALLOC*MAX_READ, cursor, 'A');
			line[REALLOC*MAX_READ] = '\0';
			
			bytesRead = read(line, cursor, MAX_READ);
			count = 0;

			//if(isEOF())
			//	MB("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");

//MessageBox(NULL, getStringValue(filePointer), "filePointer...................", MB_OK);
//MessageBox(NULL, getStringValue(bytesRead), "bytesRead...................", MB_OK);
//MessageBox(NULL, line, MyStrCat("linelinelinelineline ", (char*)getStringValue(bytesRead)), MB_OK);
		}
	}
//MessageBox(NULL, getStringValue(filePointer), "filePointer...................", MB_OK);
	return line;
}
//*/
const char *RandomAccessFile::readLine() {
	if(isEOF())
		return "";	
	
	int prevFilePointer = m_lFilePointer;

	unsigned char c = readChar();
	while(!(c == '\r' || c == '\n') && !isEOF()) {
		c = readChar();
	}
	
	int iLineLength = m_lFilePointer - prevFilePointer - 1;
	if(c == '\r') {
		m_lFilePointer++;
	}

	if(iLineLength > 0) {
		char* line = new char[sizeof(char) * iLineLength+1];
		::memset(line, 0, sizeof(char) * iLineLength+1);
		line[sizeof(char) * iLineLength] = '\0';

		fseek(m_pFileObj, prevFilePointer, SEEK_SET);
		
		int iRead = fread(line, sizeof(char), iLineLength, m_pFileObj);
		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);

		line[sizeof(char) * iLineLength] = '\0';

		m_sStringValue = line;
		SAFE_DELETE_ARRAY(line);

		return m_sStringValue.c_str();
	}
	
	return "";
}

void RandomAccessFile::writeLine(const char *line) {
	if(m_pFileObj != NULL) {
		writeChars(line);
		writeChars("\r");
		writeChars("\n");
	}
}

void RandomAccessFile::write(char* buf) {
	writeChars(buf);
}

void RandomAccessFile::write(char* buf, int offset, int len) {
	char* writeFrom = buf+offset;

	fseek(m_pFileObj, m_lFilePointer, SEEK_SET);
	
	fwrite(writeFrom, sizeof(char), len, m_pFileObj);
	m_lFilePointer += len;
}

void RandomAccessFile::writeChars(const char* buf) {
	if(m_pFileObj != NULL) {
		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);
		
		fwrite(buf, sizeof(char), strlen(buf), m_pFileObj);
		m_lFilePointer += strlen(buf);
	}
}

void RandomAccessFile::writeBytes(const char* buf) {
	writeChars(buf);
}

void RandomAccessFile::writeIntS(const char* intValue) {
	if(m_pFileObj != NULL) {
		int num = atol(intValue);

		writeInt(num);
	}
}

void RandomAccessFile::writeInt(unsigned long intValue) {
	if(m_pFileObj != NULL) {
		writeShort(intValue&0xffff);
		writeShort((intValue>>16)&0xffff);
	}
}

void RandomAccessFile::writeLong(unsigned long long longValue) {
	if(m_pFileObj != NULL) {
		writeInt(longValue&0xffffffff);
		writeInt((longValue>>32)&0xffffffff);
	}
}

void RandomAccessFile::writeLong(const char* longValue) {
	//Con::printf("ooooooooo");
	if(m_pFileObj != NULL) {
		//Con::printf("ooooooooooppppppp %s %lld ", longValue, longValue);
		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);
		unsigned long long num0 = atol(longValue);
		/*
		writeByte((num0>>0)&0xff);
		writeByte((num0>>8)&0xff);
		writeByte((num0>>16)&0xff);
		writeByte((num0>>24)&0xff);
		//*/
		///*
		//Con::printf("lonValue num0 %d %d %d %d ", ((num0>>24)&0xff), ((num0>>16)&0xff), ((num0>>8)&0xff), ((num0>>0)&0xff));

		fputc((num0>>0)&0xff, m_pFileObj);
		m_lFilePointer += 1;
		fputc((num0>>8)&0xff, m_pFileObj);
		m_lFilePointer += 1;
		fputc((num0>>16)&0xff, m_pFileObj);
		m_lFilePointer += 1;
		fputc((num0>>24)&0xff, m_pFileObj);
		m_lFilePointer += 1;
		//*/

		unsigned long long num1 = num0>>24;
		num1 >>= 8;

		//Con::printf("lonValue num1 %d %d %d %d ", ((num1>>24)&0xff), ((num1>>16)&0xff), ((num1>>8)&0xff), ((num1>>0)&0xff));
		/*
		writeByte((num1>>0)&0xff);
		writeByte((num1>>8)&0xff);
		writeByte((num1>>16)&0xff);
		writeByte((num1>>24)&0xff);
		//*/
		///*
		fputc((num1>>0)&0xff, m_pFileObj);
		m_lFilePointer += 1;
		fputc((num1>>8)&0xff, m_pFileObj);
		m_lFilePointer += 1;
		fputc((num1>>16)&0xff, m_pFileObj);
		m_lFilePointer += 1;
		fputc((num1>>24)&0xff, m_pFileObj);
		m_lFilePointer += 1;
		//*/
	}
}
///*


int RandomAccessFile::read(char* buf) {
	return read(buf, 0, strlen(buf));
}

int RandomAccessFile::read(char* buf, int offset, int len) {
	int count = 0;
	int bufLen = strlen(buf);
	
	int bytesToRead = len;
	
	//char* sLine = new char[1024];
	//sprintf(sLine, "%d", bufLen);
//MessageBox(NULL, sLine, "read() --- > bufLen", MB_OK);
	if(offset+len > bufLen)
		bytesToRead = bufLen - offset;
	
	//sprintf(sLine, "%d", bytesToRead);
//MessageBox(NULL, sLine, "read() --- > bytesToRead", MB_OK);

	fseek(m_pFileObj, m_lFilePointer, SEEK_SET);

	while(!isEOF()) {
		if(	len > 0 
			&&
			offset < bufLen
		) {
			int bytesRead = fread(buf + offset, sizeof(char), bytesToRead, m_pFileObj);
			m_lFilePointer += bytesRead;

			fseek(m_pFileObj, m_lFilePointer, SEEK_SET);
//MessageBox(NULL, getStringValue(filePointer), "read() --- > filePointer", MB_OK);
			return bytesRead;
		}
		else {
			return count;
		}
	}

	return count;
}

//*/


const char *RandomAccessFile::getFilePointer() {
	char* pBuffer = new char[sizeof(long long)+1];
	memset(pBuffer, 0, sizeof(long long)+1);
	pBuffer[sizeof(long long)] = '\0';

	itoa(m_lFilePointer, pBuffer, 10);
	pBuffer[sizeof(long long)] = '\0';

	return pBuffer;
}

unsigned long long RandomAccessFile::getFilePointerLong() {
	return m_lFilePointer;
}

void RandomAccessFile::skipBytes(unsigned long long numOfBytesToSkip) {
	if(m_pFileObj != NULL) {
		m_lFilePointer += numOfBytesToSkip;
		fseek(m_pFileObj, numOfBytesToSkip, SEEK_SET);
		
		//filePointer = ftell(m_pFileObj);
	}
}

void RandomAccessFile::seek(unsigned long long pos) {
	if(m_pFileObj != NULL) {
		//long position = atol(pos);

		m_lFilePointer = pos;
		fseek(m_pFileObj, m_lFilePointer, SEEK_SET);
	}
}

long RandomAccessFile::getFileLength() {
	if(m_pFileObj!= NULL) {
		int errNo = fseek(m_pFileObj, 0l, SEEK_END);
//MB(getStringValue(errNo));
		if(errNo == 0) {
			
			//long fileLen = ftell(m_pFileObj);
			//return fileLen;
			
			fpos_t pos_t;
			int err = fgetpos(m_pFileObj, &pos_t);

			//MB(getStringValue((long)pos_t));
			
			return (long)pos_t;
		}
	}

	return -1;
}

unsigned long long	RandomAccessFile::length() {
	long int fileLength = 0;
	if(m_pFileObj != NULL) {
		long lastPos = ftell(m_pFileObj);//save last position.

		fseek(m_pFileObj, 0l, SEEK_END);
		fileLength = ftell(m_pFileObj);

		fseek(m_pFileObj, lastPos, SEEK_SET);
	}
	
	return fileLength;
}

const char *RandomAccessFile::lengthS() {
	long int fileLength = length();

	char* pBuffer = new char[sizeof(fileLength)+1];
	memset(pBuffer, 0, sizeof(fileLength)+1);
	pBuffer[sizeof(fileLength)] = '\0';

	sprintf(pBuffer, "%ld", fileLength);
	pBuffer[sizeof(fileLength)] = '\0';

	return pBuffer;
}

int RandomAccessFile::getFD() {
	int fd = -1;
	if(m_pFileObj != NULL) {
		fd = ::fileno(m_pFileObj);
	}

	return fd;
}

const char *RandomAccessFile::getFileDescriptor() {
	char* pBuffer = new char[sizeof(int)+1];
	memset(pBuffer, 0, sizeof(int)+1);
	pBuffer[sizeof(int)] = '\0';
	
	if(m_pFileObj != NULL) {
		sprintf(pBuffer, "%d", getFD());
		pBuffer[sizeof(int)] = '\0';
	}

	return pBuffer;
}

void RandomAccessFile::setLength(const char *newLength) {
	if(m_pFileObj != NULL) {
		long newLen = atol(newLength);
		if(_chsize(getFD(), newLen) == 0) {
			printf("File length sucessfully altered to %ld\n", newLen);
			
			m_lFilePointer = newLen;
			fseek(m_pFileObj, m_lFilePointer, SEEK_SET);
		}
	}
}

void RandomAccessFile::renameFile(const char *newFileName) {
	if(m_pFileObj != NULL) {
		//printf("rename FILE NAME = %s\n", uName);
		//rename(m_pFileObj, newFileName);
	}
}

static void deleteFile(const char *fileToDelete) {
	int errVal = remove(fileToDelete); 
	if(errVal != 0) {
		printf("Error deleting FILE %d\n", errVal);
	}
	else
		printf("FILE \"%s\" deleted sucessfully!!!\n", fileToDelete);
}

/*
ConsoleFunction( deleteFile, const char *, 2, 2, "() Deletes the specified file if it exists.")//Aa
{
	deleteFile(argv[1]);
	return 0;
}

ConsoleMethod( RandomAccessFile, getFilePointer, const char *, 2, 2, "() Returns the current file pointer.")//Aa
{
	return object->getFilePointer();
}

ConsoleMethod( RandomAccessFile, openForWrite, const char *, 3, 3, "() Opens connection to a file for writing.")//Aa
{
	return object->openForWrite(argv[2]);
}

ConsoleMethod( RandomAccessFile, openForAppend, const char *, 3, 3, "() Opens connection to a file for writing.")//Aa
{
	return object->openForAppend(argv[2]);
}

//ConsoleMethod( RandomAccessFile, open, const char *, 4, 4, "() Opens connection to a file.")//Aa
ConsoleMethod( RandomAccessFile, openForRead, const char *, 3, 3, "() Opens connection to a file.")//Aa
{
	return object->openForRead(argv[2]);//, argv[3]);
}

ConsoleMethod( RandomAccessFile, close, void, 2, 2, "() closes connection to a file.")//Aa
{
	object->closeFile();
}

ConsoleMethod( RandomAccessFile, renameFile, void, 3, 3, "() renames a file.")//Aa
{
	object->renameFile(argv[2]);
}

ConsoleMethod( RandomAccessFile, readByte, const char *, 2, 2, "() Reads a byte.")//Aa
{
	return object->readByte();
}

ConsoleMethod( RandomAccessFile, readChar, const char *, 2, 2, "() Reads a byte.")//Aa
{
	return object->readChar();
}

ConsoleMethod( RandomAccessFile, readBoolean, const char *, 2, 2, "() Reads a boolean.")//Aa
{
	return object->readBoolean();
}

ConsoleMethod( RandomAccessFile, readShort, const char *, 2, 2, "() Reads a byte.")//Aa
{
	return object->readShort();
}

ConsoleMethod( RandomAccessFile, readInt, const char *, 2, 2, "() Reads a byte.")//Aa
{
	return object->readInt();
}

ConsoleMethod( RandomAccessFile, readLong, const char *, 2, 2, "() Reads a byte.")//Aa
{
	return object->readLong();
}

ConsoleMethod( RandomAccessFile, readLine, const char *, 2, 2, "() Reads a line.")//Aa
{
	return object->readLine();
}

ConsoleMethod( RandomAccessFile, writeByte, void, 3, 3, "() Writes a byte.")//Aa
{
	return object->writeByte(argv[2]);
}

ConsoleMethod( RandomAccessFile, writeShort, void, 3, 3, "() Writes a short.")//Aa
{
	return object->writeShort(argv[2]);
}

ConsoleMethod( RandomAccessFile, writeBoolean, void, 3, 3, "() Writes a boolean.")//Aa
{
	return object->writeBoolean(argv[2]);
}

ConsoleMethod( RandomAccessFile, writeChar, void, 3, 3, "() Writes a character.")//Aa
{
	return object->writeChar(argv[2]);
}

ConsoleMethod( RandomAccessFile, writeLine, void, 3, 3, "() Writes a string to the file as a sequence of characters with CrLf.")//Aa
{
	return object->writeLine(argv[2]);
}

ConsoleMethod( RandomAccessFile, writeChars, void, 3, 3, "() Writes a string to the file as a sequence of characters.")//Aa
{
	return object->writeChars(argv[2]);
}

ConsoleMethod( RandomAccessFile, writeBytes, void, 3, 3, "() Writes a string to the file as a sequence of bytes.")//Aa
{
	return object->writeBytes(argv[2]);
}

ConsoleMethod( RandomAccessFile, writeInt, void, 3, 3, "() Writes an interger.")//Aa
{
	return object->writeInt(argv[2]);
}

ConsoleMethod( RandomAccessFile, writeLong, void, 3, 3, "() Writes a long interger.")//Aa
{
	return object->writeLong(argv[2]);
}

ConsoleMethod( RandomAccessFile, skipBytes, void, 3, 3, "() skips number of bytes.")//Aa
{
	return object->skipBytes(argv[2]);
}

ConsoleMethod( RandomAccessFile, seek, void, 3, 3, "() positions the filepointer to the position specified from the beginning of the file.")//Aa
{
	return object->seek(argv[2]);
}

ConsoleMethod( RandomAccessFile, length, const char*, 2, 2, "() ")
{
	return object->length();
}

ConsoleMethod( RandomAccessFile, getFileDescriptor, const char*, 2, 2, "() return the file descriptor, an integer value associated with the filepointer")
{
	return object->getFileDescriptor();
}

ConsoleMethod( RandomAccessFile, setLength, void, 3, 3, "() sets the files lenth to the one specified.")
{
	return object->setLength(argv[2]);
}

ConsoleMethod( RandomAccessFile, isEOF, const char *, 2, 2, "() checks for EOF.")
{
	return object->isEOF();
}

/*
INFO: 
fseek(m_pFileObj, filePointer, SEEK_SET);
where,
	SEEK_SET	Beginning of file
	SEEK_CUR	Current position of the file pointer
	SEEK_END	End of file
*/
