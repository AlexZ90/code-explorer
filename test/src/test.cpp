#include <stdio.h>
#define DEFBEG_777 printf ("start %s \n",__func__);
#define DEFEND_777 printf ("end %s \n",__func__);
#define DRE7 } 
#define DRS7 { DEFEND_777 
#include "test.h"

int 
A::hello()
{
	foo();     
	return 1;
}