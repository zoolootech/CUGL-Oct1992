/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 XDIR - Hard Disk Manager
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Apr 03, 1987
  DESCRIPTION:	 Hard Disk Manager for IBM PC
  KEYWORDS:	 Hard Disk Manager Dump Directory
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      xdir.C
  WARNINGS:	 None
  CRC:		 N/A
  SEE-ALSO:	 HDIR.DOC and XDIR.DOC
  AUTHOR:	 Mike Blakley 15645 SW 82 Cir Ln #76, Miami, Fl 33193
  COMPILERS:	 ECO-C
  REFERENCES:	 XDIR.DOC
*/


/*
   xdir.c
   Superwash
   to process the DOS directories

*/
#include "hdir.doc"          /* program documentation */
#include "stdio.h"
#include "dos.h"
#include "dir.h"
#include "xdir.h"
struct tdir *pnam[MAXDIR];   /* directory entries */
struct tdir *fnam[MAXFILE];  /* file names */

char  cdir[PATHSIZ];     /* current directory */
char  helpfile[PATHSIZ] = "xdir.doc";      /* name of helpfile */
unsigned cdate;          /* comparison date */
/*
  process DOS directories

*/
main(argc,argv)
int  argc;     /* argument count */
char **argv;   /* argument values */
{
     int    strcmp(char *,char *);
     void   dironly(char **buffer, int type,int level);
     void   update (char **buffer, int type,int level);
     char   *strdsave(struct direct *, char *root);

     char   *alloc(int amt);
     int    i,j;
     char   answer[MAXFILE];      /* for responses */
     char   srmask[15];           /* search mask */
     int    att;                  /* attribute search */
     struct tdir *tp;
     static int pcount, fcount;
     int    cf,ct;                /* flag for see files */
     char   work[20];             /* work area selected dir */
     static int firstime = 0;    /* for command line directory */
     char   c,temp[20];             /* debug */


     if (argc == 2)             /* process command line args */
      {
      if (strcmp(argv[1],"help") == 0) typ(helpfile);
      if (strcmp(argv[1],"-v")   == 0) ;
      }
     else if (argc == 1)
      {
      i = help();               /* display help message */
      if (i == 1) typ(helpfile);
      }

     pcount = bildir();                   /* build path array */
     qksort(pcount,pcomp,pswap);          /* sort by path name */

     if (argc == 2)                       /* test command line directory */
     {
     strcpy(work,argv[1]);
     j = testdir(work,pcount);
     if (j < 0) firstime = 0;
        else
         {
         firstime = 1;
         strcpy(cdir,work);
         clear(answer,MAXFILE,' ');
         answer[j] = 'S';
         }
     }


     while (1)                         /* main loop */
     {

     if (firstime)   firstime = 0;
         else
         {
         clear(answer,MAXFILE,' ');   /* clear answers */
         disx(pnam,pcount,answer,1); /* ask what directory  <=== */
         }

     for (i=ct=0;i<MAXFILE;i++)        /* get directory selection */
     {
     if (answer[i] == 'S')   /* selection criteria */
        {
        ++ct;
        tp = pnam[i];
        strcpy(cdir,tp->d_name);       /* note current directory */
        if (cdir[0] == 0) strcpy(cdir,"**ROOT**");
        break;
        }
     }

     if (ct == 0) break;                 /* nothing selected */

     while (1)                           /* get search criteria */
     {
     j = criteria(srmask,&att,&cf);
     if (j) break;
     }

     fcount = bilfil(pnam[i],srmask,cdate,att); /* build file names array */
     if (fcount == 0)
        {
        xmsg("No entries selected - press any key ");
        continue;
        }

     if (fcount == -1)
        {
        xmsg("Couldn't allocate memory ");
        break;
        }

     switch (cf)    /* determine sort method */
     {
     case 'D':
       qksort(fcount,fcompd,fswap);
       break;
     case 'S':
       qksort(fcount,fcomps,fswap);
       break;
     default:
       qksort(fcount,fcomp,fswap);     /* sort alpha on file names */
       break;
     }

     if ((cf == 'Y') || (cf == 'D') || (cf == 'S') || (cf == 'A'))
          dfnam(fcount,cdir);    /* display files */

     clear(answer,MAXFILE,' ');
     disx(fnam,fcount,answer,2);   /* ask what files */
     for (i=ct=0;i<MAXFILE;i++)    /* count responses */
        if (answer[i] != ' ') ++ct;

     if (ct) action(answer,pcount);    /* process answer */
     for (i=0;i<fcount;i++) free(fnam[i]);  /* free storage */

     clrscr();
     cursor(5,10);
     writestr("Processing completed - enter next directory name ");
     cursor(6,10);
     writestr("Press ENTER for root directory, ESC to end ");
     c = getch();
     if (c == 27) break;
     if (c == 13) work[0] = 0;
        else
        {
        putchar(c);
        gets(temp);
        work[0] = c;
        work[1] = 0;
        strcat(work,temp);
        }

     j = testdir(work,pcount);
     if (j < 0) firstime = 0;
        else
         {
         firstime = 1;
         strcpy(cdir,work);
         clear(answer,MAXFILE,' ');
         answer[j] = 'S';
         }

     }   /* end while 1 processing */
     clrscr();
}

/*
  action
  take the requested action
*/
void action(answer,pcount)
char *answer;
int  pcount;
{

     static int  i,j,k,l,c;
     int    rc;                 /* return code from delete */
     char filename[80];         /* used in rename and copy */
     struct tdir *tp, *fp;
     int  row;
     char key[30];              /* encryption key */
     static int dflag;          /* flag for display */
     int copybulk = 0;          /* no bulk copy */
     char bulkname[80];         /* bulk file name */
     int found;                 /* found flag for search of paths */
     char workname[80];         /* work area for file name */

     dflag = 1;                 /* indicate need for screen display */

     for (i=0;i<MAXFILE;i++)
     {
     tp = fnam[i];
     j = (int) answer[i];

     switch (j)
     {
     case 'T':                  /* list a file to console */
       typ(tp->d_path);
       dflag = 1;               /* indicate new display required */
       break;
     case 'L':                  /* list file to printer */
       clrscr();
       writestr("    File ");
       writestr(tp->d_path);
       writestr(" will be listed to the printer ");
       lis(tp->d_path);
       break;
     case 'C':                  /* copy file */
       if (copybulk)   /* bulk copy no questions */
          {
          strcpy(filename,bulkname);
          strcat(filename,"\\");
          strcat(filename,tp->d_name);
          xcopy(tp->d_path,filename);
          break;
          }

          dscr();         /* display the screen */
          row = 4;
          cursor(row++,20);
          writestr("Copy from file ");
          writestr(tp->d_path);
          writestr(" to ? ");
          gets(filename);
          if (strlen(filename) == 0) break;
          for (l=0,k=strlen(filename);l<k;l++)
              filename[l] = toupper(filename[l]);

          if (filename[0] != '\\')
             {
             cvp(tp->d_path,workname);
             strcat(workname,"\\");
             strcat(workname,filename);
             strcpy(filename,workname);  /* build output name */
             }

          else
          {
          for (l=found=0;l<pcount;l++)   /* see if a path */
            {
            fp = pnam[l];
            k = strcmp(fp->d_path,filename);
            if (k == 0) found = 1;
            }

          if (found == 1)
             {
             cursor(row++,20);
             writestr("Desire bulk copy? (Y)es (N)o ");
             c = toupper(getch());
             if (c == 'Y')
                {
                copybulk = 1;
                strcpy(bulkname,filename);
                strcat(filename,"\\");
                strcat(filename,tp->d_name);
                xcopy(tp->d_path,filename);
                }
             }

           }

           xcopy(tp->d_path,filename);     /* copy the file */

       break;
     case 'R':         /* rename the file */
       dscr();         /* display the screen */
       row = 4;
       cursor(row,20);
       writestr("Rename file ");
       writestr(tp->d_path);
       writestr(" to ? ");
       gets(filename);
       if (strlen(filename) == 0) break;
       if (filename[0] != '\\')
          {
          cvp(tp->d_path,workname);
          strcat(workname,"\\");
          strcat(workname,filename);
          strcpy(filename,workname);  /* build output name */
          }
       xrename(tp->d_path,filename);
       break;
     case 'D':            /* dump a file */
       xdump(tp->d_path);
       break;
       dflag = 1;         /* indicate new display */
     case 'X':            /* delete a file */
       if (dflag) {dscr();dflag = 0;row=3;}
       cursor(row++,20);
       writestr("Delete file ");
       writestr(tp->d_path);
       writestr(" ? Enter 'Y' to delete ");
       c = toupper(getch());
       if (c == 'Y')
          {
          rc = unlink(tp->d_path);
          if (rc == -1) xmsg("Unable to delete protected file");
          }
       if (row > 20) {dscr(); row =4;dflag=0;}
       break;
     case 'H':    /* hide the file */
       upmod(tp->d_path,"h");
       break;
     case 'U':    /* unhide */
       upmod(tp->d_path,"u");
       break;
     case 'P':    /* protect */
       upmod(tp->d_path,"r");
       break;
     case 'W':    /* write - remove read-only status */
       upmod(tp->d_path,"xa");
       break;
     case 'E':    /* encrypt */
       dscr();         /* display the screen */
       dflag = 1;      /* indicate new display is required */
       row = 4;
       cursor(row++,20);
       writestr("Encrypt file ");
       writestr(tp->d_path);
       cursor(row++,20);
       writestr("New name ");
       gets(filename);
       if (strlen(filename) == 0) break;
       cursor(row++,20);
       writestr("Encryption key ");
       gets(key);
       secure(tp->d_path,filename,key,1);   /* do the security */
       break;
     case 'S':         /* search for text */
       ctlr(tp->d_path);
       dflag = 1;            /* indicate new display */
       break;
     }   /* end case */
     }   /* end for  */

}

/*
     get the file selection criteria
     criteria
     return a 1 if criteria are ok, otherwise 0

*/
int criteria(srmask,att,cf)
char *srmask;     /* search mask */
int  *att;        /* attributes */
int  *cf;         /* see files */
{

     char c;
     int  item, refresh;
     char smask[20], satt[20], scf[20];
     char sdate[20];
     int  i;

     smask[0] = satt[0] = scf[0] = sdate[0] = item = 0;
     refresh = 1;

     while (1)
     {

     if (refresh)
     {
     clrscr();
     boxx(79,24,1,1);
     cursor(2,20);
     writestr("Hard Disk Manager - File Selection Criteria ");
     cursor(3,20);
     writestr("Processing directory - ");
     writestr(cdir);

     cursor(10,20);                  /* display questions */
     writestr("List (A)lpha (D)ate (S)ize? [ESC=end] ");
     cursor(10,60);
     writestr(scf);
     cursor(11,20);
     writestr("After date YYMMDD? ");
     cursor(11,50);
     writestr(sdate);
     cursor(12,20);
     writestr("Search mask? [*.*]");
     cursor(12,50);
     writestr(smask);
     cursor(13,20);
     writestr("File attribute to search? ");
     cursor(13,50);
     writestr(satt);
     cursor(14,20);
     writestr("Above items OK? (Y,N) ");
     cursor(20,20);
     writestr("Press F1 for help");
     refresh = 0;
     }

     switch (item)
     {
     case 0:      /* list type */
         cursor(10,60);                 /* get answers */
         c = toupper(getch());
         switch (c)
         {
         case 'A':
         case 'D':
         case 'S':
         case 13:
            if (c == 13) c = 'A';
            putchar(c);
            *cf = (int) c;
            ++item;
            break;
         case 27:   /* escape */
           *att = 0;    /* select all attributes */
           cdate = 0;                  /* all dates */
           strcpy(srmask,"*.*");       /* all masks */
           *cf = 0;                    /* no display */
           return (1);
           break;
         case 0:    /* extended character */
           c = getch();
         default:
           xmsg("Enter sort sequence for file display or ESC = default");
           break;
         }         /* end switch c */
         break;
      case 1:     /* after date */
        cursor(11,50);
        i = gethelp(sdate);
        if (i) xmsg("Select file by date, e.g. 860226 = Feb 26,1986");
        else
          {
          cdate = cvdate(sdate);       /* convert to binary */
          if (strlen(sdate) == 0)
             {
             cursor(11,50);
             writestr("ALL");
             }
          ++item;
          }
        break;
      case 2:         /* search mask */
        cursor(12,50);
        i = gethelp(smask);
        if (i) xmsg("File search mask (e.g. *.bak) default *.*");
        else
        {
        if (strlen(smask) == 0)
           {
           strcpy(srmask,"*.*");
           cursor(12,50);
           writestr(srmask);
           }
        else strcpy(srmask,smask);
        ++item;
        }
        break;
      case 3:         /* File attribute */
        boxx(15,6,18,5);
        cursor(19,6);
        writestr("Attributes");
        cursor(20,6);
        writestr("(R)eadonly");
        cursor(21,6);
        writestr("(H)idden");
        cursor(22,6);
        writestr("(S)ystem ");
        cursor(23,6);
        writestr("(A)rchive ");

        cursor(13,50);
        i = gethelp(satt);
        if (i) xmsg("File attributes - default is all ");
        else
        {
        if (strlen(satt) == 0)
              {
              *att = 0;
              cursor(13,50);
              writestr("ahdr");
              }
        else *att = cvatt(satt);  /* convert attributes */
        ++item;
        }
        break;
      case 4:
        cursor(14,50);
        c = toupper(getch());
        if (c == 'Y') return 1;
        if (c == 'N') return 0;
        putchar(0x07);
        xmsg("Enter Yes or No [Y,N]");
        break;
        }          /* end of switch */

    }     /* end while 1 */
}         /* end function */

/*
     dscr
     display the screen for action

*/
void dscr()
{
    clrscr();
    boxx(79,24,1,1);
    cursor(2,20);
    writestr("Hard Disk Manager - Selection Processing ");
    return;

}

/*
    fcompd
    compare files by dates

*/
fcompd(i,j)
int i,j;
{
   struct tdir *tpi;
   struct tdir *tpj;

   tpi = fnam[i];
   tpj = fnam[j];
   if (tpi->d_date < tpj->d_date) return (1);
   else
   if (tpi->d_date > tpj->d_date) return (-1);
   else
   if (tpi->d_time < tpj->d_time) return (1);  /* same date diff time */
   else
   if (tpi->d_time > tpj->d_time) return (-1);
   else return (0);

}


/*
    fcomps
    compare files by size

*/
fcomps(i,j)
int i,j;
{
   struct tdir *tpi;
   struct tdir *tpj;

   tpi = fnam[i];
   tpj = fnam[j];
   if (tpi->d_size < tpj->d_size) return (-1);
   else
   if (tpi->d_size > tpj->d_size) return (1);
   else return (0);

}

/*
  help
  display initial help message
  return 1 if help is requested, otherwise 0

*/
help()
{
   int  i;

   clrscr();
   boxx(79,24,1,1);
   cursor(5,10);
   writestr("XDIR - extended directory processing");
   cursor(6,10);
   writestr("version 1.0 ");
   cursor(8,10);
   writestr("The following options are available: ");
   cursor(10,10);
   writestr("F1 = Help ESC = end any other key = continue ");
   cursor(11,10);
   writestr("Note: to bypass this screen type xdir xxx ");
   cursor(12,10);
   writestr("  where xxx is any directory name");

   cursor(17,10);
   writestr("Action to take? ");
   cursor(17,27);
   i = getch();
   if (i == 27)
      {
      clrscr();
      exit(0);
      }

   if (i == 0) i = getch();
   if (i == 59) return (1);
      else return 0;

}

/*
   xmsg
   display a message at bottom, await reply then erase message

*/
void xmsg(msg)
char *msg;
{
    int i,j;

    cursor(23,2);
    writestr(msg);
    i = getch();
    if (i == 0) getch();
    j = strlen(msg);
    cursor(23,2);
    for (i=0;i<j;i++) putchar(' ');

}

/*
    gethelp
    fill buffer
    return 1 if help requested, otherwise 0

*/
gethelp(buffer)
char *buffer;
{
      char temp[40], work[40];
      int  c;

      c = getch();     /* first character */
      if (c == 0)
         {
         c = getch();
         if (c == 59) return (1);
         }

      if (c == 13) {buffer[0] = 0;return 0;}

      putchar(c);
      temp[0] = c;       /* any other character */
      temp[1] = 0;
      gets(work);
      strcat(temp,work);
      strcpy(buffer,temp);
      return 0;
}

/*
      testdir
      test that character string is a valid directory
      return 0 if valid else -1
*/
testdir(dirname,pcount)
char *dirname;        /* name to test */
int  pcount;          /* count of valid directory names */
{
     char work[20];
     int  i,j;
     char c, *cp;
     struct tdir *tp;

     i = 0;
     cp = dirname;
     while (c = *cp++) work[i++] = toupper(c);  /* make upper case */
     work[i] = 0;
     strcpy(dirname,work);

     for (i=0;i<pcount;i++)               /* see if dir is valid */
     {
     tp = pnam[i];
     j = strcmp(tp->d_name,work);
     if (j == 0) break;
     }
     if (j == 0) return (i);
        else return (-1);
}
