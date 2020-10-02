
/*      HEADER:     CUG0000;
	TITLE:      Header file for small model mouse library;
	FILENAME:   TCMOUSE.H;
	VERSION:    1.0;
	DATE:       12/31/89;
	AUTHOR:     Michael Kelly;

	COMPILER:   Turbo C V. 1.5+;
	NOTES:      Collection of calls to Microsoft Mouse Driver Functions
		    assembled seperately and built with Tlib;
*/

			   /*  Int 33h  Functions */

#if !defined(TC_MOUSE_H)
#define TC_MOUSE_H

#define LEFT_BUTTON 1
#define RIGHT_BUTTON 2
#define BOTH_BUTTONS 3

int mouse_reset(int *num_buttons);      /*  Func 00h    */
void mouse_show(void);                  /*  Func 01h    */
void mouse_hide(void);                  /*  Func 02h    */
int mouse_get_pos(int *x, int *y);      /*  Func 03h    */
void mouse_set_pos(int x, int y);       /*  Func 04h    */

    /* Func 05h */
int mouse_button_press(int *x, int *y, int *count, int button);

    /* Func 06h */
int mouse_button_rel(int *x, int *y, int *count, int button);

void mouse_limit_x(int min_x, int max_x);   /* Func 07h */
void mouse_limit_y(int min_y, int max_y);   /* Func 08h */

    /* Func 09h */
void mouse_set_graphcursor(int xspot,int yspot,void far *masks);

    /* Func 0Ah */
void mouse_set_textcursor(int cursor_type,int scr_mask,int curs_mask);

void mouse_get_movement(int *x,int *y);     /* 0Bh */

    /* Func 0Ch */
void mouse_set_eventhandler(int event_mask, void far (*mhandler)());

void mouse_start_pen_emul(void);    /* Func 0Dh */
void mouse_stop_pen_emul(void);     /* Func 0Eh */

    /* Func 0Fh */
void mouse_set_movement_ratio(int x_ratio,int y_ratio);

    /* Func 10h */
void mouse_conditional_off(int left,int top,int right,int bottom);

void mouse_set_speed(int threshold);    /* Func 13h */

    /* Func 14h */
void far *mouse_swap_vector(int new_mask,int *old_mask,void far *new_vector);

int mouse_get_bufsize(void);                    /* Func 15h */
void mouse_save_state(void far *state_buf);     /* Func 16h */
void mouse_restore_state(void far *state_buf);  /* Func 17h */

    /* Func 18h */
void mouse_set_alt_handler(int alt_mask,void (*func)());

    /* Func 19h */
int mouse_get_alt_handler(int alt_mask,void far *handler);

    /* Func 1Ah */
void mouse_set_sens(int x_ratio,int y_ratio,int threshold);

    /* Func 1Bh */
void mouse_get_sens(int *x_ratio,int *y_ratio,int *threshold);

void mouse_set_crt_page(int page);      /* Func 1Dh */
int mouse_get_crt_page(void);           /* Func 1Eh */

    /* Func 1Fh */
int mouse_disable_drvr(void far *vector_33h);

void mouse_enable_drvr(void);           /* Func 20h */
int mouse_soft_reset(int *nbuttons);    /* Func 21h */


#endif
