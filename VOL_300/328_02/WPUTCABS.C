/*! wputcabs()
 *	Put char, attr at screen position x,y (screen physical co-ords)
 *	Works in text or graphics mode. In either mode, co-ords are text-mode
 * 	co-ords (not pixel co-ords). Putmode is ignored in text mode.
 *	In graphics mode, putmode can be: WGOVERWRITE, WGOR, WGXOR, WGAND
 */


#include  "wscreen.h"
#include  "wsys.h"







void wputcabs ( int x, int y, unsigned char c, unsigned char a,
	unsigned char putmode )
	{
	register unsigned char far *p;

#ifndef TEXTONLY

	/* declarations for graphics mode
	 */
	unsigned int 	hn;
	int 		row;	/* 0-7, which row of bits in the bitmap */


	unsigned char foreground, background;	/* attributes */

	unsigned char far *font;  	/* bit map of current row in font */

	unsigned char far *base;
	unsigned char      mask; 	/* EGA register mask and latch byte*/
	unsigned char 	   latch;



	unsigned char hmask;

#endif /* end declarations for graphics mode */




	if ( wmode == 'T' )
		{
		p = wpage_ram + 2*80*y + 2* x;
		*p = c;
		*(++p) = a;

		return;
		}


#ifndef TEXTONLY
	/* graphics mode wputcabs */



	if  ( x < 0 || y< 0 || x > wxabsmax || y > wyabsmax )
		{
		return;
		}

	#if wpychar == 8

	if  ( c > 127 )
		{
		font = (unsigned char far *) whifont +
			( ((int)(c) - 128)<<3 );
		}
	else
	if ( c < 32 )
		{
		font = (unsigned char far *) wlofont +
			( c<<3 );
		}
	else 	{
		font = ROM_FONT + ( ((int) c) <<3) ;
		}
	#else
		#error("Need 16-bit hi font tables ");
	#endif	/* wpychar */


	foreground =   a & 0x0f;	/* separate attribute byte */
	background = ( a & 0xf0 ) >> 4;

	switch ( wmonitor )
		{
	case ( 'H' ):
		/* NOTE addressing hercules graphics ram:
		 *	x, y are 'text-mode' addresses.
		 *	720 pixels/row, wpxchar pixels per 'x' increment
		 *		so 1st factor = wxabsmax = chars per row
		 *	divide wpychar by 4(=# herc banks) =
		 *		gives #of sets of banks (4 pixel rows each)
		 *	wpxchar/8 =1 always, included only for clarity.
		 */
		base	=  wpage_ram
			+ ( (720/wpxchar)*(wpychar/4)*y ) + x ;

		/* in hercules, 'dither' blue & green bits */
		foreground = (foreground & 0x03) ? 0xff : 0x00;
		background = (background & 0x03) ? 0xff : 0x00;

		for ( 	row = hn = 0;
			row < wpychar;
			++row, ++font, hn +=0x2000 )
			{
			if ( hn == 0x8000 )
				{
				/* finished one set of 4 banks,
				 * move up to next tier.
				 */
				hn = 0;
				base += 90;
				}

			hmask  = ( (  *font ) & foreground )
					 | ( (~ *font ) & background )  ;

			switch (putmode)
				{
			case (WGOVERWRITE):
				*(base+hn) = hmask;
				break;
			case (WGXOR):
				*(base+hn) ^= hmask;
				break;
			case (WGOR):
				*(base+hn) |= hmask;
				break;
			case (WGAND):
				*(base+hn) &= hmask;
				break;

				}/* end switch ( putmode ) */

			}/* end loop for hercules rows */
		break;   	/* end case (hercules) */

	case ( 'E' ):
	case ( 'V' ):


		base = wpage_ram + ( wegarowsize * wpychar * y ) + x;



		EGA_OUT (1, EGA_ENABLE);
		EGA_BITPLANE (0x0f);
		EGA_OUT (3, putmode);	/* OVERWRITE, OR, AND, etc.. */

		for ( 	row = 0;
			row < wpychar;
			 ++row, base += 640/wpxchar, ++font )
			{

			/* write foregraound */
			mask   = *font;
			latch  = *base;         /* latch data to EGA regs */
			EGA_OUT (0, foreground);/* color */
			EGA_OUT (8, mask);	/* mask ON bits from font map */
			*base  = 0xff;		/* change as governed by mask */

			/* write background */
			mask   =  ~mask;	/* invert mask */
			latch  = *base;         /* latch data  */
			EGA_OUT (0, background);
			EGA_OUT (8, mask);
			*base = 0xff;

			}

		/* reset EGA */
		EGA_OUT (0,0);			/* BLACK    */
		EGA_OUT (3, 0x0);	        /* OVERWRITE*/
		EGA_OUT (8, 0xff);              /* all bits */


		break;
		} 	/* end switch (wgdriver) */


#endif	/*TEXTONLY - end of graphics mode wputcabs */





	return; /* wputcabs */


	}
/*--------------------- end of WPUTCABS.C -------------------------*/
