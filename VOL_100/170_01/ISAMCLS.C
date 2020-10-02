/*
**                 ISAMC - Written by John M. Dashner
*/

#include <stdio.h>
#include <ctype.h>

#include <isam.h>

/*
**                  CLOSE - Close an ISAMC Index
*/

isamcls(hdr)
struct isam *hdr;
{
    if (hdr->q5 == 1)           /* ck if index needs to be sorted */
        if (isamsrt(hdr) == ERROR)
            return ERROR;
    else                        /* if not, update the control records */
        if (isamupd(hdr) == ERROR)
            return ERROR;
    if (close(hdr->q7) == ERROR)
    {
        isam_err = 8;
        return ERROR;
    }
    free(hdr);
    return NULL;
}

