
/* count_files() added 11/89 by T Clune for R Webb.  count_files() */
/* accepts a filespec template and returns the number of matching files */

/* fget_time_date() added 10/89 by T Clune, to construct a string */
/* that identifies a file's time and date of creation.  Added by */
/* T Clune for F Van de Velde */

/* get_dir() modified to accept pointers instead of character arrays */
/* for the returned names.  Now get_dir() is compatible with menu() in */
/* menu.c, and dir_menu() in that file has been eliminated.  Changed */
/* 12/88 by T Clune */

/* del_fines() and parse_filename() deleted 12/88 by T Clune.  Use */
/* del_fhandle() for all file deletions */

/* dos_func.c was created 11/88 by T Clune to incorporate the various */
/* DOS software interrupt functions that were spread over a variety of */
/* applications into one file.  Copyright (c) 1988, Eye Research Institute */
/* All Rights Reserved. */


#include "dos_func.h" /* this file's typedefs and declarations */

#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <malloc.h>

    /* support functions for del_fhandle() and get_dir() */
static unsigned int get_first_filename();
static unsigned int get_next_filename();
static unsigned long get_dta();
static void set_dta();




/* count_files() looks in the directory in filespec for the file(s) specified */
/* by filespec (same format as DOS dir command for filespec).  The function */
/* returns the number of matching files found.  */
/* added 11/89 by T Clune */

int count_files(filespec)
char filespec[];
{
    int i=0;    /* the number of matching files counter */
    unsigned int cflag; /* the carry flag returned by get_first_filename() */
		    /* and get_next_filename() function calls */
    unsigned long int fnameptr;  /* flespec cast as an unsigned long int */
    dta dir_struct;  /* the file dta */
    unsigned long psp_dta;  /* the dta pointer on entry to get_dir() */


    psp_dta=get_dta();
    set_dta((unsigned long)&dir_struct);

	/* get first filename */
    fnameptr=(unsigned long int)filespec;
    cflag=get_first_filename(fnameptr);

    while(!cflag)
    {
	cflag=get_next_filename(fnameptr);
	i++;    /* increment match-count */
    }

	/* restore original dta before exiting */
    set_dta(psp_dta);

    return i;  /* return number of matches found */

}






/* del_fhandle() was modified 12/88 by T. Clune to support wildcards. */
/* del_fhandle() deletes all files specified by FILESPEC.  The advantage */
/* of del_fhandle() over del_files() is that you can specify a path instead */
/* of having to use the default path. Returns 0 if successful, 1 if not. */
/* See Programmers Problem Solver, p. 272-275 for details of function 0x41. */

int del_fhandle(filespec)
char filespec[];
{
     /* the first block of declarations is for getting the filenames */
    dta dir_struct;  /* the file dta */
    unsigned long psp_dta;  /* the dta pointer on entry to get_dir() */
    unsigned long int fnameptr; /* filespec cast as a number */
    unsigned int cflag; /* the carry flag returned by get_first_filename() */
	    /* and get_next_filename() */
    char *ptr;
    char pathname[40];
    char fullname[80];

	/* this block of declarations is for doing the deletions */
    address_union address;
    union REGS inregs, outregs;
    struct SREGS segregs;

	/* get path name of filespec */
    strcpy(pathname, filespec);
    ptr=strrchr(pathname, '\\');
    if(!ptr)
	ptr=strrchr(pathname,':');
    if(ptr)
    {
	ptr++;
	*ptr ='\0';
    }
    else
	pathname[0]='\0';

    psp_dta=get_dta();
    set_dta((unsigned long)&dir_struct);

	/* get first filename */
    fnameptr=(unsigned long int)filespec;
    cflag=get_first_filename(fnameptr);

	/* set up the delete-file software interrupt */
    address.pointer=(unsigned long int)fullname;
    segregs.ds=address.s.segment;
    inregs.x.dx=address.s.offset;
    inregs.h.ah=0x41;   /* the erase filespec function in DOS */

    while(!cflag)
    {
	    /* construct name of matching file (including path) to delete */
	strcpy(fullname, pathname);
	strcat(fullname, dir_struct.name);
	intdosx(&inregs, &outregs, &segregs);  /* delete the file */
	if(outregs.x.cflag) /* abort on failure to delete */
	    break;
	cflag=get_next_filename(fnameptr);
    }

	/* restore original dta before exiting */
    set_dta(psp_dta);

    return outregs.x.cflag;

}






/* disk_space() returns the number of bytes available on a disk */
/* you specify the disk in the call (0 = default drive, 1=A, etc.) */
/* and it returns a LONG INT with the size of the available space */
/* any DOS programming guide will explain the routine, but two that */
/* did a good job are _Programmer's Guide to MS DOS_ by Dennis Jump */
/* p. 95-96 and my old stand-by _Programmer's Problem Solver_ by Robert */
/* Jourdain, p.247. */

long disk_space(drive)
int drive;  /* drive choice. 0=default, 1=A, 2=B, etc. */
{
	long bytes_free;    /* return value in bytes for disk specified */
	union REGS inregs, outregs;     /* as per dos.h */

	inregs.h.ah = 0x36;    /* function call specifies get disk space */
	inregs.h.dl = drive;   /* which drive? */

	intdos(&inregs, &outregs); /* call the interrupt */

			/* calculate bytes free */
	bytes_free = (long) outregs.x.ax*outregs.x.bx*outregs.x.cx;

	return bytes_free;

}




/* fget_time_date() gets the time and date of creation of the file FILESPEC */
/* and writes the data as a string into TIMEDATE, in the format; */
/* MM-DD-YY HH:MM.SS  where MM is month, DD is day, YY is year, HH is HOUR, and */
/* MM is minutes, and SS is seconds.  The TIMEDATE string length is */
/* <= 18 chars, including terminating null. Added 10/89 by T Clune. */

void fget_time_date(filespec,timedate)
char filespec[], timedate[];
{
    dta dir_struct;  /* the file dta */
    address_union address;
    unsigned long psp_dta; /* the dta on entry to fname_unused() */
    union REGS regs;
    struct SREGS segregs;
    filestamp td;
    int i;
    char string[10];

    if(fname_unused(filespec))
    {
	strcpy(timedate, "00-00-00 0:00.00");
	return;
    }

    psp_dta=get_dta();
    set_dta((unsigned long)&dir_struct);

	/* get first filename */
    address.pointer=(unsigned long int)&filespec[0];
    segregs.ds=address.s.segment;
    regs.x.dx=address.s.offset;
    regs.x.cx=0;  /* no funny attributes in file */
    regs.h.ah=0x4E;   /* first file match function number of int 0x21 */
    intdosx(&regs, &regs, &segregs);
	/* the 0x21 function 0x57 is the "official" time/date interrupt */
	/* function, but anything that gets a file using the file handle */
	/* method will initialize the dta for you. */

	/* restore original dta before exiting */
    set_dta(psp_dta);
    td.w.time=dir_struct.time;
    td.w.date=dir_struct.date;
    i=td.b.mon;
    itoa(i,string,10);
    strcpy(timedate, string);
    strcat(timedate,"-");
    i=td.b.day;
    itoa(i,string,10);
    strcat(timedate, string);
    strcat(timedate,"-");
    i=td.b.yr;
    i +=80;
    itoa(i,string,10);
    strcat(timedate, string);
    strcat(timedate," ");
    i=td.b.hrs;
    itoa(i,string,10);
    strcat(timedate, string);
    strcat(timedate,":");
    i=td.b.mins;
    if(i<10)
	strcat(timedate,"0");
    itoa(i,string,10);
    strcat(timedate, string);
    strcat(timedate,".");
    i=td.b.secs;
    if(i<10)
	strcat(timedate,"0");
    itoa(i,string,10);
    strcat(timedate, string);

}






/* fname_unused() checks the directory in filespec for the file(s) specified */
/* by filespec to see whether the name has been used already.   Returns 0 if */
/* name is already used, 1 otherwise */

int fname_unused(filespec)
char filespec[];
{
    dta dir_struct;  /* the file dta */
    address_union address;
    unsigned long psp_dta; /* the dta on entry to fname_unused() */
    union REGS inregs, outregs;
    struct SREGS segregs;

    psp_dta=get_dta();
    set_dta((unsigned long)&dir_struct);

	/* get first filename */
    address.pointer=(unsigned long int)&filespec[0];
    segregs.ds=address.s.segment;
    inregs.x.dx=address.s.offset;
    inregs.x.cx=0;  /* no funny attributes in file */
    inregs.h.ah=0x4E;   /* first file match function number of int 0x21 */
    intdosx(&inregs, &outregs, &segregs);

	/* restore original dta before exiting */
    set_dta(psp_dta);

    return outregs.x.cflag;

}






/* get_dir() looks in the directory in filespec for the file(s) specified */
/* by filespec (same format as DOS dir command for filespec)  The matching */
/* files are written into fnames.  The function returns the number of */
/* matching files found, or MAX_FILES+1 if too many matches for the array  */

int get_dir(filespec,fnames,max_files)
char filespec[], *fnames[];
int max_files;  /* the size of fnames array */
{
    int i=0;    /* the number of matching files counter */
    int name_size;  /* strlen(filename) */
    unsigned int cflag; /* the carry flag returned by get_first_filename() */
		    /* and get_next_filename() function calls */
    unsigned long int fnameptr;  /* flespec cast as an unsigned long int */
    dta dir_struct;  /* the file dta */
    unsigned long psp_dta;  /* the dta pointer on entry to get_dir() */


    psp_dta=get_dta();
    set_dta((unsigned long)&dir_struct);

	/* get first filename */
    fnameptr=(unsigned long int)filespec;
    cflag=get_first_filename(fnameptr);

    while((!cflag) && i<max_files)
    {
	name_size=strlen(dir_struct.name);
	fnames[i]=malloc(name_size+1);
	if(fnames[i]==NULL)
	{
	    printf("Error allocating file names buffer space.  Program aborting.\n");
	    exit(-1);
	}
	strcpy(fnames[i], dir_struct.name); /* write matching filename into */
				    /* the fnames array */
	cflag=get_next_filename(fnameptr);
	i++;    /* increment match-count */
    }
    if((i == max_files) && (!cflag))
	i++;

	/* restore original dta before exiting */
    set_dta(psp_dta);

    return i;  /* return number of matches found */

}






/* get_dta() returns the address of the current dta, as an unsigned long int */

static unsigned long get_dta()
{
    address_union address;
    union REGS inregs, outregs;
    struct SREGS segregs;

    inregs.h.ah=0x2f;
    intdosx(&inregs, &outregs, &segregs);

    address.s.segment=segregs.es;
    address.s.offset=outregs.x.bx;

    return address.pointer;

}








/* get_first_filename() gets the first match of the filespec and returns 0 */
/* if a match was found, non-zero otherwise.  The name is in the current dta */

static unsigned int get_first_filename(fnameptr)
unsigned long int fnameptr;
{
    address_union address;  /* address conversion union */
    union REGS inregs, outregs;
    struct SREGS segregs;


	/* get first filename */
    address.pointer=fnameptr;
    segregs.ds=address.s.segment;
    inregs.x.dx=address.s.offset;
    inregs.x.cx=0;  /* no funny attributes in file */
    inregs.h.ah=0x4E;   /* first file match function number of int 0x21 */
    intdosx(&inregs, &outregs, &segregs);

    return outregs.x.cflag;

}


/* get_next_filename() gets a match after the get_first_filename() call, */
/* and returns 0 if a match was found, nonzero otherwise.  The name is in the */
/* current dta */

static unsigned int get_next_filename(fnameptr)
unsigned long int fnameptr;
{
    address_union address;  /* address conversion union */
    union REGS inregs, outregs;
    struct SREGS segregs;


	/* get first filename */
    address.pointer=fnameptr;
    segregs.ds=address.s.segment;
    inregs.x.dx=address.s.offset;
    inregs.x.cx=0;  /* no funny attributes in file */
    inregs.h.ah=0x4F;  /* set function number for continuing search */
    intdosx(&inregs, &outregs, &segregs);

    return outregs.x.cflag;

}





/* set_dta() sets the address for the start of the new dta */

static void set_dta(pointer)
unsigned long pointer;
{
    address_union address;
    union REGS inregs, outregs;
    struct SREGS segregs;

	    /* set up the dta */
    address.pointer=pointer;
    segregs.ds=address.s.segment;
    inregs.x.dx=address.s.offset;
    inregs.h.ah=0x1A;   /* the function that tells DOS that POINTER is a dta */
    intdosx(&inregs, &outregs, &segregs);

}



