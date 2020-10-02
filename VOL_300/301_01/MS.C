/*
 * Mouse routines adapted from C User's Journal 
 * by Andrew Markley, Sept/Oct 1988, page 88
 *
 * Additional services and interface routines
 * added by Mark Johnson, Oct 1989
 */


#include <dos.h>

#define MOUSE_RESET      0 /* Mouse Service 0                            */
#define MOUSE_ON         1 /* Service 1                                  */
#define MOUSE_OFF        2 /* 2                                          */
#define MOUSE_STATUS     3 /* 3                                          */
#define MOUSE_SET        4 /* 4                                          */
#define MOUSE_PRESSED    5 /* 5                                          */
#define MOUSE_RELEASED   6 /* 6                                          */
#define MOUSE_COLMAX     7 /* 7                                          */
#define MOUSE_ROWMAX     8 /* 8                                          */
#define MOUSE_GIMAGE     9 /* 9                                          */
#define MOUSE_TIMAGE    10 /* 10                                         */
#define MOUSE_MOTION	11 /* 11 - get mouse motion in cx,dx		 */
#define MOUSE_SETSPEED	15 /* 15 - set micky-to-pixel ratio              */

/* Used to test BX after Service 3                                       */
#define LEFTBUTTON       1
#define RIGHTBUTTON      2
#define BOTHBUTTONS      LEFTBUTTON|RIGHTBUTTON

/* Used with Service 5 and 6                                             */
#define LEFT_STATUS      0
#define RIGHT_STATUS     1

/* function declaration and prototyping per ANSI standards               */
int mouse (int service, int *bx_register, int *cx_register, int *dx_register) {

struct REGS inregs, outregs;

    inregs.x.ax = service;
    inregs.x.bx = *bx_register;
    inregs.x.cx = *cx_register;
    inregs.x.dx = *dx_register;
    int86 (51, &inregs, &outregs);
    *bx_register = outregs.x.bx;
    *cx_register = outregs.x.cx;
    *dx_register = outregs.x.dx;
    return (outregs.x.ax);
}

void install_cursor_image (int picture [16][2]) {
struct SREGS segregs;
struct REGPACK regpak;

    segread (&segregs);
    regpak.r_ax = MOUSE_GIMAGE;
    regpak.r_bx = 0;
    regpak.r_cx = 0;
    regpak.r_dx = (int) picture;
    regpak.r_es = segregs.ds;
    intr (51, &regpak);
}

int mouse (int, int *, int *, int *);
void install_cursor_image (int [16][2]);

/* following interface routines added by Mark Johnson */

ms_init() {
	int junk = 0;
	return mouse(MOUSE_RESET, &junk, &junk, &junk);
}

ms_hbounds(xlow, xhigh) {
	int junk = 0;
	return mouse(MOUSE_COLMAX, &junk, &xlow, &xhigh);
}

ms_vbounds(ylow, yhigh) {
	int junk = 0;
	return mouse(MOUSE_ROWMAX, &junk, &ylow, &yhigh);
}

ms_button() {
	int btn, junk = 0;
	mouse(MOUSE_STATUS, &btn, &junk, &junk);
	return btn;
}

ms_motion(xp, yp) int *xp, *yp; {
	int junk;
	return mouse(MOUSE_MOTION, &junk, xp, yp);
}

ms_setspeed(xrat, yrat) {
	int junk;
	return mouse(MOUSE_SETSPEED, &junk, &xrat, &yrat);
}
