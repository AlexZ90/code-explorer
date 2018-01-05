#ifndef INSTRUMENT_DEFINES_777
#define INSTRUMENT_DEFINES_777
#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#ifdef SYS_gettid
#define PRINTTID syscall(SYS_gettid)
#else
#warning "SYS_gettid unavailable on this system"
#define PRINTTID 0
#endif
#define DEFBEG_777 printf ("\nstart %s %ld %s %d\n",__func__,PRINTTID,__FILE__,__LINE__); 
#define DEFEND_777 printf ("\nend %s %ld %s %d\n",__func__,PRINTTID,__FILE__,__LINE__); 
#define DEFRET_777 if (0 & printf ("\nret %s %ld %s %d\n",__func__,PRINTTID,__FILE__,__LINE__)); else 
#endif
#include <stdio.h>
extern "C" {int bar ();}
extern "C" {void foo();}

class A{

unsigned var : 1;

public:
  A(){ DEFBEG_777  DEFEND_777 }
  int test_method (int a) { DEFBEG_777  if (a > 0)  DEFRET_777 return 1; else  DEFRET_777 return 0; DEFEND_777 }
  int test_method (int a, int b) { DEFBEG_777  DEFRET_777 return 1; DEFEND_777 }
  int hello();
  bool getVar() const { DEFBEG_777 printf ("printf inside header\n");  DEFRET_777 return !var; DEFEND_777 }
};
