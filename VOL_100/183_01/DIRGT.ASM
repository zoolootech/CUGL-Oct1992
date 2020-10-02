page ,132
title **** DIRGT.ASM ***
comment *

=========================================================================
DIRGT.ASM
03-06-85  by  Thomas E. Link

     Procedure to be called from IBM PASCAL V2.0 or function in LATTICE C
to get disk directory including VOLUME NAME, file LENGTH,DATE, and TIME.

To use in PASCAL first:
        type  dirrect = record
                fname:string(12);
                ftime:word;
                fdate:word;
                flen:array[0..1] of word;
                end;
              lookst = string(40);
              volst = string(13);
              countt = word;
        var   dir_rec : array[0..111] of dirrect;

       looks := 'drive id:\path\filename.ext'+CHR(0)
           ' name to search for, same as with DIR command
           ' path works only when used with DOS 2.x
           ' This string must terminate with a CHR$(0) !

   procedure DIRGET(var looks lookst; var dir_rec[0]:dirrect;
                    var vols:volst; var count:countt);extern;
        If Assembling for PASCAL leave the equate (pascal equ 1) below.
        If using LATTICE C delete that line.
=========================================================================
*
pascal equ 1
page
workstruc       struc
        dircount        dw      (?)
        dos_version     db      (?)
        filler          db      (?)
        saved_sp        dw      (?)
        fcb             db      48 dup (?)
        dta             db      48 dup (?)
workstruc       ends

ifdef   pascal  ;-------------------------------------------------------
stack   struc                        ; Define the stack area for PASCAL
        workarea        db      type workstruc dup (?)
        pushed_bp       dw      (?)
        return          dw      (?)
        address         dw      (?)
        countnumad      dw      (?)
        volstrgad       dw      (?)
        dirrecarad      dw      (?)
        lookstringad    dw      (?)
stack   ends
        poplen  equ     lookstringad - address

else    ;----------------- 

stack   struc                      ; Define the stack area for C.
        workarea        db      type workstruc dup (?)
        pushed_bp       dw      (?)
        address         dw      (?)
        countnumad      dw      (?)
        volstrgad       dw      (?)
        dirrecarad      dw      (?)
        lookstringad    dw      (?)
stack   ends
        poplen  equ     0

endif   ;---------------------------------------------------------------

data    segment public  'DATA'
data    ends
dgroup  group   data

ifdef   pascal ;---------- Pascal --------------------------------------
prog    segment public 'CODE'
        assume CS:prog, DS:nothing, ES:nothing
        public  DIRGET
DIRGET  proc    far

else    ;----------------- C definitions
pgroup  group   prog
        assume  CS:pgroup, DS:nothing, ES:nothing
prog    segment public  'PROG'
        public  DIRGET,_DIRGET,DIRGET_
DIRGET  proc    near

endif   ;----------------------------------------------------------------

page
DIRGET_:
_DIRGET:
        cld                          ; Clear direction flag.
        push    bp
        mov     bp,sp                ; Point to arguments on stack.
        sub     bp,type workstruc    ; Give us room to work
        mov     [bp].saved_sp,sp     ; Save the stack pointer
        mov     sp,bp                ; and move it below workarea.
        mov     [bp].dircount,0      ; Move zero to directory count.

;----------------- determine DOS version, store in variable
        mov     ah,48                ; find DOS version number
        int     21h
        mov     [bp].dos_version,al  ; and store in variable

;-----------------  parse filename
        mov     ax,ss
        mov     es,ax           ; Have ES point at STACK **************
        mov     di,bp           ; and DI point 
        add     di,offset fcb   ;            to FCB area.
        mov     si,[bp].lookstringad    ; Point SI at file specifier
        mov     ah,29H          ; DOS function to parse filename
        mov     al,1            ; scan off leading separators
        int     21H             ; DOS function call
        push    ds              ; Get DATA seg into ES.***************
        pop     es
        cmp     al,0FFH         ; Any errors on parsing ?
        jne     dir2            ; No - continue
        jmp     done            ; YES - end.

;----------------- get addresses of the parms
dir2:
        mov     dx,bp           ; Put address of workspace into DX
        add     dx,offset dta   ; Bump to DTA
        push    ds              ; Save value of DS
        mov     ax,ss           ; and move STACK into DS.**************
        mov     ds,ax
        mov     ah,1AH          ; Set AH for *SET DTA* function call.
        int     21H             ; Use DOS interrupt.

        cmp     [bp].dos_version,2 ; if DOS 2.0 or greater
        jae     dos2_search        ; use special routine

;----------------- DOS 1.0, 1.1 search
        mov     dx,bp           ; Point DX at workspace
        add     dx,offset fcb   ;       bump to FCB area.
        mov     ah,11H          ; Set AH for directory search.
        int     21H             ; Use DOS interrupt.
        cmp     al,0FFH         ; Any matches found ?
        je      done1           ; No - so end
        inc     [bp].dircount   ; Yes - count first entry.
        call    move            ; Go move entry into fnames().
dirloop:
        mov     ah,12H          ; Set AH for continuation of search.
        int     21H             ; Use DOS interrupt.
        cmp     al,0FFH         ; Any matches found ?
        je      done1           ; No - jump to exit
        inc     [bp].dircount   ; Yes - count this entry.
        call    move            ; Go move entry into arrays.
        jmp     dirloop         ; Do again.

;----------------- DOS 2.x  search
dos2_search:
        mov     dx,[bp].lookstringad    ; Point DX at file specifier
        mov     ah,4EH          ; Set AH for directory search.
        mov     cx,10H          ; search for files and directories.
        int     21H             ; Use DOS interrupt.
        jc      findlabel       ; No matches - so look for a label
        inc     [bp].dircount   ; found - count first entry.
        call    move2           ; Go move entry into arrays.
dir2loop:
        mov     cx,10H
        mov     ah,4FH          ; Set AH for continuation of search.
        int     21H             ; Use DOS interrupt.
        jc      findlabel       ; No matches - so look for a label
        inc     [bp].dircount   ; match - count this entry.
        call    move2           ; Go move entry into arrays.
        jmp     dir2loop        ; Do again.
findlabel:
        call    get_label       ; look for a volume label
done1:
        pop     ds              ; restore DS to DATA  ****************
done:
        mov     ax,[bp].dircount      ; Move entries count to AX.
        mov     di,[bp].countnumad    ; Get address of COUNT%
        mov     [di],ax         ; Put entry count in COUNT%.
        mov     sp,[bp].saved_sp      ; Restore SP.
        pop     bp              ; Restore BP.
        ret     poplen          ; Return and throw away parms if PASCAL

;........................................................................
;----------------- procedure to store results from DOS 1.x search
move    proc    near
;----------------- first move filename into string array
        mov     di,[bp].dirrecarad   ; Move name address to DI and BX.
        mov     bx,di
        add     bx,20           ; Have BX point at next record
        mov     [bp].dirrecarad,bx   ; and save in variable
        mov     si,bp
        add     si,offset dta+1 ; put file name(in DTA) address into SI
        mov     cx,4            ; Length of name in words
        rep     movsw           ; Move name into array.
        mov     byte ptr [di],'.' ; Put period in filename.
        inc     di              ; Bump DI past '.'
        mov     cx,3            ; Length of extension in bytes
        rep     movsb           ; Move extension into array.
;-----------------  get the file time into array
        add     si,11           ; move to file time area
                movsw           ; Move time into array.
;-----------------  get the file date into array
                movsw           ; Move size into array.
;-----------------  get the file length into array
        add     si,2            ; Point SI at file size.
                movsw           ; Move size into array.
                movsw
        ret
move    endp

;........................................................................
;----------------- procedure to store results of DOS 2.x search
move2   proc    near
;----------------- first get the file time into array
        mov     si,bp
        add     si,offset dta +22 ; Point AX at DTA + 22
        mov     bx,[bp].dirrecarad    ; Move record address to BX and DI.
        mov     di,bx
        add     di,12           ; Bump DI to time.
                movsw           ; Move time into array.
;----------------- now get the file date into array
                movsw           ; Move size into array.
;----------------- get the file length into array
                movsw           ; Move size into array.
                movsw
;-----------------  move filename into string array
        mov     di,bx           ; Point DI at name.
        add     bx,20           ; Bump BX to next record.
        mov     [bp].dirrecarad,bx ; Save it in variable space.
        mov     cx,8            ; Length of name in bytes
move201:
                lodsb           ; Move character into al
        cmp     al,'.'          ; If is it a period ... jump.
        je      move202
        cmp     al,0            ; If zero - dont jump.
        jne     move201A
        add     cx,4
        mov     al,' '          ; Fill with spaces
        rep     stosb           ; till 12 characters filled.
        jmp     move203         ; Skip to subdir check.
move201A:
                stosb           ; A character - store it in output string
        loop    move201
        inc     si              ; We found 8 characters - bump SI past .
        jmp     move2020
move202:
        mov     al,' '          ; Pad short name with spaces.
        rep     stosb
move2020:
        inc     di
        mov     cx,3            ; Length of extension in bytes
move202A:
                lodsb           ; Move character into AL.
        cmp     al,0            ; Is it a zero ?
        je      move202B        ; Yes .. jump.
                stosb           ; No .. Store it and loop
        loop    move202A
        jmp     move203
move202B:
        mov     al,' '
        rep     stosb
move203:
        mov     byte ptr[di]-4,'.'      ; Store a period.
        cmp     byte ptr [bp].dta+21,10H        ; is it a subdirectory ?
        jne     move23          ; no - jump
        std                     ; change direction to decrement SI and DI
        dec     di              ; Back up to last character of extension
        mov     cx,3            ; Store `...' as the extension name when
        mov     al,'.'          ; a subdirectory entry.
        rep     stosb
        cld                     ; clear direction flag (increment)
move23:
        ret
move2   endp

;........................................................................

get_label proc near             ;Display the label for the
                                ;disk in the selected drive 
        push    es              ; Save ES
        push    ss              ; Get STACK into ES **************
        pop     es
;----------------- first change the FCB to an extended FCB
        mov     di,bp 
        add     di,offset fcb   ; Move FCB address to DI
        mov     dx,di           ; And also put into DX.
        mov     al,byte ptr[di] ; Get drive id
        mov     byte ptr [di+7],al    ; move it to offset 7
        mov     byte ptr [di],0FFH    ; indicate extended FCB
        inc     di
        mov     cx,5            ; Store 5 zeros
        xor     al,al
        rep     stosb
        mov     byte ptr [di],08H   ; Store volume attribute
        inc     di
        inc     di              ; bump past drive id
        mov     cx,11
        mov     al,'?'          ; Store 11 `?' in filename
        rep     stosb
        mov     cx,25           ; End with 25 zeros
        xor     al,al
        rep     stosb
        pop     es              ; restore ES
;----------------- now search for volume
        mov     ah,11H          ; using DOS function 11H
        int     21h
        cmp     al,0FFH         ;any volume label found?
        je      get_label2      ;no,jump
;----------------- store volume name in variable
        mov     di,[bp].volstrgad    ; Point DI at VOLUMES
        mov     si,bp
        add     si,offset dta +8; Point AX at File name in DTA
        mov     cx,11           ; Length of name in bytes
        rep     movsb           ; Move name into string
get_label2:
        ret
get_label endp

;........................................................................

dirget          endp
prog            ends
                end
