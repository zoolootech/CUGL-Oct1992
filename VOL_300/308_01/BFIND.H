

/*
	HEADER:         CUG308;
	TITLE:          binary search header file;

	DATE:           5/6/90;
	VERSION:        2.01;
	FILENAME:       BFIND.H;
	SEE-ALSO:       BFIND.C, LIST.DOC, LIST.H;

	REQUIREMENTS:   LIST.OBJ V. 2.0 or higher, LIST.H, BFIND.C;

	AUTHOR:         Michael Kelly
			254 Gold St. Boston Ma. 02127;
	COPYRIGHT:	May be used freely if authorship is acknowledged;
*/


#if !defined(BFIND_H)
#define BFIND_H
#include "list.h"

int bfind(List *list, void *item);
#endif

