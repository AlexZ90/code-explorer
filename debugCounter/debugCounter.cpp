#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cstdio>

#include "commonFunctions.h"

typedef struct Counter {
	unsigned long int start;
	unsigned long int end;
} Counter;


int countFunctionCalls (const std::string &inFilePath);

int countFunctionCalls (const std::string &inFilePath)
{
    std::string line = "";
    std::fstream inFile;
    unsigned long int level = 1;
    std::vector<std::string> splitResult;
    std::unordered_map<std::string,Counter> functionCalls;

    inFile.open(inFilePath, std::ios::in);

    if (inFile.is_open())
    {
		while (std::getline(inFile,line))
        {

        	iag::split(line,std::string(" "),splitResult);

        	if (line.find("start ") == 0)
            {
            	//std::cout << line << " " << level << std::endl;

            	if (functionCalls.find(splitResult[1]) == functionCalls.end())
            	{
            		Counter cnt;
            		cnt.start = 0;
            		cnt.end = 0;
            		functionCalls[splitResult[1]] = cnt;
            	}

            	functionCalls[splitResult[1]].start++;
            }
            else if ((line.find("end ") == 0) || (line.find("ret ") == 0))
            {
            	if (functionCalls.find(splitResult[1]) == functionCalls.end())
            	{
            		Counter cnt;
		            cnt.start = 0;
            		cnt.end = 0;
            		functionCalls[splitResult[1]] = cnt;
            	}

            	functionCalls[splitResult[1]].end++;
            }
        }
        inFile.close();
    }
    else
    {
    	std::cout << "Error opening input file: " << inFilePath << "\n\r";
    	inFile.close();
        return 0;
    }


    int error = 0;

	for (auto it : functionCalls) 
	{
		if (it.second.start != it.second.end)
		{
			std::cout << " " << it.first << ": " << "start " << it.second.start << " end " <<  it.second.end << std::endl;	
            error = 1;
		}
	}

    if (error == 1)
    {
        std::cout << std::endl << "Errors in hierarchy occured." << std::endl;
    }
    else
    {
        std::cout << "There are no errors in hierarchy." << std::endl;
    }
    

	inFile.close();
	// outFile.open(outFilePath, std::ios::out | std::ios::in | std::ios::trunc);
	// outFile << line << " " << level << std::endl;
	// outFile.close();

    return 1;

}





int main(int argc, const char **argv) {


	if (argc < 2)
	{
		std::cout << "Usage: countFunctionCalls inFilePath" << std::endl;
		return 0;
	}
	else
	{
		std::string inFilePath = std::string(argv[1]);
		countFunctionCalls (inFilePath);
	}





return 1;

 
}
    
