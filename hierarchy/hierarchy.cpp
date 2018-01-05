#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cstdio>

int createHierarchy (const std::string &inFilePath, const std::string &outFilePath);

int createHierarchy (const std::string &inFilePath, const std::string &outFilePath)
{
    std::string line = "";
    std::fstream inFile;
    std::fstream outFile;
    unsigned long int level = 1;
    unsigned char level_marker = '`';


    inFile.open(inFilePath, std::ios::in);
    outFile.open(outFilePath, std::ios::out | std::ios::in | std::ios::trunc);

    if (inFile.is_open())
    {
    	if (outFile.is_open())
    	{

			while (std::getline(inFile,line))
	        {

	        	if (line.find("start ") == 0)
	            {
	            	//std::cout << line << " " << level << std::endl;
	            	outFile << line << " " << level_marker << level << std::endl;
	            	level++;
	            }
	            else if ((line.find("end ") == 0) || (line.find("ret ") == 0))
	            {
					level--;
	            	outFile << line << " " << level_marker << level << std::endl;
	            }
	        }
	        inFile.close();
	        outFile.close();
    	}
    	else
    	{
    		std::cout << "Error opening output file: " << outFilePath << "\n\r";
    		inFile.close();
    		outFile.close();
        	return 0;
    	}
    }
    else
    {
    	std::cout << "Error opening input file: " << inFilePath << "\n\r";
    	inFile.close();
    	outFile.close();
        return 0;
    }

	inFile.close();
	outFile.close();

	// outFile.open(outFilePath, std::ios::out | std::ios::in | std::ios::trunc);
	// outFile << line << " " << level << std::endl;
	// outFile.close();

    return 1;

}





int main(int argc, const char **argv) {


	if (argc < 3)
	{
		std::cout << "Usage: createHierarchy inFilePath outFilePath" << std::endl;
		return 0;
	}
	else
	{
		std::string inFilePath = std::string(argv[1]);
		std::string outFilePath = std::string(argv[2]);
		createHierarchy (inFilePath, outFilePath);
	}





return 1;

 
}
    
