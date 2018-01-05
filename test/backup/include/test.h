#include <stdio.h>
extern "C" {int bar ();}
extern "C" {void foo();}

class A{

unsigned var : 1;

public:
  A(){}
  int test_method (int a) { if (a > 0) return 1; else return 0;}
  int test_method (int a, int b) {return 1;}
  int hello();
  bool getVar() const {printf ("printf inside header\n"); return !var;}
};
