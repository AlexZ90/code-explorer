#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cstdio>
#include "commonFunctions.h"


std::string file1Path = "./log.txt";
std::string file2Path = "./log_processed.txt";

int compLogs (const std::string &filePath1, const std::string &filePath2, std::vector<int> colNumsToCompare)
{
	std::string line1;
	std::string line2;
    std::fstream file1;
    std::fstream file2;
    
    std::vector<std::string> splitResult1;
    std::vector<std::string> splitResult2;

	std::string compareString1 = "";
	std::string compareString2 = "";
	unsigned long int lineNum = 1;
	char fileEquals = 1;
	std::vector<int> notEqualsColumns;
	notEqualsColumns.clear();
	char choose;

    file1.open(filePath1, std::ios::in);
    file2.open(filePath2, std::ios::in);
    
    
    if (file1.is_open() && file2.is_open())
    {

        while (std::getline(file1,line1) && std::getline(file2,line2))
        {
			
			iag::split(line1,std::string(" "),splitResult1);
			iag::split(line2,std::string(" "),splitResult2);
			

			bool notEqualsExists = false;
			notEqualsColumns.clear();

			for (int i = 0; i < colNumsToCompare.size(); i++)
			{
				int colNum = colNumsToCompare[i];
				if (splitResult1[colNum] != splitResult2[colNum])
				{
					notEqualsColumns.push_back(colNum);
					notEqualsExists = true;
				}
			}

			if (notEqualsExists)
			{
				std::cout << "Line 1 = " << line1 << std::endl;
				std::cout << "Line 2 = " << line2 << std::endl;
				for (int i = 0; i < notEqualsColumns.size(); i++)
				{
					int colNum = notEqualsColumns[i];
					std::cout << "Error: lines not equal at line " << lineNum << " in " << colNum << " element: \"" << splitResult1[colNum] << "\"" << " and " << "\"" << splitResult2[colNum] << "\"" << std::endl;
					fileEquals = 0;
				}

				std::cout << std::endl;
				std::cout << "Enter 'c' to coninue, 'q' to quit:";
				std::cin >> choose;
				if (choose == 'q')
					break;
				std::cout << std::endl;
			}

			lineNum++;

			// if ((splitResult1[0] == splitResult2[0]))
			// {
			// 	if (splitResult1[1] == splitResult2[1])
			// 	{
			// 		lineNum ++;
			// 		continue;
			// 	}
			// 	else
			// 	{
			// 		std::cout << "Error: lines not equal at line " << lineNum << " in second element." << std::endl;
			// 		std::cout << "Line 1 = " << splitResult1[0] << " " << splitResult1 [1] << std::endl;
			// 		std::cout << "Line 2 = " << splitResult2[0] << " " << splitResult2 [1] << std::endl;
			// 		fileEquals = 0;
			// 		break;
			// 	}
			// }
			// else
			// {
			// 	std::cout << "Error: lines not equal at line " << lineNum << " in first element." << std::endl;
			// 	std::cout << "Line 1 = " << splitResult1[0] << " " << splitResult1 [1] << std::endl;
			// 	std::cout << "Line 2 = " << splitResult2[0] << " " << splitResult2 [1] << std::endl;
			// 	fileEquals = 0;
			// 	break;
			// }
		}
		
		if (fileEquals == 1)
		{
			std::cout << "Finished. Files are equals." << std::endl;
		}
		else
		{
			std::cout << "Finished. Files are not equals." << std::endl;
		}
		
        file1.close();
        file2.close();
    }
    else
    {
		file1.close();
		file2.close();
        std::cout << "Error opening files: " << file1Path << " or " << file2Path << "\n\r";
        return 0;
    }
    
    return 1;
	
}

int main(int argc, const char **argv) {

	std::vector<int> colNumsToCompare;
	colNumsToCompare.clear();
	colNumsToCompare.push_back(0);
	colNumsToCompare.push_back(1);
	colNumsToCompare.push_back(3);
	colNumsToCompare.push_back(4);
	colNumsToCompare.push_back(5);

	if (argc < 3)
	{
		std::cout << "Usage: compLogs logFilePath1 logfilePath2" << std::endl;
		return 0;
	}
	else
	{
		std::string file1Path = std::string(argv[1]);
		std::string file2Path = std::string(argv[2]);
		compLogs (file1Path,file2Path,colNumsToCompare);
	}

return 1;
 
}
