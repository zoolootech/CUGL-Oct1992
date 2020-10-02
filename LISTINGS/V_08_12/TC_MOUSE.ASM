
		TITLE	MicroSoft Mouse	Library	Module
		PAGE	60,132


    COMMENT     !

	Assembler Source Code for MicroSoft Mouse Driver Interface

	Source is Compatible with MASM 5.0 and TASM 2.0

	$$$$   IMPORTANT NOTE:   $$$$

		I originally assembled each function as a separate
		module, and constructed a .LIB file, but since the
		code for the entire interface assembles to less than
		2Kbytes, the space saved is not worth the effort of
		seperate assembly.  Just link in tc_mouse.obj normally.

		The Turbo C V.1.5 and 2.0 compatible project file
		( testq.prj ) assumes a subdirectory named "EXE"
		where your ".OBJ" files are kept.  If you are using
		the project file in this way, change the last file
		on the line form tc_mouse.lib to exe\tc_mouse.obj

	Use the command:
				TASM /ml tc_mouse

		or

				MASM /ml tc_mouse;

	to assemble to object format

	then link with small model C program

    !

	.MODEL SMALL, C

	.DATA

	.STACK

	.CODE

	public _mouse_reset
	public _mouse_show
	public _mouse_hide
	public _mouse_get_pos
	public _mouse_set_pos
	public _mouse_button_press
	public _mouse_button_rel
	public _mouse_limit_x
	public _mouse_limit_y
	public _mouse_set_graphcursor
	public _mouse_set_textcursor
	public _mouse_get_movement
	public _mouse_set_eventhandler
	public _mouse_start_pen_emul
	public _mouse_stop_pen_emul
	public _mouse_set_movement_ratio
	public _mouse_conditional_off
	public _mouse_set_speed
	public _mouse_swap_vector
	public _mouse_get_bufsize
	public _mouse_save_state
	public _mouse_restore_state
	public _mouse_set_alt_handler
	public _mouse_get_alt_handler
	public _mouse_set_sens
	public _mouse_get_sens
	public _mouse_set_crt_page
	public _mouse_get_crt_page
	public _mouse_disable_drvr
	public _mouse_enable_drvr
	public _mouse_soft_reset


;****************************************
; _mouse_reset   PROC:
;
;	 Description:	initialize the mouse driver
;
; int  mouse_reset(int *num_buttons);
;
;****************************************

_mouse_reset    PROC
		xor	ax,ax
		int	33h
		ret
_mouse_reset    ENDP

;****************************************
; _mouse_show    PROC:
;
;	 Description:	display	the mouse cursor
;
; void _mouse_show(void);
;
;****************************************

_mouse_show     PROC
		mov	ax,1
		int	33h
		ret
_mouse_show     ENDP

;****************************************
; _mouse_hide    PROC:
;
;	 Description:	hide the mouse cursor
;
; void _mouse_hide(void);
;
;****************************************

_mouse_hide     PROC
		mov	ax,2
		int	33h
		ret
_mouse_hide     ENDP

;****************************************
; _mouse_get_pos PROC:
;
;	 Description:	get the	mouse cursor position
;
; int  mouse_get_pos(int *x, int *y);
;
;****************************************

_mouse_get_pos  PROC
		push    bp
		mov     bp,sp
		mov	ax,3
		int	33h
		mov	ax,bx
		mov     bx,[bp+4]
		mov	[bx],cx
		mov     bx,[bp+6]
		mov     [bx],dx
		pop     bp
		ret
_mouse_get_pos  ENDP

;****************************************
; _mouse_set_pos PROC:
;
;	 Description:	set the	mouse cursor position;
;
; void _mouse_set_pos(int x, int y);
;
;****************************************

_mouse_set_pos  PROC
		push    bp
		mov     bp,sp
		mov	ax,4
		mov     cx,[bp+4]
		mov     dx,[bp+6]
		int     33h
		pop     bp
		ret
_mouse_set_pos  ENDP

;****************************************
; _mouse_button_press    PROC:
;
;	 Description:	get mouse button press information
;
; int  mouse_button_press(int *x, int *y, int *count, int button);
;
;****************************************

_mouse_button_press     PROC
			push    bp
			mov     bp,sp
			mov	ax,5
			mov     bx,[bp+10]
			int     33h

			push	bx
			mov     bx,[bp+8]
			pop	[bx]
			mov     bx,[bp+6]
			mov	[bx],dx
			mov     bx,[bp+4]
			mov	[bx],cx

			pop     bp
			ret
_mouse_button_press     ENDP

;****************************************
; _mouse_button_rel      PROC:
;
;	 Description:	get mouse button release information
;
;
; int  mouse_button_rel(int *x, int *y, int *count, int button);
;
;****************************************

_mouse_button_rel       PROC
			push    bp
			mov     bp,sp
			mov	ax,6
			mov     bx,[bp+10]
			int	33h
			push	bx

			mov     bx,[bp+8]
			pop	[bx]
			mov     bx,[bp+6]
			mov	[bx],dx
			mov     bx,[bp+4]
			mov	[bx],cx

			pop     bp
			ret
_mouse_button_rel       ENDP

;****************************************
; _mouse_limit_x PROC:
;
;	 Description:	limit mouse cursor movement on the x axis (column)
;
; void _mouse_limit_x(int min_x, int max_x);
;
;****************************************

_mouse_limit_x  PROC
		push    bp
		mov     bp,sp
		mov	ax,7
		mov     cx,[bp+4]
		mov     dx,[bp+6]
		int     33h
		pop     bp
		ret
_mouse_limit_x  ENDP

;****************************************
; _mouse_limit_y PROC:
;
;	 Description:	limit mouse cursor movement on the y axis (row)
;
; void _mouse_limit_y(int min_y, int max_y);
;
;****************************************

_mouse_limit_y  PROC
		push    bp
		mov     bp,sp
		mov	ax,8
		mov     cx,[bp+4]
		mov     dx,[bp+6]
		int     33h
		pop     bp
		ret
_mouse_limit_y  ENDP


;****************************************
; _mouse_set_graphcursor   PROC
;
;	 Description:	set mouse graphics cursor shape
;
; void _mouse_set_graphcursor(int xspot,int yspot,void far *masks);
;
;****************************************

_mouse_set_graphcursor  PROC
			push    bp
			mov     bp,sp
			push    es
			mov	ax,9
			mov     bx,[bp+4]
			mov     cx,[bp+6]
			les     dx,[bp+8]
			int     33h
			pop     es
			pop     bp
			ret
_mouse_set_graphcursor  ENDP


;****************************************
; _mouse_set_textcursor  PROC:
;
;	 Description:	set mouse text cursor shape
;
; void _mouse_set_textcursor(int cursor_type,int scr_mask,int curs_mask);
;
;****************************************

_mouse_set_textcursor   PROC
			push    bp
			mov     bp,sp
			mov	ax,0Ah
			mov     bx,[bp+4]
			mov     cx,[bp+6]
			mov     dx,[bp+8]
			int     33h
			pop     bp
			ret
_mouse_set_textcursor   ENDP

;****************************************
; _mouse_get_movement    PROC:
;
;	 Description:	get mouse cursor movement information
;
; void _mouse_get_movement(int *x,int *y);
;
;****************************************

_mouse_get_movement     PROC
			push    bp
			mov     bp,sp
			push    es
			mov	ax,0Bh
			int	33h

			mov     bx,[bp+4]
			mov	[bx],cx
			mov     bx,[bp+6]
			mov     [bx],dx
			pop     es
			pop     bp
			ret
_mouse_get_movement     ENDP

;****************************************
; _mouse_set_eventhandler        PROC:
;
;	 Description:	set up a mouse event handler to	be called by
;			 by the	mouse driver.
;
; void _mouse_set_eventhandler(int event_mask, void far (*mhandler)());
;
;****************************************

_mouse_set_eventhandler PROC
			push    bp
			mov     bp,sp
			push    es
			mov	ax,0Ch
			mov     cx,[bp+4]
			les     dx,[bp+6]
			int     33h
			pop     es
			pop     bp
			ret
_mouse_set_eventhandler ENDP

;****************************************
; _mouse_start_pen_emul  PROC:
;
;	 Description:	start mouse light-pen emulation
;
; void _mouse_start_pen_emul(void);
;
;****************************************

_mouse_start_pen_emul   PROC
			mov	ax,0Dh
			int	33h
			ret
_mouse_start_pen_emul   ENDP

;****************************************
; _mouse_stop_pen_emul   PROC:
;
;	 Description:	stop mouse light-pen emulation
;
; void _mouse_stop_pen_emul(void);
;
;****************************************

_mouse_stop_pen_emul    PROC
			mov	ax,0Eh
			int	33h
			ret
_mouse_stop_pen_emul    ENDP

;****************************************
; _mouse_set_movement_ratio      PROC:
;
;	 Description:	set mouse cursor movement ratio
;
; void _mouse_set_movement_ratio(int x_ratio,int y_ratio);
;
;****************************************

_mouse_set_movement_ratio       PROC
				push    bp
				mov     bp,sp
				mov	ax,0Fh
				mov     cx,[bp+4]
				mov     dx,[bp+6]
				int     33h
				pop     bp
				ret
_mouse_set_movement_ratio       ENDP

;****************************************
; _mouse_conditional_off PROC:
;
;	 Description:	conditionally turn the mouse cursor  off
;
; void _mouse_conditional_off(int left,int top,int right,int bottom);
;
;****************************************

_mouse_conditional_off  PROC

			push    bp
			mov     bp,sp
			push    si
			push    di

			mov	ax,10h
			mov     cx,[bp+4]
			mov     dx,[bp+6]
			mov     si,[bp+8]
			mov     di,[bp+10]
			int     33h

			pop     di
			pop     si
			pop     bp
			ret
_mouse_conditional_off  ENDP

;****************************************
; _mouse_set_speed       PROC:
;
;	 Description:	set mouse cursor speed
;
; void _mouse_set_speed(int threshold);
;
;****************************************

_mouse_set_speed PROC
		push    bp
		mov     bp,sp
		mov	ax,13h
		mov     bx,[bp+4]
		int     33h
		pop     bp
		ret
_mouse_set_speed ENDP

;****************************************
; _mouse_swap_vector     PROC:
;
;	 Description:	swap mouse interrupt vectors
;
; void far *_mouse_swap_vector(int new_mask,int *old_mask,void far *new_vector);
;
;****************************************

_mouse_swap_vector      PROC
			push    bp
			mov     bp,sp
			push    es

			mov	ax,14h
			mov     cx,[bp+4]
			mov     bx,[bp+6]
			mov	[bx],cx
			les     dx,[bp+8]
			int	33h
			mov	ax,es
			xchg    ax,dx
			pop     es
			pop     bp
			ret
_mouse_swap_vector      ENDP

;****************************************
; _mouse_get_bufsize     PROC:
;
;	 Description:	get buffer size	needed to store	mouse "state" info
;
; int  mouse_get_bufsize(void);
;
;****************************************

_mouse_get_bufsize      PROC
			mov	ax,15h
			int	33h
			mov	ax,bx
			ret
_mouse_get_bufsize      ENDP

;****************************************
; _mouse_save_state      PROC:
;
;	 Description:	save the "state" of the	mouse driver
;
; void _mouse_save_state(void far *state_buf);
;
;****************************************

_mouse_save_state       PROC
			push    bp
			mov     bp,sp
			push    es

			mov	ax,16h
			les     dx,[bp+4]
			int     33h

			pop     es
			pop     bp
			ret
_mouse_save_state       ENDP

;****************************************
; _mouse_restore_state   PROC:
;
;	 Description:	restore	the mouse driver "state"
;
; void _mouse_restore_state(void far *state_buf);
;
;****************************************

_mouse_restore_state    PROC
			push    bp
			mov     bp,sp
			push    es

			mov	ax,17h
			les     dx,[bp+4]
			int     33h

			pop     es
			pop     bp
			ret
_mouse_restore_state    ENDP

;****************************************
; _mouse_set_alt_handler PROC:
;
;	 Description:	set an alternate mouse event handler
;
; void _mouse_set_alt_handler(int alt_mask,void far (*func)());
;
;****************************************

_mouse_set_alt_handler  PROC
			push    bp
			mov     bp,sp
			push    es

			mov	ax,18h
			mov     cx,[bp+4]
			les     dx,[bp+6]
			int     33h

			pop     es
			pop     bp
			ret
_mouse_set_alt_handler  ENDP

;****************************************
; _mouse_get_alt_handler PROC:
;
;	 Description:	get address of alternate mouse event handler
;
; int  mouse_get_alt_handler(int alt_mask,void far *handler);
;
;****************************************

_mouse_get_alt_handler  PROC
			push    bp
			mov     bp,sp
			push    es
			push    di

			mov	ax,19h
			mov     cx,[bp+4]
			int	33h
			les     di,[bp+6]
			mov	word ptr es:[di],dx
			mov	word ptr es:[di+2],bx
			mov     ax,cx

			pop     di
			pop     es
			pop     bp
			ret
_mouse_get_alt_handler  ENDP

;****************************************
; _mouse_set_sens        PROC:
;
;	 Description:	set mouse sensitivity
;
; void _mouse_set_sens(int x_ratio,int y_ratio,int threshold);
;
;****************************************

_mouse_set_sens PROC
		push    bp
		mov     bp,sp
		mov	ax,1Ah
		mov     bx,[bp+4]
		mov     cx,[bp+6]
		mov     dx,[bp+8]
		int     33h
		pop     bp
		ret
_mouse_set_sens ENDP

;****************************************
; _mouse_get_sens        PROC:
;
;	 Description:	get mouse sensitivity
;
; void _mouse_get_sens(int *x_ratio,int *y_ratio,int *threshold);
;
;****************************************

_mouse_get_sens  PROC
		push    bp
		mov     bp,sp
		mov	ax,1Bh
		int	33h
		push	bx
		mov     bx,[bp+4]
		pop	[bx]
		mov     bx,[bp+6]
		mov	[bx],cx
		mov     bx,[bp+8]
		mov     [bx],dx
		pop     bp
		ret
_mouse_get_sens ENDP

;****************************************
; _mouse_set_crt_page    PROC:
;
;	 Description:	set mouse crt page (video page)
;
; void _mouse_set_crt_page(int crt_page);
;
;****************************************

_mouse_set_crt_page     PROC
			push    bp
			mov     bp,sp
			mov	ax,1Dh
			mov     bx,[bp+4]
			int     33h
			pop     bp
			ret
_mouse_set_crt_page     ENDP

;****************************************
; _mouse_get_crt_page    PROC:
;
;	 Description:	get mouse crt page (video page)
;
; int  mouse_get_crt_page(void);
;
;****************************************

_mouse_get_crt_page     PROC
			mov	ax,1Eh
			int	33h
			mov	ax,bx
			ret
_mouse_get_crt_page     ENDP

;****************************************
; _mouse_disable_drvr    PROC:
;
;	 Description:	disable	mouse driver
;
; int  mouse_disable_drvr(void far *vector_33h);
;
;****************************************

_mouse_disable_drvr     PROC
			push    bp
			mov     bp,sp
			push    es

			mov	ax,1Fh
			int	33h
			cmp	ax,-1
			je      disable_exit
			mov	dx,es
			mov	cx,bx
			les     bx,[bp+4]
			mov	word ptr es:[bx],cx
			mov	word ptr es:[bx+2],dx
	disable_exit:
			pop     es
			pop     bp
			ret
_mouse_disable_drvr     ENDP

;****************************************
; _mouse_enable_drvr     PROC:
;
;	 Description:	enable mouse driver
;
; void _mouse_enable_drvr(void);
;
;****************************************

_mouse_enable_drvr      PROC
			mov	ax,20h
			int	33h
			ret
_mouse_enable_drvr      ENDP

;****************************************
; _mouse_soft_reset      PROC:
;
;	 Description:	mouse software reset
;
; int  mouse_soft_reset(int *nbuttons);
;
;****************************************

_mouse_soft_reset       PROC
			push    bp
			mov     bp,sp
			mov	ax,21h
			int	33h
			cmp	ax,-1
			jne     soft_exit
			push	bx
			mov     bx,[bp+4]
			pop	[bx]

	soft_exit:
			pop     bp
			ret
_mouse_soft_reset       ENDP

			END
