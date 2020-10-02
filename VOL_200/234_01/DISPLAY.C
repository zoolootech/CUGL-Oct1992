/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 XDIR - Hard Disk Manager
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Apr 03, 1987
  DESCRIPTION:	 Hard Disk Manager for IBM PC
  KEYWORDS:	 Hard Disk Manager Dump Directory
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      display.c
  WARNINGS:	 None
  CRC:		 N/A
  SEE-ALSO:	 HDIR.DOC and XDIR.DOC
  AUTHOR:	 Mike Blakley 15645 SW 82 Cir Ln #76, Miami, Fl 33193
  COMPILERS:	 ECO-C
  REFERENCES:	 XDIR.DOC
*/


/*
  display
  display a screen and accept the answers

*/
#include "stdio.h"
#include "dir.h"
#include "xdir.h"
extern char cdir[];
extern char helpfile[];
/*
   disx
   handle display and maintenance of files
*/
void disx(text,pcount,answer,typx)
struct tdir *text[];
int  pcount;   /* number of elements */
char *answer;  /* array of answers */
int  typx;      /* 1 = dir , 2 = files */
{

     int  i,c,row,col,ct;
     struct tdir *tp;
     int  brkey;          /* break key */

     brkey = 0;
     while (1)
     {
     display(text,answer,pcount,typx);   /* process the parameters */
     for(i=ct=0;i<MAXFILE;i++)
       if (answer[i] != ' ') ++ct;
     if (ct == 0) break;
     if (typx == 1) break;  /* nothing further for DIR */

     clrscr();
     boxx(79,24,1,1);
     cursor(2,20);
     writestr("Hard Disk File Manager - Verify Options ");
     row = 4;
     col = 20;


     for (i=0;i<MAXFILE;i++)
     {

     if (row > 20)
        {
        cursor(row,col);
        writestr("Press any key for next page or 'X' to cancel ");
        c = getch();
        if (c == 'X') {brkey = 1; break;}
        clrscr();                           /* fresh screen */
        boxx(79,24,1,1);
        cursor(2,20);
        writestr("Hard Disk File Manager - Verify Options ");
        row = 4;
        }

     if (brkey) break;
     c = (int) answer[i];
     tp = text[i];
     switch (c)
       {
       case 'X':     /* delete */
       case 'L':     /* list */
       case 'T':     /* type */
       case 'R':     /* rename */
       case 'C':     /* copy */
       case 'D':     /* dump */
       case 'P':     /* protect */
       case 'H':     /* hide */
       case 'U':     /* unhide */
       case 'W':     /* unprotect */
       case 'E':     /* encrypt */
       case 'S':     /* search */
         cursor(row++,col);
         writestr("File ");
         writestr(tp->d_name);
         writestr(" will be ");
         break;
     }     /* end switch */

     switch (c)
       {
       case 'X':
         writestr("deleted ");
         break;
       case 'L':
         writestr("listed ");
         break;
       case 'T':
         writestr("typed ");
         break;
       case 'R':
         writestr("renamed ");
         break;
       case 'C':
         writestr("copied ");
         break;
       case 'D':
         writestr("dumped ");
         break;
       case 'P':
         writestr("protected");
         break;
       case 'W':
         writestr("unprotected ");
         break;
       case 'U':
         writestr("unhidden ");
         break;
       case 'H':
         writestr("hidden ");
         break;
       case 'E':
         writestr("encrypted ");
         break;
       case 'S':
         writestr("searched ");
         break;
     }     /* end switch */

     }     /* end while */

     if (brkey) break;
     cursor(row,col);
     writestr("Continue processing? (Y)es (N)o ");
     i = toupper(getch());
     if (i == 'Y') break;
     }     /* end while 1 */
}
/*
  display
  display filenames on multiple pages
*/
void display(text,answer,fcount,typx)
struct tdir *text[];
char *answer;
int  fcount;
int  typx;     /* 1 = directories , 2 = files */
{
      int i, row, col;
      int  c, cntl;
      char disp[60];
      int  pageno;       /* page number 0 - x */
      int  ff, lf;       /* first and last field number */
      struct tdir *tp;

      pageno = 0;        /* initialize page */

      while (1)          /* main loop */
      {
      if (pageno < 0) pageno = 0;
      if ((pageno*72) > fcount) pageno = 0;
      ff = pageno * 72;
      lf = ff + 71;
      if (lf >= fcount) lf = fcount-1;
      lf -= (pageno*72);

      clrscr();
      boxx(79,24,1,1);
      cursor(2,20);
      writestr("Hard Disk File Manager - Directory ");
      tp = text[0];
      if (typx == 1) writestr("**ROOT**");
          else writestr(cdir);          /* display directory */
      cursor(22,4);

      if (typx == 2)
      {
      writestr("Options: <CR> = end, F1 = Help ");
      writestr(" X = delete, (L)ist (T)ype (R)ename (S)earch");
      cursor(24,4);
      writestr("(C)opy (D)ump (H)ide, (U)nhide, (P)rotect, (W)rite (E)ncrypt ");
      }
      else writestr("Options: <CR> = end, (S)elect F1 = Help");

      cursor(23,4);
      writestr("PgUp = previous display PgDn = next");


      for (i=0;i<=lf;i++)    /* display the file names */
      {
      col = ((i/18)*20)+2;
      row = (i % 18) + 3;
      cursor(row,col);
      putchar('[');
      putchar(answer[ff+i]);
      putchar(']');
      putchar(' ');
      tp = text[ff+i];
      if (((ff+i) == 0) && (typx == 1)) strcpy(disp,"**ROOT**");
      else strcpy(disp,tp->d_name);
      if (tp->d_attrib & 0x02)  /* hidden */
         writestr("\033[7m");
      writestr(disp);
      if (tp->d_attrib & 0x02) writestr("\033[0m");
      }

      i = 0;                          /* initialize field number */
      while (1)                       /* maintain the options */
      {
      col = ((i/18)*20)+3;
      row = (i%18) +3;
      cursor(row,col);
      if ((c=getch()) == 0)
         {cntl = 1;c=getch();}
      else cntl = 0;

      switch (cntl)
      {
      case 0:                        /* a normal character was entered */
        c = toupper(c);
        if ((typx == 2) && (c == 0x0d)) break;
        if (isval(c,typx))   /* verify valid response */
          {
          answer[ff+i] = c;
          putchar(c);
          }
        if ((typx == 1) && (c == 'S')) return;
        break;
      case 1:                       /* extended character was entered */
        if (c == 0x48) --i;                             /* up arrow */
        if (c == 0x50) ++i;                             /* down arrow */
        if (c == 0x4b) i -= 18;                         /* left arrow */
        if (c == 0x4d) i += 18;                         /* right arrow */
        if (c == 0x47) i = 0;                           /* home */
        if (c == 0x49) --pageno;                        /* page up */
        if (c == 0x51) ++pageno;                        /* page down */
        if (c == 0x3b) xhelp(typx);                     /* display help */
        break;
      }                   /* end switch */

      if (c == 0x0d) break;  /* CR */
      if ((c == 0x49) || (c== 0x51)) break;      /* PgUp PgDn */
      if (c == 0x3b) break;  /* F1 */
      if (i<0) i = 0;
      if (i>=lf) i = lf;
      }   /* end while  main options */
      if (c == 0x0d) break;  /* CR */
      }   /* end while -  main loop */
}

/*
   isval
   is a valid response

*/
isval(c,typx)
int  c;
int  typx;  /* 1 = directories, 2 = files */
{

    if (typx == 2)       /* files */
    {
    switch (c)
    {
      case ' ':
      case 'X':          /* delete */
      case 'L':          /* list */
      case 'T':          /* type */
      case 'R':          /* rename */
      case 'C':          /* copy */
      case 'D':          /* dump */
      case 'P':          /* protect */
      case 'U':          /* unhide */
      case 'W':          /* write */
      case 'H':          /* hide */
      case 'E':          /* encrypt */
      case 'S':          /* search */
        return (1);
        break;
      default:
        return (0);
        break;
      }   /* end case */
    }  /* end if type is 2 - files */

   else
   {
    if ((c== ' ') || (c == 'S')) return (1);
    else return (0);
   }      /* end else */

}         /* end function */
/*
  cvn
  convert file name

*/
void cvn(ibuff,obuff)
char *ibuff, *obuff;
{
    int  i,j;
    char temp[80];

    strcpy(temp,ibuff);
    i = strlen(ibuff);

    for (j=i;j>0;j--)
    {
    if (temp[j] == '\\') break;
    }
    ++j;
    strcpy(obuff,temp+j);

}

/*
  cvp
  convert path name

*/
void cvp(ibuff,obuff)
char *ibuff, *obuff;
{
    int  i,j;
    char temp[80];

    strcpy(temp,ibuff);
    i = strlen(ibuff);

    for (j=i;j>0;j--)
    {
    if (temp[j] == '\\') break;
    }
    temp[j] = 0;
    strcpy(obuff,temp);

}


/*******

   Function to draw a box that is "wide" units wide and "deep"
   rows deep. The row-column coordinates are used for the upper-
   lefthand corner of the box.

   Attribute list:   int wide      width of the box to draw
                     int deep    number of row for depth
                     int row     place for upper-left corner
                     int column  of the box

   Return value:      none

********/


void boxx(wide, deep, row, column)
int wide;
int deep;
int row;
int column;
{
   unsigned j;

   cursor(row, column);
   putchar(218);                  /* Corner */
   linex(wide - 2, 196);                  /* dash   */
   putchar(191);

   j = 1;

   while (j < deep) {
      cursor(row + j, column);
      putchar(179);
      cursor(row + j, wide + (column - 1));
      putchar(179);
      ++j;
   }
   cursor(row + deep, column);
   putchar(192);
   linex(wide - 2, 196);
   putchar(217);
}


/********

   Function to draw a dashed line of "n" characters using the
   the character specified by "c".

   Nothing useful is returned.

*********/


void linex(n, c)
int n;               /* number of dashes to print */
char c;               /* character to use        */
{
   while (n--)
      putchar(c);
}

/*
     xhelp
     display messages and help if necessary

*/
void xhelp(typx)
int  typx;
{
         int  c;

         clrscr();
         boxx(79,24,1,1);
         cursor(2,10);
         writestr("XDIR - Extended Directory Help ");

         if (typx == 1)
            {
            cursor(3,10);
            writestr("S - select a directory for processing ");
            cursor(4,10);
            writestr("<ENTER> - no further processing ");
            cursor(5,10);
            writestr("F1 - Help (this screen)");
            }
         else
           {
           cursor(3,10);
           writestr("<ENTER> - no further processing ");
           cursor(4,10);
           writestr("X - delete a file ");
           cursor(5,10);
           writestr("L - list a file to the printer ");
           cursor(6,10);
           writestr("T - type or search a file ");
           cursor(7,10);
           writestr("R - rename a file ");
           cursor(8,10);
           writestr("C - copy a single file or a directory ");
           cursor(9,10);
           writestr("D - dump a file in hex and ascii");
           cursor(10,10);
           writestr("H - hide a file from viewing ");
           cursor(11,10);
           writestr("U - unhide a file (opposite of hide)");
           cursor(12,10);
           writestr("P - protect a file from accidental erasure ");
           cursor(13,10);
           writestr("W - enable a read-only file to be written ");
           cursor(14,10);
           writestr("E - encrypt or decrypt a file ");
           cursor(15,10);
           writestr("F1 - help (this screen)");
           cursor(16,10);
           writestr("PgDn - if screen is full see next page ");
           cursor(17,10);
           writestr("PgUp - if screen is full see prior page ");
           cursor(18,10);
           writestr("S  - search for character strings in text");
           }    /* end if typx == 1 */

           cursor(20,10);
           writestr("Press F1 if further help is needed ");
           c = getch();
           if (c == 0)
              {
              c =getch();
              if (c == 0x3b) typ(helpfile);
              }

}                          /* end subroutine */

