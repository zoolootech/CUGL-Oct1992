#include "timer1.h"

# define NROW 10
# define NCOLUMN 10

int a[NROW][NCOLUMN];
register int i, j;
	
DO_STMT("Two-Dimensional looping")
	for (i = 0; i < NROW; i++)
		for (j = 0; j < NCOLUMN; j++)
			a[i][j] = 0;

OD

}
