/* <<<<<   Start of cc9    >>>>> */
#ifdef OPTIMIZE
peephole(ptr) char *ptr; {
 char val[6],*valptr;
 char val1[6];
 int i,addval;
 while(*ptr) {
 if(optimize) {
#ifdef TAB
 if(streq(ptr,"\tLEAY ")) {
  valptr = val; /* reset pointer */
  i=6;
  while(*(ptr+i) != ',') {*valptr++ = *(ptr+i);++i;}
  *valptr=0;
  addval=i+21;
  if(streq(ptr+i,",S\n\tTFR  Y,D\n\tPSHS D\n")) {
   if(streq(ptr+addval,"\tLDD  [,S++]\n")) {
    ot("LDD  ");outstr(val);outstr(",S");nl();
    ptr=ptr+addval+13;
   }
   else if(streq(ptr+addval,"\tLDD  #")) {
    addval=addval+7;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,"\tSTD  [,S++]\n")) {
     ot("LDD  #");outstr(val1);nl();
     ot("STD  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    }
    else if(streq(ptr+addval,"\tSTB  [,S++]\n")) {
     ot("LDD  #");outstr(val1);nl();
     ot("STB  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    } 
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,"\tLDB  [,S++]\n")) {
    ot("LDB  ");outstr(val);outstr(",S");nl();
    ptr=ptr+addval+13;
   }
   else if(streq(ptr+addval,"\tLEAY ")) {
    addval=addval+6;
    valptr = val1;
    while(*(ptr+addval) != ',')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,
    ",S\n\tTFR  Y,D\n\tPSHS D\n\tLDD  [,S++]\n")) {
     ot("LEAY ");outstr(val);outstr(",S");nl();
     ol("PSHS Y");
     ot("LDD  ");outstr(val1);outstr(",S");nl();
     ptr=ptr+addval+34;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,"\tLDB  #")) {
    addval=addval+7;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,"\tSTB  [,S++]\n")) {
     ot("LDB  #");outstr(val1);nl();
     ot("STB  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,"\tLBSR ")) {
    addval=addval+6;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,"\tSTD  [,S++]\n")) {
     ot("LBSR ");outstr(val1);nl();
     ot("STD  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,
   "\tPSHS D\n\tLDD  [,S++]\n\tADDD #")) {
    addval=addval+28;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,"\tSTD  [,S++]\n")) {
     ot("LDD  ");outstr(val);outstr(",S");nl();
     ot("ADDD #");outstr(val1);nl();
     ot("STD  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,
   "\tPSHS D\n\tLDD  [,S++]\n\tSUBD #")) {
    addval=addval+28;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,"\tSTD  [,S++]\n")) {
     ot("LDD  ");outstr(val);outstr(",S");nl();
     ot("SUBD #");outstr(val1);nl();
     ot("STD  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else cout(*ptr++,output);
  }
  else cout(*ptr++,output);
 }
 else if(streq(ptr,"\tPSHS D\n\tLDD  #")) {
  valptr = val;
  i = 15;
  while(*(ptr+i) != '\n') {*valptr++ = *(ptr+i);++i;}
  *valptr=0;
  addval=i+12;
  if(streq(ptr+i,"\n\tADDD ,S++\n")) {
   ot("ADDD #");outstr(val);nl();
   ptr=ptr+addval;
  }
  else if(streq(ptr+i,
  "\n\tPULS X\n\tPSHS D\n\tTFR  X,D\n\tSUBD ,S++\n")) {
   ot("SUBD #");outstr(val);nl();
   ptr=ptr+i+38;
  }
  else cout(*ptr++,output);
 }
 else cout(*ptr++,output);
#else
 if(streq(ptr," LEAY ")) {
  valptr = val; /* reset pointer */
  i=6;
  while(*(ptr+i) != ',') {*valptr++ = *(ptr+i);++i;}
  *valptr=0;
  addval=i+21;
  if(streq(ptr+i,",S\n TFR  Y,D\n PSHS D\n")) {
   if(streq(ptr+addval," LDD  [,S++]\n")) {
    ot("LDD  ");outstr(val);outstr(",S");nl();
    ptr=ptr+addval+13;
   }
   else if(streq(ptr+addval," LDD  #")) {
    addval=addval+7;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval," STD  [,S++]\n")) {
     ot("LDD  #");outstr(val1);nl();
     ot("STD  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    }
    else if(streq(ptr+addval," STB  [,S++]\n")) {
     ot("LDD  #");outstr(val1);nl();
     ot("STB  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    } 
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval," LDB  [,S++]\n")) {
    ot("LDB  ");outstr(val);outstr(",S");nl();
    ptr=ptr+addval+13;
   }
   else if(streq(ptr+addval," LEAY ")) {
    addval=addval+6;
    valptr = val1;
    while(*(ptr+addval) != ',')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,
    ",S\n TFR  Y,D\n PSHS D\n LDD  [,S++]\n")) {
     ot("LEAY ");outstr(val);outstr(",S");nl();
     ol("PSHS Y");
     ot("LDD  ");outstr(val1);outstr(",S");nl();
     ptr=ptr+addval+34;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval," LDB #")) {
    addval=addval+7;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval," STB  [,S++]\n")) {
     ot("LDB  #");outstr(val1);nl();
     ot("STB  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval," LBSR ")) {
    addval=addval+6;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval," STD  [,S++]\n")) {
     ot("LBSR ");outstr(val1);nl();
     ot("STD  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,
   " PSHS D\n LDD  [,S++]\n ADDD #")) {
    addval=addval+28;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval," STD  [,S++]\n")) {
     ot("LDD ");outstr(val);outstr(",S");nl();
     ot("ADDD #");outstr(val1);nl();
     ot("STD  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,
   " PSHS D\n LDD  [,S++]\n SUBD #")) {
    addval=addval+28;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval," STD  [,S++]\n")) {
     ot("LDD  ");outstr(val);outstr(",S");nl();
     ot("SUBD #");outstr(val1);nl();
     ot("STD  ");outstr(val);outstr(",S");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else cout(*ptr++,output);
  }
  else cout(*ptr++,output);
 }
 else if(streq(ptr," PSHS D\n LDD  #")) {
  valptr = val;
  i = 15;
  while(*(ptr+i) != '\n') {*valptr++ = *(ptr+i);++i;}
  *valptr=0;
  addval=i+12;
  if(streq(ptr+i,"\n ADDD ,S++\n")) {
   ot("ADDD #");outstr(val);nl();
   ptr=ptr+addval;
  }
  else if(streq(ptr+i,
  "\n PULS X\n PSHS D\n TFR  X,D\n SUBD ,S++\n")) {
   ot("SUBD #");outstr(val);nl();
   ptr=ptr+i+38;
  }
  else cout(*ptr++,output);
 }
 else cout(*ptr++,output);
#endif
 }
 else cout(*ptr++,output);
 }
}
#endif

/* <<<<<  End of compiler  >>>>> */
