/*
 * Copyright © 2013 Marek
 */
#include "test.h"
#include <stdio.h>

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <new> //c++ include

//test for conflicts in macros


#define my_printf printf

#define hello_world A->func_call()

void macrofunc1();
void macrofunc1() {my_printf("Printf MACRO1\n");}
//redefine macrofunc1(). macrofunc1 redefine works because firstly preprocessor expands all macro(I think). If not redefined also works because function macrofunc1() is defined
#define macrofunc1()  
#define MACRO1 macrofunc1();

//

//if that MACRO2 is not used in code then there are no errors. In other case there is error "macrofunc2() undeclared"`
#define macrofunc2 macrofunc1
#define MACRO2 macrofunc2();

//return redefinition
#define return return       

#define SPACES    

#define BODY \
SPACES { \
   printf ("##name_str\r\n"); \
     return true; \
}

#define EXT1(name_str) \
static inline bool \
func##name_str() \
BODY

EXT1(1)
EXT1(2)

#define BODY2 \
{ \
   printf ("##name_str\r\n"); \
     return true; \
}

#define EXT2(name_str) \
static inline bool \
func##name_str() \
BODY2

#undef TEST_UNDEF

#define MACRO4 MACRO5
#ifdef MACRO5
 #define MACRO6
#elif defined (MACRO4)
 #define MACRO7
#endif

EXT2(3)

#undef EXT1
#undef EXT2

#pragma 

# line 50

int main ()
{

  A* a = new A();
  a->hello();
  func1();
  func2();
  func3();

  //printf ("%s\n",#include "string.txt"); //invalid

  //char string[] = #include"string.h" ;  //invalid

  MACRO1
  MACRO2

  printf ("a->getVar() = %d\n\r",a->getVar()?1:0);
  int b = 1;
  printf ("b = %d\n\r", b     );

  #if (TEXT == 'HELLO')
  #endif

  #ifdef HELLO
    printf ("HELLO IS DEFINED\n");
  #endif

  #ifndef HELLO
    printf ("HELLO IS NOT DEFINED\n");
  #endif    

  #if (defined (HELLO) && HELLO > 0)
    printf ("HELLO > 0\n");
  #elif (defined /*PRIN
  TF*/ (HELLO) && HELLO > 3)
    printf ("HELLO > 3\n");
  #endif

  int a1 = 1;
  int b1 = 2;
  int c1 = a1 + b1;
  if (a1==3 || b1==4)
    return printf ("a1==3\n"),printf ("a1==3\n") || printf ("a1==3\n"),printf ("b1==4\n");

 // !!!may be!!! Clang use another headers (macros) than gcc, so preprocess file diffrently, 
 // so clang can make wrong decisions (for us) about where to insert instrumantation code
  #if defined (__SSE__)
     //ret main show wrong line number. Perha[s because of defines.
    return printf ("SSE_DEFINED\n"), (1);
  #else
    return printf ("SSE_NOT_DEFINED\n"), (1);
  #endif

}


/*This comment should make clang return wrong positions(minus is 3 bytes!)*/
/*N – 1*/

typedef int myType;

void
my_func (myType func)
{}
