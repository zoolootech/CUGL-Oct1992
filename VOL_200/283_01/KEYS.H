/* keys.h -- header file defines extended ascii characters
**           4/20/88, d.c.oshel
*/

/*
on return from keyshift_status(), flag bits (from AL) are set as follows:
 7.......  1=Ins active (sticky)
 .6......  1=Caps Lock (sticky)
 ..5.....  1=Num Lock (sticky)
 ...4....  1=Scroll Lock (sticky)
 ....3...  1=Alt depressed (not sticky)
 .....2..  1=Ctrl depressed (not sticky)
 ......1.  1=Left Shift depressed (not sticky)
 .......0  1=Right Shift depressed (not sticky)
*/


/* these definitions require keyin() from the ciao library, or another
   of my own functions; the style presented is consistent with the
   emerging Borland function key standard, i.e., there seems to be a
   de facto agreement among developers that IBM function keys should
   return these values to C programs.  The hardware considerations are
   not important (the Borland standard is an optimization of BIOS
   behavior), so long as all future programs are consistent with these
   values.
   */

#define F1   0x3b00
#define F2   0x3c00
#define F3   0x3d00
#define F4   0x3e00
#define F5   0x3f00
#define F6   0x4000
#define F7   0x4100
#define F8   0x4200
#define F9   0x4300
#define F10  0x4400

#define UP   0x4800
#define DN   0x5000
#define LF   0x4b00
#define RT   0x4d00
#define HOME 0x4700
#define END  0x4f00
#define PGUP 0x4900
#define PGDN 0x5100
#define INS  0x5200
#define DEL  0x5300

#define Shift_F1   0x5400
#define Shift_F2   0x5500
#define Shift_F3   0x5600
#define Shift_F4   0x5700
#define Shift_F5   0x5800
#define Shift_F6   0x5900
#define Shift_F7   0x5a00
#define Shift_F8   0x5b00
#define Shift_F9   0x5c00
#define Shift_F10  0x5d00

/* -- trivial case, these all map to digits
#define Shift_UP   0x38		
#define Shift_DN   0x32
#define Shift_LF   0x34
#define Shift_RT   0x36
#define Shift_HOME 0x37
#define Shift_END  0x31
#define Shift_PGUP 0x39
#define Shift_PGDN 0x33
#define Shift_INS  0x30
#define Shift_DEL  0x2e
*/

#define Alt_F1   0x6800
#define Alt_F2   0x6900
#define Alt_F3   0x6a00
#define Alt_F4   0x6b00
#define Alt_F5   0x6c00
#define Alt_F6   0x6d00
#define Alt_F7   0x6e00
#define Alt_F8   0x6f00
#define Alt_F9   0x7000
#define Alt_F10  0x7100

/*  -- these are apparently not defined as ASCII on an IBM PC keyboard
#define Alt_UP
#define Alt_DN   
#define Alt_LF   
#define Alt_RT   
#define Alt_HOME 
#define Alt_END  
#define Alt_PGUP 
#define Alt_PGDN 
#define Alt_INS  
#define Alt_DEL
*/

#define Ctrl_F1   0x5e00
#define Ctrl_F2   0x5f00
#define Ctrl_F3   0x6000
#define Ctrl_F4   0x6100
#define Ctrl_F5   0x6200
#define Ctrl_F6   0x6300
#define Ctrl_F7   0x6400
#define Ctrl_F8   0x6500
#define Ctrl_F9   0x6600
#define Ctrl_F10  0x6700

/* #define Ctrl_UP   -- does not exist */
/* #define Ctrl_DN   -- does not exist */
#define Ctrl_LF   0x7300
#define Ctrl_RT   0x7400
#define Ctrl_HOME 0x7700
#define Ctrl_END  0x7500
#define Ctrl_PGUP 0x8400
#define Ctrl_PGDN 0x7600
/* #define Ctrl_INS  -- does not exist */
/* #define Ctrl_DEL  -- does not exist */


/* miscellaneous */

#define CR  '\r'
#define BS	'\b'
#define ESC '\x1b'
#define SPC ' '
#define BLANK ' '
#define TAB '\t'
#define Shift_TAB 0x0f00

#define EOL '\0'
#define APOSTROPHE '\''
#define QUOTE '"'


