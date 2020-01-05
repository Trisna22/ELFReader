#include "stdafx.h"

#include "ELFHeader.h"
#include "ELFFunction.h"

#ifndef ELFReader_H
#define ELFReader_H
class ELFReader : public ELFHeader, public ELFFunction
{
public:
	ELFReader(string);

	/*   ELF Header.   */
	void readAllELF();

	/*   Section Header.   */
	void readSectionHeader();
	void readSectionHeader(string);
	void readSectionHeader(int);

	/*   Symbols.   */
	void readAllSymbols();
	void readSymbol(string);

	bool IsReady();
private:
	FILE* readFile = NULL;
};
#endif // !~ELFReader_H


ELFReader::ELFReader(string FileName) : ELFHeader::ELFHeader(FileName),
	 ELFFunction::ELFFunction(FileName)
{
	readFile = fopen(FileName.c_str(), "rb");
}

void ELFReader::readAllELF()
{
	if (ELFHeader::IsReady() == false)
	{
		cout << "ELFReader class Not ready yet!\n" << endl;
		return;
	}

	// Print out all headers.
	ELFHeader::readELFHeader();
	ELFHeader::readProgramHeader();
	ELFHeader::readSectionHeader();
}

/*   Reads only the section headers.   */
void ELFReader::readSectionHeader()
{
	if (ELFHeader::silentReadELFHeader() == false)
	{
		printf("ELFReader: Failed to read ELF header!\n\n");
		return;
	}

	ELFHeader::readSectionHeader();
}

/*   Reads one section header from list. (name specific)   */
void ELFReader::readSectionHeader(string sectionName)
{
	if (ELFHeader::silentReadELFHeader() == false)
	{
		printf("ELFReader: Failed to read ELF header!\n\n");
		return;
	}

	ELFHeader::readSectionHeader(sectionName);
}

/*   Reads one section header from list. (index specific)   */
void ELFReader::readSectionHeader(int index)
{
	if (ELFHeader::silentReadELFHeader() == false)
	{
		printf("ELFReader: Failed to read ELF header!\n\n");
		return;
	}

	ELFHeader::readSectionHeader(index);
}

/*   Reads all symbols in the file.   */
void ELFReader::readAllSymbols()
{
	if (ELFFunction::silentReadELFHeader() == false)
	{
		printf("ELFReader: Failed to read ELF header in silent mode!\n\n");
		return;
	}

	ELFFunction::readSymbols();
}

/*   Reads one symbol from file.   */
void ELFReader::readSymbol(string symbolName)
{
	if (ELFFunction::silentReadELFHeader() == false)
	{
		printf("ELFReader: Failed to read ELF header in silent mode!\n\n");
		return;
	}
	if (ELFFunction::silentReadSectionHeaders() == false)
	{
		printf("ELFReader: Failed to read section header in silent mode!\n\n");
		return;
	}

	ELFFunction::readSymbol(symbolName);
}
