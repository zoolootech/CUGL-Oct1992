
/*
	TITLE:          C++ SingleList Object Header;
	DESCRIPTION:    "Header file for singly-linked list object";
	VERSION:        1.0;
	DATE:           9/21/90;
	COMPILERS:      Borland Turbo C++ V.1.0;
	KEYWORDS:       list object header;
	FILENAME:       Sgl_List.hpp;
	SEE-ALSO:       Sgl_List.cpp;

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

	NOTES:		These singly-linked lists have the same
			functionality as the doubly-linked versions
			( DoubleList and CircularDoubleList )
			but sacrifice speed to save space.  A
			link pointer is saved for each item in
			the list.
*/

 //             ----------< SingleList Object Header >----------


#if !defined(SGL_LIST_HPP)
#define SGL_LIST_HPP

#include <string.h>
#include "baselist.hpp"

class  SingleList  :  public BaseList  {
  protected:
    struct SingleLink  {
	SingleLink	*Next;
	Entry           *entry;
    };

    SingleLink	*First, *Last, *Current;
    int compare(void *item1, void *item2)
	{ return strcmp((const char *) item1, (const char *) item2); }

  public:
    SingleList(void)
	{ First = Last = Current = NULL; }	// constructor

    ~SingleList(void);    				// destructor

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
    Boolean prev(void);		// more involved than next() with 1 link
    Boolean sort(void);
};  // class SingleList


class CircularSingleList  :  public SingleList  {
  public:
    Boolean next(void)
	{  return (SingleList::next()) ? True : first(); }
    Boolean prev(void)
	{  return (SingleList::prev()) ? True : last();  }
};

#endif
