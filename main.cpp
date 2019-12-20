#include "ELFReader.h"

#include "HexReader.h"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Usage: ELFReader %%filename\n");
		return -1;
	}

	ELFReader reader(argv[1]);
	reader.readAllELF();

//	HexReader hex("./test32");
//	cout << hex.readAllBytes() << endl;
	return 0;
}
