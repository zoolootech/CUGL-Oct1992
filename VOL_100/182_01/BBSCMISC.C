/*
    bbscmisc.c

    Mike Kelly

    06/12/83 v1.0   written
    07/07/83 v1.0   updated
*/

#include "bbscdef.h"
#include <sys/types.h>
#include <sys/stat.h>

#define LASTDATE  " 07/07/83 "

#define PGMNAME "BBSCMISC "
#define VERSION " 1.0 "


strfill(buf,fillchar,length)    /* fill a string with fillchar */
char    *buf;           /*  for length -1 */
int fillchar,
    length;
    {
    while(--length)     /* really is length -1 */
        {
        *buf++ = fillchar;
        }
    *buf++ = '\0';      /* need room for this */
    }

substr(from,to,start,length)    /* moves chars from "from" to "to" */
char    *from, *to ;        /*  starting at "start" for */
                    /*  "length" number of chars */
int start, length ;     /* for beginning of string use 1, not 0 */
    {
    int cnt;

    cnt = 0;

    while(--start)      /* adjust sending field pointer */
        {
        from++;     
        }

    while((cnt < length) && (*to++ = *from++))  /* do the moving */
        {
        cnt++;      
        }
    
    *to = '\0';

    }

char *itoa(str,n)       /* taken from float.c */
char *str;
    {
    char *sptr;
    sprintf(str,"%d",n) ;
    return(str) ;
    }
/*  end of function     */

seek(fildes,posit,dummy) int fildes,posit,dummy ;
    {
    return(lseek(fildes,posit << 7,0)) ;
    }
/*  end of function     */
    
/*  end of program      */