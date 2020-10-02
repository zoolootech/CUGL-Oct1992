/****************************************************************************/
/* error.h                                                                  */
/* this header is required for error.c                                      */
/*  (c) by Ronald Michaels. This program may be freely copied, modified,    */
/*  transmitted, or used for any non-commercial purpose.                    */
/****************************************************************************/

void error           (int,int);  /* prototype for error function */
#define WARN  1
#define FATAL 2

#define NO_OF_ERRORS (sizeof(err_mgs)/sizeof(char *))


