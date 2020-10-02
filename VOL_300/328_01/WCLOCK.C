/*! wclock routines
 *
 *
 * PORTABILITY note: The clock function is an interrupt handler.
 *	Turbo C, Microsoft, and a few others have this ability...
 *
 *
 *
 */
#include "wsys.h"

#include <dos.h>
#include <time.h>

#ifndef __TURBOC__
	/* In microsoft C, interrupts must be explicitly declared 'far'
	 * and the compiler can't handle parenthesis on function prototypes
	 * so wclockold is defined as a generic ptr
	 * (which, I think, make the prototype dangerous)
	 */
	static interrupt far wclock(void);
	static void      far *wclockold =NULL;
#else
	/* in TurboC, interrupts  match the model.
	 * and the compiler is able to recognize ptrs to interrupt functions.
	 */
	static void interrupt    wclock    (void);
	static void interrupt  (*wclockold)(void) =NULL;
#endif		/* Microsoft fix */

static int  ch10, ch, cm10, cm;


static int clockx, clocky;
static char clock_inst =0;		/* is clock currently operating ? */
static char exit_inst  =0;		/* has atexit() been installed  ? */


void wclockinstall (int x, int y)
	{
	time_t 		t_now;
	struct tm 	*tm_now;
	
	if ( clock_inst )
		{
		return;
		}
	else
		{
		clock_inst = 1;
		}
		
	if ( ! exit_inst )
		{
		exit_inst = 1;
		atexit ( wclockremove );
		}
		

	/* save the screen location
	 * where the interrupt-driven clock can find it
	 */
	clockx =x;
	clocky = y;

	/* initialize time */
	time( &t_now );
	tm_now = localtime(&t_now);


	if (( ch = tm_now-> tm_hour) >12)
		{
		/*convert from 24 hour clock */
		ch -= 12;
		}

	if ( ch > 9 )
		{
		ch10 = '1';
		ch   = '0'-10 + ch;
		}
	else 	{
		ch10 = '0';
		ch   = '0' + ch;
		}

	cm = '0' + ( (tm_now->tm_min) - 10*((tm_now->tm_min)/10) );
	cm10 = '0' + (tm_now->tm_min)/10;



	/* install clock */
	wclockold = getvect (0x1c);
	disable();
	setvect (0x1c, wclock);
	enable();


	return;
	}

void wclockremove (void)
	{

	if ( ! clock_inst )
		{
		return;
		}

	clock_inst = 0;
	disable();
	setvect (0x1c, wclockold);
	enable();

	return;
	}


/* wclock() function body - 
 * This is the actual clock calculator.
 * (PORTABILITY note: make the function definition match the prototype.)
 */
#ifndef __TURBOC__
	static interrupt far wclock(void)
#else
	static void interrupt    wclock    (void)
#endif		/* Microsoft fix */
	
	/* function body follows 
	 */
	{
	#define TPS	18
	static int tick = 0;
	static int cs=0, cs10 = 0;

	++tick;

	if (tick == TPS/2)
		{
		wputcabs (clockx+2   ,clocky, ' ',  wmenuattr, WGOVERWRITE);
		}
	else
	if (tick == 18)  /* 18.2 ticks per second, correction every 10 s */
		{
		tick = 0;
		if ( ++cs > 9 )
			{
			cs = 0;
			tick -= 2;  /* need 2 extra ticks every 10 secs */
			if ( ++cs10 == 6 ) /* 60 secs */
				{
				cs10 =0;
				if ( ++cm > '9' )
					{
					cm = '0';
					if ( ++(cm10) == '6')
						{
						cm10 = '0';
						if ( ++(ch) >'9')
						   {
						   ch10 = '1';
						   ch =  '0';
						   }
						if (  ch10 =='1'
						   && ch == '3' )
						   {
						   ch10 = ' ';
						   ch = '1';
						   }
						}  /* end 1 hour */
					} /*end 10 minutes */
				}  /* end 1 minute*/
			}  /* end 10 seconds*/
		wputcabs (clockx     ,clocky, ch10, wmenuattr, WGOVERWRITE);
		wputcabs (clockx+1   ,clocky, ch,   wmenuattr, WGOVERWRITE);
		wputcabs (clockx+2   ,clocky, ':',  wmenuattr, WGOVERWRITE);
		wputcabs (clockx+3   ,clocky, cm10, wmenuattr, WGOVERWRITE);
		wputcabs (clockx+4   ,clocky, cm,   wmenuattr, WGOVERWRITE);
		} /* end 18 ticks/sec*/

	return;		/* wclock */
	} 
/*--------------------------- END WCLOCK--------------------------*/

