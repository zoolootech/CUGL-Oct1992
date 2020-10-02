/*
 *
 * update history
 * --------------
 * Jun, 5. 1987    function initgraf used wrong port addresses - 0x3b0 changed
 *                 to 0x3b4, 0x3b1 changed to 0x3b5.
 *
 */

#include <stdio.h>
#include <dos.h>

#define NOLLHINC

#include "herc.h"
#include "graphics.h"

#ifdef	USEPROTT
	extern void	inton(void);
	extern void	intoff(void);
#endif

#ifdef USEVOID
void
#endif
initgraf(autocls)
int autocls;          /* clear graphics screen? (YES/NO) */
/**
* name          initgraf()
*
* synopsis      initgraf(autocls);
*               int autocls; / Should the graphics screen be cleared  /
*                            / after mode-switch?                     /
*
* description   This routine switches the card to graphics mode. Page one is
*               used to display the graphic. There is no page-switch
*               supported! The caller may clear the previous graphic or leave
*               it unchanged (variable "autocls").
*
* warning       This function depends on hardware-parameters!
**/
{
static unsigned char init_par[] = {  /* CRT 6845 register values. */
                                  0x35, 0x2d, 0x2e, 0x07,
                                  0x5b, 0x02, 0x57, 0x57,
                                  0x02, 0x03, 0x00
                                  };
int i;
int cnt;

for(cnt = 0 ; cnt < 2 ; ++cnt) /* must be executed twice - I don't know why! */
  {
  if(autocls)           /* clear previous graphic? */
    clrgraph(0);
  intoff();
  outp(CRTPORT, 0);     /* switch off video */
  for(i = 0 ; init_par[i] ; i++)
    {
    outp(0x3b4, i);            /* select register */
    outp(0x3b5, init_par[i]);  /* initialize register */
    }
  outp(MLXPORT, 3);     /* allow graphics-mode */
  outp(CRTPORT, 0xaa);   /* page 1, video */
  inton();
  }
}

#ifdef USEVOID
void
#endif
exitgraf()
/**
* name          exitgraf()
*
* synopsis      exitgraf();
*
* description   This routine switches the card back to text-mode. Bios-
*               services (INT 10h, set mode) are used, so it is not
*               guaranteed, that the graphics page isn't destroyed!
**/
{
union REGS regs;

regs.x.ax = 0007;
int86(0x10, &regs, &regs);   /* set mode 7 (internal b/w mode) */
}

prtgrf()
/**
* name          prtgrf
*
* synopsis      ret = prtgrf();
*               int ret      return -1 if succesful, 0 otherwise
*
* description   This function prints the entire graphics-screen on a
*               dot-matrixprinter. The function was developed using a
*               NEC P6 printer, but should run with litle alteration on
*               every EPSON compatible printer.
*
* warning       This function depends on hardware-parameters!
**/
{
int state;                          /* return value -1 success, 0 failure */
int y;                              /* y-coordinate */
register int yo;                    /* y-offset in mainloop */
int x;                              /* x-coordinate */
register unsigned char prtbyte;     /* printer-byte */
unsigned char buf[720];             /* output buffer */
int lstnon0;                        /* last non-zero byte */
FILE *printer;

state = -1;
if((printer = fopen("PRN", "wb")) == NULL)
  state = 0;
else
  {
  fprintf(printer, "%c%c%c", (char) 0x1b, '3', 24);  /* 24 / 180 inch */
  for(y = 0 ; y <= 340 ; y += 8)    /* Bytewise down */
    {
    lstnon0 = -1;
    for(x = 0 ; x < 720 ; x++)     /* all pixel of one row */
      {
      prtbyte = 0;                 /* reset all pixel */
      for(yo = 0 ; yo < 8 ; yo++)  /* 8 rows per byte */
        prtbyte |= (getpixel(x, y+yo) << (7 - yo)); /* set pixel */
      buf[x] = prtbyte;
      if(prtbyte != 0)
        lstnon0 = x;
      }
    if(lstnon0 != -1)
      {
      lstnon0++; /* now number of bytes, not index */
      fprintf(printer, "%c%c%c%c%c", (char) 0x1b, '*', (char) 6,
             (char) (lstnon0 % 256), (char) (lstnon0 / 256));
      fwrite(buf, sizeof(unsigned char), lstnon0, printer);
      }
    fprintf(printer, "\n\r");
    }
  fprintf(printer, "%c%c", (char) 0x1b, '2');  /* 6 lpi - normal */
  }
fclose(printer);
return(state);
}
