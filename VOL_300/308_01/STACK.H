
/*
	HEADER:         CUG308;
	TITLE:          Generic push-down stack header file;
	DESCRIPTION:    "header file for generic push-down
			stack module.";
	DATE:           5/6/90;
	VERSION:        2.01;
	FILENAME:       STACK.H;
	REQUIREMENTS:	STACK.C, DEBUG.OBJ, DEBUG.H, LIST.OBJ, LIST.H;
	SEE-ALSO:       STACK.C, LIST.DOC, LIST.C, LIST.H, DEBUG.H;
	USAGE:          "link stack.obj, debug.obj and list.obj with
			your main program";

	AUTHOR:         Michael Kelly
			254 Gold St. Boston Ma. 02127;
	COPYRIGHT:	May be used freely if authorship is acknowledged;
*/


#if !defined(STACK_H)
#define STACK_H

int push(void *data, size_t datasize);
int pop(void *databuf);
int copytos(void *databuf);	/* copy item on stack top without popping */
size_t tossize(void);           /* returns size of item on stack top     */

#endif
