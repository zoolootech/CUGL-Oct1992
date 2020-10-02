/*
** peephole optimizer
**
** basically a simple text replacement processor
*/
 
char nooptimize = 0; /* this flag disables optimization if set (by debug) */
 
peephole(ptr) char *ptr; {
  while(*ptr) {
    char *ptr1;
    ptr1 = ptr; /* save for later compare */
    /*
    ** if optimization is disabled, skip everything
    ** all patterns start with tab, so look for one
    */
    while(*ptr && (*ptr != '\t'|| nooptimize)) cout(*ptr++,output);
    if(streq(ptr, "\tLEA AX,[BP]")) ptr = optim1(ptr, 12);
    if(streq(ptr, "\tMOV AX,1\n")) ptr = optim3(ptr, 10);
    if(streq(ptr, "\tMOV AX,")) ptr = optim4(ptr, 8);
    if(streq(ptr, "\tPUSH AX\n\tMOV AX,")) ptr = optim5(ptr, 17);
    if(streq(ptr, "\tPUSH AX\n\tLEA AX,[BP]")) ptr = optim6(ptr, 21);
    if(streq(ptr, "\tMOV BX,DX\n\tPOP BX\n")) ptr = optim7(ptr, 19);
    if(streq(ptr, "\tMOV BX,")) ptr = optim8(ptr, 8);
    if(streq(ptr, "\tJNZ $+5\n")) ptr = optim9(ptr+9, "JNZ $+");
    if(streq(ptr, "\tJZ $+5\n")) ptr = optim9(ptr+8, "JZ $+");
    /* additional optimizing logic goes here  */
    if(ptr == ptr1) cout(*ptr++, output);
    }
  }
 
/*
** optimize strings beginning "\tLEA AX,[BP]"
*/
optim1(ptr, bump) char *ptr; int bump; {
  char offset[20], *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = offset;
  while((*ptr2++ = *ptr1++) > ' ') ++bump; /* save offset from LEA */
  *--ptr2 = 0;
  ++bump;
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tMOV BX,AX\n\tMOV SI,AX\n\tMOV AX,[SI]\n"))
    ptr = optim11(ptr, bump+35, offset);
  else if(streq(ptr1, "\tMOV SI,AX\n\tMOV AX,[SI]\n"))
    ptr = optim12(ptr, bump+24, offset);
  else if(streq(ptr1, "\tMOV SI,AX\n\tMOV AL,[SI]\n")) {
    ot("MOV AL,[BP]");
    outstr(offset);
    outstr(" ;optim1 - 1");
    nl();
    ptr = ptr1 + 24;
    }
  else if(streq(ptr1, "\tPUSH AX\n\tLEA AX,[BP]"))
    ptr = optim13(ptr, bump+21, offset);
  else if(streq(ptr1, "\tPUSH AX\n\tMOV AX,"))
    ptr = optim14(ptr, bump+17, offset);
  else if(streq(ptr1, "\tPUSH AX\n\tMOV SI,AX\n"))
    ptr = optim15(ptr, bump+20, offset);
  return ptr;
  }
 
/*
** optimize strings beginning
**   "\tLEA AX,[BP]xxxx\n\tMOV BX,AX\n\tMOV SI,AX\n\tMOV AX,[SI]\n"
*/
optim11(ptr, bump, offset) char *ptr, offset[]; int bump; {
  char *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tINC AX\n\tMOV [BX],AX\n"))
    ptr = optim111(ptr, bump+21, offset);
  else if(streq(ptr1, "\tDEC AX\n\tMOV [BX],AX\n"))
    ptr = optim112(ptr, bump+21, offset);
  return ptr;
  }
 
/*
** optimize strings beginning
**   "\tLEA AX,[BP]xxxx\n\tMOV BX,AX\n\tMOV SI,AX\n\tMOV AX,[SI]\n"
**   "\tINC AX\n\tMOV [BX],AX\n"
*/
optim111(ptr, bump, offset) char *ptr, offset[]; int bump; {
  char *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tDEC AX\n\tMOV SI,AX\n\tMOV AL,[SI]\n")) {
    if(ptr1[32]) { /* if not end of expression */
      ot("MOV BX,[BP]");
      outstr(offset);
      nl();
      ot("INC WORD PTR [BP]");
      outstr(offset);
      nl();
      ol("MOV AL,[BX] ;optim111 - 1");
      ptr = ptr1 + 32;
      }
    else
      ptr = opti1111(ptr, bump+32, offset);
    }
  else if(streq(ptr1, "\tDEC AX\n")) {
    if(ptr1[8]) { /* if not end of expression */
      ot("MOV AX,[BP]");
      outstr(offset);
      nl();
      ot("INC WORD PTR [BP]");
      outstr(offset);
      outstr(" ;optim111 - 2");
      nl();
      ptr = ptr1 + 8;
      }
    else
      ptr = opti1111(ptr, bump+8, offset);
    }
  else if(streq(ptr1, "\tMOV SI,AX\n\tMOV AL,[SI]\n")) {
    if(ptr1[8]) { /* if not end of expression */
      ot("INC WORD PTR [BP]");
      outstr(offset);
      nl();
      ot("MOV BX,[BP]");
      outstr(offset);
      nl();
      ol("MOV AL,[BX] ;optim111 - 3");
      ptr = ptr1 + 24;
      }
    else
      ptr = opti1111(ptr, bump+24, offset);
    }
  else {
    if(ptr1[0]) { /* if not end of expression */
      ot("INC WORD PTR [BP]");
      outstr(offset);
      nl();
      ot("MOV AX,[BP]");
      outstr(offset);
      outstr(" ;optim111 - 4");
      nl();
      ptr = ptr1;
      }
    else
      ptr = opti1111(ptr, bump, offset);
    }
  return ptr;
  }
 
/*
** optimize "++" operators on simple auto variables with no destination
*/
opti1111(ptr, bump, offset) char *ptr, offset[]; int bump; {
  ot("INC WORD PTR [BP]");
  outstr(offset);
  outstr(" ;opti1111 - 1");
  nl();
  return ptr + bump;
  }
 
/*
** optimize strings beginning
**   "\tLEA AX,[BP]xxxx\n\tMOV BX,AX\n\tMOV SI,AX\n\tMOV AX,[SI]\n"
**   "\tDEC AX\n\tMOV [BX],AX\n"
*/
optim112(ptr, bump, offset) char *ptr, offset[]; int bump; {
  char *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tINC AX\n\tMOV SI,AX\n\tMOV AL,[SI]\n")) {
    if(ptr1[32]) { /* if not end of expression */
      ot("MOV BX,[BP]");
      outstr(offset);
      nl();
      ot("DEC WORD PTR [BP]");
      outstr(offset);
      nl();
      ol("MOV AL,[BX] ;optim112 - 1");
      ptr = ptr1 + 32;
      }
    else
      ptr = opti1121(ptr, bump+32, offset);
    }
  else if(streq(ptr1, "\tINC AX\n")) {
    if(ptr1[8]) { /* if not end of expression */
    ot("MOV AX,[BP]");
    outstr(offset);
    nl();
    ot("DEC WORD PTR [BP]");
    outstr(offset);
    outstr(" ;optim112 - 2");
    nl();
    ptr = ptr1 + 8;
      }
    else
      ptr = opti1121(ptr, bump+8, offset);
    }
  else if(streq(ptr1, "\tMOV SI,AX\n\tMOV AL,[SI]\n")) {
    if(ptr1[24]) { /* if not end of expression */
      ot("DEC WORD PTR [BP]");
      outstr(offset);
      nl();
      ot("MOV BX,[BP]");
      outstr(offset);
      nl();
      ol("MOV AL,[BX] ;optim112 - 3");
      ptr = ptr1 + 24;
      }
    else
      ptr = opti1121(ptr, bump+24, offset);
    }
  else {
    if(ptr1[0]) { /* if not end of expression */
      ot("DEC WORD PTR [BP]");
      outstr(offset);
      nl();
      ot("MOV AX,[BP]");
      outstr(offset);
      outstr(" ;optim112 - 4");
      nl();
      ptr = ptr1;
      }
    else
      ptr = opti1121(ptr, bump, offset);
    }
  return ptr;
  }
 
/*
** optimize "--" operators on simple auto variables with no destination
*/
opti1121(ptr, bump, offset) char *ptr, offset[]; int bump; {
  ot("DEC WORD PTR [BP]");
  outstr(offset);
  outstr(" ;opti1121 - 1");
  nl();
  return ptr + bump;
  }
 
/*
** optimize strings beginning
**   "\tLEA AX,[BP]xxxx\n\tMOV SI,AX\n\tMOV AX,[SI]\n"
*/
optim12(ptr, bump, offset) char *ptr, offset[]; int bump; {
  char *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tMOV SI,AX\n\tMOV AX,[SI]\n")) {
    ot("MOV SI,[BP]");
    outstr(offset);
    nl();
    ol("MOV AX,[SI] ;optim12 - 1");
    ptr = ptr1 + 24;
    }
  else if(streq(ptr1, "\tMOV SI,AX\n\tMOV AL,[SI]\n\tCBW\n\tAND AX,AX\n")) {
    ot("MOV SI,[BP]");
    outstr(offset);
    nl();
    ol("CMP BYTE PTR [SI],0 ;optim12 - 2");
    ptr = ptr1 + 40;
    }
  else if(streq(ptr1, "\tMOV SI,AX\n\tMOV AL,[SI]\n")) {
    ot("MOV SI,[BP]");
    outstr(offset);
    nl();
    ol("MOV AL,[SI] ;optim12 - 3");
    ptr = ptr1 + 24;
    }
  else {
    ot("MOV AX,[BP]");
    outstr(offset);
    outstr(" ;optim12 - 4");
    nl();
    ptr = ptr1;
    }
  return ptr;
  }
 
/*
** optimize strings beginning "\tLEA AX,[BP]xxxx\n\tPUSH AX\n\tLEA AX,[BP]"
*/
optim13(ptr, bump, offset) char *ptr, offset[]; int bump; {
  char offset2[20], *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = offset2;
  while((*ptr2++ = *ptr1++) > ' ') ++bump; /* save source from LEA */
  *--ptr2 = 0;
  ++bump;
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tMOV SI,AX\n\tMOV AX,[SI]\n\tPOP BX\n\tMOV [BX],AX\n")) {
    ot("MOV AX,[BP]");
    outstr(offset2);
    nl();
    ot("MOV [BP]");
    outstr(offset);
    outstr(",AX ;optim13 - 1");
    nl();
    ptr = ptr1 + 45;
    }
  else if(streq(ptr1,
      "\tMOV SI,AX\n\tMOV AL,[SI]\n\tCBW\n\tPOP BX\n\tMOV [BX],AL\n")) {
    ot("MOV AL,[BP]");
    outstr(offset2);
    nl();
    ol("CBW");
    ot("MOV [BP]");
    outstr(offset);
    outstr(",AL ;optim13 - 2");
    nl();
    ptr = ptr1 + 50;
    }
  return ptr;
  }
 
/*
** optimize strings beginning "\tLEA AX,[BP]xxxx\n\tPUSH AX\n\tMOV AX,"
*/
optim14(ptr, bump, offset) char *ptr, offset[]; int bump; {
  char offset2[20], *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = offset2;
  while((*ptr2++ = *ptr1++) > ' ') ++bump; /* save source from MOV */
  *--ptr2 = 0;
  ++bump;
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tPOP BX\n\tMOV [BX],AX\n")) {
    ot("MOV AX,");
    outstr(offset2);
    nl();
    ot("MOV [BP]");
    outstr(offset);
    outstr(",AX ;optim14 - 1");
    nl();
    ptr = ptr1 + 21;
    }
  else if(streq(ptr1, "\tPOP BX\n\tMOV [BX],AL\n")) {
    ot("MOV AX,");
    outstr(offset2);
    nl();
    ot("MOV [BP]");
    outstr(offset);
    outstr(",AL ;optim14 - 2");
    nl();
    ptr = ptr1 + 21;
    }
  return ptr;
  }
 
/*
** optimize strings beginning
**   "\tLEA AX,[BP]xxxx\n\tPUSH AX\n\tMOV SI,AX\n"
*/
optim15(ptr, bump, offset) char *ptr, offset[]; int bump; {
  char *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tMOV AX,[SI]\n")) {
    ot("LEA SI,[BP]");
    outstr(offset);
    nl();
    ol("PUSH SI");
    ol("MOV AX,[SI] ;optim15 - 1");
    ptr = ptr1 + 13;
    }
  else if(streq(ptr1, "\tMOV AL,[SI]\n")) {
    ot("LEA SI,[BP]");
    outstr(offset);
    nl();
    ol("PUSH SI");
    ol("MOV AL,[SI] ;optim15 - 2");
    ptr = ptr1 + 13;
    }
  return ptr;
  }
 
/*
** optimize strings beginning "\tMOV AX,1\n"
*/
optim3(ptr, bump) char *ptr; int bump; {
  char *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tJE $+3\n\tDEC AX\n\tAND AX,AX\n\tJNZ $+5\n") |
      streq(ptr1, "\tJZ $+3\n\tDEC AX\n\tAND AX,AX\n\tJNZ $+5\n")) {
    ol(";optim3 - 1");
    ptr = optim9(ptr1+36, "JZ $+");
    }
  else if(streq(ptr1, "\tJNE $+3\n\tDEC AX\n\tAND AX,AX\n\tJNZ $+5\n") |
      streq(ptr1, "\tJNZ $+3\n\tDEC AX\n\tAND AX,AX\n\tJNZ $+5\n")) {
    ol(";optim3 - 1B");
    ptr = optim9(ptr1+37, "JNZ $+");
    }
  else if(streq(ptr1, "\tJG $+3\n\tDEC AX\n\tAND AX,AX\n\tJNZ $+5\n")) {
    ol(";optim3 - 2");
    ptr = optim9(ptr1+36, "JG $+");
    }
  else if(streq(ptr1, "\tJL $+3\n\tDEC AX\n\tAND AX,AX\n\tJNZ $+5\n")) {
    ol(";optim3 - 3");
    ptr = optim9(ptr1+36, "JL $+");
    }
  else if(streq(ptr1, "\tJGE $+3\n\tDEC AX\n\tAND AX,AX\n\tJNZ $+5\n")) {
    ol(";optim3 - 4");
    ptr = optim9(ptr1+37, "JGE $+");
    }
  else if(streq(ptr1, "\tJLE $+3\n\tDEC AX\n\tAND AX,AX\n\tJNZ $+5\n")) {
    ol(";optim3 - 5");
    ptr = optim9(ptr1+37, "JLE $+");
    }
  return ptr;
  }
 
/*
** optimize strings beginning "\tMOV AX,"
*/
optim4(ptr, bump) char *ptr; int bump; {
  char offset[20], *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = offset;
  while((*ptr2++ = *ptr1++) > ' ') ++bump; /* save source from MOV */
  *--ptr2 = 0;
  ++bump;
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tINC AX\n\tMOV ")) ptr = optim41(ptr, bump+13, offset);
  else if(streq(ptr1, "\tDEC AX\n\tMOV ")) ptr = optim42(ptr, bump+13, offset);
  else if(streq(ptr1, "\tMOV BX,")) ptr = optim43(ptr, bump+8, offset);
  return ptr;
  }
 
/*
** optimize strings beginning "\tMOV AX,xxxx\n\tINC AX\n\tMOV "
*/
optim41(ptr, bump, offset) char *ptr, offset[]; int bump; {
  char *ptr1, *ptr2, *ptr3;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, offset)) {
    ptr3 = offset;
    while(*ptr3++) {++ptr1; ++bump;} /* skip over value */
    if(streq(ptr1, ",AX\n\tDEC AX\n")) {
      if(ptr1[12]) {
        ot("MOV AX,");
        outstr(offset);
        nl();
        ot("INC ");
        outstr(offset);
        outstr(" ;optim41 - 1");
        nl();
        ptr = ptr1 + 12;
        }
      else ptr = optim411(ptr, bump+12, offset);
      }
    else if(streq(ptr1, ",AX\n")) {
      if(ptr1[4]) {
        ot("INC ");
        outstr(offset);
        nl();
        ot("MOV AX,");
        outstr(offset);
        outstr(" ;optim41 - 2");
        nl();
        ptr = ptr1 + 4;
        }
      else ptr = optim411(ptr, bump+4, offset);
      }
    }
  return ptr;
  }
 
/*
** optimize "++" operators on simple static variables with no destination
*/
optim411(ptr, bump, offset) char *ptr, offset[]; int bump; {
  ot("INC ");
  outstr(offset);
  outstr(" ;optim411 - 1");
  nl();
  return ptr + bump;
  }
 
/*
** optimize strings beginning "\tMOV AX,xxxx\n\tDEC AX\n\tMOV "
*/
optim42(ptr, bump, offset) char *ptr, offset[]; int bump; {
  char *ptr1, *ptr2, *ptr3;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, offset)) {
    ptr3 = offset;
    while(*ptr3++) {++ptr1; ++bump;} /* skip over value */
    if(streq(ptr1, ",AX\n\tINC AX\n")) {
      if(ptr1[12]) {
        ot("MOV AX,");
        outstr(offset);
        nl();
        ot("DEC ");
        outstr(offset);
        outstr(" ;optim42 - 1");
        nl();
        ptr = ptr1 + 12;
        }
      else ptr = optim421(ptr, bump+12, offset);
      }
    else if(streq(ptr1, ",AX\n")) {
      if(ptr1[4]) {
        ot("DEC ");
        outstr(offset);
        nl();
        ot("MOV AX,");
        outstr(offset);
        outstr(" ;optim42 - 2");
        nl();
        ptr = ptr1 + 4;
        }
      else ptr = optim421(ptr, bump+4, offset);
      }
    }
  return ptr;
  }
 
/*
** optimize "--" operators on simple auto variables with no destination
*/
optim421(ptr, bump, offset) char *ptr, offset[]; int bump; {
  ot("DEC ");
  outstr(offset);
  outstr(" ;optim421 - 1");
  nl();
  return ptr + bump;
  }
 
/*
** optimize strings beginning "\tMOV AX,xxxx\n\tMOV BX,"
*/
optim43(ptr, bump, offset) char *ptr, offset[]; int bump; {
  char offset2[20], *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = offset2;
  while((*ptr2++ = *ptr1++) > ' ') ++bump; /* save source from MOV */
  *--ptr2 = 0;
  ++bump;
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\n\tADD AX,BX\n\tMOV SI,AX\n\tMOV AX,[SI]\n")) {
    ot("MOV BX,");
    outstr(offset);
    nl();
    ot("ADD BX,");
    outstr(offset2);
    nl();
    ol("MOV AX,[BX] ;optim43 - 1");
    ptr = ptr1 + 36;
    }
  return ptr; /* "MOV BX," may be separately optimizable */
  }
 
/*
** optimize strings beginning "\tPUSH AX\n\tMOV AX,"
*/
optim5(ptr, bump) char *ptr; int bump; {
  char offset[20], *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = offset;
  while((*ptr2++ = *ptr1++) > ' ') ++bump; /* save source from MOV */
  *--ptr2 = 0;
  ++bump;
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tPOP BX\n\tADD AX,BX\n")) {
    ot("ADD AX,");
    outstr(offset);
    outstr(" ;optim5 - 1");
    nl();
    ptr = ptr1 + 19;
    }
  else if(streq(ptr1, "\tPOP BX\n\tSUB AX,BX\n\tNEG AX\n")) {
    ot("SUB AX,");
    outstr(offset);
    outstr(" ;optim5 - 2");
    nl();
    ptr = ptr1 + 27;
    }
  else if(streq(ptr1, "\tPOP BX\n\tSUB AX,BX\n")) {
    ot("SUB AX,");
    outstr(offset);
    nl();
    ol("NEG AX ;optim5 - 3");
    ptr = ptr1 + 19;
    }
  else if(streq(ptr1, "\tPOP BX\n\tCMP BX,AX\n")) {
    ot("CMP AX,");
    outstr(offset);
    outstr(" ;optim5 - 4");
    nl();
    ptr = ptr1 + 19;
    }
  else if(streq(ptr1, "\tPOP BX\n\tXCHG AX,BX\n")) {
    ot("MOV BX,");
    outstr(offset);
    outstr(" ;optim5 - 5");
    nl();
    ptr = ptr1 + 20;
    }
  else if(streq(ptr1, "\tPOP BX\n")) {
    ol("MOV BX,AX");
    ot("MOV AX,");
    outstr(offset);
    outstr(" ;optim5 - 6");
    nl();
    ptr = ptr1 + 8;
    }
  return ptr;
  }
 
/*
** optimize strings beginning "\tPUSH AX\n\tLEA AX,[BP]"
*/
optim6(ptr, bump) char *ptr; int bump; {
  char offset[20], *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = offset;
  while((*ptr2++ = *ptr1++) > ' ') ++bump; /* save offset from LEA */
  *--ptr2 = 0;
  ++bump;
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tMOV SI,AX\n\tMOV AX,[SI]\n\tPOP BX\n")) {
    ol("MOV BX,AX");
    ot("MOV AX,[BP]");
    outstr(offset);
    outstr(" ;optim6 - 1");
    nl();
    ptr = ptr1 + 32;
    }
  return ptr;
  }
 
/*
** optimize strings beginning "\tMOV BX,DX\n\tPOP BX\n"
*/
optim7(ptr, bump) char *ptr; int bump; {
  ol(";optim7 - 1");
  return ptr + 11; /* just skip over the useless part */
  }
 
/*
** optimize strings beginning "\tMOV BX,"
*/
optim8(ptr, bump) char *ptr; int bump; {
  char offset[20], *ptr1, *ptr2;
  ptr1 = ptr + bump; /* skip text already recognized */
  ptr2 = offset;
  while((*ptr2++ = *ptr1++) > ' ') ++bump; /* save offset from LEA */
  *--ptr2 = 0;
  ++bump;
  ptr2 = ptr; /* save for compare */
  if(streq(ptr1, "\tADD AX,BX\n")) {
    ot("ADD AX,");
    outstr(offset);
    outstr(" ;optim8 - 1");
    nl();
    ptr = ptr1 + 11;
    }
  else if(streq(ptr1, "\tSUB AX,BX\n")) {
    ot("SUB AX,");
    outstr(offset);
    outstr(" ;optim8 - 2");
    nl();
    ptr = ptr1 + 11;
    }
  return ptr;
  }

