/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_MAIN.C   ***************************/

#include "mydef.h"
#include <dos.h>
#include <string.h>

/* define our external variables */
/* window definitions */

struct screen_structure scr;
struct window_structure w[MAX_WINDOW+1];
struct help_structure hlp;

int main(int argc,char *argv[])
{
extern struct screen_structure scr;
extern struct window_structure w[];
extern struct help_structure hlp;

  int i;
  unsigned int seg,off;
  long l;
  char string[10];
  int return_code;

set_screen_attr();      /* set screen attributes */
test_dv();              /* check for DesqView */

/* extract segment and offset of screen buffer */
seg=FP_SEG(scr.buffer); off=FP_OFF(scr.buffer);

/* check command line values for override values */
 if(argc>1)
 for(i=1;i<=argc;i++){

   copy(argv[i],string,2,4); /* copy any values. i.e. r=30 */
                             /* note: copy() is found in l_copy.c */
   /* check for segment override */
   if ( toupper(argv[i][0])=='S') if(atoi(string)) seg=atoi(string);

   /* check for offset override */
   if ( toupper(argv[i][0])=='O') if(atoi(string)) off=atoi(string);

   /* check for row override */
   if ( toupper(argv[i][0])=='R')
    if(atoi(string)) scr.rows=atoi(string);

   /* chec for column override */
   if ( toupper(argv[i][0])=='C')
    if(atoi(string)) scr.columns=atoi(string);

   /* check for black & white override */
   if ( toupper(argv[i][0])=='B') set_mode(BW_80);

   /* check for snow override (turn off snow avoidance)  */
   if ( toupper(argv[i][0])=='N') scr.snow=FALSE;
 }

scr.buffer=MAKE_FP(seg,off);

init_window();          /* initialize window structure */
update_margins();
                        /* initialize help structure */
hlp.filename[0]='\0';   /* empty string (no current help file) */

/* set text which appears in window frame */
strcpy(hlp.message,"Esc: to exit ");  
hlp.page=0;                   /* page zero */
hlp.x=1;                      /* upper left column of help window */
hlp.y=1;                      /* upper left row of help window */
hlp.interior_attr=scr.normal; /* set window interior to normal */
hlp.frame_attr=scr.normal;    /* set window frame to normal */

return_code=start();
return(return_code);
}


static void init_window(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];
int i;

/* initialize initial window (desk top) */
  w[0].frame=FALSE;
  wherexy(&w[0].x,&w[0].y); /* save current cursor location*/
  what_cursor(&w[0].start,&w[0].end);
  w[0].left=1;w[0].right=scr.columns;
  w[0].top=1;w[0].bottom=scr.rows;
  w[0].attribute=scr.normal;  /* assume default normal attribute */

 /* initialize window pointer array */

 for(i=0;i<=MAX_WINDOW;i++){
  scr.list[i]=i;
 }
}


/*****************************************************************

 Usage: static void set_screen_attr(void);

 Ascertains what type of graphics card is in use, sets external
 text attributes and screen buffer address according.

*****************************************************************/

static void set_screen_attr(void)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

 what_mode(&scr.mode,&scr.page);  /*set current textmode and page */

 if(scr.mode==MONOCHROME) scr.mode = MONOCHROME;

 if(scr.mode==BW_40){      /* if 40 column BW set to 80 column */
  set_mode(BW_80);
  scr.mode =BW_80;
 }

 if(scr.mode==COLOR_40){   /* if 40 column color, set to 80 column */
       set_mode(COLOR_80);
       scr.mode=COLOR_80;
 }

/* set character attributes */

  if (scr.mode==BW_80){             /* set attributes for BW */
    scr.normal=  set_color(WHITE,BLACK);
    scr.inverse= set_color(BLACK,WHITE);
   }

  if (scr.mode==COLOR_80){           /* set attributes for COLOR */
    scr.normal=  set_color(WHITE,BLACK);
    scr.inverse= set_color(BLACK,WHITE);
   }

   if (scr.mode==MONOCHROME){    /* set attributes for MONOCHROME */
    scr.normal = NORMAL;
    scr.inverse = INVERSE;
   }

    scr.current = scr.normal;   /* set screen attribute to normal */

 /* set screen buffer address */
 if(scr.mode==MONOCHROME)
    scr.buffer= (char far *)0xb0000000;    /* monochrome address */
   else
     scr.buffer=(char far *)0xb8000000;    /* color address */

  scr.rows=25;scr.columns=80;  /* assume standard values for now */
  scr.update=TRUE;
  scr.bold_caps=FALSE;
  scr.ptr=0;scr.active=0;

  /* set snow flag if not ega and not monochrome */
  scr.snow=  (!test_ega() && (scr.mode!=MONOCHROME));

  /* set screen margins */
  scr.top=1;
  scr.bottom=scr.rows;
  scr.left=1;
  scr.right=scr.columns;

}


#if defined TURBOC

static void test_dv(void)
{
extern struct  screen_structure scr;
int seg,off;

   struct REGPACK regs;

   seg = FP_SEG(scr.buffer);
   off = FP_OFF(scr.buffer);

   regs.r_ax=0xfe00;

   regs.r_es= seg;
   regs.r_di= off;

   intr(0x10, &regs);

 if(regs.r_es != seg || regs.r_di != off)
 scr.buffer=MAKE_FP(regs.r_es,regs.r_di);
}
#endif


#if defined QUICKC

static void test_dv(void)
{
extern struct  screen_structure scr;
int seg,off;

union REGS inregs,outregs;
struct SREGS segregs;

  seg = FP_SEG(scr.buffer);
  off = FP_OFF(scr.buffer);

   inregs.h.ah=0xfe;
   segregs.es= seg;
   inregs.x.di= off;

   int86x(0x10, &inregs,&outregs,&segregs);

 if(segregs.es != seg || inregs.x.di != off)
  scr.buffer=MAKE_FP(segregs.es,inregs.x.di);

}
#endif


int test_ega(void)
{
 union REGS reg;

 reg.h.ah = 0X12;         /* call an ega function */
 reg.h.bl = 0X10;
 reg.x.cx = 0Xffff;       /* stuff the register */

 int86(0X10, &reg, &reg);
  if (reg.x.cx==0xffff) return (0); /* if it isn't changed, no ega */
    else
     return(1); /* contents changed, there is a ega */
}


void update_margins(void)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

 scr.top= w[scr.active].top;
 scr.bottom= w[scr.active].bottom;
 scr.left= w[scr.active].left;
 scr.right= w[scr.active].right;
}
