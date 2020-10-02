/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 XDIR - Hard Disk Manager
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Apr 03, 1987
  DESCRIPTION:	 Hard Disk Manager for IBM PC
  KEYWORDS:	 Hard Disk Manager Dump Directory
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      ctlr.C
  WARNINGS:	 None
  CRC:		 N/A
  SEE-ALSO:	 HDIR.DOC and XDIR.DOC
  AUTHOR:	 Mike Blakley 15645 SW 82 Cir Ln #76, Miami, Fl 33193
  COMPILERS:	 ECO-C
  REFERENCES:	 XDIR.DOC
*/


/*
   ctlr
   display main menu screen for word search
   process request
*/
#include "stdio.h"
#include "dos.h"
#include "dir.h"
/*
    xscr.h
    header file for scan function of XDIR

*/
/*  xscan functions */

int xscan(char *fnam, char string[10][80],int ans[],char *options);
int sbuff(char *buff,char *string, int max, char *options);
int strxcmp(char *buff1, char *buff2, int maxlen, char *options);
int strxchr(char *buff, char c, int max);

/* xword functions */
void xword(char *buffer,char words[10][80]);

/* xscr functions */
int ctlr(char *fnam);
int xscr(char *fnam, char *xnam, char *stext,
         int *sflag, int *stype, char *ext, char *opts);
int getitem(int *item, int row, int col, char *text, int len);
void sepname(char *all, char *path, char *file);
void cline(int row, int col, int len);

/* sdir functions */

int sdir(char *buff, char *filename, int action, char ext[10][80],int type);
int cmpext(char *fnam, char *ext);

/*********** end of header definition *************************/

/*
main()
{
      ctlr("\\bin\\xdir.exe");

}
*/
/*
   ctlr
   controller module

*/
ctlr(fnam)
char *fnam;
{
     int  i,j, k, sflag, stype;
     char xnam[80], stext[80], extents[80];
     int  ans[10];               /* answer for search */
     char words[10][80];         /* words to search */
     char filename[20];          /* work area for file name */
     char extnames[10][80];      /* extent names */
     char options[80];           /* search options */
     struct {
       char snam[40];            /* search file name */
       int  sopts[10];           /* search hits */
       } sfiles[100];            /* store max of 100 files */
     int scount;

     i = xscr(fnam,xnam,stext,&sflag,&stype,extents,options);


     if (i == -1)
        {writestr("\nSearch aborted ");
        return(-1);
        }

     cursor(2,50);
     writestr("Begin search ... ");
     cursor(5,10);
     writestr("SEARCHING ");
     cursor(7,10);
     writestr("Located ");
     scount = 0;

     for (i=0;i<10;i++) words[i][0] = 0;   /* init */
     xword(stext,words);          /* xlate search text to words */

     if (stype == 0)       /* search a single file */
        {
        i = xscan(xnam,words,ans,options);
        if (i != 1)
            {
            writestr("\nXscan failure ");
            getch();
            exit(0);          /* scan failure */
            }
        for (i=j=0;i<10;i++)
           if (ans[i] == 1) ++j;      /* determine any hits */

        if (j)
           {
           writestr("\nSearch words found in file ");
           writestr(xnam);
           for (i=0;i<10;i++)
             if (ans[i]==1) {putchar('\n');writestr(words[i]);}
           }
        else
           {
           writestr("\nNo match found in file ");
           writestr(xnam);
           }
         }    /* end if stype  = 0 */

       else                    /* search a directory */
        {
           for (i=0;i<10;i++) extnames[i][0] = 0;   /* init */
           xword(extents,extnames);          /* xlate search extents to words */

          i = sdir(xnam,filename,1,extnames,sflag);   /* open the directory */
          while (i = sdir(xnam,filename,0,extnames,sflag))
           {
           cursor(5,23);
           for (i=0;i<20;i++) putchar(' ');
           cursor(5,23);
           writestr(filename);
           j = xscan(filename,words,ans,options);   /* search the file */
           if (j != 1)
              {
              writestr("\nXscan failure");
              getch();
              exit(0);
              }

           for (i=j=0;i<10;i++) if (ans[i] == 1) ++j;      /* determine any hits */

           if (j)
             {
             cursor(7,23);
             for (i=0;i<20;i++) putchar(' ');
             cursor(7,23);
             if (scount < 100)
                {
                strcpy(sfiles[scount].snam,filename);
                for (i=0;i<10;i++) sfiles[scount].sopts[i] = ans[i];
                ++scount;
                }
             for (i=0;i<10;i++)
               if (ans[i]==1) {putchar(' ');writestr(words[i]);}
             }
          }        /* end while */
         }    /* end if stype  != 0 */

        /*************** print out the results of search *************/
        clrscr();
        writestr("\nResults of search located text as follows: ");
        for (i=k=0;((i<10)&&(words[i][0] != 0));i++) ++k;

        for (i=0;i<scount;i++)
        {
        writestr("\nFile ");
        writestr(sfiles[i].snam);
        for (j=0;j<k;j++)
           if (sfiles[i].sopts[j] == 1) {putchar(' ');writestr(words[j]);}
        }  /* end of dump loop */
        writestr("\nPress any key to continue ");   /* pause screen */
        getch();

}

/*
   xscan
   scan a file for text strings
   to be included with xdir upon completion of module

*/
#define BUFFSIZE 8000

int  xscan(fnam,string,ans,options)
char *fnam;
char string[10][80]; /* search strings */
int  ans[];          /* search results */
char *options;       /* search options */
{
     int  fd, flen,i,j,k,l,m;
     char *alloc(int amt);
     char *buff;
     /* char temp[20]; */

     fd = open(fnam,0);
     if (fd == EOF)
        {
        writestr("\nCan't open file ");
        writestr(fnam);
        return(-1);
        }

     buff = alloc(BUFFSIZE+1);
     if (buff == NULL) return (-1);

     for (i=0;string[i][0] != 0;i++) ans[i] = 0;  /* init to false */
     k = i;           /* count of search strings */

     flen = BUFFSIZE;

     while (flen == BUFFSIZE)
     {
       for (j=l=0;l<k;l++) j += ans[l];   /* determine if search s/b ended
                                          because all strings located */
       if (j == k) break;                 /* yes, all found */
       flen = read(fd,buff,BUFFSIZE);

       for (i=0;string[i][0] != 0;i++)
       {
       if (ans[i] == 0)    /* only test where no match yet */
          {
          m = xcomp(buff,string[i],options,flen);   /* search buffer */
          if (m == 0) ans[i] = 1;     /* record answer */
          }
       }
     }

     free(buff);           /* free the buffer */
     close(fd);            /* close the file */
     return (1);
}


/*
    xword
    parse a string into words

*/

void xword(buffer,words)
char *buffer;   /* buffer to parse */
char words[10][80];
{

      int  i, j, state;
      char *cp, *cpo;
      int  c;
      int  num;            /* used in octal numbers */
      char temp[80];
      int  tflag;          /* flag - current words in tic marks */

      for (i=0;i<10;i++) words[i][0] = 0;

      state = i = j = tflag = 0;
      cp = buffer;
      cpo = temp;

      while (1)
      {
      c = (int) *cp++;      /* get a character */


          switch (state)
          {
          case 0:           /* begin a new word */
            tflag = 0;
            cpo = temp;
            if (c == 0) state = 98;
            else if (c == ' ') state = 0;     /* skip leading spaces */
            else if (c == '\'') {tflag = 1;state = 1;}
            else {*cpo++ = c;state = 1;}
            break;

          case 1:           /* start a new word tick mark */
            if (c == '\\') state = 2;
            else if ((c == '\'') && tflag)
                {*cpo = 0;state = 0;strcpy(words[i++],temp);}
            else if ((c == ' ') && (tflag == 0))
                {*cpo = 0;state = 0;strcpy(words[i++],temp);}

            else if (c == 0) state=98;

            else {*cpo++ = c;state = 1;}
            break;

          case 2:
            num = 0;     /* set octal constant to zero, if any */
            if (c == 0) state=98;
            else
            if (c == 'n') {*cpo++ = '\n';state =1;}
            else
            if (c == 'b') {*cpo++ = '\b';state =1;}
            else
            if (c == '\''){*cpo++ = '\'';state = 1;}
            else
            if (c == '\\') {*cpo++ = '\\';state = 1;}
            else
            if (c == 'r') {*cpo++ = '\r';state =1;}
            else
            if (c == 'f') {*cpo++ = '\f';state =1;}
            else
            if (c == '\"'){*cpo++ = '\"';state = 1;}
            else
            if ((c >= '0') && (c <= '7')) {num = c - '0';state = 5;}
            else          {*cpo++ = c; state=1;}
            break;


          case 5:        /* octal constant */
            if (c == 0) {*cpo++ = num; state = 98;}
            else
            if ((c >= '0') && (c <= '7'))
               {
               num *= 8;
               num += c - '0';
               state = 6;
               }
            else {*cpo++ = num;*cpo++=c;state = 1;}
            break;

         case 6:        /* octal constant */
            if (c == 0) {*cpo++ = num; state = 98;}
            else
            if ((c >= '0') && (c <= '7'))
               {
               num *= 8;
               num += c - '0';
               *cpo++ = num;
               state = 1;
               }
            else {*cpo++ = num;*cpo++=c;state = 1;}
            break;

          case 98:      /* exit point */
            *cpo = 0;
            strcpy(words[i],temp);
            state = 99;
            break;
          }             /* end case */

        if (state == 99) break;

      }              /* end while */


}            /* end subroutine */

/*
   xscr
   display main menu screen for word search
   process request
*/

xscr(fnam,xnam,stext,sflag,stype,extent,opts)
char *fnam;      /* input file name */
char *xnam;      /* output dir or file */
char *stext;     /* search text */
int  *sflag;     /* skip flag 0 = include, 1 = omit default 1 */
int  *stype;     /* search type 0=File, 1=Dir */
char *extent;    /* file extents to include/omit */
char *opts;
{
   char dir[20];
   char file[20];
   static char text[80]  = "   ";
   char skipcom[5];         /* skip com files Omit Include  */
   char dirsrch[5];         /* search directory File Directory */
   char ok[5];              /* items ok */
   int  item,i;
   static char extents[80] = "exe com";    /* extents to skip/include */
   static char options[80] = "u";          /* ignore uppercase */
   char work[80];            /* work area */

   dirsrch[0] = 'F';      /* default is file */
   skipcom[0] = 'O';      /* default is omit */
   clrscr();
   cursor(2,10);
   writestr("XDIR - Text Search Aide ");
   sepname(fnam,dir,file);   /* separate name into path and file */
   cursor(5,10);
   writestr("Directory   [                          ]");
   cursor(5,23);
   writestr(dir);
   cursor(7,10);
   writestr("File        [                          ]");
   cursor(7,23);
   writestr(file);
   cursor(9,10);
   writestr("Search text [                                 ]");
   cursor(9,23);
   writestr(text);
   cursor(11,10);
   writestr("File extents[                                 ]");
   cursor(11,23);
   writestr(extents);
   cursor(13,10);
   writestr("Options     [                                 ]");
   cursor(13,23);
   writestr(options);
   cursor(15,10);
   writestr("Include/omit I/O    [O]");
   cursor(17,10);
   writestr("File/Directory  D/F [F]");
   cursor(19,10);
   writestr("Above items OK? Y/N [N]");
   cursor(20,10);
   writestr("Options: Arrow keys, PgUp, PgDn, Home, End, Esc, F1 = Help");

   ok[0] = 'N';           /* initial value is no */
   item = 2;
   while (1)
   {
       switch (item)
       {
       case 0:   /* directory selection */
         i=getitem(&item,5,23,dir,25);
         break;

       case 1:   /* file selection */
         i=getitem(&item,7,23,file,15);
         break;

       case 2:   /* search text */
         i=getitem(&item,9,23,text,33);
         break;

       case 3:  /* file extents */
         i = getitem(&item,11,23,extents,33);
         break;

       case 4:  /* search options */
         i = getitem(&item,13,23,options,33);
         break;

       case 5:  /* include/omit */
         i=getitem(&item,15,31,skipcom,1);
         break;

       case 6: /* dirsrch */
         i=getitem(&item,17,31,dirsrch,1);
         break;

       case 7:   /* OK  */
         i=getitem(&item,19,31,ok,1);
         break;
       }        /* end switch */

       if (ok[0] == 'Y') break;
       if (i == -1) return(-1);  /* escape key was pressed */
     }          /* end while */

       if (dirsrch[0] == 'F')
          {
          *stype = 0;
          strcpy(work,dir);      /* build up a fully qualified name */
          strcat(work,"\\");
          strcat(work,file);
          strcpy(xnam,work);
          }
       else
          {
          *stype = 1;
          strcpy(xnam,dir);
          }

       if (skipcom[0] == 'O')
          *sflag = 1;
          else *sflag = 0;

       strcpy(stext,text);
       strcpy(extent,extents);
       strcpy(opts,options);
       return (0);
}               /* end function */


/*
   getitem
   get an item

*/
#define LASTITEM 7
getitem(item,row,col,text,len)
int *item;
int row, col;
char *text;
int  len;         /* display length */
{
     int thisitem;
     char temp[80];
     int  c;

     thisitem = *item;     /* get contents */

         cursor(row,col);
         c = getch();
         if (c == 0)
            {
            c = getch();
            if (c==80)      ++thisitem;    /* up arrow */
            else if (c==72) --thisitem;    /* down arrow */
            else if (c==71) thisitem = 0;  /* Home */
            else if (c==73) thisitem = 0;  /* PgUp */
            else if (c==79) thisitem = LASTITEM;  /* End */
            else if (c==81) thisitem = LASTITEM;  /* PgDn */
            else if (c==59) ;              /* F1 - Help */
            else putchar(7);
            }
         else if (c==13) ++thisitem;
         else if (c==27) return(-1);
         else if (thisitem < 5)
            {
            temp[0] = c;
            cline(row,col,len);     /* clear the line to spaces */
            cursor(row,col);        /* reset the cursor */
            putchar(c);
            gets(temp+1);
            strcpy(text,temp);
            ++thisitem;
            }
         else
            {
            c = toupper(c);
            switch (thisitem)
            {
            case 5:
              if ((c == 'I') || (c == 'O'))
                 {text[0] = c;putchar(c);++thisitem;}
                 else putchar(7);
              break;
            case 6:
              if ((c == 'F') || (c == 'D'))
                  {text[0] = c;putchar(c);++thisitem;}
                 else putchar(7);
              break;
            case 7:
              if ((c == 'Y') || (c == 'N'))
                    {text[0] = c;putchar(c);++thisitem;}
                 else putchar(7);
              break;
            }
            }

     if (thisitem < 0) thisitem = LASTITEM;
     if (thisitem > LASTITEM) thisitem = 0;

     if (c == 59)
        {
        int i;
        cursor(22,10);
        for (i=0;i<50;i++) putchar(' ');
        cursor(22,10);

        switch (thisitem)
        {
        case 0:
           writestr("Enter name of directory such as \\123 ");
           break;
        case 1:
           writestr("For single file search enter DOS name ");
           break;
         case 2:
           writestr("Enter text to search ");
           break;
         case 3:
           writestr("File extents to skip/include (e.g.EXE and .COM) ");
           break;
         case 4:
           writestr("Search options [u]pper case [w]hole words ");
           break;
         case 5:
           writestr("Enter [I]nclude or [O]mit ");
           break;
         case 6:
           writestr("Search [F]ile or [D]irectory ");
           break;
         case 7:
           writestr("If all info is ok, [Y]es, else change ");
           break;
         } /* end case */

       }   /* end if */

       *item = thisitem;
       return (0);

}         /* end function */

/*
  sepname
  separate a path + file name into their parts

*/
void sepname(all,path,file)
char *all, *path, *file;
{
     int  i,j,k,l;
     char temp[80];

     i = strlen(all);

     for (j=i,k=0;j>=0;j--)
     {
     if (all[j] == '\\') break;
     ++k;
     }

     /* k is the length of the file name */

     --k;
     strcpy(temp,all);
     l = i - 1 - k;       /* length of path */
     temp[l] = 0;
     strcpy(path,temp);
     strcpy(file,temp+l+1);

}


/*
   cline
   clear a line

*/
void cline(row,col,len)
int  row,col,len;
{

     cursor(row,col);

     while (len--) putchar(' ');

}
/*
   sdir.c
   process the DOS directories

*/

int  sdir(buff,filename,action,ext,type)
char *buff;         /* directory name */
char *filename;     /* output filename */
int  action;        /* 0 = read , 1=open, 2=close */
char ext[10][80];   /* file extension to search */
int  type;          /* 0 = include, 1 = omit */
{
   static DIR *dir;
   struct direct *dir_entry;
   char   *calloc(int count, int size);
   static char  workfnam[80];
   static char  fileext[14];
   int    i,j,k;

   if (action == 1)           /* open the file, setup extension */
   {
   clear(workfnam,80,0);
   strcpy(workfnam,buff);
   strcat(workfnam,"\\*.*");

   dir = opendir(workfnam);     /* open directory */
   setdirat(dir,0x3f);          /* search all */
   for (i=0;i<10;i++)
       for (j=0;j<3;j++) ext[i][j] = toupper(ext[i][j]);
   return (1);
   }

   /************ sequential directory search **********************/

   if (action == 0)
   {
      while ((dir_entry = readdir(dir)) != NULL)
      {
      if (dir_entry->d_attrib & 0x18) continue;  /* skip label and dir */

      for (j=k=0;j<10;j++)
      {
      i = cmpext(dir_entry->d_name,ext[j]);     /* file type validation */
      if ((i == 0) && (type == 0))
         {
         strcpy(filename,buff);
         strcat(filename,"\\");
         strcat(filename,dir_entry->d_name);
         return (1);
         }

       if ((i == 0) && (type == 1))  ++k;  /* count matches */
      }       /* end for */

      if ((type == 1) && (k == 0))
         {
         strcpy(filename,buff);
         strcat(filename,"\\");
         strcat(filename,dir_entry->d_name);
         return (1);
         }

   }  /* end while */

         closedir(dir);
         return(0);

   }  /* end if action == 0 */

}

/*
  cmpext
  compare file extents

*/
cmpext(fnam,ext)
char *fnam, *ext;
{
     int i,j;
     char temp[10];
     char work[30];

     strcpy(work,fnam);
     i = strlen(fnam);
     for (j=i;j>=0;j--) if(fnam[j] == '.') break;
     ++j;

     strcpy(temp,work+j);     /* setup file extension for compare */
     i = strcmp(temp,ext);    /* compare */
     return i;                /* result of compare */
}


/*
  xcomp
  comparison routine
  for use within scan routine in xdir

*/
#define NULL 0

xcomp(str1,match,options,len)
char *str1;           /* null terminated string */
char *match;          /* string to match */
char *options;        /* options for match */
int  len;             /* length of buffer */
{
     char *cp;
     int  fnd,j;

     cp =str1;        /* initialize to first string */
     fnd = 1;         /* indicate no find */
     j = len;
     j += 2;

     while (1)  /* search the buffer */
     {
     fnd = xcmp(cp,match,options,j);
     if (fnd == 0) break;
     ++cp;
     --j;
     if (j <= 0) break;
     }

     return (fnd);
}

/*
   xcmp
   compare two strings, various options

*/
xcmp(str1,match,options,max)
char *str1;
char *match;          /* string to match */
char *options;
int  max;             /* maximum chars to search */
{
     char *cp, *cps, *cpm;
     char cs,cm,c;
     int  uflag;
     int  wflag;
     int  state;

     cps = str1;
     cpm = match;
     uflag = wflag = 0;
     cp = options;

     while (c= toupper(*cp++))   /* initialize options */
     {
     if (c == 'U') uflag = 1;
     else
     if (c == 'W') wflag = 1;
     }

     state = 0;

     while (1)
     {
     switch (state)
     {
     case 0:            /* initial state */
          cs = *cps++;  /* get a character from input stream */
          if (uflag) cs = toupper(cs);
          --max;
          if (max <= 0) return (-1);
          cm = *cpm++;  /* get a match character */
          if (uflag) cm = toupper(cm);
          if (cm == 0)  return (-1);
             else state = 1;
          break;

     case 1:  /* test cm is '?'  */
          if (cm == '?') state = 3;
             else state = 2;
          break;

     case 2:  /* cm not '?' so test equal */
          if (cm == '*') state = 10;
          else if (cs != cm)  return (-1);
            else state = 3;
          break;

     case 3:  /* get another set of comparison characters */
          cs = *cps++;
          if (uflag) cs = toupper(cs);
          --max;
          if (max <= 0) return (-1);
          cm = *cpm++;
          if (uflag) cm = toupper(cm);
          if (cm == 0)                   /* end of match string */
             {
             cs = toupper(cs);
             if ((wflag) && (cs>='A') && (cs <= 'Z'))   return (-1);
             else return 0;                /* a match */
             }
          state = 1;
          break;

     case 10:  /* cm was '*'  */
          cm = *cpm++;       /* get terminating character */
          if (cm == 0) return 0;      /* no term, it matches */
          if (uflag) cm = toupper(cm);
          state = 11;
          break;

     case 11:                /* loop searching for term char */
          cs = *cps++;       /* next input character */
          if (uflag) cs = toupper(cs);
          --max;
          if (cs <= 0) return (-1);
          if (cs == cm)      /* terminator reached */
             state = 3;      /* get another set of characters */
          break;

          }         /* end switch */


     }              /* end while */


}



