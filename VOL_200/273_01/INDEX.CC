#include <stdlib.h>
char *index(char *str, char c)
/*------------------------------------------------------------------------------
INDEX - returns a pointer to the first occurance of 'c' in the string pointed
to by 'str', or NULL if 'str' does not contain 'c'.
------------------------------------------------------------------------------*/
{
 while( (*str != c) && *(str++) );
 if( *str == c )
   return( str );
 return( NULL );
}
