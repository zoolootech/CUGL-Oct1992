/*  061  14-Feb-87  ovfile.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include <stdio.h>
#include <dos.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ov.h"
#include "dosfile.h"

static int stowidx;
static FILE_ENT *stowfp;
static DRIVE_ENT *drvlst = NULL;
static char *none_tagged = "There are NO tagged files.";
static char *notitself = "You can't copy a file to itself!";
static char *mustbedir = "You must enter a drive and/or directory name!";

extern WINDOW cw;
extern int diridx;
extern char **dirlst;
extern FILE_ENT files[];
extern char *nullname, *cantopen;

DRIVE_ENT *findrive(int);
struct search_block *nxtfile();
char far *largest_f(unsigned int, unsigned int *);
char *strupr(), *strchr(), *dirplus(FILE_ENT *, char *);
char *parsepath(char *, char *, char *, int, char **, char **);
int free_f(char far *), stowfile(struct search_block *, char *);
int readbuf(int,char far *,unsigned int), writebuf(int,char far *,unsigned int);
int add2windows(char *, char *, FILE_ENT *), delfromwins(char *, char *);

/******************************************************************************
 **                        G E T F I L E S                                   **
 *****************************************************************************/

getfiles() {           /* get data for the files in the current dir */

   int firsttime;
   char pathbuf[MAX_PATHLEN+6];
   register struct search_block *sbp;

   cw.files_size = 0;                  /* no files yet */
   firsttime = TRUE;                   /* tell nxtfile() its the first call */

   stowidx = 0;                        /* start storing at the begining */
   stowfp = files;

   /* if showall mode is active, call scantree to get all files, otherwise
      scan the current dir ourselves */

   if (cw.showall) {   /* scan the current disk? */

      *pathbuf = '\0';
      strncat(pathbuf,cw.dirbuf,2);
      scantree("\\",pathbuf,0x16,stowfile);    /* scan entire disk */

   } else        /* scan the current directory */

      while (stowidx < MAX_FILES && (sbp = nxtfile("*.*",0x16,&firsttime)))
            stowfile(sbp,NULL);

   /* sort the file names if there are any, also set nfiles */

   if (cw.nfiles = stowidx)
      sort_files(NULL);

   /* set current file pointer to the first file */

   cw.curidx = 0;

   /* set file counters */

   cw.num_files = cw.nfiles;           /* files in the dir */
   cw.num_tagged = 0;                  /* no files tagged yet */
   cw.tag_size = 0;

}


/*****************************************************************************
                             S T O W F I L E
 *****************************************************************************/

static int
stowfile(sbp,dirp)     /* store a file in files[] */
register struct search_block *sbp;
char *dirp;
{
   register int len;
   static char *lastdir = NULL;
   register FILE_ENT *fp = stowfp;     /* fast stow pointer */

   /* if dirp is not NULL, this call is defining which dir is being scanned,
      add this dir name to the showall dir list - return NZ (keep going) if
      dir name added okay, 0 (stop scanning) if can't add dir name - don't
      incr diridx unless everything is okay.  The dir name passed will always
      have a trailing \ which we don't really want - don't keep it unless
      this happens to be the root dir (len == 3) */

   if (dirp) {
      if (diridx < MAX_DIR) {
         len = strlen(dirp);
         dirlst[diridx++] = lastdir = Strndup(dirp,len > 3 ? len-1 : len);
         return(1);
      }
      return(0);
   }

   /* ignore . and .. entries.  If the file mask is defined, make
      sure the file name matches.  Make sure the file attributes
      match the selection attributes */

   if (stowidx >= MAX_FILES || *sbp->fn == '.' ||
       (cw.selatrs & sbp->attrib) != sbp->attrib ||
       (*cw.mask && (cw.maskcmp != match_name(sbp->fn,cw.mask))))

      return(1);       /* don't want this one, but keep looking */

   /* we want the file, fill in files[] entry */

   fp->size = sbp->size;
   fp->flags = sbp->attrib & 0x3f;
   fp->index = stowidx++;
   fp->date = sbp->date;
   fp->time = sbp->time;
   strcpy(fp->name,sbp->fn);
   fp->dirp = cw.showall ? lastdir : cw.dirbuf;   /* point to files dir str */
   cw.files_size += fp->size;                     /* accumulate total size */

   stowfp++;                             /* update for next time */
   return(1);                            /* keep looking */
}


/******************************************************************************
 **                             I N F O                                      **
 *****************************************************************************/

info() {               /* toggle the extended info display */

   cw.info_display ^= 1;               /* toggle info on/off */

   infocnt(cw.info_display ? 1 : -1);  /* one more or less info window */

   adjust_window();                    /* resize the window data */

   update_window(1);                   /* display files in new format */
}


/******************************************************************************
                              R E N _ C U R
 ******************************************************************************/

ren_cur() {            /* rename (or move) the current file */

   int rc;
   register FILE_ENT *fp;
   char *newname, fn[MAX_NAMELEN+1];
   static char ptxt[] = "Enter new file name or directory name for ";
   char *fnp, *target, *todir, *tofn, *dirp, pmsg[sizeof(ptxt)+MAX_NAMELEN+1];

   fp = &files[cw.curidx];

   strcpy(pmsg,ptxt);                          /* build prompt string */
   strcat(pmsg,strcpy(fn,fp->name));

   newname = strupr(prompt("Rename current file",pmsg,NULL,0,MAX_REPLY));
   if (strlen(newname) == 0)
      return;

   /* figure out the full target name, the dir, and the fn */

   rc = isadir(dirp=fp->dirp,newname); /* need to know if this is a dir name */

   target = parsepath(dirp,fn,newname,rc,&todir,&tofn);

   fnp = fname(fp);                    /* from name */

   rc = rename(fnp,target);            /* rename/move it */

   if (rc == 0) {                      /* rename okay? */

      /* note: fn is a local array instead of a pointer to fp->name because
         add2windows() may shift the entries in files[] around thereby making
         fp->name point to the wrong file - same goes for local dirp */

      add2windows(todir,tofn,fp);      /* add to new window(s) */
      delfromwins(dirp,fn);            /* del from old window(s) */

   } else              /* unable to rename */

      show_error(1,0,3,"Unable to rename ",fn,": ");

   free(target);               /* release temp strings */
   free(todir);
   free(tofn);
   free(fnp);
}


/******************************************************************************
                              R E N _ T A G
 ******************************************************************************/

ren_tag() {            /* rename (move) all tagged files to another dir */

   int i, rc;
   register FILE_ENT *fp;
   char *newdir, *fnp, *target, *todir, *dirp, fn[MAX_NAMELEN+1];

   if (cw.num_tagged == 0)             /* are there any tagged files? */
      show_error(0,14,1,none_tagged);

   newdir = strupr(prompt("Move tagged files","Enter new directory name",
            NULL,0,MAX_REPLY));

   if (strlen(newdir) == 0)
      return;

   /* the name given must be a directory name */

   if (!isadir(cw.dirbuf,newdir))
      show_error(0,12,1,mustbedir);

   /* Okay, move the tagged files */

   for (i = 0, fp = files; i < cw.nfiles && !brkout(); i++, fp++)

      if (fp->flags & TAGGED) {                /* this file tagged? */

         /* figure out the target names */

         dirp = fp->dirp;
         target = parsepath(dirp,strcpy(fn,fp->name),newdir,1,&todir,NULL);

         fnp = fname(fp);              /* from name */
         rc = rename(fnp,target);      /* move the file */

         if (rc == 0) {                /* move okay? */

            /* note: fn is a local array instead of a pointer to fp->name 'cause
            add2windows() may shift the entries in files[] around thereby making
            fp->name point to the wrong file */

            add2windows(todir,fn,fp);  /* add to new window(s) */
            delfromwins(dirp,fn);      /* del from old window(s) */

         } else        /* error, unable to rename/move this file */

            show_error(1,0,3,"Unable to move ",fn,": ");

         free(target);                 /* must be tough on malloc & friends */
         free(todir);
         free(fnp);
      }
}


/******************************************************************************
 **                    E R A S E _ C U R R E N T                             **
 *****************************************************************************/

erase_current() {      /* erase the current file */

   int ch;
   char askmsg[30];
   register FILE_ENT *fp;

   fp = &files[cw.curidx];

   strcpy(askmsg,"Erase ");
   strcat(askmsg,fp->name);
   strcat(askmsg," ? (y/N): ");
   ch = ask(askmsg);

   if (yes(ch))
      if (delfile(fp)) {                       /* delete the file */
         delfromwins(fp->dirp,fp->name);       /* remove from windows */
         getvolsiz(*cw.dirbuf,&cw.drivep->vol_size, /* vol stats have changed */
            &cw.drivep->vol_free,&cw.drivep->clustersiz);
      }
}


/*****************************************************************************
                          E R A S E _ T A G G E D
 *****************************************************************************/

erase_tagged() {       /* erase the tagged files */

   int ch;
   register int i;
   register FILE_ENT *fp;

   if (cw.num_tagged == 0)             /* are there any tagged files? */
      show_error(0,14,1,none_tagged);

   ch = ask("Erase all tagged files? (y/N): ");
   if (!yes(ch))
      return;

   /* scan files[] looking for tagged files, when found, delete it */

   for (i = 0, fp = files; i < cw.nfiles && !brkout(); i++, fp++)
      if (fp->flags & TAGGED) {
         disp_msg(2,"Erasing ",fp->name);
         if (delfile(fp))
            delfromwins(fp->dirp,fp->name);
      }

   clr_msg();                                  /* clear last erasing msg */

   getvolsiz(*cw.dirbuf,&cw.drivep->vol_size,  /* volume stats have changed */
      &cw.drivep->vol_free,&cw.drivep->clustersiz);
}


/*****************************************************************************
                               D E L F I L E
 *****************************************************************************/

static int
delfile(fp)            /* delete a file */
register FILE_ENT *fp;
{
   int rc;
   char *fn;

   rc = unlink(fn = fname(fp));        /* actually delete the file */
   free(fn);

   if (rc)                             /* delete okay? */
      show_error(1,0,3,"Unable to erase ",fp->name,": ");

   return(rc == 0);
}


/******************************************************************************
 **                      C O P Y _ C U R R E N T                             **
 *****************************************************************************/

copy_current() {       /* copy the current file */

   FILE_ENT tmpfent;
   register FILE_ENT *fp;
   register DRIVE_ENT *dp;
   char *dest, *target, *todir, *tofn;

   dest = prompt("Copy current file","Copy to where? ",NULL,0,MAX_REPLY);

   if (strlen(dest)) {
      fp = &files[cw.curidx];
      target = parsepath(fp->dirp,fp->name,dest,isadir(fp->dirp,dest),&todir,&tofn);

      if (strcmp(fp->dirp,todir) != 0 || strcmp(fp->name,tofn) != 0) {
         if (copyfile(fp,target)) {
            tmpfent = *fp;                     /* new file will have */
            tmpfent.flags |= ARCHIVE;          /* ARCHIVE attrib on  */
            add2windows(todir,tofn,&tmpfent);
            dp = findrive(*todir);
            getvolsiz(dp->drive,&dp->vol_size,&dp->vol_free,&dp->clustersiz);
         }
      } else           /* attempt to copy file to itself! */

         show_error(0,0,1,notitself);

      free(target);
      free(todir);
      free(tofn);
   }
}


/******************************************************************************
 **                        C O P Y _ T A G G E D                             **
 *****************************************************************************/

copy_tagged() {        /* mass copy of tagged files to somewhere */

   int i, ch;
   FILE_ENT tmpfent;
   register FILE_ENT *fp;
   register DRIVE_ENT *dp;
   char *dir, *target, *todir, *tofn;

   if (cw.num_tagged == 0)             /* are there any tagged files? */
      show_error(0,14,1,none_tagged);

   dir = prompt("Copy tagged files","Copy to which dir? ",NULL,0,MAX_REPLY);
   if (strlen(dir) == 0)
      return;

   if (!isadir(cw.dirbuf,dir))         /* user must give a directory name */
      show_error(0,12,1,mustbedir);

   /* Okay, copy the tagged files */

   for (fp = files, i = 0; i < cw.nfiles && !brkout(); i++, fp++)

      if (fp->flags & TAGGED) {
         disp_msg(2,"Copying ",fp->name);

         target = parsepath(fp->dirp,fp->name,dir,1,&todir,&tofn);

         if (strcmp(fp->dirp,todir) != 0 || strcmp(fp->name,tofn) != 0) {

            if (copyfile(fp,target)) {
               tmpfent = *fp;                     /* new file will have */
               tmpfent.flags |= ARCHIVE;          /* ARCHIVE attrib on  */
               add2windows(todir,tofn,&tmpfent);
            }

         } else        /* trying to copy a file to itself */

            show_error(0,0,1,notitself);

         free(target);                 /* release temp strings */
         free(todir);
         free(tofn);
      }

   clr_msg();                          /* clear last copying msg */

   /* update volume stats after copy */

   dp = strlen(dir) > 1 && dir[1] == ':' ? findrive(*dir) : cw.drivep;
   getvolsiz(dp->drive,&dp->vol_size,&dp->vol_free,&dp->clustersiz);

}


/******************************************************************************
 **                         C O P Y F I L E                                  **
 *****************************************************************************/

static int
copyfile(fp,to)        /* copy from to */
register FILE_ENT *fp;
char *to;
{
   char *fn;
   int fd, td, len;
   char far *buffer;
   unsigned int bufsiz;
   static char *copyabort = " -- copy aborted";

   /* open the from file */

   fd = open(fn = fname(fp),O_RDONLY|O_BINARY);
   free(fn);

   if (fd == -1) {
      show_error(1,0,3,cantopen,fp->name,": ");
      return(0);
   }

   /* allocate space for the in memory copy buffer */

   buffer = largest_f(63*1024,&bufsiz);  /* get largest possible blk up 2 63k */
   if (bufsiz < 1024) {                  /* give it up if < 1k available */
      if (bufsiz)                        /* release the tiny buffer */
         free_f(buffer);
      show_error(0,0,2,"No free memory to allocate copy buffer",copyabort);
      return(0);
   }

   /* open the output file */

   if ((td = open(to,O_CREAT|O_BINARY|O_TRUNC|O_RDWR,S_IWRITE)) == -1) {
      free_f(buffer);
      show_error(1,0,3,cantopen,to,": ");
      return(0);
   }

   /* copy the file, a buffer at a time */

   while (len = readbuf(fd,buffer,bufsiz))
      if (writebuf(td,buffer,len) == -1) {
         free_f(buffer);
         show_error(1,0,3,"Error writting to ",to,": ");
         return(0);
      }

   setftime(td,fp->date,fp->time);     /* set the to file time = from file */

   close(fd);                          /* files copied, close files */
   close(td);
   free_f(buffer);                     /*   and release copy buffer */

   /* give the new file the same attributes as the old, but add ARCHIVE */

   setattrib(to,(fp->flags & (ARCHIVE | SYSTEM | HIDDEN | RDONLY)) | ARCHIVE);

   return(1);          /* tell caller we think we worked */
}


/******************************************************************************
                                L O G I N
 *****************************************************************************/


login() {              /* login to another directory */

   char *new_dir, *todir;

   new_dir = prompt("","Login to which drive/directory? ",NULL,0,MAX_REPLY);

   /* if they give a relative dir spec and showall is in effect, we want
      the relative dir spec to be relative to the current file's dir, not
      the current dir */

   if (strlen(todir = new_dir)) {
      if (cw.showall && cw.curidx < cw.nfiles)
         parsepath(files[cw.curidx].dirp,"",new_dir,1,&todir,NULL);

      switch_dir(todir);

      if (todir != new_dir)            /* free mem if parsepath called */
         free(todir);
   }
}


/******************************************************************************
 **                       S W I T C H _ D I R                                **
 *****************************************************************************/

switch_dir(dir)        /* change the current directory */
char *dir;
{
   int rc;
   register DRIVE_ENT *dp;

   if ((rc = change_dir(dir)) == 0) {  /* dir switched okay? */

      if (cw.showall)                  /* turn off showall mode if it was */
         showoff();                    /*   in effect in this window */

      getcwd(cw.dirbuf,MAX_PATHLEN);   /* get the current dir name */
      initdrive(*cw.dirbuf);           /* (re)init DRIVE_ENT for this drive */
      getfiles();                      /* load the files[] structure */
      adjust_window();                 /* resize window data */
      update_header();                 /* update the header  */
      update_window(1);                /* and the window data */

   } else {                            /* can't switch, tell user */

      show_error(1,0,3,"Can't change to ",dir,": ");
   }
   return(rc);
}


/*****************************************************************************
                            I N I T D R I V E
 *****************************************************************************/

initdrive(drive)       /* initialize DRIVE_ENT for 'drive' */
int drive;
{
   register DRIVE_ENT *dp;

   dp = findrive(*cw.dirbuf);       /* get DRIVE_ENT address 4 drive */
   cw.drivep = dp;                  /* keep address in window block */

   get_set_vol(dp->volbuf,NULL);            /* get the new volume name */
   getvolsiz(*cw.dirbuf,&dp->vol_size,      /*  and the volume size stats */
             &dp->vol_free,&dp->clustersiz);/*  incase they are different */

}


/******************************************************************************
                            F I N D R I V E
 *****************************************************************************/

DRIVE_ENT *
findrive(drive)        /* find or allocate a DRIVE_ENT for drive */
int drive;
{
   register DRIVE_ENT *dp, *ldp = NULL;

   dp = drvlst;                        /* try to find the DRIVE_ENT in the */
   while (dp != NULL) {                /*   linked list of DRIVE_ENTs */
      if (dp->drive == drive)
         break;
      ldp = dp;
      dp = dp->next;
   }

   if (dp == NULL) {                   /* allocate a new DRIVE_ENT */
      dp = (DRIVE_ENT *) Malloc(sizeof(DRIVE_ENT));
      dp->next = NULL;
      dp->drive = drive;
      if (ldp)
         ldp->next = dp;
      else
         drvlst = dp;
   }

   return(dp);
}


/*****************************************************************************
                               F I N D I R
 *****************************************************************************/

char *
findir(dir)    /* find a dirlst entry that matches dir */
char *dir;
{
   register int i;
   register char **dp;

   for (dp = dirlst, i = diridx; i; i--, dp++)
      if (strcmp(dir,*dp) == 0)
         return(*dp);

   return("");                 /* better never happen! */
}


/******************************************************************************
 **                         S E T _ V O L                                    **
 *****************************************************************************/

set_vol() {            /* set the volume label */

   char *label;

   label = prompt("Set volume label","Enter new volume label: ",NULL,0,11);
   if (strlen(label) == 0)
      return;

   get_set_vol(cw.drivep->volbuf,label);    /* one call to set it */
   get_set_vol(cw.drivep->volbuf,NULL);     /* another to see what comes back */

   gotorc(VOL_ROW,1);                  /* display volume label */
   out_str(cw.drivep->volbuf,11,' ');
}


/*****************************************************************************
                          M A T C H _ N A M E
 *****************************************************************************/

match_name(np,pp)      /* determine if file name matched pattern */
register char *np, *pp;
{
   /* Why you might ask is this code like this, well..., I have plans to port
      this code to a system that has file names up to 79 characters long, and
      I thought this type of code would be easier to port than something that
      knew MS-DOS names are 8+3 */

   while (*np) {
      if (*np == *pp)                          /* name char match pattern? */
         goto match;
      else
         if (*np == '.') {                     /* watch to switches to the */
            while (*pp == '*' || *pp == '?')   /* extension, the pattern   */
               pp++;                           /* must have one also */
            if (*pp != '.')
               return(0);                      /* no pattern ext, no match */
            goto match;
         } else
            if (*pp == '*') {                  /* allow wildcards */
               np++;                           /* NOTE: doesn't advance the */
               continue;                       /*       pattern pointer */
            } else
               if (*pp == '?')
                  goto match;
               else
                  return(0);                   /* don't match, not wild */
   match:
      np++;                                    /* chars match, advance */
      pp++;
   }

   /* the name string matched so far, make sure the pattern only has
      wildcards left (if anything) */

   while (*pp)
      if (*pp != '*' && *pp != '?' && *pp != '.')
         return(0);
      else
        pp++;

   return(1);                          /* A MATCH! */
}


/*****************************************************************************
                           P A C K F I L E S
 *****************************************************************************/

packfiles() {          /* pack the files structure */

   FILE_ENT *endp;
   register FILE_ENT *p, *q;

   p = q = files;
   endp = &files[cw.nfiles];

   while (q < endp)
      if (*p->name) {
         if (p == q)
            p = ++q;
         else
            p++;
      } else
         if (*q->name == '\0')
            q++;
         else {
            *p++ = *q;
            *q->name = '\0';
            q++;
         }

   cw.nfiles = p - files;              /* new # of files in files[] */
}


/*****************************************************************************
                                F N A M E
 *****************************************************************************/

char * ALTCALL
fname(fp)              /* return the address of a file name */
register FILE_ENT *fp;
{
   return(dirplus(fp,fp->name));       /* maybe this routine is too simple */
}

/*****************************************************************************
                             D I R P L U S
 *****************************************************************************/

char *
dirplus(fp,pp)         /* return a string which is the files dir + whatever */
register FILE_ENT *fp;
char *pp;
{
   register char *dp;

   if (cw.showall) {
      dp = (char *) Malloc(strlen(fp->dirp)+strlen(pp)+2);
      strcpy(dp,fp->dirp);
      if (dp[strlen(dp)-1] != '\\')
         strcat(dp,"\\");
      strcat(dp,pp);
      return(dp);
   } else
      return(Strdup(pp));
}
