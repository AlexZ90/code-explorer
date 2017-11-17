#include "commonFunctions.h"

namespace iag //input actions generator
{

int close_file (std::fstream& file, std::string file_path)
{
  file.close();

  if (file.is_open() == true)
  {
    std::cout << "Error while closing the file : " << file_path << ";\n";
    show_rdstate (file);
    return 0;
  }

  return 1;
}

void show_rdstate (std::fstream& file)
{
  std::ios::iostate rdstate_val;

  rdstate_val = file.rdstate();


  if ((rdstate_val & std::ifstream::goodbit) != 0)
    std::cout << "goodbit detected: No errors (zero value iostate)" << "\n";

  if ((rdstate_val & std::ifstream::eofbit) != 0)
    std::cout << "eofbit detected: End-of-File reached on input operation" << "\n";

  if ((rdstate_val & std::ifstream::failbit) != 0)
    std::cout << "failbit detected: Logical error on i/o operation" << "\n";

  if ((rdstate_val & std::ifstream::badbit) != 0)
    std::cout << "badbit detected: Read/writing error on i/o operation" << "\n";


}

int split (std::string in_string, std::string delimeters, std::vector<std::string>& split_result)
{

  std::string str = "";
  int i = 0;
  int j = 0;
  int delim_matches_exists = 0;

  split_result.clear();

  if (in_string.empty() || (delimeters.size() == 0))
  {
    //  Error! split(): input string or delimetrs is empty
    return 0;
  }

  for (i = 0; i < in_string.size(); i++)
  {
    delim_matches_exists = 0;

    for (j = 0; j < delimeters.size(); j++)
    {
      if (in_string[i] == delimeters[j])
        delim_matches_exists = 1;
    }



    if (delim_matches_exists == 0)
    {
      str += in_string[i];
    }
    else if (!str.empty())
    {

      split_result.push_back(str);
      str.clear();
    }
  }

  if (!str.empty()) split_result.push_back(str);

  return 1;
}


unsigned int calcMaxStringSize (std::vector<std::string> &strings)
{
  if (strings.size() == 0)
    return 0;

  unsigned int maxSize = strings[0].size();

  for (auto str: strings) {
    if (str.size() > maxSize)
      maxSize = str.size();
  }

  return maxSize;

}

std::string formatString (std::string str, unsigned int max, bool align_left)
{
  std::string formatStr = "";
  if (align_left)
  {
    formatStr+=str;
    for (unsigned int i = 0; i < max-str.size(); i++)
      formatStr+=" ";
  }
  else
  {
    for (unsigned int i = 0; i < max-str.size(); i++)
      formatStr+=" ";
    formatStr+=str;
  }

  return formatStr;
}


int autoMap(std::vector<std::string> names_1, std::vector<std::string> names_2, std::unordered_map<std::string, std::string> &resultMap)
{

  std::string name_2 = "";
  std::vector<std::string> split_result;
  std::string name_2_var_1 = "";
  std::string name_2_var_2 = "";

  rmUnusedMap(names_1, names_2, resultMap);

  for (int i = 0; i < names_1.size(); i++)
  {
    std::string name_1 = names_1[i];

    auto it = resultMap.find(name_1);
    if ( it == resultMap.end() )
    {
      for (int j = 0; j < names_2.size(); j++)
      {
        name_2 = names_2[j];

        if (name_2.find("[") != std::string::npos)
        {
          iag::split(name_2,"[]", split_result);
          name_2_var_1 = split_result[0] + "_" + split_result[1];
          name_2_var_2 = split_result[0] + split_result[1];
        }

        if (name_1 == name_2 || name_1 == name_2_var_1 || name_1 == name_2_var_2)
        {
          resultMap.insert(std::pair<std::string, std::string>(name_1,name_2));
          break;
        }
      }
    }
  }
  return 1;
}

int delete_file(std::string file_path)
{
  std::fstream test_file;
  test_file.open(file_path,std::fstream::out | std::fstream::in);
  if (test_file.is_open()) //file exists
  {
    test_file.close();
    if (std::remove(file_path.c_str()) != 0)
    {
      return 0;
    }
    else
      return 1;
  }
}

int rmUnusedMap(std::vector<std::string> &names_1, std::vector<std::string> &names_2, std::unordered_map<std::string, std::string> &map)
{
  std::string search_name = "";
  bool name_1_exists = false;
  bool name_2_exists = false;
  std::vector<std::string> mapsToDelete;
  mapsToDelete.reserve(map.size());

  for (auto& it : map)
  {

    name_1_exists = false;
    name_2_exists = false;

    search_name = it.first;
    for (size_t i = 0; i < names_1.size(); i++)
    {
      if (search_name == names_1[i])
      {
        name_1_exists = true;
        break;
      }
    }

    search_name = it.second;
    for (size_t i = 0; i < names_2.size(); i++)
    {
      if (search_name == names_2[i])
      {
        name_2_exists = true;
        break;
      }
    }

    if ((name_1_exists == false) || (name_2_exists == false))
    {
      mapsToDelete.push_back(it.first);
    }

  }

  for (size_t i = 0; i < mapsToDelete.size(); i++)
    map.erase(mapsToDelete[i]);

  return 1;
}

int compareVectors(std::vector<std::string> &v1, std::vector<std::string> &v2, CompareResults &compRes)
{
  compRes.in12.clear();
  compRes.in1.clear();
  compRes.in2.clear();
  compRes.result = 0;
  int returnVal = 0;

  if ((v1.size() == 0) && (v2.size() == 0))
  {
    compRes.result = 2;
    return 2;
  }

  if (v1.size() != v2.size())
    returnVal = 0;

  for (std::vector<std::string>::iterator it1 = v1.begin(); it1 != v1.end(); ++it1)
  {
    std::vector<std::string>::iterator it2 = std::find(v2.begin(), v2.end(),*it1);
    if (it2 == v2.end())
    {
      compRes.in1.push_back(*it1);
    }
    else
    {
      compRes.in12.push_back(*it1);
    }
  }

  for (std::vector<std::string>::iterator it2 = v2.begin(); it2 != v2.end(); ++it2)
  {
    std::vector<std::string>::iterator it1 = std::find(v1.begin(), v1.end(),*it2);
    if (it1 == v1.end())
    {
      compRes.in2.push_back(*it2);
    }
  }

  if ((v1.size() != 0) && (v1.size() == compRes.in12.size()) && (v1.size() == v2.size()))
    returnVal = 1;

  compRes.result = returnVal;
  return returnVal;
}

int findVectorInLine(std::string &line, std::vector<std::string> &v)
{
  int count = 0;
  for (int i = 0; i < v.size(); i++)
  {
    if (line.find(v[i]) != std::string::npos)
    {
      count++;
    }
  }
  return count;
}

int findLineInVector(std::string &line, std::vector<std::string> &v)
{
  for (int i = 0; i < v.size(); i++)
  {
    if (v[i].find(line) != std::string::npos)
    {
      return 1;
    }
  }
  return 0;
}


}
