/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 XDIR - Hard Disk Manager
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Apr 03, 1987
  DESCRIPTION:	 Hard Disk Manager for IBM PC
  KEYWORDS:	 Hard Disk Manager Dump Directory
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      util.c
  WARNINGS:	 None
  CRC:		 N/A
  SEE-ALSO:	 HDIR.DOC and XDIR.DOC
  AUTHOR:	 Mike Blakley 15645 SW 82 Cir Ln #76, Miami, Fl 33193
  COMPILERS:	 ECO-C
  REFERENCES:	 XDIR.DOC
*/


/*
   utility routines for xdir
   xrename   - rename a file
   xcopy     - copy files
   xdump     - dump a file  in hex and ascii
   dline     - build a dump line in hex and ascii
   upmod     - change file attributes
   getpath   - get path name
*/
#include "stdio.h"
#include "dir.h"
#include "xdir.h"
/*
    xrename
    rename a file

*/
void xrename(old,new)
char *old, *new;
{
    int  i;
    i = rename(old,new);
    if (i != 0)
       {
       writestr("\nCouldn't rename ");
       writestr(old);
       writestr(" to ");
       writestr(new);
       writestr(" - press enter ");
       getch();
       }

}

/*
    xcopy
    copy a file

*/
#define BUFFSIZE 128
void xcopy(fromf,tof)
char *fromf, *tof;
{
     int fdi;
     int fdo;
     static int  i;
     static char buffer[BUFFSIZE];
     static int numbytes;

     fdo = creat(tof,0);
     if (fdo == EOF)
        {
        writestr("\nCan't open output file ");
        writestr(tof);
        writestr(" - press any key");
        getch();
        return;
        }

     fdi = open(fromf,0);
     if (fdi == EOF)
        {
        writestr("\nCan't open input file ");
        writestr(fromf);
        writestr(" - press any key");
        getch();
        unlink(tof);
        return;
        }

      while ((numbytes = read(fdi,buffer,BUFFSIZE)) != EOF)
      {


      if (numbytes < 0)
         {
         writestr("\nRead error occurred - ");
         writestr(fromf);
         writestr("press any key ");
         getch();
         unlink(tof);
         return;
         }

       if (numbytes == 0) break;
       i = write(fdo,buffer,numbytes);
       if (i <= 0)
          {
          writestr("\nError writing output file ");
          writestr(tof);
          writestr(" -press any key");
          getch();
          unlink(tof);
          return;
          }

       if (numbytes < BUFFSIZE) break;
       }    /* end while */

       i = close(fdi);
       if (i == EOF)
          {
          writestr("\nCouldn't close input ");
          writestr(fromf);
          writestr(" press any key ");
          getch();
          }

       i = close(fdo);
       if (i == EOF)
          {
          writestr("\nCouldn't close output ");
          writestr(tof);
          writestr(" press any key ");
          getch();
          }

}        /* end function */

/*
    xdump
    dump a file

*/
#define DUMPSIZE 256
void xdump(fnam)
char *fnam;
{
     int    fdi;                   /* file descriptor */
     static int  i,j, k, c;
     static char buffer[DUMPSIZE];
     static int numbytes;
     char   pline[79];
     long   offset;                /* byte offset of record displayed */
     static int currpage, lastpage;
     long   foff, lseek();
     char   temp[20];

     currpage = 0;
     offset = 0L;
     fdi = open(fnam,0);
     if (fdi == EOF)
        {
        writestr("\nCan't open input file ");
        writestr(fnam);
        writestr(" - press any key");
        getch();
        return;
        }
      foff =  lseek(fdi,0L,2);                /* search end of file */
      foff /= DUMPSIZE;
      lastpage = (int) foff;                  /* determine last page */

      while (1)
      {
      foff = (long) currpage;                /* current page */
      foff *= (long) DUMPSIZE;
      offset = lseek(fdi,foff,0);              /* do a seek to position */


      numbytes = read(fdi,buffer,DUMPSIZE);


      if (numbytes < 0)
         {
         writestr("\nRead error occurred - ");
         writestr(fnam);
         writestr("press any key ");
         getch();
         return;
         }


       k = DUMPSIZE - numbytes;     /* indicate eof fill pattern */
       for (j=0;j<k;j++) buffer[numbytes+j] = 0x1a;

       clrscr();
       writestr("\n\033[1mDump of file \033[0m");
       writestr("\033[7m");
       writestr(fnam);
       writestr("\033[0m");
       writestr("\n\n\n\n");

       for (i=0;i<16;i++)
       {
       j = i * 16;
       dline(buffer+j,pline,offset+(long)j);   /* build dump line */
       putchar('\n');
       writestr(pline);
       }

       writestr("\n\n\n\033[7mEnd of page -\033[0m");
       writestr("\033[1mESC\033[0m = Exit PgUp PgDn Home End +n -n F1=Help ");

       c = toupper(getch());
       if (c == 0) c= getch();     /* pickup up special keys */

       if (c == '\033') break;
       else
       if (c == 73)    /* PgUp */ --currpage;
       else
       if (c == 81)    /* PgDn */ ++currpage;
       else
       if (c == 13)    /* CR   */ ++currpage;
       else
       if (c == 79)    /* End  */ currpage = lastpage;
       else
       if (c == 71)    /* Home */ currpage = 0;
       else
       if (c == '+')
          {
          putchar(c);
          gets(temp);
          currpage += atoi(temp);
          }
       else
       if (c == '-')
          {
          putchar(c);
          gets(temp);
          currpage -= atoi(temp);
          }
       else
       if (c == 59)     /* Help */
          {
          clrscr();
          writestr(" Hex/Ascii Dump - Help Screen ");
          writestr("\n\n\nKey                Function");
          writestr("\n_____________________________\n");
          writestr("\nPgUp               Previous Page");
          writestr("\nPgDn               Next Page    ");
          writestr("\nHome               Restart at beginning ");
          writestr("\nEnd                Go to End    ");
          writestr("\nF1                 Help - This screen ");
          writestr("\n+nnn               Forward  nnn 256 byte blocks ");
          writestr("\n-nnn               Backward nnn 256 byte blocks ");
          writestr("\n<CR>               Next Page ");
          writestr("\n\n\nPress any key to continue ");
          getch();
          }

       if (currpage < 1) currpage = 0;
       if (currpage > lastpage) currpage = lastpage;

       }    /* end while */

       i = close(fdi);
       if (i == EOF)
          {
          writestr("\nCouldn't close input ");
          writestr(fnam);
          writestr(" press any key ");
          getch();
          }

}        /* end function */


/*
   dline
   build a dump line from 16 bytes

*/
void dline(ibuff,xbuff,offset)
char *ibuff, *xbuff;
long  offset;
{
     int  i,j,k,c;
     char lbuff[80], rbuff[20], obuff[10];
     char *cpl, *cpr;


     clear(lbuff,80,' ');
     clear(rbuff,20,' ');
     cpl = lbuff;
     cpr = rbuff;

     j = k = (int) offset;
     j &= 0xf000;
     j >>= 12;
     j &= 0x0f;
     if (j < 10) obuff[0] = j + '0';
        else     obuff[0] = j + 'a' - 10;
     j = k & 0x0f00;
     j >>= 8;
     if (j < 10) obuff[1] = j + '0';
        else     obuff[1] = j + 'a' - 10;
     j = k & 0x00f0;
     j >>= 4;
     if (j < 10) obuff[2] = j + '0';
        else     obuff[2] = j + 'a' - 10;
     j = k & 0x000f;
     if (j < 10) obuff[3] = j + '0';
        else     obuff[3] = j + 'a' - 10;
     obuff[4] = 0;

     for (i=0;i<16;i++)
     {
     c = (int) *ibuff++;   /* get character */
     if ((c >= 0x20) && (c <= 0x7f))
        *cpr++ = c;
        else *cpr++ = '.';

     j = (c & 0xf0);
     j >>= 4;
     if (j < 10) *cpl++ = j + '0';
        else *cpl++ = j + 'a' - 10;

     j = (c & 0x0f);
     if (j < 10) *cpl++ = j + '0';
        else *cpl++ = j + 'a' - 10;
     *cpl++ = ' ';
     }
     *cpl = *cpr = 0;

     strcpy(xbuff,obuff);
     strcat(xbuff,"  ");
     strcat(xbuff,lbuff);
     strcat(xbuff,"    ");
     strcat(xbuff,rbuff);

}    /* end of dline */


/*
  upmod
  modify file attribute byte

*/
void upmod(fnam,atstr)
char *fnam, *atstr;
{
   int  i,att,c,watt;
   static DIR *dir;
   struct direct *dir_entry;
   char *cp;
   int  flag;
   char pathname[80];
   char workname[80];
   int  path;
   int  unhide;       /* determine if unhide is required */

   cp = atstr;        /* decode the string   */
   flag = watt = unhide = 0;   /* initialize settings */

   while (c=toupper(*cp++))    /* decode desired attributes */
   {
   switch (c)
   {
   case 'X':   /* reset */
     flag = 1;
     break;
   case 'U':   /* unhide */
     unhide = 1;
     break;
   case 'R':   /* read only */
     watt |= 0x01;
     break;
   case 'H':   /* hidden    */
     watt |= 0x02;
     break;
   case 'S':   /* system    */
     watt |= 0x04;
     break;
   case 'L':   /* label     */
     watt |= 0x08;
     break;
   case 'D':   /* directory */
     watt |= 0x10;
     break;
   case 'A':   /* archive   */
     watt |= 0x20;
     break;
   }          /* end switch */
   }          /* end while */

   path = getpath(fnam,pathname);    /* determine if wild cards */

   dir = opendir(fnam);
   setdirat(dir,0x3f);     /* search attributes are all */
   while (1)
   {
   dir_entry = readdir(dir);    /* read the directory */
   if (dir_entry == NULL) break;
   writestr("\nProcessing ");
   writestr(dir_entry->d_name);
   att = (int) dir_entry->d_attrib;
   if ((att == 0x08) || (att == 0x10)) continue;
   if (flag)  att = watt;
      else att |= watt;
   if (unhide) att &= 0x3d;      /* reset the "hide" bit */

   if (path)
      {
      strcpy(workname,pathname);
      strcat(workname,dir_entry->d_name);
      }
   else strcpy(workname,fnam);

   i = chmod(workname,att);
   }

   closedir(dir);    /* free up the memory */

}

/*
  getpath
  determine if a filename contains wild cards
    and if so, extract the path name

*/
getpath(fnam,pnam)
char *fnam;      /* filename */
char *pnam;      /* pathname */
{
    int  i,j,found,c;

    char *cp, *cpi;
    char work[80];


    strcpy(work,fnam);

    cp = work;
    found = 0;
    while (c = (int) *cp++)
    {
    if ((c == '?') || (c=='*')) {found = 1;break;}
    }

    if (found)  ;
    else {*pnam = 0;return 0;}

    i = strlen(work);
    cpi = work + i;
    --cpi;

    for (j=0;j<i;j++)
    {
    if ((c = (int) *cpi--) == '\\') {++cpi;*cpi=0;break;}
    }

    strcat(work,"\\");
    strcpy(pnam,work);
    return (1);

}


