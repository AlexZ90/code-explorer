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
#include "bar.h"
#include <stdio.h>
#define STMT printf("return");  DEFRET_777 return 1  ;

int bar ()
{ DEFBEG_777     
	STMT
 DEFEND_777 }
