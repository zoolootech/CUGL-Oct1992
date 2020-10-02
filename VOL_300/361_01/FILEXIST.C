
/* FileExist.c --> Taken from CUG-273 & Turbo-C Bible.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 13 September 91/EK
 */

#include <dir.h>
#include <stdek.h>
#include <gadgets.h>

int FileExists(char *FileName)
{
    struct ffblk fb;
        
    return !findfirst(FileName, &fb, 0);
}

