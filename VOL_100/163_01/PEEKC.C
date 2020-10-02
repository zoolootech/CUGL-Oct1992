/*
** get a byte from a specified segment and offset
*/
 
peekc(seg, offset) int seg; char *offset; {
  #asm
       MOV BX,[BP]+6   ;get segment
       MOV ES,BX
       MOV BX,[BP]+4   ;get offset
       MOV AL,ES:[BX]  ;get the byte into the return code reg
       CBW             ;sign extend
       MOV BX,DS
       MOV ES,BX       ;restore ES
  #endasm
  }
