#include "timer1.h"
	short n;
	int fn();
	int (*pfn)() = &fn;

	DO_STMT("fn call")			fn();			OD;
	DO_STMT("fn call w save")	fns();			OD;
	DO_STMT("fn w 1 arg")		fn1(n);			OD;
	DO_STMT("fn w 2 arg")		fn2(n, n);		OD;
	DO_STMT("fn w 3 arg")		fn3(n, n, n);	OD;
	DO_STMT("fn call *")		(*pfn)();		OD;
	}
int fn() {}
int fns() {register int i; }
int fn1(a) int a; {}
int fn2(a, b) int a, b; {}
int fn3(a, b, c) int a, b, c; {}
