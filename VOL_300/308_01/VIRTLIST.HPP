
/*
	TITLE:          C++ virtual doubly-linked list object header;
	DESCRIPTION:    "Header for virtual doubly-linked list object";
	VERSION:        1.01;
	DATE:           9/22/90;
	COMPILERS:      Borland Turbo C++ V.1.0;
	KEYWORDS:       virtual list object header;
	FILENAME:       VirtList.hpp;
	SEE-ALSO:       VirtList.cpp;

	AUTHOR:         Michael Kelly
			254 Gold St. Boston, Ma. 02127
			Copyright 1990;

*/


#if !defined(VIRT_LIST_HPP)
#define VIRT_LIST_HPP

#include <dos.h>
#include <alloc.h>

#if !defined(DISKLIST_HPP)
#include "disklist.hpp"
#endif


#define SIZE_MAX 32000
const unsigned long dynamic_min = SIZE_MAX  + _stklen;

class VirtualList  :  public DiskList  {
  protected:
    static size_t buff_references;
    static void *buff1, *buff2;
    int compare(void *item1, void *item2);

  public:

    VirtualList(void);
    ~VirtualList(void);

    Boolean
	add_item(void *item, size_t itemsize, Place place = LastPlace);
    Boolean get_item(void *itembuf);

    size_t get_size(void);
    int compare_item(void *item1);
};

#endif
