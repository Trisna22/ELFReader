#include "stdafx.h"

#include "ELFHeader.h"
#include "ELFFunction.h"

#ifndef ELFReader_H
#define ELFReader_H
class ELFReader : public ELFHeader, public ELFFunction
{
public:
	ELFReader(string);
	void readAllELF();
	void readSectionHeader();
	void readSectionHeader(string);
	void readSectionHeader(int);
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
	// Super class.
	ELFHeader::readELFHeader();
	ELFHeader::readProgramHeader();
	ELFHeader::readSectionHeader();

	printf("ELF rest data...\n");
}

/*   Reads only the section headers.   */
void ELFReader::readSectionHeader()
{
	ELFHeader::readSectionHeader();
}

/*   Reads one section header from list. (name specific)   */
void ELFReader::readSectionHeader(string sectionName)
{
	ELFHeader::readSectionHeader(sectionName);
}

/*   Reads one section header from list. (index specific)   */
void ELFReader::readSectionHeader(int index)
{
	ELFHeader::readSectionHeader(index);
}
