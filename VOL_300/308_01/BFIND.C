
/*
	HEADER:         CUG308;
	TITLE:          binary search of sorted List;

	DATE:           5/6/90;
	VERSION:        2.01;
	FILENAME:       BFIND.C;
	SEE-ALSO:       BFIND.H, LIST.DOC, LIST.H, LIST.C;

	USEAGE:         "Compile to .obj and link with list.obj and
			your main program";

	REQUIREMENTS:   LIST.OBJ V. 2.0 or higher, LIST.H, BFIND.H;

	AUTHOR:         Michael Kelly
			254 Gold St. Boston Ma. 02127;
	COPYRIGHT:	May be used freely if authorship is acknowledged;
*/


/*
 *  BFIND
 */

#include <stddef.h>
#include "bfind.h"


/*
 *  routine to binary search sorted List ( see list.h )
 *
 *  requires:	List pointer to a List that has been sorted with sort()
 *		List member function
 *
 *  warning:	the current list.compare() function should be the same
 *		as, or consistent with, the one used for the sort
 *
 *		( see list.chgcompare() in "list.doc" file )
 *
 *  returns:	1 if item found -- item is now "current"
 *
 *		0 if not found -- "current" item undefined
 */
int bfind(List *list, void *item)
{
    size_t i;                   /* loop counter			         */
    size_t gap;			/* gap used to halve search path 	 */
    const int opt = 3;		/* use this for performance tuning 	 */
    const int termcount = 2;	/* for ending "forever" while loop 	 */

    enum direction{ left, right } direc;  	/* direction to step     */
    int (*step[2])();				/* to step through list  */
    int dif;                                    /* comparison result     */
    int count = 0;				/* count of single steps */


    if(! list->entries)
	return 0;

    if(list->entries < opt)
	return(list->find(list->id, item));

    step[left] = list->prev;
    step[right] = list->next;

    list->first(list->id);

    /*
     *  if item less than smallest item in list,
     *  dif is < 0 (!dif == 0 or not found);
     *  if item == smallest item in list,
     *  dif is 0 (!dif == 1 or found);
     *  --  results erroneous
     *  if list not sorted in ascending order
     */
    if((dif = list->cmpitem(list->id, item)) < 1)
	return !dif;

    list->last(list->id);

    /*
     *  compare to largest item in list
     */
    if((dif = list->cmpitem(list->id, item)) > -1)
	return !dif;

    /*
     *  if not found at either end of list,
     *  partition the list by shifting the
     *  number of entries right one bit, then
     *  round odd numbers up after shift so
     *  we're not short of middle
     */
    gap = (list->entries >> 1) + (list->entries & 1);
    direc = left;

    while(gap)  {       /*  forever  */
	for(i=0;i<gap;i++)
	    step[direc](list->id);

	if(!(dif = list->cmpitem(list->id, item)))
	    return 1;
	else if(dif < 0)
	    direc = left;
	else
	    direc = right;

	/*
	 *  quit if not found after a couple of single steps
	 */
	if(gap == 1)
	    ++count;
	if(count == termcount)
	    break;

	gap = (gap >> 1) + (gap & 1);
    }
    return 0;
}
