#include <stdio.h>
#include <dos.h>

#define NOLLHINC

#include "bios.h"
#include "graphics.h"

int gpage;                                  /* current graphics page */

#ifdef USEVOID
void
#endif
setmode(mode)
int mode;
/**
* name          setmode
*
* synopsis      setmode(mode);
*               int mode           video-mode
*
* description   This routine switches to the given video-mode.
*               For valid values see the BIOS video mode tables.
**/
{
union REGS regs;

regs.h.ah = 0;     /* action: set mode */
regs.h.al = mode;
int86(0x10, &regs, &regs);
gpage = 0;         /* currently graphics page zero! */
}


#ifdef USEVOID
void
#endif
setpixel(x, y, color)
int x, y;
int color;
/**
* name          setpixel
*
* synopsis      setpixel(x, y, color);
*               int x, y
*               int color
*
* description   This routine is used to set the pixel (x, y) to the
*               specified color. For valid colorvalues see the BIOS-tables.
**/
{
union REGS regs;

regs.h.ah = 0xc;     /* action: set pixel */
regs.h.al = color;
regs.h.bh = gpage;
regs.x.cx = x;
regs.x.dx = y;
int86(0x10, &regs, &regs);
}


getpixel(x, y)
int x, y;
/**
* name          getpixel
*
* synopsis      getpixel(x, y);
*               int x, y
*
* description   This routine reads the color of the specified pixel.
*               The color is returned as function return value.
**/
{
union REGS regs;

regs.h.ah = 0xd;     /* action: get pixel */
regs.h.bh = gpage;
regs.x.cx = x;
regs.x.dx = y;
int86(0x10, &regs, &regs);
return((int) regs.h.al);
}


#ifdef USEVOID
void
#endif
selpage(pgnr)
int pgnr;
/**
* name          selpage
*
* synopsis      selpage(pgnr);
*               int pgnr
*
* description   This routine allows the selection of the active video page.
*               Note that the number of available pages depends on the
*               video mode.
**/
{
union REGS regs;

regs.h.ah = 5;       /* action: select active page */
regs.h.al = pgnr;
int86(0x10, &regs, &regs);
gpage = pgnr;
}


#ifdef USEVOID
void
#endif
clrgraph(dummy)
int dummy;
/**
* name          clrgraph
*
* synopsis      clrgraph(dummy)
*               int dummy   no meaning, only for compitibility with herc-lib
*
* description   This routine clears the current graphics-page. Because there
*               is no "graphics clear screen", it first get the actuall video
*               mode and then switches back to that mode. The mode switch
*               includes a clear-screen.
*               The current version of this routine clears always video page
*               zero - if it is the current page or not!
*               The dummy argument is included only for compatibility with the
*               hercules-modules.
**/
{
union REGS regs;

regs.h.ah = 0x0f;        /* action: read current video mode */
int86(0x10, &regs, &regs);
regs.h.ah = 0x00;        /* action: set video mode (and clear screen!) */
int86(0x10, &regs, &regs);
if(gpage)                /* current page other than page zero? */
  {                      /* Yes, must switch! */
  regs.h.ah = 0x05;      /* action: select vido page */
  regs.h.al = gpage;     /* libraries page */
  int86(0x10, &regs, &regs);
  }
}
