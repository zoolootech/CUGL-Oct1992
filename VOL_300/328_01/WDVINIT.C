/* wdvinit.c 
 * 		Initialization code for DeskView aware programming.
 *		Call this routine immediately after call to winit()
 *		...ie, BEFORE any direct screen writes.
 * 		Incompatible with multiple page display (winit_pages)
 *		untested in all graphics modes.
 *	
 *	METHOD:	(see the deskview manual, ver 2. page 195)
 *		generate an invalid request to DOS setdate.
 *		DOS returns unusual error code implies DeskView is present
 *		interrupt func 0x10 subfunc 0xFE to get deskview virtual buffer.
 *		NOTE: this function works only if winit() does NOT write to the screen.
 *
 *	RETURNS: 0     = not a DeskView task
 *			 non_0 = a DeskView task (#is DeskView (major) version number 
 */
 
#ifndef __TURBOC__ 
	#error Sorry Microsoft users - works in TurboC only
#endif 	/* TURBOC */
 
#include "wsys.h"

int wdvinit (void)
	{
	unsigned int 	video_ram_seg, video_ram_off;
	int          	major_version;
	unsigned int 	valid_date;
	PSEUDOREGS
	
	_DX = 0x5351;		/* an invalid date */
	_CX = 0x4445;		/* DOS set date function */
	_AX = 0x2B01;
	INTERRUPT ( 0x21 );
	valid_date = _AL;
	major_version = _BH;
	
	if ( valid_date != 0xff )
		{
		/* deskview is present */
		_AH = 0xFE;
		INTERRUPT ( 0x10 );		/* get deskview buffer address */
		video_ram_seg = _ES;
		video_ram_off = _DI;
		wvideo_ram = wpage_ram = MK_FP ( video_ram_seg, video_ram_off );
		}
	else 
		{
		major_version = 0;
		}
	return (major_version);		/* wdvinit */
	}
	/*------------------- end of DeskView initialization -----------------*/
