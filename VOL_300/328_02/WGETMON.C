/* wdetect_monitor ()
 * 	Detect the type of monitor,
 *		store the info in wmonitor
 *              store amount of EGA ram in wegasize  ( 0 for <256, 1=256k);
 *
 *
 */
#include  "wscreen.h"
#include  "wsys.h"


#include <dos.h>


/* return  codes from interrupt 0x10, function 0x1A subfunction 0
 *	BL = active monitor, BH = inactive mon.
 */
#define FOUND_UNRECOGNIZABLE  0xFF
#define FOUND_NO_MONITOR	0
#define FOUND_MDA			1
#define FOUND_CGA			2
#define FOUND_reserved_1	3
#define FOUND_EGA			4
#define FOUND_EGA_mono		5
#define FOUND_Profess		6
#define FOUND_VGA_mono		7
#define FOUND_VGA     		8
#define FOUND_reserved_2	9
#define FOUND_MCGA_1		0x0A
#define FOUND_MCGA_2		0x0B
#define FOUND_MCGA_3		0x0C






void  	wgetmon (void)
	{


	PSEUDOREGS


	unsigned char adapter;
	int trials;
	int active;
	unsigned char egaram;
	unsigned char egadata;


	/* ROM BIOS equipment list - second word */
	adapter = *( (unsigned char far *) 0x00000411L) & 0x30;

	if (adapter == 0x01)	/* 40 column monitor */
		{
		perror ("Unsupported monitor");
		wmonitor = 'X';
		exit (99);
		}
	/* adapter = 2 for 80x25 text color, adapter = 3 for 80x25 mono */


	egadata = *( (unsigned char far *)0x00000487L );


	/* attempt to read video info
	 * USE A VGA only call.( although some newer BIOS return values
	 *		even if they're hercules or EGA. )
	 */
	_BX = 0;
	_AH = 0x1A;	/* get video info subservice for PS/2, 386, newer AT. */
	_AL = 0;	/* 0=get, 1 = set */

	INTERRUPT (0x10);

	active   = _BL;
	/* the inactive monitor ID is in  _BH, if you should ever switch */
	active &= 0x0f;	/* take low order half */

	if ( active == 0  || active == 0x0f )
		{
		/* BIOS did not recognize the call - could be an old EGA or CGA or H.
		 */
		if ( egadata & 0x80 )		/* it's an EGA */	
			{
			active = FOUND_EGA;
			}
		else
		/* not VGA, and not EGA,... */
		if ( adapter == 2 )
			{
			/* not EGA, VGA, but is color... */
			active = FOUND_CGA;	
			}
		else 
		/* not a color monitor ... */
		active = FOUND_MDA;	
		
		}
		
		
		
	
	
	
	switch (active)
		{
	case (FOUND_VGA):
	case (FOUND_VGA_mono):
		wmonitor = 'V';
		break;

	case (FOUND_EGA):
		wmonitor = 'E';

		egaram= egadata & 0x60 ;

		if (egaram == 0x60)
			{
			wega256 = 1;	/* 256 k avail on-board */
			}
		else
		if (egaram == 00)
			{
			/* only 64k on this pathetic board --
			 * no hi-res graphics 
			 */
			wmonitor = 'C';	/* disables graphics modes */
			}
		else
			{
			wega256 = 0;	/* less than 256 k */
			}


		break;
	case (FOUND_MDA):
		wmonitor = 'M';

		/* in text mode, BIOS thinks hercules is an MDA
		 */


		/* if the vertical retrace bit ever goes hi,
		 * this is a hercules card. MGA keeps this low always.
		 */
		trials = 10000;
		while ( --trials )		/* NOTE loop contains break statement */
			{
			if ( inp(0x3BA)  & 0x80 )
				{
				wmonitor = 'H';
				break;				/* BREAK from while loop */
				}
			}


		break;		/* break from switch */
		
	case (FOUND_CGA):
	case (FOUND_MCGA_1):
	case (FOUND_MCGA_2):
	case (FOUND_MCGA_3):
		wmonitor = 'C';
		break;

	case (FOUND_reserved_1):
	case (FOUND_reserved_2):
	case (FOUND_Profess):
		perror ("Unsupported monitor");
		wmonitor = 'X';
		exit(99);


		}	/* end switch */

	return;



	/* wgetmon */
	}


