
/*
	TITLE:		DoubleList;
	DESCRIPTION:	"C++ doubly-linked list	object;
	VERSION:	1.0;
	DATE:		9/7/90;
	COMPILERS:	Borland	Turbo C++ V.1.0;
	KEYWORDS:	C++ linked list	object;
	FILENAME:	Dbl_List.cpp;
	SEE-ALSO:	Dbl_List.hpp, BaseList.hpp;

	AUTHOR:		Michael	Kelly
			254 Gold St. Boston Ma.	02127
			Copyright 1990;

	COPYRIGHT:	This code may not be commercially distributed
			without prior arrangement with the author.  It
			may be used by programmers, without royalty, for
			their personal programs and for "one of a kind"
			or "custom" applications, provided that said
			programmers assume all liability concerning
			same.
*/


 //		    ----------<	DoubleList Object >----------

#include <stdlib.h>
#include "dbl_list.hpp"


//		----------< Public Methods >----------


// destructor

DoubleList::~DoubleList(void)
{
    while(delete_item())
	;	// empty loop -- deletes every link in list

    if(lerror == EMPTY_LIST)
	lerror = OK;
}


// add a new item to list at Place place

Boolean	DoubleList::add_item(void *item, size_t	itemsize, Place	place)
{
    Entry *newentry;
    DoubleLink *newlink;
    void *ptr =	item;	// avoids empty	char array being flagged as NULL ptr

    lerror = OK;

    if(! ptr)
	lerror = NULL_PTR;
    if(itemsize	< 1)
	lerror = INV_SIZE;

    if(lerror)
	return False;

    if(! (newlink = new	DoubleLink))  {
	lerror = NO_MEM;
	return False;
    }
    if(! (newentry = new Entry))  {
	delete newlink;
	lerror = NO_MEM;
	return False;
    }
    if(! (newentry->item = new char[itemsize]))	 {
	delete newentry;
	delete newlink;
	lerror = NO_MEM;
	return False;
    }

    memmove(newentry->item, item, itemsize);
    newentry->itemsize = itemsize;
    newlink->entry = newentry;

    if(! entries)  {				// adding to empty list
	newlink->Next =	newlink->Prev =	NULL;
	First =	Last = Current = newlink;
	entries	= 1;
	return True;
    }

    if(place ==	FirstPlace)  {
	newlink->Prev =	NULL;
	newlink->Next =	First;
	First->Prev = newlink;
	First =	newlink;
    }
    else if(place == LastPlace	||  Current == Last)  {
	newlink->Next =	NULL;
	newlink->Prev =	Last;
	Last->Next = newlink;
	Last = newlink;
    }
    else  {
	newlink->Next =	Current->Next;
	newlink->Prev =	Current;
	Current->Next->Prev = newlink;
	Current->Next =	newlink;
    }

    Current = newlink;
    entries++;
    return True;
}


// delete the current item from	the list

Boolean	DoubleList::delete_item(void)
{
    lerror = OK;

    if(! entries)
	lerror = EMPTY_LIST;
    if(entries	&&  ! Current)
	lerror = NULL_PTR;

    if(lerror)
	return False;

    delete Current->entry->item;
    delete Current->entry;

    if(entries == 1)  {	       // deleting the only item in the	list
	delete First;
	First =	Last = Current = NULL;
	entries	= 0;
	return True;
    }

    if(Current == First)  {
	First =	First->Next;
	First->Prev = NULL;
    }
    else if(Current == Last)  {
	Last = Last->Prev;
	Last->Next = NULL;
    }
    else  {
	Current->Prev->Next = Current->Next;
	Current->Next->Prev = Current->Prev;
    }

    delete Current;
    Current = First;
    entries--;
    return True;
}


// copy	the current item in the	list to	buffer

Boolean	DoubleList::get_item(void *itembuf)
{
    void *ptr =	itembuf;

    lerror = OK;

    if(! entries)
	lerror = EMPTY_LIST;
    if(entries	&&  (! ptr  ||	! Current))
	lerror = NULL_PTR;

    if(lerror)
	return False;

    memmove(itembuf, Current->entry->item, Current->entry->itemsize);
    return True;
}


// compare item1 with current item in list

int DoubleList::compare_item(void *item1)
{
    void *ptr =	item1;

    lerror = OK;

    if(! entries)
	lerror = EMPTY_LIST;
    if(entries	&&  (! Current	||  ! ptr))
	lerror = NULL_PTR;

    if(lerror)
	return 0;

    return compare(item1, Current->entry->item);
}


// search the list for item that matches item1

Boolean	DoubleList::find_item(void *item1)
{
    int	dif;
    void *ptr =	item1;

    lerror = OK;


    if(! entries)
	lerror = EMPTY_LIST;
    if(entries	&&  (! first() || ! ptr))
	lerror = NULL_PTR;

    if(lerror)
	return False;

    while((dif = compare_item(item1))  &&  DoubleList::next())
	;	// empty loop -- sequential search for match

    return (Boolean) !dif; // logically	convert	integer	result to boolean
}


// replace the "current" item in list with newitem

Boolean	DoubleList::replace_item(void *newitem,	size_t newsize)
{
    void *ptr =	newitem;
    void *tmp;

    lerror = OK;

    if(! entries)
	lerror = EMPTY_LIST;
    if(entries	&&  ! ptr)
	lerror = NULL_PTR;
    if(! newsize)
	lerror = INV_SIZE;

    if(lerror)
	return False;

    if(! (tmp =	new char[newsize]))  {
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
 *  sort() uses	compare() function indirectly through dcompare()
 *  with host qsort() function to sort the DoubleList
 *
 *  returns:	    0	if DoubleList is empty or not enough memory
 *			for sorting table
 *
 *			sets lerror to error code
 *
 *
 *		    1	if sort	completed
 *
 *			first item in DoubleList is "current" item
 */
Boolean	DoubleList::sort(void)
{
    Entry **entry_array;
    Entry **save_array_base;

    lerror = (entries >	0) ? OK	: EMPTY_LIST;

    if(entries < 2)
	return (Boolean) entries;

    if(! (save_array_base =
	(Entry **) new char[entries * sizeof(Entry *)]))  {
	    lerror = NO_MEM;
	    return False;
    }
    entry_array	= save_array_base;

    Current = First;

    do	{

	*entry_array++ = Current->entry;
    }
    while(Current = Current->Next);

    Current = First;
    entry_array	= save_array_base;
    this_list =	this;
    qsort(entry_array, entries,	sizeof entry_array[0], qcompare);

    do	{

	Current->entry = *entry_array++;
    }
    while(Current = Current->Next);

    Current = First;
    delete save_array_base;
    return True;
}


