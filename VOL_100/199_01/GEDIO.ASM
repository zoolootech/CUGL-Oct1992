;------------------------------------------------------------------
; Gedio.asm: Screen and keyboard interface routines for the PC
;
; For GED WordStar-like screen editor
; DeSmet C implementation by Mel Tearle - 08/06/86
;
; Microsoft Macroassembler implementation by Michael Yokoyama - 12/06/86
; Revised by Mel Tearle - 07/25/87
;
; data starts here
;------------------------------------------------------------------

_data segment word public 'data'
const segment word public 'const'
const ends

_bss segment word public 'bss'
_bss ends

 dgroup group const,_bss,_data
 assume cs: _text, ds: dgroup, ss: dgroup, es: dgroup

; In this implementation, all special and function keys are translated
; to the following values.
; control key translations


; control key translations - mapped into cursor-pad.

up_char         equ   5     ;arrow up
down_char       equ  24     ;arrow down
left_char       equ  19     ;arrow left
right_char      equ   4     ;arrow right
bol_char        equ  15     ;Home
eol_char        equ  16     ;End
pageup_char     equ  18     ;PgUp
pagedown_char   equ   3     ;PgDn
Del_char        equ   7     ;Del

; function key definitions

M1      equ 128
M2      equ 129
M3      equ 130
M4      equ 145
M5      equ 132
M6      equ 133
M7      equ 134
M8      equ 135
M9      equ 136
M10     equ 137

; special keys - toggles and keypress

Insert_key      equ 138
Caps_Lock_on    equ 139
Caps_Lock_off   equ 140
Alt_key         equ 146

grey_minus      equ 142
grey_plus       equ 143


; the table that is used to make the translation
; note - not all used

convert db  72,  up_char
        db  80,  down_char
        db  75,  left_char
        db  77,  right_char
        db  71,  bol_char
        db  79,  eol_char
        db  73,  pageup_char
        db  81,  pagedown_char
;       db  78,  grey_plus
;       db  74,  grey_minus
        db  82,  Insert_key
        db  83,  Del_char

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
        db   0, 255     ;illegal character


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

; used in set_video

mono_adr     equ  0b000h   ;mono adapter
graph_adr    equ  0b800h   ;graphics adapter*

vadrs        dw  0

; the interrupt and codes for the keyboard interface.

keyboard     equ 16h     ;interrupt 16 to deal with keyboard

cicode       equ 0       ;code for reading a character
cstscode     equ 1       ;reports if character is ready
kbstatus     equ 2       ;get keyboard status - shifts, caplocks

kbset        db  0

; caution: must change column number if 40 column mode

crt_cols     equ 80

; Note: 25 for MS-DOS, 24 for CP/M as cp/m steals the bottom line.

_scr_rows dw 24           ; current number of rows, for ged, was 25
_scr_cols dw crt_cols     ; current number of columns
_scr_mode db 0            ; current screen mode
_scr_page db 0            ; current page
_scr_attr db 7            ; current attributes for screen
                          ; 7 is white letters on black
_scr_window_top db 1      ; first line to scroll, was 0

_data ends

;------------------------------------------------------------------
;  code starts here
;------------------------------------------------------------------

 _text segment byte public 'code'
 assume cs:_text

; Variables available to C programs

 public _scr_cols,    _scr_rows,     _scr_xy,      _scr_clr
 public _scr_mode,    _scr_page,     _scr_attr,    _scr_window_top
 public _scr_setup,   _scr_term,     _scr_csts,    _scr_setmode
 public _scr_clrl,    _scr_cls,      _scr_scup,    _scr_scrup
 public _scr_scrdn,   _scr_co,       _scr_ci,      _get_graphic
 public _scr_sinp,    _scr_cursoff,  _scr_curson,  _scr_mark
 public _scr_putch,   _scr_delete,   _get_mode,    _calc_video
 public _scr_putstr,  _scr_aputch,   _get_kbstat,  _caps_on
 public _scr_scdn,    _scr_aputs,    _set_video


;-------------------------------------------------------------------------
;  _scr_setup   Call _scr_setup before using any other routine unless the
;               starting mode is 80X25 character mode (3,4, or 7).
;               This routine must be called for monochrome (mode 7)
;               for _scr_curson to set a proper cursor.
;
;  Usage: mode = _scr_setup();
;-------------------------------------------------------------------------

_scr_setup proc near
    push  bp
    mov   bp, sp

    mov   ah,state           ; get current state
    int   video
    mov   _scr_mode, al      ; current mode
    mov   cl,ah              ; make cols a word
    mov   ch,0
    mov   _scr_cols, cx      ; 40 or 80 columns
    mov   _scr_page, bh
    mov   _scr_attr, 7       ; set to white chars on black
    cmp   al, 4              ; see if a character mode
    jc    got_attr
    cmp   al, 7              ; 7 is for graphics mode
    jz    got_attr
    mov   _scr_attr, 0       ; attribute is zero in graphics
got_attr:
    mov   ah,0               ; return int containing mode
    push  ax                 ; slight detour
    call  _set_video         ; get display address
    pop   ax

    pop   bp
    ret
_scr_setup endp


;-------------------------------------------------------
; _scr_term   Do any required termination.
;
;  Usage:     _scr_term();
;-------------------------------------------------------

_scr_term proc near
    ret
_scr_term endp


;-------------------------------------------------------
;  _scr_setmode    Set a new screen mode.
; 
;  Usage:          _scr_setmode(new mode);
;-------------------------------------------------------

_scr_setmode proc near
    push  bp
    mov   bp, sp

    mov   al, [bp+4]  ; new mode value
    mov   ah, mode
    int   video       ; set new mode
    call  _scr_setup  ; remember new values

    pop   bp
    ret
_scr_setmode endp


;-------------------------------------------------------
;  _scr_xy   Sets cursor at any location.
;
;  Usage:    _scr_xy( col, row );
;-------------------------------------------------------

_scr_xy  proc near
    push  bp
    mov   bp, sp

    mov   dx, [bp+4]          ; col
    mov   ax, [bp+6]          ; row
    mov   dh, al
    mov   bh, _scr_page       ; force page zero
    mov   ah, setcur          ; set cursor location
    int   video               ; call BIOS

    pop   bp
    ret
_scr_xy endp


;-------------------------------------------------------
;  _scr_clr   Clear entire screen
;
;  Usage:     _scr_clr();
;-------------------------------------------------------

_scr_clr proc near
    push  bp
    mov   bp, sp

    mov   al, 0              ; ask for a clear window
    xor   cx, cx             ; start at 0,0
    mov   dh, byte ptr _scr_rows     ;last line
    dec   dh
    mov   dl, byte ptr _scr_cols     ;clear entire width
    dec   dl                 ; last column is width-1
    mov   bh, _scr_attr      ; attributes for new blanks
    mov   ah, scrollup       ; ask for a scrollup to clear
    int   video              ; do the clear

    pop   bp
    ret
_scr_clr endp


;-------------------------------------------------------
;  _scr_clrl    Clear rest of line.
;
;  Usage:       _scr_clrl();
;-------------------------------------------------------

_scr_clrl proc near
    push  bp
    mov   bp, sp

    mov   bh, _scr_page
    mov   ah, readcur         ; see where we are
    int   video

    mov   cl, byte ptr _scr_cols     ; calc how many chars left in line
    sub   cl, dl              ; number left
    mov   ch, 0               ; number of blanks needed
    mov   al, ' '             ; write blanks
    mov   bl, _scr_attr       ; normal attributes
    mov   bh, _scr_page       ; page number
    mov   ah, writeach        ; write the blanks
    int   video

    pop   bp
    ret
_scr_clrl endp


;-------------------------------------------------------
;  _scr_cls     Clear rest of screen starting at y
;               set line pointer, clear screen, reset line pointer
;
;  Usage:      _scr_cls(y);
;-------------------------------------------------------

_scr_cls proc near
    push  bp
    mov   bp, sp

    mov   dx, 0               ; hard-code column at 0
    mov   ax, [bp+4]          ; get row
    mov   dh, al

    mov   bh, _scr_page       ; force page zero
    mov   ah, setcur          ; set cursor location
    int   video               ; call bios

    call  _scr_clrl           ; clear rest of line

    mov   al, 0               ; ask for a clear window
    mov   ch, [bp+4]          ; re-use row

    inc   ch                  ; bump row
    cmp   ch, byte ptr _scr_rows   ; see if in last line
    jz    cleared             ; all done
    mov   cl, 0               ; first column
    mov   dh, byte ptr _scr_rows   ; 24 is the last line
    dec   dh
    mov   dl, byte ptr _scr_cols   ; clear entire width
    dec   dl                  ; last column is width-1
    mov   bh, _scr_attr       ; attributes for new blanks
    mov   ah, scrollup        ; ask for a scrollup to clear
    int   video               ; do the clear
cleared:
    mov   dx, 0               ; set column again
    mov   ax, [bp+4]          ; get row
    mov   dh, al

    mov   bh, _scr_page       ; force page zero
    mov   ah, setcur          ; set cursor location
    int   video               ; call bios

    pop   bp
    ret
_scr_cls endp


;--------------------------------------------------------
;  _scr_delete    Clears rest of line starting at x,y.
;
;  Usage:         _scr_delete(x,y);
;--------------------------------------------------------

_scr_delete proc near
    push  bp
    mov   bp, sp

    mov   dx, [bp+4]          ; col
    mov   ax, [bp+6]          ; row
    mov   dh, al

    mov   bh, _scr_page       ; force page zero
    mov   ah, setcur          ; set cursor location
    int   video               ; call BIOS

    mov   cl, byte ptr _scr_cols        ; calc how many chars left in line
    sub   cl, dl              ; number left
    mov   ch, 0               ; number of blanks needed
    mov   al, ' '             ; write blanks
    mov   bl, _scr_attr       ; normal attributes
    mov   bh, _scr_page       ; page number
    mov   ah, writeach        ; write the blanks

    int   video
    pop   bp
    ret
_scr_delete endp


;--------------------------------------------------------
;  _scr_scup    Scroll text up from last line to
;               _scr_window_top, leaving top lines alone.
;
;  Usage:      _scr_scup();
;--------------------------------------------------------

_scr_scup proc near

    mov   ax, _scr_cols        ; need last column of screen
    dec   ax
    push  ax
    mov   ax, _scr_rows        ; scroll through last line
    dec   ax
    push  ax
    xor   ax, ax               ; from column 0
    push  ax
    mov   al, _scr_window_top  ;leave top line alone
    push  ax
    mov   al, 1
    push  ax                   ; scroll by 1
    call  _scr_scrup           ; do the scroll
    add   sp, 10               ; clear arge

    ret
_scr_scup endp


;---------------------------------------------------------------
;  _scr_scrup      Scroll the screen up. The window is scrolled
;                  up nline lines. A zero nline will clear the
;                  window. Top left of the screen in 0,0.
;
;  Usage:          _scr_scrup(nline,fromrow,fromcol,torow,tocol);
;---------------------------------------------------------------

_scr_scrup proc near
    push bp
    mov  bp, sp

    mov  al, [bp+4]          ; number of lines
    mov  ch, [bp+6]          ; starting row
    mov  cl, [bp+8]          ; starting column
    mov  dh, [bp+10]         ; ending row
    mov  dl, [bp+12]         ; ending column
    mov  bh, _scr_attr       ; current attribute
    mov  ah, scrollup
    int  video               ; do the scroll

    pop  bp
    ret
_scr_scrup endp


;-------------------------------------------------------------
;  _scr_scdn    Scroll all but the top lines down one.
;
;  Usage:      _scr_scdn();
;-------------------------------------------------------------

_scr_scdn proc near

    mov   ax, _scr_cols         ; need last column of screen
    dec   ax
    push  ax
    mov   ax, _scr_rows         ; scroll through last line
    dec   ax
    push  ax
    xor   ax, ax                ; from column 0
    push  ax
    mov   al, _scr_window_top   ; leave top lines alone
    push  ax
    mov   al, 1
    push  ax                    ; scroll by 1
    call  _scr_scrdn            ; do the scroll
    add   sp, 10                ; clear arge

    ret
_scr_scdn endp


;---------------------------------------------------------------
;  _scr_scrdn    Scroll the screen down. the window is scrolled
;                down nline lines. A zero nline will clear the
;                window. Top left of the screen in 0,0.
;
;  Usage:        _scr_scrdn(nline,fromrow,fromcol,torow,tocol);
;---------------------------------------------------------------

_scr_scrdn proc near
    push  bp
    mov   bp, sp

    mov   al, [bp+4]          ; number of lines
    mov   ch, [bp+6]          ; starting row
    mov   cl, [bp+8]          ; starting column
    mov   dh, [bp+10]         ; ending row
    mov   dl, [bp+12]         ; ending column
    mov   bh, _scr_attr       ; current attribute
    mov   ah, scrolldn
    int   video               ; do the scroll

    pop   bp
    ret
_scr_scrdn endp


;---------------------------------------------------------------
;  _scr_co     Write a character to the screen TTY style. This
;              routine increments the cursor position
;              after writing.
;
;  Usage:      _scr_co(character);
;---------------------------------------------------------------

_scr_co proc near
    push  bp
    mov   bp, sp

    mov   al, [bp+4]          ; character to write
    mov   bh, _scr_page
    mov   ah, wtty            ; use tty write routine
    int   video

    pop bp
    ret
_scr_co endp


;---------------------------------------------------------------
;  _scr_ci        Keyboard input. function and soft keys are
;                 translated. see equates for values.
;
;  Usage:         character = _scr_ci();
;---------------------------------------------------------------

_scr_ci proc near
    push  bp
    mov   ah, cicode          ; ask for a keyboard character
    int   keyboard            ; do interrupt

    cmp   ax, 4a2dh           ; test for grey keys
    je    minus_grey

    cmp   ax, 4e2bh           ; grep plus by default
    jne   continue

    mov   al, grey_plus
    jmp   not_special

minus_grey:
    mov   al, grey_minus
    jmp   not_special

continue:
    cmp   al, 0               ; is low byte equal to zero?
    jne   not_special         ; it's not - must be ascii so return it
    mov   bx, ds:[offset convert]  ; else convert to special key
ci_loop:
    cmp   byte ptr [bx], 0    ; are we done?
    jz    got_it              ; it's zero - so end routine
    cmp   ah, byte ptr [bx]   ; else does it equal the high byte?
    je    got_it              ; got a hit - finish up
    add   bx, 2               ; else bump bx
    jmp   ci_loop
got_it:
    inc   bx                  ; get return value from convert
    mov   al, byte ptr [bx]   ; put it in low byte
not_special:
    mov   ah, 0               ; normal return

    pop   bp
    ret
_scr_ci endp


;---------------------------------------------------------------
;  _scr_csts     Returns character if available, otherwise zero.
;
;  Usage:        character = _scr_csts();
;---------------------------------------------------------------

_scr_csts proc near
    push  bp
    mov   ah, cstscode
    int   keyboard

    mov   ax, 0
    jz    kbstat
    call  _scr_ci             ; get the coded character
    jmp   fin
kbstat:
    call  _get_kbstat
fin:
    pop bp
    ret
_scr_csts endp


;---------------------------------------------------------------
; _get_kbstat   check status of insert key, capslock and alt key
;               return special value or zero.
;
; Usage:        internal routine used by scr_csts();
;---------------------------------------------------------------

_get_kbstat  proc near
    push  bp

    mov   ah, kbstatus      ; get keyboard status
    int   keyboard

    cmp   al, kbset         ; set if keyboard status is changed
    je    return_0          ; if same - return 0

    mov   kbset, al         ; new keyboard toggle - save it

    test  al, 08h           ; test for alt press
    jnz   alt_pressed

    test  al, 40h           ; test for caps lock on
    jnz   caps_on

    and   al, 40h           ; reverse bits
    test  al, 40h           ; test if caps lock off
    jz    caps_off          ; if zero then toggled off

return_0:
    mov   al, 0             ; return 0 - no hits
    jmp   fin3
alt_pressed:
    mov   al, Alt_key
    jmp   fin3
caps_on:
    mov   al, Caps_Lock_on
    jmp   fin3
caps_off:
    mov   al, Caps_Lock_off
fin3:
    mov   ah, 0

    pop   bp
    ret
_get_kbstat endp


;---------------------------------------------------------------
; _caps_on     check status of capslock only
;              if on then return CAPSLKON
;
; Usage:       if ( caps_on() )  do something
;---------------------------------------------------------------

_caps_on proc near
     push  bp

     mov   ah, kbstatus      ; get keyboard status
     int   keyboard

     test  al, 40h           ; test for caps lock on
     jnz   caps_on1

     mov   al, 0             ; return 0 - no hits
     jmp   fin4
caps_on1:
     mov   al, Caps_Lock_on
fin4:
     mov   ah, 0

     pop   bp
     ret
_caps_on  endp


;---------------------------------------------------------------
;  _get_graphic   return the next char in buffer - unfiltered
;
;  Usage:         character = get_graphic();
;---------------------------------------------------------------

_get_graphic proc near
    push  bp

    mov   ah, cicode          ; ask for a keyboard character
    int   keyboard            ; do interrupt
    mov   ah, 0               ; normal return

    pop   bp
    ret
_get_graphic endp


;---------------------------------------------------------------
;  _scr_cursoff       Turns cursor off.
;
;  Usage:            _scr_cursoff();
;---------------------------------------------------------------

_scr_cursoff proc near
    push  bp                 ; save registers
    cmp   _scr_mode, 4       ; see if graphics
    jc    text_coff
    cmp   _scr_mode, 7
    jnz   no_cur
text_coff:
    mov   cx, 0f00h          ; should turn cursor off
new_cur:
    mov   ah, curtype        ; set a new cursor type
    int   video
no_cur:

    pop bp
    ret
_scr_cursoff endp


;---------------------------------------------------------------
;  _scr_curson       Turns cursor on.
;
;  Usage:           _scr_curson();
;---------------------------------------------------------------

_scr_curson proc near
    push  bp
    mov   cx, 0c0dh           ; assume monocrome
    cmp   _scr_mode, 7        ; true is mono
    jz    new_cur             ; set it
    mov   cx,0607h            ; assume color card in text mode
    cmp   _scr_mode,4         ; color text is 0 to 3
    jc    new_cur

    pop   bp                  ; do nothing if in graphics mode
    ret
_scr_curson endp


;---------------------------------------------------------------
;  _scr_aputs     Writes a string and attributes to the screen.
;                 The cursor is moved normally.
;                 Standard Bios write.
;
;  Usage:        _scr_aputs( x,y, "Print This", attribute );
;---------------------------------------------------------------

_scr_aputs proc near
    push  bp
    mov   bp, sp

    mov   dx, [bp+4]       ; col
    mov   ax, [bp+6]       ; row
    mov   dh, al

    mov   bh, _scr_page    ; force page
    mov   ah, setcur       ; set cursor location
    int   video            ; dx is cursor location, bh is page

    mov   si, [bp+8]       ; string pointer
    mov   cx, 1            ; number of characters to write
    mov   bl, [bp+10]      ; attribute
naputs:
    cld
    lodsb                  ; next character to write
    or    al, al           ; zero at end
    jz    eaputs
    cmp   al, 10           ; look for LF
    jnz   normal_ap
ap_scroll:
    mov   bp, sp           ; reset pointer to next char
    mov   [bp+4], si
    mov   al, 13           ; use tty output to scroll screen
    mov   ah, wtty
    int   video            ; write cr,lf
    mov   ah, wtty
    mov   al, 10
    int   video
    pop   bp
    jmp   _scr_aputs       ; start over
normal_ap:
    mov   bh, _scr_page    ; page number
    mov   ah, writeach     ; write char and attr
    int   video            ; write character and attributes
    inc   dl               ; next column
    cmp   dl, byte ptr crt_cols     ; see if wrapping around
    jc    set_loc
    mov   dl, 0            ; at start of column
    inc   dh               ; at next row
    cmp   dh, byte ptr _scr_rows    ; see if need a scroll
    jc    set_loc
    jmp   ap_scroll        ; do a scroll up
set_loc:
    mov   ah, setcur       ; move the cursor
    int   video
    jmp   naputs
eaputs:

    pop bp
    ret
_scr_aputs endp


;-------------------------------------------------------------------
;  _scr_putch     Write a character to the screen with attribute set.
;                 standard Bios write
;
;  Usage:        _scr_putch( character, attribute );
;-------------------------------------------------------------------

_scr_putch proc near
    push  bp
    mov   bp, sp

    mov   ah, readcur        ; see where we are
    mov   bh, _scr_page
    int   video              ; dx is cursor location, bh is page

    mov   al, [bp+4]         ; character to write
    mov   bl, [bp+6]         ; attribute of char
    mov   bh, _scr_page      ; set page number
    mov   cx, 1              ; write just one char
    mov   ah, writeach       ; write char and attr
    int   video

    inc   dl                 ; move to next column
    mov   ah, setcur         ; move the cursor
    int   video

    pop   bp
    ret
_scr_putch endp


;--------------------------------------------------------------
;
;   The following routines _get_mode, _calc_video, _set_video
;   and _scr_putstr were adapted from
;   Computer Language Magazine - January 1986
;   "Assembly Video Routines for your P.C.",  Thomas Webb
;
;--------------------------------------------------------------


;--------------------------------------------------------------
;  _get_mode       get video mode
;
;  Usage:          internal subroutine used in _scr_putstr
;--------------------------------------------------------------

_get_mode  proc near
    push bp

    mov  ax, 0
    mov  ah, 15
    int  10h

    pop  bp
    ret
_get_mode endp


;----------------------------------------------------------
;  _calc_video    calculate offset into video buffer
;                 set begining location and attribute
;
;  Usage:         internal subroutine used in _scr_putstr
;----------------------------------------------------------

_calc_video  proc near
    mov  ax, byte ptr crt_cols
    mul  dx
    add  ax, cx
    shl  ax, 1

    ret
_calc_video endp


;-------------------------------------------------------
;  _set_video     set video address
;
;  Usage:         internal subroutine used in _scr_putstr
;-------------------------------------------------------

_set_video  proc near
    call  _get_mode
    cmp   al,  7h
    jne   adp_graphics
    mov   ax, mono_adr
    jmp   return
adp_graphics:
    mov   ax, graph_adr
return:
    mov   vadrs, ax

    ret
_set_video endp


;----------------------------------------------------------
;  _scr_putstr      Writes a string directly to video buffer,
;                   given the (x,y) location and attribute.
;
;  Usage:           _scr_putstr(x, y, string, attribute);
;----------------------------------------------------------

_scr_putstr proc near
    push  es
    push  bp
    mov   bp, sp

    mov   dx, [bp+8]       ; get col
    mov   cx, [bp+6]       ; get row

; locate position in video buffer - calc video moved to save a call

    mov   ax, crt_cols     ; calc video routine
    mul   dx               ; moved here
    add   ax, cx           ; to save a call
    shl   ax, 1
    mov   di, ax           ; return position

    mov   si, [bp+10]      ; get string
    mov   dh, [bp+12]      ; get attribute
    mov   es, vadrs        ; get display address from setup

str_loop:
    mov   dl, byte ptr [si]    ; move the cursor
    cmp   dl, 0                ; if end then exit
    je    str_exit

    mov   es:word ptr [di], dx   ; move word into screen display
    add   di, 2h             ; bump string pointer
    inc   si
    jmp   str_loop

str_exit:

    pop   bp
    pop   es
    ret
_scr_putstr endp


;---------------------------------------------------------------
;  _scr_aputch         Writes a character to the screen with
;                      attribute set.  See _scr_aputs and _scr_co
;
;  Usage: _scr_aputch(x,y,char,attribute);
;
;  After getting this to work found that it isn't that fast.
;  However, it's a good replacement for gotoxy() _scr_putch()
;  if you're only writing one char to a specific location.
;
;---------------------------------------------------------------

_scr_aputch proc near
    push  es
    push  bp
    mov   bp, sp

    mov   dx, [bp+8]         ; get col
    mov   cx, [bp+6]         ; get row
    call  _calc_video        ; locate position in video buffer
    mov   di, ax             ; return position
    mov   dl, [bp+10]        ; get char
    mov   dh, [bp+12]        ; get attribute
    mov   ax, vadrs          ; get video address
    mov   es, ax             ; move buffer address to es
    mov   es:word ptr [di], dx

    pop   bp
    pop   es
    ret
_scr_aputch endp


;---------------------------------------------------------------
;  _scr_sinp    Screen input (read character from the screen).
;
;  Usage:       character = _scr_sinp();   not used
;---------------------------------------------------------------

_scr_sinp proc near
    push  bp               ; save the registers
    mov   bh, _scr_page
    mov   ah, readch       ; Code to read a character
    int   video            ; al is letter, ah=attributes
    or    al, al           ; zero returned instead of blank in
                           ; graphics mode
    jnz   ret_ch
    mov   al, ' '
ret_ch:
    mov   ah, 0            ; kill the attributes

    pop   bp
    ret
_scr_sinp endp


;---------------------------------------------------------------
;  _scr_mark       Marks the current character on the screen.
;                  Used to delimit block.
;
;   Usage: _scr_mark(current character)   not used
;---------------------------------------------------------------

;mark the passed char, cursor does not advance
_scr_mark proc near
    push bp
    mov  al, 219             ; just write a block character
    mov  bl, _scr_attr       ; normal attributes
    mov  cx, 1               ; one character
    mov  bh, _scr_page       ; page number
    mov  ah, writeach        ; write char and attr
    int  video               ; write character and attributes

    pop  bp
    ret
_scr_mark endp

_text ends

; That's all!

end
