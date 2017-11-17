#include "bar.h"
#include <stdio.h>
#define STMT printf(""); return 1  ;

int bar ()
{    
	STMT
}
