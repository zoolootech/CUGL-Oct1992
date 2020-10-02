/*  015  14-Feb-87  ovprompt.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.

   Note, some of these routines use putchr() and putstr() instead of
   disp_*() because on an IBM PC type system, putchr() will update
   the hardware cursor position while the disp_*() routines MAY not.
   The put*() routines can be #defined to be disp_*() routines if the
   disp_*() routines update the hardware cursor position.
*/


#include <ctype.h>
#include <setjmp.h>
#include "ov.h"

extern int brkhit;
extern int errno;
extern int sys_nerr;
extern char *sys_errlist[];

static char *boxsave;
static int box_row, box_rows, box_col, box_cols, reply_row, reply_col;

static char reply[MAX_REPLY+1];        /* buffer for user replys */

static char *esc2quit = "Press ESC to quit";
static char *esc2cont = "Press ESC to continue";

extern jmp_buf back_to_main;

int ALTCALL kilbox(), ALTCALL makbox();


/******************************************************************************
                             P R O M P T
 *****************************************************************************/


char * ALTCALL
prompt(t,s,ps,ip,rlen) /* prompt user, and read string reply */
char *t, *s;           /* prompt box title, prompt string */
char *ps;              /* initial prompt string */
int ip;                /* initial position in initial string */
int rlen;              /* length of reply allowed */
{
   char *sp;

   /* create the dialog box with the prompt */

   makbox(FIRST_NROW+4,t,s,rlen,esc2quit);

   sp = read_str(rlen,ps,ip);          /* read the users reply */

   setvattrib(DIS_NORM);               /* restore default video attrib */

   kilbox();                           /* remove the dialog box */

   return(sp);                         /* return the string */
}


/*****************************************************************************
                               A S K
 *****************************************************************************/

ask(s)                 /* prompt user, and read single char reply */
char *s;
{
   int ch;

   makbox(FIRST_NROW+4,"",s,1,"");     /* create dialog box with prompt */

   showcursor();                       /* so user knows where he is */

   ch = getchr();                      /* get a single character */

   hidecursor();                       /* don't need this anymore */

   setvattrib(DIS_NORM);               /* restore default video attrib */

   kilbox();                           /* remove the dialog box */

   return(ch);                         /* and tell user what was read */
}


/*****************************************************************************
                             B R K O U T
 *****************************************************************************/

brkout() {     /* see if user wants to break out of some operation */

   register int ch;

   /* The user can break out of some operations by signaling an interrupt -
      we consider a ^C, ^U (for old WordStar users), or ESC to be an
      interrupt signal */

   if ((ch = peekchr()) == ('C' - 0x40) || ch == ('U' - 0x40) || ch == 27) {
         ch = getchr();                /* flush char from buffer */
         brkhit = 1;                   /* use code below to interrupt */
   }

   /* The brkhit flag is set above if certain keys hit, it may also be set
      by the int 23h trap (^C) which is another way to interrupt */

   if (brkhit) {
      brkhit = 0;
      ch = ask("Interrupt? (Y/n): ");
      if (yes(ch))
         return(1);            /* yes, break out (interrupt) */
   }

   return(0);                  /* doesn't want to interrupt */
}


/*****************************************************************************
                             R E A D _ S T R
 *****************************************************************************/

char * ALTCALL
read_str(rlen,initial,offset)  /* read a string of length rlen */
int rlen, offset;
char *initial;
{
   register int ch;
   register char *cp;
   int insert = FALSE;
   char *defval, *endp;

   defval = initial ? initial : "";    /* set the initial (default) value */
   strcpyfill(reply,defval,rlen,' ');  /*   into the reply buffer */
   *(endp = reply + rlen)  = '\0';     /* terminate the reply */

   showcursor();                       /* let user see where the cursor is */

   cp = reply;                         /* start at the begining */
   for (; offset; --offset)            /* skip to callers offset position */
      putchr(*cp++);

   while ((ch = getchr()) != '\r' && ch != ESC_KEY) {

      if (ch == DEL) {                         /* delete current char key? */
         if (cp < endp) {
            strncpy(cp,cp+1,rlen-(cp-reply)-1);
            *(endp-1) = ' ';
            putstr_nomove(cp);
         } else
            beep();
         continue;
      }

      if (ch == RUBOUT) {                      /* rubout? */
         if (cp > reply) {
            strncpy(cp-1,cp,rlen-(cp-reply));
            *(endp-1) = ' ';
            putchr('\b');
            putstr_nomove(--cp);
         } else
            beep();
         continue;
      }

      if (ch == LEFT) {                        /* left arrow? */
         if (cp > reply) {
            --cp;
            putchr('\b');
         } else
            beep();
         continue;
      }

      if (ch == RIGHT) {                       /* right arrow? */
         if (cp < endp)
            putchr(*cp++);
         else
            beep();
         continue;
      }

      if (ch == INS) {                         /* insert mode key? */
         insert ^= 1;
         continue;
      }

      if (ch < ' ' || ch > 0x7f) {             /* don't enter weird chars */
         beep();
         continue;
      }

      if (cp < endp) {                         /* add char unless at end */

         if (insert) {
            reply[rlen-1] = '\0';
            memcpy(cp+1,cp,rlen-(cp-reply)-1); /* watches for overlap */
         }

         *cp++ = ch;
         putchr(ch);

         if (insert)
            putstr_nomove(cp);

      } else
         beep();

   }

   hidecursor();                       /* done, get rid of the cursor */

   /* if the user didn't escape out, remove trailing blanks from string and
      pass it back to caller.  If he/she did escape, return a null string. */

   if (ch != ESC_KEY) {
      for (cp = reply + rlen - 1; cp >= reply && *cp == ' '; )
         *cp-- = '\0';
      return(reply);
   } else
      return("");
}


/*****************************************************************************
                          S H O W _ E R R O R
 *****************************************************************************/

show_error(show_dos,ljmp,count,m1)     /* show user an error msg */
int show_dos, ljmp, count;
char *m1;
{
   char **mp, fullmsg[SCREEN_COLS*2+1];

   /* create one string from callers strings */

   mp = &m1;                           /* variable # arguments, point to 1st */
   *fullmsg = '\0';                    /* clear full msg area */

   for ( ; count; count--, mp++)       /* concat each char string */
      strcat(fullmsg,*mp);

   /* if caller wants DOS msg, include that too */

   if (show_dos && errno <= sys_nerr)
      strcat(fullmsg,sys_errlist[errno]);

   makbox(FIRST_NROW+4,"",fullmsg,0,esc2cont);  /* create dialog box with msg */
   setvattrib(DIS_NORM);

   beep();                             /* make sure user is awake */
   while (getchr() != 27) ;            /* wait for an ESC */

   kilbox();                           /* remove the dialog box */

   if (ljmp)                           /* longjmp back to main if a longjmp */
      longjmp(back_to_main,ljmp);      /*   code was given */
}


/*****************************************************************************
                             M A K B O X
 *****************************************************************************/

static int ALTCALL
makbox(frow,title,msg,rlen,lastmsg)    /* make a dialog box on the screen */
int frow, rlen;
char *msg, *title, *lastmsg;
{
   register int i;
   int lm, ll, llr, rl = 0;

   box_row = frow;                     /* save starting row # */

   /* determine the number of rows/cols and start column for the dialog box */

   box_rows = 5;
   if ((i = rlen + (lm = strlen(msg))) > SCREEN_COLS - 4) {
      box_rows += 2;
      i = rl = lm > rlen ? lm : rlen;  /* when rl is NZ, reply on sep line */
   }

   if (ll = strlen(lastmsg)) {         /* is a last line msg specified? */
      box_rows += 2;
      i = ll > i ? ll : i;
      llr = box_row + box_rows - 2;
   }

   box_cols = i + 4;
   box_col = (SCREEN_COLS - box_cols) >> 1;

   /* popup the dialog box and display prompt string */

   boxsave = (char *) Malloc(box_rows * box_cols * 2);

   setvattrib(DIS_BOX);
   popup(frow,box_col,box_rows,box_cols,boxsave);

   if (strlen(title)) {                   /* display box title if any */
      disp_char_at(' ',frow,box_col+2);
      disp_str(title);
      disp_char(' ');
   }

   disp_str_at(msg,box_row+2,box_col+2);  /* display prompt string */

   if (ll)                                /* display last line msg if any */
      disp_str_at(lastmsg,llr,(SCREEN_COLS - ll) >> 1);

   reply_row = box_row + 2;               /* display row containing the reply */
   reply_col = box_col + 2 + lm;          /* display col where reply starts */

   if (rl) {                           /* adjust reply area address if */
      reply_row += 2;                  /*   more than one line is being used */
      reply_col = box_col + 2;
   }

   gotorc(reply_row,reply_col);        /* position at reply area */

   if (rlen) {
      setvattrib(DIS_HIBOX);           /* display the reply field in hilite */
      disp_rep(' ',rlen);              /* so user knows how big is is */
      gotorc(reply_row,reply_col);     /* return to start of reply area */
   }
}


/*****************************************************************************
                            K I L B O X
 *****************************************************************************/

static int ALTCALL
kilbox() {             /* remove that dialog box that makbox() built */

   popdwn(box_row,box_col,box_rows,box_cols,boxsave);    /* nothing to it */
   free(boxsave);
}
