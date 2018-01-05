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

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}


int compLogs (const std::string &filePath1, const std::string &filePath2, const std::string& fileName, const std::string &filePath)
{
	std::string line1;
	std::string line2;
    std::fstream file1;
    std::fstream file2;
    std::fstream file3;
    
    std::vector<std::string> splitResult1;
    std::vector<std::string> splitResult2;

	std::string compareString1 = "";
	std::string compareString2 = "";
	unsigned long int lineNum = 1;
	char fileEquals = 1;
	std::vector<int> notEqualsColumns;
	notEqualsColumns.clear();
	char choose;
	bool addLineEnabled = false;
	bool addIncludeEnabled = false;

    unsigned long int numOfReadLines = 0;
    unsigned long int linesToAdd = 0;
    unsigned long int srcFilePos = 0;
    
    unsigned long int curLine = 0;
    unsigned long int prevLine = 0;
    unsigned long int srcStartLine = 0;
    unsigned long int srcCurLine = 0;
    unsigned long int srcPrevLine = 0;


    file1.open(filePath1, std::ios::in);

    if (file1.is_open())
    {

        while (std::getline(file1,line1))
        {
			iag::split(line1,std::string(" "),splitResult1);


			if (splitResult1.size() >= 3)
			{
				if  ((splitResult1[0] == "#") && (is_number(splitResult1[1])))
				{
					iag::split(splitResult1[2],std::string("\""),splitResult2);
					if (splitResult2.size() == 1)
					{
						std::string filename = splitResult2[0];
				 	if (filename == fileName)
				 	{
						curLine = std::stoi(splitResult1[1]);

						if (curLine != prevLine && srcCurLine != 0)
						{
							srcStartLine = srcPrevLine;
						}
						else
						{	prevLine = curLine;
							srcPrevLine = srcCurLine;
						}

				 		addLineEnabled = true;
				 	}
					}
					else
					{
						std::cout << "Error. Unknown construction in line: " << line1 << std::endl;
						return 1;
					}
				}
			}
			srcCurLine++;
		}
		
        file1.close();
    }
    else
    {
		file1.close();
        std::cout << "Error opening files: " << filePath1 << "\n\r";
        return 0;
    }


    file1.open(filePath1, std::ios::in);
    file2.open(filePath2, std::ios::in | std::ios::out | std::ios::trunc);
    file3.open(filePath, std::ios::in);

    if (file1.is_open() && file2.is_open() && file3.is_open())
    {

    	unsigned long int lineCount = 0;

        while (std::getline(file1,line1))
        {
			

        	if (lineCount < srcStartLine)
        	{
        		lineCount++;
    			continue;
        	}

			iag::split(line1,std::string(" "),splitResult1);


			if (splitResult1.size() >= 3)
			{
			 if  ((splitResult1[0] == "#") && (is_number(splitResult1[1])))
			 {

			 	iag::split(splitResult1[2],std::string("\""),splitResult2);
			 	if (splitResult2.size() == 1)
			 	{
			 		std::string filename = splitResult2[0];
				 	if (filename == fileName)
				 	{
						srcFilePos = std::stoi(splitResult1[1]);
				 		addLineEnabled = true;
				 	}
				 	else
				 	{
				 		if (addLineEnabled)
				 		{
				 			while (numOfReadLines != (srcFilePos + linesToAdd))
				 			{
				 				std::getline(file3,line2);
				 				numOfReadLines++;
				 				std::cout << line2 << std::endl;
				 				std::cout << numOfReadLines << " " << srcFilePos + linesToAdd << std::endl;
				 				getchar();
				 			}
				 			linesToAdd = 0;
				 			//file2 << "#include \"" << filename << "\"" << std::endl;	
				 			
				 			getchar();
				 			file2 << line2 << std::endl;
				 		}
				 		addLineEnabled = false;
				 	}
			 	}
			 	else
			 	{
			 		std::cout << "Error. Unknown construction in line: " << line1 << std::endl;
			 		return 1;
			 	}
			 }
			}

			if (addLineEnabled)
			{
				file2 << line1 << std::endl;
				linesToAdd++;
			}
		}
		
        file1.close();
        file2.close();
        file3.close();
    }
    else
    {
		file1.close();
		file2.close();
		file3.close();
        std::cout << "Error opening files: " << filePath1 << " or " << filePath2 << " or " << filePath << "\n\r";
        return 0;
    }
    
    return 1;
	
}

int main(int argc, const char **argv) {

	if (argc < 4)
	{
		std::cout << "Usage: procpreproc inFilePath outFilePath fileName filePath" << std::endl;
		return 0;
	}
	else
	{
		std::string inFilePath = std::string(argv[1]);
		std::string outFilePath = std::string(argv[2]);
		std::string fileName = std::string(argv[3]);
		std::string filePath = std::string(argv[4]);
		compLogs (inFilePath,outFilePath,fileName,filePath);
	}

return 1;
 
}
