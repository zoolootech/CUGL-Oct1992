/*  016  24-Jan-87  ovcmd.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include <stdio.h>
#include <process.h>
#include "ov.h"

extern WINDOW cw;
extern FILE_ENT files[];

char *strrchr();


/******************************************************************************
 **                         D O _ C M D                                      **
 *****************************************************************************/

do_cmd() {             /* read and execute a single dos command */

   char *cmd;

   cmd = prompt("","Enter a single DOS command to execute",NULL,0,MAX_REPLY);
   if (strlen(cmd) == 0)
      return;

   run_cmd(cmd,TRUE);                  /* execute the command */
}


/******************************************************************************
                             E X E C U T E
 ******************************************************************************/

execute() {            /* execute the current file */

   char title[MAX_NAMELEN+10], *fn, *ext, *cmd, *fnbuf;

   /* make sure the file looks like its executable */

   if (files[cw.curidx].flags & DIR)
      show_error(0,11,1,"Can't execute a directory!");

   if ((ext = strrchr(fn = files[cw.curidx].name,'.')) == NULL ||
       strlen(ext) < 4 || strstr(".EXE .COM .BAT",ext) == NULL)
      show_error(0,11,2,fn,
                 " does not have a EXE, COM, or BAT extension, can't execute!");

   fnbuf = fname(&files[cw.curidx]);   /* put cmd less ext into bufers */
   *strrchr(fnbuf,'.') = '\0';
   strcpy(title,"Execute ");
   strcat(title,fn);

   cmd = prompt(title,"Add parameters to command, return to execute.",
         fnbuf,strlen(fnbuf)+1,MAX_REPLY);

   free(fnbuf);                        /* free fname() space */

   if (strlen(cmd) == 0)               /* user can cancel by clearing line */
      return;

   run_cmd(cmd,TRUE);                  /* run the command */
}


/******************************************************************************
 **                          R U N _ C M D                                   **
 *****************************************************************************/

run_cmd(cmd,pause)     /* execute a dos command via command.com */
char *cmd;
int pause;
{

   clr_scr();                          /* home the cursor and clear screen */
   showcursor();                       /* let user see the cursor */

   system(cmd);                        /* do the DOS command */

   if (pause) {
      putstr("\r\nPress any key to return to OVERVIEW.");
      getchr();
   }

   /* who knows what state things might be after a DOS command, restart
      everything from scratch */

   reinit_tty();                       /* make sure the tty is setup right */
   renew();                            /* renew the screen, etc */
}


/******************************************************************************
 **                      S P A W N _ C L I                                   **
 *****************************************************************************/

void
spawn_cli() {          /* spawn a copy of the command interpeter */

   char comspec[MAX_PATHLEN+1], *ep, *getenv();

   clr_scr();                          /* home cursor, clear screen */
   showcursor();                       /* show user the cursor */

   putstr("\r\nEnter the command 'EXIT' to return to OVERVIEW.\r\n");

   if (ep = getenv("COMSPEC"))         /* use COMSPEC env variable if defined */
      strcpy(comspec,ep);
   else
      strcpy(comspec,"\COMMAND.COM");  /* otherwise hope its in the root dir */

   spawnl(P_WAIT,comspec,NULL);        /* run a copy of command.com */

   /* who knows what state things might be after going to DOS, restart
      everything from scratch */

   reinit_tty();                       /* make sure tty is setup right */
   renew();                            /* renew screen, etc */
}
