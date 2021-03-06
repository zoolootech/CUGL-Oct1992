/******************************************************************************
*******************************************************************************


		     THIS UTILITY SELECTS THE ACTIVE DISPLAY
	     USAGE:  switch [MONITOR] [MODE]

		     [MONITOR] = monochrome or color.
			type in either mono or monochrome,
				or col or color for color display.
		      *****MUST BE IN LOWER CASE*****

		     [MODE] = (FOR COLOR ONLY)
			0     40 * 25 BLACK AND WHITE
			1     40 * 25 COLOR
			2     80 * 25 BLACK AND WHITE
			3     80 * 25 COLOR
			4     320 * 200 COLOR  (MED. RES.)
			5     320 * 200 BLACK AND WHITE  (MED. RES.)
			6     640 * 200 BLACK AND WHITE  (HIGH RES.)

*******************************************************************************
****		  THIS PROGRAM BROUGHT TO YOU BY THE			   ****
****			  THE UNHOLY PIRATE				   ****
*******************************************************************************





*******************************************************************************
******************************************************************************/


#include "stdio.h"
#include "conio.h"
#include "utility.h"
#include "screen.h"
#include "string.h"
main(argc, argv)
int argc;
char **argv;
{
	ADS equip_flag, new_equip_flag;
	int comlinptr, ax, bx, cx, dx, mode, columns, page;
	unsigned flag;

	if(argc < 2)/*check for command line argument*/
		utabort("NO COMMAND LINE ARGUMENT  switch [MONITOR] [MODE]");

	equip_flag.s = 0x0040;/*rom bios data segment*/
	equip_flag.r = 0x0010;/*offset*/
	new_equip_flag.s = _defds;/*default data segment*/
	new_equip_flag.r = &flag;


	if((stsindex("mono", argv[1])) == 0){/*switch to monochrome*/
		if(!scmode(&mode, &columns, &page))/*check not on monochrome*/
			utabort("ALREADY ON MONOCHROME ADAPTER");
		bios(17, &ax, &bx, &cx, &dx);/*get current flag into ax*/
		flag = ax | 0x0030;/*set equipment flag to monochrome*/
		utslmove(&new_equip_flag, &equip_flag, 4);/*return flag*/
		screset(0);/*bios set mode call*/
		exit();
	}

	if((stsindex("col", argv[1])) == 0){/*switch to color display*/
	     if(argc != 3 || argc > 3)/*correct number of arguments?*/
		utabort("NO COMMAND LINE ARGUMENT  switch [MONITOR] [MODE]");
	     if(scmode(&mode, &columns, &page))/*check not on color already*/
		      utabort("ALREADY ON COLOR ADAPTER");
	     utslmove(&equip_flag, &new_equip_flag, 4);
	     flag = flag ^ 0x0030;/*erase old display info*/
	     flag = flag | 0x0020;/*set as 80 * 25 color card*/
	     utslmove(&new_equip_flag, &equip_flag, 4);/*return flag*/
	     stcd_i(argv[2], &comlinptr);/*set com line arg to integer*/
	     screset(comlinptr);/*set mode from command line*/
	     exit();
	}
	utabort("NO COMMAND LINE ARGUMENT  switch [MONITOR] [MODE]");
}





                
