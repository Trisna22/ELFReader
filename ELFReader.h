#include "stdafx.h"

#include "ELFHeader.h"

#ifndef ELFReader_H
#define ELFReader_H
class ELFReader : public ELFHeader
{
public:
	ELFReader(string);
	void readAllELF();
	bool IsReady();
private:
	FILE* readFile = NULL;
};


#endif // !~ELFReader_H


ELFReader::ELFReader(string FileName) : ELFHeader::ELFHeader(FileName)
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
