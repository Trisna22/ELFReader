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

	int GetIntFromBytes(char, char);

private:
	/*   ELF file format header structure.   */
	typedef struct ELF_Header {
		unsigned char magicNumber[4];	// Identifier of ELF format
		unsigned char bitSystem;	// 1 = 32 bit, 2 = 64 bit
		unsigned char endianType; 	// 1 = little endian, 2 = big endian
		unsigned char ELFversion;	// 1 = original version
		unsigned char targetABI;	// Target ABI OS
		unsigned char ABIVersion;	// ABI version
		unsigned char unUsedspace[7];	// Padding
		unsigned char objFileType[2];	// Object file type.
		unsigned char architecture[2];	// Architecture type.
		unsigned int origVersion;	// Original version.
	} ELF_HEADER;

	/*   32 bit system addresses.   */
	typedef struct ELF32_MemoryAddresses {
		unsigned int entryAddress;		// The entry point address for execution.
		unsigned int programHeaderTable;	// Mostly 0x34, right after the header table.
		unsigned int sectionAddress;		// The start of section header table.
	} ELF32_MEMADDRS;

	/*   64 bit system addresses.   */
	typedef struct ELF64_MemoryAddresses {
		unsigned long entryAddress;		// The entry point address for execution.
		unsigned long programHeaderTable;	// Mostly 0x40, right after the header table.
		unsigned long sectionAddress;		// The start of section header table.
	} ELF64_MEMADDRS;

	/*   The rest of the ELF header structure.   */
	typedef struct ELF_HeaderRest {
		unsigned char flags[4];			// Flags of header.
		unsigned char thisHeaderSize[2];	// Size of this header.
		unsigned char programHeaderEntry[2];	// Program header size.
		unsigned char programHeaderCount[2];	// Entries of program header table.
		unsigned char sectionHeaderSize[2];	// Section header size.
		unsigned char sectionHeaderCount[2];	// Section number entries.
		unsigned char sectionHeaderIndex[2];	// Index of section names.
	} ELF_HEADERREST;


	/*   Program header structure.   */
	typedef struct ELF_ProgramHeader32 {
		unsigned int segmentType;		// Segment type.
		unsigned int segmentOffset;		// Segment offset.
		unsigned int VirtualAddress;		// Virtual address.
		unsigned int physicalAddress;		// Physical address.
		unsigned int segmentSize;		// Segment size.
	} ELF_PROGRAMHEADER32;

	typedef struct ELF_ProgramHeader64 {
		unsigned int segmentType;		// Segment type.
		unsigned int segmentFlags;		// Segment flags.
		unsigned long segmentOffset;		// Segment offset.
	} ELF_PROGRAMHEADER64;

	/*   Section header structure.   */
protected:
	void readELFHeader();
	void readProgramHeader();
	void readSectionHeader();
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

/*   Get int from 2 chars.   */
int ELFHeader::GetIntFromBytes(char byte1, char byte2)
{
	return (byte1 | byte2 << 8);
}

/*   Reading the ELF header bytes of ELF file.   */
void ELFHeader::readELFHeader()
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
	ELF_Header identifier;
	int status = fread(&identifier, sizeof(ELF_Header), 1, readFile);
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
		printf(":Bit System: \t\t\tx32 (0x01)\n");
	else if (identifier.bitSystem == 0x02)
		printf(":Bit System: \t\t\tx64 (0x02)\n");

	this->BitSystem = identifier.bitSystem;

	// Getting endian type.
	if (!(identifier.endianType == 0x01 || identifier.endianType == 0x02))
	{
		printf("ELFHeader: Unknown endian type: %d\n", identifier.endianType);
		this->InvalidELFFormat = true;
		return;
	}
	else if (identifier.endianType == 0x01)
		printf(":ELF endian: \t\t\tlittle endian (0x01)\n");
	else if (identifier.endianType == 0x02)
		printf(":ELF endian: \t\t\tbig endian (0x02)\n");

	this->Endian = identifier.endianType;

	// ELF version.
	printf(":ELF version:\t\t\t%d\n", identifier.ELFversion);

	// ABI target OS.
	this->targetABI = targetABI;
	printf(":Target ABI:\t\t\t");
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
	printf(":ABI version:\t\t\t%d\n", identifier.ABIVersion);

	// Object file type.
	printf(":Object file type:\t\t");
	switch (GetIntFromBytes(identifier.objFileType[0], identifier.objFileType[1]))
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
	printf(":Architecture(ISA):\t\t");
	switch (GetIntFromBytes(identifier.architecture[0], identifier.architecture[1]))
	{
		case 0x00:
			printf("No specific architecture set (0x00)\n");
			break;
		case 0x02:
			printf("SPARC (0x02)\n");
			break;
		case 0x03:
			printf("x86 (0x03)\n");
			break;
		case 0x08:
			printf("MIPS (0x08)\n");
			break;
		case 0x14:
			printf("PowerPRC (0x14)\n");
			break;
		case 0x16:
			printf("S390 (0x16)\n");
			break;
		case 0x28:
			printf("ARM (0x28)\n");
			break;
		case 0x2A:
			printf("SuperH (0x2A)\n");
			break;
		case 0x32:
			printf("IA-64 (0x32)\n");
			break;
		case 0x3E:
			printf("x86-64 (0x3E)\n");
			break;
		case 0xB7:
			printf("AArch64 (0xB7)\n");
			break;
		case 0xF3:
			printf("RISC-V (0xF3)\n");
			break;
		default:
			printf("Unknown architecture (%p)\n", identifier.architecture);
			break;
	}

	// Original version of ELF.
	printf(":Original version:\t\t%x\n", identifier.origVersion);

	// Memory addressing, this is bit system dependend.
	if (identifier.bitSystem == 0x01)
	{
		ELF32_MEMADDRS memAddresses;
		if (fread(&memAddresses, sizeof(ELF32_MEMADDRS), 1, readFile) == 0)
		{
			printf("ELFHeader: Failed to read x32 memory bytes!\n");
			this->InvalidELFFormat = true;
			return;
		}

		printf("\n:Entry address:\t\t\t0x%x\n", memAddresses.entryAddress);
		printf(":Program header offset:\t\t%d bytes in file\n", memAddresses.programHeaderTable);
		printf(":Section header offset:\t\t%d bytes in file\n", memAddresses.sectionAddress);
	}
	else
	{
		ELF64_MEMADDRS memAddresses;
		if (fread(&memAddresses, sizeof(ELF64_MEMADDRS), 1, readFile) == 0)
		{
			printf("ELFHeader: Failed to read x64 memory bytes!\n");
			this->InvalidELFFormat = true;
			return;
		}

		printf("\n:Entry address:\t\t\t0x%lx\n", memAddresses.entryAddress);
		printf(":Program header offset:\t\t%ld bytes in file\n", memAddresses.programHeaderTable);
		printf(":Section header offset:\t\t%ld bytes in file\n", memAddresses.sectionAddress);
	}

	// Reading the rest of the bytes.
	ELF_HEADERREST restHeader;
	if (fread(&restHeader, sizeof(ELF_HEADERREST), 1, readFile) == 0)
	{
		printf("ELFHeader: Failed to read the rest of the ELF header!\n");
		this->InvalidELFFormat = true;
		return;
	}

	// Flags.
	printf(":Flags:\t\t\t\t0x%x\n", GetIntFromBytes(restHeader.flags[0], restHeader.flags[1]));

	// This header size.
	printf("\n:This header size:\t\t%d bytes\n", GetIntFromBytes(restHeader.thisHeaderSize[0],
		restHeader.thisHeaderSize[1]));

	// Program header entry.
	printf(":Program header size:\t\t%d bytes\n", GetIntFromBytes(restHeader.programHeaderEntry[0],
		restHeader.programHeaderEntry[1]));

	// Program header entry count.
	printf(":Program header entries:\t%d\n", GetIntFromBytes(restHeader.programHeaderCount[0],
		restHeader.programHeaderCount[1]));

	// Section header size.
	printf("\n:Section header size:\t\t%d bytes\n", GetIntFromBytes(restHeader.sectionHeaderSize[0],
		restHeader.sectionHeaderSize[1]));

	// Section header entries.
	printf(":Section header entries:\t%d\n", GetIntFromBytes(
		restHeader.sectionHeaderCount[0], restHeader.sectionHeaderCount[1]));

	// Section header index.
	printf("Section string table index:\t%d\n", GetIntFromBytes(
		restHeader.sectionHeaderIndex[0], restHeader.sectionHeaderIndex[1]));
}

/*   Reading the program header of file   */
void ELFHeader::readProgramHeader()
{
	
}

/*   Reading the section header of file   */
void ELFHeader::readSectionHeader()
{

}
