

/*
	HEADER:         CUG308;
	TITLE:          Generic push-down stack using
			LIST module;
	DESCRIPTION:    "Uses a List object to implement a
			generic push-down stack.";
	KEYWORDS:       stack generic;
	DATE:           5/6/90;
	VERSION:	2.01;
	FILENAME:       STACK.C;
	REQUIREMENTS:	STACK.H, DEBUG.OBJ, DEBUG.H, LIST.OBJ, LIST.H;
	USAGE:          "link stack.obj, debug.obj and list.obj with
			your main program";
	SEE-ALSO:       STACK.H, LIST.DOC, LIST.C, LIST.H, DEBUG.H;

	AUTHOR:         Michael Kelly
			254 Gold St. Boston Ma. 02127;
	COPYRIGHT:	May be used freely if authorship is acknowledged;
*/


/*
 *  STACK
 */


#define DEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "debug.h"
#include "list.h"
#include "stack.h"

static int dummy();            	/* placeholder for initlist() function  */
static void init_stack(void);
static List stack;


static void init_stack(void)
{
    if(! initlist(&stack, dummy))
	err_exit("\n\n\tError initializing Stack!\n");
}


int push(void *data, size_t datasize)
{
    /*
     *  if pointer to compare function is NULL,
     *  stack has not been initialized;
     *  if stack.id is -1, stack has been "destroyed"
     */
    if(! stack.compare  ||  stack.id == -1)
	init_stack();

    return stack.add(stack.id, data, datasize, FIRST);
}


int pop(void *databuf)
{
    if(! stack.compare  ||  stack.id == -1)
	return 0;

     return stack.remitem(stack.id, databuf);
}

int copytos(void *databuf)
{
    if(! stack.compare  ||  stack.id == -1)
	return 0;


    return stack.getitem(stack.id, databuf);
}

size_t tossize(void)
{
    if(! stack.compare  ||  stack.id == -1)
	return 0;

    return stack.getsize(stack.id);
}


/*
 *  placeholder for initlist() cmpfunc parameter
 *  no comparison function is needed since we will not sort the stack
 */
static int dummy()
{
    return 0;	/* all things being equal */
}


