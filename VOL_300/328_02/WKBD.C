/*! wkbd.c       low-level keyboard access
 *
 *	get a key from the keyboard
 *
 *	wkbd_init() - detect enhanced kbd. setup interrupts as needed.
 *      wread_kbd() - read a keystroke
 *	wready_kbd()- see if a keystroke is waiting
 *	wenhanced_kbd() - see if enhanced keyboard is present.
 *	wflush_kbd() - flushes all keystrokes from the kbd buffer.
 *
 * NOTE: enhanced kbd support added for version 1.1, July 1990
 *	
 *	The method used in VERSION_10 is only available in TurboC, not Microsoft.
 *	 TurboC lets program check the zero flag, needed for wready_kbd()
 *	 Microsoft doesn't allow this, so can't check for wready_kbd()
 *	  in MIcrosoft version, call is interrupt 0x21 (DOS) instead.
 *		(using the microsoft library function kbhit() )
 *
 *	In VERSION_11, a new method uses direct access to BIOS memory areas
 *	as described in the C User's Journal July 1990 article, "Controlling the
 *	Keyboard Buffer" by Steve Gruel. This method works in either Turbo C
 *	or Microsoft C. 
 *
 *	To revert to BIOS calls using interrupt x16, undefine the symbol VERSION_11
 *
 *					@ David Blum, 1986, 1989, 1990
 */

#include "wsys.h"

/* Option to use BIOS MEMORY AREAS rather than interrupts.
 */
#define VERSION_11  1.1


/* enhanced keyboard support -
 * if enh. kbd is present, change these interrupt functions to x10, x11, etc
 */
static unsigned char   W_NEAR   func_get = 0;




#ifdef VERSION_11
	/* Keyboard buffer location pointers, stored in BIOS
	 * 	the buffer is a revolving buffer located at 0x0040:001E 
	 *		with an offset to the next empty spot (tail)
	 *		and  to oldest keystroke in the buffer (head)
	 *	The index range is 30->60, 2 bytes per key.
	 *	The next keystroke is at 0x0040:K_HEAD
	 *	The offsets for HEAD and TAIL are stored at 0x0040:001A and 0x0040:001C
	 *
	 * The buffer is empty if the BIOS pointers to head and tail are equal.
	 */
	#define K_HEAD  *((int far *)0x0040001AL)
	#define K_TAIL  *((int far *)0x0040001CL)

	#define KBD_BUFFER_EMPTY     ( K_HEAD == K_TAIL )
	#define KBD_BUFFER_NOTEMPTY  ( K_HEAD != K_TAIL )
#else
	/* in early version, need to have a seperate interrupt function
	 * for reg. versus enh. kbd.  VALUE IS 0x01 if reg kbd, 0x11 for enh kbd.
	 */
	static unsigned char   W_NEAR   func_chk = 1;
#endif  /* VERSION_11 */





#ifdef VERSION_11
void wkbd_init ( void )
	{

	/* logic is to stuff a char in the kbd buffer
	 * and use the enhanced check function to see if
	 * anything is there
	 */
	int n;
	unsigned char key, scancode;
	PSEUDOREGS	
	
	/* flush keyboard by manipulating keyboard buffer
	 * (more compact than calling wflush_kbd() in large model.)
	 */
	K_TAIL = K_HEAD;


	/* write to enh. kbd buffer
	 *  CX contains scancodes and keycodes that are 'enhanced characters'
	 */
	_AH = 0x05;
	_CH = 0xff;
	_CL = 0xff;
	INTERRUPT ( 0x16 );


	/*try to read back from the enhanced kbd. 
	 * If 16 tries doesn't return xff written above, then isn't enh. kbd.
	 */
	for ( n=0; n<16; ++n ) 	/* note: 16 = size of kbd buffer */
		{
		_AH = 0x10;
		INTERRUPT ( 0x16 );
		key = _AL;
		scancode = _AH;
		if ( key == 0xff  && scancode == 0xff )
			{
			/* is enhanced kbd, so change function request codes.
			 */
			func_get = 0x10;
			/* dont need to set func_chk = 0x11 in version 1.1 */

			break;
			}
		}

	return;		/* wkbd_init, version 1.1 */
	}


#else
	/* NOT VERSION_11, so Microsoft C cant detect enh. kbd. */

void wkbd_init ( void )
	{

	#ifdef __TURBOC__


	/* logic is to stuff a char in the kbd buffer
	 * and use the enhanced check function to see if
	 * anything is there
	 */
	int n;
	unsigned char key, scancode;
	

	wflush_kbd ();
	

	/* write to enh. kbd buffer
	 *  CX contains scancodes and keycodes that are 'enhanced characters'
	 */
	_AH = 0x05;
	_CH = 0xff;
	_CL = 0xff;
	INTERRUPT ( 0x16 );


	/*try to read back from the enhanced kbd. 
	 * If 16 tries doesn't return xff written above, then isn't enh. kbd.
	 */
	for ( n=0; n<16; ++n ) 	/* note: 16 = size of kbd buffer */
		{
		_AH = 0x10;
		INTERRUPT ( 0x16 );
		key = _AL;
		scancode = _AH;
		if ( key == 0xff  && scancode == 0xff )
			{
			/* is enhanced kbd, so change function request codes.
			 */
			func_get = 0x10;
			func_chk = 0x11;

			break;
			}
		}


	#endif	/* TurboC */
	/* if microsoft, don't check for enh kbd (can't use anyway)
	 *  just leave func_get = 0x00 and leave func_chk = 0x01.
	 */


	return;		/* wkbd_init, version 1.0 */
	}
#endif		/* else not VERSION_11 */





int wkbd_enhanced (void)
	{
	return (func_get);	/* 0 for non-AT keyboards */
	}


/* wready_kbd()
 *
 *	tests keyboard only o see if a key has been hit.
 *
 */
int wready_kbd(void)
	{
#ifdef VERSION_11
		/* a key is ready if buffer head is not same as buffer tail.
		 */
		return ( KBD_BUFFER_NOTEMPTY );	/* wready_kbd */
#else	
		/* VERSION 1.0 logic, tests zero flag, only works for TurboC */
	#ifdef __TURBOC__

		register int n;

		_AH = func_chk;    /* service code for checking wready */
		INTERRUPT (0x16);
		n = _FLAGS;
		return ( (n & 0x40) ? 0 : 1 );

	#else /* MICROSOFT C - compiler can't test the zero flags
		   */
			#include <conio.h>
			return ( kbhit() );

	#endif /* VERSION 1.0 check kbd for not TURBOC */
#endif 	/* else not VERSION_11 */

	}




/*! wread_kbd ()
 *
 *	read keyboard, map extended key codes
 *
 *	NOTE for std/enhanced keyboard:
 *		The std keybd returns keys 0-127, 0 means check scancode.
 *		The enh keybd returns keys 0-255, 0 means check scancode.
 *			but key values 128 & up
 *			    diferentiate num pad from cursor
 *
 *		The asignment using signed and unsigned chars bypasses this
 *		so numeric keypad and cursor keypad are the same.
 *		(that's how most users think about them anyways)
 */
int wread_kbd (void)
	{
	/* NOTE use of signed & unsigned to provide correct conversions.
	 */
	signed char key;
	unsigned char  scancode;
	register int      retval;

	PSEUDOREGS

	_AH = func_get;  /*service requested is get data */
	INTERRUPT ( 0x16 );
	key =      (signed char) _AL;
	scancode = (unsigned char) _AH;

	/* key mapping to place all keys into one return value
	 */
	retval = (key>0) ?  key : 128+scancode ;

	return (retval);
	}


void wflush_kbd (void)
	{
	#ifdef VERSION_11
	/* flush keyboard by manipulating keyboard buffer
	 */
	K_TAIL = K_HEAD;
	
	#else
	/* flush keyboard using interrupts.
	 */
	while ( wready_kbd()  )
		{
		_AH = func_get;
		INTERRUPT ( 0x16 );	/* flush kbd */
		}
	#endif 	/* not VERSION_11 */
	}




	/*------------- end of wkbd.c --------------------------*/
