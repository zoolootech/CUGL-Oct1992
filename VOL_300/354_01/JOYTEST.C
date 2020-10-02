#include <dos.h>
#include <bios.h>
#include <conio.h>
#include <\c_work\cesjoy.h>



int x, y, b1, b2, xx, yy, b3, b4;

main()


{
    clrscrn();
    poscurs(19,20);
	cprintf("PC Joy Stick Test, Copyright 1989 C.E.S.");
    poscurs(20,28);
	cprintf("Press any key to exit.");

	for(;;)
		{
		x = getjoyAX();
		y = getjoyAY();
		b1 = getjoyAB1();
		b2 = getjoyAB2();
		xx = getjoyBX();
		yy = getjoyBY();
		b3 = getjoyBB1();
		b4 = getjoyBB2();

        poscurs(5,10);
        cprintf("Joystick #1. X= %3d", x);
        poscurs(6,10);
        cprintf("Joystick #1. Y= %3d", y);
        poscurs(8,10);
        cprintf("Joystick #1, Button #1= %1d", b1);
        poscurs(9,10);
        cprintf("Joystick #1, Button #2= %1d", b2);
        poscurs(11,10);
        cprintf("Joystick #2. X= %3d", xx);
        poscurs(12,10);
        cprintf("Joystick #2. Y= %3d", yy);
        poscurs(14,10);
        cprintf("Joystick #2, Button #1= %1d", b3);
        poscurs(15,10);
        cprintf("Joystick #2, Button #2= %1d", b4);


		if (kbhit()) break;
		}

    clrscrn();
}

