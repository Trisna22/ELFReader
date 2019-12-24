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

private:
	/*   Identification structure from ELF header.   */
	typedef struct ELFHeaderStruct {
		unsigned char magicNumber[4];		// Magic number.
		unsigned char bitSystem;		// Bit system.
		unsigned char endianType;		// Endian type.
		unsigned char ELFVersion;		// ELF version.
		unsigned char targetABI;		// ABI target.
		unsigned char abiVersion;		// ABI version.
		unsigned char unusedSpace[7];		// Padding.
	} ELF_HEADER;

	/*   Program header structure. x32   */
	typedef struct ELF_ProgramHeader32 {
		unsigned int segmentType;		// Segment type.
		unsigned int segmentOffset;		// Segment offset.
		unsigned int virtualAddress;		// Virtual address.
		unsigned int physicalAddress;		// Physical address.
		unsigned int segmentSizeFile;		// Segment size in file image.
		unsigned int segmentSizeMemory;		// Segment size in memory.
		unsigned int segmentFlags;		// Segment flags.
		unsigned int alignment;			// Aligment.
	} ELF_PROGRAMHEADER32;

	/*   Program header structure. x64   */
	typedef struct ELF_ProgramHeader64 {
		unsigned int segmentType;		// Segment type.
		unsigned int segmentFlags;		// Segment flags.
		unsigned long segmentOffset;		// Segment offset.
		unsigned long virtualAddress;		// Virtual address.
		unsigned long physicalAddress;		// Physical address.
		unsigned long segmentSizeFile;		// Segment size in file image.
		unsigned long segmentSizeMemory;	// Segment size in memory.
		unsigned long alignment;		// Aligment.
	} ELF_PROGRAMHEADER64;

	/*   Section header structure. x32   */
	typedef struct ELF_SectionHeader32 {
		unsigned int sectionAddrName;		// Address location of name.
		unsigned int sectionType;		// Section type.
		unsigned int sectionAttributes;		// Section attributes.
		unsigned int virtualAddress;		// Virtual address.
		unsigned int offset;			// Offset.
		unsigned int sectionSizeFile;		// Size of section in file.
		unsigned int sectionIndex;		// Section index.
		unsigned int extraInfo;			// Additional information.
		unsigned int requiredAlign;		// Required alignment
		unsigned int entrySize;			// Size of entry.

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
	int GetIntFromBytes(char, char);

protected:
	// ELF header structures.
	ELFHeaderStruct* identifier;
	Elf32_Ehdr* elfHeader32;
	Elf64_Ehdr* elfHeader64;

	// Section header arrays.
	vector<ELF_SECTIONHEADER32> SectionHeaders32;
	vector<ELF_SECTIONHEADER64> SectionHeaders64;

	/*   Read all headers   */
	void readELFHeader();
	void readELFHeader32();
	void readELFHeader64();

	void readProgramHeader();
	void readProgramHeader32();
	void readProgramHeader64();

	void readSectionHeader();
	void readSectionHeader32();
	void readSectionHeader64();

	/*   Read specific headers   */
	void readSectionHeader(string);
	void readSectionHeader(int);
	void printSectionHeader(ELF_SECTIONHEADER32);
	void printSectionHeader(ELF_SECTIONHEADER64);

	/*   Silent functions of reading headers   */
	bool silentReadELFHeader();
	bool silentReadSectionHeaders();
};

#endif // !~ ELFHeader_H

/*   Constructor with string of filename.   */
ELFHeader::ELFHeader(string FileName)
{
	this->readFile = fopen(FileName.c_str(), "rb");
	if (this->readFile == NULL)
	{
		printf("ELFHeader: Failed to open file! Error code: %d\n", errno);
		this->InvalidELFFormat = true;
		return;
	}

	// Check the bitsystem and if is ELF format.
	this->identifier = ReadELF_Identifier();
}

/*   Deconstructor of the class.   */
ELFHeader::~ELFHeader()
{
	if (this->readFile != NULL)
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

/*   Get the bitsystem of the file, also check if is really ELF format.   */
ELFHeader::ELFHeaderStruct* ELFHeader::ReadELF_Identifier()
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

/*   Get int from 2 chars.   */
int ELFHeader::GetIntFromBytes(char byte1, char byte2)
{
	return (byte1 | byte2 << 8);
}

/*   Reading ELF header from file.   */
void ELFHeader::readELFHeader()
{
	printf("╔╔╔═════════════════════════════════════╗╗╗\n");
	printf("║║║┼─────────────ELF Header────────────┼║║║\n");
	printf("╚╚╚═════════════════════════════════════╝╝╝\n\n");

	// Check if class is ready.
	if (IsReady() == false)
	{
		printf("ELF class not ready yet!\n");
		return;
	}

	if (this->identifier->bitSystem == 0x01) {
		readELFHeader32();
	}
	else if (this->identifier->bitSystem == 0x02) {
		readELFHeader64();
	}
}

/*   Reading the ELF header bytes of ELF file. x32   */
void ELFHeader::readELFHeader32()
{
	// Get size of file.
	struct stat st;
	fstat(readFile->_fileno, &st);

	// Read ELF header structure from file.
	char* FileContent = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, readFile->_fileno, 0);
	Elf32_Ehdr* ehdr = (Elf32_Ehdr*)FileContent;

	// Bit system and endian type.
	printf("Bitsystem:\t\t\tx32\n");
	if (this->identifier->endianType == 0x01)
		printf("Endian type:\t\t\tlittle endian (0x01)\n");
	else
		printf("Endian type:\t\t\tbig endian (0x02)\n");

	// ELF/ABI version and ABI target.
	printf("ELF version:\t\t\t%d\n", this->identifier->ELFVersion);
	printf("ABI version:\t\t\t%d\n", this->identifier->abiVersion);
	printf("Target ABI:\t\t\t");
	switch(this->identifier->targetABI)
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
			printf("Unknown (0x%d)\n", this->identifier->targetABI);
			break;
	}

	// Object file type.
	printf("Object file type:\t\t");
	switch(ehdr->e_type)
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
		case 0xff00:
			printf("Lower processor-specific (0xff00)\n");
			break;
		case 0xffff:
			printf("Higher processor-specific (0xffff)\n");
			break;
		default:
			printf("Unknown file object type. 0x%x\n", ehdr->e_type);
			break;
	}

	// Instruction set architecture.
	printf("Architecture (ISA):\t\t");
	switch (ehdr->e_machine)
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
			printf("Unknown architecture (0x%x)\n", ehdr->e_machine);
			break;
	}

	// Original version of ELF.
	printf("Original version:\t\t0x%x\n", ehdr->e_version);

	// Entry address of program.
	printf("Entry address:\t\t\t0x%x\n", ehdr->e_entry);

	// Program header offset.
	printf("Program header offset:\t\t%d bytes (0x%x)\n", ehdr->e_phoff, ehdr->e_phoff);

	// Section header offset.
	printf("Section header offset:\t\t%d bytes (0x%x)\n", ehdr->e_shoff, ehdr->e_shoff);

	// Flags.
	printf("Flags:\t\t\t\t0x%x\n", ehdr->e_flags);

	// ELF header size.
	printf("ELF header size: \t\t%d bytes (0x%x)\n", ehdr->e_ehsize, ehdr->e_ehsize);

	// Program header size.
	printf("Program header entry:\t\t%d bytes into file (0x%x)\n", ehdr->e_phentsize, ehdr->e_phentsize);

	// Program header entries.
	printf("Program header entries:\t\t%d\n", ehdr->e_phnum);

	// Section header size.
	printf("Section header size:\t\t%d bytes (0x%x)\n", ehdr->e_shentsize, ehdr->e_shentsize);

	// Section header entries.
	printf("Section header entries:\t\t%d\n", ehdr->e_shnum);

	// Section header strings index.
	printf("Section header names index:\t%d\n\n", ehdr->e_shstrndx);

	this->elfHeader32 = ehdr;
}

/*   Reading the ELF header bytes of ELF file. x64   */
void ELFHeader::readELFHeader64()
{
	// Get size of file.
	struct stat st;
	fstat(readFile->_fileno, &st);

	// Read ELF header structure from file.
	char* FileContent = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, readFile->_fileno, 0);
	Elf64_Ehdr* ehdr = (Elf64_Ehdr*)FileContent;

	// Bit system and endian type.
	printf("Bitsystem:\t\t\tx64\n");
	if (this->identifier->endianType == 0x01)
		printf("Endian type:\t\t\tlittle endian (0x01)\n");
	else
		printf("Endian type:\t\t\tbig endian (0x02)\n");

	// ELF/ABI version and ABI target.
	printf("ELF version:\t\t\t%d\n", this->identifier->ELFVersion);
	printf("ABI version:\t\t\t%d\n", this->identifier->abiVersion);
	printf("Target ABI:\t\t\t");
	switch(this->identifier->targetABI)
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
			printf("Unknown (0x%d)\n", this->identifier->targetABI);
			break;
	}

	// Object file type.
	printf("Object file type:\t\t");
	switch(ehdr->e_type)
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
		case 0xff00:
			printf("Lower processor-specific (0xff00)\n");
			break;
		case 0xffff:
			printf("Higher processor-specific (0xffff)\n");
			break;
		default:
			printf("Unknown file object type. 0x%x\n", ehdr->e_type);
			break;
	}

	// Instruction set architecture.
	printf("Architecture (ISA):\t\t");
	switch (ehdr->e_machine)
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
			printf("Unknown architecture (0x%x)\n", ehdr->e_machine);
			break;
	}

	// Original version of ELF.
	printf("Original version:\t\t0x%x\n", ehdr->e_version);

	// Entry address of program.
	printf("Entry address:\t\t\t0x%lx\n", ehdr->e_entry);

	// Program header offset.
	printf("Program header offset:\t\t%ld bytes (0x%lx)\n", ehdr->e_phoff, ehdr->e_phoff);

	// Section header offset.
	printf("Section header offset:\t\t%ld bytes (0x%lx)\n", ehdr->e_shoff, ehdr->e_shoff);

	// Flags.
	printf("Flags:\t\t\t\t0x%x\n", ehdr->e_flags);

	// ELF header size.
	printf("ELF header size: \t\t%d bytes (0x%x)\n", ehdr->e_ehsize, ehdr->e_ehsize);

	// Program header size.
	printf("Program header entry:\t\t%d bytes into file (0x%x)\n", ehdr->e_phentsize, ehdr->e_phentsize);

	// Program header entries.
	printf("Program header entries:\t\t%d\n", ehdr->e_phnum);

	// Section header size.
	printf("Section header size:\t\t%d bytes (0x%x)\n", ehdr->e_shentsize, ehdr->e_shentsize);

	// Section header entries.
	printf("Section header entries:\t\t%d\n", ehdr->e_shnum);

	// Section header strings index.
	printf("Section header names index:\t%d\n\n", ehdr->e_shstrndx);

	this->elfHeader64 = ehdr;
}

/*   Read program header.   */
void ELFHeader::readProgramHeader()
{
	printf("╔╔╔═════════════════════════════════════╗╗╗\n");
	printf("║║║┼───────────Program Header──────────┼║║║\n");
	printf("╚╚╚═════════════════════════════════════╝╝╝\n\n");

	// Check if class is ready.
	if (IsReady() == false)
	{
		printf("ELF class not ready yet!\n");
		return;
	}

	// Bit system specific.
	if (this->identifier->bitSystem == 0x01)
	{
		readProgramHeader32();
	}
	else
	{
		readProgramHeader64();
	}
}
void ELFHeader::readProgramHeader32()
{
	// First change position to given offset.
	fseek(readFile, this->elfHeader32->e_phoff, SEEK_SET);

	// For every entry we can read the program header.
	for (int i = 0; i < this->elfHeader32->e_phnum; i++)
	{
		ELF_PROGRAMHEADER32 programHeader;
		if (fread(&programHeader, sizeof(ELF_PROGRAMHEADER32), 1, this->readFile) == 0)
		{
			printf("ProgramHeader: Failed to read bytes for program header[%d]!\n", i);
			this->InvalidELFFormat = true;
			return;
		}

		printf("Program header [%d]:\n", i);

		// Get the type for this entry.
		printf("  Segment type:\t\t\t");
		switch (programHeader.segmentType)
		{
			case 0x00:
				printf("Entry unused (PT_NULL)\n");
				break;
			case 0x00000001:
				printf("Loadable segment (PT_LOAD)\n");
				break;
			case 0x00000002:
				printf("Dynamic linking information (PT_DYNAMIC)\n");
				break;
			case 0x00000003:
				printf("Interpreter information (PT_INTERP)\n");
				break;
			case 0x00000004:
				printf("Auxillary information (PT_NOTE)\n");
				break;
			case 0x00000005:
				printf("Reserved (PT_SHLIB)\n");
				break;
			case 0x00000006:
				printf("Contains program header table. (PT_PHDR)\n");
				break;
			default:
				printf("Unknown entry 0x%x\n", programHeader.segmentType);
				break;
		}

		// Segment offset.
		printf("  Segment file offset:\t\t0x%d bytes (0x%x)\n", programHeader.segmentOffset, programHeader.segmentOffset);

		// Virtual address of segment.
		printf("  Virtual address:\t\t0x%x\n", programHeader.virtualAddress);

		// Physical address of segment.
		printf("  Physical address:\t\t0%d bytes (0x%x)\n", programHeader.physicalAddress, programHeader.physicalAddress);

		// Size in bytes in the file image.
		printf("  Segment size:\t\t\t%d bytes (0x%x)\n", programHeader.segmentSizeFile, programHeader.segmentSizeFile);

		// Size in bytes in memory.
		printf("  Memorysize segment:\t\t%d bytes (0x%x)\n", programHeader.segmentSizeMemory, programHeader.segmentSizeMemory);

		// Flags.
		printf("  Flags:\t\t\t0x%x\n", programHeader.segmentFlags);

		// Alignment.
		printf("  Alignment:\t\t\t0x%x\n\n", programHeader.alignment);
	}
}
void ELFHeader::readProgramHeader64()
{
	// First change position to given offset.
	fseek(readFile, this->elfHeader64->e_phoff, SEEK_SET);

	// For every entry we can read the program header.
	for (int i = 0; i < this->elfHeader64->e_phnum; i++)
	{
		ELF_PROGRAMHEADER64 programHeader;
		if (fread(&programHeader, sizeof(ELF_PROGRAMHEADER64), 1, readFile) == 0)
		{
			printf("ProgramHeader: Failed to read bytes for program header[%d]!\n", i);
			this->InvalidELFFormat = true;
			return;
		}

		printf("Program header [%d]:\n", i);

		// Get the type for this entry.
		printf("  Segment type:\t\t\t");
		switch (programHeader.segmentType)
		{
			case 0x00:
				printf("Entry unused (PT_NULL)\n");
				break;
			case 0x00000001:
				printf("Loadable segment (PT_LOAD)\n");
				break;
			case 0x00000002:
				printf("Dynamic linking information (PT_DYNAMIC)\n");
				break;
			case 0x00000003:
				printf("Interpreter information (PT_INTERP)\n");
				break;
			case 0x00000004:
				printf("Auxillary information (PT_NOTE)\n");
				break;
			case 0x00000005:
				printf("Reserved (PT_SHLIB)\n");
				break;
			case 0x00000006:
				printf("Contains program header table. (PT_PHDR)\n");
				break;
			default:
				printf("Unknown entry 0x%x\n", programHeader.segmentType);
				break;
		}

		// Segment offset.
		printf("  Segment file offset:\t\t0x%ld bytes (0x%lx)\n", programHeader.segmentOffset, programHeader.segmentOffset);

		// Virtual address of segment.
		printf("  Virtual address:\t\t0x%lx\n", programHeader.virtualAddress);

		// Physical address of segment.
		printf("  Physical address:\t\t0%ld bytes (0x%lx)\n", programHeader.physicalAddress, programHeader.physicalAddress);

		// Size in bytes in the file image.
		printf("  Segment size:\t\t\t%ld bytes (0x%lx)\n", programHeader.segmentSizeFile, programHeader.segmentSizeFile);

		// Size in bytes in memory.
		printf("  Memorysize segment:\t\t%ld bytes (0x%lx)\n", programHeader.segmentSizeMemory, programHeader.segmentSizeMemory);

		// Flags.
		printf("  Flags:\t\t\t0x%x\n", programHeader.segmentFlags);

		// Alignment.
		printf("  Alignment:\t\t\t0x%lx\n\n", programHeader.alignment);
	}
}

/*   Read section header.   */
void ELFHeader::readSectionHeader()
{
	printf("╔╔╔═════════════════════════════════════╗╗╗\n");
	printf("║║║┼───────────Section Header──────────┼║║║\n");
	printf("╚╚╚═════════════════════════════════════╝╝╝\n\n");

	// Check if class is ready.
	if (IsReady() == false)
	{
		printf("ELF class not ready yet!\n");
		return;
	}

	// Bit system specific.
        if (this->identifier->bitSystem == 0x01)
	{
		readSectionHeader32();
	}
	else
	{
		readSectionHeader64();
	}
}
void ELFHeader::readSectionHeader32()
{
	// Get file size.
	struct stat st;
	fstat(this->readFile->_fileno, &st);

	// First change position to given offset.
	fseek(this->readFile, 0, SEEK_SET);

	// Read file again, in memory.
	char* p = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE,
		this->readFile->_fileno, 0); // File int and offset.
	Elf32_Shdr* shdr = (Elf32_Shdr*)(p + this->elfHeader32->e_shoff);

	// Get the names from string table.
	Elf32_Shdr* sh_strtab = &shdr[this->elfHeader32->e_shstrndx];
	const char* const sh_strtab_p = p + sh_strtab->sh_offset;

	// Set file pointer to the section offset again.
	fseek(this->readFile, this->elfHeader32->e_shoff, SEEK_SET);

	// Loop trough file and print section info.
	for (int i = 0; i < this->elfHeader32->e_shnum; i++)
	{
		// Read the section header with our struct.
		ELF_SECTIONHEADER32 sectionHeader;
		if (fread(&sectionHeader, sizeof(ELF_SECTIONHEADER32), 1, readFile) == 0)
		{
			printf("SectionHeader: Failed to read bytes for section header!\n");
			this->InvalidELFFormat = true;
			return;
		}

		// Add header to array of headers.
		this->SectionHeaders32.push_back(sectionHeader);

		// Section name and address.
		printf("Section header [%d]:\n", i);
		printf("  Name:\t\t\t\t%s\n", sh_strtab_p + shdr[i].sh_name);
		printf("  Address of name:\t\t%d bytes in header (0x%x)\n",
			sectionHeader.sectionAddrName, sectionHeader.sectionAddrName);

		// Section type.
		printf("  Section type: \t\t");
		switch (sectionHeader.sectionType)
		{
			case 0x0:
				printf("Section table entry unused\n");
				break;
			case 0x1:
				printf("Program data\n");
				break;
			case 0x2:
				printf("Symbol table\n");
				break;
			case 0x3:
				printf("String table\n");
				break;
			case 0x4:
				printf("Relocation entries (addends)\n");
				break;
			case 0x5:
				printf("Hash table\n");
				break;
			case 0x6:
				printf("Dynamic linking table\n");
				break;
			case 0x7:
				printf("Notes/comments\n");
				break;
			case 0x8:
				printf("Uninitialized space\n");
				break;
			case 0x9:
				printf("Relocation entries (no addens)\n");
				break;
			case 0x0A:
				printf("Reserved\n");
				break;
			case 0x0B:
				printf("Dynamic linker symbol table\n");
				break;
			case 0x0E:
				printf("Array of constructors\n");
				break;
			case 0x0F:
				printf("Array of deconstructors\n");
				break;
			case 0x10:
				printf("Array of pre-constructors\n");
				break;
			case 0x11:
				printf("Section group\n");
				break;
			case 0x12:
				printf("Extended section\n");
				break;
			case 0x13:
				printf("Number of defined types\n");

			default:
				printf("Unknown section table entry\n");
				break;
		}

		// Section attributes.
		printf("  Attributes: \t\t\t");
		switch (sectionHeader.sectionAttributes)
		{
			case 0x1:
				printf("Writeable\n");
				break;
			case 0x2:
				printf("Occupies memory during execution\n");
				break;
			case 0x4:
				printf("Executable\n");
				break;
			case 0x10:
				printf("Might be merged\n");
				break;
			case 0x20:
				printf("Contains null terminated strings\n");
				break;
			case 0x30:
				printf("Contains indexes\n");
				break;
			case 0x80:
				printf("Preserve order after combining\n");
				break;
			case 0x100:
				printf("Non-standard OS checking\n");
				break;
			case 0x200:
				printf("Section is member of group\n");
				break;
			case 0x400:
				printf("Section hold thread-local data\n");
				break;
			case 0x0ff00000:
				printf("OS specific\n");
				break;
			case 0xf0000000:
				printf("Processor specific\n");
				break;
			default:
				printf("Unknown attributes\n");
				break;
		}

		// Virtual address.
		printf("  Virtual address:\t\t0x%x\n", sectionHeader.virtualAddress);

		// Offset of section in file.
		printf("  File offset: \t\t\t%d bytes (0x%x)\n", sectionHeader.offset, sectionHeader.offset);

		// Size of section.
		printf("  Section size: \t\t%d bytes (0x%x)\n", sectionHeader.sectionSizeFile, sectionHeader.sectionSizeFile);

		// Index of section.
		printf("  Section index:\t\t%d\n", sectionHeader.sectionIndex);

		// Extra info.
		printf("  Extra info:\t\t\t0x%x\n", sectionHeader.extraInfo);

		// Required alignment.
		printf("  Required alignment:\t\t%d\n", sectionHeader.requiredAlign);

		// Entry size.
		printf("  Entry size: \t\t\t%d bytes (0x%x)\n\n", sectionHeader.entrySize, sectionHeader.entrySize);
	}

}
void ELFHeader::readSectionHeader64()
{
	// Get file size.
	struct stat st;
	fstat(this->readFile->_fileno, &st);

	// First change position to given offset.
	fseek(this->readFile, 0, SEEK_SET);

	// Read file again, in memory.
	char* p = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE,
		this->readFile->_fileno, 0); // File int and offset.
	Elf64_Shdr* shdr = (Elf64_Shdr*)(p + this->elfHeader64->e_shoff);

	// Get the names from string table.
	Elf64_Shdr* sh_strtab = &shdr[this->elfHeader64->e_shstrndx];
	const char* const sh_strtab_p = p + sh_strtab->sh_offset;

	// Set file pointer to the section offset again.
	fseek(this->readFile, this->elfHeader64->e_shoff, SEEK_SET);

	// Loop trough file and print section info.
	for (int i = 0; i < this->elfHeader64->e_shnum; i++)
	{
		// Read the section header with our struct.
		ELF_SECTIONHEADER64 sectionHeader;
		if (fread(&sectionHeader, sizeof(ELF_SECTIONHEADER64), 1, readFile) == 0)
		{
			printf("SectionHeader: Failed to read bytes for section header!\n");
			this->InvalidELFFormat = true;
			return;
		}

		// Add header to vector of headers.
		this->SectionHeaders64.push_back(sectionHeader);

		// Section name and address.
		printf("Section header [%d]:\n", i);
		printf("  Name:\t\t\t\t%s\n", sh_strtab_p + shdr[i].sh_name);
		printf("  Address of name:\t\t%d bytes in header (0x%x)\n",
			sectionHeader.sectionAddrName, sectionHeader.sectionAddrName);

		// Section type.
		printf("  Section type: \t\t");
		switch (sectionHeader.sectionType)
		{
			case 0x0:
				printf("Section table entry unused\n");
				break;
			case 0x1:
				printf("Program data\n");
				break;
			case 0x2:
				printf("Symbol table\n");
				break;
			case 0x3:
				printf("String table\n");
				break;
			case 0x4:
				printf("Relocation entries (addends)\n");
				break;
			case 0x5:
				printf("Hash table\n");
				break;
			case 0x6:
				printf("Dynamic linking table\n");
				break;
			case 0x7:
				printf("Notes/comments\n");
				break;
			case 0x8:
				printf("Uninitialized space\n");
				break;
			case 0x9:
				printf("Relocation entries (no addens)\n");
				break;
			case 0x0A:
				printf("Reserved\n");
				break;
			case 0x0B:
				printf("Dynamic linker symbol table\n");
				break;
			case 0x0E:
				printf("Array of constructors\n");
				break;
			case 0x0F:
				printf("Array of deconstructors\n");
				break;
			case 0x10:
				printf("Array of pre-constructors\n");
				break;
			case 0x11:
				printf("Section group\n");
				break;
			case 0x12:
				printf("Extended section\n");
				break;
			case 0x13:
				printf("Number of defined types\n");

			default:
				printf("Unknown section table entry\n");
				break;
		}

		// Section attributes.
		printf("  Attributes: \t\t\t");
		switch (sectionHeader.sectionAttributes)
		{
			case 0x1:
				printf("Writeable\n");
				break;
			case 0x2:
				printf("Occupies memory during execution\n");
				break;
			case 0x4:
				printf("Executable\n");
				break;
			case 0x10:
				printf("Might be merged\n");
				break;
			case 0x20:
				printf("Contains null terminated strings\n");
				break;
			case 0x30:
				printf("Contains indexes\n");
				break;
			case 0x80:
				printf("Preserve order after combining\n");
				break;
			case 0x100:
				printf("Non-standard OS checking\n");
				break;
			case 0x200:
				printf("Section is member of group\n");
				break;
			case 0x400:
				printf("Section hold thread-local data\n");
				break;
			case 0x0ff00000:
				printf("OS specific\n");
				break;
			case 0xf0000000:
				printf("Processor specific\n");
				break;
			default:
				printf("Unknown attributes\n");
				break;
		}

		// Virtual address.
		printf("  Virtual address:\t\t0x%lx\n", sectionHeader.virtualAddress);

		// Offset of section in file.
		printf("  File offset: \t\t\t%ld bytes (0x%lx)\n", sectionHeader.offset, sectionHeader.offset);

		// Size of section.
		printf("  Section size: \t\t%ld bytes (0x%lx)\n", sectionHeader.sectionSizeFile, sectionHeader.sectionSizeFile);

		// Index of section.
		printf("  Section index:\t\t%d\n", sectionHeader.sectionIndex);

		// Extra info.
		printf("  Extra info:\t\t\t0x%x\n", sectionHeader.extraInfo);

		// Required alignment.
		printf("  Required alignment:\t\t%ld\n", sectionHeader.requiredAlign);

		// Entry size.
		printf("  Entry size: \t\t\t%ld bytes (0x%lx)\n\n", sectionHeader.entrySize, sectionHeader.entrySize);
	}

}

/*   Reads one section header from list. (name specific)   */
void ELFHeader::readSectionHeader(string sectionName)
{
	if (IsReady() == false)
	{
		printf("ELF class not ready yet!\n");
		return;
	}

	// Get file size.
	struct stat st;
	fstat(this->readFile->_fileno, &st);

	// First change position to given offset.
	fseek(this->readFile, 0, SEEK_SET);

	if (this->identifier->bitSystem == 0x01)
	{
		// We need to get the names from string table.
		char* p = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE,
			this->readFile->_fileno, 0); // File int and offset.
		Elf32_Shdr* shdr = (Elf32_Shdr*)(p + this->elfHeader32->e_shoff);

		// Get the names from string table.
		Elf32_Shdr* sh_strtab = &shdr[this->elfHeader32->e_shstrndx];
		const char* const sh_strtab_p = p + sh_strtab->sh_offset;

		fseek(this->readFile, this->elfHeader32->e_shoff, SEEK_SET);

		for (int i = 0; i < this->elfHeader32->e_shnum; i++)
		{
			ELF_SECTIONHEADER32 section;
			if (fread(&section, sizeof(ELF_SECTIONHEADER32), 1, this->readFile) == 0)
			{
				printf("Silent SectionHeader: Failed to read bytes for section header!\n");
				this->InvalidELFFormat = true;
				return;
			}

			this->SectionHeaders32.push_back(section);

			// If the name doesn't fit with the section table index, continue.
			string name = sh_strtab_p + shdr[i].sh_name;
			if (name != sectionName)
				continue;

			// If the name does fit, print out the packet.
			printf("NAME: %s\n", name.c_str());
			printSectionHeader(section);
			return;
		}

		// If the section header is not found.
		printf("%s section not found!\n\n", sectionName.c_str());
		return;
	}
	else
	{
		// We need to get the names from string table.
		char* p = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE,
			this->readFile->_fileno, 0); // File int and offset.
		Elf64_Shdr* shdr = (Elf64_Shdr*)(p + this->elfHeader64->e_shoff);

		// Get the names from string table.
		Elf64_Shdr* sh_strtab = &shdr[this->elfHeader64->e_shstrndx];
		const char* const sh_strtab_p = p + sh_strtab->sh_offset;

		fseek(this->readFile, this->elfHeader64->e_shoff, SEEK_SET);

		for (int i = 0; i < this->elfHeader64->e_shnum; i++)
		{
			ELF_SECTIONHEADER64 section;
			if (fread(&section, sizeof(ELF_SECTIONHEADER64), 1, this->readFile) == 0)
			{
				printf("Silent SectionHeader: Failed to read bytes for section header!\b");
				this->InvalidELFFormat = true;
				return;
			}

			this->SectionHeaders64.push_back(section);

			// If the name doesn't fit with the section table index, continue.
			string name = sh_strtab_p + shdr[i].sh_name;
			if (name != sectionName)
				continue;

			// If the name does fit, print out the packet.
			printf("NAME: %s\n", name.c_str());
			printSectionHeader(section);
			return;
		}

		// If the section header is not found.
		printf("%s section not found!\n\n", sectionName.c_str());
		return;
	}
}

/*   Reads one section header from list. (index specific)   */
void ELFHeader::readSectionHeader(int index)
{
	printf("INDEX: %d\n", index);
	if (IsReady() == false)
	{
		printf("ELF class not ready yet!\n");
		return;
	}

	// Get file size.
	struct stat st;
	fstat(this->readFile->_fileno, &st);

	// First change position to given offset.
	fseek(this->readFile, 0, SEEK_SET);

	if (this->identifier->bitSystem == 0x01)
	{
		// Read file again, in memory.
		char* p = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE,
			this->readFile->_fileno, 0); // File int and offset.
		Elf32_Shdr* shdr = (Elf32_Shdr*)(p + this->elfHeader32->e_shoff);

		// Get the names from string table.
		Elf32_Shdr* sh_strtab = &shdr[this->elfHeader32->e_shstrndx];
		const char* const sh_strtab_p = p + sh_strtab->sh_offset;

		fseek(this->readFile, this->elfHeader32->e_shoff, SEEK_SET);

		if (this->elfHeader32->e_shnum < index)
		{
			printf("Index doesn't exists!\n");
			return;
		}

		for (int i = 0; i < this->elfHeader32->e_shnum; i++)
		{
			ELF_SECTIONHEADER32 section;
			if (fread(&section, sizeof(ELF_SECTIONHEADER32), 1, this->readFile) == 0)
			{
				printf("Silent SectionHeader: Failed to read bytes for section header!\n");
				this->InvalidELFFormat = true;
				return;
			}

			this->SectionHeaders32.push_back(section);

		}

		printf("  Name:\t\t\t\t%s\n", sh_strtab_p + shdr[index].sh_name);
		printSectionHeader(this->SectionHeaders32.at(index));
		return;
	}
	else
	{
		// Read file again, in memory.
		char* p = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE,
			this->readFile->_fileno, 0); // File int and offset.
		Elf64_Shdr* shdr = (Elf64_Shdr*)(p + this->elfHeader64->e_shoff);

		// Get the names from string table.
		Elf64_Shdr* sh_strtab = &shdr[this->elfHeader64->e_shstrndx];
		const char* const sh_strtab_p = p + sh_strtab->sh_offset;

		fseek(this->readFile, this->elfHeader64->e_shoff, SEEK_SET);

		if (this->elfHeader64->e_shnum < index)
		{
			printf("Index doesn't exists!\n");
			return;
		}

		for (int i = 0; i < this->elfHeader64->e_shnum; i++)
		{
			ELF_SECTIONHEADER64 section;
			if (fread(&section, sizeof(ELF_SECTIONHEADER64), 1, this->readFile) == 0)
			{
				printf("Silent SectionHeader: Failed to read bytes for section header!\b");
				this->InvalidELFFormat = true;
				return;
			}

			this->SectionHeaders64.push_back(section);

		}

		printf("  Name:\t\t\t\t%s\n", sh_strtab_p + shdr[index].sh_name);
		printSectionHeader(this->SectionHeaders64.at(index));
		return;
	}
}

/*   Print out the specified section header.   */
void ELFHeader::printSectionHeader(ELF_SECTIONHEADER32 sectionHeader)
{
	printf("  Address of name:\t\t%d bytes in header (0x%x)\n",
		sectionHeader.sectionAddrName, sectionHeader.sectionAddrName);

	// Section type.
	printf("  Section type: \t\t");
	switch (sectionHeader.sectionType)
	{
		case 0x0:
			printf("Section table entry unused\n");
			break;
		case 0x1:
			printf("Program data\n");
			break;
		case 0x2:
			printf("Symbol table\n");
			break;
		case 0x3:
			printf("String table\n");
			break;
		case 0x4:
			printf("Relocation entries (addends)\n");
			break;
		case 0x5:
			printf("Hash table\n");
			break;
		case 0x6:
			printf("Dynamic linking table\n");
			break;
		case 0x7:
			printf("Notes/comments\n");
			break;
		case 0x8:
			printf("Uninitialized space\n");
			break;
		case 0x9:
			printf("Relocation entries (no addens)\n");
			break;
		case 0x0A:
			printf("Reserved\n");
			break;
		case 0x0B:
			printf("Dynamic linker symbol table\n");
			break;
		case 0x0E:
			printf("Array of constructors\n");
			break;
		case 0x0F:
			printf("Array of deconstructors\n");
			break;
		case 0x10:
			printf("Array of pre-constructors\n");
			break;
		case 0x11:
			printf("Section group\n");
			break;
		case 0x12:
			printf("Extended section\n");
			break;
		case 0x13:
			printf("Number of defined types\n");

		default:
			printf("Unknown section table entry\n");
			break;
	}

	// Section attributes.
	printf("  Attributes: \t\t\t");
	switch (sectionHeader.sectionAttributes)
	{
		case 0x1:
			printf("Writeable\n");
			break;
		case 0x2:
			printf("Occupies memory during execution\n");
			break;
		case 0x4:
			printf("Executable\n");
			break;
		case 0x10:
			printf("Might be merged\n");
			break;
		case 0x20:
			printf("Contains null terminated strings\n");
			break;
		case 0x30:
			printf("Contains indexes\n");
			break;
		case 0x80:
			printf("Preserve order after combining\n");
			break;
		case 0x100:
			printf("Non-standard OS checking\n");
			break;
		case 0x200:
			printf("Section is member of group\n");
			break;
		case 0x400:
			printf("Section hold thread-local data\n");
			break;
		case 0x0ff00000:
			printf("OS specific\n");
			break;
		case 0xf0000000:
			printf("Processor specific\n");
			break;
		default:
			printf("Unknown attributes\n");
			break;
	}

	// Virtual address.
	printf("  Virtual address:\t\t0x%x\n", sectionHeader.virtualAddress);

	// Offset of section in file.
	printf("  File offset: \t\t\t%d bytes (0x%x)\n", sectionHeader.offset, sectionHeader.offset);

	// Size of section.
	printf("  Section size: \t\t%d bytes (0x%x)\n", sectionHeader.sectionSizeFile, sectionHeader.sectionSizeFile);

	// Index of section.
	printf("  Section index:\t\t%d\n", sectionHeader.sectionIndex);

	// Extra info.
	printf("  Extra info:\t\t\t0x%x\n", sectionHeader.extraInfo);

	// Required alignment.
	printf("  Required alignment:\t\t%d\n", sectionHeader.requiredAlign);

	// Entry size.
	printf("  Entry size: \t\t\t%d bytes (0x%x)\n\n", sectionHeader.entrySize, sectionHeader.entrySize);
}
void ELFHeader::printSectionHeader(ELF_SECTIONHEADER64 sectionHeader)
{
	// Section name and address.
	printf("  Address of name:\t\t%d bytes in header (0x%x)\n",
		sectionHeader.sectionAddrName, sectionHeader.sectionAddrName);

	// Section type.
	printf("  Section type: \t\t");
	switch (sectionHeader.sectionType)
	{
		case 0x0:
			printf("Section table entry unused\n");
			break;
		case 0x1:
			printf("Program data\n");
			break;
		case 0x2:
			printf("Symbol table\n");
			break;
		case 0x3:
			printf("String table\n");
			break;
		case 0x4:
			printf("Relocation entries (addends)\n");
			break;
		case 0x5:
			printf("Hash table\n");
			break;
		case 0x6:
			printf("Dynamic linking table\n");
			break;
		case 0x7:
			printf("Notes/comments\n");
			break;
		case 0x8:
			printf("Uninitialized space\n");
			break;
		case 0x9:
			printf("Relocation entries (no addens)\n");
			break;
		case 0x0A:
			printf("Reserved\n");
			break;
		case 0x0B:
			printf("Dynamic linker symbol table\n");
			break;
		case 0x0E:
			printf("Array of constructors\n");
			break;
		case 0x0F:
			printf("Array of deconstructors\n");
			break;
		case 0x10:
			printf("Array of pre-constructors\n");
			break;
		case 0x11:
			printf("Section group\n");
			break;
		case 0x12:
			printf("Extended section\n");
			break;
		case 0x13:
			printf("Number of defined types\n");

		default:
			printf("Unknown section table entry\n");
			break;
	}

	// Section attributes.
	printf("  Attributes: \t\t\t");
	switch (sectionHeader.sectionAttributes)
	{
		case 0x1:
			printf("Writeable\n");
			break;
		case 0x2:
			printf("Occupies memory during execution\n");
			break;
		case 0x4:
			printf("Executable\n");
			break;
		case 0x10:
			printf("Might be merged\n");
			break;
		case 0x20:
			printf("Contains null terminated strings\n");
			break;
		case 0x30:
			printf("Contains indexes\n");
			break;
		case 0x80:
			printf("Preserve order after combining\n");
			break;
		case 0x100:
			printf("Non-standard OS checking\n");
			break;
		case 0x200:
			printf("Section is member of group\n");
			break;
		case 0x400:
			printf("Section hold thread-local data\n");
			break;
		case 0x0ff00000:
			printf("OS specific\n");
			break;
		case 0xf0000000:
			printf("Processor specific\n");
			break;
		default:
			printf("Unknown attributes\n");
			break;
	}

	// Virtual address.
	printf("  Virtual address:\t\t0x%lx\n", sectionHeader.virtualAddress);

	// Offset of section in file.
	printf("  File offset: \t\t\t%ld bytes (0x%lx)\n", sectionHeader.offset, sectionHeader.offset);

	// Size of section.
	printf("  Section size: \t\t%ld bytes (0x%lx)\n", sectionHeader.sectionSizeFile, sectionHeader.sectionSizeFile);

	// Index of section.
	printf("  Section index:\t\t%d\n", sectionHeader.sectionIndex);

	// Extra info.
	printf("  Extra info:\t\t\t0x%x\n", sectionHeader.extraInfo);

	// Required alignment.
	printf("  Required alignment:\t\t%ld\n", sectionHeader.requiredAlign);

	// Entry size.
	printf("  Entry size: \t\t\t%ld bytes (0x%lx)\n\n", sectionHeader.entrySize, sectionHeader.entrySize);
}

/*   Reading the headers in silent. (without output)   */
bool ELFHeader::silentReadELFHeader()
{
	if (IsReady() == false)
	{
		printf("ELF class not ready yet!\n");
		return false;
	}

	// Change pointer to beginning of file.
	fseek(this->readFile, 0, SEEK_SET);

	// Get size of file.
	struct stat st;
	fstat(this->readFile->_fileno, &st);

	// Read file.
	char* p = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE,
		this->readFile->_fileno, 0);

	// Based on bit system.
	if (this->identifier->bitSystem == 0x01)
	{
		this->elfHeader32 = (Elf32_Ehdr*)p;
	}
	else
	{
		this->elfHeader64 = (Elf64_Ehdr*)p;
	}

	return true;
}
bool ELFHeader::silentReadSectionHeaders()
{
	if (IsReady() == false)
	{
		printf("ELF class not ready yet!\n");
		return false;
	}

	if (this->identifier->bitSystem == 0x01)
	{
		fseek(this->readFile, this->elfHeader32->e_shoff, SEEK_SET);

		for (int i = 0; i < this->elfHeader32->e_shnum; i++)
		{
			ELF_SECTIONHEADER32 section;
			if (fread(&section, sizeof(ELF_SECTIONHEADER32), 1, this->readFile) == 0)
			{
				printf("Silent SectionHeader: Failed to read bytes for section header!\b");
				this->InvalidELFFormat = true;
				return false;
			}

			this->SectionHeaders32.push_back(section);
		}

		return true;
	}
	else
	{
		fseek(this->readFile, this->elfHeader64->e_shoff, SEEK_SET);

		for (int i = 0; i < this->elfHeader64->e_shnum; i++)
		{
			ELF_SECTIONHEADER64 section;
			if (fread(&section, sizeof(ELF_SECTIONHEADER64), 1, this->readFile) == 0)
			{
				printf("Silent SectionHeader: Failed to read bytes for section header!\b");
				this->InvalidELFFormat = true;
				return false;
			}

			this->SectionHeaders64.push_back(section);
		}

		return true;
	}

	// Change pointer to beginning of file.
	fseek(this->readFile, 0, SEEK_SET);
}
