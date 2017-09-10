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
