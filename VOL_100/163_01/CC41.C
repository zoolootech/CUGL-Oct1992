/*
** Note on conditional jumps:
**
** Unfortunately, we have to generate:
**   Jxx $+5
**   JMP label
** so that we never have the possibility of a short jump out of range.
** It would be nice if the assembler were smart enough to do this for us.
** Failing that, we will probably have to wait for a version which generates
** OBJ modules directly to get this type of optimization in any but some
** special cases.
*/
 
/*
** print all assembler info before any code is generated
*/
header()  {
  outstr("CODESEG "); ol("SEGMENT BYTE PUBLIC 'CODE'");
  ol("ASSUME CS:CODESEG,SS:DATASEG,ES:DATASEG,DS:DATASEG");
  ol("DB 0"); /* assure that no procedure has an address of "NULL" */
  outstr("DATASEG"); ol("SEGMENT PUBLIC 'DATA'");
  dmode = 1;
  ol("DB 0"); /* assure that no variable has an address of "NULL" */
  }
 
/*
** print any assembler stuff needed at the end
*/
trailer()   {
  int k;
  char c;
  if(dmode) {outstr("DATASEG "); ol("ENDS");}
  outstr("CODESEG "); ol("ENDS");
  outstr("%OUT End of pass");
  nl();
#ifdef HASH
  if(search("MAIN_ENT", macn, NAMESIZE+2, MACNEND, MACNBR, 0)) {
    ot("END");
    k=getint(cptr+NAMESIZE, 2);
    if(*(macq+k) >= ' ') { /* don't output end label if null */
      outbyte(' ');
      outname(macq+k);
      }
    nl();
    }
#else
  if(k=findmac("MAIN_ENT")) {
    ot("END");
    if(*(macq+k) >= ' ') { /* don't output end label if null */
      outbyte(' ');
      outname(macq+k);
      }
    nl();
    }
#endif
  else { /* force a reference to the setup module */
    ot("EXTRN ");
    outname("_LOGIN");
    outstr(":NEAR");
    nl();
    ol("END");
    }
  }
 
/*
** load # args before function call
*/
loadargc(val) int val;  {
#ifdef HASH
  if(search("NOCCARGC", macn, NAMESIZE+2, MACEND, MACNBR, 0)==0) {
#else
  if(findmac("NOCCARGC")==0) {
#endif
    ot("MOV DL,");
    outdec(val);
    nl();
    }
  }
 
/*
** generate assembler comment delimiter
*/
comment() {
  cout(';', output);
  }
 
/*
** generate assembler comment delimiter in staging buffer
*/
comment2() {
  outbyte(';');
  }
 
/*
** generate a name which does not conflict with compiler reserved words
*/
outname(name) char *name; {
  outstr("Q");
  outstr(name);
  }
 
/*
** declare entry point ("PUBLIC")
*/
entry(size) int size; {
  ot("PUBLIC ");
  outname(ssname);
  nl();
  }
 
/*
** generate start of global data
*/
startglob() {
  if(dmode==0) {outstr("DATASEG"); ol("SEGMENT PUBLIC 'DATA'"); dmode = 1;}
  outname(ssname);
  }
 
/*
** generate start of a function
*/
startfun(funname) char funname[]; {
  if(dmode) {outstr("DATASEG"); ol("ENDS"); dmode = 0;}
  outname(funname);
  ol("PROC NEAR");
  ol("PUSH BP");
  ol("MOV BP,SP");
  }
 
/*
** generate end of a function
*/
endfun(funname) char funname[]; {
  outname(funname);
  ol("ENDP");
  }
 
/*
** generate start of literals from a function
*/
startlit() {
  if(dmode==0) {outstr("DATASEG"); ol("SEGMENT PUBLIC 'DATA'"); dmode = 1;}
  }
 
/*
** declare external reference
*/
external(name,type,ident) char *name; int type, ident; {
  if(ident==FUNCTION) {
    if(dmode) {outstr("DATASEG"); ol("ENDS"); dmode = 0;}
    ot("EXTRN ");
    outname(name);
    outstr(":NEAR");
    }
  else {
    if(dmode==0) {outstr("DATASEG"); ol("SEGMENT PUBLIC 'DATA'"); dmode = 1;}
    ot("EXTRN ");
    outname(name);
    if((type==CCHAR) & (ident!=POINTER))
      outstr(":BYTE");
    else
      outstr(":WORD");
    }
  nl();
  }
 
/*
** fetch object indirect to primary register
*/
indirect(lval) int lval[]; {
  /*
  ** (It would be advantageous if the compiler loaded addresses into the
  ** secondary when it plans to follow immediately with an "indirect()".
  ** Failing this, or in addition to it, "peephole()" could look for
  ** combinations such as "getloc()" followed by "indirect()" and replace
  ** the generated code with a direct load relative to BP.)
  */
  ol("MOV SI,AX");
  if(lval[1]==CCHAR) {
    ol("MOV AL,[SI]");
    ol("CBW");
    }
  else ol("MOV AX,[SI]");
  }
 
/*
** fetch a static memory cell into primary register
*/
getmem(lval)  int lval[]; {
  char *sym;
  sym=lval[0];
  if((sym[IDENT]!=POINTER)&(sym[TYPE]==CCHAR)) {
    ot("MOV AL,");
    outname(sym+NAME);
    nl();
    ol("CBW");
    }
  else {
    ot("MOV AX,");
    outname(sym+NAME);
    nl();
    }
  }
 
/*
** fetch addr of the specified global symbol into primary register
*/
address(sym) char *sym; {
  ot("MOV AX,OFFSET ");
  outname(sym+NAME);
  nl();
  }
 
/*
** fetch addr of what follows into primary register
*/
addr2() {
  ot("MOV AX,OFFSET ");
  }
 
/*
** fetch addr of the specified local symbol into primary register
*/
getloc(sym) char *sym; {
  int displace; /* displacement from BP */
  if((displace=getint(sym+OFFSET, OFFSIZE))>=0) {
    ot("LEA AX,[BP]+");
    outdec(displace);
    }
  else {
    ot("LEA AX,[BP]-");
    outdec(-displace);
    }
  nl();
  }
 
/*
** store primary register into static cell
*/
putmem(lval)  int lval[]; {
  char *sym;
  ot("MOV ");
  sym=lval[0];
  outname(sym+NAME);
  if((sym[IDENT]!=POINTER)&(sym[TYPE]==CCHAR)) outstr(",AL");
  else outstr(",AX");
  nl();
  }
 
/*
** put on the stack the type object in primary register
** (store primary register at address in secondary register)
*/
putstk(lval) int lval[]; {
  if(lval[1]==CCHAR) ol("MOV [BX],AL");
  else ol("MOV [BX],AX");
  }
 
/*
** move primary register to secondary
*/
move() {
  ol("MOV BX,AX");
  }
 
/*
** swap primary and secondary registers
*/
swap() {
  ol("XCHG AX,BX");
  }
 
/*
** partial instruction to get immediate value
** into primary register
*/
immed() {
  ot("MOV AX,");
  }
 
/*
** partial instruction to get immediate operand
** into secondary register
*/
immed2() {
  ot("MOV BX,");
  }
 
/*
** push primary register onto stack
*/
push() {
  ol("PUSH AX");
  csp=csp-BPW;
  }
 
/*
** pop stack to the secondary register
*/
pop() {
  ol("POP BX");
  csp=csp+BPW;
  }
 
/*
** swap primary register and stack
*/
swapstk() {
  ot("XCHG AX,[BP]-");
  outdec(-csp);
  nl();
  }
 
/*
** process switch case statements
*/
sw(label, value) int label, value; {
  ot("CMP AX,");
  outdec(value);
  nl();
  ol("JNE $+5");
  ol("DB 0E9H"); /* force a long jump the hard way */
  ot("DW ");
  printlabel(label);
  outstr("-$-2");
  nl();
  }
 
/*
** call specified subroutine name
*/
call(sname) char *sname; {
  ot("CALL ");
  outname(sname);
  nl();
  }
 
/*
** return from subroutine
*/
ret() {
  ol("POP BP"); /* restores caller's stack frame pointer */
  ol("RET"); /* return to caller */
  }
 
/*
** perform subroutine call to value on the stack
*/
callstk() {
  ol("POP AX");
  ol("CALL AX");
  csp=csp+BPW;
  }
 
/*
** jump to internal label number
*/
jump(label) int label; {
  ot("JMP ");
  printlabel(label);
  nl();
  }
 
/*
** test primary register and jump if false
*/
testjump(label) int label; {
  ol("AND AX,AX");
  ol("JNZ $+5");
  jump(label);
  }
 
/*
** test primary register against zero and jump if false
*/
zerojump(oper, label, lval) int oper, label, lval[]; {
  clearstage(lval[7], 0); /* purge conventional code */
  oper(label);
  }
 
/*
** define storage according to size
*/
defstorage(size) int size; {
  if(size==1) ot("DB ");
  else        ot("DW ");
  }
 
/*
** generate zeros for storage initialization (0,0, . . . or nnn DUP(0))
*/
genzeros(count) int count; {
  outdec(count);
  outstr(" DUP(0)");
  nl();
  return 0; /* would return >0 if not all zeros could be gened on one line */
  }
 
/*
** point to following object(s)
** (generated for initialization of static pointers, eg:
**   "char *x = &"hi there"
** I think)
*/
point() {
  ol("DW $+2");
  }
 
/*
** modify stack pointer to value given
*/
modstk(newsp) int newsp; {
  int k;
  k=newsp-csp;
  if(k==0) return newsp;
  if(newsp==0) {
    ol("MOV SP,BP");
    return newsp;
    }
  if(k>=0) {
    if(k<5) {
      if(k&1) {
        ol("INC SP");
        k--;
        }
      while(k) {
        ol("POP CX");
        k=k-BPW;
        }
      return newsp;
      }
    }
  if(k<0) {
    if(k>-5) {
      if(k&1) {
        ol("DEC SP");
        k++;
        }
      while(k) {
        ol("PUSH CX");
        k=k+BPW;
        }
      return newsp;
      }
    }
  ot("LEA SP,[BP]-");
  outdec(-newsp);
  nl();
  return newsp;
  }
 
/*
** double primary register
*/
doublereg() {ol("ADD AX,AX");}
 

