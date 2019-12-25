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

	ELF_HEADER* ReadELF_Identifier();
	int GetIndexOfSection(string);
protected:
	// ELF header structures.
	ELF_HEADER* identifier;
	Elf32_Ehdr* elfHeader32;
	Elf64_Ehdr* elfHeader64;

	// Section header array.
	vector<ELF_SECTIONHEADER32> SectionHeaders32;
	vector<ELF_SECTIONHEADER64> SectionHeaders64;

	// Symbol table array.
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
ELFFunction::ELF_HEADER* ELFFunction::ReadELF_Identifier()
{
	/*   Reads our structure from file.   */
	ELF_HEADER *elfHeader = new ELF_HEADER();
	int status = fread(elfHeader, sizeof(ELF_HEADER), 1, readFile);
	if (status == 0)
	{
		printf("ELFFunction: failed to read bytes from file!\n");
		this->InvalidELFFormat = true;
		return 0;
	}

	/*   Checks if it is really an ELF structure.   */

	// Based by: magic number
	if (!(elfHeader->magicNumber[0] == 0x7F && elfHeader->magicNumber[1] == 'E' &&
		elfHeader->magicNumber[2] == 'L' && elfHeader->magicNumber[3] == 'F'))
	{
		printf("ELFFunction: No magic number detected! Wrong ELF format!\n");
		this->InvalidELFFormat = true;
		return 0;
	}

	// Based by: bitsystem
	if (!(elfHeader->bitSystem == 0x01 || elfHeader->bitSystem == 0x02))
	{
		printf("ELFFunction: Wrong bitsystem detected! Wrong ELF format or not supported!\n");
		this->InvalidELFFormat = true;
		return 0;
	}

	fseek(readFile, 0, SEEK_SET);
	return elfHeader;
}

/*   Gets the index number from the section table.   */
int ELFFunction::GetIndexOfSection(string sectionName)
{
	// Get size of file.
	struct stat st;
	fstat(this->readFile->_fileno, &st);

	if (this->identifier->bitSystem == 0x01)
	{
		// Read file again, in memory.
		char* p = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE,
			this->readFile->_fileno, 0); // File int and offset.
		Elf32_Shdr* shdr = (Elf32_Shdr*)(p + this->elfHeader32->e_shoff);

		// Get the names from string table.
		Elf32_Shdr* sh_strtab = &shdr[this->elfHeader32->e_shstrndx];
		const char* const sh_strtab_p = p + sh_strtab->sh_offset;

		// Keep looping until we got the right index.
		for (int i = 0; i < this->elfHeader32->e_shnum; i++)
		{
			string name = sh_strtab_p + shdr[i].sh_name;
			if (name == sectionName)
				return i;
		}

		return -1;
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

		// Keep looping until we got the right index.
		for (int i = 0; i < this->elfHeader64->e_shnum; i++)
		{
			string name = sh_strtab_p + shdr[i].sh_name;
			if (name == sectionName)
				return i;
		}

		return -1;
	}
}

/*   Read all symbols.   */
void ELFFunction::readSymbols()
{
	if (silentReadSectionHeaders() == false)
	{
		printf("ELFFunction: Failed to read section headers in silent!\n\n");
		this->InvalidELFFormat = true;
		return;
	}

	if (this->identifier->bitSystem == 0x01)
	{
		readSymbols32();
	}
	else
	{
		readSymbols64();
	}

}
void ELFFunction::readSymbols32()
{
	// Get the symbol table from section header.
	int index = GetIndexOfSection(".symtab");

	// Read offset of section table with index of symbol table.
	int symTableOffset = this->SectionHeaders32.at(index).offset;
	fseek(this->readFile, symTableOffset, SEEK_SET);

	// Size of section .symtab / size of entry (ELF_SECTIONHEADER32::entrySize) = count.
	int countOfSymbols = this->SectionHeaders32.at(index).sectionSizeFile /
		this->SectionHeaders32.at(index).entrySize;
	printf("Counted %d symbols\n", countOfSymbols);

	printf("Index | Address  |   Binding   | Size | Value\n");

	// Loop trough every symbol.
	for (int i = 0; i < countOfSymbols; i++)
	{
		Elf32_Sym symbol;
		if (fread(&symbol, sizeof(Elf32_Sym), 1, this->readFile) == 0)
		{
			printf("ELFFunction: Failed to read symbol [%d]\n\n", i);
			this->InvalidELFFormat = true;
			return;
		}

		/*
			http://www.skyfree.org/linux/references/ELF_Format.pdf

			page 23 (1-17)

			tearing apart the st_info byte into sections.
			And think of a new way to output this.

		*/

		// Symbol name address.
		printf("Symbol [%d]:\n", i);
		printf("  Offset:\t\t%d bytes in string table (0x%x)\n",
			symbol.st_name, symbol.st_name);

		// I guess the same thing with the section table,
		//  get the actual name from the string table.
		if (symbol.st_name == 0)
		{
			printf("  Name:\t\t\n");
		}
		else
		{
			int stringTableIndex = GetIndexOfSection(".strtab");

			struct stat st;
			fstat(this->readFile->_fileno, &st);

			char *p = (char*)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE,
				this->readFile->_fileno, 0);

			Elf32_Shdr* shdr = (Elf32_Shdr*)(p + this->elfHeader32->e_shoff);

			Elf32_Shdr* sh_strtab = &shdr[stringTableIndex];
			const char * const sh_strtab_p = p + sh_strtab->sh_offset;

			printf("  Name:\t\t\t%s\n", sh_strtab_p + symbol.st_name);
		}

		// Symbol binding.
		printf("  Binding:\t\t");
		switch (symbol.st_info)
		{
			case 0x00:
				printf("INVISIBLE");
				break;
			case 0x01:
				printf("GLOBAL");
				break;
			case 0x02:
				printf("WEAK");
				break;
			case 0x010:
				printf("ENVIRON");
				break;
			default:
				printf("DEFAULT");
				break;
		}
		printf("\n");

		// Symbol size.
		printf("  Size:\t\t\t%d bytes (%p)\n", symbol.st_size, reinterpret_cast<void*>(symbol.st_size));

		// Symbol value.
		printf("  Function address:\t0x%x\n\n", symbol.st_value);

	}
}
void ELFFunction::readSymbols64()
{

}


/*   Silent functions of reading headers   */
bool ELFFunction::silentReadELFHeader()
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
bool ELFFunction::silentReadSectionHeaders()
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
}


