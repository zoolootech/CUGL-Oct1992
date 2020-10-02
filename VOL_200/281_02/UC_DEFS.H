/*   uc_defs.h
 *
 *   This header provides the defines required for unicorn functions.
 *
 */

/*----------------------------------------------------------------------*/
/*  Attribute defines							*/
/*----------------------------------------------------------------------*/

#define BLACK	     0
#define BLUE	     1
#define GREEN	     2
#define RED	     4
#define CYAN	     (BLUE+GREEN)
#define MAGENTA      (BLUE+RED)
#define BROWN	     (RED+GREEN)
#define WHITE	     (BLUE+RED+GREEN)

#define BRIGHT	     8
#define LIGHT	     8
#define DIM	     0
#define BLINK	     16

#define NORMAL	     7
#define UNDERLINE    1
#define REVERSE      0x70

/*---------------------------------------------------------------------*/
/*     Video contoller port addresses				       */
/*---------------------------------------------------------------------*/

#define MONO_PORT    0x03b8
#define COLOR_PORT   0x03d8


/*----------------------------------------------------------------------*/
/* The following section contains defines used for windowing functions. */
/*----------------------------------------------------------------------*/

#define v_address(row,col) ((row*NUM_COLS*2+(col)*2)

/*--------------------------------------------------------------------*/
/* Defines for Unicorn Bar Menu System				      */
/*--------------------------------------------------------------------*/

#define NORMAL_VID    0x01700
#define HIGHLITE_VID  0x07000


#define ENTER	      13
#define SPACE	      32
#define ESC	      27

#define TRUE	       1
#define FALSE	       0

#define RIGHT_ARROW   77 + 'z'
#define LEFT_ARROW    75 + 'z'

#define MAXBMENUITEMS 25
#define NAME_ROW       2
#define NAME_COL       1
#define DESCRIPTION_ROW 3
#define DESCRIPTION_COL 2


typedef struct
{
   int row;
   int col;
   int start;
   int stop;
} CURSOR, *CURSORPTR;


/*--------------------------------------------------------------------*/
/*  Timer definitions						      */
/*--------------------------------------------------------------------*/

#define TIMER_CLK     1193180L
#define TIMER_MAX     65536
#define TICKRATE      TIMER_CLK/TIMER_MAX
#define PPI	      0x061

#define TIMER_CTRL    0x043
#define TIMER_COUNT   0x042
#define TIMER_PREP    0x0b6
#define SPKR	      0x03




/*--------------------------------------------------------------------*/
/*   Mathematics definitions					      */
/*--------------------------------------------------------------------*/

#define PI	      3.14159265358979323846

#define inch_cm(x)	  ((x)*2.54)
#define cm_inch(x)	  ((x)*0.3937008)
#define deg_rad(x)	  ((x)*0.017453293)
#define rad_deg(x)	  ((x)*57.29577951)
#define oz_gm(x)	  ((x)*28.349523)
#define gm_oz(x)	  ((x)*0.035274)
#define gal_lt(x)	  ((x)*3.78541)
#define lt_gal(x)	  ((x)*0.2642722)
#define lb_kg(x)	  ((x)*0.45359237)
#define kg_lb(x)	  ((x)*2.20462262)
#define mi_km(x)	  ((x)*1.609344)
#define km_mi(x)	  ((x)*.6213712)
#define m_yd(x) 	  ((x)*1.094)
#define yd_m(x) 	  ((x)*0.9144)


/*--------------------------------------------------------------------*/
/*  Definitions for printer control functions.			      */
/*--------------------------------------------------------------------*/

#define BUZZER	 '\007'
#define LF	 '\012'
#define FF	 '\014'
#define CR	 '\015'
#define WIDEON	 '\016'
#define CMPRESON '\017'
#define CMPRESOF '\022'
#define WIDEOFF  '\024'
#define CLRBUFF  '\030'
#define BS	 '\010'
#define TAB	 '\011'
#define BELL	     7
#define HT	 '\011'
#define VT	 '\013'
#define US	 '\037'
#define RS	 '\036'
#define EM	 '\031'



#define  sign(x)   ((x)>0?1:((x)==0?0:(-1)))
