/****************************************************************************
 *                                                                          *
 *                               jjbkbd.h                                   *
 *                                                                          *
 *   Copyright (c) 1988, 1989, JJB. All rights reserved.                    *
 *                                                                          *
 *   Purpose:                                                               *
 *                                                                          *
 *      The purpose of this file is to make definitions for the keyboard    *
 *      and to explain to you how important the get_ch() function is        *
 *      and why it is considered to be at the heart of the entire JJB       *
 *      programming environment.                                            *
 *                                                                          *
 *                                                                          *
 *     JJB, 9236 Church Rd suite 1082, Dallas, Tx 75231 (214) 341-1635      *
 ****************************************************************************/

#ifndef BL
#define BL   32
#define RET  '\015'
#define AST  '\052'
#define ESC  '\033'
#define BSPC '\010'

/****************************************************************************
 *                                                                          *
 *           get_ch()      get_num()     get_any();                         *
 *                                                                          *
 *  If you have the printed documentation of JJB, you can see how there are *
 *  There are 5 low level functions you can use below the function get_ch() *
 *  and 3 higher level functions above it which give you absolute control   *
 *  over the keyboard.                                                      *
 *                                                                          *
 *      char get_ch();   the equivalent of 'getch()'                        *
 *      char get_num();  read numeric                                       *
 *      char get_any();  wait for any key to be pressed                     *
 *                                                                          *
 ****************************************************************************/

#define SPACEBAR_PRESSED    (get(IKEY) == 32)
#define ESC_PRESSED         (getc(KEY) == ESC)
#define F1_PRESSED          (!get(IKEY) && get(IK_EXT) == 59)
#define ALT_PRESSED         (get(IK_CODE) & 8)
#define RETURN_PRESSED      (getc(KEY) == RET)
#define REMOVE_PRESSED      (getc(KEY) == getc(REMOVE) )
#define BSPC_PRESSED        (getc(KEY) == BSPC)
#define LEFTSHIFT_PRESSED   (get(IK_CODE) & 1)
#define RIGHTSHIFT_PRESSED  (get(IK_CODE) & 2)
#define SHIFT_PRESSED       (get(IK_CODE) & 3)
#define CTRL_PRESSED        (get(IK_CODE) & 4)
#define SCROLLLOCK_ACTIVE   (get(IK_CODE) & 16)
#define NUMLOCK_ACTIVE      (get(IK_CODE) & 32)
#define CAPS_ACTIVE         (get(IK_CODE) & 64)
#define INSERT_ACTIVE       (get(IK_CODE) & 128)

#define LEFTARROW_PRESSED   (!get(IKEY) &&  get(IK_EXT) == 75)
#define RIGHTARROW_PRESSED  (!get(IKEY) &&  get(IK_EXT) == 77)
#define UPARROW_PRESSED     (!get(IKEY) &&  get(IK_EXT) == 72)
#define DOWNARROW_PRESSED   (!get(IKEY) &&  get(IK_EXT) == 80)

#define PGUP_PRESSED        (!get(IKEY) &&  get(IK_EXT) == 73)
#define PGDN_PRESSED        (!get(IKEY) &&  get(IK_EXT) == 81)
#define HOME_PRESSED        (!get(IKEY) &&  get(IK_EXT) == 71)
#define END_PRESSED         (!get(IKEY) &&  get(IK_EXT) == 79)
#define CTRLG_PRESSED       (get(IKEY) == 7)
#define FUNCT_PRESSED       (!get(IKEY)  && between(get(IK_EXT),59,72))


/****************************************************************************
 *                                                                          *
 *                            keyboard                                      *
 *                                                                          *
 *   Most C programmers are used to 'ch = getch();'                         *
 *                                                                          *
 *   In JJB, just use:    'get_ch();'                                       *
 *                        'get_num();'                                      *
 *                        'get_any();'                                      *
 *                                                                          *
 *   You can still 'ch = get_ch()', however JJB operates at such a high-    *
 *   level of programming that it is not necessary.                         *
 *                                                                          *
 ****************************************************************************/



/****************************************************************************
 *                                                                          *
 *                              get_ch()                                    *
 *                                                                          *
 *   Use get_ch() instead of getch().                                       *
 *   The reason you must use get_ch() is because JJB needs to look at every *
 *   keypress to see if ALT has been pressed for changing options.          *
 *                                                                          *
 *   also, JJB is designed to execute get_ch() without expecting            *
 *   a return value. Everytime you use get_ch(), JJB does a BIOS            *
 *   read, updating four characters: KEY, KEYU, K_EXT & K_CODE and four     *
 *   integers: IKEY, IKEYU, IK_EXT, and IK_CODE.  These values are          *
 *   stored in arrays and are accessible from any object module             *
 *   with the 'get(' and 'set(' functions described in  'JJBSET.H'.         *
 *                                                                          *
 *   Here is an example:                                                    *
 *                                                                          *
 *            get_ch();                                                     *
 *                                                                          *
 *            if (ESC_PRESSED) do_something();                              *
 *                                                                          *
 *                                                                          *
 *   Everytime you use get_ch(), JJB sets in arrays:                        *
 *                                                                          *
 *     K_AL_REG  to low byte  reg.h.al in keyboard interrupt kbint()        *
 *     K_AH_REG  to high byte reg.h.ah in keyboard interrupt kbint()        *
 *     K_IAL_REG to integer of K_AL_REG                                     *
 *     K_IAH_REG to integer of K_AH_REG                                     *
 *                                                                          *
 *     KEY       to the character of the key you pressed.                   *
 *     KEYU      to KEY taken to upper case.                                *
 *     IKEY      to integer of KEY. If 'a' pressed, IKEY would be 97.       *
 *     IKEYU     to integer of KEYU. If 'a' pressed, IKEYU would be 65      *
 *     K_EXT     if KEY = '\0', K_EXT holds the value of the special key.   *
 *     IK_EXT    to the integer value of K_EXT.                             *
 *     K_CODE    to the keyboard scan code.                                 *
 *     IK_CODE   to the keyboard scan code taken to an integer value.       *
 *                                                                          *
 *   You may still use 'ch = get_ch()', but it is no longer necessary       *
 *   because JJB handles everything for you. The JJB input and enter        *
 *   functions are designed to be high level functions not requiring        *
 *   low level testing of keypresses. JJB does that for you.                *
 *                                                                          *
 *    'get_num()' is the same as get_ch()' except that JJB will only allow  *
 *    you to press a number or a special key.                               *
 *                                                                          *
 *    'get_any() will wait for any key to be pressed.                       *
 *                                                                          *
 ****************************************************************************/


/* miscellaneous definitions */

#define T 1                /* T for true                              */
#define beep printf("\a")
#define kk getch()         /* JJB uses this only for debugging only   */
#define BEGIN while(1) {   /* BEGIN....AGAIN is an easy way to see    */
#define AGAIN }            /*    continuous loops                     */
#define DRAWLINE 99        /* use in jjb_setup() to separate options  */

#endif




