	#include <iostream>
#include <fstream>
#include <vector>

int main (int argc, char** argv)
{
	std::fstream file1;
	std::fstream file2;
	char buf = ' ';
	file1.open(argv[1], std::ios::in | std::ios::binary);
	file2.open("./copy.txt", std::ios::out | std::ios::binary);

	while (1)
	{

		file1.read(&buf,1);
		if (!file1.eof())
		{
			std::cout << "*";
			file2.write(&buf,1);
		}
		else
			break;
	}
	std::cout << std::endl;


	file1.close();
	file2.close();

	return 1;






}