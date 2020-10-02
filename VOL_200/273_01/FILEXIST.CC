#include <dir.h>
file_exist(char *fn)
/* Check to see of a Find exists.
	RETURN 1 if exist 0 if not.
*/
{
        struct ffblk fb;
        int rc;
        rc=findfirst(fn,&fb,0);
        if(rc==0) return(1);
        return(0);
}
