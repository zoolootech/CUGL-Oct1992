
#ifndef	TRUE	/* check to see if include file is needed */
#include <C.DEF>
#endif

/* Begin a comment line for the assembler */
comment()
{
	outbyte(';');
	}
/* Print all assembler info before any code is generated */
header()
{
	outstr("@start:\tcsect");nl();
	ol("jmp\t@init");
	ol("extrn\t@and,@asl,@asr,@comp,@com,@div");
	ol("extrn\t@gchar,@gint,@incdec");
	ol("extrn\t@mult,@neg,@nlog,@or,@pint,@sub");
	ol("extrn\t@sxt,@ucomp,@xor,@init,@switch");
	ol("extrn\t@gintss,@gintsl,@gcharss,@gcharsl");
	ol("extrn\t@pintss,@pintsl,@pcharss,@pcharsl");

	outstr("@eq\tequ\t@comp");nl();
	outstr("@ge\tequ\t@comp+35h");nl();
	outstr("@gt\tequ\t@comp+1ah");nl();
	outstr("@le\tequ\t@comp+43h");nl();
	outstr("@lt\tequ\t@comp+28h");nl();
	outstr("@ne\tequ\t@comp+0dh");nl();
	outstr("@uge\tequ\t@ucomp+6ch");nl();
	outstr("@ugt\tequ\t@ucomp+51h");nl();
	outstr("@ule\tequ\t@ucomp+79h");nl();
	outstr("@ult\tequ\t@ucomp+5fh");nl();

	outstr("@preinc\tequ\t@incdec");nl();
	outstr("@postinc\tequ\t@incdec+10h");nl();
	outstr("@predec\tequ\t@incdec+08h");nl();
	outstr("@postdec\tequ\t@incdec+1ah");nl();
	}
/* Print any assembler stuff needed after all code */
trailer()
{
	ol("end");
	}
/*							*/
/*	rewritten  4/30/81 by Mike Bernson		*/
/*							*/
/* Fetch a static memory cell into the primary register */
getmem(sname,typ,off)
char *sname;
int  typ;
int  off;
{
	if(typ==cchar) {
		ot("lda\t");
		outstr(sname);
		if (off) {
			outstr("+");
			outdec(off);
			}
		nl();
		call("@sxt");
		}
	else {
		ot("lhld\t");
		outstr(sname);
		if (off) {
			outstr("+");
			outdec(off);
			}
		nl();
		}
	}
/*							*/
/*	written by Mike Bernson 6/8/81			*/
/*							*/
/*	load a byte offset from current stack		*/
/*							*/
getmem_stack(lval,typ)
int  *lval;
char *typ;
{
	char *ptr;
	int  off;

	ptr=lval[0];
	off=ptr[offset]+ptr[offset+1]*256+lval[2]-sp;
	
	if (typ == cint)
		if (isbyte(off)) call("@gintss"); else call("@gintsl");
	else
		if (isbyte(off)) call("@gcharss"); else call("@gcharsl");
	defvalue(off);
	}
/*							*/
/*	rewritten 4/30/81 By Mike Bernson		*/
/*							*/	
/* Store the primary register into the specified	*/
/*	static memory					*/
putmem(sname,typ,off)
char *sname;
int typ;
int off;
{
	if (typ==cchar) {
		ol("mov\ta,l");
		ot("sta\t");
		}
	else ot("shld\t");
	outstr(sname);
	if (off) {
		outstr("+");
		outdec(off);
		}
	nl();
	}

/* Store the specified object type in the primary register */
/*      at the address on the top of the stack */
putstk(typeobj)
char typeobj;
{
	pop();
	putsec(typeobj);
	}
/* store the specified object type in the primary register */
/*	at the address in the secondary register	*/
putsec(typeobj)
char typeobj;
{
	if (typeobj == cchar) {
		ol("mov\ta,l");
		ol("stax\td");
		}
	else call("@pint");
	}
/*					*/
/*	written by Mike Bernson 6/9/81	*/
/*					*/
/*	store hl value after call plus	*/
/*	current stack			*/
/*					*/
putmem_stack(lval,typ)
int  *lval;
char typ;
{
	int off;
	char *ptr;

	ptr=lval[0];
	off=ptr[offset]+ptr[offset+1]*256+lval[2]-sp;

	if (typ == cint) 
		if (isbyte(off)) call("@pintss"); else call("@pintsl");
	else
		if (isbyte(off)) call("@pcharss"); else call("@pcharsl");
	defvalue(off);
	}

/* Fetch the specified object type indirect through the */
/*      primary register into the primary register */
indirect(typeobj)
char typeobj;
{
	if (typeobj == cchar) call("@gchar");
	else call("@gint");
	}
/* add primary and secondary registers */
add_address()
{
	ol("dad\td");
	}
/* Swap the primary and secondary registers */
swap()
{
	ol("xchg");
	}
/* Print partial instruction to get an immediate value */
/*      into the primary register */
immed()
{
	ot("lxi\th,");
	}
/* Push the primary register onto the stack */
push()
{
	ol("push\th");
	sp=sp-2;
	}
/* push the secondary register	on stack */
pushs()
{
	ol("push\td");
	sp=sp-2;
	}
/* Pop the top of the stack into the secondary register */
pop()
{
	ol("pop\td");
	sp=sp+2;
	}
/* Swap the primary register and the top of the stack */
swapstk()
{
	ol("xthl");
	}
/* call routine and subtract 2	from stack pointer */
ccall(sname)
char *sname;
{
	sp=sp+2;
	call(sname);
	}
/* Call the specified subroutine name */
call(sname)
char *sname;
{
	ot("call\t");
	outstr(sname);
	nl();
	}
/* Return from subroutine */
ret()
{
	ol("ret");
	}
/* Perform subroutine call to calue on top of stack */
callstk()
{
	immed();
	outstr("S+5");
	nl();
	swapstk();
	ol("pchl");
	sp=sp+2;
	}
/* Jump to specified internal label number */
jump(label)
int label;
{
	ot("jmp\t");
	printlabel(label);
	nl();
	}
/* test the primary register and jump if treu to label */
truejump(label,status)
int label;
int status;
{
	if (!status) {
		ol("mov\ta,h");
		ol("ora\tl");
		}
	ot("jnz\t");
	printlabel(label);
	nl();
	}


/* Test the primary register and jump if false to label */
testjump(label,status)
int label;
int status;
{
	if (!status) {
		ol("mov\ta,h");
		ol("ora\tl");
		}
	ot("jz\t");
	printlabel(label);
	nl();
	}
/* routine to search switch table */
exec_switch(count,label,end_label)
int count;	/* number of case statement in switch */
int label;	/* label for switch table */
int end_label;	/* label to execute when table search ends and not found */
{
	ot("lxi\td,");
	printlabel(label);
	nl();
	ot("lxi\th,");
	printlabel(end_label);
	nl();
	ot("mvi\tb,");
	outdec(count);
	nl();
	ot("jmp\t@switch");
	nl();
	}
/* output extrn and symbol name */
extrn(n)
char *n;
{
	outstr("\textrn\t");
	outstr(n);nl();
	}

/* output control section  name and key word */
csect(n)
char *n;
{
	outstr(n);
	col();
	ol("CSECT");
	}

/* output data section name and key word */
dsect(n)
char *n;
{
	outstr(n);
	col();
	ol("dsect");
	}
/* define value using min amount of stroage space */
defvalue(num)
int num;
{
	if (isbyte(num)) defbyte(); else defword();
	outdec(num);
	nl();
	}

/* check to see if number is a byte */
isbyte(num)
int num;
{
	return (num >= 0 && num <=255);
	}

/* Print pseudo-op to define a byte */
defbyte()
{
	ot("db\t");
	}
/* Print pseudo-op to define storage */
defstorage()
{
	ot("ds\t");
	}
/* Print pseudo-op to define a word */
defword()
{
	ot("dw\t");
	}
/* Modify the stack pointer to the new value indicated */
modstk(newsp)
int newsp;
{
	int k;
	k=newsp-sp;
	if (!k) return newsp;
	if ( k >= 0) {
		if(k<7) {
			if (k&1) {
				ol("inx\tsp");
				k--;
				}
			while(k) {
				ol("pop\tb");
				k=k-2;
				}
			return newsp;
			}
		}
	if (k<0) {
		if (k>-7) {
			if (k&1) {
				ol("dcx\tsp");
				k++;
				}
			while(k) {
				ol("push\tb");
				k=k+2;
				}
			return newsp;
			}
		}
	swap();
	immed();
	outdec(k);
	nl();
	ol("dad\tsp");
	ol("sphl");
	swap();
	return newsp;
	}
/*	Double the primary register	*/
doublereg()
{
	ol("dad\th");
	}


dad\th");
	}


	}


CH         Ow  #wt! GETCH         ��  