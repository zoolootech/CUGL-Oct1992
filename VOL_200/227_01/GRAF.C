/*
 * g r a f . c
 * -----------
 * This is the hardware-independant, high-level part of the compatible
 * graphics library.
 *
 * update history
 * --------------
 * May, 27. 1987    Use extended functionality of the hercules low-level
 *                  driver. This driver now allows to write 8 pixel with
 *                  one function call, what dramatically decraeses execution
 *                  time.
 * May, 30. 1987    Same as above for the ega low-level driver.
 *                  Also stack space and execution time needed by paint
 *                  reduced, theirby removing a undocumented function
 *                  (paintfil).
 * May, 31. 1987    Function "polyline" added; begin use of graphics error
 *                  system.
 * Jul, 25. 1987    Begin implemention on reverse-color functions.
 * Aug, 30. 1987    Function fellipsis implemented.
 *		    Implemention of reverse-color funtions cancelled,
 *		    begin implemention of the more powerful write mode
 *		    system (includes reverse colors).
 * Nov, 14. 1987    wrtpixel function added.
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
#define LIB_MODE 2			/* I am the Master module!	*/

#include <stdio.h>
#include <dos.h>
#include <math.h>
#ifdef	 __TURBOC__
  #include <stdlib.h>
#endif

#include "graphics.h"

#define last3bit(x) ((x) &  0x0007)
#define exclst3b(x) ((x) & ~0x0007)

/*
 * global data-items (the user program may access these items) ...
 * are 'til now (Jun, 20. 1987) defined in "graphics.h"!
 */

/*
 * The following static items are used by the global/local coordinate system.
 */
static float x_start = 0.0;   /* Beginning x coordinate     */
static float x_step  = 1.0;   /* size of a logical x - unit */
static float y_start = 0.0;   /* Beginning y coordinate     */
static float y_step  = 1.0;   /* size of a logical y - unit */

/*
 * function prototypes (if allowed)
 */
#ifdef	USEPROTT
extern void	hline(int, int, int, int);
extern int	paintfil(int, int);
extern int	selcolor(int, int);
extern void	fline(int, int, int, int);
extern void	ffillbox(int, int, int, int);
#endif

/*
 *	Auxiliary function set,
 *	the following functions are only called internally from other
 *	functions within this module. This function set duplicates most
 *	of the main function set functions, with the execpetion that
 *	the functions containd in the auxiliary perform their operation
 *	in the non-standard write modes (writemod != WM_NORM).
 *
 *	This improves speed dramatically, but unfortunatly needs a lot
 *	of memory. If anyone think's that the memory requirements are to
 *	big, please contact me. I then will implement an much smaller (but
 *	also much slower) function set. I don't implement it today, because
 *	I see no reason for it. I hope the CP/M programmers will share this
 *	opinion.
 */

int	selcolor(x, y)
int	x, y;
/*
 * name		selcolor
 *
 * synopsis	color = selcolor(x, y);
 *		int	x, y;		pixel coordinate
 *
 * description	This function selects the drawing color acording to the
 *		current write mode and the requested color.
 *
 * Note		The requested color must be contained in field curcolor!
 */
{
register int	retcolor;		/* selected color		*/

retcolor = getpixel(x, y);
switch(writemod)
	{
	case WM_XOR:	retcolor ^= curcolor;
			break;
	case WM_AND:	retcolor &= curcolor;
			break;
	case WM_OR:	retcolor |= curcolor;
			break;
	case WM_INV:	retcolor = ~retcolor;
			break;
	}
return(retcolor);
}


static void	fline(x1, y1, x2, y2)
int x1, y1;
int x2, y2;
/**
* name          fline
*
* synopsis      line(x1, y1, x2, y2);
*               int x1, y1;            starting coordinate
*               int x2, y2;            ending coordinate
*
* description   This function draws a line of "color" between the
*               starting (x1, y1) and ending (x2, y2) coordinate.
**/
{
register int count;      /* loop counter, highest difference */
register int berr;
int ix, ox, iy, oy;
int zwsp;                /* to swap x2 and y2 */

if(y1 == y2)              /* horizontal line? */
  {
  if(x1 > x2)
    {
    zwsp = x1;
    x1 = x2;
    x2 = zwsp;
    }
  for( ; x1 <= x2 ; ++x1)
      setpixel(x1, y1, selcolor(x1, y1));
  }
else if(x1 == x2)          /* vertikal line? */
  {
  if(y1 > y2)
    {
    zwsp = y1;
    y1 = y2;
    y2 = zwsp;
    }
  for( ; y1 <= y2 ; y1++)
      setpixel(x1, y1, selcolor(x1, y1));
  }
else
  {
  if((x2 -= x1) > 0)  /* x difference */
    ix = ox = 1;      /* add in loop */
  else
    {
    ix = ox = -1;     /* subtract in loop */
    x2 = -x2;         /* use absolute difference! */
    }
  if((y2 -= y1) > 0)  /* y difference */
    iy = oy = 1;      /* add in loop */
  else
    {
    iy = oy = -1;     /* subtract in loop */
    y2 = -y2;         /* use absolute difference! */
    }
  if(x2 >= y2)        /* select inner/outer loop to add/subtract */
    {
    zwsp = x2;        /* swap x2, y2 */
    x2 = y2;
    y2 = zwsp;
    ix =              /* modify x-coordinate in outer   */
    oy = 0;           /* and y-coordinate in inner loop */
    }
  else
    iy =              /* modify y-coordinate in outer   */
    ox = 0;           /* and x-coordinate in inner loop */
  berr = (x2 << 1) - y2;
  count = y2 + 1;
  do   /* main loop, draws line */
    {
    setpixel(x1, y1, selcolor(x1, y1));
    if(berr > 0)
      {
      berr += (x2 - y2) << 1;
      x1 += ix;
      y1 += iy;
      }
    else
      berr += x2 << 1;
    x1 += ox;
    y1 += oy;
    }
  while(--count > 0);  /* warning: do ... while! */
  }
}


static void	ffillbox(x1, y1, x2, y2)
register int x1;
int y1;
int x2, y2;
/**
* name          fillbox
*
* synopsis      fillbox(x1, y1, x2, y2, color);
*               int x1, y1;            upper left corner
*               int x2, y2;            lower right corner
*               int color;             fill color
*
* description   This function fills a given box with the specified color.
*               The box is specified through the upper left (x1, y1) and
*               the lower right (x2, y2) corner.
*               This function is the counterpart to box, which draws the
*               border.
**/
{
int zwsp;				/* to swap coordinates		*/
register int yt;			/* temporary y, inner loop	*/

if(x1 > x2)
  {
  zwsp = x1;
  x1 = x2;
  x2 = zwsp;
  }
if(y1 > y2)
  {
  zwsp = y1;
  y1 = y2;
  y2 = zwsp;
  }
for( ; x1 <= x2 ; ++x1)				/* x loop		*/
  for(yt = y1 ; yt <= y2 ; ++yt)		/* y loop		*/
    setpixel(x1, yt, selcolor(x1, yt));
}


static void fcircle(x, y, radius, aspect)
register int x, y;
int radius;
double aspect;
/**
* name          circle
*
* synopsis      circle(x, y, radius, color, aspect)
*               int x, y;              center coordinate
*               int radius;            circle radius
*               double aspect;         aspect ratio
*
* description   This function draws a circle.
**/
{
long asp;                /* use internal long - its faster */
unsigned int invrad;     /* inverse radius - grows form 0 to radius/2 */
int decision;            /* controls decrement of radius */
int tmp;                 /* temporary to modify desicion */
unsigned long offs;      /* compute y-offset */
unsigned int offsi;      /* result y-offset (faster then long!) */
unsigned int round;      /* used to round (y * asp) / 256 */

asp = (long) (aspect * 256);   /* using long is much faster! */
invrad = 0;
decision = 3 - (radius << 1);
while(invrad <= radius)
  {
  offs = radius * asp;
  round = ((unsigned int) (offs & 0x00000080)) >> 7;
  offsi = (unsigned int) ((offs >> 8) + round); /* divide by 256 (rounded) */
  setpixel(x + invrad, y + offsi, selcolor(x + invrad, y + offsi));
  setpixel(x + invrad, y - offsi, selcolor(x + invrad, y - offsi));
  setpixel(x - invrad, y - offsi, selcolor(x - invrad, y - offsi));
  setpixel(x - invrad, y + offsi, selcolor(x - invrad, y + offsi));
  offs = invrad * asp;
  round = ((unsigned int) (offs & 0x00000080)) >> 7;
  offsi = (unsigned int) ((offs >> 8) + round); /* divide by 256 (rounded) */
  setpixel(x + radius, y + offsi, selcolor(x + invrad, y + offsi));
  setpixel(x + radius, y - offsi, selcolor(x + invrad, y - offsi));
  setpixel(x - radius, y - offsi, selcolor(x - invrad, y - offsi));
  setpixel(x - radius, y + offsi, selcolor(x - invrad, y + offsi));
  if(decision >= 0)  /* decrement radius? */
    {
    tmp = ((invrad - radius) << 2) + 10;
    radius--;
    }
  else
    tmp = (invrad << 2) + 6;
  decision += tmp;
  invrad++;
  }
}


#if 0
#ifdef USEVOID
void
#endif
paint(x, y, paintclr, border)
int x, y;
int paintclr;
int border;
/**
* name          paint
*
* synopsis      paint(x, y, paintclr, border)
*               int x, y;              coordinate of a point within the area
*               int paintclr;          the color used to paint
*               int border;            is the color of the border defining the
*                                      area
*
* description   This function paints an area. The area is defined by a border
*               of a specified color ("border") and the coordinate of one
*               pixel within the area (x, y). The color used to paint is
*               given in "paintclr".
*               This function ueses several subroutines and a recursive
*               algorithm! It's only the "initializer", the main work is
*               performed by the other Routines.
**/
{
pcolor = paintclr;  /* Initialize static variables */
bcolor = border;
#if defined(EGAGRAF)
setewm2();
#endif
paintfil(x, y);     /* start recursive algorithms */
#if defined(EGAGRAF)
rsestdwm();
#endif
}

static paintfil(x, y)
int x, y;
/**
* name          paintfil
*
* synopsis      paintfil(x, y);
*               int x, y;              coordinate of a point to process
*
* description   This function is only internal to paint, and such ist subject
*               to change without notice!
*               This is the main painting algorithm. It's called recursively
*               every new cycle.
**/
{
int x_right;
int x_left;
static int color;     /* red pixelcolor (by getpixel) */

/*
 * name          fillnew
 *
 * synopsis      paintfil(x, y, x_min);
 *               int x, y;
 *               int x_min;
 *
 * description   Select new fill area. This was macro was in the first release
 *               of the library a function. But the whole procedure paint()
 *               need so a lot of stack space, that it is now implemented as
 *               a macro, saving 2 Byte (or 4 Byte in the large model) of
 *               stack space per call. This reduces space requirements dra-
 *               matically. In addition 2 Byte stack space are now freed,
 *               which were previously allocated to the auto variable color,
 *               which is now common to paintfil and fillnew (it's now static,
 *               too).
 *
 * warning       because it's now a macro, there should be no side-effects in
 *               the function arguments!
 */
static int fillnewx;
#if defined(EGAGRAF)
  #define fillnew(x,y,x_min)\
  {\
  fillnewx = (x);\
  while(fillnewx >= (x_min))\
    {\
    color = qgetpix(fillnewx, (y));\
    if((color != pcolor) && (color != bcolor))\
      fillnewx = paintfil(fillnewx, (y));\
    fillnewx--;\
    }\
  }
#else /* EGAGRAF not defined */
  #define fillnew(x,y,x_min)\
  {\
  fillnewx = (x);\
  while(fillnewx >= (x_min))\
    {\
    color = getpixel(fillnewx, (y));\
    if((color != pcolor) && (color != bcolor))\
      fillnewx = paintfil(fillnewx, (y));\
    fillnewx--;\
    }\
  }
#endif

x_right = x_left = x;
#if defined(EGAGRAF)
qsetpix(x, y, pcolor);
#else
setpixel(x, y, pcolor);
#endif
while((++x_right) <= X_HIGH)
  {
  #if defined(EGAGRAF)
  color = qgetpix(x_right, y);
  #else
  color = getpixel(x_right, y);
  #endif
  if((color == pcolor) || (color == bcolor))
    break;  /* exit loop */
  }
x_right--;
while((--x_left) >= X_LOW)
  {
  #if defined(EGAGRAF)
  color = qgetpix(x_left, y);
  #else
  color = getpixel(x_left, y);
  #endif
  if((color == pcolor) || (color == bcolor))
    break;  /* exit loop */
  }
x_left++;
hline(x_left,x_right,y,pcolor);  /* fill area */
if(--y >= Y_LOW)
  fillnew(x_right, y, x_left);
y += 2;
if(y <= Y_HIGH)
  fillnew(x_right, y, x_left);
return(x_left);
#undef fillnew  /* local to paintfil */
}
#endif


#ifdef USEVOID
void
#endif
fstellipsis(x, y, rx, ry, ws, we)
int x, y;
int rx, ry;
int ws, we;
/**
* name          ellipsis
*
* synopsis      ellipsis(x, y, rx, ry, ws, we)
*               int x, y      center coordinate
*               int rx        x - 'radius'
*               int ry        y - 'radius'
*               int ws        begin angle 0..360
*               int we        end  angle 0..360
*
* description   This function draws any sort of ellipsis. It is often called
*               circle, but I think this name should better be reserverd for
*               a function, which only draws a FULL circle (see above).
*               This function may not only draw a circle or any possible
*               ellipsis, it is also capable of drawing only parts of them.
*               This feature is often used in pie-charts. Because of its
*               great flexibility, this function ist much slower than circle.
*               If you only want a full circle (or ellipsis) you should call
*               circle.
**/
{
#define round(x) (int) (x + .5)
register int xt, yt;			/* temporary coordinates	*/
int w;
int drawlin;
float begarc, endarc;
float wb;
float step;

drawlin = 0;
if(rx < 0)
  rx = -rx;
if(ry < 0)
  ry = -ry;
if(ws < 0)
  {
  ws = -ws;
  drawlin = -1;
  }
while(ws > 360)
  ws = ws - 360;
if(we < 0)
  {
  we = -we;
  drawlin = -1;
  }
while(we > 360)
  we = we - 360;
if(ws > we)
  {
  w = ws;
  ws = we;
  we = w;
  }
step = (float) (rx + ry) / 50.0;
begarc = ws * step;			/* beginning angle */
endarc = we * step;			/* ending angle */
step = PI_D_180 / step;			/* compute new step (radiant) */
begarc = step * begarc;			/* beginning angle in radiant */
endarc = step * endarc;			/* ending angle in radiant */
for(wb = begarc ; wb <= endarc ; wb += step)
  {
  xt = round(x + rx * cos(wb));
  yt = round(y - ry * sin(wb));
  setpixel(xt, yt, selcolor(xt, yt));
  }
if(drawlin && !(ws == 0 && we == 360)) /* must draw line? */
  {                                    /* Yes!            */
  xt = round(x + rx * cos(begarc));
  yt = round(y - ry * sin(begarc));
  line(xt, yt, x, y, selcolor(xt, yt));
  xt = round(x + rx * cos(endarc));
  yt = round(y - ry * sin(endarc));
  line(xt, yt, x, y, selcolor(xt, yt));
  }
}


static void ffellipsis(x, y, rx, ry, ws, we)
int x, y;
int rx, ry;
int ws, we;
/*
 * name          fellipsis
 *
 * synopsis      fellipsis(x, y, rx, ry, ws, we, color)
 *               int x, y      center coordinate
 *               int rx        x - 'radius'
 *               int ry        y - 'radius'
 *               int ws        begin angle 0..360
 *               int we        end  angle 0..360
 *
 * description   This function is much like ellipsis(), but it's faster because
 *		 it useses only full angles to draw the ellipsis. For a
 *		 description see under ellipsis.
 */
{
register int	xt, yt;			/* temporary coordinates	*/
int w;
int drawlin;

drawlin = 0;
if(rx < 0)
  rx = -rx;
if(ry < 0)
  ry = -ry;
if(ws < 0)
  {
  ws = -ws;
  drawlin = -1;
  }
while(ws > 360)
  ws = ws - 360;
if(we < 0)
  {
  we = -we;
  drawlin = -1;
  }
while(we > 360)
  we = we - 360;
if(ws > we)
  {
  w = ws;
  ws = we;
  we = w;
  }
for(w = ws ; w <= we ; ++w)
  {
  xt = round(x + rx * intcos(w));
  yt = round(y - ry * intsin(w));
  setpixel(xt, yt, selcolor(xt, yt));
  }
if(drawlin && !(ws == 0 && we == 360)) /* must draw line? */
  {                                    /* Yes!            */
  xt = round(x + rx * intcos(ws));
  yt = round(y - ry * intsin(ws));
  line(xt, yt, x, y, selcolor(xt, yt));
  xt = round(x + rx * intcos(we));
  yt = round(y - ry * intsin(we));
  line(xt, yt, x, y, selcolor(xt, yt));
  }
}
#undef	round


/*
 *	Main function set,
 *	these functions are called by the user. Each function determines for
 *	itself which auxiliary function is to call if writemod != WM_NORM.
 *	All of the following functions are highly optimized.
 */


int		setwm(mode)
WRITEMOD	mode;
/*
 * name		setwm
 *
 * synopsis	setwm(mode);
 *		WRITEMOD mode;			new write mode
 *
 * description	This function sets the library write mode. All write
 *		operations are performed in respect to the current
 *		write mode.
 *		For a list of currently supported write modes see the
 *		definition of WRITEMOD in graphics.h!
 */
{
int	state;

if((mode >= WM_MIN) && (mode <= WM_MAX))
	{
	writemod = mode;
	state = OK;
	}
else
	{
	gr_error = GE_WMWRG;
	state = WRONG;
	}
return(state);
}


void wrtpixel(x, y, color)
register int x, y;
int color;
/*
 * name          wrtpixel
 *
 * synopsis      wrtpixel(x, y, color);
 *               int x, y;              pixel coordinate
 *               int color;             pixel color
 *
 * description   This function sets a pixel in the given color at the
 *	         specified coordinate. The pixel is set in respect to
 *		 the current write mode.
 */
{
curcolor = color;
setpixel(x, y, selcolor(x, y));
}


static void hline(x1, x2, y, color)
int x1;
int x2;
int y;
int color;
/*
 * name          hline
 *
 * synopsis      line(x1, x2, y, color);
 *               int x1;                starting x-coordinate
 *               int x2;                ending x-coordinate
 *               int y;                 line y-coordinate
 *               int color;             line color
 *
 * description   This function draws a horizontal line of "color" between the
 *               starting (x1, y) and ending (x2, y) coordinates.
 *
 * warning       This function is only for internal use. It requires that ega
 *               write mode 2 is set, if the ega device is used. X1 must be
 *               less than or equal to x2.
 */
{
#if defined(HERCGRAF) || defined(EGAGRAF)
  register int xt;      /* used for horizontal, bytewise line-drawing */
#endif

#if defined(HERCGRAF) || defined(EGAGRAF)
if((x2 - x1) >= 10)
  {
  color = (color & 1) ? 0xff : 0x00;   /* set fill byte                   */
  if((xt = last3bit(x1)))              /* x-coordinate on 8 bit boundery? */
    for(xt = 8 - xt ; xt ; ++x1, --xt) /* no - draw before 8-bit boundery */
      #if defined(EGAGRAF)
        qsetpix(x1, y, color);
      #else
        setpixel(x1, y, color);
      #endif
  xt = last3bit(x2) + 1; /* xt contains now the nbr of pixels to draw after */
  x2 = exclst3b(x2) - 8; /* byte-draws are completed. x2 = nbr of byte-draws*/
  for( ; x1 <= x2 ; x1 += 8)  /* perform byte drawing */
    #if defined(EGAGRAF)
      setbyte(x1, y, color, 0xff);  /* fill whole byte */
    #else
      setbyte(x1, y, color);
    #endif
  for( ; xt ; ++x1, --xt)  /* draw after 8-bit boundery */
    #if defined(EGAGRAF)
      qsetpix(x1, y, color);
    #else
      setpixel(x1, y, color);
    #endif
  }
else
  { /* less than 10 bit - use normal algorithm */
#endif
  for( ; x1 <= x2 ; ++x1)
    #if defined(EGAGRAF)
      qsetpix(x1, y, color);
    #else
      setpixel(x1, y, color);
    #endif
#if defined(HERCGRAF) || defined(EGAGRAF)
  }
#endif
}


void line(x1, y1, x2, y2, color)
int x1, y1;
int x2, y2;
int color;
/**
* name          line
*
* synopsis      line(x1, y1, x2, y2, color);
*               int x1, y1;            starting coordinate
*               int x2, y2;            ending coordinate
*               int color;             line color
*
* description   This function draws a line of "color" between the
*               starting (x1, y1) and ending (x2, y2) coordinate.
**/
{
register int count;      /* loop counter, highest difference */
register int berr;
int ix, ox, iy, oy;
int zwsp;                /* to swap x2 and y2 */

if(writemod != WM_NORM)
  {
  curcolor = color;
  fline(x1, y1, x2, y2);
  }
else
  {
  #if defined(EGAGRAF)
  setewm2();
  #endif
  if(y1 == y2)              /* horizontal line? */
    {
    if(x1 > x2)
      {
      zwsp = x1;
      x1 = x2;
      x2 = zwsp;
      }
    hline(x1, x2, y1, color); /* draw the line */
    }
  else if(x1 == x2)          /* vertikal line? */
    {
    if(y1 > y2)
      {
      zwsp = y1;
      y1 = y2;
      y2 = zwsp;
      }
    for( ; y1 <= y2 ; y1++)
      #if defined(EGAGRAF)
	qsetpix(x1, y1, color);
      #else
	setpixel(x1, y1, color);
      #endif
    }
  else
    {
    if((x2 -= x1) > 0)  /* x difference */
      ix = ox = 1;      /* add in loop */
    else
      {
      ix = ox = -1;     /* subtract in loop */
      x2 = -x2;         /* use absolute difference! */
      }
    if((y2 -= y1) > 0)  /* y difference */
      iy = oy = 1;      /* add in loop */
    else
      {
      iy = oy = -1;     /* subtract in loop */
      y2 = -y2;         /* use absolute difference! */
      }
    if(x2 >= y2)        /* select inner/outer loop to add/subtract */
      {
      zwsp = x2;        /* swap x2, y2 */
      x2 = y2;
      y2 = zwsp;
      ix =              /* modify x-coordinate in outer   */
      oy = 0;           /* and y-coordinate in inner loop */
      }
    else
      iy =              /* modify y-coordinate in outer   */
      ox = 0;           /* and x-coordinate in inner loop */
    berr = (x2 << 1) - y2;
    count = y2 + 1;
    do   /* main loop, draws line */
      {
      #if defined(EGAGRAF)
	qsetpix(x1, y1, color);
      #else
	setpixel(x1, y1, color);
      #endif
      if(berr > 0)
	{
	berr += (x2 - y2) << 1;
	x1 += ix;
	y1 += iy;
	}
      else
	berr += x2 << 1;
      x1 += ox;
      y1 += oy;
      }
    while(--count > 0);  /* warning: do ... while! */
    }
  #if defined(EGAGRAF)
  rsestdwm();
  #endif
  }
}


#ifdef USEVOID
void
#endif
box(x1, y1, x2, y2, color)
int x1, y1;
int x2, y2;
int color;
/**
* name          box
*
* synopsis      box(x1, y1, x2, y2, color);
*               int x1, y1;            upper left corner
*               int x2, y2;            lower right corner
*               int color;             border color
*
* description   This function draws a border of "color" covering the
*               given box. The box is specified through the upper
*               left (x1, y1) and to lower right (x2, y2) corner. The
*               box itself isn't modified (e. g. must be cleared explicitly).
**/
{
line(x1, y1, x1, y2, color);  /* right line  */
line(x1, y2, x2, y2, color);     /* bottom line */
line(x2, y2, x2, y1, color);  /* left line   */
line(x1, y1, x2, y1, color);     /* top line    */
}


void fillbox(x1, y1, x2, y2, color)
int x1, y1;
int x2, y2;
int color;
/**
* name          fillbox
*
* synopsis      fillbox(x1, y1, x2, y2, color);
*               int x1, y1;            upper left corner
*               int x2, y2;            lower right corner
*               int color;             fill color
*
* description   This function fills a given box with the specified color.
*               The box is specified through the upper left (x1, y1) and
*               the lower right (x2, y2) corner.
*               This function is the counterpart to box, which draws the
*               border.
**/
{
int zwsp;				/* to swap coordinates		*/	
register int yt;			/* temporary y, inner loop	*/
#if defined(HERCGRAF) || defined(EGAGRAF)
register int xt;			/* temporary x, inner loop	*/
#endif

if(writemod != WM_NORM)
  {
  curcolor = color;
  ffillbox(x1, y1, x2, y2);
  }
else
  {
  #if defined(EGAGRAF)
  setewm2();
  #endif
  if(x1 > x2)
    {
    zwsp = x1;
    x1 = x2;
    x2 = zwsp;
    }
  if(y1 > y2)
    {
    zwsp = y1;
    y1 = y2;
    y2 = zwsp;
    }
  #if defined(HERCGRAF) || defined(EGAGRAF)
  if((x2 - x1) >= 10)
    {
    color = (color & 1) ? 0xff : 0x00;   /* set fill byte                   */
    if((xt = last3bit(x1)))              /* x-coordinate on 8 bit boundery? */
      for(xt = 8 - xt ; xt ; x1++, xt--) /* no - draw before 8-bit boundery */
	for(yt = y1 ; yt <= y2 ; yt++)
	  #if defined(EGAGRAF)
	    qsetpix(x1, yt, color);
	  #else
	    setpixel(x1, yt, color);
	  #endif
    xt = last3bit(x2) + 1; /* xt contains now the nbr of pixels to draw after */
    x2 = exclst3b(x2) - 8; /* byte-draws are completed. x2 = nbr of byte-draws*/
    for( ; x1 <= x2 ; x1 += 8)  /* perform byte drawing */
      for(yt = y1 ; yt <= y2 ; yt++)  /* y loop */
	#if defined(EGAGRAF)
	  setbyte(x1, yt, color, 0xff);  /* set whole byte */
	#else
	  setbyte(x1, yt, color);
	#endif
    for( ; xt ; x1++, xt--)  /* draw after 8-bit boundery */
      for(yt = y1 ; yt <= y2 ; yt++)
	#if defined(EGAGRAF)
	  qsetpix(x1, yt, color);
	#else
	  setpixel(x1, yt, color);
	#endif
    }
  else
    { /* less than 10 bit - use normal algorithm */
  #endif
    for( ; x1 <= x2 ; x1++)    /* x loop */
      for(yt = y1 ; yt <= y2 ; yt++)  /* y loop */
	#if defined(EGAGRAF)
	qsetpix(x1, yt, color);
	#else
	setpixel(x1, yt, color);
	#endif
  #if defined(HERCGRAF) || defined(EGAGRAF)
    }
  #endif
  #if defined(EGAGRAF)
  rsestdwm();
  #endif
  }
}


void	circle(x, y, radius, color, aspect)
register int x, y;
int radius;
int color;
double aspect;
/**
* name          circle
*
* synopsis      circle(x, y, radius, color, aspect)
*               int x, y;              center coordinate
*               int radius;            circle radius
*               int color;             circle color
*               double aspect;         aspect ratio
*
* description   This function draws a circle.
**/
{
long asp;                /* use internal long - its faster */
unsigned int invrad;     /* inverse radius - grows form 0 to radius/2 */
int decision;            /* controls decrement of radius */
int tmp;                 /* temporary to modify desicion */
unsigned long offs;      /* compute y-offset */
unsigned int offsi;      /* result y-offset (faster then long!) */
unsigned int round;      /* used to round (y * asp) / 256 */

if(!radius)		 /* gets improved in next release	*/
	return;
if(writemod != WM_NORM)
  {
  curcolor = color;
  fcircle(x, y, radius, aspect);
  }
else
  {
  #if defined(EGAGRAF)
  setewm2();
  #endif
  asp = (long) (aspect * 256);   /* using long is much faster! */
  invrad = 0;
  decision = 3 - (radius << 1);
  while(invrad <= radius)
    {
    offs = radius * asp;
    round = ((unsigned int) (offs & 0x00000080)) >> 7;
    offsi = (unsigned int) ((offs >> 8) + round); /* divide by 256 (rounded) */
    #if defined(EGAGRAF)
    qsetpix(x + invrad, y + offsi, color);
    qsetpix(x + invrad, y - offsi, color);
    qsetpix(x - invrad, y - offsi, color);
    qsetpix(x - invrad, y + offsi, color);
    #else
    setpixel(x + invrad, y + offsi, color);
    setpixel(x + invrad, y - offsi, color);
    setpixel(x - invrad, y - offsi, color);
    setpixel(x - invrad, y + offsi, color);
    #endif
    offs = invrad * asp;
    round = ((unsigned int) (offs & 0x00000080)) >> 7;
    offsi = (unsigned int) ((offs >> 8) + round); /* divide by 256 (rounded) */
    #if defined(EGAGRAF)
    qsetpix(x + radius, y + offsi, color);
    qsetpix(x + radius, y - offsi, color);
    qsetpix(x - radius, y - offsi, color);
    qsetpix(x - radius, y + offsi, color);
    #else
    setpixel(x + radius, y + offsi, color);
    setpixel(x + radius, y - offsi, color);
    setpixel(x - radius, y - offsi, color);
    setpixel(x - radius, y + offsi, color);
    #endif
    if(decision >= 0)  /* decrement radius? */
      {
      tmp = ((invrad - radius) << 2) + 10;
      radius--;
      }
    else
      tmp = (invrad << 2) + 6;
    decision += tmp;
    invrad++;
    }
  #if defined(EGAGRAF)
  rsestdwm();
  #endif
  }
}


/**
* The following static variables are used by the paint-function as
* a generall communication buffer between her und her subroutines. This
* saves stack-space because of the recursive calls.
**/
static int pcolor;        /* paint color  */
static int bcolor;        /* border color */

void paint(x, y, paintclr, border)
int x, y;
int paintclr;
int border;
/**
* name          paint
*
* synopsis      paint(x, y, paintclr, border)
*               int x, y;              coordinate of a point within the area
*               int paintclr;          the color used to paint
*               int border;            is the color of the border defining the
*                                      area
*
* description   This function paints an area. The area is defined by a border
*               of a specified color ("border") and the coordinate of one
*               pixel within the area (x, y). The color used to paint is
*               given in "paintclr".
*               This function ueses several subroutines and a recursive
*               algorithm! It's only the "initializer", the main work is
*               performed by the other Routines.
**/
{
pcolor = paintclr;  /* Initialize static variables */
bcolor = border;
#if defined(EGAGRAF)
setewm2();
#endif
paintfil(x, y);     /* start recursive algorithms */
#if defined(EGAGRAF)
rsestdwm();
#endif
}

static paintfil(x, y)
int x, y;
/**
* name          paintfil
*
* synopsis      paintfil(x, y);
*               int x, y;              coordinate of a point to process
*
* description   This function is only internal to paint, and such ist subject
*               to change without notice!
*               This is the main painting algorithm. It's called recursively
*               every new cycle.
**/
{
int x_right;
int x_left;
static int color;     /* red pixelcolor (by getpixel) */

/*
 * name          fillnew
 *
 * synopsis      paintfil(x, y, x_min);
 *               int x, y;
 *               int x_min;
 *
 * description   Select new fill area. This was macro was in the first release
 *               of the library a function. But the whole procedure paint()
 *               need so a lot of stack space, that it is now implemented as
 *               a macro, saving 2 Byte (or 4 Byte in the large model) of
 *               stack space per call. This reduces space requirements dra-
 *               matically. In addition 2 Byte stack space are now freed,
 *               which were previously allocated to the auto variable color,
 *               which is now common to paintfil and fillnew (it's now static,
 *               too).
 *
 * warning       because it's now a macro, there should be no side-effects in
 *               the function arguments!
 */
static int fillnewx;
#if defined(EGAGRAF)
  #define fillnew(x,y,x_min)\
  {\
  fillnewx = (x);\
  while(fillnewx >= (x_min))\
    {\
    color = qgetpix(fillnewx, (y));\
    if((color != pcolor) && (color != bcolor))\
      fillnewx = paintfil(fillnewx, (y));\
    fillnewx--;\
    }\
  }
#else /* EGAGRAF not defined */
  #define fillnew(x,y,x_min)\
  {\
  fillnewx = (x);\
  while(fillnewx >= (x_min))\
    {\
    color = getpixel(fillnewx, (y));\
    if((color != pcolor) && (color != bcolor))\
      fillnewx = paintfil(fillnewx, (y));\
    fillnewx--;\
    }\
  }
#endif

x_right = x_left = x;
#if defined(EGAGRAF)
qsetpix(x, y, pcolor);
#else
setpixel(x, y, pcolor);
#endif
while((++x_right) <= X_HIGH)
  {
  #if defined(EGAGRAF)
  color = qgetpix(x_right, y);
  #else
  color = getpixel(x_right, y);
  #endif
  if((color == pcolor) || (color == bcolor))
    break;  /* exit loop */
  }
x_right--;
while((--x_left) >= X_LOW)
  {
  #if defined(EGAGRAF)
  color = qgetpix(x_left, y);
  #else
  color = getpixel(x_left, y);
  #endif
  if((color == pcolor) || (color == bcolor))
    break;  /* exit loop */
  }
x_left++;
hline(x_left,x_right,y,pcolor);  /* fill area */
if(--y >= Y_LOW)
  fillnew(x_right, y, x_left);
y += 2;
if(y <= Y_HIGH)
  fillnew(x_right, y, x_left);
return(x_left);
#undef fillnew  /* local to paintfil */
}


#ifdef USEVOID
void
#endif
ellipsis(x, y, rx, ry, ws, we, color)
int x, y;
int rx, ry;
int ws, we;
int color;
/**
* name          ellipsis
*
* synopsis      ellipsis(x, y, rx, ry, ws, we, color)
*               int x, y      center coordinate
*               int rx        x - 'radius'
*               int ry        y - 'radius'
*               int ws        begin angle 0..360
*               int we        end  angle 0..360
*               int color     line color
*
* description   This function draws any sort of ellipsis. It is often called
*               circle, but I think this name should better be reserverd for
*               a function, which only draws a FULL circle (see above).
*               This function may not only draw a circle or any possible
*               ellipsis, it is also capable of drawing only parts of them.
*               This feature is often used in pie-charts. Because of its
*               great flexibility, this function ist much slower than circle.
*               If you only want a full circle (or ellipsis) you should call
*               circle.
**/
{
#define round(x) (int) (x + .5)
int w;
int drawlin;
float begarc, endarc;
float wb;
float step;

drawlin = 0;
if(rx < 0)
  rx = -rx;
if(ry < 0)
  ry = -ry;
if(ws < 0)
  {
  ws = -ws;
  drawlin = -1;
  }
while(ws > 360)
  ws = ws - 360;
if(we < 0)
  {
  we = -we;
  drawlin = -1;
  }
while(we > 360)
  we = we - 360;
if(ws > we)
  {
  w = ws;
  ws = we;
  we = w;
  }
step = (float) (rx + ry) / 50.0;
begarc = ws * step;        /* beginning angle */
endarc = we * step;        /* ending angle */
step = PI_D_180 / step;   /* compute new step (radiant) */
begarc = step * begarc;    /* beginning angle in radiant */
endarc = step * endarc;    /* ending angle in radiant */
for(wb = begarc ; wb <= endarc ; wb += step)
  setpixel(round(x + rx * cos(wb)), round(y - ry * sin(wb)), color);
if(drawlin && !(ws == 0 && we == 360)) /* must draw line? */
  {                                    /* Yes!            */
  line(round(x + rx * cos(begarc)), round(y - ry * sin(begarc)), x, y, color);
  line(round(x + rx * cos(endarc)), round(y - ry * sin(endarc)), x, y, color);
  }
}


#ifdef USEVOID
void
#endif
fellipsis(x, y, rx, ry, ws, we, color)
int x, y;
int rx, ry;
int ws, we;
int color;
/*
 * name          fellipsis
 *
 * synopsis      fellipsis(x, y, rx, ry, ws, we, color)
 *               int x, y      center coordinate
 *               int rx        x - 'radius'
 *               int ry        y - 'radius'
 *               int ws        begin angle 0..360
 *               int we        end  angle 0..360
 *               int color     line color
 *
 * description   This function is much like ellipsis(), but it's faster because
 *		 it useses only full angles to draw the ellipsis. For a
 *		 description see under ellipsis.
 */
{
register int w;
int drawlin;

drawlin = 0;
if(rx < 0)
  rx = -rx;
if(ry < 0)
  ry = -ry;
if(ws < 0)
  {
  ws = -ws;
  drawlin = -1;
  }
while(ws > 360)
  ws = ws - 360;
if(we < 0)
  {
  we = -we;
  drawlin = -1;
  }
while(we > 360)
  we = we - 360;
if(ws > we)
  {
  w = ws;
  ws = we;
  we = w;
  }
for(w = ws ; w <= we ; ++w)
  setpixel(round(x + rx * intcos(w)), round(y - ry * intsin(w)), color);
if(drawlin && !(ws == 0 && we == 360)) /* must draw line? */
  {                                    /* Yes!            */
  line(round(x + rx * intcos(ws)), round(y - ry * intsin(ws)), x, y, color);
  line(round(x + rx * intcos(we)), round(y - ry * intsin(we)), x, y, color);
  }
}



#ifdef USEVOID
void
#endif
setgloco(x_beg, y_beg, x_end, y_end)
double x_beg, y_beg;
double x_end, y_end;
/**
* name          setgloco
*
* synopsis      setgloco(x_beg, y_beg, x_end, y_end)
*               double x_beg, y_beg    minimum coordinates values
*               double x_end, y_end    maximum coordinates values
*
* description   This function initializes the global/local coordinate
*               system. This system allows you to adress your pixels based
*               on a global coordinate system. This system is independet
*               from the hardware coordinate system. Global coordinates
*               may be converted to local coordinates, wich are to be used
*               to adress the hardware.
*               So your application hasn't to look at the present video
*               hardware but use always a hardware independent own coordinate
*               system.
*               In addition you may use floats, not only integers as
*               coordinates. This is a great advantage in numerical
*               applications.
* caution       This function initilizes the system, so any call to the
*               convert routines will return grabbage, until this funtion
*               is called!
**/
{
float t;           /* temporary storage   */

x_start = x_beg;  /* set static variables */
y_start = y_beg;
t = x_end - x_start;
glo_maxx = ((t < 0) ? -t : t) + 1.5;      /* compute maximum x resoultion */
x_step = (float) MAX_X / (t + ((t < 0) ? -1 : 1)); /* compute x step-rate */
t = y_end - y_start;
glo_maxy = ((t < 0) ? -t : t) + 1.5;      /* compute maximum y resoultion */
y_step = (float) MAX_Y / (t + ((t < 0) ? -1 : 1)); /* compute y step-rate */
}


convxco(x)
double x;
/**
* name          convxco
*
* synopsis      locx = convxco(x)
*               double x     x coordinate to convert
*               int locx     hardware x coordinate
*
* description   This function converts a global coordinate to a local,
*               hardware dependent coordinate. Its return value may be
*               directly passed to other functions.
**/
{
return((int) ((x - x_start) * x_step + .5));
}


convyco(y)
double y;
/**
* name          convyco
*
* synopsis      locy = convyco(y)
*               double y     y coordinate to convert
*               int locy     hardware y coordinate
*
* description   This function converts a global coordinate to a local,
*               hardware dependent coordinate. Its return value may be
*               directly passed to other functions.
**/
{
return((int) ((y - y_start) * y_step + .5));
}


convxdis(xdis)
double xdis;
/**
* name          convxdis
*
* synopsis      locxdist = convxdis(xdis)
*               double xdis  distance to convert
*               int locxdist hardware distance
*
* description   This function converts a global x distance to a local,
*               hardware dependent x distance. Its return value may be
*               directly passed to other functions.
**/
{
return(iabs((int) (x_step * xdis + .5)));
}


convydis(ydis)
double ydis;
/**
* name          convydis
*
* synopsis      locydist = convydis(ydis)
*               double ydis  distance to convert
*               int locydist hardware distance
*
* description   This function converts a global y distance to a local,
*               hardware dependent y distance. Its return value may be
*               directly passed to other functions.
**/
{
return(iabs((int) (y_step * ydis +.5)));
}


polyline(color, coords)
int color;
int *coords;
/*
 * name          polyline
 *
 * synopsis      ret = polyline(color, coords, ...)
 *               int ret;     0 on success, -1 otherwise.
 *               int color    color of the line
 *               int coords   coordinate list (x, y), ends with -1, -1
 *
 * description   This function draws a polyline. That means a line, which
 *               is made of multiple coordinates. You supply as many
 *               coordinates as you need to describe the line in an integer
 *               array. They must be in (x,y) order. The end of the coordinate
 *               list is indicated by the coordinate pair (-1,-1).
 *               The function starts now at the first coordinate and draws a
 *               line from this point to the second coordinate. From there it
 *               draws a line to the third coordinate and so on, until the end
 *               of coordinate list is reached. If the coordinate list is in-
 *               valid, the function returns a true (-1) result, otherwise
 *               it returns false (0). In case of an error return the graphics
 *               error variable may be examined.
 *               The line is drawn in color "color".
 */
{
int lastx, lasty;
#define ENDLIST ((*coords == -1) && (*(coords + 1) == -1))

gr_error = GE_OK;
if(!ENDLIST)
  {
  lastx = *coords;
  lasty = *(++coords);
  for(coords++ ; !ENDLIST ; coords++)
    {
    line(lastx, lasty, *coords, *(coords + 1), color);
    lastx = *coords;
    lasty = *(++coords);
    }
  }
else
  gr_error = GE_NOCO;
#undef ENDLIST
return((gr_error == GE_OK) ? 0 : -1);
}

/*
 * vi - mode line, please do not remove them
 *
 * vi: set sw=2 | set noic :
 */
