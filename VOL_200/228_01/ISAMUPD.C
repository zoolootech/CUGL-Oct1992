/*
HEADER:         CUGXXX;
TITLE:          Update control record (of ISAM system);
DATE:           3-31-86;
DESCRIPTION:    Part of ISAM Library;
KEYWORDS:       ISAM;
FILENAME:       ISAMUPD.C;
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
**                  UPDATE - Control Record Update
*/

isamupd(hdr)
struct isam *hdr;
{
    char buf[MAXKEY + 2];
    struct rec1 *r1;
    struct rec2 *r2;
    r1 = (struct rec1 *) buf;
    r2 = (struct rec2 *) buf;

    lseek(hdr->q7, 0L, 0);
    r1->ttl_recs = hdr->q1;
    r1->del_recs = hdr->q2;
    if (write(hdr->q7, buf, hdr->q6 + 2))
    {
        isam_err = 8;
        return ERROR;
    }
    r2->srt_recs = hdr->q3;
    r2->del_ptr  = 0;       /* not presently used */
    if (write(hdr->q7, buf, hdr->q6 + 2))
    {
        isam_err = 8;
        return ERROR;
    }
    return NULL;
}


