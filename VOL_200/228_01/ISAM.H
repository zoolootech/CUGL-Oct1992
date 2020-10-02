/*
HEADER:         CUGXXX;
TITLE:          Header file (of ISAM system);
DATE:           3-31-86;
DESCRIPTION:    Part of ISAM Library;
KEYWORDS:       ISAM;
FILENAME:       ISAM.H;
WARNINGS:       None;
AUTHORS:        John M. Dashner;
COMPILER:       Lattice C;
REFERENCES:     US-DISK 1310;
ENDREF
*/
/*
**                  ISAMC - Written by John M. Dashner
*/

/*
**                  ISAMC - Header File
*/

struct isam
{
    int q1;     /* number of records */
    int q2;     /* number of deleted records */
    int q3;     /* number of records in the sorted portion */
    int q4;     /* current record number */
    int q5;     /* 0 == sorted; 1 == unsorted */
    int q6;     /* key length */
    int q7;     /* file pointer */
};

struct rec1
{
    int ttl_recs;       /* total recs */
    int del_recs;       /* deleted recs */
};

struct rec2
{
    int srt_recs;       /* sorted portion */
    unsigned del_ptr;   /* points to delete chain */
};

struct rec3
{
    unsigned rec_ptr;   /* data record ptr */
    char  idx_key[1];   /* record key */
};

#define MINKEY 2
#define MAXKEY 254

extern int isam_err;    /* ISAMC error indicator */

