#include "stdafx.h"

#ifndef ELFFunction_H
#define ELFFunction_H
class ELFFunction
{
public:
	explicit ELFFunction(string);
	ELFFunction(FILE*);
	~ELFFunction();
	bool IsReady();
private:
	FILE* readFile = NULL;
	bool InvalidELFFormat = false;

private:
	/*   Identification structure from ELF header.   */
	typedef struct ELFHeaderStruct {
		unsigned char magicNumber[4];           // Magic number.
		unsigned char bitSystem;                // Bit system.
		unsigned char endianType;               // Endian type.
		unsigned char ELFVersion;               // ELF version.
		unsigned char targetABI;                // ABI target.
		unsigned char abiVersion;               // ABI version.
		unsigned char unusedSpace[7];           // Padding.
	} ELF_HEADER;

	ELFHeaderStruct* ReadELF_Identifier();
protected:
	ELFHeaderStruct* identifier;
};

#endif // !~ ELFFunction_H

/*   Constructor with string of fileName.   */
ELFFunction::ELFFunction(string FileName)
{
	this->readFile = fopen(FileName.c_str(), "rb");
	if (this->readFile == NULL)
	{
		printf("ELFFunction: Failed to open file! Error code: %d\n", errno);
		this->InvalidELFFormat = true;
		return;
	}

	// Check the bitsystem and if is ELF format.
	this->identifier = ReadELF_Identifier();
}

/*   Deconstructor of the class.   */
ELFFunction::~ELFFunction()
{
	if (this->readFile != NULL)
		fclose(this->readFile);
}

/*   Checks if the class is ready   */
bool ELFFunction::IsReady()
{
	// Check if file is opened.
	if (this->readFile == NULL)
		return false;

	// Check if invalid ELF format.
	if (InvalidELFFormat == true)
		return false;
}

/*   Get the bitsystem of the file, also check if is really ELF format.   */
ELFFunction::ELFHeaderStruct* ELFFunction::ReadELF_Identifier()
{
	/*   Reads our structure from file.   */
	ELF_HEADER *elfHeader = new ELF_HEADER();
	int status = fread(elfHeader, sizeof(ELF_HEADER), 1, readFile);
	if (status == 0)
	{
		printf("ELFHeader: failed to read bytes from file!\n");
		this->InvalidELFFormat = true;
		return 0;
	}

	/*   Checks if it is really an ELF structure.   */

	// Based by: magic number
	if (!(elfHeader->magicNumber[0] == 0x7F && elfHeader->magicNumber[1] == 'E' &&
		elfHeader->magicNumber[2] == 'L' && elfHeader->magicNumber[3] == 'F'))
	{
		printf("ELFHeader: No magic number detected! Wrong ELF format!\n");
		this->InvalidELFFormat = true;
		return 0;
	}

	// Based by: bitsystem
	if (!(elfHeader->bitSystem == 0x01 || elfHeader->bitSystem == 0x02))
	{
		printf("ELFHeader: Wrong bitsystem detected! Wrong ELF format or not supported!\n");
		this->InvalidELFFormat = true;
		return 0;
	}

	fseek(readFile, 0, SEEK_SET);
	return elfHeader;
}

//	typedef struct
//	{
//		Elf64_Word st_name;  /* (4 bytes) Symbol name  */
//		unsigned char st_info;  /* (1 byte) Symbol type and binding */
//		unsigned char st_other; /* (1 byte) Symbol visibility */
//		Elf64_Section st_shndx; /* (2 bytes) Section index */ 
//		Elf64_Addr st_value; /* (8 bytes) Symbol value */
//		Elf64_Xword st_size; /*  (8 bytes) Symbol size */

//	} Elf64_Sym;
