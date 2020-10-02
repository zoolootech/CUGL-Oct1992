/*
**                 ISAMC - Written by John M. Dashner
*/

#include <stdio.h>
#include <ctype.h>

#include <isam.h>

/*
**                  GET - Retrieve Index Record by Key
*/

static int generic;        /* generic search flag */
static char buf[MAXKEY + 2];

unsigned isamget(hdr, key)
struct isam *hdr;
char *key;
{
    int rc;
    unsigned lo, hi, cur_rec;
    char wrk[MAXKEY + 1];

    if (strlen(key) == 0)
    {
        cur_rec = 2;
        return(_isget(hdr, cur_rec));
    }
    if((strlen(key) == 1) && (*key == '*'))
    {
        cur_rec = hdr->q3 - 1;
        return(_isget(hdr, cur_rec));
    }
    if (strlen(key) < hdr->q6)
        generic = TRUE;             /* set generic search */
    else
        generic = FALSE;

    setmem(wrk, MAXKEY + 1, 0);     /* set wrk space to low-values */
    strcpy(wrk, key);
    lo = 2;                         /* set up search variables */
    hi = hdr->q3 - 1;
    cur_rec = (lo + hi) / 2;
    for(;;)
    {
        if (lo > hi)
        {
            isam_err = 1;
            return ERROR;
        }
        if(_isget(hdr, cur_rec) == ERROR)
            return ERROR;
        if (buf[2] == 255)          /* ck for a deleted rec */
        {
            cur_rec--;              /* look dn one */
            if (cur_rec >= lo)      /* ck bounds */
                continue;
            else
            {
                lo = (lo + hi) / 2 + 1;
                cur_rec = (lo + hi) / 2;
                continue;
            }
        }
        else
        {
            if (generic)
            {
                if (strcmp(wrk, &buf[2]) > 0)
                    return(_srgen(hdr, wrk, hi));
                hi = cur_rec - 1;
                cur_rec = (lo + hi) / 2;
                continue;
            }
            if ((rc = strcmp(wrk, &buf[2])) == 0)
                return(_srfrst(hdr, wrk));
            if (rc > 0)
            {
                lo = cur_rec + 1;
                cur_rec = (lo + hi) / 2;
                continue;
            }
            else
            {
                hi = cur_rec - 1;
                cur_rec = (lo + hi) / 2;
                continue;
            }
        }
    }
}

static _isget(hdr, rec)        /* utility get routine */
struct isam *hdr;
int rec;
{
    long lrec;

    hdr->q4 = rec;
    lrec = rec * (hdr->q6 + 2);
    lseek(hdr->q7, lrec, 0);
    if (read(hdr->q7, buf, hdr->q6 + 2) == ERROR)
    {
        isam_err = 8;
        return ERROR;
    }
    return NULL;
}

static _srgen(hdr, okey, key, h)       /* finish up generic search */
struct isam *hdr;
char *okey, *key;
unsigned h;
{
    int i;
    unsigned l, rec;
    struct rec3 *r3;

    l = hdr->q4 + 1;             /* set low bound at current rec + 1 */
    rec = (l + h) / 2;
    if (l > h)
    {
        isam_err = 1;       /* no rec which is >= to key */
        return ERROR;
    }
    if (_isget(hdr, rec) == ERROR)
        return ERROR;
    if (strcmp(key, &buf[2]) > 0)
        return(_srgen(hdr, key, h));    /* recursively decrease span */
    for(rec = l;rec <= h;rec++)          /* slide up to it */
    {
        if (_isget(hdr, rec) == ERROR)
            return ERROR;
        if (strcmp(key, &buf[2]) <= 0)
        {
            isam_err = 0;
            r3 = (struct rec3 *) buf;
            for(i=0; i<hdr->q6; i++)     /* return actual key found */
                okey[i] = r3->idx_key[i];
            return r3->rec_ptr;
        }
    }
    isam_err = 1;       /* didn't make it */
    return ERROR;
}

static _srfrst(hdr, key)        /* slide down to first occurence */
struct isam *hdr;
char *key;
{
    struct rec3 *r3;
    unsigned rec;
    r3 = (struct rec3 *) buf;

    for(rec = hdr->q4 - 1;rec > 1; rec--)
    {
        if (_isget(hdr, rec) == ERROR)
            return ERROR;
        if (strcmp(key, &r3->idx_key[0]) != 0)
        {
            if(_isget(hdr, rec + 1) == ERROR)
                return ERROR;
            isam_err = 0;
            return r3->rec_ptr;
        }
    }
    isam_err = 0;
    return r3->rec_ptr;
}

