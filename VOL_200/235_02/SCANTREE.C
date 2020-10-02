/*  003  14-Feb-87  scantree.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include "ov.h"                /* only needed for Strdup() define */
#include "dosfile.h"

#ifndef NULL
#define NULL (0)
#endif

static char *scanpath;
static int (*scanfunc)(), scanattr, firstime;

struct scanent { struct scanent *next; char *name; };

char *strrchr(), *strupr();
struct search_block *nxtfile();

/******************************************************************************
                               S C A N T R E E
 *****************************************************************************/

scantree(dir,pathbuf,attrib,func)  /* scan the specified dir tree */
char *dir, *pathbuf;
int attrib, (*func)();
{
   /* caller must initialize pathbuf - put in the drive spec, e.g. "C:"
      if starting at the root and dir names are going to be processed */

   scanfunc = func;                    /* ptr to function called with names */
   scanattr = attrib;                  /* attributes to scan for */
   scanpath = pathbuf;                 /* dir name work buffer */

   walk(dir);                          /* walk the dir tree */
}

/******************************************************************************
                                   W A L K
 *****************************************************************************/

walk(dirname)          /* walk the dir tree */
char *dirname;
{
   int pathlen;
   struct scanent *subdirs = NULL;
   register struct search_block *sbp;
   register struct scanent *nsub = (struct scanent *) &subdirs;

   /* build the pathname of the dir to scan */

   pathlen = strlen(scanpath);         /* remember callers length */
   if (strcmp(dirname,"\\") != 0) {    /* special case if root dir */
      strcat(scanpath,dirname);        /* add name of dir to scan */
      strcat(scanpath,"\\");
   } else                              /* starting at root */
      strcat(scanpath,dirname);        /* add root to scanpath */

   /* tell users routine what dir is about to be scanned, only scan it if
      users routine returns a NZ value - skip this one if 0 - note that the
      dir names all have trailing \'s when passed to user */

   if ((*scanfunc)(NULL,scanpath)) {   /* tell user what dir it is (has \) */

      strcat(scanpath,"*.*");          /* add wildcard string */

      /* scan all files in directory, call scanfunc to process each file entry,
         stack the names of any subdirectories so we can then scan them later */

      firstime = 1;

      while (sbp = nxtfile(scanpath,scanattr,&firstime)) {

         if ((*scanfunc)(sbp,NULL) == 0)  /* let somebody else see it */
            break;                        /* skip the rest if they say stop */

         /* if this is a subdir to also scan, build a scanent for it */

         if (sbp->attrib & 0x10 && *sbp->fn != '.')
            if (nsub->next = (struct scanent *) Malloc(sizeof(struct scanent))) {
               nsub = nsub->next;
               nsub->next = NULL;
               nsub->name = Strdup(sbp->fn);
            }
      }
   }

   /* if any subdirectories were found, scan 'em.  This isn't done
      earlier so the file search isn't complicated by the directory
      switches. */

   if (nsub = subdirs) {                       /* NULL if no sub's found */
      scanpath[strlen(scanpath)-3] = '\0';     /* remove *.* for next level */
      do {
         walk(nsub->name);             /* call ourselves to scan this subtree */
         free(nsub->name);             /* release sub dir name space   */
         subdirs = nsub;               /* eat our own tail as we crawl */
         nsub = nsub->next;            /*   down the list of subdirs   */
         free((char *)subdirs);        /*   you can't go home again    */
      } while (nsub);                  /* do all the subs found above  */
   }

   scanpath[pathlen] = '\0';           /* restore dir pathname for caller */
}


