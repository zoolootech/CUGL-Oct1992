/*
	TITLE:		BASELIST;
	DESCRIPTION:	C++ Virtual Base Class for Linked List
			Classes;

	FILENAME:	BASELIST.HPP;
	VERSION:	1.0;
	DATE:           9/21/90;
	COMPILER:	BORLAND TURBO C++ V.1.0;

	AUTHOR:		Michael Kelly
			254 Gold Street
			Boston, MA 02127
			Copyright 1990;

	COPYRIGHT:	This code may not be commercially distributed
			without prior arrangement with the author.  It
			may be used by programmers, without royalty, for
			their personal programs and for "one of a kind"
			or "custom" applications, provided that said
			programmers assume all liability concerning
			same.
*/

#if !defined(BASELIST_HPP)
#define BASELIST_HPP

#include <stdio.h>
#include <stddef.h>

 //  global List error indicator (like errno)

typedef enum Lerror  {
    OK,                 // no error
    NO_MEM,             // not enough memory
    NULL_PTR,           // NULL pointer - (usually a function argument)
    EMPTY_LIST,         // empty List
    INV_SIZE,           // invalid size - (usually itemsize == 0)
    INV_OP		// invalid operation - (inappropriate method)
} Lerror;

extern Lerror lerror;

typedef enum Boolean  {
    False, True } Boolean;

typedef enum Place  {
    FirstPlace, LastPlace, AfterCurrentPlace } Place;


  //     ----------------< BaseList Class Definition >----------------

		//  Abstract Class for Linked List Type Classes

class BaseList  {
  protected:
    struct Entry  {
	size_t itemsize;
	void   *item;
    };

    size_t entries;
    Lerror lerror;
    virtual int compare(void *item1, void *item2) = 0;
  public:
    BaseList(void)  					// constructor
	{ entries = 0; lerror = OK; }

    virtual ~BaseList(void) {}

  friend int qcompare(const void *entry1, const void *entry2);

    size_t get_entries(void)
	{ return entries; }
    Lerror error(void)
	{ return lerror; }

    virtual const void *get_ptr(void) = 0;
    virtual size_t get_size(void) = 0;
    virtual Boolean
	add_item(void *item, size_t itemsize, Place place = LastPlace) = 0;
    virtual Boolean delete_item(void) = 0;
    virtual Boolean get_item(void *itembuf) = 0;
    virtual Boolean remove_item(void *itembuf) = 0;
    virtual int compare_item(void *item1) = 0;
    virtual Boolean find_item(void *item1) = 0;
    virtual Boolean replace_item(void *newitem, size_t newsize) = 0;

    virtual Boolean first(void) = 0;
    virtual Boolean last(void) 	= 0;
    virtual Boolean next(void) 	= 0;
    virtual Boolean prev(void) 	= 0;
    virtual Boolean sort(void) 	= 0;
};

extern BaseList *this_list;

#endif
