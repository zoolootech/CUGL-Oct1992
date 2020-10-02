/*
HEADER:		Small-C source part 9;
FILENAME:	CC9.C;
AUTHORS:	Dieter H. Flunkert;
COMPILERS:Turbo C V2.0 Medium Memory Model;
SYSTEM: MSDOS (Brian Brown, Nov 1989)
*/

#include <stdio.h>
#include "ccdef.c"

/* external function declarations */
extern int cout(), nl(), ol(), ot(), outstr(), streq();

/* external variable declaration */
extern int optimize, output;

#ifdef OPTIMIZE
peephole(ptr) char *ptr; {
 char val[6],*valptr;
 char val1[6];
 int i,addval;
 while(*ptr) {
 if(optimize) {
#ifdef TAB
 if(streq(ptr,"\tleay ")) {
  valptr = val; /* reset pointer */
  i=6;
  while(*(ptr+i) != ',') {*valptr++ = *(ptr+i);++i;}
  *valptr=0;
  addval=i+21;
  if(streq(ptr+i,",S\n\ttfr  y,d\n\tpshs d\n")) {
   if(streq(ptr+addval,"\tldd  [,s++]\n")) {
    ot("ldd  ");outstr(val);outstr(",s");nl();
    ptr=ptr+addval+13;
   }
   else if(streq(ptr+addval,"\tldd  #")) {
    addval=addval+7;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,"\tstd  [,s++]\n")) {
     ot("ldd  #");outstr(val1);nl();
     ot("std  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else if(streq(ptr+addval,"\tstb  [,s++]\n")) {
     ot("ldd  #");outstr(val1);nl();
     ot("stb  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,"\tldb  [,s++]\n")) {
    ot("ldb  ");outstr(val);outstr(",s");nl();
    ptr=ptr+addval+13;
   }
   else if(streq(ptr+addval,"\tleay ")) {
    addval=addval+6;
    valptr = val1;
    while(*(ptr+addval) != ',')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,
	",s\n\ttfr  y,d\n\tpshs d\n\tldd  [,d++]\n")) {
     ot("leay ");outstr(val);outstr(",d");nl();
     ol("pshs y");
     ot("ldd  ");outstr(val1);outstr(",s");nl();
     ptr=ptr+addval+34;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,"\tldb  #")) {
    addval=addval+7;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,"\tstb  [,s++]\n")) {
     ot("ldb  #");outstr(val1);nl();
     ot("stb  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,"\tlbsr ")) {
    addval=addval+6;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,"\tstd  [,s++]\n")) {
     ot("lbsr ");outstr(val1);nl();
     ot("std  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,
   "\tpshs d\n\tldd  [,s++]\n\taddd #")) {
    addval=addval+28;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,"\tstd  [,s++]\n")) {
     ot("ldd  ");outstr(val);outstr(",s");nl();
     ot("addd #");outstr(val1);nl();
     ot("std  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,
   "\tpshs d\n\tldd  [,s++]\n\tsubd #")) {
    addval=addval+28;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,"\tstd  [,s++]\n")) {
     ot("ldd  ");outstr(val);outstr(",s");nl();
     ot("subd #");outstr(val1);nl();
     ot("std  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else cout(*ptr++,output);
  }
  else cout(*ptr++,output);
 }
 else if(streq(ptr,"\tpshs d\n\tldd  #")) {
  valptr = val;
  i = 15;
  while(*(ptr+i) != '\n') {*valptr++ = *(ptr+i);++i;}
  *valptr=0;
  addval=i+12;
  if(streq(ptr+i,"\n\taddd ,s++\n")) {
   ot("addd #");outstr(val);nl();
   ptr=ptr+addval;
  }
  else if(streq(ptr+i,
  "\n\tpuls x\n\tpshs d\n\ttfr  x,d\n\tsubd ,s++\n")) {
   ot("subd #");outstr(val);nl();
   ptr=ptr+i+38;
  }
  else cout(*ptr++,output);
 }
 else cout(*ptr++,output);
#else
 if(streq(ptr," leay ")) {
  valptr = val; /* reset pointer */
  i=6;
  while(*(ptr+i) != ',') {*valptr++ = *(ptr+i);++i;}
  *valptr=0;
  addval=i+21;
  if(streq(ptr+i,",s\n tfr  y,d\n pshs d\n")) {
   if(streq(ptr+addval," ldd  [,s++]\n")) {
    ot("ldd  ");outstr(val);outstr(",s");nl();
    ptr=ptr+addval+13;
   }
   else if(streq(ptr+addval," ldd  #")) {
    addval=addval+7;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval," std  [,s++]\n")) {
     ot("ldd  #");outstr(val1);nl();
     ot("std  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else if(streq(ptr+addval," stb  [,s++]\n")) {
     ot("ldd  #");outstr(val1);nl();
     ot("stb  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval," ldb  [,s++]\n")) {
    ot("ldb  ");outstr(val);outstr(",s");nl();
    ptr=ptr+addval+13;
   }
   else if(streq(ptr+addval," leay ")) {
    addval=addval+6;
    valptr = val1;
    while(*(ptr+addval) != ',')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval,
    ",s\n tfr  y,d\n pshs d\n ldd  [,s++]\n")) {
     ot("leay ");outstr(val);outstr(",s");nl();
     ol("pshs y");
     ot("ldd  ");outstr(val1);outstr(",s");nl();
     ptr=ptr+addval+34;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval," ldb #")) {
    addval=addval+7;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval," stb  [,s++]\n")) {
     ot("ldb  #");outstr(val1);nl();
     ot("stb  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval," lbsr ")) {
    addval=addval+6;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval," std  [,s++]\n")) {
     ot("lbsr ");outstr(val1);nl();
     ot("std  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,
   " pshs d\n ldd  [,s++]\n addd #")) {
    addval=addval+28;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval," std  [,s++]\n")) {
     ot("ldd ");outstr(val);outstr(",s");nl();
     ot("addd #");outstr(val1);nl();
     ot("std  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else if(streq(ptr+addval,
   " pshs d\n ldd  [,s++]\n subd #")) {
    addval=addval+28;
    valptr = val1;
    while(*(ptr+addval) != '\n')
     {*valptr++ = *(ptr+addval); ++addval;}
    *valptr=0;
    ++addval;
    if(streq(ptr+addval," std  [,s++]\n")) {
     ot("ldd  ");outstr(val);outstr(",s");nl();
     ot("subd #");outstr(val1);nl();
     ot("std  ");outstr(val);outstr(",s");nl();
     ptr=ptr+addval+13;
    }
    else cout(*ptr++,output);
   }
   else cout(*ptr++,output);
  }
  else cout(*ptr++,output);
 }
 else if(streq(ptr," pshs d\n ldd  #")) {
  valptr = val;
  i = 15;
  while(*(ptr+i) != '\n') {*valptr++ = *(ptr+i);++i;}
  *valptr=0;
  addval=i+12;
  if(streq(ptr+i,"\n addd ,s++\n")) {
   ot("addd #");outstr(val);nl();
   ptr=ptr+addval;
  }
  else if(streq(ptr+i,
  "\n puls x\n pshs d\n tfr  x,d\n subd ,s++\n")) {
   ot("subd #");outstr(val);nl();
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
