/*
HEADER:         CUGXXX;
TITLE:          Sort ISAM index (of ISAM system);
DATE:           3-31-86;
DESCRIPTION:    Part of ISAM Library;
KEYWORDS:       ISAM;
FILENAME:       ISAMSRT.C;
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
**                  SORT - Sort ISAMC Index (Reorganize)
*/

#define MAXSRT 2000     /* practical work size */

struct srt_wrk
{
    unsigned ptr;       /* data record pointer */
    char  key[1];       /* beginning of key */
};

struct srt_ptr
{
    struct srt_wrk *w[1];   /* symbolic definition of ptr array */
};

isamsrt(hdr)
struct isam *hdr;
{
    struct srt_ptr *s1;
    struct srt_wrk *s2, *temp;
    unsigned recs, n, off_set, beg, end;
    int passes, pass, run, run_limit, i, r, r_sz, n1, n2, n3, n4, n5;
    long lrec;

    if(hdr->q5 == 0) return NULL;    /* don't reorg if we don't need to */

    off_set = 0;    /* init run offset */
    n = MAXSRT;     /* init sort work size */
    recs = hdr->q1 - 1;
    r_sz = hdr->q6 + 2;

    /* request a fair sized sort work area and accept something smaller */

    while(n && (s2 = (struct srt_wrk *) calloc(n, r_sz)) == NULL)
        n = n * 9 / 10; /* reduce by approx 10% */

    if ((n < (recs / 50)) /* if sort work is less than 2% then give up */
       || n == 0)
    {
        if (s2) free(s2);
        isam_err = 9;
        return ERROR;
    }
    /* now get a pointer array */
    if((s1 = (struct srt_ptr *) malloc(n * sizeof(unsigned))) == NULL)
    {
        free(s2);
        isam_err = 9;
        return ERROR;
    }
    /* build array pointers */
    for(i = 0; i < n; i++)
        s1->w[i] = s2 + i * r_sz;

    if (recs > n)        /* calculate number of passes */
        passes = ((recs * 2) - 1) / (n + 1);
    else
        passes = 1;

    for (pass = 0; pass < passes; pass++)   /* MAIN Sort Loop */
    {
        run_limit = (recs - off_set) / 
                     n - (((recs - off_set) % n) > (n / 2)) - 1;
        if (run_limit < 0)
            run_limit = 0;

        for(run = 0; run <= run_limit; run++) /* SORT Run Loop */
        {
            beg = (run * n) + off_set + 1;   /* set start for this run */
            end = beg + n - 1;               /* and end point */
            if (end > recs)                  /* don't fall off the end */
                end = recs;
            if (end < (beg + 1))    /* make sure we have something to do */
                continue;
            for(i=0, r = beg; r <= end; i++, r++) /* Read a RUN Loop */
            {
                lrec = (r + 1) * r_sz;    /* calc byte offset in file */
                lseek(hdr->q7, lrec, 0);
                if (read(hdr->q7, s1->w[i], r_sz) == ERROR)
                {
                    free(s1);
                    free(s2);
                    isam_err = 8;
                    return ERROR;
                }
            }
            n1 = end - beg + 1;         /* set up the sort */
            n2 = n1 / 2;
            while (n2)                  /* SORT Loop */
            {
                n3 = n2 + 1;
l1:             n4 = n3 - n2;
l2:             n5 = n4 + n2;
                if (strncmp(&s1->w[n4]->key[0],
                            &s1->w[n5]->key[0], hdr->q6) <= 0)
                    goto l3;
                else
                {
                    temp = s1->w[n4];
                    s1->w[n4] = s1->w[n5];
                    s1->w[n5] = temp;
                }
                n4 -= n2;
                if (n4 > 0) goto l2;
l3:             n3++;
                if (n3 <= n1) goto l1;
                n2 /= 2;
            }
            for(i=0, r = beg; r <= end; i++, r++) /* Write a RUN Loop */
            {
                lrec = (r + 1) * r_sz;    /* calc byte offset in file */
                lseek(hdr->q7, lrec, 0);
                if (write(hdr->q7, s1->w[i], r_sz) == ERROR)
                {
                    free(s1);
                    free(s2);
                    isam_err = 8;
                    return ERROR;
                }
            }
        }
        if (off_set)
            off_set = 0;
        else
            off_set = n / 2;
    }
    hdr->q5 = 0;                    /* now it's sorted */
    hdr->q3 = hdr->q1 - hdr->q2;    /* update number in sorted portion */
    return(isamupd(hdr));           /* update control rec & return */
}

