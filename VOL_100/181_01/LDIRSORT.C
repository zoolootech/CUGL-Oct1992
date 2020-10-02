 /*	LDIR 	Library Directory display program */
#define	VERSION		1 
#define	REVISION	00
#define MOD_DATE	"82-11-14"

/*	Copyright (c) 1982 by Gary P. Novosielski.
	All rights reserved.

	Permission is hereby granted for noncommercial use.
	Duplication or use of this or any derivative work for
	commercial advantage without prior written consent of
	the author is prohibited.

LIFO Revision Summary:

        ?.??    83-09-17        
Modified as LDIRSORT.COM.  Will optionally generate 'NAMES.SUB'
file for use by CAT2.COM.  Will not handle LBR names exceeding
seven chars in length.  This to allow for the '-' required by the
PDS catalog system (i.e. CAT1.COM).  I must consider this program a
kludge, but it works.  I left intact most of Gary N.'s code.  Most
changes are additions to his source.  Control structure is unchanged
except for the exit() if the user tries to catalog an LBR whose name
is eight chars long.  
  Also modified program to produce sorted report.  Filenames are
arranged vertically, as in UDIR.COM, instead of horizontally, as in
SD.COM.  With this change, both the file output and the screen
output use the new function FORMNAM2(), instead of the original
function, FORMNAME().
  Also, I did not modify the original programs ability to handle
multiple command-line arguments.  It still lists the internal files
foò aó manù fileó aó iô diä previously, unless user asks for more
than one NAMES.SUB files.  Thus¬ iæ thå prograí ió run thus:
                 A> ldirc file1 - file2 file3<cr>
then the NAMES.SUB file will have the catalog information only for 
FILE2.LBR.  This keeps garbage out of your MAST.CAT file.

                Joe S. Vogler
		3622 Highland Lakes Drive
		Kingwood, Texas  77339
		(713) 358-7586 (evenings)

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

Syntax:
		LDIR <name1> [<name2>...[<nameN>]]
	where <name1> etc. are library names of the form:
		[<drive>:]<lbrname>[.<ext>]
==defaults==>	 Current	     LBR
		 drive

	Ambiguous names are not permitted.  Typing LDIR with
	no arguments presents a help message.
*/

#include "bdscio.h"

/* Pseudo typedef's */
#define FLAG		char
#define FD		int

/* Number of displayed entries per line: */
#define NWIDE		4

/* EXTERNALS */
FD   lbrfile;			 /* fd for library file	*/
char lbrname[20];
int outbuf;                       /* JSV */
int catflag;             /* add to names.sub while TRUE */
char flabel[20];        /* for copy of lbrname[20] */

int strcmp();		/*	JSV		*/
#define MAXLINES	300
char *ftab[MAXLINES];	/*			*/
int fcount;		/*    for sorting	*/
char *allocp;		/*			*/

#define FROM_START	0
#define AS_INPUT	0

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
			/* Future expansion space */
#define EXPSIZ			16
    char expand[EXPSIZ];
}
*directory, *curentry;	/* two pointers to such a beast */

/*
		END OF EXTERNALS
*/

main(argc,argv)
int  argc;
char **argv;
{
    int i;		/* JSV: temporary counter	*/
    printf(
      "Library DIRectory   Ver:%d.%02d   %s\n%s\n",
      VERSION,REVISION,MOD_DATE,
      "Copyright (c) 1982  Gary P. Novosielski");
    puts("Modified 09/17/83 for sorted output ");
    puts("and for optional NAMES.SUB file");
    puts("\n   by Joe S. Vogler  83/09/17 ");
    puts("\n\nPress CTRL-S to pause; CTRL-C to cancel\n");
    if (bdos(12)%256 < 0x20) 
	abend("\nRequires CP/M 2.0 or better");

    /* Initialize flags */
    lbropen = FALSE;
    directory = NULL;
    catflag = FALSE;                            /* JSV */

    if (argc < 2)
    {
	puts("\n\nCorrect syntax is:");
	puts("\n\n\tLDIR <name1> [<name2>]...");
	puts("\n\nwhere <name1>, <name2>, etc.");Š	puts(" are library names.");
	puts("\nFiletype .LBR is assumed if not specified.");
	puts("\nAmbiguous names are not allowed.");
	puts("\n\n ** NEW:  An option to create NAMES.SUB file for ONE");
	puts("\nLBR arg.  Just preceed desired LBR arg with a 'DASH' ");
	puts("\nplus a SPACE...as in:  A> ldirsort <lbr1> - <lbr2><CR>");
	puts("\n(NAMES.SUB file will be created for <lbr2>).");
	puts("\n		Joe S. Vogler  713-358-7586, evenings\n");
    }
    else
    {
	/* Process command line arguments */
	while(--argc)
	{
	    if (argv[1][0] == '-')		{
		if (catflag)	{
		    exit(puts("\nERROR:  No more than one NAMES.SUB files!"));
		}
		fcreat("names.sub",outbuf=sbrk(BUFSIZ));
		catflag = TRUE;
		argv++;
		continue;
	    }


	    if (!namel(*(++argv)))  {
		dirlist();
            }
	    else
		printf("\n%s not found on disk.\n",lbrname);

	}
    }
    /* End of command line.  Clean up and exit */

    exit();
}
/* End of main function */


namel(name)
/* Open *name as the current library */
char *name;
{
    if (lbropen && close(lbrfile) == ERROR)
	abend("\nCan't close library: %s",lbrname);
    lbropen = FALSE;
    if (isambig(name) || indexc(name,' ') != ERROR)
	abend("\nBad library name: %s",name);
    strcpy(lbrname,name);
    if (indexc(name,'.') == ERROR)
	strcat(lbrname,".LBR");
    if ((lbrfile = open(lbrname,AS_INPUT)) != ERROR)
    {
	printf("\nLibrary: %s has ",lbrname);
	readdir();
    }
    else
	return ERROR;
    lbropen = TRUE;
    printf ("%d entries, %d free:",entries,freeent);
    return OK;
}


FLAG avail(request)
/* Return flag saying if the requested number of memory bytes
   are available.  Try to make them available if possible.
*/
unsigned request;	/* in bytes */
{Š    char *ptr;
    unsigned  *curavail;
    curavail = "\0\0";	/* Pseudo-static kluge */

    if(request < *curavail)
	return TRUE;
    if ((ptr = sbrk(++request - *curavail)) == ERROR)
	return FALSE;

    /* If this is the first success, initialize pointer */
    if (directory == NULL)
	directory =  ptr;

    *curavail = request; /* Modify static for next call */
    return TRUE;
}


readdir()
/* Read the directory into memory */
{
    if (!avail(SECSIZ))
	memerr();
    rseek(lbrfile, 0, FROM_START);

    if (
      rread(lbrfile, directory, 1) != 1
      || entcmp(directory,"\0           ")
      || directory->indx
      || !directory->size
       )
	abend("no directory");

    if (directory->size > 1)
    {
	if (!avail(SECSIZ * directory->size))
	    memerr();
	if (rread(lbrfile, directory+EPS, directory->size - 1)
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



memerr()Š{
    abend("run out of memory");
}


int indexc(s, c)
/* Search string *s for character c.  Return offset */
char *s, c;
{
    int i;
    for (i = 0; *s; i++) 
	if(*s++ == c)
	    return i;
    return ERROR;
}


abend(p1, p2, p3, p4)
/* Terminate program with error message */
{
    printf(p1, p2, p3, p4);
    puts("\n\nFor help, type LDIR alone.");
    exit();
}

bitcmp(a, b, count, mask)
/* compare string a to string b ignoring some bits of each */
char *a, *b, mask;
int count;
{
    int r;
    while(count--)
	if (r = (*a++ & mask) - (*b++ & mask))
	    return r;
    return 0;
}



dirlist()
/* List the directory of the current library, and return number
   of free entries
*/
{
    char name[20], linbuf[20];
    int  row, col, nrow, index, pcount, i;
    unsigned del, act;

    fcount = 0;		/* JSV				*/
    curentry = directory;
    if (catflag)  {                          /* JSV */
    	strcpy(flabel,lbrname);
        if( *(flabel+1) == ':' ) { strcpy(flabel,flabel+2); }
        if( *(flabel+8) == '.' ) {
        	toolong(lbrname); 
                unlink(outbuf);
                exit();
	}
    }
    for ((act = del = 0, i = entries - freeent); --i;)
    {Š	if ((++curentry)->status == ACTIVE) 
	{
bufin:	    						/* JSV	*/
	    strcpy3(linbuf, curentry);
	    if (( allocp = sbrk( strlen( linbuf) + 1)) == NULL )
		exit(puts("out of allocation space\n"));
	    ftab[fcount++] = allocp;
	    strcpy( allocp, linbuf);		/* JSV */

	    ++act;
	}
	else
	    ++del;
    }
bufsort:
	    qsort(ftab, fcount, 2, &strcmp);

bufcat:
    if (catflag)			/* JSV: only catalog the first LBR */
    {
	    fprintf(outbuf,"-%s\n",flabel);

	    for (i = 0; i < fcount; i++)	{
		formnam2( linbuf, ftab[i]);
		strcat( linbuf, "\n");
		fputs( linbuf, outbuf);
	    }
	    putc(CPMEOF, outbuf);
	    jclose(outbuf);
    }
bufout:
    nrow = ( fcount % NWIDE ? fcount/NWIDE + 1 : fcount/NWIDE );
    for ( row = 0, pcount = 0 ; row < nrow ; row++)
    {
	    for ( col = 0; col < NWIDE; col++, pcount++ )
	    {
			putchar( pcount % NWIDE ? '\t' : '\n');	
			index = row + (nrow * col);
			if ( index < fcount )	{
				printf("%-8s", formnam2(linbuf, ftab[index]));
			}
	    }
    }
    printf("\n Active entries: %u, Deleted: %u, Free: %u, Total: %u.\n",
      ++act, del, freeent, entries);
    return --act;
}


/*
formname(dst,src)
/* form a string in dst from a standard format name in src */
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
    }Š    if (src[9] != ' ')
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
*/


formnam2(dst,src)	/* JSV: Version for qsort table entries */
/* form a string in dst from a standard format name in src */
char *dst, *src;
{
    int i,j;
    j = 0;

/* Remove attributes first so compares will work */
    for (i = 0; i < 11; i++)
	src[i] &= 0x7F;
    for (i = 0; i < 8; i++)
    {
	if (src[i] == ' ')
	    break;
	dst[j++] = src[i];
    }Š/*
  if (src[8] != ' ')
*/
	dst[j++] = '.';
    for (i = 8; i < 11; i++)
    {
	if (src[i] == ' ')
	    break;
	dst[j++] = src[i];
    }
    dst[j] = '\0';
    return dst;
}


int isambig(name)
/* Return TRUE if name is ambiguous */
char *name;
{
    char c;

    while (c = *name++)
	if (c == '*' || c == '?') 
	    return TRUE;
    return FALSE;
}

entcmp(a,b)
/* Compare two directory entries. Note that status is major
   sort field.
*/
char *a, *b;
{
    int  i, r;

    for (i = (1+8+3); i--; a++, b++)
	if ((r = *a - *b) && *b != '?')
	    return r;
    return 0;
}



int strcmp(s,t)
char **s, **t;
{
	char *s1, *t1;
	s1 = *s;
	t1 = *t;
	int i;
	i = 0;
	while (s1[i] == t1[i])
		if (s1[i++] == '\0')
			return 0;
	return s1[i] - t1[i];
}



toolong(s)
int *(s);
{
   printf("\n\nERROR!!");
   printf("\n\n  Cannot catalog %s, because...",s);
   printf("\n  Library NAME cannot exceed 7 characters");
   printf("\n\n Rename LBR file and rerun.");
}

jclose(buf)
int *buf;
{	fflush(buf)» 
	fclose(buf);
	puts("\n>>>> File NAMES.SUB created on DEFAULT drive <<<<");
}

strcpy3( dst, src,)
char *dst, *src;
{
	int i, j;
	src++;
	for ( i = 0; i < 11; i++)
		*dst++ = *src++;
	*dst = *src = NULL;
}
/*
*/