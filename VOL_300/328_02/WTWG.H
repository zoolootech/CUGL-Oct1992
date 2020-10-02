/*
HEADER:		wtwg.h
TITLE:		include (header) file for window routines in wtwg library.
VERSION:	1.0
DATE:		5/1/90

DESCRIPTION: include (header) file for window routines.

KEYWORDS:	windows, keyboard, mouse, text, graphics, expanded memory, menus;
SYSTEM:		MS-DOS v3.0, probably works with v2.0 but untested;
FILENAME:	wtwg.h

WARNINGS:	

SEE-ALSO:	readme.doc, define.doc, funcs.doc	
				
AUTHOR:	David Blum
COMPILERS:	Turbo C v2.0, Microsoft C v5.1
*/


#ifndef  WTWG_HEADER
    #define  WTWG_HEADER

	#include <stdio.h>			
	#include <stdlib.h>		/* required for FILE definition */
	#include <time.h>		/* req'd for time_t */


#ifdef __cplusplus
	extern "C"	{
#endif	/* c++ */



#ifndef ON
	#define ON 	1
	#define OFF	0
#endif	/* ifndef ON */


#ifndef TRUE
	#define	TRUE	1
	#define FALSE 	0
#endif	/* ifndef TRUE */

#ifndef BLINK
	#define BLINK  	0x80
#endif  /* BLINK */

#ifndef BRIGHT
	#define BRIGHT  0x08
#endif  /* BRIGHT */

/* these peculiar defines are required to prevent 'redefinitions'
 * ie, conflicts with graphics.h
 */
#ifndef __COLORS
#define __COLORS

enum COLORS {
        BLACK =	0   ,
        BLUE	    ,
        GREEN       ,
        CYAN        ,
        RED         ,
        MAGENTA	    ,
        BROWN       ,
		LIGHTGRAY   ,
        DARKGRAY    ,
        LIGHTBLUE   ,
        LIGHTGREEN  ,
        LIGHTCYAN   ,
        LIGHTRED    ,
        LIGHTMAGENTA,
        YELLOW      ,
	WHITE	    };


#define LIGHTGREY LIGHTGRAY
#define DARKGREY  DARKGRAY

#endif  /* COLORS */


/* keyboard symbolic definetions
 */
#define CTRL(L)		(L - 0x40)  /* L must be uppercase */


/* Function keys  apply only to F1-F10. see below for F11, F12
 */

#define FKEY(x)		(128+ 58 +(x))
#define SHIFT_FKEY(x)	(128+ 83 +(x))
#define SHIFTFKEY(x)	(128+ 83 +(x))
#define CTRL_FKEY(x)	(128+ 94 +(x))
#define CTRLFKEY(n)	(128+ 94 +(n))


#define EXTENDED_KEY   	00
#define BACKSPACE	(0x08)
#define CTRL_BACKSPACE  127
#define SHIFT_BACKSPACE   8


#define ENTER		(0x0d)
#define RETURN		(0x0d)
#define CTRL_ENTER	(0x0a)
#define ESCAPE 		(0x1b)
#define CTRL_LEFTBRACKET 0x1b
#define CTRL_RIGHTBRACKET 0x1c
#define END 		(128+ 79)
#define CTRL_END	(128+ 117)
#define HOME		(128+ 71)
#define CTRL_HOME 	(128+ 119)
#define PAGE_DOWN	(128+ 81)
#define PAGE_DN		(128+ 81)
#define	CTRL_PgDn	(128+ 118)
#define	CTRL_PAGE_DN	(128+ 118)
#define	CTRL_PAGE_DOWN  (128+ 118)
#define PAGE_UP	 	(128+ 73)
#define UP_ARROW 	(128+ 72)
#define DOWN_ARROW	(128+ 80)
#define DN_ARROW	(128+ 80)
#define LEFT_ARROW	(128+ 75)
#define LT_ARROW	(128+ 75)
#define CTRL_LT_ARROW	(128+ 115)
#define CTRL_LEFT_ARROW (128+ 115)
#define RIGHT_ARROW 	(128+ 77)
#define RT_ARROW	(128+ 77)
#define CTRL_RT_ARROW   (128+ 116)
#define CTRL_RIGHT_ARROW (128+ 116)
#define INSERT		(128+ 82)
#define DELETE		(128+ 83)
#define TAB		(9)
#define	SHIFT_TAB	(128+ 15)
#define	BACKTAB		(128+ 15)
#define CTRL_PRTSCRN	(128+ 114)
#define CTRL_MINUS	31
#define CTRL_BACKSLASH  28




#define MOUSE		128





/*! ALT key functions.  apply only to ALT_letter combinations
 */
int ALT 	(int val);
int unALT 	(int keypress);

#define 	ALTFKEY(n)	(128 +103+ (n))
#define		ALT_FKEY(n)	(128 +103+ (n))



#define  isALT(xx)   ( (xx) >= 0x10+128  && (xx) <= 0x32+128 )



#define ALT_A  (0x1E+128)
#define ALT_B  (0x30+128)
#define ALT_C  (0x2E+128)
#define ALT_D  (0x20+128)
#define ALT_E  (0x12+128)
#define ALT_F  (0x21+128)
#define ALT_G  (0x22+128)
#define ALT_H  (0x23+128)
#define ALT_I  (0x17+128)
#define ALT_J  (0x24+128)
#define ALT_K  (0x25+128)
#define ALT_L  (0x26+128)
#define ALT_M  (0x32+128)
#define ALT_N  (0x31+128)
#define ALT_O  (0x18+128)
#define ALT_P  (0x19+128)
#define ALT_Q  (0x10+128)
#define ALT_R  (0x13+128)
#define ALT_S  (0x1F+128)
#define ALT_T  (0x14+128)
#define ALT_U  (0x16+128)
#define ALT_V  (0x2F+128)
#define ALT_W  (0x11+128)
#define ALT_X  (0x2d+128)
#define ALT_Y  (0x15+128)
#define ALT_Z  (0x2C+128)




#define ALT_1  (128+120)
#define ALT_2  (128+121)
#define ALT_3  (128+122)
#define ALT_4  (128+123)
#define ALT_5  (128+124)
#define ALT_6  (128+125)
#define ALT_7  (128+126)
#define ALT_8  (128+127)
#define ALT_9  (128+128)
#define ALT_0  (128+129)



#define ALT_MINUS	(128+130)

#define ALT_EQ		(128+131)
#define ALT_EQUAL	(128+131)

#define CTRL_PgUp	(128+132)
#define CTRL_PAGE_UP	(128+132)



/* enhanced kbd only
 */
#define ALT_ESCAPE 	(128+1  )
#define ALT_BKSPACE	(128+14 )
#define ALT_LT_BRACKET	(128+26 )
#define ALT_RT_BRACKET	(128+27 )
#define ALT_ENTER       (128+28 )
#define ALT_SEMICOLON	(128+39 )
#define ALT_QUOTE	(128+40 )
#define ALT_ACCENT	(128+41 )
#define ALT_BKSLASH	(128+48 )
#define ALT_COMMA	(128+51 )
#define ALT_PERIOD	(128+52 )
#define ALT_SLASH	(128+53 )
#define CTRL_UP_ARROW   (128+141)
#define CTRL_DN_ARROW   (128+145)
#define ALT_HOME        (128+151)
#define ALT_UP_ARROW	(128+152)
#define ALT_PGUP        (128+153)
#define ALT_LT_ARROW	(128+155)
#define ALT_RT_ARROW    (128+157)
#define ALT_END         (128+159)
#define ALT_DN_ARROW    (128+160)
#define ALT_PGDN        (128+161)
#define ALT_INSERT	(128+162)
#define ALT_DEL         (128+163)
#define ALT_TAB         (128+165)

	/* numpad 5 with numlock off - enh kbd only.
	 */
#define NUMPAD_5	(128+ 76)


/* NOTE that FKEY 11 and 12 arehandled separate from F1-10
 *	enh kbd only.
 */
#define FKEY_11		(128+133)
#define FKEY_12		(128+134)
#define SHIFT_FKEY_11	(128+135)
#define SHIFT_FKEY_12	(128+136)
#define CTRL_FKEY_11	(128+137)
#define CTRL_FKEY_12	(128+138)
#define ALT_FKEY_11	(128+139)
#define ALT_FKEY_12	(128+140)





/* monochrome attributes */
#define WHITE_ON_BLACK 0x02  /*green*/
#define BLACK_ON_WHITE 0x70
#define UNDERLINE      0x01  /*blue */   /*red =x04 */
#define BOLDFACE       0x0A  /* = 0x08 + green */



 /* timout limit for screen saver routine
  *	currently set at 3 minutes 
  * If you change this definition you should recompile wgetc.c
  */
#define WTIMEOUT_LIMIT   3*60



/* line-drawing characters. see wbox[]
 */
struct  WBOX_PATTERN
		{
		unsigned char nw, horiz, ne, sw, vert, se ;
		};



/* array values for wbox[]
 */
#define NO_BORDER		0
#define SINGLE_BORDER 		1
#define DOUBLE_BORDER		2
#define SPECKLE_BORDER  	3
#define SOLID_BORDER		4
#define MIXED2H1V_BORDER	5
#define MIXED1H2V_BORDER	6
#define HANGING_BORDER		7
#define STANDUP_BORDER		8



/* buttons
 */
struct WBUTTON_STRUCT
	{
	struct  WBUTTON_STRUCT   *Bchain;		/* next button */
	int				Bval;					/* keyboard value */
	unsigned int    Brange;					/* range scrollbars*/

	char    		*Btext;					/* display text */
	unsigned char	Bscrollval;				/* screen value of scrollbar*/

	unsigned char   Blen; 					/* len+1 of text*/
	unsigned char   Bstyle;					/* style flag     */

	unsigned char 	Bx;
	unsigned char  	By;
	unsigned char 	Bxend;
	unsigned char 	Byend;
	};





typedef struct WBUTTON_STRUCT  WBUTTON;


/* button flags specifiable by user when calling wbutton_add()
 */
	#define WBTN_BOX 	0x01


/* following button flags reserved for internal use.
 */
	#define WBTN_ACTIVE     0x02
	#define WBTN_SCROLL		0x04
	#define WBTN_BUTTON 	0x08



struct WHEAP_STRUCT
	{
	struct WHEAP_STRUCT 	*whp_next;	/* linked lists of heap */
	struct WHEAP_STRUCT 	*whp_prev;

	size_t			 		whp_nb;  	/* number of bytes */

	char	 		 		whp_flag;	/* type of heap element */
	unsigned char		 	whp_priority;	/* priority		*/

	/*---------- end of common part ---------- */

	void far 				*whp_ram;	/* where stored or accessed*/

	/* double linked lists  of disk blocks,
	 * so we can find 'holes' for new allocations
	 */
	struct WHEAP_STRUCT 	*whp_dskhi;	/* ptrs to lo/hi blocks */
	struct WHEAP_STRUCT 	*whp_dsklo;

	unsigned long 		 	whp_position;	/* position in disk */

	int			 			whp_handle;		/* XMS handle */
	int			 			whp_npages;		/* XMS number of pages */

	/* for disk type storage: append (new block), update (old block),
	 *	or create (new file)
	 */
	char 			 		whp_typewrite;

	char 			 		whp_filler;		/* alignment */


	};

typedef struct WHEAP_STRUCT  WHEAP;




	/* flag values for whx_flag...tells how this blobk is stored
	 */
#define WHP_XMS		0x01		/* expanded memory 		*/
#define WHP_RAM		0x02		/* DOS heap ( void far * )	*/
#define WHP_DSK		0x04		/* disk file 			*/


	/* maximum allowable allocation by heap manager
	 *	this is 64k -16 bytes for DOS memory mgt info.
	 */
#define WHEAP_MAX	(65520u)


/* priority for window save areas
 */
#define WIN_HP_PRIORITY	 1






struct _WINDOW
	{
	struct 	_WINDOW *winchain;	/* pointer to previous window   */

	WHEAP           *winsave;	/* restore screen contents area */

	WBUTTON         *winbutton;	/* pointer to buttons, if any   */

	int				winpxmax;	/* size of window in pixels 	*/
	int				winpymax;	/*      hihgest VALID pixel #s  */
	int 			winpx;  	/* last graphics pos. from wopen*/
	int 			winpy;          /*    in window-viewport co-ords*/

	unsigned char 	winpage;	/* video page # =  0...wlastpage*/
	unsigned char   winputstyle;	/* FLAGS see below		*/
	unsigned char   winleft;      	/* current window positon  	*/
	unsigned char   wintop;
	unsigned char   winxmax;	/* highest valid column    	*/
	unsigned char   winymax;        /* highest valid row       	*/
	unsigned char   winattr;        /* text attribute	   	*/
	unsigned char   winx;		/* current text postion    	*/
	unsigned char   winy; 		/*     range 0...winymax	*/
	unsigned char   winbox;        	/* style of border 		*/
	unsigned char   winboxcolor;  	/* color of border 		*/
	unsigned char 	winflag;	/* internal flag	 	*/
	};


typedef struct _WINDOW WINDOW;





/* put styles - flag values for winputstyle
 */
#define WPUTANSI	0x80	/* ON = use ANSI format codes
							 * OFF= put IBM graphic chars instead
				 			 */
#define WPUTWRAP	0x40	/* ON = wrap text to next line at End Of Line
							 * OFF= truncate text, stop output
							 */
#define WPUTSCROLL	0x20	/* ON = scroll window up one line
							 * 	when text ouptut reaches bottom
							 * OFF= halt text output at lower right char.
							 */
#define WPUTN2RN	0x10	/* ON = translate \n into \r\n. (like ANSI C)
							 * OFF= no carriage return. like TurboC cputs
							 */
#define WPUTALARM	0x08	/* ON = sound alarm for \a.
							 * OFF= no alarm.
							 */
#define	WPUTATTR	0x04	/* ON = char. following \a is an attribute
							 * OFF= char following \a is an ordinary char
							 * ONLY enabled if WPUTALARM is OFF
							 */

#define WFL_CURSOR	0x80	/* ON = (text mode only) cursor is on.
							 * OFF= cursor is off.
							 * ONLY set by wcursor() routine.
							 * DO NOT set this yourself.
							 */






/* wlocation structure:
 */
struct WLOCATION
	{
	int  wloc_x;		/* x,y offsets from above location	  */
	int  wloc_y;
	char wloc_type; 	/* type of location desired - see defines */
	char wloc_reserved;	/* mainains word alignment */
	};


	#define WLOC_CENTER 	0
	#define WLOC_ATXY       1
	#define WLOC_ATWIN	2
	#define WLOC_ATCUR	3












 /* MENUS
  */
struct WMENU_st
	{
	char 			*mu_entry;     	/*text for menu line				 */
	void 			(*mu_func)(void);		/* func to do */
	char 			*mu_help;       /*help 		     					*/
	struct WMENU_st	*mu_menu;       /*nested menu        				*/
	unsigned char 	*mu_enable;		/*ptr to flag to enable this entry	*/
	int           	mu_highlight;   /*which letter (0-n) to highlight 	*/
	int				mu_key;			/*keypress to select this one     	*/
	};

typedef struct WMENU_st  WMENU;





/* FORMS
 */
struct WFORM_st
	{

	char 		*wflabel;	/* identifying text */
	void 		*wfuser;	/* pointer to user data area */
	char 		**wfpick;	/* optional - pointer to picklist */

	char 		*wformat;	/* % format for printf/scanf */

					 /* the next entry is a user-supplied
					  * field validation function
					  */
	int 		(*wfvalidate)(struct WFORM_st *, char *);

	unsigned char 	wflen;		/* number of bytes incl. term. null*/
	unsigned char   wfspecial;	/* internal - force WORD alignment */

	unsigned char 	wfx;	/* x,y co-ords of start of data item */
	unsigned char 	wfy;


	};

typedef struct WFORM_st WFORM;



	/* automatic generation of entries for string, int, float data
	 * assuming no user validation or pickllist functions
	 * and default lengths for numeric data
	 *
	 * give user-friendly names to the data elements.
	 *
	 * _S = string data, _I = int data, _F = float data,
	 * _DT = date, mm/dd/yy or mm/dd/yyyy MUST BE 11 bytes or more.
	 * _TM = time, hh:mm:ss MUST BE 9 bytes or more.
	 *
	 * all that's left to specify is x,y
	 */
#define  WFORMENTRY_S(xx)  #xx " ",  xx, NULL, "%s", NULL,       sizeof(xx), 0
#define  WFORMENTRY_I(xx)  #xx " ", &xx, NULL, "%i", NULL,       6,          0
#define  WFORMENTRY_F(xx)  #xx " ", &xx, NULL, "%f", NULL,       12,         0
#define  WFORMENTRY_DT(xx) #xx " ",  xx, NULL, "%s", wfvdate,    sizeof(xx), 0
#define  WFORMENTRY_TM(xx) #xx " ",  xx, NULL, "%s", wfvtime,    sizeof(xx), 0
#define  WFORMENTRY_END()  NULL,   NULL, NULL, NULL,wfvall,0,        0        






/* wmouse
 */
typedef struct _WMOUSE
	{
	int 	wms_xmickey,		/* mouse position in mickeys (internal use)*/
			wms_ymickey;

	int 	wms_px, wms_py;			/* pixel coords relative to window */

	int 	wms_pxabs, wms_pyabs;	/* pixels in abs.screen coords*/

	int		wms_x, wms_y;			/* text-based position in window */

	int 	wms_xabs, wms_yabs; 	/* text-based absolute  coords*/

	/* mouse flags -- zero if false, non-zero if true.
	 *
	 *	note:
	 *        wms_present: if non-zero, tells number of mouse buttons
	 *	  		wms_used: see defines below to tell how mouse was used
	 *		    	but you'll never see WMS_RIGHT_PRS because it's
	 *				changed to ESCAPE automatically.
	 *
	 *
	 */
	unsigned char 	wms_present,	/* was a mouse detected            */
			wms_used,				/* was mouse used since last call  */
			wms_inwindow,			/* is mouse in current window      */
			wms_internal;			/* internal use */
	} WMOUSE;





	/* defined values for wmouse.wms_used
	 */

		/*------- recent button release ------*/

	#define	WMS_LEFT_RLS	0x01

	#define WMS_CENTER_RLS	0x02

		/*------ mouse mvt with Left or Center button held down --------*/

	#define WMS_MOVED	 	0x04

		/*------ recent button press --------*/

	#define WMS_LEFT_PRS	0x08

	#define WMS_CENTER_PRS 	0x10

	#define WMS_RIGHT_PRS 	0x20

	#define WMS_RIGHT_RLS	0x80
	
/* macro name table
 */
typedef char WMACRO_NAMES[10][9];


#ifdef __TURBOC__
/* mouse drawing
 */

typedef struct
	{
	char	wdr_type;
	char 	wdr_filler;
	int 	wdr_linestyle;
	int 	wdr_thickness;
	int 	wdr_color;
	int		wdr_x1, wdr_y1;		/* start or center co-ords (@button press)*/
	int	 	wdr_x2, wdr_y2;		/* end co-ords			   (@button release)*/
	}
	WOBJECT_D_ART;

	#define WDR_LINE		'l'
	#define WDR_RECT		'r'
	#define WDR_XHAIR   	'x'
	#define WDR_VERT 		'v'
	#define WDR_HORZ		'h'
	#define WDR_SQUIGGLE 	's'



/* wdraw lets user draw an object, 
 * returns a ptr to allocated struct WOBJECT_D_ART describing the object.
 * Not available in microsoft C version.
 */
WOBJECT_D_ART *wdraw ( char type, int color, int linestyle, int thickness );

#endif /* TurboC mouse drawing */





/*---------------------- global variables. -------------------------*
 *
 *  NOTE: defined in order of alignment precedence.
 *	  so it doesn't matter if you compile with -a on or off
 *
 *------------------------------------------------------------------*/


	extern 	WINDOW  	*(w0);

	extern  WINDOW		*(wfullscreen);

	extern  char 		*whelp_ptr;

	extern  int 		wtabwidth;	/* default = 8 */

	extern 	unsigned int wcurscanln;	/* cursor size - internal use only */


	/* screen size, in characters co=ords (even in graphics modes
	 * the number of the first row/column is 0/0
	 * the number of the last row/column  is wyabsmax /wxabsmax
	 * loop control logic should be like this:
	 *	for (row=0; row <= wyabsmax; ++row) {process each row}
	 * the actual screen size if wxabsmax+1 by wyabsmax+1.
	 */
	extern  int 		wxabsmax, wyabsmax;



	/* graphics- specific values -- not used in text-mode functions.
	 */
	extern  int 		wpxabsmax, wpyabsmax; /* number of pixels */




	extern struct  WLOCATION wlocation;

	extern WMOUSE wmouse;



	/* graphics
	 *     wpath_BGI = path name for BGI interface. Default is "C:\\TC"
	 *		   change before calling winit()
	 */
	extern  char		*wpath_BGI;	/* path to BGI drivers */
	#define wpxchar		8
	#define wpychar		8

	extern  char *whome_dir;	/* 'HOME directory' 
								 *  max bytes = MAXDRIVE+MAXPATH see dir.h */



	extern 	char  		wmode;		/* 'T' or 'G' */
	extern  char 		wmonitor;	/* 'H' 'E' 'V' or 'C' */



	/* page number
	 * 	range is 0 thru wlastpage, inclusive.
	 */
	extern  unsigned char 	wlastpage;      /* constant set by winit() */
	extern 	unsigned char 	wnextpage;      /* change prior to wopen() */

	extern  unsigned char   wneedpage;	/* internal use only */




	/* colors for menus, menu box, buttons... defaults noted below.
	 */
	extern  unsigned char 	wmenuattr,     	/* GREEN */
				wmenuboxattr,	/* GREEN */
				wbuttonattr,	/* WHITE<<4 */
				whelpattr;	/* CYAN<<4  */

	/* table of border characters. see defintion above
	 */
	extern const struct WBOX_PATTERN wbox[];



	/* globals used by the wgetc() routines
	 */
	extern int 	wunget_buffer;

	extern int 	(*wkeytrap)( int );

	extern int	(*wpipein )( void );
	extern void	(*wpipeout)( int  );



void winit ( char newmode );		/* newmode = 'W' or 'T' in caps */
void winit_pages ( char newmode );
int  wdvinit (void);				/* DeskView */

int w50line (int want_50);	

void  wSysRq_install ( void (*u_func)() );	
void  wSysRq_remove ( void );

void  wSysRq_ask (void);             /* sample u_func */

int w_ega_color(void);		/* ON = color, OFF=ega monochrome */


void wclockinstall (int x, int y);	/* x,y = screen coords for clock */
void wclockremove  (void);


void wsetpalette ( unsigned char  palette[17] );
void wgetpalette ( unsigned char palette[17] );





	/* wopen, wdefine - x, y give top left corner.
	 * 		xmax, ymax speicfy number of columns, rows needed
	 *      	once window is opened, winxmax = 1 less than xmax
	 *		(coords start at 0)
	 */

WINDOW	*wopen
		(int x, int y, int xmax, int ymax,
		 unsigned char attr, int box, unsigned char box_attr,
		 unsigned char save2 );



	/* save modes for wopen
	 */
	#define WSAVE2NULL	0	/* don't save window */
	#define WSAVE2RAM	1	/* save to RAM 	*/
	#define WSAVE2DISK	2	/* save to disk */

WINDOW *wdefine (int x, int y, int xmax, int ymax,
		 unsigned char attr, int box, unsigned char box_attr );



void    wabandon (void);	/* close window, no screen changes */



#define wclose()  ( wrestore(), wabandon() )	/* restore screen, close */


	/* wreopen, wbury - move specified window to top of stack
	 * 			or move current window to bottom of stack
	 *		no screen access takes place.
	 */

void 	wreopen ( WINDOW *Wnew );
void    wbury   ( void );

int 	wrelocate (int x, int y);	/* returns 0 if OK */




#define wsetlocation(LOC, X, Y)	    \
	 wlocation.wloc_type=(LOC), \
	 wlocation.wloc_x   =(X),   \
	 wlocation.wloc_y   =(Y)


void wlocate ( int *addr_x, int *addr_y, int xmax, int ymax );




void wstrrowcol( char *text, int *rows, int *cols );/* #rows & cols in str*/




/* whide() and wshow() hide current window and show current window
 *	PARAMETER to wshow = ptr returned by whide().
 */
WHEAP  *whide ( void );
void    wshow ( WHEAP *hideptr );



void 		wframe  (int l, int t, int r, int b,
			 int style, unsigned char color);

#define	wtitle(title) wborderwrite ('t', 'c', (title))
#define	wps(footnote) wborderwrite ('b', 'r', (footnote))


void wborderwrite (char line, char justify, char *title);






/*! wsetvisualpage () or wturnpage() changes the displayed page.
 *	does not change which page wputc/wputs etc write to...
 */
void 	wsetvisualpage (int page);
#define wturnpage(newP)	wsetvisualpage( (newP) )





/* wsave saves current contents of screen indicated by window pointer
 * 	and stores the far * pointer to the save area in winsave.
 *      NOTE if w0->winsave is not NULL on entry, wsave will use that area.
 *		to hold the screen image,without checking for area size.
 *
 * wrestore restores the screen underlying current window.
 *	Does not free the storage.
 */
void     wsave    (void);
void     wrestore (void);



/*! wdrag()	move current window.
 * 	Parameter: keystroke = an ARROW key
 *	RETURNS: non-zero (original keystroke) if move was successful.
 *               0 if move was not successful, which may be because:
 *			invalid keystroke, outside screen edge, or WSAVE2NULL
 */
int	wdrag (int  keystroke);


/* mouse keytraps  call once at startup.
 */
void wmsdrag ( void );
void wmspopup ( int (*func)(void) );
void wpopfkey ( char **fkeylabels );



int 	wputc		(char c);
int  	wputs		(char *s);
void	wputfl		( char *, int );
int 	wprintf     	(char *format, ...);

	#define 	WPRINTF_LIMIT	250

void	werror		(int errcode, char *errmsg);

#define wsetattr(x)   	( w0-> winattr = (x)  )
#define wbright()   	( w0-> winattr |=  BRIGHT )
#define wdim()    	( w0-> winattr &=  (0xff - BRIGHT) )
#define wgetattr()      ( w0-> winattr )

#define wreverseattr(xx)  (  ( ((xx)&0x0f)<<4 ) | ( ((xx)&0xf0)>>4) )

void wclear 	(void);
void wclearline (void);

void wsetc ( char c );	

#define wclearln() wclearline()




void 	wscroll	(void);  /* insert a blank line at cur postion. scroll up */






/*! wgoto() to a specified row column. either x,y =-1 --- no mvt.
 */
void 	wgoto (int row, int col);



#define wherex()     ( w0-> winx )
#define wherey()     ( w0-> winy )






int 	wgetc       (void);

int		wgets       (int nbytes, char *buffer, char behavior);
	/* gets behavior flags:  _FORM entry, accept _INT only, accept _DEC pt
	 * 						and auto-return when buffer filled
	 */
	#define WGETS_FORM		0x80
	#define WGETS_INT		0x40
	#define WGETS_DEC		0x20
	#define WGETS_SKIP		0x10
	#define WFM_AUTOSKIP	0x10

int 	wready(void);	/* 0 if no user input avail (kbd or mouse) */


void wflush (void);	/* flush kbd and mouse */

int  wkbd_enhanced (void);	/* RETURNS 0 if not an enhanced kbd */


void		wcursor     (int);	/* ON or OFF, text mode only */


int  	wpipefin  ( char *filename, void (*whendone)(void) );
void    wpipefout ( char *filename, int  stopcode );

void	wmacro_install  (void);
int 	wmacro_select	(char *message);
WMACRO_NAMES *wmacro_names (void);
void 	wmacro_assign	(char *filename, int key);




void 	       	wmouse_turn 	(int);	/* ON/OFF, must use in pairs */

void			wmouse_limit    (int);	/* ON/OFF limit mouse to window */

void 			wmouse_move       ( int x, int y );	
void 			wmouse_move_pixel ( int px, int py );	

void			wmouse_flush (void);		/* wait until all keys RLS'd */

#define wpushc(xx)      wunget_buffer = (xx)

#define wungetc(xx)	wunget_buffer = (xx)




/*--------------------- 'BUTTON'  functions -----------------------------*
 *
 * wbutton_add ()
 *		PARAMETERS:
 *			char *text  = text to place in button.
 *					may be longer or shorter than button
 *			int  x, y   = x,y position of button in open window.
 *			int len	    = length of button, include terminal NULL.
 *			int value = keyboard bvalue of button.
 *			unsinged char style = WBTN_BOX to draw a box.
 *
 *		RETURNS: void.
 *
 *              OPTIONS for bstyle:
 *			WBTN_BOX = draw a box around button
 *
 * wbutton_mark () - places a 'mark' to left of a button
 *
 *
 * wbutton_delete()- remove specified button. & redraw screen
 *		PARAMETERS:
 *			int	value = keyboard value of button
 *              	int     how   = 0= clear screen area
 * wbutton_inactivate()    - temporarily inactivate button.
 *		PARAMETERS:
 *			int	value = button value
 *			int     how   = 0= clear screen area
 *					1= rewrite text in current attr
 * wbutton_activate()	- reactivate the button. and redraw in wbutton_attr
 *
 * wbutton_getptr()	- returns ptr to button that matches a value
 *
 * wbutton_test()		- returns button value or 0 if not a button
 *
 *
 */
void wbutton_add (char *btext, int bx, int by, int blen,
		int bval, unsigned char bstyle );

void wbutton_delete (int bval, int how);

void wbutton_inactivate (int bval, int how);

void wbutton_activate (int bval);

void wbutton_mark ( int bval, char mark );

WBUTTON *wbutton_getptr ( int bval );

int wbutton_test ( int bval );




	/* internal button functions
	 */
	void wbutton_draw ( WBUTTON *, unsigned char  );
	void wbutton_frame ( WBUTTON *, unsigned char , unsigned char );

/* SCROLL BAR functions.
 * 	use wscrollbar_add() to create a scrollbar.
 *		CAUTIONS: y2-y1 must be >3. range >2. key must be untypable (-1,-2,etc)
 *	when wgetc() returns the scrollbar_id_key, call wscrollbar_scroll().
 * 	
 */
WBUTTON *wscrollbar_add ( int scrollbar_id_key, 
			unsigned char x, unsigned char ytop, unsigned char ybottom,
			unsigned int virtual_range, unsigned int virtual_start );
			
void 			wscrollbar_draw   ( WBUTTON *Bptr );
unsigned int 	wscrollbar_scroll ( WBUTTON *Bptr );  /*returns virtual_val */
void 			wscrollbar_reset  ( WBUTTON *Bptr, unsigned int new_val );




	/* wpromptc() - parms are: title, message, list of choices...
	 *		last parm in the choices list MUST be NULL
	 */
int wpromptc(char *title, char *msg, ...);



int wprompts ( char *title, char *msg, char *reply, int nbytes );



	/* wpicklist() - parms are title and list of ptrs to strings.
	 * 	RETURNS n= index to chosen string or index to terminal NULL
	 */
int wpicklist ( char *title, char **list );



/*! wscanform() and wprintform()
 *	PARAMS: a title and a pointer to a WFORM table.
 *	RETURNS: ENTER or ESCAPE
 *
 * the last line terminates the table with a NULL in the wflabel position.
 *	the last line also gives the xmax and ymax for the form window
 *	(be sure to leave enough room for all data & text )
 *
 *	A validation function may be specified for the whole form.
 *	 If the validation function for the whole form returns non-zero,
 *			no updating will take place.
 *
 * data items may be any type supported by printf/scanf
 *	EXCEPT: longs, doubles, long doubles and pointers are NOT supported
 *		variable lengths (%*i) is NOT supported.
 *		item count reporting (%ni) is NOT supported.
 *
 *      a picklist may be specified instead of hand-typed entry.
 *
 *      a form item may be a 'pure label' ie, only the wflabel is printed,
 *		ie, leave the wfuser field NULL.
 *
 * validation functions: Each entry in the WFORM table can call a function
 *	The pointer to the function is specified in the 'wfvalidate' entry.
 *	The function prototype MUST look like this:
 *		int func_name (WFORM *form, char *buffer)
 *	The function is called each time data is entered into the field.
 *	Parameters passed to the function:
 *		WFORM *form     ptr to the WFORM entry for this filed.
 *		char  *buffer   ptr to the string holding the new value
 *				The string is AT MOST (form->wflen) bytes.
 *
 *	should RETURN: 0 if the data is valid, non-zero if not valid.
 *		The function may optionally communicate errors to the user.
 *
 *
 *
 * you must specify the length of a temporary work-area buffer
 *	length must be enough to hold the data in string form with terminal 0
 *	length MUST include terminal NULL.
 *	for string data, length is # bytes that will be returned,
 *		incl. terminal NULL.
 *
 * the x,y positions for data items determine the screen start of the data
 *	the label is placed before the data.
 *	Be sure the label fits in the space you provided.
 *
 *CAUTION: data 'labels' should not be the first or last entry in the form tbl.
 *
 * wprintform() opens a window, draws the form, and leaves the window open
 *	with data showing. No data entry or chages to data values occur.
 *
 */
int wscanform  (char *title, WFORM *form);
void wprintform (char *title, WFORM *form);

	extern char  wfmatr_lbl,		/* attr for labels */
				 wfmatr_lbl_active,	/*      label indicating active data item */
				 wfmatr_border,		/*      form border */
				 wfmatr_title,
				 wfmatr_data,	 	/*      inactive data */
				 wfmatr_data_active;/*      active data item (while typing) */
				 
	extern int	wfm_border_type;	/* DOUBLE BORDER */	
	
	extern char wfm_autoskip;		/* next field when this on is filled in? */
									/* default=0, or set to WFM_AUTOSKIP	 */
	



/* Tools for form validation functions
 *
 * When a form validation function (see above) is called, it is passed
 * a ptr to the form table for the current entry and a ptr to the char *
 * buffer holding the current value.
 *
 * Form validation functions may call the following sub-procedures
 * to simplify work.
 *
 * wform_showerror() - display an error msg undr the field
 *		If NULL is passed, a standard error message is displayed.
 *		The error msg must be 1 line and short enough to fit onscreen
 *		(it is located starting below and to the left of the data)
 *
 * wfvall() - pass the start address of the form, and buffer.
 *		This routine loops through all the fields and calls all
 *		validation routines one at a time, keeping track of errors.
 *		Use this function as all or part of the total form validation
 *		(ie, name the function on the form header line)
 *
 * wfvdate() validates date data for forms
 */

void wform_showerror(WFORM *form, char *errmsg);
int  wfvall (WFORM *form, char *buffer);
int  wfvdate        (WFORM *form, char *buffer);
int  wfvtime        (WFORM *form, char *buffer);

int  wval_date ( char *date );
int  wval_time ( char *time );

void wdtparse ( unsigned int *, char **, char );	/* internal */ 




/*! wfscanform() wfprintform()  - file oriented form processing.
 *
 *
 *
 *  wfscanform -
 *	parse a file using the same form table as above.
 * 	one input item is looked for on each line of the file
 *	doesn't matter if file is opened as text or binary (for wfscanform)
 *		the order of the data items in the file doesn't matter
 *
 *	scanning is continued until a line beginning with stopcode is reached
 *
 *	validation functions are not called by wfscanform
 *
 *
 *  wfprintform -
 *	the data and labels are printed to a file, one line at a time
 *		in the order of the table.
 *
 *
 */
void wfscanform ( FILE *infile,  WFORM *formtable, char *stopcode );

void wfprintform (FILE *outfile, WFORM *formtable );



/* date and time processing
 * 	NOTE the buffer for dates must be mm/dd/yyyy = 11 bytes (with \0)
 *		 buffer for times must be hh:mm:ss   = 9 bytes.
 */
time_t mktime ( struct tm * );		/* ANSI routine TurboC forgot *
									 * WARNING - only returns timt_t,
									 * 		does not modify struct values 
									 */
									 
void wt2showt ( time_t *t, char *date, char *time );
void wshowt2t ( char *date, char *time, time_t *t );

void whotkey_install ( int hotkey_keyvalue,  void (*hotkey_func)(void) );

void wpulldown 		( WMENU *topmenu );			/* install topmenu */
void wpulldown_pages	( WMENU *topmenu );		/* use if mult video pages */
void wpulldown_draw     ( void );				/* redraw top 2 lines */

extern int  wpulldown_enable;					/* 0=turns off menu default=1*/ 



/* install hep system. specify filename without the .hlp and .hx extensions
 */
void whelp_install(char *filename);				

void	wdos		(void);		/* not avail in graphics mode */
void 	wexit 		(void);		/* ask user if exit desired */

WINDOW 	*wunlink (void);
void    wlink (WINDOW  *Wnew);






	/*--------- memory mgt routines ---------------*/
	/* wmalloc & wfarmalloc & wrealloc - ptr is NORMALIZED
	 * if errmsg is not NULL, and no mem avail, prog halts.
	 * NOTE: in models with far data ptrs, wmalloc maps to wfarmalloc.
	 * MICROSOFT C warning: YOU MUST DEFINE __LARGE__ (or others as needed)
	 *						or your programs will not link in large model.
	 */
void *wmalloc         ( size_t size,  char *errmsg );
	 
#ifdef __LARGE__
	#define wmalloc(x,y)  wfarmalloc ( (x), (y) )
#endif	
#ifdef __COMPACT__
	#define wmalloc(x,y)  wfarmalloc ( (x), (y) )
#endif
#ifdef __HUGE__
	#define wmalloc(x,y)  wfarmalloc ( (x), (y) )
#endif
void  far *wfarmalloc ( unsigned long nbytes,  char *errmsg );

void *wrealloc  ( void *block,  size_t size,  char *errmsg );




 extern unsigned long wheap_ramlimit;	/* default = 256k */
	 /* this is max amount of ram used by heap manager */




void 		 wheap_init ( void );


WHEAP 		*wheap_alloc
		   (size_t numbytes, unsigned char priority, char *errmsg );
		
			/* priority=0     = lowest (always on disk), 
			 *          1-254 = intermediated (wherever it fits)
			 *          255   = highest (never on disk).
			 * 			If not enough, lower priority items are swapped.
			 * errmsg = NULL, control returned to caller if error
			 *        = string, program halts & errmsg printed.
			 *                  string should be <12 bytes.
			 * 
			 */ 


void far 	*wheap_access   ( WHEAP *heap_ptr,  int readwrite );
void 		 wheap_deaccess ( WHEAP *heap_ptr,  int readwrite );

	/* if readwrite is 0, no access to disk takes place
	 */


void		 wheap_free  ( WHEAP * );




unsigned long	 wheap_avail    ( void );	/* total ram+xms */

unsigned long	 wheap_availxms ( void );


void	 wheap_swap2dsk ( WHEAP * );
void	 wheap_swapall2dsk ( int priority ); /* all of this priority*/



void far 	*wheap_unlink ( WHEAP * );
	/*	swaps from expanded or disk storage to main ram.
	 *	removes control block from heap manager.
	 *
	 *	returned address can be released by farfree()
	 *		(free() if LARGE)
	 */


void		 wheap_freeall ( void );



void whplj_dump ( FILE * );		/* laserjet screen dump */
void whplj_install ( int key );

/*-------------------------- WINDOW INTERNALS ----------------------*
 */



	extern unsigned  char far *wvideo_ram;
	extern unsigned  char far *wpage_ram;

	extern unsigned  int	   wpage_size;

	extern char	wega256;
	extern int  wegarowsize;	/* bytes per graphics row */



	#ifdef  __WSCREEN__
	      #ifdef TEXTONLY


			#define whifont +++(errorw_hifont_reserved_symbol)
			#define wlofont +++(errorw_lofont_reserved_symbol)


		#else		/* not TEXTONLY */

			/* graphics mode globals */
        		extern unsigned char whifont[8*128];
			extern unsigned char wlofont[8*32];

		#endif	/* TEXTONLY */



	#else		/* WSCREEN not defined */

		/* definitions to prevent referencing the font tables
        	 */
		#define whifont +++(errorw_hifont_reserved_symbol)
		#define wlofont +++(errorw_lofont_reserved_symbol)

	#endif  /* __WSCREEN__  */




char w_inDOS (void);

void 	wgetmon (void);

void 	wputcabs ( int x, int y, unsigned char c, unsigned char attr,
			unsigned char write_mode );

	#define WGOVERWRITE 	0x00
	#define	WGXOR		0x18
	#define WGOR		0x10
	#define WGAND		0x08


void 	wgscrollv ( int l, int t, int r, int b, int nr );
		/* nr = pixels, +=down -=up */

void wclearabs (int left, int t, int r, int b );

int  wreadc 	   (void);
int  wread_kbd  (void);
int  wready_kbd (void);
void  wflush_kbd (void);
void wkbd_init (void);

void  		wmouse_init 	(void);
void	  	wmouse_location (void);
void 		wmouse_textmask (unsigned int screen, unsigned int cursor);


	/* BGI interface */
void 	walign    (int current_page);
void 	wmovepxpy (void);
void 	wherepxpy (void);
void 	wtextmode (void);
void 	wgraphmode (void);

#ifdef __cplusplus
	}
#endif	/* c++ */

#endif		/* end of WTWG_HEADER */

