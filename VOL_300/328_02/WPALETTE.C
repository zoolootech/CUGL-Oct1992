/* wpalette.c
 *		contains EGA/VGA routines wsetpalette() and wgetpalette()
 *
 *					@ David Blum, 1990
 */
#include "wsys.h"






void wgetpalette ( unsigned char palette[17] )
	{
		#ifdef __TURBOC__
		/* this structure and union allow
		 * breaking an address into seg:offset components
		 * in any memory model
		 */
		struct p_struct
			{
			unsigned int off;
			unsigned int seg;
			};
		union   far_ptr
			{
			void   far      *addr;
			struct p_struct p;
			}
			fp;
		/* set all 16 colors and overscan at once
		 *
		 * first get addr. of palette save area
		 *     in small or medium model, could
		 *     just assign _ES = _DS; _DX= save_palette;
		 *     but wouldn't work in compact or large model
		 *
		 */
		fp.addr = (void far *) palette;
		_ES     = fp.p.seg;
		_DX 	= fp.p.off;
		_AH     = 0x10;
		_AL     = 0x09;
		INTERRUPT (0x10);
			

	#else

		/* not turboc - save current palette one color at a time*/
		char temp;
		int n;
		PSEUDOREGS
		for ( n= 0; n <16; ++n )
			{
			_BL = n;
			_AH = 0x10;
			_AL = 7;	/* get one palette color */
			INTERRUPT (0x10);
			temp = _BH;            	/* safety precaution */

			palette[n] = temp;

			}
		/* save overscan */

		_AH = 0x10;
		_AL = 8;    		/* read overscan color */
		INTERRUPT (0x10);
		temp = _BH;
		palette[16] = temp;


	#endif /* TURBOC */

		return;		/* wgetpalette() */
		}
		



void wsetpalette ( unsigned char palette [17] )
	{
		
	#ifdef __TURBOC__

		/* this structure and union allow
		 * breaking an address into seg:offset components
		 * in any memory model
		 */
		struct p_struct
			{
			unsigned int off;
			unsigned int seg;
			};
		union   far_ptr
			{
			void   far      *addr;
			struct p_struct p;
			}
			fp;


			/* set palette 
			 */
			fp.addr = (void far *) palette;
			_ES     = fp.p.seg;
			_DX 	= fp.p.off;
			_AH     = 0x10;
			_AL     = 0x02;
			INTERRUPT (0x10);
	
	#else
	/* NOT turbo C so do it one step at a time.
	 */
	int n;
	char temp;
	PSEUDOREGS
	
		/* Not TurboC 
		 */
		for ( n= 0; n <16; ++n )
			{
			
			_BH = palette [n];
			_BL = n;
			_AH = 0x10;
			_AL = 0;
			INTERRUPT (0x10);
			}
		_AH = 0x10;
		_BH = palette[16];		/* set overscan color */
		_AL = 1;
		INTERRUPT (0x10);
		
	#endif
	
	return;		/* wgetpalette () */
	
	}
	
	
	/*----------------- end of wpalette.c ----------------------*/



