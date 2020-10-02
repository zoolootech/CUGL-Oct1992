/* ciao.h - declarations for all global data & functions in ciao.lib
**          the module is self-initializing, vid_init() is only required
**          to set a particular text mode (7=mono, or 2=b&w or 3=color,
**          or 0=default, whatever's running).
**
**    --> IT IS RECOMMENDED PRACTICE TO ***ALWAYS*** CALL VID_INIT(n) <--
**
**          this is the revised version developed for fafnir.lib, so
**          the dlrcvt, time and sound functions are in cutils.lib, but
**          use this header file.  9/10/88, d.c.oshel
*/

#include "vidinit.h"       /* the CIAO library, globals and macros! */
    /* vidinit.h nested includes: 
    **        stdio.h, stdlib.h, dos.h, malloc.h
    */
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>


/*   video attribute info re the wputs() and wprintf() ^ commands
=========================================================================
     There are 16 public Video Attribute Registers:  vid[0] ... vid[15]

     Common            VidReg   ^ commands that set the attribute
     -------------------------------------------------------------------
     Normal            vid[0]   wputs("^0"); wputs("^");
     Bold              vid[1]   wputs("^1"); 
     Emphasis          vid[2]   wputs("^2"); 
     Attention!        vid[3]   wputs("^3"); 

                       vid[4]   wputs("^4");
                       .
                       .
                       .
                       vid[ 9]  wputs("^9");
                       vid[10]  wputs("^ê"); keystroke is ALT 234
                       vid[11]  wputs("^ë");      "       ALT 235
                       vid[12]  wputs("^ì");      "       ALT 236
                       vid[13]  wputs("^í");      "       ALT 237
                       vid[14]  wputs("^î");      "       ALT 238
                       vid[15]  wputs("^ï");      "       ALT 239


     The DEFAULT contents of these registers is as follows:

     Contents      *Color/Graphics Adapt.   Monochrome Adapter
     -----------------------------------------------------------------
     Normal         brite white on blue     normal
     Bold           brite yellow on black   bright normal
     Emphasis       brite blue on white     reverse
     Attention      blink br. white on red  blinking reverse

     vid[ 4]       *red, 4                  underline
     vid[ 5]        magenta, 5              bright underline    
     vid[ 6]        dark yellow, 6          blinking normal
     vid[ 7]        ordinary white, 7       blinking underline
     vid[ 8]        dark grey, 8            blinking bright normal 
     vid[ 9]        brite blue, 9           blinking bright underline
     vid[10]        brite green, 0x0a       normal
     vid[11]        brite cyan, 0x0b        normal
     vid[12]        brite red, 0x0c         normal
     vid[13]        brite magenta, 0x0d     normal
     vid[14]        brite yellow, 0x0e      normal
     vid[15]        brite white, 0x0f       normal

     *The default background is black for registers vid[4]..vid[15], and
      blink is off.

==========================================================================
*/

/* wprintf( fmt, variable_length_argument_list );
**
** calls wputs, recognizes ^ commands
*/

extern void wprintf(char *,...);


/* keyin( wait_fn );
**
** wait_fn executed while awaiting keyboard input
** returns characters as positive integers from 1..255
** returns extended ASCII (Fn keys, etc.) as defined in <keys.h>
**
** two candidates for the function argument to keyin are acg() or
** screenwait().  screenwait() calls acg() to bump the generator,
** blanks out the screen (until user keypress) after 8 minutes of
** quiet at the console.  keyin( noop ) is also frequent usage.
*/

extern int keyin(void (*wf)(void ));            /* usually calls screenwait */
extern int getkey(void);                /* keyin(screenwait) with key click */

extern unsigned acg( void );     /* additive random number generator, Knuth */
extern void init_acg( void );     /* initialize acg when can't keyin( acg ) */
extern void screenwait( void ); /* screen goes blank after 8 min inactivity */
extern void flush_keys( void );          /* annihilate DOS typeahead buffer */
extern int keyshift_status( void );     /* interrupt 22 shift status report */

extern void noop(void );                      /* do nothing, convenient!    */
extern void bell(void );                      /* beep: i.e., putch('\a');   */
extern unsigned int sleep(int n);  /* wait n/10ths second (more or less), 
                                               return 0 or user's keystroke */



/*-------------- these functions are in FUTILITY.LIB -------------*/


/* sound */

extern int tone;                  /* no sound effects if false */

extern void HIclack( void );      /* musical gadget declarations */
extern void LOclack( void );
extern void bopbleet( void );
extern void blopbloop( void );
extern void thurb( void );
extern void setfrq( unsigned ); /* sound generation primitives declarations */ 
extern void spkron( void );
extern void spkroff( void );
extern void note( unsigned, unsigned );


/* dollar longs */

extern char *dlrcvt(long );         /* convert long to form 999,999,999.99- */ 
extern char *crdrform(long );            /* same but with trailing Dr or Cr */


/* tm.h, February 20, 1988, d.c.oshel
**       emulate date and time functions from C Food Smorgasbord
**       (obviously, these are WORK-ALIKES, not the originals!)
*/


/* read system clock (PC/MS-DOS 2.x services)
** get_system_time() reads valid hundredths of a second, accurate
** to about 1/20th second depending on the system clock.
** get_system_date() sets dayofweek as 0=Sunday..6=Saturday
*/

extern  void get_system_time(int *hr,int *min,int *sec,int *hnd);
extern  void get_system_date(int *year,int *month,int *day,int *dayofweek);


/* formatted ASCIIZ date and time strings, or BCD byte array
cf_time and cf_date emulate C Food Smorgasbord fns time and date,
with the following mode options controlling format:

                cf_date(ptr,mode)   cf_time(ptr,mode)

BCD arrays:
                3-byte              4-byte          
                yy mm dd            hh mm ss dd
Mode Option 0. [88,02,20]          [09,55,34,58] 

ASCIIZ strings:

Mode Option 1.  880220              095534        (24-hr)
Mode Option 2.  02/20/88            09:55:34      (  "  )
Mode Option 3.  02-20-88            09553463      (  "  )
Mode Option 4.  FEB 20, 1988        09:55:34.63   (  "  )
Mode Option 5.  February 20, 1988   09:55         (  "  )
Mode Option 6.  20 FEB 88           09:55:34 AM   (12-hr, AM or PM)
Mode Option 7.  20 FEB 1988         09:55 AM      (       "       )

User must allocate, and provide a pointer to, a character buffer which
receives formatted ASCIIZ output.

These functions place formatted date and time strings into the user's
receiving buffer, and return a pointer to the '\0' character terminating
the strings.  (Ptr returned may have some use in string concatenation.)

Option 0, however, works the same way, but there is NO '\0' terminator
at the end of the receiving 3- or 4-byte array.  The pointer is to the
next byte following the BCD byte array.  (Ptr returned is useless.)

Since mode Option 0 returns binary coded decimal, sign extension will be
a consideration for the first byte of the date (year), and the hundredths
byte of the time (ranges up to 99).
*/

extern  char *cf_time(char *str,int mode);
extern  char *cf_date(char *str,int mode);

extern char *Month_table[]; /* "January", etc., 0=NULL, 1..12 */
extern char *MON_table[];   /* "Jan", etc.,     0=NULL, 1..12 */
extern char *Day_table[];   /* "Sunday", etc.,  0=NULL, 1..7  */

extern char *sysdate( void ); /* ptr to system date, like "12/25/87" */
extern char *systime( void ); /* ptr to system time, like "18:00:00" */

extern void jul2mdy (  long jday,     /* valid Julian Day number */
                       int * mm,      /* Gregorian month returned */
                       int * dd,      /* Gregorian day   returned */
                       int * yy       /* Gregorian year  returned */
                       );

extern long mdy2jul (  int month,      /* Gregorian calendar month */
                       int day,        /* Gregorian calendar day */
                       int year        /* Gregorian calendar year */
                       );

extern int day_of_week( long jday );



/*---------- these functions are in FOLD.LIB -----------*/


/* March 18, 1988 - filesize returns a long, number of bytes in open file
*/
extern  long filesize(struct _iobuf *fptr);  /* i.e., FILE *fptr */


/* crypt.h
** December 30, 1986
** Hogg's Code, d.c.oshel
*/

/*global*/  void set_crypt(char *,int );  /* key, seed */
/*global*/  int cryptic(int );
/*global*/  int uncrypt(int );
/*global*/  char *encode(char *);
/*global*/  char *decode(char *);


/* julian day number, astronomical version
** see: Press, et al., Numerical Recipes, pp. 681, 682 
*/

extern double julian( float, float, float) ;  /* year, month, day */


/* keydefs.h, 12/7/86, d.c.oshel
*/

extern int KeyDefs(char **, char *, char *); /* options array,title,prompt */


/* oldedits
*/
/* getms( receiving_buffer_pointer, maximum_length, wait_fn, esc_fn );
**
** wait_fn executed while awaiting keyboard input
** esc_fn executed if user types ESC
*/

extern int getms(char  *,int ,void ( *)(),void ( *)());

extern int ESCflag,           /* string edit functions return ESC or MINUS */ 
           MINUSflag;         /* if these booleans are non-zero */

extern int edit_masked_numeric(char *,void (*)());  /* Bufptr, FnKeyRoutine */
extern int edit_string(char *,void (*)());          /* Bufptr, FnKeyRoutine */

extern void message(int ,int ,char *,... );       /* x,y, variable arg list */



/* clairol();
**
** popout window gives users access to vid[0]..vid[3], interpreted as
** message levels Normal, Bold, Emphasis and Attention.  Users may select
** screen colors or monochrome attributes, depending on hardware.
*/

extern void clairol(void );


/* typeafile( pathname, wrap, tx, ty, bx, by );
**
** example: typeafile("\\games\\userman.doc", TRUE, 0,0,79,24 );
** types the file, if found, in a window box of the given dimensions
** word wrap in paragraphs if the wrap argument is TRUE
** hanging indentation in the document may cause a messy display because
** leading spaces are preserved in the middle of wrapped lines (!)
** the PgUp key allows you to scroll backwards in the file (!) -- there
** is a remote chance of stack overflow because the algorithm is recursive
** preserves entry screen, restores it on exit
*/

extern int ambiguity,      /* clairol: ambiguous message levels on entry? */
           Control_C_Flag; /* Typeafile exit condition flag */

extern void typeafile(char *,int ,int ,int ,int ,int );

