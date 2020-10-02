/*
	TITLE:		Class BaseList;
	DESCRIPTION:	"C++ virtual base class for deriving linked-list
			objects";

	VERSION:        1.0;
	DATE:           9/21/90l
	COMPILERS:      Borland Turbo C++ V.1.0;
	KEYWORDS:	linked list base class;
	FILENAME:	BaseList.cpp;
	SEE-ALSO:	BaseList.hpp;

	AUTHOR:         Michael Kelly
			254 Gold St. Boston, Ma. 02127
			Copyright 1990;

	COPYRIGHT:	This code may not be commercially distributed
			without prior arrangement with the author.  It
			may be used by programmers, without royalty, for
			their personal programs and for "one of a kind"
			or "custom" applications, provided that said
			programmers assume all liability concerning
			same.
*/


#include "baselist.hpp"

BaseList *this_list = NULL;

int qcompare(const void *entry1, const void *entry2)
{
    return this_list->compare(
	(*(Entry **)entry1)->item, (*(Entry **)entry2)->item);
}
