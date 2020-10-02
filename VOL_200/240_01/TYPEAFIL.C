/*
**  typeafil.c
**  26 oct 86, dco
*/


#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <alloc.h>
#include "ciao.h"


/* 
** typeafile() -- type a file with optional word wrap in a window box
*/

#define MSG_SOURCE "ciao: typeafile"  /* version info could go here */
#define PGUP 0xC9
#define LINESIZE 128
#define ESCMSG  "--PgUp reviews, any key continues, Esc quits--"

int Control_C_Flag = 0;  /* global informs caller that exit was by Control-C */

static FILE *f;
static char *s, *word, *msg;
static jmp_buf mk;
static int lines, wide, deep, longline, wordwrap;

static int review = 0;
static int contin = -1;





static void yo_heave_ho()     /* control-C, escape, ferror() grease trap */
{
     longjmp( mk, -1 );
}


static void cctrap()
{
      Control_C_Flag = 1;     /* inform caller of system level interrupt */
      yo_heave_ho();
}
static int garm()
{
       return (0);
}


static void reframe()
{
      review = lines = 0;
      rewind(f);
      strcpy(s,"");
      strcpy(word,"");
      clrwindow();
}



static void muf( p ) char *p;
{
     register int i;
     register char *q;

     ++lines;

     /* allow typeafile() to read its own documentation; 
     ** i.e., suppress ^ escape char commands by using
     ** wink() instead of wputs()
     */

     q = p;
     while ( *q ) wink( *q++ );
     wink('\n');

     if (lines >= deep)
     {
        wprintf("^1%s^", msg ); 
        while( kbhit() ) getch();

        if ((i = keyin( noop )) == 27) yo_heave_ho();  /* FLOW STATEMENT #1 */
        review = ((i == PGUP) || (i == '9'));          /* FLOW STATEMENT #2 */

        for ( i = strlen( msg ); i; i-- ) wink('\b');
        clreol(); 
        lines = 0;
     }
}




static void showscreen()
{
    register char *q, *r;

    do
    {
         /* BLOCK:  get input, from file if necessary */

              if (strlen(word) >= wide)
              {
                  strcpy( s, word );
                  strcpy( word, "" );
              }
              else
              {
                  if (feof(f))  /* this fixes the duplicate line bug at eof */
                  {
                       contin = 0;
                       continue;
                  }
                  else
                  {
                       fgets( s, LINESIZE, f );      /* get next line from file */
                  }
                  if (ferror(f))
                  {
                       yo_heave_ho();
                  }
                  r = strchr( s, '\n' );  /* newline in string? */ 
                  longline = 0;
                  if ( r != NULL )
                  {
                      *r = '\0';          /* make line null terminated */
                  }
                  else longline = 1;      /* fixes extra space in longlines */
              }


         /* BLOCK:  handle word wrap */

              if (wordwrap && strlen(word) > 0) 
              {
                  if (strlen( s ) == 0)     /* we're at end of a paragraph! */
                  {
                      muf(word);            /* finish up last line */
                      if (review) continue;
                      muf(s);               /* put space between paragraphs */
                      strcpy(word,"");
                      continue;
                  }
                  else  /* insert old word(s) in front of new */
                  {
                      r = strrchr(word,'\0');
                      r--;
                      if (!longline)
                      {
                           if (*r != ' ')
                               strcat( word, " ");
                           if (strpbrk(r,".!?") != NULL)
                               strcat( word, " ");
                      }
                      strcat( word, s );
                      strcpy( s, word );
                      strcpy( word,"" );
                  }
              }


         /* BLOCK:  split s into s and word, if necessary */

              q = s + wide;                  /* find right margin */
              r = strrchr(s, '\0');          /* find tail end */
              if ( r > q )              /* tail is beyond margin */
              {
                 /* does q break in a blank field or a text field? */

                 if (*q == ' ')  /* q is in a blanks field, easy case! */
                 {
                      *q = '\0'; 
                      while (*++q == ' ')    /* skip right to text (if any) */
                          ;
                      strcpy( word, q );           /* save (null?) remainder */
                 }
                 else  /* q is in a text field */
                 {
                      r = q;                      /* in case there's no help */
                      while ((q > s) && (*q != ' '))  /* find blanks to left */
                          q--;
                      if ( q == s )  /* gosh, no blanks! */
                      {
                          strcpy( word, r );
                          *r = '\0'; 
                      }
                      else /* if ( q > s ) */
                      {
                          q++;                       /* advance to word nose */
                          strcpy( word, q );               /* save remainder */
                          *q = '\0';             /* truncate s after a blank */
                      }
                 }
              }

         /* BLOCK:  output the line */

              muf( s );
              strcpy(s,"");
              if (!wordwrap && strlen(word) > 0)
              {
                /* wordwrap suppressed, behave like ordinary type command */
                   muf( word );
                   strcpy(word,"");
              }

    } while (lines > 0 && contin && !review);

    if (!contin)  /* end of file condition */
    {
        wputs("^2 eof ^1 --PgUp reviews, any key quits--^");
        while (kbhit()) getch();
        review = (lines = keyin(noop)) == PGUP;
    }

    contin |= (strlen(word) > 0 || review);
}




static void show()        /* recursive, puts page addresses on the stack */
{
    auto long filepos;    /* where each screen page starts in the file */

    while (contin)        /* and the garden path begins here...! */
    {
         if (review)      /* show returned!  back up exactly one screen page */
         {
              reframe();
              fseek(f,filepos,SEEK_SET);
         }
         else 
              filepos = ftell(f);  /* mark place for THIS screen! */

         showscreen();    /* show THIS screen; sets contin, review flags */

         if (review) 
              return;     /* (gasp! twisty! devious!) show LAST screen! */
         else 
              show();     /* show NEXT screen! stack gets deeper */
    }
}



void typeafile( p, wrap, tx, ty, bx, by ) char *p; int wrap, tx, ty, bx, by;
{
     char far *b;
     union REGS cursor;

     Control_C_Flag = 0;
     ctrlbrk ( cctrap );
     b = savescreen( &cursor );
     hidecursor();

     if (((s = malloc( LINESIZE * 2 )) == NULL) || ((word = malloc( LINESIZE * 2 )) == NULL))
     {
          thurb();
          defcursor();
          wprintf("%s ERROR: not enough k",MSG_SOURCE);
          exit(1);
     }

     windowbox( tx, ty, bx, by );     /* set up display area */
     getwindow( &tx, &ty, &bx, &by ); /* was the window squashed? */

     fullscreen();
     gotoxy( tx + 1, ty - 1 );
     wprintf("µ^2 %s ^Æ", p );
     setwindow( ++tx, ty, bx, by );   /* get a little bit of right margin */
     gotoxy(0,0);

     wide = bx - tx;
     deep = by - ty;

     msg = ESCMSG;
     if ((wide < strlen(msg)) || (deep < 2))
     {
          thurb();
          fullscreen();
          defcursor();
          wprintf("\n%s ERROR: bad window size: %dx%d?",MSG_SOURCE,wide,deep);
          exit(1);
     }

     if ( (f = fopen( p, "r" )) != NULL )
     {
        if (setjmp(mk) != 0)
        {
             if ferror(f) /* while attempting to read open file */
             {
                  fclose(f);  
                  goto zoo;
             }
             /* otherwise, Control-C or ESC gave us a nice exit */
        }
        else
        {
             wordwrap = wrap;    /* makes the feature optional */
             contin = -1;
             while (contin)      /* usually, just passing through! */
             {
                  reframe();
                  show();        /* normally, contin = 0 when show returns */

               /* But if contin is still non-zero at this point, we've 
               ** got a nonsensical request to review page minus one.
               ** Go to BOF and start over.  Rewind clears feof flag,
               ** if set, while fseek(f,0L,SEEK_SET) does not.
               */
             }
        }
        fclose(f);  /* WOI-FM is playing midnight space music. */
     }
     else 
     {
zoo:    thurb();
        wprintf("\n^1%s ERROR: couldn't read %s^\n",MSG_SOURCE,p);
        setsynch(1); 
        perror( "Might have been"); 
        setsynch(0); 
        wprintf("\n^1--any key continues--^"); 
        while (kbhit()) getch(); getch(); 
     }

     free( s );
     free( word );
     restorescreen( b, &cursor );
     ctrlbrk( garm );
}

/* eof: typeafil.c */

