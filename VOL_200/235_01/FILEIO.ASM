        PAGE    60,132
        TITLE   Routines to do low level file i/o

; 005   14-Jan-87 fileio.asm

;       Copyright (c) 1987 by Blue Sky Software.  All rights reserved.

_TEXT   SEGMENT  BYTE PUBLIC 'CODE'
_TEXT   ENDS
_DATA   SEGMENT  WORD PUBLIC 'DATA'
_DATA   ENDS
CONST   SEGMENT  WORD PUBLIC 'CONST'
CONST   ENDS
_BSS    SEGMENT  WORD PUBLIC 'BSS'
_BSS    ENDS
DGROUP  GROUP   CONST,  _BSS,   _DATA
        ASSUME  CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP

_DATA   SEGMENT
        EXTRN  _errno:WORD
        EXTRN  __doserrno:WORD
_DATA   ENDS

_TEXT   SEGMENT

;*****************************************************************************
;
;  l_seek(fh,offset)
;  int fh;             file handle
;  long offset;        offset in file to seek to (from start of file)
;
;  Seek to a specific position in file.
;
;*****************************************************************************

        PUBLIC  _l_seek

_l_seek PROC    NEAR
        push    bp
        mov     bp,sp

;       Basically lseek(fh,offset,SEEK_SET)

        mov     ax,4200h               ;seek from start of file
        mov     bx,[bp+4]              ;file handle
        mov     cx,[bp+8]              ;offset (most significant)
        mov     dx,[bp+6]              ;       (least significant)
        int     21h

        jnc     seekex                 ;jmp if no error

        call    fakerror               ;fake errno/_doserrno values
        mov     ax,-1                  ;error, tell couldnt seek by
        mov     dx,ax                  ;       returning -1L

seekex: mov     sp,bp
        pop     bp
        ret

_l_seek ENDP


;*****************************************************************************
;
;  readbuf(fh,bp,bl)
;  int fh;                     /* file handle */
;  char far *bp;               /* memory loc to read to */
;  unsigned int bl;            /* amount to read */
;
;  Read a buffer from the current location in file.
;
;*****************************************************************************

        PUBLIC  _readbuf

_readbuf PROC NEAR
        push    bp
        mov     bp,sp

        push    ds                     ;save current data seg

        mov     ah,3fh                 ;read file
        mov     bx,[bp+4]              ;file handle
        mov     cx,[bp+10]             ;bl (buffer length)
        lds     dx,DWORD PTR [bp+6]    ;bp (buffer pointer)
        int     21h

        pop     ds                     ;restore ds

        jnc     readex                 ;jmp if no error

        call    fakerror               ;fake errno/_doserrno values
        xor     ax,ax                  ;error, tell caller 0 bytes read

readex: mov     sp,bp
        pop     bp
        ret

_readbuf ENDP


;*****************************************************************************
;
;  writebuf(fh,bp,bl)
;  int fh;                     /* file handle */
;  char far *bp;               /* memory loc to read to */
;  unsigned int bl;            /* amount to read */
;
;  Write a buffer to the current location in file.
;
;*****************************************************************************

        PUBLIC  _writebuf

_writebuf PROC NEAR
        push    bp
        mov     bp,sp

        push    ds                     ;save current data seg

        mov     ah,40h                 ;write file
        mov     bx,[bp+4]              ;file handle
        mov     cx,[bp+10]             ;bl (buffer length)
        lds     dx,DWORD PTR [bp+6]    ;bp (buffer pointer)
        int     21h

        pop     ds                     ;restore ds

        jnc     writex                 ;jmp if no error

        call    fakerror               ;fake errno/_doserrno values
        mov     ax,-1                  ;tell caller error happened

writex: mov     sp,bp
        pop     bp
        ret

_writebuf ENDP


;*****************************************************************************
;
;  setftime(fh,date,time)
;  int fh;                     /* file handle */
;  unsigned int date;          /* date to set on file */
;  unsigned int time;          /* time to set on file */
;
;  Set the creation/revision date/time on a file.
;
;*****************************************************************************

        PUBLIC  _setftime

_setftime PROC NEAR
        push    bp
        mov     bp,sp

        mov     ax,5701h               ;set file date/time function
        mov     bx,[bp+4]              ;file handle
        mov     cx,[bp+8]              ;file time
        mov     dx,[bp+6]              ;file date
        int     21h

        mov     sp,bp                  ;no error checking done
        pop     bp
        ret

_setftime ENDP


;*****************************************************************************
;
;  getcdir(drive,buffer)
;  int drive;
;  char *buffer;
;
;  Get the current dir for given drive.
;
;*****************************************************************************

        PUBLIC  _getcdir

_getcdir PROC   NEAR
        push    bp
        mov     bp,sp

        push    si

        mov     ah,47h                 ;get current dir
        mov     dl,BYTE PTR [bp+4]     ;drive code to DL
        mov     si,[bp+6]              ;buffer ptr to DS:SI
        mov     BYTE PTR [si],0        ;make sure its terminated if error
        int     21h

        pop     si

        mov     sp,bp                  ;no error checking done
        pop     bp
        ret

_getcdir ENDP


;*****************************************************************************
;
;  setattrib(fn,attrs)
;  char *fn;                   /* file name */
;  unsigned int attrs;         /* attributes to set */
;
;  Set the file attributes
;
;*****************************************************************************

        PUBLIC  _setattrib

_setattrib PROC NEAR
        push    bp
        mov     bp,sp

        mov     ax,4301h               ;CHMOD dos function
        mov     cx,[bp+6]              ;file attributes
        mov     dx,[bp+4]              ;file name
        int     21h

        mov     sp,bp                  ;no error checking done
        pop     bp
        ret

_setattrib ENDP


;*****************************************************************************
;
;   fakerror - fake errno and _doserrno values based on error code in ax
;
;   This is very very MSDOS/MSC dependent!
;
;*****************************************************************************

EACCES = 13    ; errno value for access error    *** these values correspond **
EBADF  = 9     ; errno value for bad file handle *** to values in errno.h    **

fakerror PROC NEAR

        mov   __doserrno,ax            ;save raw error code in _doserrno

        cmp   ax,6                     ;did dos say bad file handle?
        jne   access

        mov   _errno,EBADF             ;bad file handle
        ret

access: mov   _errno,EACCES            ;return any other as access error
        ret

fakerror ENDP

_TEXT   ENDS
END
