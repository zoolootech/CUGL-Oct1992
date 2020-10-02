/* WSCREEN.H
 *	This header is only used in those routines that directly access
 *		the screen, or alter parameters that control screen access.
 *	Any routines that do not #inlcude this header don't access the screen
 *
 */

#define  __WSCREEN__



	#define HERC_TEXT_RAM  	(unsigned char far *)(0xB000L << 16)
	#define EGA_TEXT_RAM      (unsigned char far *)(0xB800L << 16)

	#define	HERC_GRAPH_RAM	( (unsigned char far *) 0xb0000000L )
	#define EGA_GRAPH_RAM 	( (unsigned char far *) 0xa0000000L )



/* where the mouse driver looks to figure out which video page we're on
 *	only used in HERCULES monitiors in graphics modes...
 *	this is the BIOS video mode indicator ... not used otherwise on HERC
 *	so place 6 here for page 0 and 5 here for page 1
 *
 */
#define HERC_ROM_VIDMODE_AREA  ((unsigned char far *) 0x00400049L )


/* font tables: 8*128 bytes, each char 8x8 bits, 8 bytes per char */
#define ROM_FONT 	( (unsigned char far *) 0xf000fa6eL )

/* write to EGA control register and bit plane enable.
 */
#define EGA_OUT(index, val)  outp(0x3ce, (index)), outp(0x3cf, (val));
#define EGA_BITPLANE(planes) outp(0x3c4, 2),       outp(0x3c5, (planes) );

#define EGA_ENABLE 	    0x0f

/* ------------------- END of WSCREEN.H ------------------------ */

