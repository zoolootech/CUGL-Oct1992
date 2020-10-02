/*
HEADER:         CUGXXX;
TITLE:          Get previous record (of ISAM system);
DATE:           3-31-86;
DESCRIPTION:    Part of ISAM Library;
KEYWORDS:       ISAM;
FILENAME:       ISAMPRV.C;
WARNINGS:       None;
AUTHORS:        John M. Dashner;
COMPILER:       Lattice C;
REFERENCES:     US-DISK 1310;
ENDREF
*/
/*
**                 ISAMC - Written by John M. Dashner
*/

#include <stdio.h>
#include <ctype.h>

#include <isam.h>

/*
**                  GET PREVIOUS - Sequential Index Traverse
*/

isamprv(hdr)
struct isam *hdr;
{
    char buf[MAXKEY + 2];
    long lrec;
    struct rec3 *r3;
    r3 = (struct rec3 *) buf;

    hdr->q4 -= 1;       /* decrement current record */
    if (hdr->q4 == 1)
    {
        isam_err = 2;   /* boundary limit */
        return ERROR;
    }
    lrec = hdr->q4 * (hdr->q6 + 2);
    lseek(hdr->q7, lrec, 0);
    if (read(hdr->q7, buf, hdr->q6 + 2) == ERROR)
    {
        isam_err = 8;
        return ERROR;
    }
    if (r3->idx_key[0] == 255)
        return(isamprv(hdr));   /* recurse on deleted record */
    return r3->rec_ptr;
}

