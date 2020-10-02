/* C_WDEF.INC - Include file for C_WINDOW Vers 1.0 */


#define FALSE   0
#define TRUE    1

/* Window border types sent to w_open() */
#define SPACES  0       /* blanks */
#define SNGL_LN 1       /* single lines */
#define DBL_LN  2       /* double lines */
#define MXD_1   3       /* double horiz, single vert */
#define MXD_2   4       /* double vert, single horiz */
#define BLK_LN  5       /* block characters */

/* Monochrome defines */
#define NORM    0x07    /* std. white on black */
#define REVERS  0x70    /* reverse video */
/* Character display attributes, to be */
/* added or ORed to the above base modes */
#define BRIGHT  0x08
#define BLINK   0x80

/* Color screen foreground defines */
#define FG_BLACK        0x00
#define FG_BLUE         0x01
#define FG_GREEN        0x02
#define FG_CYAN         0x03
#define FG_RED          0x04
#define FG_MAGENTA      0x05
#define FG_BROWN        0x06
#define FG_WHITE        0x07
#define FG_GRAY         0x08
#define FG_LTBLUE       0x09    /* light blue */
#define FG_LTGREEN      0x0A    /* light green */
#define FG_LTCYAN       0x0B    /* light cyan */
#define FG_LTRED        0x0C    /* light red */
#define FG_LTMAGENTA    0x0D    /* light magenta */
#define FG_YELLOW       0x0E
#define FG_BRTWHITE     0x0F    /* bright white */
/* Color screen background defines to be ORed to */
/* the above foreground colors */
#define BG_BLACK        0x00
#define BG_BLUE         0x10
#define BG_GREEN        0x20
#define BG_CYAN         0x30
#define BG_RED          0x40
#define BG_MAGENTA      0x50
#define BG_BROWN        0x60
#define BG_WHITE        0x70
/* Color screen blink attribute to be ORed to the */
/* above foreground colors */
#define COLRBLINK       0x80

/* Direction of window scroll */
#define UP      0
#define DOWN    1

/* Border parameter sent to w_def () */
#define NOBORDER 0
#define BORDER  1

/* Parameter sent to w_open () */
#define NOCLR   0       /* No window pre-clear */
#define CLR     1       /* Pre-clear window */

/* Allow access to C_WINDOW functions */
extern  w_init ();
extern  w_def ();
extern  w_open ();
extern  w_close ();
extern  w_kill ();
extern  w_write ();
extern  w_dsp1 ();
extern  w_getstr ();
extern  w_scroll ();
extern  w_getcx ();
extern  w_getcy ();
extern  w_gotoxy ();

/* Non-windowing specific */
extern  border ();
extern  keyin ();
extern  beep ();

/* Flag altered by keyin () */
extern  char extend;

/* Flag used in error handling */
extern  char err_exit; 

ow, plus the section of the screen that each
	window overlays.  The window buffer size has been defined in the
	C_