#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cstdio>

#include "Rplsmt.h"
#include "commonFunctions.h"
void writeDefines(std::fstream& fs);

#define RPLSMTS_CONTAINER_TYPE std::unordered_map<std::string, std::vector<Rplsmt>>


std::string replacementsPath = "./replacements.txt";
std::string resultPath = "./resultReplacements.txt";

int main(int argc, const char **argv) {

    std::string line;
    std::fstream myfile;
    std::string filePath = "";
    int result = 0;
    std::vector<std::string> splitResult;
    std::size_t pos;

    unsigned long int curFilePos = 0;
    unsigned long int replFilePos = 0;

    std::fstream tmpFile;
    std::string tmpFilePath = "./tmpFile.txt";

    std::fstream curProcFile;
    std::string curProcFilePath = "";

    bool curProcFileChanged = false;
    std::string replText = "";
    char buf = ' ';


    RPLSMTS_CONTAINER_TYPE replacements;

    myfile.open(replacementsPath, std::ios::in | std::ios::binary);
    if (myfile.is_open())
    {

        while (std::getline(myfile,line))
        {
            if (( pos = line.find(":")) != std::string::npos)
            {
                filePath = line.substr(pos+1);
            }
            else if (line.size() > 1)
            {
                iag::split(line,std::string(" "),splitResult);
                //Rplsmt r(std::stoul(splitResult[0],nullptr,10),splitResult[1]);
                Rplsmt r(std::stoul(splitResult[0],nullptr,10),splitResult[1]);

                if (replacements.find(filePath) == replacements.end())
                {
                    replacements[filePath].push_back(r);
                }
                else
                {
                    iag::addToVector(replacements[filePath],r);
                }
            }
        }
        myfile.close();
    }
    else
    {
        std::cout << "Error opening replacements file: " << replacementsPath << std::endl;
        return 0;
    }

    myfile.open(resultPath, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    if (myfile.is_open())
    {
        for (RPLSMTS_CONTAINER_TYPE::iterator it = replacements.begin(); it != replacements.end(); ++it)
        {
            myfile << "file:" << (*it).first << std::endl;
            std::vector<Rplsmt> rplsmtsList = (*it).second;
            std::sort(rplsmtsList.begin(),rplsmtsList.end(),Rplsmt::rplsmtSmallerOrEqual);
            for (auto i = 0; i < rplsmtsList.size(); i++)
            {
                myfile << rplsmtsList.at(i) << "\n";
            }
        }
        myfile.close();
    }
    else
    {
        std::cout << "Error opening result replacements file: " << resultPath << std::endl;
        return 0;
    }

    printf ("%s %ld\r\n", __FILE__,__LINE__);

    /* Вставка текста в файлы в соответствии со сгенерированными правилами. */

    //Открыть файл с заменами
    myfile.open(resultPath, std::ios::in);
    if (!myfile.is_open())
    {
        std::cout << "Error opening result replacements file: " << resultPath << std::endl;
        return 0;
    }

    while (1)
    {
        if (!std::getline(myfile,line))
        {
            myfile.close();
            std::cout << "File with replacements finished" << std::endl;
            return -1;
        }
        else if (line.find(":") != std::string::npos)
        {
            break;
        }
    }

    iag::split(line,std::string(":"),splitResult);
    curProcFilePath = splitResult[1]; 
    curProcFile.open(curProcFilePath, std::ios::in | std::ios::binary);
    if (!curProcFile.is_open())
    {
        std::cout << "Error opening curProcFile file: " << curProcFilePath << std::endl;
        return 0;
    }

    tmpFile.open(tmpFilePath, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!tmpFile.is_open())
    {
        std::cout << "Error opening tmpFile file: " << tmpFilePath << std::endl;
        return 0;
    }

    writeDefines(tmpFile);

    curFilePos = 0;

    while (1)
    {
        line = "";
        if ((!std::getline(myfile,line)) || (line.find(":") != std::string::npos))
        {
            std::cout << line << std::endl;

            while (1)
            {

                curProcFile.read(&buf,1);
                if (!curProcFile.eof())
                {
                    tmpFile.write(&buf,1);
                }
                else
                {
                    break;
                }
            }

            curProcFile.close();
            tmpFile.close();
            if (curProcFileChanged)
            {

                iag::split(curProcFilePath,"/",splitResult);
                std::cout << splitResult[splitResult.size()-1] << std::endl;
                rename(tmpFilePath.c_str(), curProcFilePath.c_str());

                curFilePos = 0;
                curProcFileChanged = false;
            }
            
            // if( remove(tmpFilePath.c_str()) != 0 )
            //     std::cout << "Error deleting file\n\r";

            if ((myfile.rdstate() & std::ifstream::eofbit) != 0 )
            {
                std::cout << "End of replacement" << std::endl;
                return 0;
            }
            else
            {
                iag::split(line,std::string(":"),splitResult);
                curProcFilePath = splitResult[1]; 
                curProcFile.open(curProcFilePath, std::ios::in | std::ios::binary);
                if (!curProcFile.is_open())
                {
                    std::cout << "Error opening curProcFile file: " << curProcFilePath << std::endl;
                    return 0;
                }

                tmpFile.open(tmpFilePath, std::ios::out | std::ios::trunc | std::ios::binary);
                if (!tmpFile.is_open())
                {
                    std::cout << "Error opening tmpFile file: " << tmpFilePath << std::endl;
                    return 0;
                }  

                writeDefines(tmpFile);
            }
        }
        else if (line.size() > 1)
        {
            iag::split(line,std::string(" "),splitResult);
            std::cout << splitResult[0] << std::endl;
            replFilePos = std::stoul(splitResult[0],nullptr,10);
            replText = splitResult[1];

            while (curFilePos != replFilePos)
            {
                curProcFile.read(&buf,1);
                if (!curProcFile.eof())
                {
                    curProcFileChanged = true;
                    tmpFile.write(&buf,1);
                    curFilePos++;
                }
                else
                {
                    std::cout << "curProcFile finished unexpectedly" << std::endl;
                    return 0;
                }
            }

//            if (replText == "B")
//            {
//                tmpFile << " DEFBEG_777 ";  
//            }
//            else if (replText == "E")
//            {
//                tmpFile << " DEFEND_777 ";  
//            }
//            else if (replText == "R")
//            {
//                tmpFile << " DEFRET_777 ";  
//            }
            
            tmpFile << getRplsmtText(replText);

            

        }
    }

    //



    myfile.close();
    return 1;
}
    
void writeDefines(std::fstream& fs)
{
	writeRplsmtDefines(fs);
}