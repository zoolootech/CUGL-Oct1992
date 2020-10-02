hier13(lval)  int lval[];  {
  int k;
  char *ptr;
  if(match("++")) {                   /* ++lval */
    if(hier13(lval)==0) {
      needlval();
      return 0;
      }
    step(inc, lval);
    return 0;
    }
  else if(match("--")) {              /* --lval */
    if(hier13(lval)==0) {
      needlval();
      return 0;
      }
    step(dec, lval);
    return 0;
    }
  else if (match("~")) {              /* ~ */
    if(hier13(lval)) rvalue(lval);
    com();
    lval[4] = ~lval[4];
    return (lval[7]=0);
    }
  else if (match("!")) {              /* ! */
    if(hier13(lval)) rvalue(lval);
    lneg();
    lval[4] = !lval[4];
    return (lval[7]=0);
    }
  else if (match("-")) {              /* unary - */
    if(hier13(lval)) rvalue(lval);
    neg();
    lval[4] = -lval[4];
    return (lval[7]=0);
    }
  else if(match("*")) {               /* unary * */
    if(hier13(lval)) rvalue(lval);
    if(ptr=lval[0])lval[1]=ptr[TYPE];
    else lval[1]=CINT;
    lval[2]=0;  /* flag as not pointer or array */
    lval[3]=0;  /* flag as not constant */
    lval[4]=1;  /* omit rvalue() on func call */ /*50*/
    return 1;
    }
  else if(match("&")) {               /* unary & */
    if(hier13(lval)==0) {
      error("illegal address");
      return 0;
      }
    ptr=lval[0];
    lval[2]=ptr[TYPE];
    if(lval[1]) return 0;
    /* global & non-array */
    address(ptr);
    lval[1]=ptr[TYPE];
    return 0;
    }
  else {
    k=hier14(lval);
    if(match("++")) {                 /* lval++ */
      if(k==0) {
        needlval();
        return 0;
        }
      step(inc, lval);
      dec(lval[2]>>2);
      return 0;
      }
    else if(match("--")) {            /* lval-- */
      if(k==0) {
        needlval();
        return 0;
        }
      step(dec, lval);
      inc(lval[2]>>2);
      return 0;
      }
    else return k;
    }
  }

hier14(lval)  int *lval; {
  int k, const, val, lval2[8], mdval;			/* fas 2.6 */
  char *ptr, *before, *start;
  k=primary(lval);
  ptr=lval[0];
  blanks();
  if((ch=='[')|(ch=='(')) {
    lval[5]=1;    /* secondary register will be used */
    while(1) {
      if(match("[")) {                /* [subscript] */
        if(ptr==0) {
          error("can't subscript");
          junk();
          needtoken("]");
          return 0;
          }
        else if(ptr[IDENT]==POINTER) rvalue(lval);
        else if(ptr[IDENT]!=ARRAY) {
          error("can't subscript");
          k=0;
          }
        if(arrptr = (ptr[AINDEX] * ARRINDEX * BPW)){	/* fas 2.6 */
          arrptr += sarrptr;				/* fas 2.6 */
          arrcount = 0;					/* fas 2.6 */
        }else{						/* fas 2.6 */
          arrptr = sarrptr;				/* fas 2.6 */
        }
        do{						/* fas 2.6 */
          setstage(&before, &start);
          lval2[3]=0;
          plnge2(0, 0, hier1, lval2, lval2); /* lval2 deadend */
          needtoken("]");
          if(lval2[3]) {
            clearstage(before, 0);
            if(lval2[4]) {
              if(ptr[TYPE]==CINT) const2(lval2[4]<<LBPW);
              else                const2(lval2[4]);
              if(mdval = arrptr[arrcount])		/* fas 2.6 */
                 getmd(mdval);				/* fas 2.6 */
              ffadd();
              }
            }
          else {
            if(ptr[TYPE]==CINT) doublereg();
            if(mdval = arrptr[arrcount]){		/* fas 2.6 */
               swap2();					/* fas 2.6 */
               getmd(mdval);				/* fas 2.6 */
               }
            ffadd();
            }
        ++arrcount;					/* fas 2.6 */
      }while(match("["));				/* fas 2.6 */
        lval[2]=0;                                 /*15*/
        lval[1]=ptr[TYPE];
        k=1;
        }
      else if(match("(")) {           /* function(...) */
        if(ptr==0) callfunction(0);
        else if(ptr[IDENT]!=FUNCTION) {
          if(k && !lval[4]) rvalue(lval);  /*13*//*14*//*50*/
          callfunction(0);
          }
        else callfunction(ptr);
        k=lval[0]=lval[3]=lval[4]=0;       /*50*/
        }
      else return k;
      }
    }
  if(ptr==0) return k;
  if(ptr[IDENT]==FUNCTION) {
    address(ptr);
    lval[0]=0;                              /*14*/
    return 0;
    }
  return k;
  }

primary(lval)  int *lval; {
  char *ptr, sname[NAMESIZE];           /*19*/
  int k;
  if(match("(")) {          /* (expression,...) */
    do k=hier1(lval); while(match(","));    /*26*/
    needtoken(")");
    return k;
    }
  putint(0, lval, 8<<LBPW); /* clear lval array */
  if(symname(sname, YES)) {             /*19*/
    if(ptr=findloc(sname)) {            /*19*/
#ifdef STGOTO
      if(ptr[IDENT]==LABEL) {
        experr();
        return 0;
        }
#endif
      getloc(ptr);
      lval[0]=ptr;
      lval[1]=ptr[TYPE];
      if(ptr[IDENT]==POINTER) {
        lval[1]=CINT;
        lval[2]=ptr[TYPE];
        }
      if(ptr[IDENT]==ARRAY) {
        lval[2]=ptr[TYPE];
        return 0;
        }
      else return 1;
      }
    if(ptr=findglb(sname))               /*19*/
      if(ptr[IDENT]!=FUNCTION) {
        lval[0]=ptr;
        lval[1]=0;
        if(ptr[IDENT]!=ARRAY) {
          if(ptr[IDENT]==POINTER) lval[2]=ptr[TYPE];
          return 1;
          }
        address(ptr);
        lval[1]=lval[2]=ptr[TYPE];
        return 0;
        }
    ptr=addsym(sname, FUNCTION, CINT, 0, 0, 0, 0, &glbptr, AUTOEXT);
						/* fas 2.3 2.6 */
                                           /*19*//*37*/
    lval[0]=ptr;
    lval[1]=0;
    return 0;
    }
  if(constant(lval)==0) experr();
  return 0;
  }

experr() {
  error("invalid expression");
  const(0);
  junk();
  }
 
callfunction(ptr)  char *ptr; { /* symbol table entry or 0 */
  int nargs, const, val;
  nargs=0;
  blanks();               /* already saw open paren */
                                     /*36*/
  while(streq(lptr,")")==0) {
    if(endst()) break;
    if(ptr) {                        /*36*/
      expression(&const, &val);      /*36*/
      push();                        /*36*/
      }                              /*36*/
    else {                           /*36*/
      push();                        /*36*/
      expression(&const, &val);      /*36*/
      swapstk();                     /*36*/
      }                              /*36*/
    nargs=nargs+BPW;      /* count args*BPW */
    if (match(",")==0) break;
    }
  needtoken(")");
  if(streq(ptr+NAME, "CCARGC")==0) loadargc(nargs>>LBPW);
  if(ptr) ffcall(ptr+NAME);
  else callstk();
  csp=modstk(csp+nargs, YES);
  }

