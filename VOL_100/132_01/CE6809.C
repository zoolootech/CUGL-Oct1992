/* This section contains the code generator */
/*
	This version adapted for the 6809 processor
	Jan 1,1982 by ... A. Griggs
*/
/*	Jan 23,1982
	1. Deleted calls to ccgchar,ccgint,ccpchar,ccpint.
	2. Used LEAY k,s in getloc() to generate stack offset address
	   when a single post index byte may be used.
	3. Eliminated pop() in putstk() along with ccpchar and ccpint
	   calls. Used indirect post increment off the stack pointer.
*/
/*	Feb 6,1982
	1. Added function addstk() to replace pop()-add() sequence
	   with addstk() eliminating redundant pull-push.
*/
/*
	March 7,1982
	1. Changed .byte and .word 6809 opcode generations to assembly
	   language for real 6809 assembler.
*/
#INCLUDE CDEF.H
/* >>>>>> start of cc8 <<<<<<< */
/* */
/* Begin a comment line for the assembler	*/
comment()
{
	outbyte(';');
}
/* Print all assembler information before any code is generated */
header()
{	comment();
	outstr("small-c compiler rev 1.1");
	nl();
}
/* Include the runtime package	*/
inclrn()
{
	ol(";include 6809run.asm");
}
/* Print any assembler items required after code output */
trailer()
{	ol(";the end");
	outbyte(26);
}
/* Fetch a static memory cell into the primary register */
getmem(sym)
	char *sym;
{
	if((sym[ident]!=pointer)&&(sym[type]==cchar))
		{ot("ldb	");
		outstr(sym+name);
		nl();
		ol("SEX ");
		}
	else
		{ot("LDD	");
		outstr(sym+name);
		nl();
		}
}
/* Fetch the address of the specified symbol into the primary register */
getloc(sym)
	char *sym;
{
	int	symval;
/*  #### note: the following assumes 8080 style byte storage !
	low byte before high byte. If porting this compiler to 
	a machine which does this differently (ie,6800,6809)
	change offset to offset+1 and vice versa.
	It's the COMPILE time environment that calls the tune. Cross
	compilers running on 8080 based systems need not change.
*/
	symval= ((sym[offset]&255)+((sym[offset+1]&255)<<8))-sp ;
	if( absolute(symval) < 16) {
		ot("LEAY 	");
		outdec(symval); /* output indexed postbyte */ 
		outstr(",S");
		nl();
		ol("TFR	Y,D");
		}
	else {
	ol("TFR	S,D");
	ot("ADDD	#");
	outdec(symval);
	nl();
	}
}
/*
	absolute returns the absolute of the number passed to it
*/
absolute(num)
int num;
{
	if(num < 0) return( -num);
	else	   return(num);
}
/* Store the primary register into the specified static memory cell */
putmem(sym)
	char *sym;
{
	if((sym[ident]!=pointer)&&(sym[type]==cchar))
		{ot("STA	");
		}
	else {ot("STD	");
	     }
	outstr(sym+name);
	nl();
	}
/* Store the specified object type in the primary register at the */
/* address on the top of the stack				  */
putstk(typeobj)
	char typeobj;
{
	if(typeobj==cchar) {
		ol("STB	[,S++]");
		}
	else ol("STD	[,S++]");
	sp=sp + 2;
}
/* Fetch the specified object type indirect through the primary */
/* register into the primary register				*/
indirect(typeobj)
	char typeobj;
{	if(typeobj==cchar)	{
		ol("TFR	D,X");
		ol("LDB	,X");
		ol("SEX");
	}
	else {
		ol("TFR	D,X");
		ol("LDD	,X");
	     }
}
/* Swap the primary and secondary registers */
swap()
{	ol("EXG	D,X");
}
/* Print partial instruction to get an immediate value into the */
/* primary register						*/
immed()
{	ot("LDD	#");
}
/* Push the primary register onto the stack */
push()
{	ol("PSHS	D");
	sp=sp-2;
}
/* Pop the top of the stack into the primary register		*/
pop()
{	ol("PULS	X");
	sp=sp+2;
}
/* Swap the primary register and the top of the stack       	*/
swapstk()
{	ol("PULS	Y");
	ol("PSHS	D");
	ol("EXG	Y,D");
}
/* Call the specified subroutine name	*/
call(sname)
	char *sname;
{
	ot("jsr ");
	outstr(sname);
	nl();
}
/* Return from subroutine	*/
ret()
{	ol("rts");
}
/* Perform the subroutine call to the value on the top of the stack */
callstk()
{	ol("JSR	[s++]");
	sp=sp+2;
}
/* Jump to the specified internal label number	*/
jump(label)
	int	label;
{	ot("jmp ");
	printlabel(label);
	nl();
}
/* Test the primary register and jump if false to label	*/
testjump(label)
	int label;
{	ol("ADDD	#0");
	ol("bne	*+5");
	ot("jmp	");
	printlabel(label);
	nl();
}
/* Print pseudo-op to define a byte	*/
defbyte()
{
	ot("FCB	");
}
/* Print pseudo-op to define storage 	*/
defstorage()
{	ot("RMB	");
}
/* Print pseudo-op to define a word	*/
defword()
{	ot("FDB	");
}
/* Modify the stack pointer to the new value indicated	*/
modstk(newsp)
	int	newsp;
{	int k;
	k=newsp-sp;
	if(k==0) return newsp;
	if(absolute(k) < 16) {
		ot("LEAS	");
		outdec(k);
		outstr(",S");
		nl();
	}
	else
		{
	ot("LEAS	");
	outdec(k);
	outstr(",S");
	nl();
	}
	return newsp;
}
/* Double the primary register */
doublereg()
{	ol("ASLB ! ROLA");
}
/* Add the primary and secondary registers	*/
add()
{	ol("PSHS	X");
	ol("ADDD	,S++");
}
/*
	add the top of stack value to the primary register
	Added value is removed from the stack.
*/
addstk()
{
	ol("ADDD	,S++");
	sp=sp+2;
}
/* Subtract the primary register from the secondary (result in primary)*/
sub()
{	call("ccsub");
}
/* Multiply the primary and secondary registers	(result in primary) */
mult()
{	call("ccmult");
}
/* Divide the secondary register by the primary	*/
/* quotient in primary, remainder in secondary  */
div()
{	call("ccdiv");
}
/* Compute remainder (mod) of secondary register divided by the primary */
mod()
{	div();
	swap();
}
/* Inclusive "or" the primary and the secondary registers */
or()
{	call("ccor");
}
/* Exclusive 'or' the primary and the secondary registers */
xor()
{	call("ccxor");
}
/* 'and' the primary and secondary registers (result in primary) */
and()
{	call("ccand");
}
/* Arithmetic shift right the secondary register number of times in */
/*   primary (results in primary) 				    */
asr()
{	call("ccasr");
}
/* Aritmetic left shift , similar to asr */
asl()
{	call("ccasl");
}
/* Form two's complement of primary register	*/
neg()
{ 	call("ccneg");
}
/* Form the one's complement of the primary register */
com()
{
	ol("COMA ! COMB");
}
/* Increment the primary register by one */
inc()
{	ol("ADDD	#1");
}
/* Decrement the primary register by one */
dec()
{	ol("SUBD	#1");
}
/* Following are the conditional operators	*/
/* They compare the secondary register against the primary 	*/
/* register and put a literal 1 in the primary if the condition is */
/* true, otherwise they clear the primary register		*/
/* */
/* Test for equality	*/
eq()
{	call("cceq");
}
/* Test for not equal	*/
ne()
{	call("ccne");
}
/* Test for less than (signed)	*/
lt()
{	call("cclt");
}
/* Test for less than or equal to 	*/
le()
{	call("ccle");
}
/* Test fro greater than (signed)	*/
gt()
{	call("ccgt");
}
/* Test for greater than or equal to (signed)	*/
ge()
{	call("ccge");
}
/* Test for less than (unsigned)		*/
ult()
{	call("ccult");
}
/* Test for less than or equal to (unsigned)	*/
ule()
{	call("ccule");
}
/* Test for greater than (unsigned)		*/
ugt()
{	call("ccugt");
}
/* Test for greater than or equal to (unsigned) */
uge()
{	call("ccuge");
}
");
}
 */
uge()
{	call("ccuge");
}
;
}
ge()
{	call("ccuge");
}

}
cuge");
}
ll("ccuge");
}
all("ccuge")