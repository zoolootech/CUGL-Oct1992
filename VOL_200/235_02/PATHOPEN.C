/*  002  14-Feb-87  pathopen.c

        Pathopen will open a file somewhere along the PATH.

        Copyright (c) 1987 by Blue Sky Software.  Alll rights reserved.
*/

#include <stdio.h>
#include "ov.h"          /* only needed for Strdup() define */

FILE *
pathopen(fn,mode)      /* open a file somewhere along the path */
char *fn;
char *mode;
{
   FILE *fp;
   int lastch;
   register char *path, *sp;
   char *psave, fname[100], *strchr(), *getenv();

   if ((fp = fopen(fn,mode)) == NULL)          /* try to open in current dir */

      if (path = getenv("PATH")) {             /* didn't open, any PATH? */
         path = psave = Strdup(path);          /* need zapable copy of PATH */

         while (strlen(path)) {                /* while something to check... */

            if (sp = strchr(path,';'))         /* ; seperates dir names in */
               *sp = '\0';                     /*  PATH, only chk 1 at a time */

            strcpy(fname,path);                /* build file name to chk for */

            if ((lastch = fname[strlen(fname)-1]) != '\\' && lastch != '/' &&
                 lastch != ':')
               strcat(fname,"\\");             /* add \ if not already a dir */

            strcat(fname,fn);                  /* add help name to dir name */

            if (fp = fopen(fname,mode))        /* we're done if it opened */
               break;

            if (sp)                    /* if there was a ; there might be */
               path = sp + 1;          /*   another dir in the path to chk */
            else
               break;                  /*   otherwise, just give up */
         }

         free(psave);                  /* clean up after ourselves */
      }

   return(fp);                 /* return whatever we got */
}
