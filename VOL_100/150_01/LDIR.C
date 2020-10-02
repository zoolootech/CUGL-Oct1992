/*
       HEADER:  150.??;
        TITLE:  LDIR - Library Directory Display Program;
      VERSION:  3.0;
         DATE:  02/29/1984;
  DESCRIPTION:  "Displays the file names and sizes (in sectors or kilobytes)
                 of the files which are contained in a library (.LBR) file.
                 Intended for use on Remote CP/M systems where the full library
                 utility (LU) is not made available for security reasons.
                 A library file is a single large file to the operating system 
                 which has been built from many smaller files.  The purpose of 
                 a library file is to save the diskette space which otherwise 
                 would be wasted, due to the allocation of disk space in units 
                 of fixed size.";
     KEYWORDS:  Library File, .LBR, Directory, Disk Space;
       SYSTEM:  MSDOS;
     FILENAME:  LDIR.C;
     WARNINGS:  "1) User documentation not included.  Program prints syntax
                    and options summary when called with no command line 
                    options.
                 2) Copyrighted.  Permission granted for noncommercial use.";
     SEE-ALSO:  Library Utility, LU, LTYPE.C;
      AUTHORS:  Gary P. Novosielski for CP/M, modified by Pete Mack for MSDOS;
    COMPILERS:  Lattice C; 
   REFERENCES:  AUTHORS: Gary P. Novosielski; TITLE: "LDIR";
                CITATION: "PC-SIG Disk 50 or 142" 
       ENDREF;
*/
/*	LDIR 	Library Directory display program */

#define	VERSION		3 
#define	REVISION	0
#define MOD_DATE	"84-02-29"

/*
3-1-84

This is a hack of the latest (Oct 83) version of ldir..
This version is intended to run under MS-dos, and has
most of the changes neccessary to pull it away from non
standard BDS-C.

It has been set up for Lattice.c and will compile with
standard ctype.h and stdio.h files.

	Pete Mack
	Simi Valley Ca.
*/
/*
Legal Notices:
	Copyright (c) 1982, 1983 by Gary P. Novosielski.
	All rights reserved.

	Permission is hereby granted for noncommercial use.
	Use or duplication of this or any derivative work for
	commercial advantage without prior written consent
	of the author is prohibited.

LIFO Revision Summary:
	3.00	84-29-84	Revised for use on IBM-PC running
				MS-DOS. Compiler = Lattice 'c'
				(Pete Mack)
	2.20	83-10-13	Changed Kb size calculation to
				round upward.  Added max drive
				validation.  Moved copyright
				display to help section.
	2.11	83-03-21	BDS 1.5 support.
				Size display in Kb.
	2.10	82-12-09	Size display in sectors.
	2.00	82-11-20	[Not released]
	1.00	82-11-14	Initial source release
		Gary P. Novosielski

Program Description:
	This program is intended for use on RCPM systems to
	allow callers to see the contents of the directories
	of .LBR files on the system.  You probably won't need
	it on your home system, since the -L function of LU.COM
	provides this ability.  Since LU is not active on
	remote systems, a program like this is necessary
	to allow you to see member names in a library without
	your having to download the library first.
*/

#include "stdio.h"
#include "ctype.h"

/* Pseudo typedef's */
#define FLAG		char

/* Number of displayed entries */
#define NWIDE	4

/* 		EXTERNALS */
FILE   *lbrfile, *fopen();			 /* fd for library file	*/
char lbrname[20];

#define FROM_START	0

FLAG lbropen;
int  entries, freeent;

/* Entry Size */
#define ESIZE		32

/* Entries per sector */
#define EPS		(SECSIZ / ESIZE)

/* Structure of a directory entry */
struct direntry
{
    char status;	/* Possible values */
#define  ACTIVE			0x00
#define  KILLED			0xFE
#define	 VIRGIN			0xFF
    char id[8+3]; 	/* filename.ext */
    unsigned indx;	/* Pointer to first sector */
    unsigned size;	/* Size of member in sectors */
    unsigned crc;	/* CRC check word */
    /* Future expansion space */
#define EXPSIZ			14
    char expand[EXPSIZ];
}
*directory, *curentry;	/* two pointers to such a beast */

typedef struct direntry dirtype;

char sopt;		/* size option: S, N, or K */
char *drmsg;		/* Max drive letter allowed */

/*		END OF EXTERNALS */
#define SECSIZ	128
#define OK	0

/************************************************
 main
*************************************************/

main (argc,argv)
unsigned  argc;
char *argv[];
{
    cprintf(
      "Library DIRectory   Ver:%d.%02d   %s\n\r%s\n\r",
      VERSION,REVISION,MOD_DATE,
      "Press CTRL-S to pause; CTRL-C to cancel"
      );


    /*
    The FIRST character of the following message is actually
    used in the test for the maximum legal drive.  This will
    allow sites which do not support a C compiler to easily
    find and patch this value in the object code.
    */
    drmsg = "P: is highest valid drive";


    /* Initialize flags */
    sopt = 'K';			/* Default option setting */
    lbropen = FALSE;		/* No library open */
    directory = NULL;		/* No directory in memory */


    if (argc < 2)		/* No command line arguments */
    {
	cputs("\n\rCopyright (c) 1982, 1983 by Gary P. Novosielski");
	cputs("\n\r\nCorrect syntax is:");
	cputs("\n\rLDIR [<option>] name1 [[<option>] [name2...[nameN]]]");
	cputs("\n\r\nWhere:\tname1 through");
	cputs("\n\r\tnameN\tare library names; default type: .LBR");
	cputs("\n\rOptions:\n\r\t-n\tonly show names of members.");
	cputs("\n\r\t-s\talso show size in sectors.");
	cputs("\n\r\t-k\tlike -s, but size in Kbytes. (default)");
	cputs("\n\rOption flags stay in effect for subsequent names.");
	cputs("\n\rAmbiguous names are not permitted.");



    }
    else
	/* Process command line arguments */
	while(--argc)
	{
	    if (**(++argv) == '-')
		procopt(*argv);
	    else if (!namel(*argv))
		dirlist();
	    else
		cprintf("\n\r%s not found on disk.\n\r",lbrname);

	}
    /* End of command line.  Clean up and exit */
}
/* End of main function */

/************************************************
 Open *name as the current library
*************************************************/

namel(name)
char *name;
{
    if (lbropen && close(lbrfile) == ERROR)
	abend("\n\rCan't close library: %s",lbrname);
    lbropen = FALSE;
    if (isambig(name) || indexc(name,' ') != ERROR)
	abend("\n\rBad library name: %s",name);
    if (name[1] == ':' && *name > *drmsg)
	abend("\n\r%s",drmsg);
    strcpy(lbrname,name);
    if (indexc(name,'.') == ERROR)
	strcat(lbrname,".LBR");
    if ((lbrfile = fopen(lbrname,"r")) != NULL)
    {
	cprintf("\n\rLibrary: %s has ",lbrname);
	readdir();
    }
    else
	return ERROR;
    lbropen = TRUE;
    cprintf ("%d entries, %d free:\n\r",entries,freeent);
    return OK;
}

/************************************************
   Return flag saying if the requested number of memory bytes
   are available.  Try to make them available if possible.
*************************************************/

FLAG avail(request)
unsigned request;	/* in bytes */
{
    char *ptr;
    unsigned  *curavail, temp;
    temp = 0;

    curavail = &temp;	/* Pseudo-static kluge */

    if(request < *curavail)
	return TRUE;
    if ((ptr = (char *) sbrk(++request - *curavail)) == ERROR)
	return FALSE;

    /* If this is the first success, initialize pointer */
    if (directory == NULL)
	directory =  (dirtype *) ptr;

    *curavail = request; /* Modify static for next call */
    return TRUE;
}

/************************************************
/* Read the directory into memory */
*************************************************/

readdir()
{
    if (!avail(SECSIZ))
	memerr();
    fseek(lbrfile, 0, FROM_START);

    if (
      fread(directory,128,1,lbrfile) != 1
      || entcmp(directory,"\0           ")
      || directory->indx
      || !directory->size
      )
	abend("no directory");

    if (directory->size > 1)
    {
	if (!avail(SECSIZ * directory->size))
	    memerr();
	if (fread(directory+EPS,128, directory->size - 1,lbrfile)
	  != directory->size - 1)
	    abend("a bad directory");
    }

    freeent = entries = EPS * directory->size;

    for(
      curentry = directory;
      curentry->status != VIRGIN && freeent;
      ++curentry
      )
	--freeent;
}

/************************************************
 memory error
*************************************************/

memerr()
{
    abend("an absurdly huge directory");
}

/************************************************
  Search string *s for character c.  Return offset 
*************************************************/

int indexc(s, c)
char *s, c;
{
    int i;
    for (i = 0; *s; i++) 
	if(*s++ == c)
	    return i;
    return ERROR;
}

/************************************************
  Return TRUE if s contains asterisk(s) or question(s)
*************************************************/

isambig(s)
char *s;
{
    if (indexc(s,'*') != ERROR || indexc(s,'?') != ERROR)
	return TRUE;
    return FALSE;
}

/************************************************
 Terminate program with error message 
*************************************************/

abend(p1, p2, p3, p4)
{
    cprintf(p1, p2, p3, p4);
    cputs("\n\r\nFor help, type LDIR alone.");
    exit();
}

/************************************************
 compare string a to string b ignoring some bits of each
*************************************************/

bitcmp(a, b, count, mask)
char *a, *b, mask;
int count;
{
    int r;
    while(count--)
	if (r = (*a++ & mask) - (*b++ & mask))
	    return r;
    return 0;
}

/************************************************
 form a string in dst from a standard format name in src
*************************************************/

formname(dst,src)
char *dst, *src;
{
    int i,j;
    j = 0;

/* Remove attributes first so compares will work */
    for (i = 1; i < 12; i++)
	src[i] &= 0x7F;
    for (i = 1; i < 9; i++)
    {
	if (src[i] == ' ')
	    break;
	dst[j++] = src[i];
    }
    if (src[9] != ' ')
	dst[j++] = '.';
    for (i = 9; i < 12; i++)
    {
	if (src[i] == ' ')
	    break;
	dst[j++] = src[i];
    }
    dst[j] = '\0';
    return dst;
}

/************************************************
   Compare two directory entries. Note that status is major
   sort field.
*************************************************/

entcmp(a,b)
char *a, *b;
{
    int  i, r;

    for (i = (1+8+3); i--; a++, b++)
	if ((r = *a - *b) && *b != '?')
	    return r;
    return 0;
}

/************************************************
   List the directory of the current library, and return number
   of free entries
*************************************************/

dirlist()
{
    char name[20];
    int  i;
    unsigned del, act;

    curentry = directory;
    for ((act = del = 0, i = entries - freeent); --i;)
    {
	if ((++curentry)->status == ACTIVE) 
	{
	    if(!(act % NWIDE))  
		cputs("\n\r");
	    formname(name, curentry);

	    switch (sopt)
	    {
	    case 'S':	/* Size in sectors */
		cprintf("%-12s%5ds ", name, curentry->size);
		break;
	    case 'K':	/* Size in Kilobytes */
		cprintf("%-12s%5dk ",name, (curentry->size+7)/8);
		break;
	    case 'N':	/* Name only. More names per line */
		cprintf("%-14s",name);
	    }
	    ++act;
	}
	else
	    ++del;
    }
    cprintf("\n\r Active entries: %u, Deleted: %u, Free: %u, Total: %u.\n\r",
      ++act, del, freeent, entries);
    return --act;
}

/************************************************
 Process option string (-xx)
*************************************************/

procopt(s)
char *s;
{

    while(*(++s))
	switch (*s)
	{
	case 'S':
	case 'N':
	case 'K':
	case 's':
	case 'n':
	case 'k':
	    sopt = toupper(*s);
	    break;
	default:
	    abend("'%c' is an invalid option",*s);
	}
}
