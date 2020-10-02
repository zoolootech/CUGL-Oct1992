#include "timer1.h"

	int i, j, k;

	DO_STMT("i = k; j = k")	i = k; j = k	OD

	DO_STMT("i = j = k")	 i = j = k	OD
}
