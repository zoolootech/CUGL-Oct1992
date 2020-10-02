
/*
	HEADER          CUG308;
	TITLE:          Binary search on sorted List Object;
	DATE:           9/21/90;
	VERSION:        1.0;
	FILENAME:       BFIND.CPP;
	COMPILER:       Borland C++ V.1.0;
	REQUIRES:	BASELIST.CPP, BASELIST.HPP, BFIND.HPP;
	SEE-ALSO:       DBL_LIST.HPP, SGL_LIST.HPP;

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

/*
 *  BFIND
 */

#include <stddef.h>
#include "bfind.hpp"

/*
 *  routine to binary search sorted linked list instance
 *  derived from class BaseList.
 *
 *  note:	BaseList is a virtual base class for linked list
 *		objects.  It uses virtual functions, so calls made
 *		through the BaseList pointer call the appropriate
 *		function in the derived class.  I recommend using
 *		class DoubleList with this function as the prev()
 *		method for the SingleList class is slow ( having
 *		only a forward pointing link in each node ).
 *
 *  arguments:  list -  a pointer of class type BaseList
 *
 *		item - void pointer to data to search for
 *
 *  warnings:	The linked list must be sorted or results are erroneous
 *
 *  returns:	True if item found -- item is the "current" item
 *
 *		False if item not found -- "current" location undefined
 *		( Use first() or last() method on return, to reset list
 *		  "cursor" or "current location" to a known state.     )
 */
Boolean bfind(BaseList *list, void *item)
{
    size_t i;                   // loop counter
    size_t gap;			// gap used to halve search path
    const int opt = 3;		// use this for performance tuning
    const int termcount = 2;	// for ending "forever" while loop

    enum direction { left, right } direc;  	// direction to step
    int dif;                                    // comparison result
    int count = 0;				// count of single steps
    size_t list_entries = list->get_entries();   // entries in List

    if(! list_entries)
	return False;

    if(list_entries < opt)
	return(list->find_item(item));

        // compare to smallest item

    list->first();
    if((dif = list->compare_item(item)) < 1)
	return (Boolean) !dif;

        // compare to largest item

    list->last();
    if((dif = list->compare_item(item)) > -1)
	return (Boolean) !dif;

        // partition list in two

    gap = (list_entries >> 1) + (list_entries & 1);

    direc = left;

	// "forever" while loop

    while(gap)  {
	if(direc == left)
	    for(i = 0;i < gap;i++)
		list->prev();
	else
	    for(i = 0;i < gap;i++)
		list->next();

	/*
	 *  after stepping to center of partion,
	 *  make another comparison
	 */
	if(!(dif = list->compare_item(item)))
	    return True;    // return True if "current" item is a match
	else if(dif < 0)    // else ...
	    direc = left;   // set direction according to comparison
	else
	    direc = right;

	if(gap == 1)
	    ++count;
	if(count == termcount)
	    break;  		// gaurantee an end to the loop

	gap = (gap >> 1) + (gap & 1);   // re-partition list
    }
    return False;
}
