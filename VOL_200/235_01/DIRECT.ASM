        PAGE   60,132
        TITLE  Routines to do direct screen I/O

;  011  12-Dec-86  direct.asm

;       Copyright (c) 1986 by Blue Sky Software.  All rights reserved.


_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS
CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS
_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS
_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS

DGROUP  GROUP   CONST,  _BSS,   _DATA
        ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP

_DATA   SEGMENT
        EXTRN   _screen:DWORD
        EXTRN   _cursor:DWORD
        EXTRN   _vid_attrib:BYTE
        EXTRN   _vid_snow:BYTE
        boxloc  dw    0           ; stores the starting offset to the dialog box
_DATA   ENDS

_TEXT      SEGMENT

;******************************************************************************
;
;  gotorc(r,c)    move 'cursor' to specified row, col
;
;******************************************************************************

        PUBLIC _gotorc

_gotorc PROC   NEAR
        push    bp
        mov     bp,sp

        mov     ax,[bp+4]                      ; row to ax
        mov     cl,5
        shl     ax,cl                          ; row * 32
        mov     bx,ax
        shl     ax,1
        shl     ax,1                           ; row * 128
        add     ax,bx                          ; row * 160

        mov     bx,[bp+6]                      ; col to bx
        shl     bx,1                           ; col * 2

        add     ax,bx                          ; row * 160 + col * 2 = cursor
        mov     WORD PTR _cursor,ax

        mov     sp,bp
        pop     bp
        ret

_gotorc ENDP

;******************************************************************************
;
;  disp_str(s)    display a string at current location
;
;******************************************************************************

        PUBLIC  _disp_str

_disp_str  PROC NEAR
        push    bp
        mov     bp,sp
        push    di
        push    si
        mov     si,[bp+4]

        mov     ah,_vid_attrib         ; attribute byte to ah
        les     di,DWORD PTR _cursor   ; cursor ptr to es:di

        jmp     SHORT tst_ch           ; skip to load/test code

chloop:

        IFDEF  nosnow                  ; if adapter doesn't snow, its quick
           stosw                       ; store char and attrib to es:[di++]
        ELSE                           ; well, its not so quick or easy....
           mov    cx,1
           call   stvideo
        ENDIF

tst_ch: lodsb                          ; string char to al
        or      al,al                  ; done when char = 0
        jne     chloop

        mov     WORD PTR _cursor,di    ; update cursor offset

        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_disp_str ENDP

;******************************************************************************
;
;   disp_char(ch)   display a single char at current location
;
;******************************************************************************

        PUBLIC  _disp_char

_disp_char  PROC NEAR
        push    bp
        mov     bp,sp
        push    di

        les     di,DWORD PTR _cursor   ; cursor loc to es:di

        mov     al,[bp+4]              ; get char to store in video memory
        mov     ah,_vid_attrib         ; get video attribute

        IFDEF nosnow
           stosw                       ; store 'em and update di
        ELSE
           mov  cx,1
           call stvideo
        ENDIF

        mov     WORD PTR _cursor,di    ; update cursor offset

        pop     di
        mov     sp,bp
        pop     bp
        ret
_disp_char      ENDP


;******************************************************************************
;
;   disp_rep(ch,cnt)   display a single char cnt times at current location
;
;******************************************************************************

        PUBLIC  _disp_rep

_disp_rep  PROC NEAR
        push    bp
        mov     bp,sp
        push    di

        les     di,DWORD PTR _cursor   ; cursor loc to es:di

        mov     al,[bp+4]              ; get char to store in video memory
        mov     ah,_vid_attrib         ; get video attribute
        mov     cx,[bp+6]              ; rep count to cx

        IFDEF  nosnow
           rep stosw                   ; store 'em and update di
        ELSE
           call stvideo
        ENDIF

        mov     WORD PTR _cursor,di    ; update cursor offset

        pop     di
        mov     sp,bp
        pop     bp
        ret
_disp_rep  ENDP


;******************************************************************************
;
;       insert_line(r,n)   insert a line at row, effects n lines
;
;******************************************************************************

        PUBLIC _insert_line

_insert_line PROC NEAR

        push    bp
        mov     bp,sp
        push    di
        push    si

        mov     bx,[bp+4]              ; ( r + n - 1) * 160 - 2 =
        add     bx,[bp+6]              ;   end of new last row
        dec     bx
        mov     ax,160
        imul    bx
        dec     ax
        dec     ax
        mov     si,ax                  ;   (but its the source for the move)

        add     ax,160                 ; call addr of dest (where new last
        mov     di,ax                  ;   line will go)

        std                            ; to insert a row, need to go backwards

        call    scroll_video           ; scroll the video buffer

        mov     al,20h                 ; fill the inserted line with blanks
        mov     ah,_vid_attrib
        mov     cx,80
        IFDEF  nosnow
           rep stosw
        ELSE
           call stvideo
        ENDIF

        cld                            ; C expects it this way

        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret

_insert_line ENDP


;******************************************************************************
;
;       delete_line(r,n)   delete a line at row, effects n lines
;
;******************************************************************************

        PUBLIC _delete_line

_delete_line PROC NEAR

        push    bp
        mov     bp,sp
        push    di
        push    si

        mov     ax,160                 ; get row
        imul    WORD PTR [bp+4]        ;   turn into offset in video ram
        mov     di,ax

        add     ax,160                 ; calc offset of next row
        mov     si,ax

        call    scroll_video           ; scroll the video buffer

        mov     al,20h                 ; fill the last line with blanks
        mov     ah,_vid_attrib
        mov     cx,80
        IFDEF  nosnow
           rep stosw
        ELSE
           call stvideo
        ENDIF

        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_delete_line  ENDP


;*****************************************************************************
;
;   scroll_video   support routine for insert/delete line
;
;*****************************************************************************

scroll_video PROC NEAR

        mov     ax,80                  ; get # rows to move and
        imul    WORD PTR [bp+6]        ;   turn into # words to move
        mov     cx,ax

        IFNDEF  nosnow
           mov  al,_vid_snow           ; movideo needs vid_snow - get it
        ENDIF                          ;   before ds gets changed

        push    ds                     ; save current ds
        mov     bx,WORD PTR _screen+2  ; segment address of video ram
        mov     ds,bx                  ; moving to/from video ram
        mov     es,bx

        IFDEF   nosnow
           rep movsw                   ; scroll the data in the video buffer
        ELSE
           call movideo
        ENDIF

        pop    ds                      ; restore ds

        ret

scroll_video ENDP


;******************************************************************************
;
;       scrcpy(to,from)
;       char far *to, far *from;
;
;       copy screen image from to
;
;******************************************************************************

        PUBLIC  _scrcpy

_scrcpy PROC NEAR

        push    bp
        mov     bp,sp
        push    di
        push    si
        push    ds

        IFNDEF  nosnow
        mov     al,_vid_snow           ; flag for movideo
        ENDIF

        les     di,[bp+4]              ; es:di is to address
        lds     si,[bp+8]              ; ds:si is from address
        mov     cx,80*25               ; HARDCODED screen size in words

        IFDEF   nosnow                 ; move the screen image
        rep movsw
        ELSE
        call    movideo
        ENDIF

        pop     ds
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret

_scrcpy ENDP


;******************************************************************************
;
;       popup(row,col,nrows,ncols,savp)
;
;       pop up a dialog box on the screen starting at row,col.
;       If savp is not NULL, the current contents of the video ram
;       under the box are saved.
;
;******************************************************************************

        PUBLIC  _popup

_popup  PROC NEAR
        push    bp
        mov     bp,sp
        push    di
        push    si

        mov     ax,160                         ; calc offset to start of
        imul    WORD PTR [bp+4]                ; dialog box -
        mov     cx,[bp+6]                      ; row * 160 + (col << 1)
        shl     cx,1
        add     ax,cx

        mov     si,ax                          ; si now -> box start offset
        mov     boxloc,si                      ; save it for later

        mov     di,[bp+12]                     ; get savp, the save addr ptr
        or      di,di                          ; does user want contents saved?
        je      skipsave

        mov     dx,si                          ; save current row offset

        mov     bh, BYTE PTR [bp+8]            ; nrows to bh
        mov     bl, BYTE PTR [bp+10]           ; ncols to bl

        mov     ax,ds                          ; set es and ds so str instrs
        mov     es,ax                          ; work the way we want
        mov     cx,WORD PTR _screen+2          ; ds points to video ram
        mov     ds, cx

        IFNDEF  nosnow
           mov  al,es: _vid_snow               ; movideo needs _vid_snow
           push ax
        ENDIF

nxtrow: mov     cl,bl                          ; move ncols to cx
        xor     ch,ch

        IFDEF   nosnow
           rep movsw                           ; move data
        ELSE
           pop  ax                             ; get/save _vid_snow
           push ax
           call movideo
        ENDIF

        add     dx,160                         ; calc offset of next row
        mov     si,dx

        dec     bh                             ; one more row done, any more?
        jnz     nxtrow

        IFNDEF  nosnow
           pop  ax                             ; clear stack
        ENDIF

        mov     ax,es                          ; restore the ds reg
        mov     ds,ax

        mov     si,boxloc                      ; reget box starting offset

skipsave:

        mov     di,si                          ; video ram is now the dest
        mov     es,WORD PTR _screen+2          ; video ram segment

        mov     al,201                         ; disp the upper left corner
        mov     ah,_vid_attrib                 ; the video attrib to use
        IFDEF  nosnow
           stosw                               ; store in video ram
        ELSE
           mov  cx,1
           call stvideo
        ENDIF

        mov     cx,[bp+10]                     ; now do the horizontal line
        dec     cx                             ; its ncols - 2 long
        dec     cx
        mov     [bp+10],cx                     ; ncols - 2 is used again
        mov     al,205                         ; horizontal bar char
        IFDEF   nosnow
           rep stosw
        ELSE
           call stvideo
        ENDIF

        mov     al,187                         ; disp the upper right corner
        IFDEF   nosnow
           stosw                               ; store in video ram
        ELSE
           mov  cx,1
           call stvideo
        ENDIF

        ; now do the blank lines in the body of the box

        mov     bx,[bp+8]                      ; the body is nrows - 2 long
        dec     bx
        dec     bx

        add     si,160                         ; bump row offset
        mov     di,si

rownxt: mov     al,186                         ; do the left vertical bar
        IFDEF   nosnow
           stosw                               ; store in video ram
        ELSE
           mov  cx,1
           call stvideo
        ENDIF

        mov     cx,[bp+10]                     ; now all the blanks
        mov     al,32
        IFDEF   nosnow
           rep stosw
        ELSE
           call stvideo
        ENDIF

        mov     al,186                         ; do the right vertical bar
        IFDEF   nosnow
           stosw                               ; store in video ram
        ELSE
           mov  cx,1
           call stvideo
        ENDIF

        add     si,160                         ; bump row offset
        mov     di,si

        dec     bx                             ; another row done
        jnz     rownxt

        ; now do the bottom row of the box

        mov     al,200                         ; disp the lower left corner
        IFDEF   nosnow
           stosw                               ; store in video ram
        ELSE
           mov  cx,1
           call stvideo
        ENDIF

        mov     cx,[bp+10]                     ; now do the horizontal line
        mov     al,205                         ; horizontal bar char
        IFDEF   nosnow
           rep stosw
        ELSE
           call stvideo
        ENDIF

        mov     al,188                         ; disp the lower right corner
        IFDEF   nosnow
           stosw                               ; store in video ram
        ELSE
           mov  cx,1
           call stvideo
        ENDIF

        pop     si                             ; done, exit
        pop     di
        mov     sp,bp
        pop     bp
        ret
_popup  ENDP


;******************************************************************************
;
;       popdwn(row,col,nrows,ncols,savp)
;
;       Remove a pop up dialog box from the screen by restoring the prior
;       contents from savp.
;
;******************************************************************************

        PUBLIC  _popdwn

_popdwn PROC NEAR
        push    bp
        mov     bp,sp
        push    di
        push    si

        mov     ax,160                         ; calc offset to start of
        imul    WORD PTR [bp+4]                ; dialog box -
        mov     cx,[bp+6]                      ; row * 160 + (col << 1)
        shl     cx,1
        add     ax,cx

        mov     di,ax                          ; di now -> box start offset

        mov     si,[bp+12]                     ; get savp, the save addr ptr

        mov     dx,di                          ; save current row offset

        mov     bh, BYTE PTR [bp+8]            ; nrows to bh
        mov     bl, BYTE PTR [bp+10]           ; ncols to bl

        mov     es,WORD PTR _screen+2          ; oh yea, es points to video ram

next:   mov     cl,bl                          ; move ncols to cx
        xor     ch,ch

        IFDEF  nosnow
           rep movsw                           ; move data
        ELSE
           mov  al,_vid_snow
           call movideo
        ENDIF

        add     dx,160                         ; calc offset of next row
        mov     di,dx

        dec     bh                             ; one more row done, any more?
        jnz     next

        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_popdwn ENDP

        IFNDEF  nosnow

;******************************************************************************
;
;   movideo - move data to/from video memory
;           - current video mode must be passed in al
;
;******************************************************************************

movideo PROC NEAR

        test    al,1                   ; do we need to check for snow?
        jnz     goslow

        rep  movsw                     ; no snow, full speed ahead
        ret                            ; that's all there is to it

goslow: push    dx                     ; it's a cga, go slow time

        mov     dx,03dah               ; dx = cga status port address

wait1:  in      al,dx                  ; get retrace status
        test    al,8                   ; vertical retrace?
        jnz     mov_it                 ; yes, go get the char
        ror     al,1                   ; low bit set? (horizontal retrace)
        jc      wait1                  ; current retrace may be almost done

        cli                            ; no interrupts until char loaded

wait2:  in      al,dx                  ; wait for start of next retrace
        ror     al,1
        jnc     wait2

mov_it: movsw                          ; move word to/from video ram

        sti                            ; allow interrupts

        loop    wait1                  ; all chars transfered?

        pop     dx                     ; restore dx

        ret

movideo ENDP


;******************************************************************************
;
;  stvideo - store the word in ax to video ram cx times
;
;******************************************************************************

stvideo PROC NEAR

        test    _vid_snow,1            ; do we need to slow for no snow?
        jnz     slowgo

        rep  stosw                     ; no snow, full speed ahead
        ret                            ; that's all there is to it

slowgo: push    bx                     ; snow, go slow
        push    dx

        mov     dx,03dah               ; dx = cga status port address
        mov     bx,ax

wait3:  in      al,dx                  ; get retrace status
        test    al,8                   ; vertical retrace?
        jnz     sto_it                 ; yes, go get the char
        ror     al,1                   ; low bit set? (horizontal retrace)
        jc      wait3                  ; current retrace may be almost done

        cli                            ; no interrupts until char loaded

wait4:  in      al,dx                  ; wait for start of next retrace
        ror     al,1
        jnc     wait4

sto_it: mov     ax,bx
        stosw                          ; store word to video ram

        sti                            ; allow interrupts

        loop    wait3                  ; all chars transfered?

        pop     dx                     ; restore dx & bx
        pop     bx

        ret
stvideo ENDP

        ENDIF

_TEXT   ENDS
        END
