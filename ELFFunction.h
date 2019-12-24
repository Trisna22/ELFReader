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

	/*   Section header structure. x32   */
        typedef struct ELF_SectionHeader32 {
                unsigned int sectionAddrName;           // Address location of name.
                unsigned int sectionType;               // Section type.
                unsigned int sectionAttributes;         // Section attributes.
                unsigned int virtualAddress;            // Virtual address.
                unsigned int offset;                    // Offset.
                unsigned int sectionSizeFile;           // Size of section in file.
                unsigned int sectionIndex;              // Section index.
                unsigned int extraInfo;                 // Additional information.
                unsigned int requiredAlign;             // Required alignment
                unsigned int entrySize;                 // Size of entry.

        } ELF_SECTIONHEADER32;

        /*   Section header structure. x64   */
        typedef struct ELF_SectionHeader64 {
                unsigned int sectionAddrName;
                unsigned int sectionType;
                unsigned long sectionAttributes;
                unsigned long virtualAddress;
                unsigned long offset;
                unsigned long sectionSizeFile;
                unsigned int sectionIndex;
                unsigned int extraInfo;
                unsigned long requiredAlign;
                unsigned long entrySize;
        } ELF_SECTIONHEADER64;

	ELFHeaderStruct* ReadELF_Identifier();
protected:
	ELFHeaderStruct* identifier;

	vector<Elf32_Sym> Symbols32;
	vector<Elf64_Sym> Symbols64;

	/*   Read all symbols   */
	void readSymbols();
	void readSymbols32();
	void readSymbols64();

	/*   Silent functions of reading headers   */
        bool silentReadELFHeader();
        bool silentReadSectionHeaders();
        bool silentReadProgramHeaders();
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

	return true;
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

/*   Read all symbols.   */
void ELFFunction::readSymbols()
{
	printf("SYMBOLS\n");

	// Read offset of section table with index of symbol table.

	// Elf32_Sym structure.

	//printing out symbols.
}
void ELFFunction::readSymbols32()
{

}
void ELFFunction::readSymbols64()
{

}


/*   Silent functions of reading headers   */
bool ELFFunction::silentReadELFHeader()
{
	return false;
}
bool ELFFunction::silentReadSectionHeaders()
{
	return false;
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

