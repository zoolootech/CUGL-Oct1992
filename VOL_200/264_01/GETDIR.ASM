; getdir.asm
; Works with Aztec C v3.20d.

codeseg segment para public 'code'
dataseg segment para public 'data'
dataseg ends
    assume cs:codeseg,ds:dataseg,es:dataseg,ss:dataseg

codeseg segment

; int setdta(char *buf)
; Set the disk transfer address to the address pointed to by the argument.
; Gives the getfirst_ and getnext_ routines a user-defined buffer where
; filenames are accessible.

    PUBLIC  setdta_
setdta_     PROC NEAR
    push    bp
    mov     bp,sp
    mov     dx,[bp+4]               ; offset of disk transfer address
    mov     ah,1Ah                  ; ms-dos set dta
    int     21h
    mov     sp,bp
    pop     bp
    ret 
setdta_     ENDP

; int getfirst(char *path, int attributes)
; First argument points to a string with an MSDOS file specification
; (possibly containing wildcards).  Second argument is a mask of attributes
; that files must match.  Calls the MSDOS "find first file" function to find
; a filename that matches the file spec.
; Returns 0 on success, non-zero on failure.

    PUBLIC  getfirst_
getfirst_   PROC NEAR
    push    bp
    mov     bp,sp
    mov     dx,[bp+4]               ; offset of string with pathname
    mov     cx,[bp+6]               ; attributes to match
    mov     ah,4Eh                  ; ms-dos find first file
    int     21h
    jc      gf_err
    mov     ax,0
    jmp     gf_end
gf_err:
    mov     ax,1
gf_end:
    mov     sp,bp
    pop     bp
    ret 
getfirst_   ENDP

; int getnext(char *path, int attributes)
; Same as getfirst_, except uses the MSDOS "find next file" function.

    PUBLIC  getnext_
getnext_    PROC NEAR
    push    bp
    mov     bp,sp
    mov     dx,[bp+4]               ; offset of string with pathname
    mov     cx,[bp+6]               ; attributes to match
    mov     ah,4Fh                  ; ms-dos find next file
    int     21h
    jc      gn_err
    mov     ax,0
    jmp     gn_end
gn_err:
    mov     ax,1
gn_end:
    mov     sp,bp
    pop     bp
    ret 
getnext_    ENDP
    
codeseg ends
dataseg segment para public 'data'
dataseg ends
    end
