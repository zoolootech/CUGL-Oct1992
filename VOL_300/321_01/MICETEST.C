#include <stdio.h>
#include "moustrap.h"

#include <graph.h>
#pragma comment(lib,"graphics")


main ()
{
	mouse_t	w,x,y,z,c;
	int	oldcolor;


/* Set up screen for demo, using Microsoft GRAPHICS library		*/	
	
	oldcolor=_gettextcolor();

	_clearscreen(_GCLEARSCREEN);

	for (x=16; x<26; ++x) {
		_settextposition(x,21);
		_settextcolor(2);
		_outtext("LLLLLLLLLL");
		_settextcolor(4);
		_outtext("RRRRRRRRRR");
		}
	
	
	_settextcolor(1);
	for (x=6; x<17; ++x) {
	        _settextposition(x,11);
		_outtext("11111111111");
		}
	

	_settextposition(7,1);
	printf("Page Z:");

	_settextcolor(3);
	for (x=8; x<15; ++x) {
		_settextposition(x,14);
		_outtext("CCCCCC");
		}
	
	_settextposition(24,10);
	printf("Page Y:");



	_settextposition(7,35);
	printf("Middle Button Anywhere       = '2'");

	_settextposition(9,35);
	printf("Left Button Anywhere in Z    = '1'");

	_settextposition(10,35);
	printf("Right Near Middle of Z       = 'C'");

	_settextposition(12,35);
	printf("Left Button, Left  side of Y = 'L'");

	_settextposition(13,35);
	printf("Left Button, Right side of Y = 'R'");

	_settextposition(14,35);
	printf("Click Middle buttom to Move on");
	
/*************************************************************************/

	
	Define_Mouse_System(M_Single_Pages);

	y=Add_Mouse_Page(M_Text_Coord,15,20,24,40); 

	z=Add_Mouse_Page(M_Text_Coord,5,10,15,20);


	Set_Mouse_Text_Cursor(0,0,TC(' ',4,4));

	Add_Mouse_Button(0,M_Middle,'2');
	Add_Mouse_Button(z,M_Left,'1');
	Add_Mouse_Button(z,M_Right,0);
	Add_Mouse_Hot_Spot(z,M_Right,7,13,13,18,'C');

	Add_Mouse_Button(y,M_Left,0);

	Add_Mouse_Hot_Spot(y,M_Left,15,20,24,30,'L');
	Add_Mouse_Hot_Spot(y,M_Left,15,30,24,40,'R');

	_settextposition(3,50);
	printf("# of buttons = %d\n",_mouse_there);


	
do {
	

	_settextposition(1,10);
	printf("Page Z Active");

	
	Activate_Mouse_Page(z);
	
	c=Get_Char_Mouse_Kbd();


	_settextposition(4,50);
	printf("Page Z: Character \"%c\"",c);

 	DeActivate_Mouse_Page(z);  


	Activate_Mouse_Page(y);

	_settextposition(1,10);
	printf("Page Y Active");

	c=Get_Char_Mouse_Kbd();


	_settextposition(5,50);
	printf("Page Y: Character \"%c\"",c);

	DeActivate_Mouse_Page(y);
	
	} while (c!='2');


	Clear_All_Mouse_Definitions();

	Define_Mouse_System(M_Overlaid_Pages);

	y=Add_Mouse_Page(M_Text_Coord,15,20,24,40);

	z=Add_Mouse_Page(M_Text_Coord,5,10,15,20); 

	Add_Mouse_Button(0,M_Middle,'2');
	Add_Mouse_Button(z,M_Left,'1');
	Add_Mouse_Button(z,M_Right,0);
	Add_Mouse_Hot_Spot(z,M_Right,7,13,13,18,'C');

	Add_Mouse_Button(y,M_Left,0);

	Add_Mouse_Hot_Spot(y,M_Left,15,20,24,30,'L');
	Add_Mouse_Hot_Spot(y,M_Left,15,30,24,40,'R');

	_settextposition(3,50);
	printf("# of buttons = %d\n",_mouse_there);

	Set_Mouse_Text_Cursor(0,0,TC('+',4,2));

do {
	

	Activate_Mouse_Page(z);
	
	_settextposition(1,10);
	printf("Page Z Active");

	
	c=Get_Char_Mouse_Kbd();

	_settextposition(4,50);
	printf("Page Z: Character \"%c\"",c);

 	DeActivate_Mouse_Page(z);  


	Activate_Mouse_Page(y);

	_settextposition(1,10);
	printf("Page Y Active");

	c=Get_Char_Mouse_Kbd();

	_settextposition(5,50);
	printf("Page Y: Character \"%c\"",c);

	Activate_Mouse_Page(z);
	
	c=Get_Char_Mouse_Kbd();


	_settextposition(6,46);
	printf("Page Y & Z: Character \"%c\"",c);
	
 	DeActivate_Mouse_Page(y);  

	} while (c!='2');


/***********************************************************************/

	_settextcolor(oldcolor);
}



