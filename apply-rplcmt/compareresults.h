#ifndef COMPARERESULTS_H
#define COMPARERESULTS_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>


class CompareResults
{
public:
  CompareResults();
  std::vector<std::string> in12;
  std::vector<std::string> in1;
  std::vector<std::string> in2;
  void printResults();
  int result;

  friend std::ostream& operator<<(std::ostream &os, const CompareResults& cr);

};

#endif // COMPARERESULTS_H
