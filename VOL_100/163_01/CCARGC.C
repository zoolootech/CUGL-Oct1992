/*
** fetch argument count from DL register
*/
CCARGC() {
#asm
       MOV AL,DL ;move argument count into function return register
       CBW ;convert to 16 bit value
#endasm
  }
