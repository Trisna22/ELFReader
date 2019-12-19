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
private:
	FILE* readFile = NULL;
	bool InvalidELFFormat = false;

	/*   ELF file format header structure.   */
	typedef struct ELF_IdentifierHeader {
		unsigned char magicNumber[4];	// Identifier of ELF format
		unsigned char bitSystem;	// 1 = 32 bit, 2 = 64 bit
		unsigned char endianType; 	// 1 = little endian, 2 = big endian
		unsigned char ELFversion;	// 1 = original version
		unsigned char targetABI;	// Target ABI OS
		unsigned char ABIVersion;	// ABI version
		unsigned char unUsedspace[7];	// Padding
		unsigned char objFileType[2];	// Object file type.
		unsigned char architecture[2];	// Architecture type.
	} ELF_HEADER;

protected:
	void readELF();
	int BitSystem, Endian, targetABI;

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
	// Check if file is opened.
	if (this->readFile == NULL)
		return false;

	// Check if invalid ELF format.
	if (InvalidELFFormat == true)
		return false;

	return true;
}

/*   Reading the first 6 bytes of ELF file.   */
void ELFHeader::readELF()
{
	printf("╔╔╔═════════════════════════════════════╗╗╗\n");
	printf("║║║┼─────────────ELF Header────────────┼║║║\n");
	printf("╚╚╚═════════════════════════════════════╝╝╝\n\n");

	// Check if class is ready.
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
		this->InvalidELFFormat = true;
		return;
	}

	// Reading the magic number.
	if (!(identifier.magicNumber[0] == 0x7F && identifier.magicNumber[1] == 'E' &&
		identifier.magicNumber[2] == 'L' && identifier.magicNumber[3] == 'F'))
	{
		printf("ELFHeader: No magic number detected! Wrong ELF format!\n");
		this->InvalidELFFormat = true;
		return;
	}

	// Checking bitsystem.
	if (!(identifier.bitSystem == 0x01 || identifier.bitSystem == 0x02))
	{
		printf("ELFHeader: Unknown bitsystem: %d\n", identifier.bitSystem);
		this->InvalidELFFormat = true;
		return;
	}
	else if (identifier.bitSystem == 0x01)
		printf(":BitSystem: \t\tx32 (0x01)\n");
	else if (identifier.bitSystem == 0x02)
		printf(":BitSystem: \t\tx64 (0x02)\n");

	this->BitSystem = identifier.bitSystem;

	// Getting endian type.
	if (!(identifier.endianType == 0x01 || identifier.endianType == 0x02))
	{
		printf("ELFHeader: Unknown endian type: %d\n", identifier.endianType);
		this->InvalidELFFormat = true;
		return;
	}
	else if (identifier.endianType == 0x01)
		printf(":ELF endian: \t\tlittle endian (0x01)\n");
	else if (identifier.endianType == 0x02)
		printf(":ELF endian: \t\tbig endian (0x02)\n");

	this->Endian = identifier.endianType;

	// ELF version.
	printf(":ELF version:\t\t%d\n", identifier.ELFversion);

	// ABI target OS.
	this->targetABI = targetABI;
	printf(":ELF target ABI:\t");
	switch(identifier.targetABI)
	{
		case 0x00:
			printf("System V (0x00)\n");
			break;

		case 0x01:
			printf("HP-UX (0x01)\n");
			break;

		case 0x02:
			printf("NetBSD (0x02)\n");
			break;

		case 0x03:
			printf("Linux (0x03)\n");
			break;

		case 0x04:
			printf("GNU Hurd (0x04)\n");
			break;

		case 0x05:
			printf("That's weird, 0x5 doesn't exists!\n");
			break;

		case 0x06:
			printf("Solaris (0x06)\n");
			break;

		case 0x07:
			printf("AIX (0x07)\n");
			break;

		case 0x08:
			printf("IRIX (0x08)\n");
			break;

		case 0x09:
			printf("FreeBSD (0x09)\n");
			break;

		case 0x0A:
			printf("Tru64 (0x0A)\n");
			break;

		case 0x0B:
			printf("Novell Modesto (0x0B)\n");
			break;

		case 0x0C:
			printf("OpenBSD (0x0C)\n");
			break;

		case 0x0D:
			printf("OpenVMS (0x0D)\n");
			break;

		case 0x0E:
			printf("NonStop Kernel (0x0E)\n");
			break;

		case 0x0F:
			printf("AROS (0x0F)\n");
			break;

		case 0x10:
			printf("Fenix OS (0x10)\n");
			break;

		case 0x11:
			printf("CloudABI (0x11)\n");
			break;

		default:
			printf("Unknown (0x%d)\n", identifier.targetABI);
			break;
	}

	// ABI version.
	printf(":ELF ABI version:\t%d\n", identifier.ABIVersion);

	// Object file type.
	printf(":Object file type:\t");
	switch (((int)identifier.objFileType[0] | identifier.objFileType[1] << 8))
	{
		case 0x00:
			printf("No file type (0x00)\n");
			break;
		case 0x01:
			printf("Relocatable file (0x01)\n");
			break;
		case 0x02:
			printf("Executable file (0x02)\n");
			break;
		case 0x03:
			printf("Shared object file (0x03)\n");
			break;
		case 0x04:
			printf("Core file (0x04)\n");
			break;
		case 0xffe00:
			printf("Lower OS-specific (0xffe00)\n");
			break;
		case 0xffeff:
			printf("Higher OS-specific (0xffeff)\n");
			break;
		case 0xff00:
			printf("Lower processor-specific (0xff00)\n");
			break;
		case 0xffff:
			printf("Higher processor-specific (0xffff)\n");
			break;
		default:
			printf("Unknown file object type. %p\n", identifier.objFileType);
	}

	// Instruction set architecture.
	printf(":Machine architecture:\t");
	switch(((int)identifier.architecture[0] | identifier.architecture[1] << 8))
	{
		case 0x00:
			printf("No specific architecture set (0x00)\n");
			break;
		case 0x02:
	}

}
