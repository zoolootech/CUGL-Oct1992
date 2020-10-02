/*
** add primary and secondary registers (result in primary)
*/
add() {ol("ADD AX,BX");}
 
/*
** subtract primary from secondary register (result in primary)
*/
sub() {
  ol("SUB AX,BX");
  ol("NEG AX");
  }
 
/*
** multiply primary and secondary registers (result in primary)
*/
mult() {ol("IMUL BX");} /* (note that DX -- arg count -- is clobbered) */
 
/*
** divide secondary by primary register
** (quotient in primary, remainder in secondary)
*/
div() {
  ol("XCHG AX,BX"); /* get the dividend into AX */
  ol("CWD"); /* extend AX sign into DX */
  ol("IDIV BX");
  ol("MOV BX,DX"); /* get remainder */
  }
 
/*
** remainder of secondary/primary
** (remainder in primary, quotient in secondary)
*/
mod() {div();swap();}
 
/*
** inclusive "or" primary and secondary registers
** (result in primary)
*/
or() {ol("OR AX,BX");}
 
/*
** exclusive "or" the primary and secondary registers
** (result in primary)
*/
xor() {ol("XOR AX,BX");}
 
/*
** "and" primary and secondary registers
** (result in primary)
*/
and() {ol("AND AX,BX");}
 
/*
** logical negation of primary register
*/
lneg() {
  ol("AND AX,AX");
  ol("MOV AX,1");
  ol("JZ $+3");
  ol("DEC AX");
  }
 
/*
** arithmetic shift right secondary register
** number of bits given by primary register
** (result in primary)
*/
asr() {
  ol("MOV CX,AX"); /* get count to count register */
  ol("MOV AX,BX"); /* get shiftee to result reg */
  ol("SAR AX,CL"); /* shift the shiftee */
  /*
  ** (note that the shift is mod 256 -- consistent with K & R)
  */
  }
 
/*
** arithmetic shift left secondary register
** number of bits in primary register
** (result in primary)
*/
asl() {
  ol("MOV CX,AX"); /* get count to count register */
  ol("MOV AX,BX"); /* get shiftee to result reg */
  ol("SAL AX,CL"); /* shift the shiftee */
  /*
  ** (note that the shift is mod 256 -- consistent with K & R)
  */
  }
 
/*
** two's complement primary register
*/
neg() {ol("NEG AX");}
 
/*
** one's complement primary register
*/
com() {ol("NOT AX");}
 
/*
** (beware: the following two functions may get an argument of zero,
** indicating that the value being incremented or decremented is not
** an address and should therefore be incremented/decremented by 1)
*/
 
/*
** increment primary register by one object of whatever size
*/
inc(n) int n; {
  if(n==2) ol("ADD AX,2");
  else     ol("INC AX");
  }
 
/*
** decrement primary register by one object of whatever size
*/
dec(n) int n; {
  if(n==2) ol("SUB AX,2");
  else     ol("DEC AX");
  }
 
/*
** test for secondary equal to primary
*/
eq()  {
  ol("CMP BX,AX");
  ol("MOV AX,1");
  ol("JE $+3");
  ol("DEC AX");
  }
 
/*
** test for equal to zero
*/
eq0(label) int label; {
  ol("AND AX,AX"); /* set condition code */
  ol("JZ $+5"); /* jump if zero */
  jump(label); /* false condition */
  }
 
/*
** test for secondary not equal to primary
*/
ne() {
  ol("CMP BX,AX");
  ol("MOV AX,1");
  ol("JNE $+5");
  ol("MOV AX,0");
  }
 
/*
** test for not equal to zero
*/
ne0(label) int label; {
  ol("AND AX,AX"); /* set condition code */
  ol("JNZ $+5"); /* jump if not zero */
  jump(label); /* false condition */
  }
 
/*
** test for secondary less than primary (signed)
*/
lt() {
  ol("CMP BX,AX");
  ol("MOV AX,1");
  ol("JL $+3");
  ol("DEC AX");
  }
 
/*
** test for less than zero
*/
lt0(label) int label; {
  ol("AND AX,AX"); /* set condition code */
  ol("JL $+5"); /* jump if negative */
  jump(label); /* false condition */
  }
 
/*
** test for secondary less than or equal to primary (signed)
*/
le() {
  ol("CMP BX,AX");
  ol("MOV AX,1");
  ol("JLE $+3");
  ol("DEC AX");
  }
 
/*
** test for less than or equal to zero
*/
le0(label) int label; {
  ol("AND AX,AX"); /* set condition code */
  ol("JLE $+5"); /* jump if not positive */
  jump(label); /* false condition */
  }
 
/*
** test for secondary greater than primary (signed)
*/
gt() {
  ol("CMP BX,AX");
  ol("MOV AX,1");
  ol("JG $+3");
  ol("DEC AX");
  }
 
/*
** test for greater than zero
*/
gt0(label) int label; {
  ol("AND AX,AX"); /* set condition code */
  ol("JG $+5"); /* jump if greater than zero */
  jump(label); /* false condition */
  }
 
/*
** test for secondary greater than or equal to primary (signed)
*/
ge() {
  ol("CMP BX,AX");
  ol("MOV AX,1");
  ol("JGE $+3");
  ol("DEC AX");
  }
 
/*
** test for greater than or equal to zero
*/
ge0(label) int label; {
  ol("AND AX,AX"); /* set condition code */
  ol("JGE $+5"); /* jump if not negative */
  jump(label); /* false condition */
  }
 
/*
** test for secondary less than primary (unsigned)
*/
ult() {
  ol("CMP BX,AX");
  ol("MOV AX,1");
  ol("JC $+3");
  ol("DEC AX");
  }
 
/*
** test for less than zero (unsigned)
*/
ult0(label) int label; {
  ot("JMP ");
  printlabel(label);
  nl();
  }
 
/*
** test for secondary less than or equal to primary (unsigned)
*/
ule() {
  ol("CMP AX,BX");
  ol("MOV AX,0");
  ol("JC $+3");
  ol("INC AX");
  }
 
/*
** test for secondary greater than primary (unsigned)
*/
ugt() {
  ol("CMP AX,BX");
  ol("MOV AX,1");
  ol("JC $+3");
  ol("DEC AX");
  }
 
/*
** test for secondary greater than or equal to primary (unsigned)
*/
uge() {
  ol("CMP BX,AX");
  ol("MOV AX,0");
  ol("JC $+3");
  ol("INC AX");
  }
