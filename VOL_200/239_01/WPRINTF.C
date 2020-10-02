
#include <stdarg.h>
#include <malloc.h>
#include "ciao.h"

/* wprintf -- printf formatted string into window
**            honors all the wputs ^ attribute escape commands
**
** example:
**   wprintf("Well, now, here are  ^1three^ little ^6%c%c%c^'s!\n",2,1,2);
**
*/

void wprintf( fmt )
char *fmt;
{
     char *q;
     va_list arg_ptr;

     q = (char *) malloc(128);
     va_start( arg_ptr, fmt );
     vsprintf( q, fmt, arg_ptr );
     va_end( arg_ptr );
     wputs( q );
     free( q );
}


