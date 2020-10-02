/* wsysrqa.c - code for wSysRq_ask()
 * 	this is a sample 'stub' function for insertion into the SysRq interrupt 
 * 	This funtion asks 'do you want to quit'
 *  and quits if the answer is yes.
 */

#include "wsys.h"
 

void wSysRq_ask (void)
	{
	register int key;

	wflush_kbd();

	wopen (15,2,50,5,YELLOW+(RED<<4),SPECKLE_BORDER,YELLOW+(RED<<4),WSAVE2RAM);
	wtitle ( " SysRq ");
	wputs (	"\n              CTRL_C to quit program"        
			"\n         ENTER and ALT-SysRq to continue"
			"\n" 
			"\n  Remember to press ALT-SysRq to unlock keyboard" );
	
	while ( ! ( (key=wread_kbd())  == CTRL('C') || key == ENTER) );

	if (key == CTRL('C')) exit (0);
	
	wclose();
	
	return;		/* wSysRq_ask */
	}

/*---------------------- end of wsysrqa.c -------------------------*/
