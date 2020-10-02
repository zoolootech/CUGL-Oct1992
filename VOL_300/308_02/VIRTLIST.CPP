
/*
	TITLE:          C++ virtual doubly-linked list object;
	DESCRIPTION:    "Doubly-linked list object that uses
			dynamic memory and disk space for run-time
			data storage";

	VERSION:        1.01;
	DATE:           9/22/90;
	COMPILERS:      Borland Turbo C++ V.1.0;
	KEYWORDS:       virtual list object;
	FILENAME:       VirtList.cpp;
	SEE-ALSO:       VirtList.hpp, dbl_list.hpp;

	REQUIRES:	Header and module files for these:

				***	C++	***

			Class		Header	      	Source

			BaseList	BaseList.hpp	BaseList.cpp
			DoubleList	dbl_list.hpp	dbl_list.cpp
			DiskList	DiskList.hpp	DiskList.cpp
			VirtualList	VirtList.hpp	VirtList.cpp

				*****	C   *****

			debug.obj	debug.h		debug.c


	NOTES:		1) BaseList is a virtual base class, the
			   chain of inheritance is from BaseList
			   directly down the list to VirtualList.
			   Most of the methods are "virtual" so
			   external functions you write can use
			   a BaseList * as a parameter, and call
			   the correct method.  The function
			   bfind(), for example, performs a binary
			   search on a sorted object derived from
			   class BaseList. ( See bfind.cpp ).

			2) See the header files for methods available
			   to the objects.  VirtualList has just been
			   debugged, so documentation is on hold until
			   I play with these objects for a while.  I'm
			   likely to find a few more significant errors,
			   so no need to rush things.

			3) The disk storage routines use the tmpfile(),
			   fseek(), ftell(), fread() and fwrite() ANSI C
			   file functions and should be compatible with
			   systems that use a "long int" for file size.

			4) The Turbo C++ extensions coreleft(), and
			   _stklen are used to determine dynamic memory
			   supply.

			5) Recommend using this object under MS DOS with
			   a "large data" memory model.  I've been using
			   the "COMPACT" model during debugging.

			6) Debug.c supplies the function err_exit(), used
			   for program error termination with optional error
			   location information for debugging.
			   ( See the macro in Debug.h ).

	ENHANCEMENTS:	1) For MS/PC DOS systems, expanded memory could be
			   used, if available, before resorting to disk
			   access.

			2) Checks for available disk space and exception
			   handlers should be added to the code for greater
			   reliability.

			3) tmpname() could be used, instead of tmpfile(),
			   so that an argument could be passed to the
			   program indicating a "ram disk" to be used
			   in place of the default drive.  The sort()
			   method is orders of magnitude faster when
			   all data items being compared are in memory.

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


#include "virtlist.hpp"

void * VirtualList::buff1 = NULL;
void * VirtualList::buff2 = NULL;
size_t VirtualList::buff_references = 0;

VirtualList::VirtualList(void)
{
    if(! buff1  &&  ! buff2)
    {
	if(! (buff1 = new char[SIZE_MAX])  ||
	   ! (buff2 = new char[SIZE_MAX]))
		err_exit("Not enough dynamic memory for virtual lists");
    }
    buff_references++;
}

VirtualList::~VirtualList(void)
{
    if(! --buff_references)  {
	if(buff2)  {
	    delete buff2;
	    buff2 = NULL;
	}

	if(buff1)  {
	    delete buff1;
	    buff1 = NULL;
	}
    }
}

Boolean VirtualList::add_item(void *item, size_t itemsize, Place place)
{
    if(itemsize > SIZE_MAX)  {
	lerror = INV_SIZE;
	return False;
    }
    if(coreleft() > dynamic_min)
	return DoubleList::add_item(item, itemsize, place);

    return DiskList::add_item(item, itemsize, place);
}

Boolean VirtualList::get_item(void *itembuf)
{
    DiskLink *tmplink;

    if(! entries)  {
	lerror = EMPTY_LIST;
	return False;
    }

    if(Current->entry->itemsize == sizeof(DiskLink))  {
	tmplink = (DiskLink *)Current->entry->item;
	if(tmplink->dl_id == DL_ID)
	    return DiskList::get_item(itembuf);
	else
	    return DoubleList::get_item(itembuf);
    }
    else
	return DoubleList::get_item(itembuf);
}

size_t VirtualList::get_size(void)
{
    DiskLink *tmplink;

    if(! entries)  {
	lerror = EMPTY_LIST;
	return False;
    }

    tmplink = (DiskLink *)Current->entry->item;

    if(tmplink->dl_id == DL_ID)
	return DiskList::get_size();
    else
	return DoubleList::get_size();
}

int VirtualList::compare_item(void *item1)
{
    DiskLink *tmplink;
    int dif;

    if(! entries)  {
	lerror = EMPTY_LIST;
	return False;
    }

    tmplink = (DiskLink *)Current->entry->item;

    if( (Current->entry->itemsize == sizeof(DiskLink)) &&
	(tmplink->dl_id == DL_ID) )
    {
	if(buff2) delete buff2;
	dif = DiskList::compare_item(item1);
	if(! (buff2 = new char[SIZE_MAX]))
	    lerror = NO_MEM;
	return dif;
    }
    else
	return DoubleList::compare_item(item1);
}

int VirtualList::compare(void *item1, void *item2)
{
    DiskLink *d1, *d2;
    void *an_item;
    int dif;

    d1 = (DiskLink *)item1;
    d2 = (DiskLink *)item2;


    if(d1->dl_id != DL_ID  &&  d2->dl_id != DL_ID)
	return DoubleList::compare(item1, item2);

    else if(d1->dl_id == DL_ID  &&  d2->dl_id == DL_ID)  {
	if(buff2) delete buff2;
	if(buff1) delete buff1;
	dif = DiskList::compare(item1, item2);
	if(! (buff1 = new char[SIZE_MAX])  ||
	   ! (buff2 = new char[SIZE_MAX]))
		lerror = NO_MEM;
	return dif;
    }

    else if(d1->dl_id == DL_ID)  {
	if(buff2) delete buff2;
	an_item = new char[d1->itemsize];
	if(! an_item)  {
	    lerror = NO_MEM;
	    return 0;
	}
	fseek(fp, d1->itemsize, SEEK_SET);
	fread(an_item, d1->itemsize, 1, fp);
	dif = DoubleList::compare(an_item, item2);
	delete an_item;
	if(! (buff2 = new char[SIZE_MAX]))
	    lerror = NO_MEM;
	return dif;
    }
    else  {
	if(buff2) delete buff2;
	an_item = new char[d2->itemsize];
	if(! an_item)  {
	    lerror = NO_MEM;
	    return 0;
	}
	fseek(fp, d2->itemsize, SEEK_SET);
	fread(an_item, d2->itemsize, 1, fp);
	dif = DoubleList::compare(item1, an_item);
	delete an_item;
	if(! (buff2 = new char[SIZE_MAX]))
	    lerror = NO_MEM;
    }
    return dif;
}
