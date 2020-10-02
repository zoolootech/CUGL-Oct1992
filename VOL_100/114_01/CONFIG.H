/* header for configuration program -- BDS C version
 *
 * Source:  config.h
 * Version: December 20, 1981.
 */

/* Values of control keys go here */

int up1;		/* insert up key */
int up2;		/* move up key */
int down1;		/* insert down key */
int down2;		/* move down key */
int left;		/* cursor left key */
int right;		/* cursor right key */

int ins;		/* enter insert mode key */
int cmnd;		/* enter command mode key */
int edit;		/* enter edit mode key */

int del;		/* delete character key */
int undo;		/* undo editing key */
int delline;		/* delete line key */

int split;		/* split line key */
int join;		/* join lines key */

/* Video screen  and printer characteristics go here */

int scrnl;		/* number of rows on screen */
int scrnw;		/* # of columns on screen */
int haseol;		/* has erase to end of line */
int hasel;		/* has erase line */
int hassup;		/* has hardware scroll up */
int hassdn;		/* has hardware scroll down */
int lwidth;		/* width of list device */

/* Define array which contains the code that the
 * user gives to do special screen functions.
 */

#define BYTEMAX	1000	/* size of byte array */
char bytes[BYTEMAX];
int bytec;		/* index of next free entry */

/* Define indices into bytes[] which point at start
 * of code for each special screen function.
 */

int gotoind;		/* index to gotoxy code */
int eolind;		/* erase to end of line */
int elind;		/* erase line */
int supind;		/* scroll up */
int sdnind;		/* scroll down */

/* Define return codes */

#define	YES	1	/* all ok */
#define NO	2	/* try again */
#define EXIT	3	/* stop the program */

/* Define special characters */

#define CR	13	/* carriage return */
#define LF	10	/* line feed */
#define	TAB	9	/* tab */

/* define output file buffer */

char	outbuf [BUFSIZ];
aseol;		/* has erase to end of line */
int hasel;		/* has erase line */
int hassup;	