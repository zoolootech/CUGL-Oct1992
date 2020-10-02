
/* typedefs and function declarations for DOS calls.  Created by */
/* T Clune for ERI.  Copyright (c) 1988 Eye Research Institute, */
/* 20 Staniford St., Boston, MA 02114.  All Rights Reserved. */

#ifndef DOS_FUNC_H
#define DOS_FUNC_H


typedef union    /* union to turn addresses into seg:offset */
{         /* uses unsigned long int for "generic" ptr, as types will vary */
    struct
    {
	unsigned int offset;
	unsigned int segment;
    } s;
    unsigned long pointer;
}address_union;



typedef struct  /* the disk transfer area struct for funcs 0x4E and 0x4F */
{       /* of int 0x21.  See PPS, p. 256 */
    unsigned char reserved[21];  /* used by DOS */
    unsigned char attrib; /* file attribute (see prog. prob. solver, p.263) */
    unsigned int time; /* time of file creation (PPS, p. 262) */
    unsigned int date; /* date of file creation (PPS, p.262) */
    unsigned long int size; /* number of bytes in file */
    unsigned char name[14]; /* name of file */
} dta;


    /* union to change dta time/date into string in fget_time_date() */
typedef union
{
    struct
    {
	unsigned int time;
	unsigned int date;
    }w;
    struct
    {
	unsigned secs: 5;
	unsigned mins: 6;
	unsigned hrs: 5;
	unsigned day: 5;
	unsigned mon: 4;
	unsigned yr: 7;
    }b;
}filestamp;


long disk_space();
int del_fhandle(), fname_unused(), get_dir(), count_files();


#endif

