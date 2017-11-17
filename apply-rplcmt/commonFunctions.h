#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "compareresults.h"

namespace iag //input actions generator
{

void show_rdstate (std::fstream& file);
int close_file (std::fstream& file, std::string file_path);
int delete_file(std::string file_path);
int split (std::string in_string, std::string delimeters, std::vector<std::string>& split_result);

template<typename T>
int addToVector(std::vector<T> &vector, T value);

template<typename T>
int removeFromVector(std::vector<T> &vector, T value);

template<typename T>
int addToVector(std::vector<T> &vector, T value)
{
  bool existed = false;
  for (typename std::vector<T>::size_type i = 0; i < vector.size(); i++)
  {
    if (vector.at(i) == value)
    {
      existed = true;
      break;
    }
  }

  if (existed)
    return 0;
  else
  {
    vector.push_back(value);
    return 1;
  }
}

template<typename T>
int removeFromVector(std::vector<T> &vector, T value)
{

  bool existed = false;

  while (1)
  {
    existed = false;
    for (typename std::vector<T>::size_type i = 0; i < vector.size(); i++)
    {
      if (vector.at(i) == value)
      {
        vector.erase(vector.begin() + i);
        existed = true;
        break;
      }
    }
    if (existed == false)
      return 1;
  }
}

unsigned int calcMaxStringSize (std::vector<std::string> &strings);

std::string formatString (std::string str, unsigned int max, bool align_left = true);

int autoMap(std::vector<std::string> names_1, std::vector<std::string> names_2, std::unordered_map<std::string, std::string> &resultMap);

int rmUnusedMap(std::vector<std::string> &names_1, std::vector<std::string> &names_2, std::unordered_map<std::string, std::string> &map);

int compareVectors (std::vector<std::string> &v1, std::vector<std::string> &v2, CompareResults &compRes);

int findVectorInLine (std::string& line, std::vector<std::string> &v);
int findLineInVector (std::string& line, std::vector<std::string> &v);

}
#endif
