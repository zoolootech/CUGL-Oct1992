/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 XDIR - Hard Disk Manager
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Apr 03, 1987
  DESCRIPTION:	 Hard Disk Manager for IBM PC
  KEYWORDS:	 Hard Disk Manager Dump Directory
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      ydir.C
  WARNINGS:	 None
  CRC:		 N/A
  SEE-ALSO:	 HDIR.DOC and XDIR.DOC
  AUTHOR:	 Mike Blakley 15645 SW 82 Cir Ln #76, Miami, Fl 33193
  COMPILERS:	 ECO-C
  REFERENCES:	 XDIR.DOC
*/


/*
   ydir.c
   functions associated with routine xdir
   to process the DOS directories
*/
#include "stdio.h"
#include "dos.h"
#include "dir.h"
#include "xdir.h"

/*
    format print for a directory entry

*/
void dirform(cp,buffer)
struct tdir *cp;     /* directory structure */
char *buffer;        /* output buffer */
{

      char temp1[100];
      char temp[12];
      char *op;      /* pointer to output */
      int  yr, mo, da, hr, mn;
      int  i;

      op = buffer;   /* init pointer to output */
      clear(temp1,100,' ');
      blockmv(temp1,cp->d_name,strlen(cp->d_name));  /* file name */
      op = temp1 + 20;
      i = (int) cp->d_attrib;
      if (i & 0x20) temp1[20] = 'a';
      if (i & 0x10) temp1[21] = 'd';
      if (i & 0x08) temp1[22] = 'l';
      if (i & 0x04) temp1[23] = 's';
      if (i & 0x02) temp1[24] = 'h';
      if (i & 0x01) temp1[25] = 'r';

      mo = (((cp->d_date)>>5) & 0xf);
      da = (cp->d_date & 0x1f);
      yr = (((cp->d_date)>>9) & 0x3f);
      yr += 80;
      hr = (((cp->d_time) >>11) & 0x1f);
      mn = (((cp->d_time) >> 5) & 0x3f);

      ltoa(temp,cp->d_size);
      i = strlen(temp);
      blockmv(temp1+34-i,temp,strlen(temp));

      prin(mo,temp);
      blockmv(temp1+35,temp,2);

      temp1[37] = '/';
      prin(da,temp);
      blockmv(temp1+38,temp,2);
      temp1[40] = '/';
      prin(yr,temp);
      blockmv(temp1+41,temp,2);

      if (hr > 12) {hr -= 12; temp1[49] = 'p';}
      else temp1[49] = 'a';

      prin(hr,temp);
      blockmv(temp1+44,temp,2);
      temp1[46] = ':';
      prin(mn,temp);
      blockmv(temp1+47,temp,2);
      blockmv(temp1+51,cp->d_path,strlen(cp->d_path)+1);
      strcpy(buffer,temp1);
      return;

}

/*
   cvdate
   convert date to unsigned format

*/
int cvdate(buffer)
char *buffer;
{
     int yr, mo, da;
     char temp[3];

     int cdate = 0;
     if (strlen(buffer) != 6) return 0;
     temp[0] = buffer[0];
     temp[1] = buffer[1];
     temp[2] = 0;
     yr = atoi(temp);
     yr -= 80;
     temp[0] = buffer[2];
     temp[1] = buffer[3];
     mo = atoi(temp);
     temp[0] = buffer[4];
     temp[1] = buffer[5];
     da = atoi(temp);
     cdate = da;
     cdate |= (mo << 5);
     cdate |= (yr << 9);
     return (cdate);
}


/*
    lis
    this function will list  a file to the printer
    updated 7/11/86 to detect printer errors
*/
void lis(filename)
char *filename;
{
    int  act,i,eval;
    char buffer[100];
    int  linecnt;       /* count of lines */
    int  stdlist;

    stdlist = open("PRN",1);     /* open for writing */

    act = 1;
    linecnt = 0;
    while (1)
    {
    i = getxline(buffer,100,filename,act);
    if (i == -1) break;
    act = 0;
    if (linecnt == 0)          /* print title at top of page */
       {
       eval = tprt();
       if (eval) {
       write(stdlist,"List of file: ",14);
       write(stdlist,filename,strlen(filename));
          }
          else {close(stdlist);return;}
       }

    eval = tprt();            /* write detail line */
    if (eval)
    {
    write(stdlist,"\n",1);
    write(stdlist,buffer,strlen(buffer));
    ++linecnt;
    }
    else {close(stdlist);return;}

    if (linecnt == 60)   /* form feed at end of logical page */
       {
       eval = tprt();
       if (eval)
       {
       write(stdlist,"\f",1);
       linecnt = 0;
       }
       else {close(stdlist);return;}
       }
    }

    eval = tprt();
    if (eval)
    write(stdlist,"\f",1);    /* last page */
    close(stdlist);

}                             /* end of function */



/*
   get1char
   get a single character

*/
get1char(cntl)
int *cntl;
{
    int c;

    c = getch();
    if (c==0)
       {
       *cntl = 1;
       c = getch();
       }
    else *cntl = 0;

    return c;

}

/*
   cvatt
   convert attributes
*/
int cvatt(buffer)
char *buffer;
{
     int  c, rc;

     rc = 0;    /* init the return code */
     while (c = toupper((int) *buffer++))
     {
     switch (c)
     {
     case 'A':    /* all */
       rc |= 0x3f;
       break;
     case 'R':    /* readonly */
       rc |= 0x01;
       break;
     case 'H':    /* hidden */
       rc |= 0x02;
       break;
     case 'S':    /* system */
       rc |= 0x04;
       break;
     case 'V':    /* volume */
       rc |= 0x08;
       break;
     case 'D':    /* directory */
       rc |= 0x10;
       break;
     case 'B':    /* archive bit */
       rc |= 0x20;
       break;
     }            /* end switch */
     }            /* end while */

     return rc;

}    /* end function */


/*
    prin
    print a date or time in printable format

*/
void prin(mo,temp)
int  mo;
char *temp;
{

     if (mo < 10)
        {
        temp[0] = '0';
        temp[1] = mo + '0';
        }
     else itoa(temp,mo);

     temp[2] = 0;

}
/*
     test printer status
     if unavailable display message and get action to take

*/
tprt()
{
    int  i;

    i = testprt();
    if (i) return (1);     /* no error in printer */

    while (1)
    {
    writestr("\nPrinter error - action to take (A)bort (R)etry ");
    i = toupper(getch());
    if (i == 'A') return (0);
    i = testprt();
    if (i) return (1);
    }

}


