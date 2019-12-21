#include "ELFReader.h"

#include "HexReader.h"


/*
	Some command line parameters idea's:

	-s ||| all sections
	-s <segment type> ||| Can be by name (.text) or number 0x01
	-a ||| print all data
	-f ||| all functions (assembler code)
	-f <function name> ||| info of function (assembler code)
	-c ||| all comments
	...


*/

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
