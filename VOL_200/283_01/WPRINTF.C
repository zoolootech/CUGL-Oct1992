
#include <stdarg.h>
#include "ciao.h"

/* wprintf -- printf formatted string into window
**            honors all the wputs ^ attribute escape commands
**
** example:
**   wprintf("Well, now, here are  ^1three^ little ^6%c%c%c^'s!\n",2,1,2);
**
*/

void wprintf( char *fmt, ... )
{
     char *q;
     va_list arg_ptr;

     if ( (q = (char *) malloc(256)) == NULL )
        return;

     va_start( arg_ptr, fmt );
     vsprintf( q, fmt, arg_ptr );
     va_end( arg_ptr );
     wputs( q );
     free( q );
}
