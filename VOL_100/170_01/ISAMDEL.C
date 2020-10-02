/*
**                 ISAMC - Written by John M. Dashner
*/

#include <stdio.h>
#include <ctype.h>

#include <isam.h>

/*
**                  DELETE - Mark Index Record Deleted
*/

isamdel(hdr)
struct isam *hdr;
{
    char buf[MAXKEY + 2];
    long lrecl;
    struct rec3 *r3;
    r3 = (struct rec3 *) buf;

    if ((hdr->q4 < 2) || (hdr->q4 >= hdr->q3))
    {
        isam_err = 2;
        return ERROR;
    }
    lrecl = hdr->q4 * (hdr->q6 + 2);
    lseek(hdr->q7, lrecl, 0);
    if (read(hdr->q7, buf, hdr->q6 + 2) == ERROR)
    {
        isam_err = 8;
        return ERROR;
    }
    r3->idx_key[0] = 255;       /* post as deleted */
    lseek(hdr->q7, lrecl, 0);
    if (write(hdr->q7, buf, hdr->q6 + 2) == ERROR)
    {
        isam_err = 8;
        return ERROR;
    }
    hdr->q2 += 1;               /* bump delete count */
    return NULL;
}

