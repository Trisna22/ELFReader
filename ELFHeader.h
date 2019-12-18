#include "stdafx.h"

#ifndef ELFHeader_H
#define ELFHeader_H
class ELFHeader
{
public:
	ELFHeader();
	void readHeader();
};

#endif // !~ ELFHeader_H


ELFHeader::ELFHeader()
{

}

void ELFHeader::readHeader()
{
	printf("HEADER...\n");
}
