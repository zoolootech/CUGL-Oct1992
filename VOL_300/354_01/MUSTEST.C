#include <conio.h>
#include <stdlib.h>
#include <cesmouse.h>

int lft,ctr,rgt,mx,my,but,nb;

main()

{
    clrscr();

    printf("CES Mouse Library Test\n");
    printf("Press Any Key to Exit\n\n");

	/* Check for the Mouse Driver */

	if (mousedrvr() == 0)
	  {
          printf("Mouse Driver Not Installed\n\n");
	      exit(1);
	  }

	/* Reset the mouse.. All registers to '0'  */

   nb = resetmouse();
   printf("Number of mouse buttons: %d",nb);

	/* Position the Mouse */
	posmouse(320,96);

	/* Show the Mouse */
	showmouse();


    gotoxy(30,8);
    cprintf("Button Values: ");
    gotoxy(30,10);
    cprintf("Left Button: ");
    gotoxy(30,11);
    cprintf("Middle Button: ");
    gotoxy(30,12);
    cprintf("Right Button: ");
    gotoxy(30,14);
    cprintf("X Value: ");
    gotoxy(30,15);
    cprintf("Y Value: ");

	/* Output Mouse Button Values */

	for(;;)
		{
		lft = lftbutton();
		ctr = ctrbutton();
		rgt = rgtbutton();
		mx = mousecursX();
		my = mousecursY();
		but = buttons();

            gotoxy(46,8);
            cprintf("%d",but);
            gotoxy(46,10);
            cprintf("%d",lft);
            gotoxy(46,11);
            cprintf("%d",ctr);
            gotoxy(46,12);
            cprintf("%d",rgt);
            gotoxy(46,14);
            cprintf("    ");
            gotoxy(46,14);
            cprintf("%d",mx);
            gotoxy(46,15);
            cprintf("    ");
            gotoxy(46,15);
            cprintf("%d",my);


		if (kbhit()) break;
		}

	/* Hide the Mouse */
	hidemouse();

	/* Reset the Mouse */
    nb = resetmouse();


    exit(0);

}
