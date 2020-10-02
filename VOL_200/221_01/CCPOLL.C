/*
**  check terminal
*/
CCPOLL()
{
#asm
 LIB CCEQU.SYS
 LDD  #0
 JSR  CHKTERM
 BEQ  *+3
 INCB
 RTS
#endasm
}
