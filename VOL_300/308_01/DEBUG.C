
/*
	HEADER:         CUG308;
	TITLE:		debugging error message module;
	DESCRIPTION:    "general purpose debugging error message routines";

	DATE:           5/6/90;
	VERSION:        2.01;
	FILENAME:       DEBUG.C;
	SEE-ALSO:       DEBUG.H;

	AUTHOR:         Michael Kelly
			254 Gold St. Boston Ma. 02127;
	COPYRIGHT:	May be used freely if authorship is acknowledged;

*/


#include <stdlib.h>
#include <stdio.h>


void err_out(const char *errmsg)
{
    fprintf(stderr,"\n%s\n",errmsg);
    exit(1);
}

void bug_out(const char *errmsg, const char *filename, int lineno)
{
    fprintf(stderr,"\n%s\n",errmsg);
    fprintf(stderr,"\tFile: %s\n",filename);
    fprintf(stderr,"\tLine: %d\n",lineno);
    exit(1);
}
