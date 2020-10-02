/*
HEADER:         ;
TITLE:          BASIC instr() (in string) function
VERSION:        1.4;

DESCRIPTION:    performs BASIC instr() function with similar syntax. This
                comes in handy when manually converting BASIC to C

       syntax: instr(starting_position,string_to_look_in,string_to_find)
                starting position is 0 to length of line - 1,
                this function returns 0 to length of line if string was
                located, or < -1 if not found

KEYWORDS:       ;
SYSTEM:         Xenix 3.4b, MSDOS;
FILENAME:       instr.c
WARNINGS:       compile with -dNO_PROTOTYPE if your system does not
                support prototyping, with -dFOR_MSDOS if you are compiling
                for MSDOS with an ANSI standard compiler.
                Defaults assume compiling with prototypes for
                Xenix 3.4b on Altos 2086 computer.

SEE-ALSO:       ;
AUTHORS:        Vern Martin, 449 W. Harrison, Alliance, Ohio 44601;
COMPILERS:      ECOSOFT ECO-C88, XENIX 3.4B STANDARD COMPILER;
*/

#include "vernmath.h"
#define NOT_FOUND   -1

int instr(position,search,find)
int position;
char *search,*find;
{
    int insearch,infind=0,match=0,prefind=0;
    insearch = position;
/*  check for error condition   */
    if (position > (strlen(search)-1)) {
        return(NOT_FOUND);
    }
    while(search[insearch] != (char) NULL &&
        find[infind] != (char) NULL && match == 0) {
        if (search[insearch] != find[infind]) {
            insearch++;
            continue;
        }
/*  record the position of the match of the first char  */
        position = insearch++;
/*  set preliminary find flag to true, preliminary match found */
        prefind = 1;
        infind++;
        while(search[insearch] != (char) NULL &&
            find[infind] != (char) NULL && match == 0) {
            if (search[insearch] != find[infind])  {
/*  do not increment insearch -- it might match the first char of infind */
                infind = 0;
                break;
            }
            insearch++;
            infind++;
        }
/*  if you made it to the end of the find string then you have a match */
        if (!find[infind]) match = 1;
/*  if the preliminary find flag is set and a full match has not yet
    been found then set the search posision to one past fist char match */
        if (prefind && !match) {
            prefind = 0;
            insearch = position+1;
        }
    }
    if (match) return(position);
    else return(NOT_FOUND);
}
