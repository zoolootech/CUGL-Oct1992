
/*
	HEADER:         CUG308;
	TITLE:          Generic doubly linked list module;
	DESCRIPTION:    "High level doubly linked list management functions";
	VERSION:        2.01;
	DATE:           5/6/90;
	COMPILERS:      Standard C;
	KEYWORDS:       linked list generic;
	FILENAME:       LIST.C;
	SEE-ALSO:       LIST.DOC, LIST.H;
	AUTHOR:         Michael Kelly
			254 Gold St. Boston Ma. 02127;
	COPYRIGHT:	May be used freely if authorship is acknowledged;


*/

/*
        WARNINGS:       "In V. 2.0 the function listsfree() has been
                        REMOVED and the List member variable id has
			been ADDED.  Client code for older versions
			must be modified to work with V. 2.0.
			See COMMENTS section in LIST.DOC.";

	REQUIREMENTS:   "Requires C compiler that dereferences function
			pointers { treats func() as (*func)() } or you
			must alter the source.  Each list must be
			initialized with initlist() or calling list
			manipulation functions will use NULL function
			pointers.";
*/


/*
 *  modification notes:
 *
 *      2-17-90     Expanded available Lists to six.            V. 1.07
 *
 *      2-18-90     Added code to clear lerror variable inside of functions
 *                  so that it reflects status of the last list operation.
 *
 *      3-19-90     Started work on major modification to eliminate limitation
 *                  on number of active lists by eliminating the static list
 *                  table and adding an "id" member variable to the list
 *                  structure.  Goal is to use the id as an index into
 *                  a dynamically sized table of list pointers, eliminate
 *                  the include files and the kludges contained therein.
 *
 *      3-20-90     New version up and running.             V. 2.0
 *                  Fatal bugs squashed.  Tuning begun.
 *
 *	3-22-90	    added compact_list_table()
 *
 *      3-23-90     added check_id() to centralize list id check
 *                  and setting of lerror variable to either OK
 *                  or INV_ID, combining repeated code.
 *
 *	4-23-90	    spruced up documentation some	    V. 2.01
 */


/*
 *  LIST
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#if defined(__TURBOC__)
#include <mem.h>
#endif

#include "list.h"


/*
 *  Global List error indicator ( like errno )
 *  initialized here -- see LIST.H
 */
enum Lerror lerror = OK;


/*
 *  Private file-wide variables
 */
static List **list = NULL;
static size_t list_slots = 0, active_lists = 0;
static int handle;      /* set to list id in lqsort() -- used by lqcmp() */


/*
 *  Member function prototypes	 --  "methods"
 */
static int ladd(int id, void *item, size_t itemsize, enum Place place);
static int lchgcompare(int id, int (*newcompare)());
static int lreplitem(int id, void *newitem, size_t newsize);
static int ldelete(int id);
static int lremitem(int id, void *itembuf);
static int lgetitem(int id, void *itembuf);
static size_t lgetsize(int id);
static void *lgetptr(int id);
static int ldestroy(int id);
static int lcmpitem(int id, void *item1);
static int lqsort(int id);
static int lfirst(int id);
static int llast(int id);
static int lnext(int id);
static int lprev(int id);
static int lfinditem(int id, void *item1);

/*
 *  Internal functions
 */
static int check_id(int id);            /*  checks for valid List id */
static void ldestructor(void);		/*  List destructor */

/*
 *  gate to client compare function, used by lqsort()
 */
static int lqcmp(Entry **entry1, Entry **entry2);


/*
 *  Public non-member function definitions
 */

/*
 *  initialize new doubly linked list
 */
int initlist(List *newlist, int (*cmpfunc)())
{
    List **templist;
    List *listptr = NULL;
    int i = 0;
    static int destructor_set = 0;

    if(! destructor_set)  {
	if(atexit(ldestructor))  {
	    fprintf(stderr,"\n\tList Destructor Installation Failure!\n");
	    exit(1);
	}

	destructor_set = 1;
    }

    if(! newlist  ||  ! cmpfunc)  {
	lerror = NULL_PTR;
	return 0;
    }

    if(list)  {
        templist = list;
        while(i++ < list_slots)  
            if(*templist == newlist)  {
                lerror = RE_INIT;
                return 0;
            }
            else
                ++templist;
    }
    
    if(! list)  {
        list = (List **) calloc( LIST_SLOT_INC, sizeof(List *));
        if(! list)  {
            lerror = NO_MEM;
            return 0;
        }
        newlist->id = i = 0;
        list[i] = newlist;
        ++active_lists;
        list_slots += LIST_SLOT_INC;
    }
    else if(active_lists == list_slots)  {
        templist = (List **)
            realloc(list, (list_slots + LIST_SLOT_INC) * sizeof(List *));
        if(! templist)  {
            lerror = NO_MEM;
            return 0;
        }
	list = templist;
	for(i = 0; i < LIST_SLOT_INC;i++)
	    list[list_slots + i] = NULL;
        i = active_lists++;
        list_slots += LIST_SLOT_INC;
        newlist->id = i;
        list[i] = newlist;
    }
    else  {
        for(i = 0;i < list_slots;i++)
        	if(! list[i])  {
        	    newlist->id = i;
        	    list[i] = newlist;
        	    ++active_lists;
        	    goto load_members;
        	}
        lerror = FATAL_ERROR;
        return 0;
    }

load_members:

    listptr = list[i];

    listptr->add = ladd;
    listptr->delete = ldelete;
    listptr->remitem = lremitem;
    listptr->chgcompare = lchgcompare;
    listptr->find = lfinditem;
    listptr->replitem = lreplitem;
    listptr->cmpitem = lcmpitem;
    listptr->sort = lqsort;
    listptr->getitem = lgetitem;
    listptr->getsize = lgetsize;
    listptr->getptr = lgetptr;
    listptr->first = lfirst;
    listptr->last = llast;
    listptr->next = lnext;
    listptr->prev = lprev;
    listptr->destroy = ldestroy;
    listptr->compare = cmpfunc;

    lerror = OK;

    listptr->First = listptr->Last = listptr->Current = NULL;
    listptr->entries = 0;

    return 1;
}

/*
 *  move active list pointers to lowest position in table and
 *  release unused table memory
 */
int compact_list_table(void)
{
    int open_slot = 0, list_index = 0;
    List *listptr;
    List **templist;
    int pass = 0;

    if(! list)  {
	lerror = NO_LISTS;
	return 0;
    }

    lerror = OK;
    if(list_slots == active_lists)
        return 1;


    if(! active_lists  &&  list)  {
        free(list);
        list = NULL;
	list_slots = 0;
        return 1;
    }

scan_table:

    while((open_slot < list_slots)  &&  list[open_slot])
        open_slot++;

    if((open_slot == list_slots)  && ! pass)
	return 1;
    pass++;

    if(list_index < open_slot)
	list_index = open_slot;
    for( ;list_index < list_slots;list_index++)  {
        if(! list[list_index]) continue;
        listptr = list[list_index];
        list[list_index] = NULL;
        listptr->id = open_slot;
        list[open_slot] = listptr;
        goto scan_table;
    }
    templist = (List **) realloc(list, active_lists * sizeof(List *));
    if(! templist)  {
        lerror = NO_MEM;
        return 0;
    }
    list = templist;
    list_slots = active_lists;
    return 1;
}


/*
 *  Private function definitions
 */


/*
 *  add a new item to list at the first, last, or after the current position
 */
static int ladd(int id, void *item, size_t itemsize, enum Place place)
{
    List *listptr = NULL;
    Entry *newentry;
    Link *newlink;
    void *ptr = NULL;


    /*
     *  need this so empty char arrays aren't flagged as
     *  NULL ptrs, at least in TurboC 2, anyway
     */
    ptr = item;

    if(! ptr)  {
	lerror = NULL_PTR;
	return 0;
    }

    if(itemsize < 1)  {
	lerror = INV_SIZE;
	return 0;
    }

    if(! check_id(id))
	return 0;

    listptr = list[id];

    newlink = (Link *) calloc(1, sizeof(Link));
    if(! newlink)  {
	lerror = NO_MEM;
	return 0;
    }

    newentry = (Entry *) calloc(1, sizeof(Entry));
    if(! newentry)  {
	lerror = NO_MEM;
	free(newlink);
	return 0;
    }

    newentry->item = calloc(1, itemsize);
    if(! newentry->item)  {
	lerror = NO_MEM;
	free(newentry);
	free(newlink);
	return 0;
    }

    memmove(newentry->item, item, itemsize);
    newentry->itemsize = itemsize;
    newlink->entry = newentry;

    /*
     *  if empty list
     */
    if(! listptr->entries)  {
	newlink->next = newlink->prev = NULL;
	listptr->First = listptr->Last = listptr->Current = newlink;
	listptr->entries = 1;
	return 1;
    }

    if(place == FIRST)  {
	newlink->prev = NULL;
	newlink->next = listptr->First;
	listptr->First->prev = newlink;
	listptr->First = newlink;
    }
    else if(place == LAST  ||  listptr->Current == listptr->Last)  {
	newlink->next = NULL;
	newlink->prev = listptr->Last;
	listptr->Last->next = newlink;
	listptr->Last = newlink;
    }
    else  {
	newlink->next = listptr->Current->next;
	newlink->prev = listptr->Current;
	listptr->Current->next->prev = newlink;
	listptr->Current->next = newlink;
    }

    listptr->Current = newlink;
    listptr->entries++;
    return 1;
}

static int lchgcompare(int id, int (*newcompare)())
{
    if(! newcompare)  {
	lerror = NULL_PTR;
	return 0;
    }

    if(! check_id(id))
	return 0;

    list[id]->compare = newcompare;

    return 1;
}

/*
 *  delete the current item from the list
 */
static int ldelete(int id)
{
    List *listptr = NULL;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->entries)  {
	lerror = EMPTY_LIST;
	return 0;
    }

    free(listptr->Current->entry->item);
    free(listptr->Current->entry);

    /*
     *  deleting the only item in the list
     */
    if(listptr->entries == 1)  {
	free(listptr->First);
	listptr->First = listptr->Last = listptr->Current = NULL;
	listptr->entries = 0;
	return 1;
    }


    if(listptr->Current == listptr->First)  {
	listptr->First = listptr->First->next;
	listptr->First->prev = NULL;
	free(listptr->Current);
    }
    else if(listptr->Current == listptr->Last)  {
	listptr->Last = listptr->Last->prev;
	listptr->Last->next = NULL;
	free(listptr->Current);
    }
    else  {
	listptr->Current->prev->next = listptr->Current->next;
	listptr->Current->next->prev = listptr->Current->prev;
	free(listptr->Current);
    }

    listptr->Current = listptr->First;
    listptr->entries--;
    return 1;
}

/*
 *  destroy the current list
 */
static int ldestroy(int id)
{
    if(! check_id(id))
	return 0;

    while(ldelete(id))
	;	/*  empty loop -- deletes every link in list  */

    if(lerror == EMPTY_LIST)
	lerror = OK;

    list[id]->id = -1;
    list[id] = NULL;
    --active_lists;
    return 1;
}

/*
 *  copy the current item in the list to buffer
 */
static int lgetitem(int id, void *itembuf)
{
    List *listptr = NULL;
    void *ptr = NULL;

    ptr = itembuf;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->entries)  {
	lerror = EMPTY_LIST;
	return 0;
    }

    if(! ptr  ||  ! listptr->Current)  {
	lerror = NULL_PTR;
	return 0;
    }

    memmove(ptr, listptr->Current->entry->item,
	listptr->Current->entry->itemsize);

    return 1;
}

/*
 *  returns size of current item in list
 */
static size_t lgetsize(int id)
{
    List *listptr = NULL;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->entries)  {
	lerror = EMPTY_LIST;
	return 0;
    }

    if(! listptr->Current)  {
	lerror = NULL_PTR;
	return 0;
    }

    return listptr->Current->entry->itemsize;
}

/*
 *  returns void * to "current" item in list
 */
static void *lgetptr(int id)
{
    List *listptr = NULL;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->entries)  {
	lerror = EMPTY_LIST;
	return NULL;
    }

    if(! listptr->Current)  {
	lerror = NULL_PTR;
	return NULL;
    }

    return listptr->Current->entry->item;
}

/*
 *  copy current item in list to itembuf, then delete it from list
 */
static int lremitem(int id, void *itembuf)
{
    if(! lgetitem(id, itembuf))
	return 0;

    return ldelete(id);
}

/*
 *  compare item1 with current item in list
 */
static int lcmpitem(int id, void *item1)
{
    List *listptr = NULL;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->entries)  {
	lerror = EMPTY_LIST;
	return 0;
    }

    if(! listptr->Current  ||  ! item1)  {
	lerror = NULL_PTR;
	return 0;
    }

    return listptr->compare(item1, listptr->Current->entry->item);
}

/*
 *  make first item in list the "current" item
 */
static int lfirst(int id)
{
    List *listptr = NULL;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->First)  {
	lerror = EMPTY_LIST;
	return 0;
    }
    else
	listptr->Current = listptr->First;

    return 1;
}

/*
 *  make last item in list the "current" item
 */
static int llast(int id)
{
    List *listptr = NULL;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->Last)  {
	lerror = EMPTY_LIST;
	return 0;
    }
    else
	listptr->Current = listptr->Last;

    return 1;
}

/*
 *  move current link pointer to next link if it exists
 *  return 0 if next is NULL, 1 otherwise
 */
static int lnext(int id)
{
    List *listptr = NULL;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->Current)  {
	lerror = EMPTY_LIST;
	return 0;
    }

    if(! listptr->Current->next)
	return 0;
    else
	listptr->Current = listptr->Current->next;

    return 1;
}

/*
 *  move current link pointer to prev link if it exists
 *  return 0 if prev is NULL, 1 otherwise
 */
static int lprev(int id)
{
    List *listptr = NULL;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->Current)  {
	lerror = EMPTY_LIST;
	return 0;
    }

    if(! listptr->Current->prev)
	return 0;
    else
	listptr->Current = listptr->Current->prev;

    return 1;
}

/*
 *  search the list for item that matches item1
 */
static int lfinditem(int id, void *item1)
{
    List *listptr = NULL;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->entries)  {
	lerror = EMPTY_LIST;
	return 0;
    }

    lfirst(id);

    while(lcmpitem(id, item1)  &&  lnext(id))
	;	/*  empty loop -- sequential search for match */

    return !(lcmpitem(id, item1));  /* reverse the result (like strcmp) */
}

/*
 *  replace the "current" item in list with newitem
 */
static int lreplitem(int id, void *newitem, size_t newsize)
{
    List *listptr = NULL;
    void *newdata;

    if(! check_id(id))
	return 0;

    listptr = list[id];

    if(! listptr->entries)  {
	lerror = EMPTY_LIST;
	return 0;
    }

    newdata = calloc(1, newsize);
    if(! newdata)  {
	lerror = NO_MEM;
	return 0;
    }

    free(listptr->Current->entry->item);
    listptr->Current->entry->item = newdata;
    listptr->Current->entry->itemsize = newsize;
    memmove(listptr->Current->entry->item, newitem, newsize);

    return 1;
}

/*
 *  lqsort() uses the client-supplied compare() function
 *  with host qsort() function to sort the List
 *
 *  returns:        0   if list is empty or not enough memory
 *			for sorting table
 *
 *			sets lerror to error code
 *
 *
 *                  1   if sort completed
 *
 *			first item in list is "current" item
 */
static int lqsort(int id)
{
    Entry **entry_array;
    List *listptr = NULL;
    Entry **save_array_base;

    if(! check_id(id))
	return 0;

    listptr = list[id];
    lerror = (listptr->entries > 0) ? OK : EMPTY_LIST;

    if(listptr->entries < 2)
	return listptr->entries;

    entry_array = (Entry **) calloc(listptr->entries, sizeof(Entry *));

    if(! entry_array)  {
	lerror = NO_MEM;
	return 0;
    }
    save_array_base = entry_array;

    if(! lfirst(id))  {
	free(entry_array);
	return 0;
    }

    do  {

	*entry_array++ = listptr->Current->entry;
    }
    while(lnext(id));

    handle = id;
    entry_array = save_array_base;
    qsort(entry_array,listptr->entries, sizeof entry_array[0], lqcmp);

    if(! lfirst(id))  {
	free(entry_array);
	return 0;
    }

    do  {

	listptr->Current->entry = *entry_array++;
    }
    while(lnext(id));

    lfirst(id);
    entry_array = save_array_base;
    free(entry_array);
    return 1;
}

/*
 *  gate to client compare function, used by lqsort
 */
static int lqcmp(Entry **entry1, Entry **entry2)   /* Private */
{
    return list[handle]->compare((*entry1)->item, (*entry2)->item);
}

/*
 *  check that id is in range, and that the list pointer
 *  in that element is not NULL
 */
static int check_id(int id)
{
    if((id > -1)  &&  (id < list_slots)  &&  list[id])  {
        lerror = OK;
        return 1;
    }

    lerror = INV_ID;
    return 0;
}

/*
 *  List destructor
 *
 *  deallocates memory for all active lists at program exit()
 */
static void ldestructor(void)
{
    int i = list_slots - 1;

    if(! list)
        return;

    do  {

	if(list[i])
	    ldestroy(i);

    }
    while(i--);

    if(list)
        free(list);
}
