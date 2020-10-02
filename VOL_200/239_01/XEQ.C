
/* xeq.c  - a module for io.lib, 8/29/86, dco
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

void xeq( dir, xeqf ) char *dir, *xeqf;
{
      static char *wd;

      wd = getcwd( NULL, 51 );
      if ( xeqf != NULL )         /* is there an exec file? */
      {
         if ( strlen( xeqf ) != 0 )
         {
            if ( dir == NULL ) dir = wd;
            else if ( strlen( dir ) == 0 ) dir = wd;
            if ( chdir( dir ) == -1 )
            {
                perror("Dir Error");
            }
            else
            {
               if ( system( xeqf ) == -1 )
               {
                   perror("Xeq Error"); /* command.com not found */
               }
               /* else command.com goes to work... */
               /* ...and we return here!           */
            }
         }
      }
      chdir( wd );
      free( wd );
}

/* eof: xeq.c */

