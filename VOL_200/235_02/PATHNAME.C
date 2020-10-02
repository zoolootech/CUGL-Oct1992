/*  003  14-Feb-87  pathname.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include "ov.h"                /* only needed for Strdup() define */
#include "dosfile.h"

#ifndef NULL
#define NULL (0)
#endif

#ifndef MAX_PATHLEN
#define MAX_PATHLEN (64)               /* max pathname len under MS/PC-DOS */
#endif

char *strupr(), *strrchr(), *strchr(), *qualdir();

struct search_block *nxtfile();


/****************************************************************************
                          P A R S E P A T H
 ****************************************************************************/

char *
parsepath(refdir,reffn,target,isdir,todirp,tofnp)
char *refdir, *reffn, *target, **todirp, **tofnp;
int isdir;
{
   /* I don't know if this should really be called parsepath, but I couldn't
      think of a better name.  Here's what it does: given a reference
      directory specification, a reference file name, and a traget name
      (dir and/or file name) it creates a fully qualified target name
      consisting of a drive specification and file name.  One of the fancier
      things this routine does (or has done) is expand relative dir spec's
      (e.g. ..\ov) to fully qualified dir spec's (e.g. c:\src\ov).  This
      requires that the reference dir be fully qualified.  parsepath()
      will also return individual dir and/or file name strings if pointers
      to char pointers are passed as todirp and tofnp.  If one or both of
      these strings are not desired, pass the value NULL instead of a
      pointer address.  All three of the parsepath() result strings are
      in dynamically allocated memory that can be free'd with the free()
      function.

      Oh yea, the isdir parameter tells parsepath() if the target name
      is just a dir name (isdir = true) or if it contains a file name
      (and maybe a dir name) (isdir = false).  This parameter was added
      for performance - parsepath() doesn't need to call isadir() each
      time if the caller already knows its not just a dir name.

      Note: the refdir is used as if it were the current dir when qualifying
      the target name.  If the target contains no dir spec, the refdir spec
      will be used.  If the target contains a relative dir spec, the refdir
      will be used as the starting point (a relative dir spec is relative to
      the refdir).

      BUT if the target contains a drive code that is different than the
      drive code in refdir, parsepath() will call the external function
      getcdir() to get the current dir spec for the target drive and use
      that as the refdir instead of the passed refdir.
*/

   char *tmp = NULL;
   char *tardir, *tarfn, *np;

   /* make sure both the reference dir and the target use \ instead of / */

   fixdirspec(refdir);  strupr(refdir);        /* uppercase to make */
   fixdirspec(target);  strupr(target);        /* compares easier   */

   /* determine if the target is on the same drive as the reference,
      get a new reference dir if not */

   if (strlen(target) > 1 && target[1] == ':')        /* target have a drive? */
      if (*refdir != *target) {                       /* different than ref?  */
         refdir = tmp = (char *) Malloc(MAX_PATHLEN+4);    /* make new refdir */
         strncpy(tmp,target,2);
         tmp[2] = '\\';
         getcdir(*target-'A'+1,tmp+3);       /* sorta like getcwd() for drive */
      }

   /* determine if the target string has a directory and/or file name */

   if (isdir) {                /* is it only a dir (no file name)? */

      tardir = Strdup(target);
      tarfn  = Strdup(reffn);

   } else              /* well is it a dir and file name? */

      if ((np = strrchr(target,'\\')) || (np = strrchr(target,':'))) {
         tardir = Strndup(target,np-target+1);
         tarfn  = Strdup(np+1);

      } else {         /* must be just a file name */

         tardir = NULL;
         tarfn  = Strdup(target);
      }

   /* if a target dir was supplied, make sure its fully qualified */

   if (tardir) {
      np = tardir;
      tardir = qualdir(refdir,tardir);
      free(np);
   } else
      tardir = Strdup(refdir);

   if (tmp)                    /* free locally allocated refdir string */
      free(tmp);

   /* make a copy of the fully qualified name for the user */

   target = (char *) Malloc(strlen(tardir)+strlen(tarfn)+2);
   strcpy(target,tardir);
   if (target[strlen(target)-1] != '\\')
      strcat(target,"\\");
   strcat(target,tarfn);

   /* return the seperate target dir/fn strings to user or release'm */

   if (todirp)
      *todirp = tardir;
   else
      free(tardir);

   if (tofnp)
      *tofnp = tarfn;
   else
      free(tarfn);

   return(target);             /* return the full target name */
}


/****************************************************************************
                              Q U A L D I R
 ****************************************************************************/

/* This routine expands a partial dir specification to a fully qualified
   dir pathname.  It requires a fully qualified refernece dir name and the
   partial dir pathname.  The reference dir spec is expected to start with
   a drive code and end without a trailing '\'.  The partial dir spec may
   or may not contain a drive code, the qualified result will.  If an error
   is encountered, this routine returns a null string. */

char *
qualdir(refdir,partdir)        /* fully qualify a partial dir spec */
char *refdir, *partdir;
{
   register char *cp, *path;
   char *psave, *sp, fullpath[MAX_PATHLEN+4];

   /* setup the drive code of the resultant dir spec */

   strncpy(fullpath,refdir,3);         /* copy over "<drive>:\" */
   fullpath[3] = '\0';

   refdir += 3;                        /* don't need ref drive\root again */
   if (strlen(partdir) > 1 && partdir[1] == ':')   /* and don't really */
      partdir += 2;                                /* need partial drive */

   /* initialize the full pathname to the reference dir name if the partial
      name doesn't start at the root */

   if (*partdir == '\\')               /* partial (relative) start at root? */
      partdir++;                       /* already got \ above, don't need now */

   else {                              /* must really be partial spec */

      strcat(fullpath,refdir);                 /* start from reference dir */
      if (fullpath[strlen(fullpath)-1] != '\\')    /* we want trailing \ */
         strcat(fullpath,"\\");                    /*   for now */
   }

   cp = fullpath + strlen(fullpath);   /* always points to end of fullpath */

   path = psave = Strdup(partdir);     /* need a copy we can modify */

   /* process the partial pathname a dir at a time - still more to do
      while path has something in it */

   while(strlen(path)) {

      if (sp = strchr(path,'\\'))      /* find end of current dir name */
         *sp = '\0';                   /* null terminate it */

      if (strcmp(path,".") == 0)       /* "." entry is current dir */
         ;                             /*   so there is nowhere to go */

      else

         if (strcmp(path,"..") == 0) {         /* ".." means goto parent dir */
            *--cp = '\0';                      /* zap trailing \ */
            if (cp = strrchr(fullpath,'\\'))   /* find \ before current */
               *++cp = '\0';                   /* zap curr - parent now last */
            else {                             /* error, no more parents! */
               *fullpath = '\0';               /* return null string */
               break;                          /* stop processing */
            }

         } else                /* not "." or "..", must be subdir name */

            if (strlen(fullpath) + strlen(path) < sizeof(fullpath)) { /* fit? */
               strcpy(cp,path);
               strcat(cp,"\\");                /* add subdir to fullpath */
               cp += strlen(cp);               /* add trailing \, new end */
            } else {                           /* error! pathname too big */
               *fullpath = '\0';
               break;
            }

      if (sp)                  /* if there was a \, skip beyond to chk */
         path = sp + 1;        /*   for another one, otherwise there can't */
      else                     /*   be anything else to process */
         break;
   }

   free(psave);                                /* free local copy */

   if (strlen(fullpath) > 3)                   /* remove trailing \ if */
      fullpath[strlen(fullpath)-1] = '\0';     /*   not at root */

   return(Strdup(fullpath));                   /* give caller his own copy */
}


/******************************************************************************
                           F I X D I R S P E C
 ******************************************************************************/

fixdirspec(dir)        /* convert /'s to \'s in dir specifications */
register char *dir;
{
   while (dir = strchr(dir,'/'))       /* make sure all /'s turned to \'s */
      *dir++ = '\\';
}


/******************************************************************************
                               I S A D I R
 *****************************************************************************/

isadir(refdir,dir)     /* returns 1 if dir represents a directory name, */
char *refdir, *dir;    /*   0 if not. */
{
   int firsttime;
   register int s;
   char *qdir, *tmp = NULL;
   register struct search_block *sbp;

    /* For MSDOS, we assume the path is a directory name if it ends in \, /,
       its just a drive specifier (A:), is "." or "..", or DOS says its a
       directory */

   /* check the easy cases first */

   fixdirspec(dir);            /* make all /'s into \'s */

   if ((s = dir[strlen(dir)-1]) == '\\' || s == ':' || strcmp(dir,".") == 0 ||
       strcmp(dir,"..") == 0)
      return(1);

   /* need to ask DOS if this is a directory */

   /* determine if the target is on the same drive as the reference,
      get a new reference dir if not - this is done here so it doesn't
      need to be done by isadir() callers - note parsepath() does pretty
      much the same thing, maybe this should be in qualdir() */

   if (strlen(dir) > 1 && dir[1] == ':')       /* target have a drive? */
      if (*refdir != *dir) {                   /* different than ref?  */
         refdir = tmp = (char *) Malloc(MAX_PATHLEN+4);    /* make new refdir */
         strncpy(tmp,dir,2);
         tmp[2] = '\\';
         getcdir(*dir-'A'+1,tmp+3);          /* sorta like getcwd() for drive */
      }

   qdir = qualdir(refdir,dir);         /* qualify it first */

   firsttime = 1;
   s = ((sbp = nxtfile(qdir,0x10,&firsttime)) && sbp->attrib & 0x10);

   free(qdir);
   if (tmp)
      free(tmp);

   return(s);
}


/******************************************************************************
 **                       C H A N G E _ D I R                                **
 *****************************************************************************/

change_dir(dir)        /* change the current directory */
register char *dir;
{
   int newdir, cur_drive, rc;

   rc = 0;                             /* assume all will go okay */

   if (strlen(dir) > 1 && dir[1] == ':') {     /* isolate any drive letter */
      cur_drive = current_drive();             /* current drive (A,B,...) */
      newdir = toupper(*dir);                  /* wanted new dir */
      change_drive(newdir);                    /* switch to named drive */
      if (current_drive() != newdir)           /* did we get there? */
         return(-1);                           /*   no!, give up now */
      dir += 2;                                /*   yes, skip over drive: */
   } else
      cur_drive = 0;                           /* means no drive switch */

   if (strlen(dir) > 0)                        /* is there a dir spec? */
      if (chdir(dir) != 0) {                   /* switch directories */
         if (cur_drive)                        /* drive switched? */
            change_drive(cur_drive);           /*   error, switch back */
         rc = -1;                              /* return non 0 error code */
      }

   return(rc);                 /* tell caller if we worked */
}


