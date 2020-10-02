
/* NONMOUSE.C is a dummy replacement for MOUSELIB.C for people who */
/* don't have mice.  Written 4/89 by T. Clune.  No rights reserved. */
/* inpause() and mouse_gets() dummies added 9/89 */


#include "mouselib.h"


mstruc button_clicks(button)
int button;
{
    mstruc b;

    printf("This system does not support mouse operation.  Press any key.\n");
    getch();

    return b;

}




mstruc button_read()
{
    mstruc b;

    printf("This system does not support mouse operation.  Press any key.\n");
    getch();

    return b;
}


int inpause()
{
    int i;

    printf("This system does not support mouse operation.  Press any key.\n");
    i=getch();

    return i;
}

mstruc m_install()
{

    mstruc m;

    printf("This system does not support mouse operation.  Press any key.\n");
    getch();

    return m;

}





mstruc mdpos_read()
{

    mstruc m;

    printf("This system does not support mouse operation.  Press any key.\n");
    getch();

    return m;

}



void mouse_clear()
{

    printf("This system does not support mouse operation.  Press any key.\n");
    getch();

}


char * mouse_gets(string)
char string[];
{
    printf("This system does not support mouse operation.  Press any key.\n");
    getch();

    return string;
}



mstruc mouse_response()
{
    mstruc ret_val;

    printf("This system does not support mouse operation.  Press any key.\n");
    getch();

    return ret_val;

}

