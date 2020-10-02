; pcio

; Most of these routines should not be called directly by the application.
; Routines are being added in term.c which relay the call to here.  In that
; way terminal specific functions will be consolidated in term.c.  By
; changing just term.c the program can be adapted to a different host,
; resulting in better portability.  Many of the routines contained here
; are unused, but may be useful for other applications.  Many of these
; functions could also be implemented mostly in C.  Some assembly language
; is essential for speed on slow systems, but it becomes less important
; on fast processors, and may eventually have to go away altogether for
; better portability.  The standard BIOS video calls are incredibly slow,
; and are mostly unused here.  On an 80386 system the screen refresh time
; is less than the keyboard autorepeat time with the interface in this
; file.
;
;                                 May 87 G. Osborn



; Converted from DeSmet to the Microsoft version  3.0 macro
; assembler format.  Microsoft does not follow data labels
; by a colon.  Code segment labels are followed by a colon, though.
; Changed "byte" to "byte ptr" throughout.

; Changed calling sequence to that of Microsoft C compiler version 3.0.
; Same as DeSmet except SI and DI must be saved in case register
; variables are used, so the new sequence is compatible with both compilers.

; publics are defined two ways for compiler compatibility.  Each is
; defined as _LABEL (Microsoft) and LABEL_ (DeSmet).

; Effort required to reconvert to DeSmet is minor

;                                            2-18-87 G. Osborn



_TEXT SEGMENT  BYTE PUBLIC 'CODE'
; instructions
_TEXT ENDS

CONST SEGMENT  WORD PUBLIC 'CONST'
; read only constants.  not constant strings.
CONST ENDS

_BSS SEGMENT  WORD PUBLIC 'BSS'
; uninitialized static data
_BSS ENDS

_DATA SEGMENT  WORD PUBLIC 'DATA'
; initialized global and static data
_DATA ENDS

DGROUP GROUP CONST, _BSS, _DATA
   ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP



;------------------------------------------------------------------
; pcio.a  --  Screen and keyboard interface routines for the PC
;             Specific to DeSmet C
;
; last modified - 08/06/86
;
;
;                        !!! PLEASE NOTE !!!
;     This is not quite the same as pcio.a from DeSmet -
;     I've made some changes here and there for use with ged.
;     Am slowly changing all Desmet functions that use x,y co-ords
;     to work as col,row - but have not finished
;                                                    Mel Tearle
;------------------------------------------------------------------

;------------------------------------------------------------------
;                         data starts here
;------------------------------------------------------------------

_DATA SEGMENT

; In this implementation, all special and function keys are translated
; to the following values.

; control key translations.  would be better done in C, because the
; convesion time is not critical.

up_char         equ   5    ;equ 196   mapped into cursor-pad
down_char       equ  24    ;equ 197
left_char       equ  19    ;equ 198
right_char      equ   4    ;equ 199
bol_char        equ  15    ;equ 200
eol_char        equ  16    ;equ 201
pageup_char     equ  18    ;equ 202
pagedown_char   equ   3    ;equ 203
Del_char        equ   7    ;equ 207
Ins_char        equ  16h

;  bof_char     equ 204      not used
;  eof_char     equ 205      not used
;  NextWord_char   equ 208   not used
;  PrevWord_char   equ 209   not used

M1      equ 210
M2      equ 211
M3      equ 212
M4      equ 213
M5      equ 214
M6      equ 215
M7      equ 216
M8      equ 217
M9      equ 218
M10     equ 219
M11     equ 220
M12     equ 221

; the table that is used to make the translation
; note - not all used

convert db  72, up_char
        db  80, down_char
        db  75, left_char
        db  77, right_char
        db  71, 220      ; <home> becomes beginning of file
        db  79, 221      ; <end> becomes end of file
        db  73, pageup_char
        db  81, pagedown_char
        db  113, 222    ; ^<up arrow> .. cursor to top of window
        db  118, 223    ; cursor to bottom of window
        db  82, Ins_char ; <ins> becomes ^V
        db  83, Del_char
        db  115, 15h      ; ^<left arrow> becomes ^U = cursor full left
        db  116, 1Dh      ; ^<right arrow> becomes ^] = cursor full right
        db  59, M1
        db  60, M2
        db  61, M3
        db  62, M4
        db  63, M5
        db  64, M6
        db  65, M7
        db  66, M8
        db  67, M9
        db  68, M10
        db  69, M11
        db  70, M12
        db   0, 255 ; illegal character


; equates for bios interface.
; the interrupt and codes for the screen interface interrupt.

video       equ 10h     ;interrupt for dealing with screen

mode        equ 0       ;code for setting new screen mode
curtype     equ 1       ;code for setting new cursor type
setcur      equ 2       ;code for addressing cursor
readcur     equ 3       ;code for reading cursor location
readlp      equ 4       ;code for reading light pen position
setpage     equ 5       ;code to select active page
scrollup    equ 6       ;code to scroll screen up
scrolldn    equ 7       ;code to scroll screen nown
readch      equ 8       ;code to read a character from screen
writeach    equ 9       ;code to write char and attributes
writech     equ 10      ;code to write character only
setpal      equ 11      ;code to set new setpal or border
wdot        equ 12      ;code to write a dot
rdot        equ 13      ;code to read a dot
wtty        equ 14      ;code to write as if teletype
state       equ 15      ;code to find current screen status

; int 10 (video) changes all registers except cs,ss,ds,es,bx,cx,dx

; used in set_video

mono_adr     equ  0b000h   ;mono adapter
graph_adr    equ  0b800h   ;graphics adapter*

vadrs        dw  0


; the interrupt and codes for the keyboard interface.

keyboard     equ 16h     ;interrupt 16 to deal with keyboard

cicode       equ 0       ;code for reading a character
cstscode     equ 1       ;code for keyboard status


; caution: must change column number if 40 column mode

crt_cols    equ 80

; variables available to a C88 program

        public  scr_cols_, scr_rows_, scr_scrollup_, scr_scrolldown_
        public  scr_mode_, scr_page_, scr_attr_, scr_window_top_

        public  _scr_cols, _scr_rows, _scr_scrollup, _scr_scrolldown
        public  _scr_mode, _scr_page, _scr_attr, _scr_window_top


_scr_cols  label byte
scr_cols_ db  crt_cols    ;current number of columns


; note- make 25 for ms-dos and 24 for cp/m as cp/m steals the bottom
; line.

scr_rows    equ  25
_scr_rows   label byte
scr_rows_   db  scr_rows     ;current number of rows, for ged, was 25

_scr_mode   label byte
scr_mode_   db  0       ;current screen mode

_scr_page   label byte
scr_page_   db  0       ;current page

_scr_attr   label byte
scr_attr_   db  7       ;current attributes for screen
                        ;7 is white letters on black

_scr_window_top label byte
scr_window_top_ db  1   ;first line to scroll, was 0

; variables needed by SEE. Not used here.

_scr_scrollup   label byte
scr_scrollup_   db  0       ;zero if scrollup leaves top line alone

_scr_scrolldown label byte
scr_scrolldown_ db  0       ;zero if scroll down supported

_DATA      ENDS

;------------------------------------------------------------------
;                          code starts here
;------------------------------------------------------------------

_TEXT       SEGMENT
            PUBLIC    _scr_setup

;-------------------------------------------------------------------------
; SCR_SETUP_  scr_setup must be called before any use of any
;             other routine unless the starting mode is 80X25
;             character mode (3,4 or 7). Must be called for monochrome
;             (mode 7) for scr_curson to set a proper cursor.
;
; Usage:      mode = scr_setup();
;-------------------------------------------------------------------------
scr_setup_:
_scr_setup:
scrsetup:
        push  bp
        mov   bp,sp
        push  di
        push  si
        mov   ah,state        ;get current state
        int   video
        mov   scr_mode_,al    ;current mode
        mov   cl,ah           ;make cols a word
        mov   ch,0
        mov   scr_cols_,cl    ;40 or 80 columns
        mov   scr_page_,bh
        mov   scr_attr_,7     ;set to white chars on black
        cmp   al,4            ;see if a character mode
        jc    got_attr
        cmp   al,7            ;7 is for graphics mode
        jz    got_attr
        mov   scr_attr_,0     ;attribute is zero in graphics
got_attr:
        mov   ah,0            ;return int containing mode
        push  ax              ;slight detour
        call  set_video_      ;get display address

        pop   ax
        pop   si
        pop   di
        pop   bp
        ret



;-------------------------------------------------------
; SCR_TERM_     do any required termination.
;
; Usage:        scr_term();
;-------------------------------------------------------

public  scr_term_
public  _scr_term
_scr_term:
scr_term_:
       ret



;-------------------------------------------------------
; SCR_SETMODE_    set a new screen mode
;
; Usage:          scr_setmode(new mode);
;-------------------------------------------------------

public  scr_setmode_
public  _scr_setmode
_scr_setmode:
scr_setmode_:
        push  bp
        mov   bp,sp
        push  di
        push  si
        mov   al,[bp+4]    ;new mode value
        mov   ah,mode
        int   video        ;set new mode
        call  scrsetup     ;remember new values
        pop   si
        pop   di
        pop   bp
        ret



;-------------------------------------------------------
; SCR_ROWCOL_    sets cursor at any location.
;
; Usage:         scr_rowcol(new row, new column);
; Note!:         wil be changed to col,row
;-------------------------------------------------------

public  scr_rowcol_
public  _scr_rowcol
_scr_rowcol:
scr_rowcol_:
        push  bp           ;save from bios
        mov   bp,sp
        push  di
        push  si
        mov   dx,[bp+6]    ;column
        mov   ax,[bp+4]    ;row
        mov   dh,al
        mov   bh,scr_page_ ;force page zero
        mov   ah,setcur    ;set cursor location
        int   video        ;call bios
        pop   si
        pop   di
        pop   bp
        ret



;-------------------------------------------------------
; SCR_CLR_      clear entire screen
;
; Usage:       scr_clr();
;-------------------------------------------------------

public  scr_clr_
public  _scr_clr
_scr_clr:
scr_clr_:
        push  bp                 ;save from video call
        mov   bp,sp
        push  di
        push  si
        mov   al,0               ;ask for a clear window
        xor   cx,cx              ;start at 0,0
        mov   dl,scr_cols_       ;clear entire width
        dec   dl                 ;last column is width-1
        mov   dh,scr_rows_       ;last line
        dec   dh
        mov   bh,scr_attr_       ;attributes for new blanks
        mov   ah,scrollup        ;ask for a scrollup to clear
        int   video              ;do the clear
        pop   si
        pop   di
        pop   bp
        ret



;-------------------------------------------------------
; SCR_CLRL_     clear rest of line.
;
; Usage:        scr_clrl();
;-------------------------------------------------------

public  scr_clrl_
public  _scr_clrl
_scr_clrl:
scr_clrl_:
        push  bp
        mov   bp,sp
        push  di
        push  si
        mov   bh,scr_page_
        mov   ah,readcur          ;see where we are
        int   video

        mov   cl,scr_cols_   ;calc how many chars left in line
        sub   cl,dl               ;number left
        mov   ch,0                ;number of blanks needed
        mov   al,' '              ;write blanks
        mov   bl,scr_attr_        ;normal attributes
        mov   bh,scr_page_        ;page number
        mov   ah,writeach         ;write the blanks
        int   video
        pop   si
        pop   di
        pop   bp
        ret



;-------------------------------------------------------
; SCR_CLS_     clear rest of screen.
;
; Usage:       scr_cls();
;-------------------------------------------------------

public  scr_cls_
public  _scr_cls
_scr_cls:
scr_cls_:
        push  bp
        mov   bp,sp
        push  di
        push  si
        call  scr_clrl_          ;clear rest of line
        mov   ah,readcur         ;see where we are
        mov   bh,scr_page_
        int   video

        mov   al,0               ;ask for a clear window
        mov   ch,dh              ;current row
        inc   ch                 ;+1
        cmp   ch,scr_rows_  ;see if in last line
        jz    cleared            ;all done
        mov   cl,0               ;first column
        mov   dh,scr_rows_  ;24 is the last line
        dec   dh
        mov   dl,scr_cols_  ;clear entire width
        dec   dl                 ;last column is width-1
        mov   bh,scr_attr_       ;attributes for new blanks
        mov   ah,scrollup        ;ask for a scrollup to clear
        int   video              ;do the clear
cleared:
        pop   si
        pop   di
        pop   bp
        ret



;--------------------------------------------------------
; SCR_SCUP_     scroll text up leaving top lines alone.
;
; Usage:        scr_scup();
;--------------------------------------------------------

public  scr_scup_
public  _scr_scup
_scr_scup:
scr_scup_:
                              ;scroll last line, screen from line
                              ;scr_windor_top to end
        push  bp
        mov   bp,sp
        push  di
        push  si

        mov   al,scr_cols_    ;need last column of screen
        dec   al
        xor   ah,ah
        push  ax
        mov   al,scr_rows_    ;scroll through last line
        dec   al
        xor   ah,ah
        push  ax
        xor   ax,ax           ;from column 0
        push  ax
        mov   al,scr_window_top_  ;leave top line alone
        push  ax
        mov   al,1
        push  ax              ;scroll by 1
        call  scr_scrup_      ;do the scroll
        add   sp,10           ;clear arge
        pop   si
        pop   di
        pop   bp
        ret



;---------------------------------------------------------------
; SCR_SCRUP_     Scroll the screen up. The window is scrolled
;                up nline lines. A zero nline will clear the
;                window. Top left of the screen in 0,0.
;
; Usage:         scr_scrup(nline,fromrow,fromcol,torow,tocol);
;---------------------------------------------------------------

public  scr_scrup_
public  _scr_scrup
_scr_scrup:
scr_scrup_:
        push  bp
        mov   bp,sp
        push  di
        push  si

        mov   al,[bp+4]   ;number of lines
        mov   ch,[bp+6]   ;starting row
        mov   cl,[bp+8]   ;starting column
        mov   dh,[bp+10]  ;ending row
        mov   dl,[bp+12]  ;ending column
        mov   bh,scr_attr_   ;current attribute
        mov   ah,scrollup
        int   video          ;do the scroll
        pop   si
        pop   di
        pop   bp
        ret



;-------------------------------------------------------------
; SCR_SCDN_     scroll all but the top lines down one.
;
; Usage:        scr_scdn();
;-------------------------------------------------------------

public  scr_scdn_
public  _scr_scdn
_scr_scdn:
scr_scdn_:
        push  bp
        mov   bp,sp
        push  di
        push  si

        mov   al,scr_cols_  ;need last column of screen
        dec   al
        xor   ah,ah
        push  ax
        mov   al,scr_rows_  ;scroll through last line
        dec   al
        xor   ah,ah
        push  ax
        xor   ax,ax         ;from column 0
        push  ax
        mov   al,scr_window_top_  ;leave top lines alone
        push  ax
        mov   al,1
        push  ax            ;scroll by 1
        call  scr_scrdn_    ;do the scroll
        add   sp,10         ;clear arge
        pop   si
        pop   di
        pop   bp
        ret



;---------------------------------------------------------------
; SCR_SCRDN_    scroll the screen down. the window is scrolled
;               down nline lines. A zero nline will clear the
;               window. Top left of the screen in 0,0.
;
; Usage:        scr_scrdn(nline,fromrow,fromcol,torow,tocol);
;---------------------------------------------------------------

public  scr_scrdn_
public  _scr_scrdn
_scr_scrdn:
scr_scrdn_:
        push  bp
        mov   bp,sp
        push  di
        push  si

        mov   al,[bp+4]   ;number of lines
        mov   ch,[bp+6]   ;starting row
        mov   cl,[bp+8]   ;starting column
        mov   dh,[bp+10]  ;ending row
        mov   dl,[bp+12]  ;ending column
        mov   bh,scr_attr_    ;current attribute
        mov   ah,scrolldn
        int   video       ;do the scroll
        pop   si
        pop   di
        pop   bp
        ret



;---------------------------------------------------------------
; SCR_CO_     write a character to the screen. this
;             routine increments the cursor position
;             after writing. normal C88 puts and printf
;             statements can also be used to write to the
;             screen.
;
; Usage:      scr_co(character);
;---------------------------------------------------------------

public  scr_co_
public  _scr_co
_scr_co:
scr_co_:
        push  bp
        mov   bp,sp
        push  di
        push  si

        mov   al,[bp+4]    ;character to write
        mov   bh,scr_page_
        mov   ah,wtty      ;use tty write routine
        int   video

        pop   si
        pop   di
        pop   bp
        ret



;---------------------------------------------------------------
; SCR_CI_     keyboard input. function and soft keys are
;             translated. see equates for values.
;
; Usage:      character = scr_ci();
;---------------------------------------------------------------

public  scr_ci_
public  _scr_ci
_scr_ci:
scr_ci_:
        push  bp
        mov   bp,sp
        push  di
        push  si

        mov   ah,cicode           ;ask for a keyboard character
        int   keyboard
        cmp   al,0
        jne   not_special
        mov   bx, offset dgroup:convert  ;convert special key
ci_loop:
        cmp   byte ptr[bx],0
        jne    ci1   ; not end of list

        and   ah,ah
        js    ci2    ; pass codes 80h - ffh through (eg. alt-160)
        xor   ah,ah
ci2:
        mov al,ah
        jmp   not_special
ci1:
        cmp   ah,[bx]
        je    got_it    ; char matches table
        add   bx,2
        jmp   ci_loop
got_it:
        inc   bx
        mov   al,[bx]
not_special:
        mov   ah,0
cix:    pop   si
        pop   di
        pop   bp
        ret



;---------------------------------------------------------------
; SCR_CSTS_   return character if any available. otherwise
;             return zero.
;
; Usage:      character = scr_csts();
;---------------------------------------------------------------

public  scr_csts_
public  _scr_csts
_scr_csts:
scr_csts_:
        push  bp
        mov   bp,sp
        push  di
        push  si

        mov   ah,cstscode
        int   keyboard
        mov   ax,0
        jz    csts_over
        call  scr_ci_     ;get the coded character
csts_over:
        pop   si
        pop   di
        pop   bp
        ret



;---------------------------------------------------------------
; SCR_SET_CURSOR_     does nothing. needed by SEE.
;
;---------------------------------------------------------------

public  scr_set_cursor_
public  _scr_set_cursor
_scr_set_cursor:
scr_set_cursor_:        ;set the visible cursor to the
                        ;current position
        ret



;---------------------------------------------------------------
; SCR_SINP_   screen input (read character from the screen).
;
; Usage:      character = scr_sinp();
;---------------------------------------------------------------

public  scr_sinp_
public  _scr_sinp
_scr_sinp:
scr_sinp_:
        push  bp          ;save the registers
        mov   bp,sp
        push  di
        push  si

        mov   bh,scr_page_
        mov   ah,readch   ;code to read a character
        int   video       ;al is letter, ah=attributes
        or    al,al       ;zero returned instead of blank in
                          ;graphics mode
        jnz   ret_ch
        mov   al,' '
ret_ch:
        mov ah,0          ;kill the attributes
        pop  si
        pop  di
        pop  bp
        ret



;---------------------------------------------------------------
; SCR_CURSOFF_    turn cursor off.
;
; Usage:          scr_cursoff();
;
; This routing must not be called directly.  See curson() in term.c.
;---------------------------------------------------------------

public  scr_cursoff_
public  _scr_cursoff
_scr_cursoff:
scr_cursoff_:
        push  bp            ;save registers
        mov   bp,sp
        push  di
        push  si

        cmp   scr_mode_,4   ;see if graphics
        jc    text_coff
        cmp   scr_mode_,7
        jnz   no_cur
text_coff:
        mov   cx,0f00h      ;should turn cursor off
new_cur:
        mov   ah,curtype    ;set a new cursor type
        int   video
no_cur:
        pop  si
        pop  di
        pop  bp
        ret



;---------------------------------------------------------------
; SCR_CURSON_     turn cursor back on.
;
; Usage:          scr_curson();
;
; This routine must not be called directly.  See curson() in term.c
;---------------------------------------------------------------

public  scr_curson_
public  _scr_curson
_scr_curson:
scr_curson_:
        push  bp
        mov   bp,sp
        push  di
        push  si

        mov   cx,0c0dh    ;assume monocrome
        cmp   scr_mode_,7 ;true is mono
        jz    new_cur     ;set it
        mov   cx,0607h    ;assume color card in text mode
        cmp   scr_mode_,4 ;color text is 0 to 3
        jc    new_cur
        pop   si
        pop   di
        pop   bp          ;do nothing if in graphics mode
        ret



;---------------------------------------------------------------
; SCR_MARK   mark the current character on the screen.
;            Used to delimit block areas in SEE. Just write
;            an X or something if reverse video is not available.
;
; Usage:     scr_mark(current character);
;---------------------------------------------------------------

public  scr_mark_
public  _scr_mark
_scr_mark:
scr_mark_:               ;mark the passed char, cursor does not advance
        push  bp
        mov   bp,sp
        push  di
        push  si

        mov   al,219          ;just write a block character
        mov   bl,scr_attr_    ;normal attributes
        mov   cx,1            ;one character
        mov   bh,scr_page_    ;page number
        mov   ah,writeach     ;write char and attr
        int   video           ;write character and attributes

        pop   si
        pop   di
        pop   bp
        ret



;---------------------------------------------------------------
; SCR_APUTS_  write a string and attributes to the screen.
;             goto col, row first. the cursor is moved normally
;
; Usage:      scr_aputs(x,y,"Print This",attribute);
;---------------------------------------------------------------

; the following routine is not used by either SEE or D88.

public scr_aputs_
public _scr_aputs
_scr_aputs:
scr_aputs_:
        push  bp
        mov   bp,sp
        push  di
        push  si
        mov   di,sp

        mov   dx,[bp+4]    ;col
        mov   ax,[bp+6]    ;row
        mov   dh,al

        mov   bh,scr_page_ ;force page
        mov   ah,setcur    ;set cursor location
        push  bp
        int   video        ;dx is cursor location, bh is page
        pop   bp
        mov   si,[bp+8]    ;string pointer
        mov   cx,1         ;number of characters to write
        mov   bl,[bp+10]   ;attribute
naputs:
        cld
        lodsb             ;next character to write
        or    al,al       ;zero at end
        jz    eaputs
        cmp   al,10       ;look for LF
        jnz   normal_ap
ap_scroll:
        mov   bp,sp       ;reset pointer to next char
        mov   [bp+4],si
        mov   al,13       ;use tty output to scroll screen
        mov   ah,wtty
        int   video       ;write cr,lf
        mov   ah,wtty
        mov   al,10
        int   video
        pop   bp
        jmp   scr_aputs_      ;start over
normal_ap:
        mov   bh,scr_page_    ;page number
        mov   ah,writeach     ;write char and attr
        int   video           ;write character and attributes
        inc   dl              ;next column
        cmp   dl,crt_cols     ;see if wrapping around
        jc    set_loc
        mov   dl,0            ;at start of column
        inc   dh              ;at next row
        cmp   dh,scr_rows_  ;see if need a scroll
        jc    set_loc
        jmp   ap_scroll       ;do a scroll up
set_loc:
        mov   ah,setcur       ;move the cursor
        int   video
        jmp   naputs
eaputs:
        mov   sp,di
        pop   si
        pop   di
        pop   bp
        ret




;-------------------------------------------------------------------
; SCR_PUTCH_  write a character to the screen with attribute set.
;             see scr_aputs and scr_co
;
; Usage:      scr_putch( character, attribute );
;-------------------------------------------------------------------

public  scr_putch_
public  _scr_putch
_scr_putch:
scr_putch_:
        push  bp
        mov   bp,sp
        push  di
        push  si

        push  bp
        mov   ah,readcur      ;see where we are
        mov   bh,scr_page_
        int   video           ;dx is cursor location, bh is page
        pop   bp
        mov   al,[bp+4]       ;character to write
        mov   bl,[bp+6]       ;attribute of char
        mov   bh,scr_page_    ;set page number
        mov   cx,1            ;write just one char
        mov   ah,writeach     ;write char and attr
        int   video

        inc   dl              ;move to next column
        mov   ah,setcur       ;move the cursor
        int   video
        pop   si
        pop   di
        pop   bp
        ret



;--------------------------------------------------------
; SCR_DELETE_   clear rest of line starting at column, row.
;
; Usage:        scr_delete(x,y);
;--------------------------------------------------------

public  scr_delete_
public  _scr_delete
_scr_delete:
scr_delete_:
        push  bp
        mov   bp,sp
        push  di
        push  si

        mov   dx,[bp+4]       ;col
        mov   ax,[bp+6]       ;row
        mov   dh,al
        mov   bh,scr_page_    ;force page zero
        mov   ah,setcur       ;set cursor location
        int   video           ;call bios

        mov   cl,scr_cols_   ;calc how many chars left in line
        sub   cl,dl           ;number left
        mov   ch,0            ;number of blanks needed
        mov   al,' '          ;write blanks
        mov   bl,scr_attr_    ;normal attributes
        mov   bh,scr_page_    ;page number
        mov   ah,writeach     ;write the blanks
        int   video
        pop   si
        pop   di
        pop   bp
        ret



;----------------------------------------------------------
;  The following routines get_mode_, calc_video_,
;  set_video_ and scr_putstr were adapted from
;  Computer Language Magazine - January 1986
;  "Assembly Video Routines for your P.C.", Thomas Webb
;----------------------------------------------------------


;----------------------------------------------------------
; GET_MODE_    get video mode
;
; Usage:       internal subroutine used in scr_putstr
;----------------------------------------------------------

public  get_mode_
public  _get_mode
_get_mode:
get_mode_:
      push  bp
      mov   bp,sp
      push  di
      push  si

      mov   ax,0
      mov   ah,15
      int   video
      pop   si
      pop   di
      pop   bp
      ret



;----------------------------------------------------------
; CALC_VIDEO_   calculate offset into video buffer
;               set begining location and attribute
;
; Usage:        internal subroutine used in scr_putstr
;----------------------------------------------------------

public  calc_video_
public  _calc_video
_calc_video:
calc_video_:
        mov   ax,crt_cols
        mul   dx
        add   ax,cx
        shl   ax,1
        ret



;-------------------------------------------------------
; SET_VIDEO_    set video address
;
; Usage:        internal subroutine used in scr_setup
;-------------------------------------------------------

public  set_video_
public  _set_video
_set_video:
set_video_:
        call  get_mode_
        cmp   al,7h
        jne   adp_graphics
        mov   ax,mono_adr
        jmp   return
adp_graphics:
        mov   ax,graph_adr
return:
        mov   vadrs,ax
        ret



;----------------------------------------------------------
; SCR_PUTSTR_   write a string directly to video buffer
;               at col, row location with attribute.
;               Doesn't change cursor position.
;
; Usage:        scr_putstr( x, y, string, attribute );
;----------------------------------------------------------

public  scr_putstr_
public  _scr_putstr
_scr_putstr:
scr_putstr_:
        push  bp
        mov   bp,sp
        push  di
        push  si
        push  es

        mov   dx,[bp+6]     ;get row
        mov   cx,[bp+4]     ;get col
        mov   ax,crt_cols   ;calc video routine
        mul   dx            ;moved here
        add   ax,cx         ;to save a call
        shl   ax,1
        mov   di,ax         ;return position

        mov   si,[bp+8]    ;get string pointer
        mov   dh,[bp+10]    ;get attribute

        mov   ax,vadrs      ;get display address from setup
        mov   es,ax         ;move absolute display address to es
        mov   cl,0          ;used to test for end of string

str_loop:
        mov   dl,[si]        ;get the next byte from the string
        cmp   dl,cl          ;if end of string then exit
        je    str_exit
        mov   es:[di],dx     ;move a word into screen display

        add   di,2h          ;bump display index
        inc   si             ;bump index to string
        jmp   str_loop
str_exit:
        pop   es
        pop   si
        pop   di
        pop   bp
        ret



;---------------------------------------------------------------
; SCR_APUTCH_  write a character to the screen with
;              attribute set. see scr_aputs and scr_co
;
; Usage:       scr_aputch(x,y,char,attribute);
;
;     After getting this to work found that it isn't that fast.
;     However, it's a good replacement for gotoxy() scr_putch()
;     if you're only writing one char to a specific location.
;---------------------------------------------------------------

public  scr_aputch_
public  _scr_aputch
_scr_aputch:
scr_aputch_:
        push  bp
        mov   bp,sp
        push  di
        push  si
        push  es

        mov   dx,[bp+6]     ;get col
        mov   cx,[bp+4]     ;get row
        call  calc_video_   ;locate position in video buffer
        mov   di,ax         ;return position

        mov   dl,[bp+8]    ;get char
        mov   dh,[bp+10]    ;get attribute

        mov   ax,vadrs      ;get video address
        mov   es,ax         ;move buffer address to es

        mov   es:[di],dx

        pop   es
        pop   si
        pop   di
        pop   bp
        ret


;  write a graphics dot
; plot(color,horz,vert)
; int color,horz,vert;

public plot_
public _plot
_plot:
plot_:
   push  bp
   mov   bp,sp
   push  di
   push  si

   call  setdot
   mov   ah,12
   int   video
   pop   si
   pop   di
   pop   bp
   ret

setdot:
   mov   bp,sp
   mov   ax,[bp+4]    ;color
   mov   cx,[bp+6]    ;horiz
   mov   dx,[bp+8]   ;vert
   ret

; *** *** *** *** *** *** *** *** ***
;
; Mixed memory model string move.  Programmed in assembly language for
; speed.  The string must not cross a 64 k boundary. Equivalent C code:
;
; movesf(sptr,fptr)
; char far *fptr;
; char near *sptr;
; {
;     while (*sptr++ = *fptr++)
;         ;
;     return;
; }
;

        PUBLIC  _movesf
_movesf  PROC NEAR
         push  bp
         mov   bp,sp
         push  si
         mov  si,[bp+4]  ;sptr
         les  bx,[bp+6]  ;fptr

msp1:    mov  al,es:[bx]
         mov  [si],al
         inc  si
         inc  bx
         or  al,al
         jne  msp1

         pop  si
         mov  sp,bp
         pop  bp
         ret

_movesf  ENDP

; that's all

_TEXT ENDS
END
