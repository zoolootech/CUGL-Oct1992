
/*
	HEADER:		;
	TITLE:		Test stack -- test driver for stack module
			(stk.obj);

	FUNCTION:	Reads ascii text file specified on command
			line, pushing each line, then pops and prints
			each line.  In effect, displays file in
			reverse line order;

	FILENAME:	TESTSTK.C;
	REQUIRES:	LIST.OBJ, LIST.H, STK.OBJ, STK.H, DEBUG.OBJ
			DEBUG.H;
	VERSION:	1.0;
	DATE:		1/18/90;
	AUTHOR:		Michael Kelly;
*/

#include <stdlib.h>
#include <stdio.h>

#define DEBUG

#include "list.h"
#include "stk.h"
#include "debug.h"


char input_buf[BUFSIZ];
void main(int argc, char **argv)
{

    FILE *fp = NULL;

    if(argc < 2)
	err_exit("Useage: teststk ascii_file_name ");

    fp = fopen(argv[1],"r");
    if(! fp)
	err_exit("Cannot open input file");

	while(fgets(input_buf, BUFSIZ, fp))  {

	if(! push(input_buf, strlen(input_buf) + 1))
	    err_exit("\n\tError in Stack Push");
    }

    while(pop(input_buf))
	fprintf(stdout,"%s", input_buf);


}