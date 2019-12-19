#include "stdafx.h"

#ifndef ELFHeader_H
#define ELFHeader_H
class ELFHeader
{
public:
	explicit ELFHeader(string);
	ELFHeader(FILE*);
	~ELFHeader();
	bool IsReady();

protected:
	void readELF();
	int BitSystem;

private:
	FILE* readFile = NULL;

	/*   First 6 bytes of ELF file   */
	typedef struct ELF_IdentifierHeader {
		unsigned char magicNumber[4];	// Identifier of ELF format
		unsigned char bitSystem;	// 1 = 32 bit, 2 = 64 bit
		unsigned char endianType; 	// 1 = little endian, 2 = big endian
		unsigned char ELFversion;	// 1 = original version
	} ELF_IDENTIFIER;
};

#endif // !~ ELFHeader_H

/*   Constructor with string of filename.   */
ELFHeader::ELFHeader(string FileName)
{
	this->readFile = fopen(FileName.c_str(), "rb");
}

/*   Constructor with pointer to file object.   */
ELFHeader::ELFHeader(FILE* readFile)
{
	this->readFile = readFile;
}

/*   Deconstructor of the class.   */
ELFHeader::~ELFHeader()
{
	fclose(this->readFile);
}

/*   Checks if the class is ready   */
bool ELFHeader::IsReady()
{
	if (this->readFile == NULL)
		return false;
	else
		return true;
}

/*   Reading the first 6 bytes of ELF file.   */
void ELFHeader::readELF()
{
	if (IsReady() == false)
	{
		printf("ELFHeader not ready yet!\n");
		return;
	}

	// Read ELF header structure from file.
	ELF_IdentifierHeader identifier;
	int status = fread(&identifier, sizeof(ELF_IdentifierHeader), 1, readFile);
	if (status == 0)
	{
		printf("ELFHeader: failed to read bytes from file!\n");
		return;
	}

	// Reading the magic number.
	if (!(identifier.magicNumber[0] == 0x7F && identifier.magicNumber[1] == 'E' &&
		identifier.magicNumber[2] == 'L' && identifier.magicNumber[3] == 'F'))
	{
		printf("ELFHeader: No magic number detected! Wrong ELF format!\n");
		return;
	}

	// Checking bitsystem.
	if (!(identifier.bitSystem == 0x01 || identifier.bitSystem == 0x02))
	{
		printf("ELFHeader: Unknown bitsystem: %d\n", identifier.bitSystem);
		return;
	}
	else if (identifier.bitSystem == 0x01)
		printf("BitSystem: x32\n");
	else if (identifier.bitSystem == 0x02)
		printf("BitSystem: x64\n");

	this->BitSystem = identifier.bitSystem;
	
}
