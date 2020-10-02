
/*
	HEADER          CUG308;
	TITLE:          Binary List search header;
	DATE:           9/8/90;
	VERSION:        1.0;
	FILENAME:       BFIND.HPP;
	COMPILER:       Borland C++ V.1.0;
	REQUIRES:	LIST.CPP, LIST.HPP, BFIND.CPP;
	SEE-ALSO:       BFIND.CPP, LIST.CPP, LIST.HPP;

	AUTHOR:         Michael Kelly
	                254 Gold St. Boston, MA 02127
			Copyright 1990;

	COPYRIGHT:	This code may not be commercially distributed
			without prior arrangement with the author.  It
			may be used by programmers, without royalty, for
			their personal programs and for "one of a kind"
			or "custom" applications, provided that said
			programmers assume all liability concerning
			same.
*/

#if !defined(BFIND_HPP)
#define BFIND_HPP
#include "baselist.hpp"

Boolean bfind(BaseList *list, void *item);
#endif

