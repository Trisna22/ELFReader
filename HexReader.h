#include "stdafx.h"

#ifndef HexReader_H
#define HexReader_H
class HexReader
{
public:
	HexReader(string);
	HexReader(FILE*);
	~HexReader();
	bool IsReady();
	string readAllBytes();
	string readBytes(int);
	string readBytes(int, int);
private:
	FILE* readFile = NULL;
};
#endif // !~ HexReader_H

/*   Constructor with string of filename.   */
HexReader::HexReader(string FileName)
{
	this->readFile = fopen(FileName.c_str(), "rb");
}

/*   Constructor with pointer to file object.   */
HexReader::HexReader(FILE* readFile)
{
	this->readFile = readFile;
}

/*   Deconstructor of the class.   */
HexReader::~HexReader()
{
	fclose(this->readFile);
}

/*   Check if the class is ready.   */
bool HexReader::IsReady()
{
	if (this->readFile == NULL)
		return false;
	else
		return true;
}

/*   Reads all bytes of the file and displayes it as hex.   */
string HexReader::readAllBytes()
{
	// Get the size of the file.
	fseek(readFile, 0, SEEK_END);
	long fileSize = ftell(readFile);

	// Set the cursor to the beginning of file.
	fseek(readFile, 0, SEEK_SET);

	// Read the content of the file.
	char* fileContent = (char*)malloc(fileSize +1); // Use heap memory.
	fread(fileContent, 1, fileSize, readFile);
	fileContent[fileSize] = '\0';

	return (string)fileContent;
}

