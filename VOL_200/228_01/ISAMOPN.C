/*
HEADER:         CUGXXX;
TITLE:          Open ISAM index (of ISAM system);
DATE:           3-31-86;
DESCRIPTION:    Part of ISAM Library;
KEYWORDS:       ISAM;
FILENAME:       ISAMOPN.C;
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
**                  OPEN - Open an ISAM Index
*/

struct isam *isamopn(name, keylen)
char *name;
int  keylen;
{
    int i, fp;
    struct isam *hdr;
    struct rec1 *r1;
    struct rec2 *r2;
    char buf[MAXKEY + 2];

    isam_err = 0;
    r1 = (struct rec1 *) buf;
    r2 = (struct rec2 *) r1;

    if((keylen < MINKEY) || (keylen > MAXKEY))
    {
        isam_err = 4;
        return ERROR;
    }
    if((fp = open(name, 2)) == ERROR)
    {
        if((fp = creat(name, 6)) == ERROR)
        {
            isam_err = 8;
            return ERROR;
        }
        r1->ttl_recs = 2;
        r1->del_recs = 0;
        lseek(fp, 0L, 0);
        for(i=0; i<2; i++)
            if(write(fp, buf, keylen + 2) == ERROR)
            {
                isam_err = 8;
                free(hdr);
                return ERROR;
            }
        close(fp);
        hdr = isamopn(name, keylen);
        isam_err = 7;
        return(hdr);
    }
    if((hdr = (struct isam *) malloc(sizeof(struct isam))) == NULL)
    {
        isam_err = 9;
        return ERROR;
    }
    if(isam_err == 0)
    {
        lseek(fp, 0L, 0);
        if(read(fp, buf, keylen + 2) == ERROR)
        {
            isam_err = 8;
            free(hdr);
            return ERROR;
        }
        hdr->q1 = r1->ttl_recs;
        hdr->q2 = r1->del_recs;
        if(read(fp, buf, keylen + 2) == ERROR)
        {
            isam_err = 8;
            free(hdr);
            return ERROR;
        }
        hdr->q3 = r2->srt_recs;
        hdr->q4 = 2;
        hdr->q5 = 0;
        hdr->q6 = keylen;
        hdr->q7 = fp;
        return hdr;
    }
}

