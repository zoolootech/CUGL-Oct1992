/*************************************************************************
main function to test some of the ansi device driving functions
which follow below.

written by Rex Jaeschke of Rockville, MD. 1983 (301) 251-8987
compiler used DeSmet v2.2

ci () is a DeSmet compiler specific function which does direct console
input of 1 character without echoing it. It is used in sgrdsr and the
testing program only. All else should run on any compiler.
*************************************************************************/

#define CTRL_C 3
#define BELL 7
#define NULL '\0'
#define LINEMIN 1       /* Minimum screen line # */
#define LINEMAX 25      /* Maximum screen line # */
#define COLMMIN 1       /* Minimum screen column # */
#define COLMMAX 80      /* Maximum screen column # */
#define LARROW 75
#define RARROW 77
#define UARROW 72
#define DARROW 80
#define HOME 71
#define END 79
#define CHOME 119

main ()
{
       int c;

       scred ();                       /* clear screen */
       scrcup (12,40);         /* move to screen center */
       while ((c = ci()) != CTRLC) {
               if (c == NULL) {        /* do we have extended code? */
                       c = ci();
                       switch (c) {
                       case LARROW:
                               scrcub (1);
                               break;
                       case RARROW:
                               scrcuf (1);
                               break;
                       case UARROW:
                               scrcuu (1);
                               break;
                       case DARROW:
                               scrcud (1);
                               break;
                       case CHOME:
                               scred ();
                               break;
                       case HOME:
                               scrcup (LINEMIN,COLMMIN);
                               break;

                       case END:
                               scrcup (LINEMAX,COLMMAX);
                               break;
                       default:
                               putchar (BELL);
                               break;
                       }
               }
               else
                       putchar (BELL);
       }               
}


#define ESCAPE 27       /* ASCII ESC character definition */

/*************************************************************************
SCR_CUB - Cursor Backward.

Moves the cursor backward n columns. Current line remains unchanged. If # of
columns exceeds left-of-screen, cursor is left at the left-most column.
*************************************************************************/

scrcub (ncolms)
int ncolms;
{
       printf ("%c[%dD",ESCAPE,ncolms);
}

/*************************************************************************
SCRCUD - Cursor Down.

Moves the cursor down n lines. Current column remains unchanged. If # of lines
to move down exceeds bottom-of-screen, cursor is left at the bottom.
*************************************************************************/

scrcud (nlines)
int nlines;
{
       printf ("%c[%dB",ESCAPE,nlines);
}

/*************************************************************************
SCRCUF - Cursor Forward.

Moves the cursor forward n columns. Current line remains unchanged. If # of
columns exceeds right-of-screen, cursor is left at the right-most column.
*************************************************************************/

scrcuf (ncolms)
int ncolms;
{
       printf ("%c[%dC",ESCAPE,ncolms);
}


/*************************************************************************
SCRCUP - Cursor Position. (same as HVP)

Moves the cursor to the specified position line,colm.
*************************************************************************/

scrcup (line,colm)
int line,colm;
{
       printf ("%c[%d;%dH",ESCAPE,line,colm);
}

/*************************************************************************
SCRCUU - Cursor Up.

Moves the cursor up n lines. Current column remains unchanged. If # of lines
to move up exceeds top-of-screen, cursor is left at the top.
*************************************************************************/

scrcuu (nlines)
int nlines;
{
       printf ("%c[%dA",ESCAPE,nlines);
}

/*************************************************************************
SCRDSR - Device Status Report.

Returns the Cursor Position Report (CPR) sequence in the form ESC[line;colmR

ci () is a DeSmet compiler specific function which does direct console
input of 1 character without echoing it.
*************************************************************************/

scrdsr (line,colm)
int *line,*colm;
{
       int i = 0;
       char cpr[10];

       printf ("%c[6n",ESCAPE);
       while ((cpr[i++] = ci ()) != 'R')
               ;
       cpr[i] = '\0';

/* format of cpr[] is ESC[rr;ccR row and colm are always two digits */

       *line = ((cpr[2]-'0')*10)+cpr[3]-'0';
       *colm = ((cpr[5]-'0')*10)+cpr[6]-'0';
}

/*************************************************************************
SCRED - Erase in Display.

Erases all of the screen leaving the cursor at home

*************************************************************************/

scred ()
{
       printf ("%c[2J",ESCAPE);
}

/*************************************************************************
SCREL - Erase in Line.

Erases from the cursor to the end of the line including the cursor position.
*************************************************************************/

screl ()
{
       printf ("%c[2K",ESCAPE);
}

/*************************************************************************
SCRHVP - Horizontal and Vertical Position. (same as CUP)

Moves the cursor to the specified position line,colm.
*************************************************************************/

scrhvp (line,colm)
int line,colm;
{
       printf ("%c[%d;%dH",ESCAPE,line,colm);
}

/*************************************************************************
SCRRCP - Restore Cursor Position.

Restores the cursor to the value it had when previously saved by scr_scp.
*************************************************************************/

scrrcp ()
{
       printf ("%c[u",ESCAPE);
}

/*************************************************************************
SCRSCP - Save Cursor Position.

Saves the current cursor position for later restoration by scr_rcp.
*************************************************************************/

scrscp ()
{
       printf ("%c[s",ESCAPE);
}

/*************************************************************************
SCRSGR - Set Graphics Rendition.


Sets the character attribute specified by the parameter.
Attributes remain in force until reset or changed.
*************************************************************************/

scrsgr (attrib)
int attrib;
{
       printf ("%c[%dm",ESCAPE,attrib);
}

/*************************************************************************
SCRSM - Set Mode.

Sets the screen width or type specified by the parameter.
*************************************************************************/

scrsm (param)
int param;
{
       printf ("%c[=%dh",ESCAPE,param);
}

/*************************************************************************
SCRRM - Reset Mode.

Sets the screen width or type specified by the parameter.
*************************************************************************/

scrrm (param)
int param;
{
       printf ("%c[=%dl",ESCAPE,param);
}
