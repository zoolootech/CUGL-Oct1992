
/* declarations to support mouse_clear() and mouse_response() added */
/* 3/89 by T. Clune. */

/* declarations for button_clicks() added 11/88 by T Clune */

/* declaration of microsoft mouses() function and the three mouse.lib */
/* function front ends in mouselib.c.  The struct variables are equivalent */
/* to the Microsoft mouse users' guide variables, m1, m2, m3 and m4. */
/* By T. Clune, 12/87 */

#ifndef MOUSELIB_H
#define MOUSELIB_H


#define LEFT_BUTTON 1  /* left button press status value from func 3 */
#define RIGHT_BUTTON 2     /* right button press */
#define BOTH_BUTTONS 3      /* both buttons press */

#define LEFT_BUTTON_IN 127  /* value returned by inpause() left button */
			    /* or both buttons press */

#define MOUSE_CHECK 0       /* function 0 of Microsoft mouse.lib */
#define BUTTON_STATUS 3     /* function 3 = Button check */
#define BUTTON_CLICKS 5     /* function 5= number of times button pressed */
			    /* since last call to function 5 */
#define POS_CHANGE  11      /* function 11 = mouse motion counter values */

#define MOTION_FACTOR 3 /* mouse must move 3 in x or y dir to count as moving */

typedef struct
{
    int opcode;  /* the function number variable */
    int status;   /* the button-press status variable for function 3 */
    int dx;     /* with call 11 (mpos_read()), dx & dy return change in x */
    int dy;     /* or y position since last call. */
} mstruc;


void mouse_clear();
int inpause();
char *mouse_gets();
mstruc m_install(), button_read(), mdpos_read(), button_clicks(), mouse_response();

#endif
