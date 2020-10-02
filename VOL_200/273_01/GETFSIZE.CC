#include <dir.h>
long getfsize(char *fn)
/* Return the filesize of a file.
   char *fn is a pointer to the filespec.

   return = -1 if file is not found.
   else returned will be the size of the file.
*/
{
        struct ffblk fb;
        int rc;
        rc=findfirst(fn,&fb,0);
        if(rc==0) return(fb.ff_fsize);
        return(-1);
}
