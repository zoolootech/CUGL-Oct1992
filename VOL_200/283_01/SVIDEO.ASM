
; SVIDEO.ASM - after Microsoft Systems Journal, Nov. 1988, p. 6
;
;               *** Caution: Small Model stack bias! ***
;
; This program handles direct writes to the IBM PC video screen buffer
; located in RAM memory.  Works with all 100% compatible clones, and
; correctly eliminates snow when using CGA.  See the article in MSJ:
; "Building a device-independent video display I/O library in Microsoft C",
; by Jeff Prosise.  Requires MASM v5.0 or MASM v5.1, per documentation.
;
; Memory model support by David C. Oshel, Ames, Iowa, Dec. 9, 1988.
; This file is essentially ASMVIDEO.ASM, modified for appropriate function
; calls and returns, and DS register management for various data objects.
;
; I've added four additional functions, all of which take a len argument,
; to Prosise's original set of three:
;                               
;                  MSJ_SetFldAttr, MSJ_DispMsgLen    12/10/88
;                  MSJ_MovScrBuf,  MSJ_MovBufScr     12/11/88
;
; Added Wozniak's pattern fill to MSJ_SetFldAttr on 12/12/88, and figured
; out why I still had snow on a CGA (NCR PC 6300) on 12/13/88.  See next.
;
;                         *** C A U T I O N ***
;
; It turns out that Prosise's XCHG AX,BX and STOSW pattern in the snow
; elimination code is a VERY time critical optimization.  I actually had
; to read the article before I could expand on the original routines.  Ouch.
;       --- 12/13/88, d.c.oshel
;

        .MODEL SMALL
        .CODE
        PUBLIC _MSJ_MovBufScr, _MSJ_MovScrBuf
        PUBLIC _MSJ_SetFldAttr, _MSJ_DispMsgLen
        PUBLIC _MSJ_DispString, _MSJ_DispCharAttr, _MSJ_GetCharAttr


;
; The C interface for these functions and structures looks like this:
;
;struct MSJ_VideoInfo {
;	unsigned char mode;
;	unsigned char rows;
;	unsigned char columns;
;	unsigned char ColorFlag;
;	unsigned char SnowFlag;
;	unsigned int  BufferStart;
;	unsigned int  SegAddr;
;};
;
;extern struct MSJ_VideoInfo video;  /* defined in CVIDEO.C */
;
;
;/* MSJ, Nov. 1988
;   */
;extern int cdecl MSJ_GetCharAttr( char row, char col, 
;                                  struct MSJ_VideoInfo * sptr );
;
;/* MSJ, Nov. 1988
;   */
;extern void cdecl MSJ_DispCharAttr( char ch, 
;                                    char row, char col, 
;                                    char VideoAttr, 
;                                    struct MSJ_VideoInfo * sptr );
;
;
;/* MSJ, Nov. 1988
;   */
;extern void cdecl MSJ_DispString( char * msg, 
;                                  char row, char col, 
;                                  char VideoAttr, 
;                                  struct MSJ_VideoInfo * sptr ); 
;
;
;/* MSJ_DispMsgLen is like MSJ_DispString, but takes a length argument,
;   and does not disturb screen attributes in the receiving field
;   12/10/88, d.c.oshel
;   */
;extern void cdecl MSJ_DispMsgLen( char * msg, 
;                                  char row, char col, 
;                                  int len,
;                                  struct MSJ_VideoInfo * sptr ); 
;
;
;/* MSJ_SetFldAttr takes a length argument and clears a field of the
;   screen to the given char having the given attribute
;   12/10/88, d.c.oshel
;   */
;extern void cdecl MSJ_SetFldAttr( char ch, 
;                                  char row, 
;                                  char col, 
;                                  char VideoAttr,
;                                  int len,
;                                  struct MSJ_VideoInfo * sptr );
;
;
;/* move count WORDS from screen to far buffer, 12/10/88, d.c.oshel 
;   */
;extern void cdecl MSJ_MovScrBuf(  char far * buffer,
;                                  char row,
;                                  char col,
;                                  int count,
;                                  struct MSJ_VideoInfo * sptr );
;
;
;/* move count WORDS from far buffer to screen, 12/10/88, d.c.oshel 
;   */
;extern void cdecl MSJ_MovBufScr(  char far * buffer,
;                                  char row,
;                                  char col,
;                                  int count,
;                                  struct MSJ_VideoInfo * sptr );
;


;-----------------------------------------------------------------------
; _MSJ_MovBufScr   copies count words from the user's far save buffer to 
;                  the active video buffer
;                  12/10/88, d.c.oshel 
;
; _MSJ_MovScrBuf   copies count words from the active video buffer to 
;                  the user's far save buffer
;                  12/10/88, d.c.oshel 
;-----------------------------------------------------------------------
;
; Arguments on the stack (SMALL MODEL):
;
;                       [bp+14]  <--- struct MSJ_VideoInfo * sptr
;                       [bp+12]  <--- int count
;                       [bp+10]  <--- char col
;                       [bp+8]   <--- char row
;                       [bp+6]   <--- seg\ char far * buffer
;                       [bp+4]   <--- ofs/ 
;
_MSJ_MovBufScr  PROC
                push    bp      ;save regs & establish stack frame
                mov     bp,sp
                push    si
                push    di
                push    es
                push    ds      ;large model uses data segments anywhere

; COUNT:        load CX with the number of bytes to move
;
                mov     cx,[bp+12]      ;CX <-- count
                cmp     cx,1            ;we're already done if CX <= 0
                jl      exitmbs

; DESTINATION:  set ES:DI to point to screen buffer absolute address
;               note - this changes DS to video structure's segment
;
                mov     bx,[bp+14]          ;DS:BX <-- ptr to video structure
                test    byte ptr [bx+4], 1  ;is video.SnowFlag clear?
                pushf                       ;save answer for later

                mov     dh,[bp+8]   ;DH <-- row
                mov     dl,[bp+10]  ;DL <-- col
                call    AddressOf   ;DI <-- screen offset
                mov     es,[bx+8]   ;ES <-- video.SegAddr (screen segment)
                                         
; SOURCE:       set DS:SI to point to user's save buffer
;               note - this changes DS to user's far buffer segment
;
                lds     si,[bp+4]       ;DS:SI <-- ptr to source
                cld
                popf
                jnz     hardway

; BEST CASE, move the string from DS:SI to ES:DI, with immediate writes
;
                rep     movsw           ;move all immediately
                jmp     exitmbs

hardway:        lodsw                   ;get char from buffer
                xchg    ax,bx

                cli                     ;disable interrupts
                mov     dx,3DAh         ;DX <- ptr to Status Register
st1:            in      al,dx           ;if we've arrived during a
                test    al,1            ;horizontal blanking interval,
                jnz     st1             ;must wait for the start of next!
st2:            in      al,dx           ;bit one is horizontal retrace
                test    al,1            ;bit three is vertical retrace
                jz      st2             ;if nz, horizontal blanking is NOW.

                ; Move up to 16 bits ONLY to the screen, taking advantage
                ; of the 8088's 4-byte prefetch buffer to save time (!),
                ; during the 7 microseconds of horizontal blanking;
                ; Prosise says timing is so tight we MUST use the XCHG
                ; instruction to get AX back, and nothing else will do.
                ;
                ; There are 200 horizontal retrace intervals per frame,
                ; only one vertical retrace interval per frame; i.e., 200
                ; lines on the screen, horizontal retrace is the zag
                ; following raster scan's zig, vertical retrace is the
                ; corner-to-corner diagonal path back to top of screen.
                ;
                ; For reference, 65 clock cycles are about 14 æsec. on
                ; a 4.77 mHz 8088.  See the article for details.
                ;
                ; In general, "snow" is caused by the CGA trying to access
                ; video RAM at the same time as the CPU (us!) is trying
                ; to change the screen.  CGA is ALWAYS accessing video
                ; RAM, except during horizontal or vertical retrace, or
                ; unless the port is turned off.  Prosise's discussion,
                ; en precis.  NOTE:  Not all hardware that uses CGA modes
                ; is actually a CGA!  The snow elimination algorithm is
                ; so slow (relatively speaking), it should be turned off
                ; unless actually needed to eliminate snow.  For example,
                ; my ATI Graphics Solution card emulates CGA but does not
                ; require snow elimination.
                ;
                ; XCHG AX,BX, STOSW is 4+11 clocks (ca. 3.23 microseconds),
                ; while MOVSW is 18 clocks (ca. 3.88 microseconds), according
                ; to the Intel Programmer's Pocket Reference Guide.
                ;
                xchg    ax,bx
                stosw                   ;from buffer to screen

                sti                     ;enable interrupts
                dec     cx              ;decrement byte counter
                jcxz    exitmbs         ;all done, CX has gone to 0
                jmp     hardway         ;loop back for more

exitmbs:        pop     ds      ;restore original DS, etc.
                pop     es
                pop     di
                pop     si
                pop     bp
                ret
_MSJ_MovBufScr  ENDP


_MSJ_MovScrBuf  PROC
                push    bp      ;save regs & establish stack frame
                mov     bp,sp
                push    si
                push    di
                push    es
                push    ds      ;large model uses data segments anywhere

; COUNT:        load CX with the number of bytes to move
;
                mov     cx,[bp+12]      ;CX <-- count
                cmp     cx,1            ;we're already done if CX <= 0
                jl      exitmsb 

; SOURCE:       set DS:SI to point to screen buffer absolute address
;               note - this changes DS to video structure's segment
;
                mov     bx,[bp+14]          ;DS:BX <-- ptr to video structure
                test    byte ptr [bx+4], 1  ;is video.SnowFlag clear?
                pushf                       ;save answer for later

                mov     dh,[bp+8]   ;DH <-- row
                mov     dl,[bp+10]  ;DL <-- col
                call    AddressOf   ;DI <-- screen offset
                mov     es,[bx+8]   ;ES <-- video.SegAddr (screen segment)
                push    es          ;save ES:DI, screen address
                push    di          ;will pop as DS:SI just below!
                                         
; DESTINATION:  set ES:DI to point to user's save buffer
;               note - this changes ES to user's far buffer segment
;
                les     di,[bp+4]       ;ES:DI <-- ptr to destination

;               note - this changes DS to the active video buffer
                pop     si
                pop     ds              ;DS:SI <-- ptr to source

                cld                     ;clear direction flag
                popf                    ;was video.SnowFlag clear?
                jnz     hw              ;no, do it the hard way

; BEST CASE, move the string from DS:SI to ES:DI, with immediate writes
;
                rep     movsw           ;all at once!
                jmp     exitmsb

; move the string from DS:SI to ES:DI, with retrace delay
;
; we're going from SCREEN to BUFFER, so must wait for blanking interval
; before READING the char/attribute

hw:             cli                     ;disable interrupts
                mov     dx,3DAh         ;DX <- ptr to Status Register
st3:            in      al,dx           ;if we've arrived during a
                test    al,1            ;horizontal blanking interval,
                jnz     st3             ;must wait for the start of next!
st4:            in      al,dx           ;bit one is horizontal retrace
                test    al,1            ;bit three is vertical retrace
                jz      st4             ;if nz, horizontal blanking is NOW.

                ; TIME CRITICAL, only have 7 æsec to read 16 bits into AX
                ;
                lodsw

                ; But going from AX to buffer can take as long as it takes
                ;
                stosw

                sti                     ;enable interrupts
                dec     cx              ;decrement byte counter
                jcxz    exitmsb         ;all done, CX has gone to 0
                jmp     hw              ;loop back for more

exitmsb:        pop     ds      ;restore original DS, etc.
                pop     es
                pop     di
                pop     si
                pop     bp
                ret
_MSJ_MovScrBuf  ENDP


;------------------------------------------------------------------------
; _MSJ_SetFldAttr  writes count copies of char/attribute to video buffer
;                  12/12/88, d.c.oshel 
;------------------------------------------------------------------------
;
; Arguments on the stack (SMALL MODEL):
;
;                       [bp+14]  <--- struct MSJ_VideoInfo * sptr
;                       [bp+12]  <--- int count
;                       [bp+10]  <--- char VideoAttr
;                       [bp+8]   <--- char col
;                       [bp+6]   <--- char row
;                       [bp+4]   <--- char ch
;
_MSJ_SetFldAttr PROC
                push    bp      ;save regs & establish stack frame
                mov     bp,sp
                push    si
                push    di
                push    es
                push    ds      ;large model uses data segments anywhere

                mov     cx,[bp+12]      ;CX <-- count
                cmp     cx,1            ;we're already done if CX <= 0
                jl      exitsfa 

; DESTINATION:  set ES:DI to point to screen buffer absolute address
;               note - this changes DS to video structure's segment
;
                mov     bx,[bp+14]      ;DS:BX <-- ptr to video structure

                mov     dh,[bp+6]       ;DH <-- row
                mov     dl,[bp+8]       ;DL <-- col
                call    AddressOf       ;DI <-- screen offset
                mov     es,[bx+8]       ;ES <-- video.SegAddr (screen segment)

; SOURCE:  load attribute and character
;
                mov     ah,[bp+10]      ;AH <-- attr
                mov     al,[bp+4]       ;AL <-- ch

; move the FIRST word from AX to DESTINATION, with possible retrace delay
;
                cld                         ;clear direction flag
                test    byte ptr [bx+4], 1  ;is video.SnowFlag clear?
                jz      quick               ;yes

                ;Rats.  Got CGA, so do it slow.  Just repeats.
                ;
                ;Although slow, this is still faster than the
                ;hardway pattern fill, which would have to wait for a
                ;horizontal blanking interval for BOTH reads and writes,
                ;since both source and destination are up in video RAM.
                ;
slow:           xchg    ax,bx   ;save char/attr
                mov     dx,3DAh ;DX <- ptr to Status Register
                cli             ;disable interrupts
at1:            in      al,dx   ;wait for non-retrace period
                test    al,1
                jnz     at1 
at2:            in      al,dx   ;wait for next horz retrace
                test    al,1
                jz      at2     ;if ready, fall into...

                ; TIME CRITICAL, must accomplish move in 7 æsec or less
                ;
                xchg    ax,bx
                stosw           ;di advances to next word

                sti             ;enable interrupts
                dec     cx      ;decrement byte counter
                jcxz    exitsfa ;all done, if CX went to 0
                jmp     slow    ;loop back until done

; Pattern Fill:  ES:DI is destination of next place to be filled
;                DS:SI is original ES:DI, one word back from current ES:DI
;                DS:[SI] now contains the pattern!
;                CX contains the count of words remaining in the pattern
;
; Example with count == 5; this is one of Steve Wozniak's old tricks:
;              s  d
;               \ |
;                .....    <- fill range of 5 places with Z's
;                Z...     <- source, after loading Z (move 4 one place right)
;                 ....    <- destination at start of move
;                 Z...    <- destination after one move is obvious
;                ZZ..     <- source after one move is subtle!
;                ZZZZZ    <- final result
;
; This works because the source modifies its own next byte by moving its
; first byte to an overlapping destination.  It's efficient because in the
; best case, we can move the entire range-1 using REP MOVSW.
;
quick:          push    es      ;save starting destination, ES:DI
                push    di
                stosw           ;load first word into pattern, DI advances
                pop     si      ;pop starting destination as source, DS:SI!
                pop     ds
                dec     cx      ;decrement byte counter
                jcxz    exitsfa ;all done, if CX went to 0

                rep     movsw   ;magic pattern fill ... !

exitsfa:        pop     ds      ;restore original DS, etc.
                pop     es
                pop     di
                pop     si
                pop     bp
                ret
_MSJ_SetFldAttr ENDP


;-----------------------------------------------------------------------
; _MSJ_DispMsgLen  writes len bytes from msg to the active video buffer
;                  without disturbing the receiving attribute field
;                  12/9/88, d.c.oshel 
;-----------------------------------------------------------------------
;
; Arguments on the stack (SMALL MODEL):
;
;                       [bp+12]  <--- struct MSJ_VideoInfo * sptr
;                       [bp+10]  <--- int len
;                       [bp+8]   <--- char col
;                       [bp+6]   <--- char row
;                       [bp+4]   <--- char * msg
;
_MSJ_DispMsgLen PROC
                push    bp      ;save regs & establish stack frame
                mov     bp,sp
                push    si
                push    di
                push    es
                push    ds      ;large model uses data segments anywhere

                mov     cx,[bp+10]      ;CX <-- length of string
                cmp     cx,1            ;we're already done if CX <= 0
                jl      dexit

; DESTINATION:  set ES:DI to point to screen buffer absolute address
;               note - this changes DS to video structure's segment
;
                mov     bx,[bp+12]      ;DS:BX <-- ptr to video structure
                test    byte ptr [bx+4], 1  ;is video.SnowFlag clear?
                pushf                       ;save result, will jz later
                mov     dh,[bp+6]       ;DH <-- row
                mov     dl,[bp+8]       ;DL <-- col
                call    AddressOf       ;DI <-- screen offset
                mov     es,[bx+8]       ;ES <-- video.SegAddr (screen segment)

; SOURCE:  set DS:SI to point to null-terminated ASCII string, load attribute
;          note - this changes DS to msg's segment, old value not needed
;
                mov     si,[bp+4]       ;DS:SI <-- ptr to msg
                cld                     ;clear direction flag
                popf                    ;was video.SnowFlag set?
                jnz     dmsg1           ;yes, do it the hard way


; move the string from SOURCE to DESTINATION, with immediate writes
;
dmsg2:          movsb                   ;move character from string to screen
                inc     di              ;  and skip over attribute on screen
                dec     cx              ;decrement byte counter
                jcxz    dexit           ;all done, CX has gone to 0
                jmp     dmsg2           ;loop back for more

; move the string from SOURCE to DESTINATION, with retrace delay
;
dmsg1:          lodsb                   ;move character from string to AL
                xchg    ax,bx
                cli
                mov     dx,3DAh         ;DX <- ptr to Status Register
wt1:            in      al,dx           ;wait for non-retrace period
                test    al,1
                jnz     wt1 
wt2:            in      al,dx           ;wait for next horz retrace
                test    al,1
                jz      wt2             ;if ready, fall into...

                ; TIME CRITICAL, must accomplish move in 7 æsec or less!
                ;
                xchg    ax,bx
                stosb                   ;move character from AL to screen
                inc     di              ;  and skip over attribute on screen
                sti                     ;enable interrupts

                dec     cx              ;decrement byte counter
                jcxz    dexit           ;all done, CX has gone to 0
                jmp     dmsg1           ;loop back for more

dexit:          pop     ds      ;restore original DS, etc.
                pop     es
                pop     di
                pop     si
                pop     bp
                ret

_MSJ_DispMsgLen ENDP


;------------------------------------------------------------
; _MSJ_DispString writes a string to the active video buffer
;------------------------------------------------------------
;
; Arguments on the stack (SMALL MODEL):
;
;                       [bp+12]  <--- struct MSJ_VideoInfo * sptr
;                       [bp+10]  <--- char VideoAttr
;                       [bp+8]   <--- char col
;                       [bp+6]   <--- char row
;                       [bp+4]   <--- char * msg
;
_MSJ_DispString PROC
                push    bp      ;save regs & establish stack frame
                mov     bp,sp
                push    si
                push    di
                push    ds
                push    es

                mov     bx,[bp+12]      ;DS:BX <-- ptr to video structure

                mov     si,[bp+4]       ;DS:SI <-- ptr to msg
                mov     dh,[bp+6]       ;DH <-- row
                mov     dl,[bp+8]       ;DL <-- col
                call    AddressOf       ;DI <-- screen offset
                mov     es,[bx+8]       ;ES <-- video.SegAddr (screen segment)
                mov     ah,[bp+10]      ;AH <-- VideoAttr
                cld

dstr1:          lodsb
                or      al,al
                jz      dstr3
                test    byte ptr [bx+4], 1  ;is video.SnowFlag clear?
                jz      dstr2
                call    WriteWithWait
                jmp     dstr1
dstr2:          stosw
                jmp     dstr1           ;found '\0', ASCIIZ string terminator

dstr3:          pop     es
                pop     ds
                pop     di
                pop     si
                pop     bp
                ret
_MSJ_DispString     ENDP




;-----------------------------------------------------------------------
; _MSJ_DispCharAttr     writes a character and attribute to the active
;                       video buffer
;-----------------------------------------------------------------------
;
; Arguments on the stack (SMALL MODEL):
;
;                       [bp+12]  <--- struct MSJ_VideoInfo * sptr
;                       [bp+10]  <--- char VideoAttr
;                       [bp+8]   <--- char col
;                       [bp+6]   <--- char row
;                       [bp+4]   <--- char ch
;
_MSJ_DispCharAttr PROC
                push    bp              ;save regs & establish stack frame
                mov     bp,sp
                push    di
                push    si
                push    ds
                push    es
                mov     bx,[bp+12]      ;DS:BX <-- ptr to video structure

                mov     dh,[bp+6]       ;DH <-- row
                mov     dl,[bp+8]       ;DL <-- col
                call    AddressOf       ;DI <-- screen offset
                mov     es,[bx+8]       ;ES <-- video.SegAddr (screen segment)
                mov     al,[bp+4]          ;AL <-- ch
                mov     ah,[bp+10]         ;AH <-- VideoAttr

                test    byte ptr [bx+4], 1 ;is DS:video.SnowFlag clear?
                jz      dca1               ;yes, write direct
                call    WriteWithWait
                jmp     short dca2
dca1:           stosw                   ;ES:[DI] <-- AX  a screen write

dca2:           pop     es
                pop     ds
                pop     si
                pop     di
                pop     bp
                ret
_MSJ_DispCharAttr   ENDP



;---------------------------------------------------------------
; _MSJ_GetCharAttr  returns the character and attribute at the
;                   specified row & column; i.e.,
;                   On Exit: AH <-- attribute, AL <-- character
;---------------------------------------------------------------
;
; Arguments on the stack (SMALL MODEL):
;
;                       [bp+8]   <--- struct MSJ_VideoInfo * sptr
;                       [bp+6]   <--- char col
;                       [bp+4]   <--- char row
;
_MSJ_GetCharAttr PROC
                push    bp              ;save regs & establish stack frame
                mov     bp,sp
                push    di
                push    si
                push    ds
                push    es
                mov     bx,[bp+8]       ;DS:BX <-- ptr to video structure
                test    byte ptr [bx+4],1  ;is video.SnowFlag clear?
                pushf

                mov     dh,[bp+4]       ;DH <-- row
                mov     dl,[bp+6]       ;DL <-- col
                call    AddressOf       ;DI <-- screen offset
                mov     es,[bx+8]       ;ES <-- video.SegAddr (screen segment)
                push    es
                pop     ds
                push    di
                pop     si
                cld
                popf
                jz      gca3

                mov     dx,3DAh   ;DX <- PORT ptr to CRTC status register
                cli
gca1:           in      al,dx     ;wait for non-retrace period
                test    al,1
                jnz     gca1
gca2:           in      al,dx     ;wait for next horizontal retrace
                test    al,1
                jz      gca2      ;if ready, fall into...

gca3:           lodsw             ;AH <- attr, AL <- char
                sti
                pop     es
                pop     ds
                pop     si
                pop     di
                pop     bp
                ret
_MSJ_GetCharAttr    ENDP


;-----------------------------------------------------------------------------
; AddressOf     returns the video buffer address that
;               corresponds to the specified row & column
;
;       Entry:  DS:BX  -  address of VideoInfo structure
;               DH,DL  -  row, column
;       Exit:   DI     -  offset address
;-----------------------------------------------------------------------------
AddressOf       PROC NEAR
                mov     al,[bx+2]       ;columns per row
                mul     dh              ;multiply by row number
                xor     dh,dh           ;add column number
                add     ax,dx
                shl     ax,1            ;mul by 2
                add     ax,[bx+6]       ;add offset of active page
                mov     di,ax           ;DI <- result
                ret
AddressOf       ENDP


;----------------------------------------------------------------
; WriteWithWait writes a character and attribute to a CGA during
;               the horizontal blanking interval; this is the
;               Microsoft-approved *** SNOWBUSTER *** routine!
;
;       Entry:  ES:DI  -  video buffer address
;               AH,AL  -  character, attribute
;       Exit:   CX and DX are trashed
;----------------------------------------------------------------
WriteWithWait  PROC NEAR
               mov     bx,ax           ;save char/attr
               mov     dx,3DAh         ;DX <- ptr to Status Register
               cli                     ;disable interrupts
www1:          in      al,dx           ;wait for non-retrace period
               test    al,1
               jnz     www1
www2:          in      al,dx           ;wait for next horz retrace
               test    al,1
               jz      www2

               ; TIME CRITICAL, must accomplish move in 7 æsec or less!
               ; MOV AX,CX will *NOT* do the job!  Must be XCHG.
               ;
               xchg    ax,bx           ;retrieve char/attr
               stosw                   ;write to video memory at ES:[DI]
               sti                     ;enable interrupts
               ret
WriteWithWait  ENDP
 
                END
 

