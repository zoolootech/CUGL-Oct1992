
/*
	TITLE:          Singly-linked list object;
	DESCRIPTION:    "C++ singly-linked list object;
	VERSION:        1.0;
	DATE:           9/21/90;
	COMPILERS:      Borland Turbo C++ V.1.0;
	KEYWORDS:       C++ linked list object;
	FILENAME:       Sgl_List.cpp;
	SEE-ALSO:	Sgl_List.hpp, BaseList.hpp;

	AUTHOR:         Michael Kelly
			254 Gold St. Boston Ma. 02127
			Copyright 1990;

	COPYRIGHT:	This code may not be commercially distributed
			without prior arrangement with the author.  It
			may be used by programmers, without royalty, for
			their personal programs and for "one of a kind"
			or "custom" applications, provided that said
			programmers assume all liability concerning
			same.
*/


 //                 ----------< SingleList Object >----------

#include <stdlib.h>
#include "sgl_list.hpp"


//      	      ----------< Public Methods >----------

/********************
SingleList::~SingleList(void):
	 Description: destructor
	 ********************/

SingleList::~SingleList(void)
{
    while(delete_item())
	;       // empty loop -- deletes every link in list

    if(lerror == EMPTY_LIST)
	lerror = OK;
}


/********************
Boolean SingleList::add_item(void *item, size_t itemsize, Place place):
	 Description: add a new item to list at Place place
	 ********************/

Boolean SingleList::add_item(void *item, size_t itemsize, Place place)
{
    Entry *newentry;
    SingleLink *newlink;
    void *ptr = item;   // avoids empty char array being flagged as NULL ptr

    lerror = OK;

    if(! ptr)
	lerror = NULL_PTR;
    if(itemsize < 1)
	lerror = INV_SIZE;

    if(lerror)
	return False;

    if(! (newlink = new SingleLink))  {
	lerror = NO_MEM;
	return False;
    }
    if(! (newentry = new Entry))  {
	delete newlink;
	lerror = NO_MEM;
	return False;
    }
    if(! (newentry->item = new char[itemsize]))  {
	delete newentry;
	delete newlink;
	lerror = NO_MEM;
	return False;
    }

    memmove(newentry->item, item, itemsize);
    newentry->itemsize = itemsize;
    newlink->entry = newentry;

    if(! entries)  {                            // adding to empty list
	newlink->Next = NULL;
	First = Last = Current = newlink;
        entries = 1;
	return True;
    }

    if(place == FirstPlace)  {
	newlink->Next = First;
	First = newlink;
    }
    else if(place == LastPlace  ||  Current == Last)  {
	newlink->Next = NULL;
	Last->Next = newlink;
	Last = newlink;
    }
    else  {
	newlink->Next = Current->Next;
	Current->Next = newlink;
    }

    Current = newlink;
    entries++;
    return True;
}


/********************
Boolean SingleList::delete_item(void):
	 Description: delete the current item from the list
	 ********************/

Boolean SingleList::delete_item(void)
{
    lerror = OK;

    if(! entries)
	lerror = EMPTY_LIST;
    if(entries  &&  ! Current)
        lerror = NULL_PTR;

    if(lerror)
	return False;

    delete Current->entry->item;
    delete Current->entry;

    if(entries == 1)  {        // deleting the only item in the list
	delete First;
	First = Last = Current = NULL;
	entries = 0;
	return True;
    }

    if(Current == First)  {
	First = First->Next;
    }
    else if(Current == Last)  {
      SingleLink *save_first = First;
	while(First->Next  &&  First->Next != Last)
	    First = First->Next;
	First->Next = NULL;
	Last = First;
	First = save_first;
    }
    else  {
      SingleLink *save_first = First;
	while(First->Next  &&  First->Next != Current)
	    First = First->Next;
	First->Next = Current->Next;
	First = save_first;
    }

    delete Current;
    Current = First;
    entries--;
    return True;
}


/********************
Boolean SingleList::get_item(void *itembuf):
	 Description: copy the current item in the list to itembuf
	 ********************/

Boolean SingleList::get_item(void *itembuf)
{
    void *ptr = itembuf;

    lerror = OK;

    if(! entries)
	lerror = EMPTY_LIST;
    if(entries  &&  (! ptr  ||  ! Current))
	lerror = NULL_PTR;

    if(lerror)
	return False;

    memmove(itembuf, Current->entry->item, Current->entry->itemsize);
    return True;
}


/********************
Boolean SingleList::prev(void):
	 Description: make previous item the "current" item
	 ********************/

Boolean SingleList::prev(void)
{
    if(Current == First)
	return False;
    else  {
      SingleLink *save_first = First;
	while(First->Next  &&  First->Next != Current)
	    First = First->Next;
	Current = First;
	First = save_first;
    }

    return True;
}

/********************
int SingleList::compare_item(void *item1):
	 Description: compare item1 with current item in list
	 ********************/

int SingleList::compare_item(void *item1)
{
    void *ptr = item1;

    lerror = OK;

    if(! entries)
	lerror = EMPTY_LIST;
    if(entries  &&  (! Current  ||  ! ptr))
	lerror = NULL_PTR;

    if(lerror)
	return 0;

    return compare(item1, Current->entry->item);
}


/********************
Boolean SingleList::find_item(void *item1):
	 Description: search the list for item that matches item1
	 ********************/

Boolean SingleList::find_item(void *item1)
{
    int dif;
    void *ptr = item1;

    lerror = OK;


    if(! entries)
	lerror = EMPTY_LIST;
    if(entries  &&  (! first() || ! ptr))
        lerror = NULL_PTR;

    if(lerror)
	return False;

    while((dif = compare_item(item1))  &&  SingleList::next())
	;	// empty loop -- sequential search for match

    return (Boolean) !dif; // logically convert integer result to boolean
}


/********************
Boolean SingleList::replace_item(void *newitem, size_t newsize):
	 Description: replace the "current" item in list with newitem
	 ********************/

Boolean SingleList::replace_item(void *newitem, size_t newsize)
{
    void *ptr = newitem;
    void *tmp;

    lerror = OK;

    if(! entries)
	lerror = EMPTY_LIST;
    if(entries  &&  ! ptr)
        lerror = NULL_PTR;
    if(! newsize)
        lerror = INV_SIZE;

    if(lerror)
	return False;

    if(! (tmp = new char[newsize]))  {
	lerror = NO_MEM;
	return False;
    }
    delete Current->entry->item;
    Current->entry->item = tmp;
    Current->entry->itemsize = newsize;
    memmove(Current->entry->item, newitem, newsize);
    return True;
}

/*
 *  sort() uses compare() function indirectly through qcompare()
 *  with host qsort() function to sort the SingleList
 *
 *  returns:        0   if SingleList is empty or not enough memory
 *			for sorting table
 *
 *			sets lerror to error code
 *
 *
 *                  1   if sort completed
 *
 *			first item in SingleList is "current" item
 */
Boolean SingleList::sort(void)
{
    Entry **entry_array;
    Entry **save_array_base;

    lerror = (entries > 0) ? OK : EMPTY_LIST;

    if(entries < 2)
	return (Boolean) entries;

    if(! (save_array_base =
	(Entry **) new char[entries * sizeof(Entry *)]))  {
	    lerror = NO_MEM;
	    return False;
    }
    entry_array = save_array_base;

    Current = First;

    do  {

	*entry_array++ = Current->entry;
    }
    while(Current = Current->Next);

    Current = First;
    entry_array = save_array_base;
    this_list = this;
    qsort(entry_array, entries, sizeof entry_array[0], qcompare);

    do  {

	Current->entry = *entry_array++;
    }
    while(Current = Current->Next);

    Current = First;
    delete save_array_base;
    return True;
}
