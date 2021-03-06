page 66,132
;+
;       .title sysint
; index  system interrupt call function   sysint()
;
; Usage
;       int flags;
;
;       flags = sysint(inum,&inreg,&outreg);
;
; in
;       int     inum;           ; interrupt number to execute
;       int     inreg[4];       ; input registers ax,bx,cx,dx
;
; out
;       int     outreg[4];      ; registers returned ax,bx,cx,dx
;       sysint                  ; flag register returned
;
; Description
;       This is a system interface call to allow system intrinsic functions
;      to be called from C. Parameters are passed via the register values
;      stored in inreg for input to the system call and returned in the
;      outreg struct. The default values for the segment registers are the
;      same as C routines.
;
; status
;       ax register is returned as status
;
; bugs
;       low level internal routine must be modified to be ported.
;
; Updates
;
;       date            vers    who     description
;       15-aug-83       0001    EJK     Added documemtation
;       13-sep-83       0002    CMC     Added flag register return
;-
pgroup  group   prog
prog    segment byte    public 'prog'
        assume  cs:pgroup
        public  sysint

sysint  proc    near
        push bp         ;save bp
        mov bp,sp       ;sp->bp
        mov ax,[bp]+4   ;get int#
        mov cs:itm+1,al ;set int#
        mov si,[bp]+6   ;in struc
        mov ax,[si]     ;set ax
        mov bx,[si]+2   ;set bx
        mov cx,[si]+4   ;set cx
        mov dx,[si]+6   ;set dx
        push bp         ;save bp2
itm     equ  this byte  ; 'this byte' is a keyword.
        int 16          ;interrupt
        pop bp          ;restore bp2
        mov si,[bp]+8   ;out struc
        mov [si],ax     ;ret ax
        mov [si]+2,bx   ;ret bx
        mov [si]+4,cx   ;ret cx
        mov [si]+6,dx   ;ret dx
        pop bp          ;restore bp
	pushf           ;save return flag register on stack
        pop ax          ;restore flag register in ax
        ret             ;return
sysint  endp
prog    ends
        end
                                                                                                                                                                                                                                                                                                                                                                                                                                                            