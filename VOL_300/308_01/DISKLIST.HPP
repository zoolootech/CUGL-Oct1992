
/*
	TITLE:		Class DiskList;
	DESCRIPTION:	"C++ DiskList class header file";
	VERSION:        1.01;
	DATE:		9/22/90;
	COMPILERS:      Borland Turbo C++ V.1.0;
	KEYWORDS:	linked list header;
	FILENAME:	DiskList.hpp;
	SEE-ALSO:	DiskList.cpp;

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


#if !defined(DISKLIST_HPP)
#define DISKLIST_HPP

#include <stdlib.h>
#include <stdio.h>

#if !defined(DBL_LIST_HPP)
#include "dbl_list.hpp"
#endif

#define DEBUG
#include "debug.h"


#define DL_ID -2

class DiskList  :  public  DoubleList  {
  protected:
    FILE   *fp;

    struct DiskLink  {
	long    item_offset;
	size_t	itemsize;
	int	dl_id;
    };

    int compare(void *item1, void *item2);

  public:
    DiskList(void);
    ~DiskList(void);

    Boolean
	add_item(void *item, size_t itemsize, Place place = LastPlace);
    Boolean get_item(void *itembuf);
    const void *get_ptr(void)
	{ lerror = INV_OP; return NULL; }

    size_t get_size(void);
    virtual Boolean remove_item(void *itembuf);
    int compare_item(void *item1);
};

#endif
