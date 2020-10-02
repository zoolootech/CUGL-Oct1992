/*
 * g r a f t u r t . c
 * -------------------
 * This module implements the turtle graphics package. Note that this
 * package always operates with its own, global coordinate system. The
 * resulution may by set by the function call turt_res() and is predefined
 * to 801 * 401, which should be a good value for many video devices.
 *
 * Realease history
 * --------------
 * Jun, 20. 1987    Begin implemention.
 *
 * Written by       Rainer Gerhards
 *                  Petronellastr. 6
 *                  D-5112 Baesweiler
 *                  West Germany
 *                  Phone (49) 2401 - 1601
 */

/*
 * Define library implemention mode - forbidden for user!
 */
#define LIB_MODE 1

#include <stdio.h>
#include <dos.h>
#include <math.h>
#ifdef	__TURBOC__
#include <conio.h>
#endif

#include "graphics.h"

/*
 * -------------------------------------------------------------------------
 *                               M A C R O S
 * -------------------------------------------------------------------------
 */
#define last3bit(x) ((x) &  0x0007)
#define exclst3b(x) ((x) & ~0x0007)

#define DIR      turt_dir
#define PEN_DN   turt_pdn
#define PEN_UP   !turt_pdn
#define X        turt_x
#define Y        turt_y
#define XC       convxco((double)turt_x)
#define YC       convyco((double)turt_y)
#define COLOR    turt_clr
#define VISIBLE  turt_vis
#define DELY_TIM turt_del

#define NXT_X(x, cnt, direct)\
	((x) + (float) itocarxc((convxdis(\
				       convxrad(glo_maxx, glo_maxy,(cnt)))),\
		       (direct)))
#define NXT_Y(y, cnt, direct)\
	((y) + (float) itocaryc((convydis((double) (cnt))), (direct)))


/*
 * -------------------------------------------------------------------------
 *                  S T A T I C   D A T A   I T E M S
 * -------------------------------------------------------------------------
 */
static int turt_dir = GTDHOME;		/* turtle direction in degrees	*/


/*
 * -------------------------------------------------------------------------
 *                          F U N C T I O N S
 * -------------------------------------------------------------------------
 */

#ifdef	USEPROTT
	extern void	clr_turt(void);
	extern void	set_turt(void);
#endif

static void clr_turt()
/*
 * name          clr_turt
 *
 * synopsis      clr_turt();
 *
 * description   This function clear the old turtle symbol (if the turtle
 *               is visible).
 *
 * Warning       This function is internal to the turtle system only. It
 *               may be changed or removed without notice!
 */
{
double tx, ty;				/* temporary turtle position	*/
int tdir;				/* temporary direction		*/

if(VISIBLE == TRUE)
	{
	tx = NXT_X(X, (turt_sz/2.0), DIR);
	ty = NXT_Y(Y, (turt_sz/2.0), DIR);
	setwm(WM_INV);
	if(PEN_DN)
	  line(XC, YC, convxco(tx), convyco(ty), WHITE);
	if((tdir = DIR - 135) < 0)
		tdir = 360 + tdir;
	line(convxco(tx), convyco(ty),
	     (int) convxco(NXT_X(tx, -turt_sz, tdir)),
	     (int) convyco(NXT_Y(ty, -turt_sz, tdir)), WHITE
	    );
	if((tdir = DIR + 135) > 360)
		tdir -= 360;
	line(convxco(tx), convyco(ty),
	     convxco(NXT_X(tx, -turt_sz, tdir)),
	     convyco(NXT_Y(ty, -turt_sz, tdir)), WHITE
	    );
	setwm(WM_NORM);
	}
}


static void set_turt()
/*
 * name          set_turt
 *
 * synopsis      set_turt();
 *
 * description   This function redraws the turtle symbol after the turtle
 *               position has changed. If the turtle is invisible, no symbol
 *               is drawn.
 *               This function is also responsible for the turtle delay.
 *
 * Warning       This function is internal to the turtle system only. It
 *               may be changed or removed without notice!
 */
{
clr_turt();
}


void gt_show()
/*
 * name          gt_show
 *
 * synopsis	 gt_show()
 *
 * description	 The turtle is shown after each drawing operation if
 *		 this function is called.
 */
{
clr_turt();
VISIBLE = TRUE;
set_turt();
}


void gt_hide()
/*
 * name          gt_show
 *
 * synopsis	 gt_show()
 *
 * description	 The turtle isn't shown anytime after this function is called.
 */
{
clr_turt();
VISIBLE = FALSE;
set_turt();
}


void gt_forwd(count)
int count;
/*
 * name          gt_forwd
 *
 * synopsis      gt_forwd(count)
 *               int count;      count to move foreward
 *
 * description   This function moves the turtle forward the specfied count
 *               of 'logical' pixel (or backward, if negative).
 */
{
double tx, ty;

clr_turt();  /* erase old turtle */
tx = NXT_X(X, count, DIR);
ty = NXT_Y(Y, count, DIR);
if(PEN_DN)
  line(XC, YC, convxco((double) tx), convyco((double) ty), COLOR);
X = tx;
Y = ty;
set_turt(); /* mark new turtle position */
}


#if USEVOID
void
#endif
gt_backw(count)
int count;
/*
 * name          gt_backw
 *
 * synopsis      gt_backw(count)
 *               int count;      count to move backward
 *
 * description   This function is the opposite to function gt_forwd.
 *               It moves the turtle the specified position in backward
 *               direction.
 *               Internaly this results in a call to function gt_forwd,
 *               and because this is so simple the function is available
 *               as macro too. So you should use arguments without side-
 *               effekt to be as portable as possible.
 */
{
gt_forwd(-count);
}


#if USEVOID
void
#endif
gt_dir(degrees)
int degrees;
/*
 * name          gt_dir
 *
 * synopsis      gt_dir(degrees);
 *               int degrees;      new turtle direction in degrees.
 *
 * description   This function sets the absolute turtle direction in
 *               degrees for further moves. If the value contained in
 *               "degrees" is invalid, it will be converted into the
 *               range 0..359. Negative degrees will be correctly converted
 *               to their positve counterpart.
 */
{
int wasneg = 0;

clr_turt();  /* erase old turtle */
degrees += 90;
if(degrees < 0)
  {
  degrees = -degrees;
  wasneg = -1;
  }
while(degrees > 359)
  degrees -= 360;
DIR = wasneg ? 360 - degrees : degrees;
set_turt(); /* mark new turtle position */
}


#if USEVOID
void
#endif
gt_tleft(degrees)
int degrees;
/*
 * name          gt_tleft
 *
 * synopsis      gt_tleft(degrees);
 *               int degrees;      turtle rotate count in degrees
 *
 * description   This function rotates the turtle in left direction, using
 *               the rotate count specified in degrees. Negative counts mean
 *               rotate to rigth position.
 */
{
gt_dir(DIR + degrees - 90);
}


#if USEVOID
void
#endif
gt_trght(degrees)
int degrees;
/*
 * name          gt_trght
 *
 * synopsis      gt_tleft(degrees);
 *               int degrees;      turtle rotate count in degrees
 *
 * description   This function rotates the turtle in rigth direction, using
 *               the rotate count specified in degrees. Negative counts mean
 *               rotate to left position.
 */
{
gt_tleft(-degrees);
}


#if USEVOID
void
#endif
gt_setco(x, y)
int x, y;
/*
 * name          gt_setco
 *
 * synopsis      gt_setco(x, y);
 *               int x,y;     new turtle coordinates.
 *
 * description   This function sets the turtle to the given position. The
 *               position is a turtle graphics coordinate. No line is draw
 *               while repositioning the turtle.
 */
{
clr_turt();  /* erase old turtle */
X = x;
Y = y;
set_turt(); /* mark new turtle position */
}


int gtgetdir()
/*
 * name          gtgetdir
 *
 * synopsis      degrees = gtgetdir();
 *               int degrees;      current turtle direction in degrees.
 *
 * description   This function returns the current turtle position in degrees.
 *               The returned value will be in range 0 .. 359.
 */
{
int direct;

direct = DIR - 90;
return((direct < 0) ? 360 + direct : direct);
}


#if USEVOID
void
#endif
gt_home()
/*
 * name          gt_home
 *
 * synopsis      gt_home();
 *
 * description   This function sets the turtle to the home position.
 *               After this function call, the turtle will be located
 *               at (0,0) (middle of screen) and points in upward direction.
 */
{
gt_dir(GTDHOME);
gt_setco(GTCO_HX, GTCO_HY);
}


#if USEVOID
void
#endif
gt_penup()
/*
 * name          gt_penup
 *
 * synopsis      gt_penup();
 *
 * description   This function lifts the turtle pen up, so that no line
 *               will be drawn by further movments. The pen may be lift
 *               down by function gt_pendn.
 *
 * Note          It is not reported as error, if the function is called
 *               while the turtle pen is already lift up.
 */
{
clr_turt();  /* erase old turtle */
turt_pdn = GTP_UP;
set_turt(); /* mark new turtle position */
}


#if USEVOID
void
#endif
gt_pendn()
/*
 * name          gt_pendn
 *
 * synopsis      gt_pendn();
 *
 * description   This function lifts the turtle pen down, so that a line
 *               will be drawn by further movments. The pen may be lift
 *               up by function gt_penup.
 *
 * Note          It is not reported as error, if the function is called
 *               while the turtle pen is already lift down.
 */
{
clr_turt();  /* erase old turtle */
turt_pdn = GTP_DOWN;
set_turt(); /* mark new turtle position */
}


#if USEVOID
void
#endif
gt_pcolr(color)
int color;
/*
 * name          gt_pcolr
 *
 * synopsis      gt_pcolr(color);
 *               int color;        new pen color
 *
 * description   This function sets the turtle pen color. All further
 *               drawing will use the specified color.
 *
 * note          The pen status is not modified, so if the pen is lift up,
 *               no drawing will take place.
 *
 */
{
clr_turt();  /* erase old turtle */
COLOR = color;
set_turt(); /* mark new turtle position */
}


void gt_init()
/*
 * name          gt_init
 *
 * synopsis      gt_init();
 *
 * description   This function initializes the turtle-graphics package.
 *               It clears the graphics screen and initializes the global
 *               to local coordinate system with the default turtle window
 *               size. All turtle parameters are reset to their defaults,
 *               which are as follows:
 *               - turtle position                    (0,0)
 *               - turtle direction                    0
 *               - turtle visible                      no
 *               - turtle delay                        none
 *               - turtle window                       -400,200,400,-200
 *               - turtle pen up                       no
 *               - turtle pen color                    1
 *
 * Note          This function MUST be called before any other turtle
 *               graphics function. If it's not called, the other function
 *               may crash. This function MAY be called at any time to reset
 *               the turtle graphics system.
 */
{
setgloco(-400.0, 200.0, 400.0, -200.0);
DIR = GTDHOME;
gt_pendn();
set_turt();
}


#if USEVOID
void
#endif
gt_usrmv(speedtab)
int *speedtab;
/*
 * name          gt_usrmv
 *
 * synopsis      gt_usrmv(speedtab);
 *               int *speedtab;      speed table for moves.
 *
 * description   This function allowes it the program to give the user
 *               control over turtle movment. The function accepts single
 *               keystroke commands. The commands are not echoed.
 *               The caller may supply a table of speed values for each
 *               supported speed (0 .. 9). This table consists of 10
 *               int-type entries, which holds the movement entity in the
 *               selected speed. If the user wishes to use the default
 *               speed table, he must supply a NULL pointer.
 *               Commands available:
 *               0 .. 9 : select turtle speed.
 *               n      : turtle direction north
 *               e      : turtle direction east
 *               w      : turtle direction west
 *               s      : turtle direction south
 *               h      : home turtle
 *               b      : move turtle backward
 *               f      : move turtle forward
 *               l      : turn turtle left 1 degree
 *               L      : turn turtle left 5 degrees
 *               r      : turn turtle right 1 degree
 *               R      : turn turtle right 5 degrees
 *               g      : go, change no direction
 *               q      : quit this function
 *
 * note          The turtle system must be initialized before calling this
 *               function. The current state of the turtle system is not
 *               altered before execution, so all parameters are as set by
 *               you.
 */
{
#define NORTH   'n'
#define EAST    'e'
#define WEST    'w'
#define SOUTH   's'
#define HOME    'h'
#define BACKW   'b'
#define FORWD   'f'
#define LEFT1   'l'
#define LEFT5   'L'
#define RIGHT1  'r'
#define RIGHT5  'R'
#define GO      'g'
#define QUIT    'q'

static int defspeed[] = {   1,         /* 0 */
                            5,         /* 1 */
                           10,         /* 2 */
                           20,         /* 3 */
                           30,         /* 4 */
                           40,         /* 5 */
                           50,         /* 6 */
                           75,         /* 7 */
                          100,         /* 8 */
                          150          /* 9 */
                          }; /* default speeds */
int curspeed;      /* current speed */
int c;             /* input character (command) */
int turt_fwd = 1;  /* move turtle in forward direction? */
int do_move = -1;  /* move at loop end? */

if(speedtab == NULL)
 speedtab = defspeed;
curspeed = *(speedtab + 2);
for(c = getch() ; c != QUIT ; c = getch(), do_move = -1)
  {
  switch(c)
    {
    case '0'      :
    case '1'      :
    case '2'      :
    case '3'      :
    case '4'      :
    case '5'      :
    case '6'      :
    case '7'      :
    case '8'      :
    case '9'      : curspeed = *(speedtab + c - '0');
                    /*
                     * caution: The above conversion is only valid in
                     *          the ASCII alphabet!
                     */
                    do_move = 0;
                    break;
    case NORTH    : gt_dir(GTDNORTH);
                    break;
    case EAST     : gt_dir(GTDEAST);
                    break;
    case WEST     : gt_dir(GTDWEST);
                    break;
    case SOUTH    : gt_dir(GTDSOUTH);
                    break;
    case HOME     : gt_home();
                    break;
    case BACKW    : turt_fwd = 0;
                    break;
    case FORWD    : turt_fwd = 1;
                    break;
    case LEFT1    : gt_tleft(1);
                    do_move = 0;
                    break;
    case LEFT5    : gt_tleft(5);
                    do_move = 0;
                    break;
    case RIGHT1   : gt_trght(1);
                    do_move = 0;
                    break;
    case RIGHT5   : gt_trght(5);
                    do_move = 0;
                    break;
    case GO       : /* nothing to do */
                    break;
    default       : /* unrecogniced command - ignore it */
                    do_move = 0;
                    break;
    }  /* end switch */
  if(do_move)
    gt_forwd(turt_fwd ? curspeed : -curspeed);
  }  /* end main loop (for(..;c != QUIT;..) */
}

/*
 * vi mode lines - do not remove!
 *
 * vi: set noic | set sw=2 :
 */
