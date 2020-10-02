/*
** lval[0] - symbol table address, else 0 for constant
** lval[1] - type of indirect obj to fetch, else 0 for static
** lval[2] - type of pointer or array, else 0 for all other
** lval[3] - true if constant expression
** lval[4] - value of constant expression
** lval[5] - true if secondary register altered
** lval[6] - function address of highest/last binary operator
** lval[7] - stage address of "oper 0" code, else 0
*/
 
/*
** skim over terms adjoining || and && operators
*/
skim(opstr, testfunc, dropval, endval, heir, lval)
  char *opstr;
  int testfunc, dropval, endval, heir, lval[]; {
  int k, hits, droplab, endlab;
  hits=0;
  while(1) {
    k=plunge1(heir, lval);
    if(nextop(opstr))  {
      bump(opsize);
      if(hits==0)  {
        hits=1;
        droplab=getlabel();
        }
      dropout(k, testfunc, droplab, lval);
      }
    else if(hits)  {
      dropout(k, testfunc, droplab, lval);
      const(endval);
      jump(endlab=getlabel());
      postlabel(droplab);
      const(dropval);
      postlabel(endlab);
      lval[1]=lval[2]=lval[3]=lval[7]=0;
      return 0;
      }
    else return k;
    }
  }
 
/*
** test for early dropout from || or && evaluations
*/
dropout(k, testfunc, exit1, lval) int k, testfunc, exit1, lval[];  {
  if(k) rvalue(lval);
  else if(lval[3]) const(lval[4]);
  testfunc(exit1); /* jumps on false */
  }
 
/*
** plunge to a lower level
*/
plunge(opstr, opoff, heir, lval)
  char *opstr;
  int opoff, heir, lval[];  {
  int k, lval2[8];
  k=plunge1(heir, lval);
  if(nextop(opstr)==0) return k;
  if(k) rvalue(lval);
  while(1)  {
    if(nextop(opstr))  {
      bump(opsize);
      opindex=opindex+opoff;
      plunge2(op[opindex], op2[opindex], heir, lval, lval2);
      }
    else return 0;
    }
  }
 
/*
** unary plunge to lower level
*/
plunge1(heir, lval) int heir, lval[];  {
  char *before, *start;
  int k;
  setstage(&before, &start);
  k=heir(lval);
  if(lval[3]) clearstage(before,0); /* load constant later */
  return k;
  }
 
/*
** binary plunge to lower level
*/
plunge2(oper, oper2, heir, lval, lval2)
  int oper, oper2, heir, lval[], lval2[];  {
  char *before, *start;
  setstage(&before, &start);
  lval[5]=1;           /* flag secondary register used */
  lval[7]=0;           /* flag as not "... oper 0" syntax */
  if(lval[3])  {       /* constant on left side not yet loaded */
    if(plunge1(heir, lval2)) rvalue(lval2);
    if(lval[4]==0) lval[7]=stagenext;
    const2(lval[4]<<dbltest(lval2, lval));
    }
  else {  /* non-constant on left side */
    push();
    if(plunge1(heir, lval2)) rvalue(lval2);
    if(lval2[3]) {     /* constant on right side */
      if(lval2[4]==0) lval[7]=start;
      if(oper==add)  { /* may test other commutative operators */
        csp=csp+2;
        clearstage(before,0);
        const2(lval2[4]<<dbltest(lval, lval2)); /* load secondary */
        }
      else  {
        const(lval2[4]<<dbltest(lval, lval2));  /* load primary */
        pop();
        }
      }
    else {             /* non-constants on both sides */
      pop();
      if((oper==add)|(oper==sub)) {
        if(dbltest(lval,lval2)) doublereg();
        if(dbltest(lval2,lval)) {
          swap();
          doublereg();
          if(oper==sub) swap();
          }
        }
      }
    }
  if(oper) {
    if(lval[3]=lval[3]&lval2[3]) {
      lval[4]=calc(lval[4], oper, lval2[4]);
      clearstage(before, 0);
      lval[5]=0;
      }
    else {
      if((lval[2]==0)&(lval2[2]==0)) {
        oper();
        lval[6]=oper;   /* identify the operator */
        }
      else {
        oper2();
        lval[6]=oper2;  /* identify the operator */
        }
      }
    if(oper==sub) {
      if((lval[2]==CINT)&(lval2[2]==CINT)) {
        swap();
        const(1);
        asr();  /** div by 2 **/
        }
      }
    if((oper==sub)|(oper==add)) result(lval, lval2);
    }
  }
 
calc(left, oper, right) int left, oper, right; {
       if(oper ==  or) return (left  |  right);
  else if(oper == xor) return (left  ^  right);
  else if(oper == and) return (left  &  right);
  else if(oper ==  eq) return (left  == right);
  else if(oper ==  ne) return (left  != right);
  else if(oper ==  le) return (left  <= right);
  else if(oper ==  ge) return (left  >= right);
  else if(oper ==  lt) return (left  <  right);
  else if(oper ==  gt) return (left  >  right);
  else if(oper == asr) return (left  >> right);
  else if(oper == asl) return (left  << right);
  else if(oper == add) return (left  +  right);
  else if(oper == sub) return (left  -  right);
  else if(oper ==mult) return (left  *  right);
  else if(oper == div) return (left  /  right);
  else if(oper == mod) return (left  %  right);
  else return 0;
  }
 
expression(const, val) int *const, *val; {
  int lval[8];
  if(heir1(lval)) rvalue(lval); /* if only lvalue returned, fetch rvalue */
  if(lval[3]) { /* if result is a constant expression . . . */
    *const=1; /* say so */
    *val=lval[4]; /* return its value */
    }
  else *const=0; /* not a constant expression */
  }
 
/*
** expr ? expr : expr implemented per A. Macpherson letter, DDJ #81
** (UNIX flavor)
*/
heir1(lval)  int lval[];  { /* expect <lvalue> <assignment op> <rvalue> */
  int k,lval2[8], oper;
  k=plunge1(heir2, lval);
  if(lval[3]) const(lval[4]); /* if constant expression, generate the value */
       if(match("|="))  oper=or;
  else if(match("^="))  oper=xor;
  else if(match("&="))  oper=and;
  else if(match("+="))  oper=add;
  else if(match("-="))  oper=sub;
  else if(match("*="))  oper=mult;
  else if(match("/="))  oper=div;
  else if(match("%="))  oper=mod;
  else if(match(">>=")) oper=asr;
  else if(match("<<=")) oper=asl;
  else if(match("="))   oper=0;
  else return k; /* not followed by assignment operator */
  if(k==0) { /* lvalue must be to left of assignment operator:  post error */
    needlval(); /* "must be lvalue" */
    return 0;
    }
  if(lval[1]) { /* if lvalue is not static */
    if(oper) { /* and if operator is not pure assignment */
      push(); /* save the lvalue (address) */
      rvalue(lval); /* fetch the rvalue (value) */
      }
    plunge2(oper, oper, heir1, lval, lval2); /* evaluate right-hand side */
    if(oper) pop(); /* if operator is not pure assignment retrieve lvalue */
    }
  else { /* if lvalue is static */
    if(oper) { /* and if operator is not pure assignment */
      rvalue(lval); /* fetch the rvalue */
      plunge2(oper, oper, heir1, lval, lval2); /* evaluate right-hand side */
      }
    else { /* lvalue is static and operator is pure assignment */
      if(heir1(lval2)) rvalue(lval2); /* evaluate right-hand side */
      lval[5]=lval2[5]; /* copy sec reg status */
      }
    }
  store(lval); /* store the result */
  return 0;
  }
 
/*
** expr ? expr : expr implemented per A. Macpherson letter, DDJ #81
** (UNIX flavor)
*/
heir2(lval)  int lval[];  { /* expect <lvalue> <assignment op> <rvalue> */
  int k;
  k = plunge1(heir3, lval);
  if(match("?")) {
    int falselab, endlab, val;
    char reg2used, selconst, explconst, sel2;
    falselab = getlabel();
    reg2used = 0;
    if(k) rvalue(lval);
    if(selconst = lval[3]) { /* selector is a constant */
      if(sel2= lval[4]) jump(falselab);
      }
    else {
      testjump(falselab);
      reg2used = lval[5];
      }
    dropout(plunge1(heir3, lval), jump, endlab = getlabel(), lval);
    if(explconst = lval[3]) val = lval[4];
    else reg2used |= lval[5];
    postlabel(falselab);
    needtoken(":");
    if(plunge1(heir3, lval)) rvalue(lval);
    if(lval[3]) {
      const(lval[4]);
      if(selconst && sel2) val = lval[4];
      }
    else reg2used |= lval[5];
    lval[5] = reg2used;
    lval[4] = val;
    lval[3] = selconst && ((!sel2 && explconst) || (sel2 && lval[3]));
    postlabel(endlab);
    return 0;
    }
  return k;
  }
 
heir3(lval)  int lval[];  {
  return skim("||", eq0, 1, 0, heir4, lval);
  }
 
heir4(lval)  int lval[];  {
  return skim("&&", ne0, 0, 1, heir5, lval);
  }
 
heir5(lval)  int lval[];  {
  return plunge("|", 0, heir6, lval);
  }
 
heir6(lval)  int lval[];  {
  return plunge("^", 1, heir7, lval);
  }
 
heir7(lval)  int lval[];  {
  return plunge("&", 2, heir8, lval);
  }
 
heir8(lval)  int lval[];  {
  return plunge("== !=", 3, heir9, lval);
  }
 
heir9(lval)  int lval[];  {
  return plunge("<= >= < >", 5, heir10, lval);
  }
 
heir10(lval)  int lval[];  {
  return plunge(">> <<", 9, heir11, lval);
  }
 
heir11(lval)  int lval[];  {
  return plunge("+ -", 11, heir12, lval);
  }
 
heir12(lval)  int lval[];  {
  return plunge("* / %", 13, heir13, lval);
  }
 

