/*
 *	tekdecode.c - converts Tek4014 picture files 
 *	copyright 1988 Ronald Florence 
 *
 *	decoding algorithm from J. Tenenbaum (2/24/87)
 *	alpha mode (2/21/88)
 *	linetypes (5/12/88)
 *	increment mode (5/23/88)
 *	corrected algorithm (6/30/88)
 */

#include        <stdio.h>
#include	"tek4.h"

#define US      0x1f
#define RS	0x1e
#define GS      0x1d
#define FS      0x1c
#define ESC	0x1b
#define BS      0x8
#define HT      0x9
#define LF      0xa
#define VT      0xb

#define NONE		0
#define VECTOR  	1
#define POINT   	2
#define ALPHA   	3
#define INCREMENT	4

#define Low_Y		0x1
#define Hi_bits		0x2
#define Hi_mask		~0x20
#define Lox_mask	~0x40
#define Loy_mask	~0x60
#define CTRL		0
#define HI		1
#define LOX		2
#define LOY		3

static	int	charht[4] = { 36, 40, 61, 66 },
		charwd[4] = { 24, 26, 40, 44 },
		cellw,			/* current width of a character cell */
		chsize = 3; 		/* font */


tekdecode (infile, opt) 
     FILE	*infile;
     int	opt;
{
  register	c, mode; 
  int	pts[Maxpts * 2], pn, got = 0, escape = 0, hisave, 
       	tx, ty, lox, loy, hix, hiy, devx, devy;
  char	gstring[256], *g;

  mode = NONE;
  Align(Left, Base);
  Set_charsize(chsize);
  while ((c = getc(infile)) != EOF) 
    {
      c &= 0x7f;
      if (escape) 
	{
	  parse(c);
	  escape = 0;
	  continue;
	}
      if (c >> 5 == CTRL)
	{
	  if (c == '\0')	/* they pad, we strip */
	    continue;
	  if (mode == ALPHA) 
	    {
	      *g = '\0';
	      if (g > gstring)
		Wr_str(devx, devy, gstring);
	      devx += (g - gstring) * cellw;
	      mode = NONE;
	    }
	  if (mode == INCREMENT) 
	    {
	      tx *= xm;
	      ty *= ym;
	      Increment(tx, ty);
	    }
	  if (mode == VECTOR && pn)
	    Vector(pn, pts);
	  if (mode == POINT && pn) 
	    Marker(pn, pts);
	 
	  switch (c) 
	    {
	    case BS:		/* cursor motions */
	    case HT:		/* we'll do nothing */
	    case VT:
	      break;
	    case GS:
	      mode = VECTOR;
	      pn = 0;
	      break;
	    case FS:
	      mode = POINT;
	      pn = 0;
	      break;
	    case US:
	      g = gstring;
	      mode = ALPHA;
	      break;
	    case ESC :
	      ++escape;
	      break;
	    case RS :
	      mode = INCREMENT;
	      tx = 0;
	      ty = 0;
	      break;
	    case LF:		/* clear bypass */
	      mode = NONE;
	      break;
	    }
	  continue;
	}
				/* else not a control char */
      switch (mode) 
	{
	case NONE:
	  continue;
	case VECTOR:
	case POINT :
	  switch (c >> 5)
	    {
	    case HI:
	      hisave = c & Hi_mask;
	      got |= Hi_bits;
	      break;
	    case LOY:
	      loy = c & Loy_mask;
	      got |= Low_Y;
	      if (got & Hi_bits) 
		{
		  hiy = hisave;
		  got &= ~Hi_bits;
		}
	      break;
	    case LOX:
	      lox = c & Lox_mask;
	      if (got == (Hi_bits + Low_Y))
		hix = hisave;
	      else if (got & Hi_bits)
		hiy = hisave;
	      got &= ~(Hi_bits | Low_Y);
				/* process the coordinates */
	      tx = ((hix << 5) + lox) << 2;
	      ty = ((hiy << 5) + loy) << 2;
	      devx = tx * xm + xscale;
	      devy = ty * ym + yscale;
	      pts[pn++] = devx;
	      pts[pn++] = devy;
	      if (pn > Maxpts * 2)
		err("data");
	      break;
	    }
	  break;
	case ALPHA:
	  *g++ = c;
	  break;
	case INCREMENT:
	  switch (c) 
	    {
	    case ' ':
	      break;
	    case 'D':		/* north */
	      ++ty;
	      break;
	    case 'H':		/* south */
	      --ty;
	      break;
	    case 'A':		/* east */
	      ++tx;
	      break;
	    case 'B':		/* west */
	      --tx;
	      break;
	    case 'E':		/* northeast */
	      ++ty;
	      ++tx;
	      break;
	    case 'J':		/* southwest */
	      --ty;
	      --tx;
	      break;
	    }
	  break;
	}
    }
}


parse(inch)
     int  inch;
{
  int	ltype = NONE;

  switch (inch) 
    {
    case '\f':
      Clear_scr();
      break;
    case '`' : 
    case 'e' :
    case 'f' :
    case 'g' :
    case 'h' :
    case 'm' :
    case 'n' :
    case 'o' :
      ltype = Solid;
      break;
    case 'a' :
    case 'i' :
      ltype = Dotted;
      break;
    case 'b' :
    case 'j' :
      ltype = Dotdash;
      break;
    case 'c' :
    case 'k' :
      ltype = Shortdash;
      break;
    case 'd' :
    case 'l' :
      ltype = Longdash;
      break;
    case '8' :
    case '9' :
    case ':' :
    case ';' :
      Set_charsize (chsize = ';' - inch);
      break;
    }
  if (ltype > 0)
    Set_line(ltype);
}
