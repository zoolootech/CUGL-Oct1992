/*============================================================

 Video_io.c.

 A set of customised video routines to work
 with the disk utilities ADU and DE.

 By Alex Cameron.

 Note:  REGS template is defined in dos.h.
==============================================================*/

#include <dos.h>
#include <stdio.h>
#define    LINLEN  64

/*----------------------Function prototypes--------------------*/

void    clrscrn( );
void    put_string(char *,int);
void    clr_window(int,int,int,int,int);
void    scroll_window(int,int,int,int,int);
void    gotoxy(int,int);
void    getxy(int *,int *);
int     putfa(int,int,char *,int,int);
void    get_string (char *);
void    put_char (int, int);

/*--------------------------------------------------------------*/


/*
** Clear the screen utilising the BIOS call.
*/
void clrscrn () {
union REGS inregs,outregs;

    inregs.h.ah = 6;       /* funtion */
    inregs.h.al = 0;
    inregs.h.bh = 7;
    inregs.x.cx = 0;
    inregs.h.dl = 79;      /* column */
    inregs.h.dh = 24;      /* row    */
    int86(0X10, &inregs, &outregs);
}
/*
** Clear the window specified.  The attribute taken, is defined
** in the IBM Tech Reference Manual.
*/
void clr_window (x1,y1,x2,y2,attr) int x1,y1,x2,y2,attr; {
union REGS inregs,outregs;

    inregs.h.ah = 6;
    inregs.h.al = 0;
    inregs.h.bh = attr;
    inregs.h.ch = y1;
    inregs.h.cl = x1;
    inregs.h.dh = y2;
    inregs.h.dl = x2;
    int86(0X10, &inregs, &outregs);
}
/*
** BIOS call to scroll the window specified by
** x1,y1 and x2,y2 in the direction passed by
** the parameter dir.
*/
void scroll_window (x1,y1,x2,y2,dir) int x1,y1,x2,dir; {
union REGS inregs,outregs;

    inregs.h.ah = dir;
    inregs.h.al = 1;
    inregs.h.bh = 7;
    inregs.h.ch = y1;
    inregs.h.cl = x1;
    inregs.h.dh = y2;
    inregs.h.dl = x2;
    int86(0X10, &inregs, &outregs);

}
/*
** BIOS call to goto point specified by x and y.
*/
void gotoxy( x,y ) int x,y; {
union REGS inregs,outregs;

    inregs.h.ah = 2;
    inregs.h.bh = 0;
    inregs.h.dh = y;           /* column   */
    inregs.h.dl = x;           /* row  */
    int86(0X10, &inregs, &outregs);

}

/*
** BIOS call to obtain the current cursor position on
** the screen.  The actual addresses of where the x and y
** coordinates are to placed are passed as arguments
** to getxy.
*/
void getxy(x,y) int *x,*y; {
union REGS outregs,inregs;

    inregs.h.ah = 3;
    inregs.h.bh = 0;
    int86(0x10,&inregs,&outregs);
    *x = outregs.h.dl;          /* pass the row back */
    *y = outregs.h.dh;          /* pass the column back */
}

/*
 Output a formated string with attributes
 according to the printf formats contained
 in *fstring.  Return the count to assist
 in cursor positioning.
*/

int putfa(x,y,fstring,arg,attr) int x,y,arg,attr; char *fstring; {
int count;
static char    out_buf[128];

     gotoxy(x,y);
     count = sprintf(out_buf,fstring,arg);
     put_string(out_buf,attr);
     return count;
}

/*
 This is a dumb routine which output a string
 at the cursor postion with the attributes
 provided.  It will not check for wrap around etc.
*/

void put_string (sptr,attr) char *sptr; int attr; {
union REGS inregs,outregs;
int x,y;

    getxy(&x,&y);           /* get the current cursor position */
    while ((inregs.h.al = *sptr++) != NULL) {
        inregs.h.ah = 9;
        inregs.h.bh = 0;
        inregs.h.bl = attr;
        inregs.x.cx = 1;
        int86(0x10,&inregs,&outregs);
        gotoxy(++x,y);      /* move the cursor along one position */
    }
}
/*
 Customised version of gets().  Used primarly to obtain
 the command line, thereby avoiding DOS's echo to the screen
 of any key which will muck up the screen layout.  This
 routine also allows customisation of the command line
 re-issued commands etc.  If you  don't like what this
 routine does, then use gets(sptr).
*/

void get_string (sptr) char *sptr; {
int c, count;
static int eolc;            /* count of characters to eol */
char *init_sptr;

    init_sptr = sptr;
    count = 0;
    while ( (c = getch()) != '\r' && count < LINLEN) {
        switch (c) {

            case 0x1b:  continue;       /* trap escape for edit */

            case '\b':  if (sptr > init_sptr) {  /* back space */
                            sptr--; count--;
                            putchar('\b');
                            putchar('\ ');
                            putchar('\b');
                        }
                        continue;

            case '\0':  c = getch();    /* process extended chars */
                        switch (c) {

                            /* F1 => Help */
                            case 59: sptr = init_sptr;
                                     *sptr++ = '?';  /* simulate ? */
                                     *sptr = '\0';
                                     return;

                            /* F3 => re-issue command */
                            case 61: put_string(sptr,0x07);
                                     sptr += eolc;  /* position pointer */
                                     count = eolc;  /* to last position */
                                     continue;      /* edit the command */

                            /* F4 => repeat the previous command */
                            case 62: put_string(sptr,0x07); 
                                     return; 
 
                        } 
                        continue;   /* disregard all the others */ 
 
            default:    count++;    /* assume normal char */ 
                        putchar(c); 
                        *sptr++ = c; 
        } 
    } 
    eolc = count;                   /* keep track of line lenght */ 
    *sptr++ = '\0'; 
 
} 
 
/* 
 Simple routine to output one character with attributes 
 at the current cursor position. 
*/ 
 
void put_char (c,attr) int c,attr; { 
union REGS outregs,inregs; 
 
    inregs.h.al = c; 
    inregs.h.ah = 9; 
    inregs.h.bh = 0;
    inregs.h.bl = attr;
    inregs.x.cx = 1;
    int86(0x10,&inregs,&outregs);
} 
 
 
 
