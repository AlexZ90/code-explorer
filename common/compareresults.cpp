#include "compareresults.h"

CompareResults::CompareResults()
{

}

void CompareResults::printResults()
{

  printf ("\r\n**************** in12 *****************\r\n");
  for (std::vector<std::string>::iterator it = in12.begin(); it != in12.end(); ++it)
  {
      printf ("%s\r\n", (*it).c_str());
  }

  printf ("\r\n**************** in1 *****************\r\n");
  for (std::vector<std::string>::iterator it = in1.begin(); it != in1.end(); ++it)
  {
      printf ("%s\r\n", (*it).c_str());
  }

  printf ("\r\n**************** in2 *****************\r\n");
  for (std::vector<std::string>::iterator it = in2.begin(); it != in2.end(); ++it)
  {
      printf ("%s\r\n", (*it).c_str());
  }

}

std::ostream & operator<<(std::ostream &os, const CompareResults &cr)
{
    os << "\r\n**************** in12 *****************\r\n";
    for (std::vector<std::string>::const_iterator it = cr.in12.begin(); it != cr.in12.end(); ++it)
    {
        os << (*it).c_str() << std::endl;
    }

    os << "\r\n**************** in1 *****************\r\n";
    for (std::vector<std::string>::const_iterator  it = cr.in1.begin(); it != cr.in1.end(); ++it)
    {
        os << (*it).c_str() << std::endl;
    }

    os << "\r\n**************** in2 *****************\r\n";
    for (std::vector<std::string>::const_iterator  it = cr.in2.begin(); it != cr.in2.end(); ++it)
    {
        os << (*it).c_str() << std::endl;
    }

    return os;
}
