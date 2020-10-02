/*
HEADER:		Small-C source part 8;
FILENAME:	CC8.C;
AUTHORS:	Dieter H. Flunkert;
COMPILERS:Turbo C V2.0 Medium Memory Model;
SYSTEM: MSDOS (Brian Brown, Nov 1989)
*/

#include <stdio.h>
#include "ccdef.c"

/* external function declarations */
extern int nl(), ol(), ot(), outbyte(), outdec(), outstr(),
  printlabel(), postlabel(), prelabel();

/* external variable declaration */
extern int stkp, asmtype;

/* declare entry point GLOBAL, it is for the linker */
entry(sname, class) char *sname; int class; {
  if(class!=statik) {
	nl();
	if( asmtype == AS6809 ) ot(".globl ");
	else if( asmtype == AS9 ) outstr("* .global ");
    outstr(sname);nl();
  }
  else {
    nl();
  }
  outstr(sname); outstr(": "); nl();
}

/* SWITCH */
sw()
{   ol("jsr ccswitch");
}

/*  declare external reference */
declexternal(n) char *n; {
  nl();
  if( asmtype == AS9 ) outstr("* external reference");  else outstr("; external reference");
  nl();
  if( asmtype == AS6809) ot(".globl ");
  else if( asmtype == AS9) outstr("* .global");
  outstr(n);nl();
}

/* Begin a comment line for the assembler */
comment()
{
  if( asmtype == AS9 ){ nl(); outbyte('*'); } else outbyte(';');
  outbyte(' ');
}

/* print NAME of the module */
defname(n)
char *n;
{
  nl();
  if(asmtype==AS6809) ot(".module ");
  else if(asmtype==AS9) outstr("* .module ");
  outstr(n);
  nl();
}

/* Print all assembler info before any code is generated */
header()
{
  comment();
  outstr(VERSION);
  nl();
}

/* Print any assembler stuff needed after all code */
trailer()
{
 if( asmtype == AS6809) outstr(";    .end"); else outstr("  END");
 nl();
}

/* Fetch a static memory cell into the primary register */
getmem(sym)
char *sym;
{
 int nameoffset;
 if(sym[storage] == statik) nameoffset=symsiz;
 else nameoffset=name;
 if((sym[ident]!=pointer)&&(sym[type]==cchar)) {
  ot("ldb  ");
  outstr(sym+nameoffset);
  nl();
  ol("sex");
 }
 else {
  ot("ldd  ");
  outstr(sym+nameoffset);
  nl();
 }
}

/*  load secondary register with value which address is on top of the stack
    Dieter H. Flunkert  13-jul-86 */
loadsec()
{
  ol("ldx  [,s]");
}

/* Fetch the address of the specified symbol */
/* into the primary register */
/* changed by Dieter H. Flunkert 16-Jan-1986 */
/* Calculates location depending of ident and type */
getloc(sym)
 char *sym;
{ int dec;

 ot("leay ");
/* next three lines changed by Dieter Flunkert */
 dec=((sym[offset] & 255) | (sym[offset+1]<<8)) - stkp;
 if((sym[ident]==variable)&(sym[type]==cchar)) ++dec;
 outdec(dec);
/* end change */
 outstr(",s");
 nl();
 ol("tfr  y,d");
}

/* Store the primary register into the specified static memory cell */
/* changed for static variables dhf 29-oct-86 */
putmem(sym)
char *sym;
{
  int nameoffset;
  if(sym[storage] == statik) nameoffset=symsiz;
  else nameoffset=name;
  if((sym[ident]!=pointer) & (sym[type]==cchar))
    ot("stb  ");
  else
    ot("std  ");
  outstr(sym+nameoffset);
  nl();
}

/* Store the specified object type in the primary register */
/* at the address on the top of the stack */
putstk(typeobj)
 char typeobj;
{
 if(typeobj==cchar)ol("stb  [,s++]");
  else ol("std  [,s++]");
 stkp = stkp + 2;
}

/* Fetch the specified object type indirect through the */
/* primary register into the primary register */
indirect(typeobj)
 char typeobj;
{
 ol("pshs d");
 if(typeobj==cchar){ ol("ldb  [,s++]");ol("sex"); }
  else ol("ldd  [,s++]");
}

/* Swap the primary and secondary registers */
swap()
{ ol("exg  d,x");
}

/* Print partial instruction to get an immediate value */
/* into the primary register */
immed()
{ ot("ldd  #");
}

/* Push the primary register onto the stack */
push()
{ ol("pshs d");
 stkp=stkp-2;
}

/* push secondary register on stack Dieter H. Flunkert  13-jul-86 */
pushsec() {
  ol("pshs x");
  stkp=stkp-2;
}

/* Pop the top of the stack into the secondary register */
pop()
{ ol("puls x");
 stkp=stkp+2;
}

/* Swap the primary register and the top of the stack */
swapstk()
{ ol("puls x");
 ol("pshs d");
 ol("tfr  x,d");
}

/* Call the specified subroutine name */
call(sname)
 char *sname;
{ ot("jsr ");
 outstr(sname);
 nl();
}

/* Return from subroutine */
ret()
{ ol("rts");
}

/* Perform subroutine call to value on top of stack */
callstk()
{ pop();
 ol("jsr  ,x");
 }

/* Jump to specified internal label number */
jump(label)
int label;
{
  ot("jmp ");
  postlabel(label);
}

/* like testjump but flags the instruction for no optimize */
testnoopt(label)
int label;
{
  ol("cmpd #0");
  ot("lbeq ");
  printlabel(label);
  outstr(" ;_  instruction flagged for non optimize");
  nl();
}

/* Test the primary register and jump if false to label */
testjump(label)
int label;
{
  ol("cmpd #0");
  ot("lbeq ");
  postlabel(label);
}

/* Test the primary register and jump if true to label */
testtruejump(label)
int label;
{
   ol("cmpd #0");
   ot("lbne ");
   postlabel(label);
}

/* Debug feature */
debug(str)
char *str;
{
  ol("jsr debug");
  ot(".fcc ");
  outbyte('"');
  outstr(str);
  outbyte('"');
  outstr(",0");
  nl();
}

/* Print pseudo-op to define storage */
defstorage(siz)
int siz;
{
	if( asmtype == AS6809 ) {
		if(siz == 1) ot(".db ");  else ot(".dw ");
	}
	else if( asmtype == AS9 ) {
		if(siz == 1) ot("FCB ");  else ot("FDB ");
	}
}

/* reserve memory and init with zero */
dumpzero(siz)
int siz;
{
  if(siz <= 0) return;
  if( asmtype==AS6809) ot(".ds "); else if( asmtype == AS9) ot("RMB ");
  outdec(siz);
  nl();
}

/* point to following object(s) */
point()
{
  if( asmtype == AS6809) ol(".dw .+2"); else if( asmtype == AS9 ) ol("FDB  *+2");
}

/* Modify the stack pointer to the new value indicated */
modstk(newsp)
int newsp;
{
  int k;
  k=newsp-stkp;
  if(k==0)return (newsp);
  ot("leas ");
  outdec(k);
  outstr(",s");
  nl();
  return (newsp);
}


/* Double the primary register */
doublereg()
{
  ol("pshs d");
  ol("addd ,s++");
}

/* Add the primary and secondary registers (which is on stack) */
/* (results in primary) */
add()
{
  ol("addd ,s++");
  stkp=stkp+2;
}

/* Subtract the primary register from the secondary (which is on stack) */
/* (results in primary) */
sub()
{
  swapstk();
  ol("subd ,s++");
  stkp=stkp+2;
}

/* Multiply the primary and secondary registers */
/* (results in primary */
mult()
{
  call("ccmult");
}

/* Divide the secondary register by the primary */
/* (quotient in primary, remainder in secondary) */
div()
{
  call("ccdiv");
}

/* Compute remainder (mod) of secondary register divided */
/* by the primary */
/* (remainder in primary, quotient in secondary) */
mod()
{
  div();
  swap();
}

/* Inclusive 'or' the primary and the secondary registers */
/* (results in primary) */
or()
{
  ol("ora  ,s+");
  ol("orb  ,s+");
  stkp=stkp+2;
}

/* Exclusive 'or' the primary and seconday registers */
/* (results in primary) */
xor()
{
  ol("eora ,s+");
  ol("eorb ,s+");
  stkp=stkp+2;
}

/* 'And' the primary and secondary registers */
/* (results in primary) */
and()
{
  ol("anda ,s+");
  ol("andb ,s+");
  stkp=stkp+2;
}

/* Arithmetic shift right the secondary register number of */
/*  times in primary (results in primary) */
asr()
{
  ol("decb");
  if( asmtype == AS6809) ol("blt  .+8");
  else if( asmtype == AS9 ) ol("blt  *+8");
  ol("lsr  ,s");
  ol("ror  1,s");
  if( asmtype == AS6809) ol("bra  .-7");
  else if( asmtype == AS9 ) ol("bra *-7");
  ol("puls d");
  stkp=stkp+2;
}

/* Arithmetic left shift the secondary register number of */
/* times in primary (results in primary) */
asl()
{
  ol("decb");
  if( asmtype == AS6809) ol("blt  .+8");
  else if( asmtype == AS9 ) ol("blt  *+8");
  ol("asl  1,s");
  ol("rol  ,s");
  if( asmtype == AS6809) ol("bra  .-7");
  else if( asmtype == AS9 ) ol("bra  *-7");
  ol("puls d");
  stkp=stkp+2;
}

/* logical not forms logical not  13-jul-86  Dieter H. Flunkert */
lognot()
{
  if( asmtype == AS6809) ol("beq  .+7");
  else if( asmtype == AS9 ) ol("beq  *+7");
  restcom();
}

/* Form two's complement of primary register */
neg()
{
  com();
  ol("addd #1");
}

/* Form one's complement of primary register */
com()
{
  ol("coma");
  ol("comb");
}

/* Increment the primary register by one */
inc()
{
  ol("addd #1");
}

/* Decrement the primary register by one */
dec()
{
  ol("subd #1");
}

/* Following are the conditional operators */
/* They compare the secondary register against the primary */
/* and put a literal 1 in the primary if the condition is */
/* true, otherwise they clear the primary register */

/* This is the rest of every compare operation */
restcom()
{
  ol("ldd  #0");
  if( asmtype == AS6809) ol("bra  .+5");
  else if( asmtype == AS9 ) ol("bra  *+5");
  ol("ldd  #1");
  stkp=stkp+2; /* adjust stack */
}

/* compare primary - contense of stack */
cmpd()
{
  ol("cmpd ,s++");
}

/* Test for equal */
eq()
{
  cmpd();
  if( asmtype == AS6809) ol("beq  .+7");
  else if( asmtype == AS9) ol("beq  *+7");
  restcom();
}

/* Test for not equal */
ne()
{
  cmpd();
  if( asmtype == AS6809) ol("bne  .+7");
  else if( asmtype == AS9 ) ol("bne   *+7");
  restcom();
}

/* Test for less than (signed) */
lt()
{
  cmpd();
  if( asmtype == AS6809) ol("bgt  .+7");
  else if( asmtype == AS9 ) ol("bgt  *+7");
  restcom();
}

/* Test for less than or equal to (signed) */
le()
{
  cmpd();
  if( asmtype == AS6809) ol("bge  .+7");
  else if( asmtype == AS9 ) ol("bge  *+7");
  restcom();
}

/* Test for greater than (signed) */
gt()
{
  cmpd();
  if( asmtype == AS6809) ol("blt  .+7");
  else if( asmtype == AS9 ) ol("blt  *+7");
  restcom();
}

/* Test for greater than or equal to (signed) */
ge()
{
  cmpd();
  if( asmtype == AS6809) ol("ble  .+7");
  else if( asmtype == AS9 ) ol("ble  *+7");
  restcom();
}

/* Test for less than (unsigned) */
ult()
{
  cmpd();
  if( asmtype == AS6809) ol("bhi  .+7");
  else if( asmtype == AS9 ) ol("bhi  *+7");
  restcom();
}

/* Test for less than or equal to (unsigned) */
ule()
{
  cmpd();
  if( asmtype == AS6809) ol("bhs  .+7");
  else if( asmtype == AS9 ) ol("bhs  *+7");
  restcom();
}

/* Test for greater than (unsigned) */
ugt()
{
  cmpd();
  if( asmtype == AS6809) ol("blo  .+7");
  else if( asmtype == AS9 ) ol("blo  *+7");
  restcom();
}

/* Test for greater than or equal to (unsigned) */
uge()
{
  cmpd();
  if( asmtype == AS6809) ol("bls  .+7");
  else if( asmtype == AS9 ) ol("bls  *+7");
  restcom();
}
