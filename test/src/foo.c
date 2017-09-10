#include <stdio.h>
#define DEFBEG_777 printf ("start %s \n",__func__);
#define DEFEND_777 printf ("end %s \n",__func__);
#define DRE7 } 
#define DRS7 { DEFEND_777 
#include "foo.h"
#define EXPR_1 ; 

void foo ()
{ 

	int a = 1;
	int b = 2;
	if (a > 1)
	{
		b++;
	}

	if (b > 2)
	{
		a++;
	}
	else
	{
		a--;
	}

	bar();
	 return EXPR_1}
