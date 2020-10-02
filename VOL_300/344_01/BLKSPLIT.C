
/*
 *      HEADER:         ;
 *      TITLE:          blksplit() Text splitting function;
 *      DESCRIPTION:    "Can be used to split a block into lines
 *                      or a line into fields";
 *      DATE:           10/21/1989;
 *      VERSION:        1.0;
 *      FILENAME:       BLKSPLIT.C;
 *      SEE-ALSO:       PB.C, PB.DOC;
 *      AUTHORS:        Michael Kelly;
 */



/*
 *  int blksplit(
 *	    char *blk, char **strptr, const char *strsep, int maxstrings
 *	);
 *
 *  blk-> NULL terminated text data block
 *  strptr-> array of char *
 *  strsep-> NULL terminated array of string seperator characters
 *  maxstrings = maximum number of char * that will fit in strptr
 *
 *  splits raw text data block into strings : returns # of strings
 *
 */


#include <string.h>
#include <stddef.h>
#include "blksplit.h"

int blksplit(char *blk, char **strptr, const char *strsep, int maxstrings)
{
    register int i = 0;


    --maxstrings;

    strptr[i] = strtok(blk,strsep);

    while(strptr[i] && i < maxstrings)
	strptr[++i] = strtok(NULL,strsep);

    return i;
}

