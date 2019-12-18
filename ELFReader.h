#include "stdafx.h"

#include "ELFHeader.h"

#ifndef ELFReader_H
#define ELFReader_H
class ELFReader : public ELFHeader
{
public:
	ELFReader();
	void readELF();

protected:

};


#endif // !~ELFReader_H


ELFReader::ELFReader()
{

}

void ELFReader::readELF()
{
	// Super class.
	ELFHeader::readHeader();

	printf("ELF rest data...\n");
}
