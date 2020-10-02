
/*
	TITLE:          C++ doubly-linked list object header file;
	DESCRIPTION:    "Header file for doubly-linked list object";
	VERSION:        1.01;
	DATE:           9/22/90;
	COMPILERS:      Borland Turbo C++ V.1.0;
	KEYWORDS:       list object header;
	FILENAME:       Dbl_List.hpp;
	SEE-ALSO:       Dbl_List.cpp;

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

 //             ----------< DoubleList Object Header >----------


#if !defined(DBL_LIST_HPP)
#define DBL_LIST_HPP

#include <string.h>

#if !defined(BASELIST_HPP)
#include "baselist.hpp"
#endif

class  DoubleList  :  public BaseList  {
  protected:
    struct DoubleLink  {
	DoubleLink	*Next;
	DoubleLink	*Prev;
	Entry           *entry;
    };

    DoubleLink	*First, *Last, *Current;
    int compare(void *item1, void *item2)
	{ return strcmp((const char *) item1, (const char *) item2); }

  public:
    DoubleList(void)
	{ First = Last = Current = NULL; }	// constructor

    ~DoubleList(void);    		    	// destructor

    const void *get_ptr(void)
	{ return (Current != NULL) ?
	    (const void *)Current->entry->item : 
	    lerror = EMPTY_LIST, (const void *)NULL; }

    size_t get_size(void)
	{ return (Current != NULL) ?
	    Current->entry->itemsize : lerror = EMPTY_LIST, 0; }

    Boolean add_item(void *item, size_t itemsize, Place place = LastPlace);
    Boolean delete_item(void);
    Boolean get_item(void *itembuf);
    Boolean remove_item(void *itembuf)
	{
	  if(! get_item(itembuf))
		return False;

	  return delete_item();
	}

    int compare_item(void *item1);
    Boolean find_item(void *item1);
    Boolean replace_item(void *newitem, size_t newsize);

    Boolean first(void)
	{ return (Current = First) ? True : lerror = EMPTY_LIST, False; }
    Boolean last(void)
	{ return (Current = Last) ? True : lerror = EMPTY_LIST, False; }
    Boolean next(void)
	{ return (Current != Last) ?
	    Current = Current->Next, True : False; }
    Boolean prev(void)
	{ return (Current != First) ?
	    Current = Current->Prev, True : False; }
	    
    Boolean sort(void);
    

};  // class DoubleList


class CircularDoubleList  :  public DoubleList  {
  public:
    Boolean next(void)
	{ return (DoubleList::next()) ? True : first(); }
    Boolean prev(void)
	{ return (DoubleList::prev()) ? True : last(); }
};

#endif
