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

/*   Prints out the help table.   */
void HelpTable()
{
	printf("Usage: ELFReader [-S] [-F] [-E] [-P] %%filename\n\n");

	printf("Options are:\n");
	printf("-a, --all\t\t\t\tPrints out all headers\n");
	printf("-S, --section-headers\t\t\tPrints out all section headers\n");
	printf("-s, --section %%index || %%name\t\tPrints out specific section header\n");
	printf("-F, --functions\t\t\t\tPrints out all symbols\n");
	printf("-f, --function %%name\t\t\tPrints out specific symbol\n");

	printf("\nCopyrights to ramb0 (c) 2019-2020\n\n");
}

/*   Checks if a string is an number.   */
bool isNumber(string str)
{
	char arr[] = "0123456789";

	for (int i = 0; i < str.length(); i++)
	{
		for (int j = 0; j < sizeof(arr); j++)
		{
			if (str[i] == arr[j])
				return true;
		}
	}
	return false;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Usage: ELFReader [-A] [-S] [-F] [-E] [-P] %%filename\n");
		return -1;
	}

	for (int i = 1; i < argc; i++)
	{
		string arg = argv[i];
		if (arg == "-h" || arg == "--help")
		{
			HelpTable();
			return 0;
		}
		else if ((arg == "-a" || arg == "--all") && argc >= 2)
		{
			if (argc != 3)
			{
				printf("Usage: ELFReader -a %%filename\n\n");
				return -1;
			}

			ELFReader reader(argv[i +1]);
			reader.readAllELF();
			return 0;
		}
		else if (arg == "-S" || arg == "--section-headers")
		{
			if (argc != 3)
			{
				printf("Usage ELFReader -S %%filename\n\n");
				return -1;
			}

			ELFReader reader(argv[i +1]);
			reader.readSectionHeader();
			return 0;
		}
		else if ((arg == "-s" || arg == "--section") && argc >= 3)
		{
			if (argc != 4)
			{
				printf("Usage: ELFReader -s %%index %%filename\n\n");
				return -1;
			}

			string file = argv[i + 2];
			string index = argv[i +1];

			ELFReader reader(file);
			if (isNumber(index) == false)
				reader.readSectionHeader(index);
			else
				reader.readSectionHeader(atoi(index.c_str()));

			return 0;
		}
		else if (arg == "-F" || arg == "--functions" || arg == "--symbols")
		{
			if (argc != 3)
			{
				printf("Usage: ELFReader -F %%filename\n\n");
				return -1;
			}

			ELFReader reader(argv[i +1]);
			reader.readAllSymbols();
			return 0;
		}
		else
		{
			printf("Unknown argument/option combination: %s\n\n", arg.c_str());
			return -1;
		}

	}
}
