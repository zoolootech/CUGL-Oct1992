/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 XDIR - Hard Disk Manager
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Apr 03, 1987
  DESCRIPTION:	 Hard Disk Manager for IBM PC
  KEYWORDS:	 Hard Disk Manager Dump Directory
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      zdir.c
  WARNINGS:	 None
  CRC:		 N/A
  SEE-ALSO:	 HDIR.DOC and XDIR.DOC
  AUTHOR:	 Mike Blakley 15645 SW 82 Cir Ln #76, Miami, Fl 33193
  COMPILERS:	 ECO-C
  REFERENCES:	 XDIR.DOC
*/


/*
   zdir.c
   Superwash
   to process the DOS directories

*/
#include "stdio.h"
#include "dos.h"
#include "dir.h"
#include "xdir.h"
extern struct tdir *pnam[];      /* directory entries */
extern struct tdir *fnam[];      /* file names */
extern char  cdir[];             /* current directory */
extern unsigned cdate;           /* comparison date */

/*
   bildir
   build directory entries

*/
bildir()
{
   static DIR *dir;
   struct direct *dir_entry;
   char *cp;
   char *calloc(int count, int size);
   struct tdir *tp;
   int   i;
   char  workfnam[PATHSIZ];
   char  workpath[PATHSIZ];
   int   pcount;


   pcount = 0;

   cp = calloc(sizeof(struct tdir),1);
   if (cp == NULL) return(-1);

   tp = (struct tdir *) cp;    /* set up the first entry */
   tp->d_attrib = 0x10;
   tp->d_size =   0L;
   pnam[pcount++] = tp;        /* priming entry */

   for (i=0;((i<pcount)&&(pcount<MAXDIR));i++)
   {
   tp = pnam[i];
   clear(workfnam,PATHSIZ,0);
   strcpy(workfnam,tp->d_path);
   strcat(workfnam,"\\*.*");
   clear(workpath,PATHSIZ,0);
   strcpy(workpath,tp->d_path);

   dir = opendir(workfnam);     /* open directory */
   setdirat(dir,0x3f);          /* was 0x10 - search all */

   while ((dir_entry = readdir(dir)) != NULL)
   {
   if ((dir_entry->d_attrib & 0x10) != 0x10) continue;
   if (dir_entry->d_name[0] == '.') continue;
   cp = calloc(sizeof(struct tdir),1);
   if (cp == NULL) return(-1);
   tp = (struct tdir *) cp;
   strcpy(tp->d_name,dir_entry->d_name);
   tp->d_attrib = dir_entry->d_attrib;
   tp->d_time =   dir_entry->d_time;
   tp->d_date =   dir_entry->d_date;
   tp->d_size =   dir_entry->d_size;
   strcpy(tp->d_path,workpath);
   strcat(tp->d_path,"\\");
   strcat(tp->d_path,tp->d_name);
   pnam[pcount++] = tp;
   if (pcount >= MAXDIR) return(MAXDIR);
   }

   closedir(dir);
   }
   return (pcount);

}

/*
   bilfil
   build the file array

*/
int bilfil(buff,srmask,cdat,att)
struct tdir *buff;
char *srmask;
int  cdat;
int  att;
{
   static DIR *dir;
   struct direct *dir_entry;
   char *calloc(int count, int size);
   struct tdir *tp;
   int   fcount,i;
   char  workfnam[PATHSIZ];
   char  workpath[PATHSIZ];
   char *cp;

   fcount = 0;

   tp = buff;                 /* get path name */
   clear(workfnam,PATHSIZ,0);
   strcpy(workfnam,tp->d_path);
   strcpy(workpath,tp->d_path);
   strcat(workfnam,"\\");
   strcat(workfnam,srmask);

   dir = opendir(workfnam);     /* open directory */
   setdirat(dir,0x3f);          /* search all */

   while ((dir_entry = readdir(dir)) != NULL)
   {
   if (dir_entry->d_attrib & 0x18) continue;
   i = dir_entry->d_attrib & att;
   if (att != 0)
      if (i == 0) continue;
   if (dir_entry->d_date < cdat) continue;
   cp = calloc(sizeof(struct tdir),1);
   if (cp == NULL) return(-1);
   tp = (struct tdir *) cp;
   strcpy(tp->d_name,dir_entry->d_name);
   tp->d_attrib = dir_entry->d_attrib;
   tp->d_time =   dir_entry->d_time;
   tp->d_date =   dir_entry->d_date;
   tp->d_size =   dir_entry->d_size;
   strcpy(tp->d_path,workpath);
   strcat(tp->d_path,"\\");
   strcat(tp->d_path,tp->d_name);
   fnam[fcount++] = tp;
   if (fcount >= MAXFILE)
       {
       free(fnam[fcount]);
       --fcount;   /* prevent overflow */
       break;
       }
   }

   closedir(dir);

   return (fcount);
}
/*
   dfnam
   display files names
*/
void dfnam(fcount,dirname)
int fcount;
char *dirname;
{

   int   i,lc,c;
   struct tdir *tp;
   char obuff[100];
   int   inter,row;
   static char msg[] =
        "Filename            Attr    Length Date     Time   Path";

   c = (int) dirname[0];        /* prevent compiler error */
   clrscr();
   boxx(79,24,1,1);
   cursor(1,30);
   writestr(cdir);
   cursor(2,2);
   writestr(msg);
   inter = lc = row = 0;

   for (i=0;i<fcount;i++)    /* display the files */
   {
   tp = fnam[i];
   dirform(tp,obuff);
   cursor(row+3,2);
   obuff[75] = 0;
   writestr(obuff);
   ++lc;
   ++row;
   if (lc > 20)
      {
      cursor(24,2);
      writestr("\033[7mPress any key for next page");
      writestr(" or X = cancel\033[0m ");
      c = toupper(getch());
      if (c == 'X') {inter=1;break;}
      inter = row = 0;
      clrscr();
      boxx(79,24,1,1);
      cursor(1,30);
      writestr(cdir);
      cursor(2,2);
      writestr(msg);
      lc = 0;
      }
   }

   if (inter == 0)
       {
       cursor(24,2);
       writestr("\033[7mEnd of display - press any key\033[0m");
       getch();
       }
   clrscr();

}

/*
   compare two pointers

*/
pcomp(i,j)
int i,j;
{
    int k;

    struct tdir *cpi;
    struct tdir *cpj;

    cpi = pnam[i];
    cpj = pnam[j];

    k = strcmp(cpi->d_path,cpj->d_path);
    return (k);

}


/*
   swap two pointers

*/
int pswap(i,j)
int i,j;
{
    struct tdir *cpi;

    cpi = pnam[i];
    pnam[i] = pnam[j];
    pnam[j] = cpi;
    return (0);
}


/*
   compare two pointers

*/
fcomp(i,j)
int i,j;
{
    int k;

    struct tdir *cpi;
    struct tdir *cpj;

    cpi = fnam[i];
    cpj = fnam[j];

    k = strcmp(cpi->d_path,cpj->d_path);
    return (k);

}


/*
   swap two pointers

*/
int fswap(i,j)
int i,j;
{
    struct tdir *cpi;

    cpi = fnam[i];
    fnam[i] = fnam[j];
    fnam[j] = cpi;
    return (0);
}

