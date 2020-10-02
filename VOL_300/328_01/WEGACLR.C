/* wegaclr.c
 *		detect whether an EGA monitor is connected to a color screen
 */

#include  "wscreen.h"
#include  "wsys.h"

#define ROM_EGA_DATA  *( (unsigned char far *)0x00000487L )

int w_ega_color(void)
	{
	unsigned char retcode = ON;

	switch ( wmonitor )
		{
		case ('E'):
			if (0x20  & ROM_EGA_DATA)
				{
				retcode =OFF;
				} 

			break;
		case ('H'):		/* fall thru for all monochrome adapters */
		case ('M'):
			retcode = OFF;
			break;
		}
	return (retcode);
	
	}

/*----------------end WEGACLR.C -----------------------*/


 