/*
**                 ISAMC - Written by John M. Dashner
*/

#include <stdio.h>
#include <ctype.h>

#include <isam.h>

/*
**                  ADD - Add an Index Record
*/

isamadd(key, rec, hdr)
char *key;
unsigned rec;
struct isam *hdr;
{
    struct rec1 *r1; struct rec2 *r2; struct rec3 *r3;
    char buf[MAXKEY + 2];
    int i, fp;
    unsigned cur_rec, chn_rec;
    int rec_len;
    long lrec;

    rec_len = hdr->q6 + 2;
    r1 = (struct rec1 *) buf;
    r2 = (struct rec2 *) buf;
    r3 = (struct rec3 *) buf;
    fp = hdr->q7;

    cur_rec = hdr->q1;          /* tag to end of file */
    hdr->q1 += 1;               /* update record count */
    lrec = cur_rec * rec_len;
    lseek(fp, lrec, 0);
    r3->rec_ptr = rec;          /* build index record */
    for(i=0;i<hdr->q6;i++)
        r3->idx_key[i] = key[i];

    if(write(fp, buf, rec_len) == ERROR)
    {
        isam_err = 8;
        return ERROR;
    }
    hdr->q4 = cur_rec;
    hdr->q5 = 1;                /* mark as unsorted */
    return NULL;
}

