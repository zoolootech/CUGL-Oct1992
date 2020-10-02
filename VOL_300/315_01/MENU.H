
/* print_headline() and menu_mouse_config() function declarations added */
/* 8/89 by T Clune */

/* mouse_flag_toggle() added 12/88 to support mouse-based menu selections */
/* Added by T Clune */

/* pathprint() added 11/88 by T. Clune */

/* fname_unused() moved to dos_func.h 10/88 by T. Clune */

/* menu.h declares the menu.c public functions */
/* revised 8/88 by T. Clune to support fname_unused(), which */
/* checks to see whether a filename already exists in the specified */
/* directory before writing the new file */

#ifndef MENU_H
#define MENU_H

 /* definitions added 12/88 by T Clune to support mouse-based selections */
#define MOUSE_PLUS_KEYBOARD 1
#define KEYBOARD_ONLY 0
#define MOUSE_ONLY -1

    /* factor to divide the mouse motion by to adjust "touchiness" */
    /* bigger number = less sensitivity */
#define MOUSE_SENSITIVITY 3
    /* the pause time in secs before and during mouse move */
#define OFF_TIME 0.15
#define DUTY_TIME 0.05

    /* cursor arrow flag definitions */
#define UP_MOVE 1
#define LEFT_MOVE 2
#define RIGHT_MOVE 4
#define DOWN_MOVE 5


    /* max. number of files supported by get_file() */
#define MAX_FILES 100

    /* the get_file() return structure */
typedef struct
{
    char string[80];
    int error_flag;
}string_struc;


string_struc get_file();
int menu(), get_mouse_flag(), mgetch();
char *mgets();
void reset_menu(), pathprint(), mouse_flag_toggle(), menu_mouse_config();
void print_headline(), clear_input_devices();

#endif
