/* wSysRrq.c
 *      Sys Rq interrupt handler
 *      Int (0x15), with _AH = 0x85 on entry.
 *		pressed= _AL = 0, released _AL = 1
 *
 *      this interrupt is generated whenever ALT-SysRq is pressed
 *
 *	this routine MUST be compiled under TurboC...
 *		with register variables ON and stack warning OFF
 *		
 *	You may have to increase the stack size if your use a complex function
 *		as your SysRq handler.
 *
 *	It will not work well with programs
 *		that use interrupt 0x21 to read the keyboard.
 *
 *	Your interrupt routine will be ignored when running as a virtual task 
 *		under DeskView 
 *	
 *
 */
#ifndef __TURBOC__
	#error Sorry, this routine will not compile under MICROSOFT C
#endif


#include "wsys.h"

#define INT_SYSRQ  (0x15)


static void interrupt  handler  
		(unsigned bp, unsigned di, unsigned si, unsigned ds, unsigned es, 
		 unsigned dx, unsigned cx, unsigned bx, unsigned ax, unsigned ip,
		 unsigned cs );

static void interrupt  (*old_handler) (void) = NULL; 


static char reentrant =0;

static void (*func)(void) = NULL;	/* function to call */	

static int exit_installed =0;		/* atexit functions */
static void cleanup_SysRq (void);


void  wSysRq_install ( void (*u_func)() )
	{
	disable();
	old_handler = getvect ( INT_SYSRQ );
	setvect ( INT_SYSRQ, handler );
	func = u_func;
	enable();
	
	if ( ! exit_installed )
		{
		exit_installed =1;
		if ( 00 != atexit ( cleanup_SysRq ) )
			{
			werror ('W', "SysRq installation failed");
			}
		}
	return;	   /* wSysRq_install */
	}

void  wSysRq_remove ( void )
	{
	disable();
	setvect ( INT_SYSRQ, old_handler );
	old_handler = NULL;
	func = NULL;
	enable();
	return;	   /* wSysRq_install */
	}


static void cleanup_SysRq (void)
	{
	if ( func )
		{
		wSysRq_remove ();
		}
	return;		/* cleanup_SysRq */
	}




/* Interrupt handler - activated on key press and on key release
 * 	tests for keypress only, makes sure not a double keypress (reentrant)
 * 	tests for inDOS() -- will only work if not using DOS to poll keyboard.
 * 
 *  NOTE: pragmas which follow: the first is OK for TurboC v2.0
 *			others require the TurboC++ version
 *			(just take them out & make sure compiler opts are set correctly)
 */
#pragma warn -par
#pragma -N-
#pragma -r


static void interrupt  handler  
		(unsigned bp, unsigned di, unsigned si, unsigned ds, unsigned es, 
		 unsigned dx, unsigned cx, unsigned bx, unsigned ax, unsigned ip,
		 unsigned cs )
	{

	if ( (ax == 0x8500u) )	
	 	{
		if ( reentrant == 0 )
			{
			if ( ! w_inDOS() ) 
				{
				reentrant = 1;
				if ( func )
					{
					(*func)();
					}
				reentrant = 0;
				}
			}
		}
	(*old_handler)();	
	return;		/* handler */
	}



/*---------------------- end of WSYSRQ.C --------------------------------*/

