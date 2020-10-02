/* TXTPLOT -- Enhanced version of BDS C txtplot() function.
 *
 * This version is in C, and does not use memory-mapped video.
 * It is written to be used in VIEW, version 1.51, for systems
 * without memory-mapped video.
 *
 * Version 1.0 -- 12/09/85
 * James Pritchett
 *
 */

#define MAXCOL  79      /* Width of screen - 1*/
#define CR  0x0d
#define TAB 0x09
#define ESC 0x1b
#define MASK 0x7f       /* For parity strip */

/* This "gotoxy" function should be customized for your specific
 * system.  The following is for an NEC PC-8801A.
 */

void gotoxy(row,col)
int row;
int col;
{
    putch(ESC);
    putch('=');
    putch(row + 32);
    putch(col + 32);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int txtplot(s,row,col,tabsize)
char *s;        /* string to plot */
int  row,col;   /* coordinates */
int  tabsize;   /* Size of tabs */
{
    int curcol;     /* Current location */
    int numsp;      /* Number of spaces to fill tabs */
    char c;

    gotoxy(row,col);    /* Locate the cursor */

/* txtplot quits plotting when it hits the end of a row or when it
 * sees a NULL or CR.
 */

    for (curcol = col; curcol < MAXCOL && *s && *s != CR; s++) {
        c = (*s) & MASK;
        if (c < ' ') {      /* If a control char . . . */
            if (c == TAB) {
                for (numsp = tabsize - (curcol % tabsize); 
                            numsp && curcol < MAXCOL; curcol++, numsp--)
                    putch(' ');
            }
        }
        else {      /* All other controls ignored */
            putch(*s);
            curcol++;
        }
    }
    return curcol;
}

/* end */

2);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

