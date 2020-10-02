/* >>>>>> start of cc8 <<<<<<< */
/*
** declare entry point GLOBAL
** it is for the linker
*/
entry(sname, class) char *sname; int class; {
  if(class!=statik) {
    ot("GLOBAL ");outstr(sname);nl();
  }
  outstr(sname);nl();
  }

/*
*  SWITCH 
*/
sw()
  {ol("JSR CCSWITCH");}

/*
**  declare external reference
*/
declexternal(n) char *n; {
  ot("EXT ");outstr(n);nl();
  }

/* Begin a comment line for the assembler */
comment()
{ outbyte(';');
}
/* print NAME of the module */
defname(n) char *n; {
  ot("NAME ");
  outstr(n);
  nl();
}
/* Print all assembler info before any code is generated */
header()
{ comment();
 outstr(VERSION);
 nl();
}
/* Print any assembler stuff needed after all code */
trailer()
{
 ot("END"); 
}
/* Fetch a static memory cell into the primary register */
getmem(sym)
 char *sym;
{int nameoffset;
 if(sym[storage] == statik) nameoffset=symsiz;
 else nameoffset=name;
 if((sym[ident]!=pointer)&&(sym[type]==cchar)) {
  ot("LDB  ");
  outstr(sym+nameoffset);
  if(sym[storage] == statik) outbyte('B'); /* dhf 29-oct-86 */
  nl();
  ol("SEX");
 }
 else {
  ot("LDD  ");
  outstr(sym+nameoffset);
  if(sym[storage] == statik) outbyte('B'); /* dhf 29-oct-86 */
  nl();
 }
}

/*
**  load secondary register with value which address
**  is on top of the stack
**  Dieter H. Flunkert  13-jul-86
*/
loadsec() {
  ol("LDX  [,S]");
}

/* Fetch the address of the specified symbol */
/* into the primary register */
/* changed by Dieter H. Flunkert 16-Jan-1986 */
/* Calculates location depending of ident and type */
getloc(sym)
 char *sym;
{ int dec;

 ot("LEAY ");
/* next three lines changed by Dieter Flunkert */
 dec=((sym[offset] & 255) | (sym[offset+1]<<8)) - stkp;
 if((sym[ident]==variable)&(sym[type]==cchar)) ++dec;
 outdec(dec);
/* end change */
 outstr(",S");
 nl();
 ol("TFR  Y,D");
}
/* Store the primary register into the specified */
/* static memory cell */
/* changed for static variables dhf 29-oct-86 */
putmem(sym)
 char *sym;
{int nameoffset;
if(sym[storage] == statik) nameoffset=symsiz;
 else nameoffset=name;
 if((sym[ident]!=pointer)&(sym[type]==cchar))
   ot("STB  ");
 else
   ot("STD  ");
 outstr(sym+nameoffset);
 if(sym[storage] == statik) outbyte('B');
 nl();
}
/* Store the specified object type in the primary register */
/* at the address on the top of the stack */
putstk(typeobj)
 char typeobj;
{
 if(typeobj==cchar)ol("STB  [,S++]");
  else ol("STD  [,S++]");
 stkp = stkp + 2;
}
/* Fetch the specified object type indirect through the */
/* primary register into the primary register */
indirect(typeobj)
 char typeobj;
{
 ol("PSHS D");
 if(typeobj==cchar){ ol("LDB  [,S++]");ol("SEX"); }
  else ol("LDD  [,S++]");
}
/* Swap the primary and secondary registers */
swap()
{ ol("EXG  D,X");
}
/* Print partial instruction to get an immediate value */
/* into the primary register */
immed()
{ ot("LDD  #");
}
/* Push the primary register onto the stack */
push()
{ ol("PSHS D");
 stkp=stkp-2;
}

/*
**  push secondary register on stack
**  Dieter H. Flunkert  13-jul-86
*/
pushsec() {
  ol("PSHS X");
  stkp=stkp-2;
}

/* Pop the top of the stack into the secondary register */
pop()
{ ol("PULS X");
 stkp=stkp+2;
}
/* Swap the primary register and the top of the stack */
swapstk()
{ ol("PULS X");
 ol("PSHS D");
 ol("TFR  X,D");
}
/* Call the specified subroutine name */
call(sname)
 char *sname;
{ ot("JSR ");
 outstr(sname);
 nl();
}
/* Return from subroutine */
ret()
{ ol("RTS");
}
/* Perform subroutine call to value on top of stack */
callstk()
{ pop();
 ol("JSR  ,X");
 }
/* Jump to specified internal label number */
jump(label)
 int label;
{ ot("JMP ");
 postlabel(label);
 }
/*
**  like testjump but flags the instruction for no optimize
*/
testnoopt(label) int label; {
  ol("CMPD #0");
  ot("LBEQ ");
  printlabel(label);
  outstr(" _");
  nl();
}
/* Test the primary register and jump if false to label */
testjump(label)
 int label;
{
  ol("CMPD #0");
  ot("LBEQ ");
  postlabel(label);
 }

/*
**  Test the primary register and jump if true to label
**  Dieter H. Flunkert    18-jul-86
*/
testtruejump(label) int label; {
   ol("CMPD #0");
   ot("LBNE ");
   postlabel(label);
}

/*
** Debug feature 
*/
debug(str) char *str; {
  ol("JSR DEBUG");
  ot("FCC ");
  outbyte('"');
  outstr(str);
  outbyte('"');
  outstr(",0");
  nl();
}

/* Print pseudo-op to define storage */
defstorage(siz) int siz; {
  if(siz==1) ot("FCB ");
  else       ot("FDB ");
}

/*
**  reserve memory and init with zero
*/
dumpzero(siz) int siz; {
  if(siz<=0) return;
  ot("RZB ");
  outdec(siz);
  nl();
}

/*
**  point to following object(s)
*/
point() {
  ol("FDB *+2");
  }

/* Modify the stack pointer to the new value indicated */
modstk(newsp)
 int newsp;
 { int k;
 k=newsp-stkp;
 if(k==0)return (newsp);
 ot("LEAS ");
 outdec(k);
 outstr(",S");
 nl();
 return (newsp);
}
/* Double the primary register */
doublereg()
{ ol("PSHS D");
 ol("ADDD ,S++");
}
/* Add the primary and secondary registers (which is on stack) */
/* (results in primary) */
add()
{ ol("ADDD ,S++");
 stkp=stkp+2;
}
/* Subtract the primary register from the secondary (which is on stack) */
/* (results in primary) */
sub()
{ swapstk();
 ol("SUBD ,S++");
 stkp=stkp+2;
}
/* Multiply the primary and secondary registers */
/* (results in primary */
mult()
{ call("ccmult");
}
/* Divide the secondary register by the primary */
/* (quotient in primary, remainder in secondary) */
div()
{ call("ccdiv");
}
/* Compute remainder (mod) of secondary register divided */
/* by the primary */
/* (remainder in primary, quotient in secondary) */
mod()
{ div();
 swap();
 }
/* Inclusive 'or' the primary and the secondary registers */
/* (results in primary) */
or()
{ ol("ORA  ,S+");
 ol("ORB  ,S+");
 stkp=stkp+2;
}
/* Exclusive 'or' the primary and seconday registers */
/* (results in primary) */
xor()
{ ol("EORA ,S+");
 ol("EORB ,S+");
 stkp=stkp+2;
}
/* 'And' the primary and secondary registers */
/* (results in primary) */
and()
{ ol("ANDA ,S+");
 ol("ANDB ,S+");
 stkp=stkp+2;
}
/* Arithmetic shift right the secondary register number of */
/*  times in primary (results in primary) */
asr()
{ol("DECB");
 ol("BLT  *+8");
 ol("LSR  ,S");
 ol("ROR  1,S");
 ol("BRA  *-7");
 ol("PULS D");
 stkp=stkp+2;
}
/* Arithmetic left shift the secondary register number of */
/* times in primary (results in primary) */
asl()
{ol("DECB");
 ol("BLT  *+8");
 ol("ASL  1,S");
 ol("ROL  ,S");
 ol("BRA  *-7");
 ol("PULS D");
 stkp=stkp+2;
}
/*
** lognot forms logical not
**  13-jul-86  Dieter H. Flunkert
*/
lognot() {
 ol("BEQ  *+7");
 restcom();
}
/* Form two's complement of primary register */
neg()
{ com();
 ol("ADDD #1");
}
/* Form one's complement of primary register */
com()
{ ol("COMA");
 ol("COMB");
}
/* Increment the primary register by one */
inc()
 {ol("ADDD #1");}
/* Decrement the primary register by one */
dec()
 {ol("SUBD #1");}
/* Following are the conditional operators */
/* They compare the secondary register against the primary */
/* and put a literal 1 in the primary if the condition is */
/* true, otherwise they clear the primary register */

/* This is the rest of every compare operation */
restcom()
{ ol("LDD  #0");
 ol("BRA  *+5");
 ol("LDD  #1");
 stkp=stkp+2; /* adjust stack */
}

/*
**  compare primary - contense of stack
*/
cmpd() {
  ol("CMPD ,S++");
}

/* Test for equal */
eq()
{ cmpd();
 ol("BEQ  *+7");
 restcom();
}
/* Test for not equal */
ne()
{ cmpd();
 ol("BNE  *+7");
 restcom();
}
/* Test for less than (signed) */
lt()
{ cmpd();
 ol("BGT  *+7");
 restcom();
}
/* Test for less than or equal to (signed) */
le()
{ cmpd();
 ol("BGE  *+7");
 restcom();
}
/* Test for greater than (signed) */
gt()
{ cmpd();
 ol("BLT  *+7");
 restcom();
}
/* Test for greater than or equal to (signed) */
ge()
{ cmpd();
 ol("BLE  *+7");
 restcom();
}
/* Test for less than (unsigned) */
ult() {
  cmpd();
  ol("BHI  *+7");
  restcom();
}

/* Test for less than or equal to (unsigned) */
ule() {
  cmpd();
  ol("BHS  *+7");
  restcom();
}

/* Test for greater than (unsigned) */
ugt() {
  cmpd();
  ol("BLO  *+7");
  restcom();
}

/* Test for greater than or equal to (unsigned) */
uge() {
  cmpd();
  ol("BLS  *+7");
  restcom();
}
