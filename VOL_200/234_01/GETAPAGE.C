/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 XDIR - Hard Disk Manager
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Apr 03, 1987
  DESCRIPTION:	 Hard Disk Manager for IBM PC
  KEYWORDS:	 Hard Disk Manager Dump Directory
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      getapage.c
  WARNINGS:	 None
  CRC:		 N/A
  SEE-ALSO:	 HDIR.DOC and XDIR.DOC
  AUTHOR:	 Mike Blakley 15645 SW 82 Cir Ln #76, Miami, Fl 33193
  COMPILERS:	 ECO-C
  REFERENCES:	 XDIR.DOC
*/


/*
   getapage
   get a page at a time from a disc file
   used in the routine XDIR to display files
*/
#include "stdio.h"
/* #define DEBUG 1  */
#define MAXPAGE 1000
#define PAGESIZE 2000
/*
   typ
   type a file to the console

*/
void typ(fnam)
char *fnam;
{
     static int  i,j,k,l,len;
     static int  found,m,c;
     static int  nflag;
     static int  fd;
     static int  sflag, oldi;
     static int  currpage, lastpage;
     static int  row, col;
     static char temp[80];
     static char temp1[80];
     static char sword[80];       /* search word */
     static char xtemp[PAGESIZE]; /* page buffer */
     static char xsave[PAGESIZE]; /* page buffer */
     struct {
        long offs;        /* offset in file */
        int  plen;        /* length of page */
        } pages[MAXPAGE];
     long foff;

     k = l = len = 0;
     i = j = 1;
     foff = 0L;

     while (1)     /* build array of file info */
     {
     j = getxline(temp,80,fnam,i);
     if (j == -1) break;
     i = 0;
     len += j;    /* update length */
     ++l;         /* line counter */
     if ((l%23) == 0)
        {
        pages[k].offs = foff;
        pages[k].plen = len;
        foff += (long) len;
        len = 0;
        ++k;
        if (k >= MAXPAGE) {--k;break;}
        }
     }

     if (len > 0)
       {
       pages[k].offs = foff;    /* update last page */
       pages[k].plen = len;
       ++k;
       }
     lastpage = k;              /* note the last page */

#ifdef DEBUG
     for (foff=0L,i=0;i<k;i++)      /* list the file info */
     {
     foff += (long) pages[i].plen;
     }
     printf("\nSize of %s is %ld",fnam,foff);
     writestr("\nPress any key ... ");
     getch();

     for (i=c=0;i<k;i++)              /* list the detail */
     {
     printf("\nPage %d offset %ld length %d",(i+1),pages[i].offs,
           pages[i].plen);
     if ((i%20) == 19)
        {
        writestr("\nNext page or X=cancel ");
        c=getch();
        }
     if (c == 'X') break;
     }
#endif

     fd = open(fnam,0);
     if (fd == -1) {writestr("\nCan't open ");
                    writestr(fnam);
                    exit(0);}


     currpage = sflag = 0;              /* begin with page 0 */
     while (1)
     {
     if (sflag == 0)
     {
     top(fnam,currpage);
     foff = lseek(fd,pages[currpage].offs,0);
     j = pages[currpage].plen;
     if (j >= PAGESIZE) j = PAGESIZE;     /* prevent overflow */
     read(fd,xtemp,j);
     j -= 2;
     xtemp[j] = 0;
     writestr(xtemp);
     bot();          /* write the bottom */
     }
     sflag = 0;
     c =toupper(getch());
     if (c == 0) c=getch();

     nflag = 0;        /* no numeric input */
     switch (c)
     {
     case '\033':      /* escape */
        break;
     case 0x0d:
     case 0x0a:
        ++currpage;
        break;
     case 73:         /* page up */
        --currpage;
        break;
     case 81:         /* page down */
        ++currpage;
        break;
     case 71:         /* home */
        currpage = 0;
        break;
     case 79:         /* end */
        currpage = lastpage-1;
        break;
     case '+':
     case '-':
     case '0':
     case '1':
     case '2':
     case '3':
     case '4':
     case '5':
     case '6':
     case '7':
     case '8':
     case '9':
       putchar(c);    /* display it */
       nflag = 1;
       break;
     case 59:         /* F1 = help */
        clrscr();
        writestr("Options are as follows: \n");
        writestr("\nOption       Description");
        writestr("\nPgUp         Displays the previous page");
        writestr("\nPgDn         Displays the following page ");
        writestr("\nHome         Displays the first page ");
        writestr("\nEnd          Displays the last page ");
        writestr("\n+nnn         Page forward nnn pages from here ");
        writestr("\n-nnn         Page backward nnn pages from here ");
        writestr("\nnnn          Go to absolute page nnn ");
        writestr("\nEnter        Next page ");
        writestr("\nF1           Help  - This page ");
        writestr("\nS<string>    Search for text <string>");
        writestr("\nInvalid key  Causes beep sound");
        writestr("\n\n\nPress any key to continue");
        getch();
        break;
     case 60:    /* F2 - display page layouts */
        clrscr();
        for (m=0,row=col=1;m<lastpage;m++)
        {
        cursor(row,col);
        writestr("Pg ");
        itoa(temp1,(m+1));
        writestr(temp1);
        writestr(" offs ");
        ltoa(temp1,pages[m].offs);
        writestr(temp1);
        writestr(" len ");
        itoa(temp1,pages[m].plen);
        writestr(temp1);
        ++row;
        if (row > 23)
           {
           row = 1;
           col +=25;
           }
        }
        getch();
        break;
     case 'S':   /* search for text string */
           sflag = 1;         /* set search flag */
           break;
     default:
        putchar('\007');   /* beep */
        break;
     }          /* end case */

     if (c == '\033') break;     /* end of program */

     if (nflag)   /* numeric input */
        {
        gets(temp);      /* get numeric */
        j = atoi(temp);
        if (c == '+') currpage += j;
        else if (c == '-') currpage -= j;
        else {
             temp1[0] = c;
             temp1[1] = 0;
             strcat(temp1,temp);
             j = atoi(temp1);
             currpage = (j-1);
             }
        }                /* end if numeric */

      if (c == 'S')      /* search string */
        {
        int  c1;
        char *cpo;
        cpo = temp;
        oldi = currpage;          /* save page number */
        putchar(c);
        writestr("earch? ");

        while (1)
        {
        c1 = getch();
        if (c1 == '\033') break;
        if (c1 == '\015') break;
        putchar(c1);
        *cpo++ = c1;
        }
        *cpo = 0;

        l = strlen(temp);
        if (c1 == '\033') break;      /* escape = no search */
        if (l != 0)                   /* default previous text */
            strcpy(sword,temp);

        for (;currpage<lastpage;currpage++)  /* search from current page forward */
        {
        int  n;
        foff = lseek(fd,pages[currpage].offs,0);
        j = pages[currpage].plen;
        read(fd,xtemp,j);
        j -= 2;
        xtemp[j] = 0;

           for (m=found=0;m<j;m++)         /* search the buffer */
           {
           if ((n=strncmp(sword,xtemp+m,l)) == 0)
              {found=1;break;}
           }

           if (found)  break;             /* stop at first occurence */
         }    /* end for (;i<k) */

        if (found)
              {
              top(fnam,currpage);
              strncpy(xsave,xtemp,m);
              xsave[m] = 0;
              writestr(xsave);
              writestr("\033[7m");
              writestr(sword);
              writestr("\033[0m");
              strcpy(xsave,xtemp+m+strlen(sword));
              writestr(xsave);
              bot();
              }
        else
             {
             writestr(" Not found ");
             currpage = oldi;
             }
        }      /* end if c == 'S' */

     if (currpage >= lastpage) currpage = lastpage-1;   /* max */
     if (currpage < 1) currpage = 0;

     }       /* end while 1 */

     close(fd);


}            /* end function typ */

getxline(buffer,maxlen,fnam,action)
char *buffer;       /* output buffer */
int  maxlen;        /* maximum length */
char *fnam;         /* file name */
int  action;        /* 0 = read, 1 =open, 2 = clse */
{
       static int fd;
       static int eofind, index, priorbyte, ocount;
       static int state, temp;
       static char inbuff[258];
       static int  eobbyte, lastblk;
       static char c, *cpo;
#ifdef DEBUG
       char xtemp[20];
#endif

       if (action == 2) {close(fd); return(0);}
       if (action == 1) state = 0;
       cpo = buffer;


       while (1)
       {
           switch (state)
           {
           case 0:    /* initial state for open file */
             cpo = buffer;
             fd = open(fnam,0);
             c = eofind = index = priorbyte = ocount = 0;
             lastblk = 0;
             if (fd == -1)
                {writestr("\nCan't open ");
                writestr(fnam);
                eofind = 1; ocount = -1; state = 90;}
             else state = 1;
             break;
           case 1:    /* prepare to read a block */
             if (lastblk == 1) {eofind = 1;state = 90;}
                else state = 2;
             break;
           case 2:   /* read a block */
             index = 0;
             eobbyte = read(fd,inbuff,256);
#ifdef DEBUG
             writestr("\nRead bytes ");
             itoa(xtemp,eobbyte);
             writestr(xtemp);
#endif
             if (eobbyte <= 0) {lastblk = 1; close(fd); state = 90;}
             else
             if (eobbyte < 256) {lastblk = 1; close(fd); state = 3;}
             else state = 3;
             break;

           case 3:   /* not used */
           case 4:   /* determine if index > eobbyte */
             if (index >= eobbyte) state = 1;
                else state = 5;
             break;

           case 5:   /* transfer an output byte */
              priorbyte = (int) c;    /* save prior byte */
              c = *(inbuff + index);  /* get current byte */
              ++index;                /* update the index */
              ++ocount;               /* update the byte counter */
              if (c == 0x0d) {*cpo++ = 0;state = 4;}
              else
              if (c == 0x0a) {state = 90;}     /* exit on LF */
              else   {*cpo++ = c;state=20;}
              break;

           case 10:      /* last byte of prior block was a CR */
              c = *(inbuff+index);
              if (c == 0x0a)
                 {
                 priorbyte = (int) c;
                 ++index;
                 ++ocount;
                 }
              state = 90;
              break;

           case 20:      /* test length of string */
              if (ocount >= maxlen)
                 state = 90;
              else state = 4;
              break;

           case 80:       /* normal entry point */
              if (eofind == 1) {ocount = -1;state = 90;}
                else state = 4;
              break;

           case 90:       /* normal exit point */
              *cpo++ = 0;
              temp = ocount;
              ocount = 0;
              state = 80;
              return temp;
              break;


           }        /* end switch */
        }   /* end while */

}           /* end function */

/*
   display top of page

*/
void top(fnam,i)
char *fnam;      /* file name */
int i;           /* page */
{
     char temp[10];

     clrscr();
     writestr("Listing of file .. ");
     writestr("\033[7m");
     writestr(fnam);
     writestr("\033[0m");
     writestr("                 page ");
     itoa(temp,(i+1));
     writestr(temp);
     putchar('\n');

}

/*
  bot

*/
void bot()
{
     cursor(24,0);
     writestr("\033[7m");
     writestr("\nOptions:");
     writestr("\033[0m");
     writestr(" ESC = exit, PgUp, PgDn, Home, End, F1 = Help ... ");
     writestr("\033[0m");
}

