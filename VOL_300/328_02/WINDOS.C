/* w_inDOS: test for inDOS flag.
 * NOTE: for programs that use int 0x21 to read the keyboard,
 * 	 this routine will return TRUE when waiting for input.
 *	 The window programs call wgetc() or wreadc()
 *       	which bypasses DOS, reaading keyboard directly.
 *
 *	Otherwise, one could install an int 0x28 handler
 *	which DOS calls whenever waiting for a keypress,
 *	and the handler could somehow communicate back that
 *	state is inDOS but waiting for keys...
 */
#ifndef __TURBOC__ 
	#error  SORRY, does not compile under Microsoft C.
#endif
 
#include "wsys.h"


char w_inDOS (void)
	{
	unsigned  int seg, off;
	char far *flag;

	_AH = 0x34;			/* function x34 = get inDOS flag addr */
	INTERRUPT (0x21);	/* interrupt DOS */
	off = _BX;
	seg = _ES;			/* NOTE: must be TURBOC not Microsoft */

	flag = MK_FP ( seg,off );

	return ( *flag );	/* inDOS */
	}
