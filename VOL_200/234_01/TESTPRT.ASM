;
;______________________________________________________________________
;
; testprt.asm
; from PC Magazine 6/24/86
; test availability of printer -1 ready -0 not ready                  |
;_____________________________________________________________________|
;
;lpt1stat - checks the current status of LPT1
;
                   include pro.h

$b$prog            segment public 'code'
                   public _testprt
_testprt           proc near
                   push bp
                   mov  bp,sp
                   push di
                   push si
                   push dx
                   mov  dx,0
                   mov  ah,2
                   int  17h
                   test ah,8
                   je   stat1            ;printer available
                   mov  ax,0             ;indicate error
                   jmp  short done
stat1:             mov  ax,1             ;printer available
                   jmp  short done
done:              pop  dx
                   pop  si
                   pop  di
                   pop  bp
                   ret
_testprt           endp
$b$prog            ends
                   end

