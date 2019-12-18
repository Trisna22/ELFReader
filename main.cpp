#include "ELFReader.h"

#include "HexReader.h"

int main(int argc, char* argv[])
{
	ELFReader reader;
	reader.readELF();


	FILE* ELFFILE = fopen("./test32", "rb");

	HexReader hex(ELFFILE);
	cout << hex.readAllBytes() << endl;
	return 0;
}
