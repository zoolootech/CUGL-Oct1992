#include "timer1.h"

# define NROW 10
# define NCOLUMN 10

int a[NROW][NCOLUMN], i = 0, j = 0;

DO_STMT("expr = 5 * expr")	a[i+5][j+5] = 5 * a[i+5][j+5]	OD

DO_STMT("expr *= 5")		a[i+5][j+5] *= 5				OD

}
