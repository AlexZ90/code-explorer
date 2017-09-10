/*
 * Copyright © 2013 Marek 
 */
#include "test.h"
#include <stdio.h>


int main ()
{

  A* a = new A();
  a->hello();
  printf ("a->getVar() = %d\n\r",a->getVar()?1:0);
  int b = 1;
  printf ("b = %d\n\r", b     );
  return printf ("Hello World"), (1);

}