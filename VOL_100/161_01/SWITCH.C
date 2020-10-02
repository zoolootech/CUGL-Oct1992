#include "timer1.h"
int n;

DO_STMT("switch1 (1)")
	{
	n = 1;
#include "switch1.i"
	}
OD

DO_STMT("switch1 (10)")
	{
	n = 10;
#include "switch1.i"
	}
OD

DO_STMT("switch1 (100)")
	{
	n = 100;
#include "switch1.i"
	}
OD

DO_STMT("switch2 (1)")
	{
	n = 1;
#include "switch2.i"
	}
OD

DO_STMT("switch2 (2)")
	{
	n = 2;
#include "switch2.i"
	}
OD

DO_STMT("switch2 (101)")
	{
	n = 101;
#include "switch2.i"
	}
OD
}
