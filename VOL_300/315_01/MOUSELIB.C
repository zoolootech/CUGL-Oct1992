
/* mouses() functions changed to cmousel() in conformity with new */
/* Microsoft mouse driver, dated 1-23-89.  Change by T. Clune, 6/89. */
/* mouse_gets() added to check for mouse button termination of an */
/* input sequence and inpause() added for mouse or key pause by T. Clune */

/* mouse_clear() and mouse_response() added 3/89 by T. Clune.  Unlike */
/* the other functions in MOUSELIB, mouse_clear() & mouse_response() */
/* are not just repackaged Microsoft mouse library basic functions. */

/* button_clicks() added 11/88 by T. Clune */

/* mouselib.c is a series of three functions for using the microsoft */
/* mouse with the microsoft-supplied mouse.lib.  Written by T. Clune, 12/87 */
/* Primary Documentation: _Microsoft Mouse User's Guide_, (subtitle: for IBM */
/* Personal Computers) p. 175 ff. May be out of print, as the new mice don't */
/* seem to include a copy of this book. The book is by Microsoft, copyright */
/* 1984 and 85, no edition given.  Document Number: 990973001-000-R00-0785. */


#include <stdio.h>  /* putc() macro def. */
#include <conio.h>  /* header for kbhit() and getche() */
#include "mouselib.h"
#include "keys.h"
#include "menu.h"   /* get_mouse_flag() header */

extern int cmousel(); /* the Microsoft mouse.lib large-model C func */


/* button_clicks() reads the number of button clicks for BUTTON */
/* since the last call to button_clicks(BUTTON).  Added 11/88 by T. Clune */
/* NOTE WELL: BUTTON is normalized to the LEFT_BUTTON, RIGHT_BUTTON definitions */
/* even though Microsoft uses different values for this function and the */
/* button_status() function below. */
/* The returned data is: b.opcode= current status of button (0=UP, 1=PRESSED) */
/* b.status = number of clicks since last call to function */
/* b.dx, b.dy=mouse position (virtual screen coords) at last button press */

mstruc button_clicks(button)
int button;
{
    mstruc b;
    b.opcode=BUTTON_CLICKS;
    if(button==LEFT_BUTTON)
	b.status=0;
    if(button==RIGHT_BUTTON)
	b.status=1;
    if((button != LEFT_BUTTON) && (button != RIGHT_BUTTON))
	b.status= -1;
    else
	cmousel(&b.opcode, &b.status, &b.dx, &b.dy);
    return b;

}



/* button_read().status returns current condition of the mouse buttons */
/* the .dx and .dy struct elements are position (NOT CHANGE IN POSITION) of */
/* the "cursor" in virtual coords */

mstruc button_read()
{
    mstruc b;

    b.opcode=BUTTON_STATUS;
    cmousel(&b.opcode, &b.status, &b.dx, &b.dy);
    return b;
}



/* inpause() waits for a keypress or a  mouse button press, then returns */
/* the key if a keyboard entry was made, or <CR> if RIGHT mouse button was */
/* pressed, ASCII 127 for any other button.  The value to return with a */
/* mouse button press was a hard choice for me.  A '\0' is used for */
/* function keys and alternate keys like the cursor keys as the first */
/* of two arguments.  An '\n' is not normally returned by the keyboard, */
/* because keyboards are by default opened as binary devices.  So <CR> */
/* seemed the least objectionable value to use. 127 just seemed like a */
/* safe no-op. inpause() is a substitute for getch(). */

int inpause()
{
    int c=LEFT_BUTTON_IN;  /* default for LEFT BUTTON or BOTH BUTTONS */
    mstruc m;

    clear_input_devices();
    for(;;)
    {
	if(kbhit())
	{
	    c=getch();
	    break;
	}
	m=button_read();
	if(m.status)    /* any button press will force a return */
	{
	    if(m.status==RIGHT_BUTTON)
		c=CARRIAGE_RETURN;
	    break;
	}
    }

    return c;

}








/* m_install().status verifies that mouse and driver installed, and resets */
/* mouse.  If m_install().status==0, not installed: if -1, installed. */

mstruc m_install()
{

    mstruc m;
    m.opcode=MOUSE_CHECK;

    cmousel(&m.opcode, &m.status, &m.dx, &m.dy);
    if(!m.opcode)
	printf("no mouse found\n");
    return m;

}





/* mdpos_read().dx and .dy are the change in x and y mouse position */
/* (unitless, -32K to +32K) since the last mdpos_read() function call */

mstruc mdpos_read()
{

    mstruc m;

    m.opcode=POS_CHANGE;

    cmousel(&m.opcode, &m.status, &m.dx, &m.dy);
    return m;

}



/* mouse_clear() clears the dx, dy registers of mdpos_read() and clears */
/* the button_clicks() LEFT and RIGHT value.  If either mouse button is being */
/* held down during mouse_clear() call, routine pauses for up to 1.0 sec and */
/* then beeps to let you know that the buttons must be released. */

void mouse_clear()
{

    int i=0;

    mdpos_read();
    do
    {
	pause(0.1);
	i++;
    }while(button_read().status && i<10);

    if(i>=10)
	while(button_read().status)
	    sound(1200, 0.1);

    button_clicks(LEFT_BUTTON);
    button_clicks(RIGHT_BUTTON);


}





/* mouse_gets() is a substitute for gets(), in which RIGHT_BUTTON is able */
/* to be detected as a carriage return.  Like gets(), the string that */
/* is returned is null-terminated. */

char * mouse_gets(sptr)
char *sptr;
{
    int i=0;
    unsigned char c='\0';

    mouse_clear();
    while(c != CARRIAGE_RETURN)
    {
	if(button_read().status==RIGHT_BUTTON)
	    c = CARRIAGE_RETURN;
	if(kbhit())
	{
	    c=getche();
	    if(c==BACKSPACE)
	    {
		putc(' ', stdout);
		putc(BACKSPACE,stdout);
		i--;
		if(i<0)
		    i=0;
		sptr[i]='\0';
	    }
	    else
	    {
		if(c != CARRIAGE_RETURN)
		{
		    sptr[i]=c;
		    i++;
		}
	    }
	}
    }
    sptr[i]='\0';
    printf("\n");

    return sptr;

}




/* mouse_response() checks for movement in the mouse or button-press activity */
/* It returns the MOTION_FACTOR-scaled dx & dy and either LEFT_BUTTON, */
/* RIGHT_BUTTON, BOTH_BUTTONS, or 0 for the .status struct element, depending */
/* on the button activity.  DOES NOT RETURN CLICK COUNT, only yes/no */

mstruc mouse_response()
{
    mstruc ret_val;

    ret_val=mdpos_read();
    ret_val.dx /=MOTION_FACTOR;
    ret_val.dy /=MOTION_FACTOR;

    ret_val.status=0;
    if(button_clicks(LEFT_BUTTON).status)
	ret_val.status = LEFT_BUTTON;
    if(button_clicks(RIGHT_BUTTON).status)
	ret_val.status +=RIGHT_BUTTON;

    return ret_val;

}

