/*
** execution begins here
*/
#ifdef CMD_LINE
main(argc, argv) int argc, *argv; {
  argcs=argc;
  argvs=argv;
#else
main() {
#endif
  sout("Small-C Version 2.0 for the IBM PC -- Release 1.01\n", stderr);
  sout("Portions Copyright 1982 by J. E. Hendrix\n", stderr);
  sout("Converted to the IBM PC by D. R. Hicks\n", stderr);
  sout("\n", stderr);
#ifdef DYNAMIC
  swnext=malloc(SWTABSZ);
  swend=swnext+((SWTABSZ-SWSIZ)>>1);
  stage=malloc(STAGESIZE);
  stagelast=stage+STAGELIMIT;
  wq=malloc(WQTABSZ*BPW);
  litq=malloc(LITABSZ);
#ifdef HASH
  macn=malloc(MACNSIZE);
  cptr=macn-1;
  while(++cptr < MACNEND) *cptr=0;
#endif
  macq=malloc(MACQSIZE);
  pline=malloc(LINESIZE);
  mline=malloc(LINESIZE);
#else
  swend=(swnext=swq)+SWTABSZ-SWSIZ;
  stagelast=stage+STAGELIMIT;
#endif
  swactive=         /* not in switch                         */
  stagenext=        /* direct output mode                    */
  iflevel=          /* #if... nesting level =0               */
  skiplevel=        /* #if... not encountered                */
  macptr=           /* clear the macro pool                  */
  csp =             /* stack ptr (relative)                  */
  errcnt=           /* number of errors flagged              */
  errflag=0;        /* not skipping errors till ";"          */
  eof=              /* not eof yet                           */
  ncmp=             /* not in compound statement             */
  dmode=            /* not in DATASEG                        */
  files=
  filearg=
  nxtlab=
  quote[1]=0;
  ccode=1;          /* enable preprocessing                  */
  wqptr=wq;         /* clear while queue                     */
  quote[0]='"';     /* fake a quote literal                  */
  input=input2=EOF;
  ask();            /* get user options                      */
  sout("\n", stderr); /* blank line to separate options from listing */
  kill(); /* reset input buffer prior to first read */
  preprocess();     /* fetch first line                      */
#ifdef DYNAMIC
#ifdef HASH
  symtab=malloc(NUMLOCS*SYMAVG + NUMGLBS*SYMMAX);
#else
  symtab=malloc(NUMLOCS*SYMAVG);
  /* global space is allocated with each new entry           */
#endif
#endif
#ifdef HASH
  cptr=STARTGLB-1;
  while(++cptr < ENDGLB) *cptr=0;
#endif
  glbptr=STARTGLB;
  glbflag=1;
  header();         /* intro code */
  setops();         /* set values in op arrays */
  parse();          /* process ALL values */
  outside();        /* verify outside any function */
  externs();        /* declare all externals to the assembler */
  trailer();        /* follow-up code */
  if(output != stdout) fclose(output);
  summary();        /* print error summary */
  }
 
/*
** process all input text
**
** At this level, only static declarations.
**       defines, includes and function
**       definitions are legal ...
*/
parse() {
  while (eof==0) {
    if(amatch("extern", 6))       dodeclare(EXTERNAL);
    else if(amatch("static", 6))  dodeclare(STATIC);
    else if(dodeclare(PUBLIC))    ;
    else if(match("#asm"))        doasm();
    else if(match("#include"))    doinclude();
    else if(match("#define"))     addmac();
    else                          newfunc(PUBLIC);
    blanks();      /* force eof if pending                   */
    }
  }
 
/*
** dump the literal pool
*/
dumplits(size) int size;  {
  int j, k;
  k=0;
  while (k<litptr)  {
    defstorage(size);
    j=10;
    while(j--) {
      outdec(getint(litq+k, size));
      k=k+size;
      if ((j==0) | (k>=litptr))  {
        nl();
        break;
        }
      outbyte(',');
      }
    }
  }
/*
** dump zeroes for default initial values
*/
dumpzero(size, count) int size, count; {
  int j;
  while (count > 0) {
    defstorage(size);
    count = genzeros(count); /* generate literal zeros */
    }
  }
/*
** verify compile ends outside any function
*/
outside()  {
  if (ncmp) error("no closing bracket");
  }
 
/*
** get run options
*/
ask() {
  int defin, defout, deflist, defi, defm, defa, defp;
  int dftin, dftout, dftlist;
  char inname[81], outname[81], listname[81];
#ifdef CMD_LINE
  int argnum, truth;
  char *ptr;
#endif
  defin = defout = deflist = defi = defm = defa = defp = 0;
  dftin = dftout = dftlist = 1;
  ctext = monitor = alarm = pause = 1;
  input = stdin;
  output = stdout;
  listfp=0;
  strcpy(inname, "con.C");
  strcpy(outname, "con.ASM");
  strcpy(listname, "NUL.LST");
#ifdef CMD_LINE
  argnum = 1;
  while(argnum < argcs) {
    ptr = argvs[argnum++]; /* cast arg as char * */
    if(*ptr == ';') { /* if defaults to be taken */
      defin = defout = deflist = defi = defm = defa = defp = 1;
      break;
      }
    else if(*ptr != '-') { /* if file name */
      if(!defin) {
        if(*ptr != ',') {
          joinname(inname, ptr);
          dupename(outname, inname);
          dftin = 0;
          dftout = 0;
          }
        defin = 1;
        }
      else if(!defout) {
        if(*ptr != ',') {
          joinname(outname, ptr);
          dftout = 0;
          }
        defout = 1;
        }
      else if(!deflist) {
        if(*ptr != ',') {
          joinname(listname, ptr);
          dftlist = 0;
          }
        deflist = 1;
        }
      else {
        fprintf(stderr, "ERROR:  Extraneous parameter %s\n", ptr);
        errcnt++;
        }
      }
    else { /* must be option */
      ptr++; /* skip "-" */
      truth = 1;
      if(upper(*ptr) == 'N') {
        truth = 0;
        ptr++;
        }
      switch(upper(*ptr)) {
        case 'I': { /* interleave C source */
          ctext = truth;
          defi = 1;
          break;
          }
        case 'M': { /* monitor function headers */
          monitor = truth;
          defm = 1;
          break;
          }
        case 'A': { /* sound alarm on errors */
          alarm = truth;
          defa = 1;
          break;
          }
        case 'P': { /* pause on errors */
          pause = truth;
          defp = 1;
          break;
          }
        default:
          fprintf(stderr, "ERROR:  Extraneous option -%c\n", *ptr);
          errcnt++;
        }
      }
    }
#endif
  line=mline; /* use macro buffer for kbd buffer */
  if(!defin) {
    fprintf(stdout, "Input filename [%s]: ", inname);
    if(prompt("", line, LINESIZE)) {
      joinname(inname, line);
      dupename(outname, inname);
      dftin = 0;
      }
    }
  if(!defout) {
    fprintf(stdout, "Output filename [%s]: ", outname);
    if(prompt("", line, LINESIZE)) {
      joinname(outname, line);
      dftout = 0;
      }
    }
  if(!deflist) {
    fprintf(stdout, "Listing filename [%s]: ", listname);
    if(prompt("", line, LINESIZE)) {
      joinname(listname, line);
      dftlist = 0;
      }
    }
  while(!defi) {
    prompt("Interleave C source? ", line, LINESIZE);
    if(upper(*line)=='Y') ctext=YES;
    else if(upper(*line)!='N') continue;
    defi = 1;
    }
  if((!defm) && dftout) { /* don't monitor if output to con */
    monitor = 0;
    defm = 1;
    }
  while(!defm) {
    prompt("Monitor function headers? ", line, LINESIZE);
    if(upper(*line)=='Y') monitor=YES;
    else if(upper(*line)!='N') continue;
    defm = 1;
    }
  while(!defa) {
    prompt("Sound alarm on errors? ", line, LINESIZE);
    if(upper(*line)=='Y') alarm=YES;
    else if(upper(*line)!='N') continue;
    defa = 1;
    }
  while(!defp) {
    prompt("Pause on errors? ", line, LINESIZE);
    if(upper(*line)=='Y') pause=YES;
    else if(upper(*line)!='N') continue;
    defp = 1;
    }
  if(!dftin) {
    input=fopen(inname,"r");
    if(!input) {
      fprintf(stderr, "ERROR: Cannot open %s for input\n", inname);
      exit(errno);
      }
    }
  if(!dftout) {
    output=fopen(outname,"w");
    if(!output) {
      fprintf(stderr, "ERROR: Cannot open %s for output\n", outname);
      exit(errno);
      }
    }
  if(!dftlist) {
    listfp=fopen(listname,"w");
    if(!listfp) {
      fprintf(stderr, "ERROR: Cannot open %s for listing\n", listname);
      exit(errno);
      }
    }
  }
 
/*
** combine file names overlaying source on default target
*/
joinname(target, source) char target[], source[]; {
  int srcdrive, srcpath, srcname, srcext;
  int tgtdrive, tgtpath, tgtname, tgtext;
  int templen;
  char *savetgt, tempstr[81], *ptr;
  savetgt = target;
  ptr = tempstr;
  templen = 0;
  parsename(source, &srcdrive, &srcpath, &srcname, &srcext);
  parsename(target, &tgtdrive, &tgtpath, &tgtname, &tgtext);
  if(srcdrive) {
    templen += srcdrive;
    if(templen > 80) {fputs("ERROR:  File name too long", stderr); exit(100);}
    strncpy(ptr, source, srcdrive);
    ptr += srcdrive;
    }
  else {
    templen += tgtdrive;
    if(templen > 80) {fputs("ERROR:  File name too long", stderr); exit(100);}
    strncpy(ptr, target, tgtdrive);
    ptr += tgtdrive;
    }
  source += srcdrive;
  target += tgtdrive;
  if(srcpath) {
    templen += srcpath;
    if(templen > 80) {fputs("ERROR:  File name too long", stderr); exit(100);}
    strncpy(ptr, source, srcpath);
    ptr += srcpath;
    }
  else {
    templen += tgtpath;
    if(templen > 80) {fputs("ERROR:  File name too long", stderr); exit(100);}
    strncpy(ptr, target, tgtpath);
    ptr += tgtpath;
    }
  source += srcpath;
  target += tgtpath;
  if(srcname) {
    templen += srcname;
    if(templen > 80) {fputs("ERROR:  File name too long", stderr); exit(100);}
    strncpy(ptr, source, srcname);
    ptr += srcname;
    }
  else {
    templen += tgtname;
    if(templen > 80) {fputs("ERROR:  File name too long", stderr); exit(100);}
    strncpy(ptr, target, tgtname);
    ptr += tgtname;
    }
  source += srcname;
  target += tgtname;
  if(srcext) {
    templen += srcext;
    if(templen > 80) {fputs("ERROR:  File name too long", stderr); exit(100);}
    strncpy(ptr, source, srcext);
    ptr += srcext;
    }
  else {
    templen += tgtext;
    if(templen > 80) {fputs("ERROR:  File name too long", stderr); exit(100);}
    strncpy(ptr, target, tgtext);
    ptr += tgtext;
    }
  ptr[0] = 0; /* assure null termination */
  strcpy(savetgt, tempstr);
  }
 
/*
** copy just file name, not path or extension, to target
*/
dupename(target, source) char target[], source[]; {
  int srcdrive, srcpath, srcname, srcext;
  int tgtdrive, tgtpath, tgtname, tgtext;
  int templen;
  char *savetgt, tempstr[81], *ptr;
  savetgt = target;
  ptr = tempstr;
  parsename(source, &srcdrive, &srcpath, &srcname, &srcext);
  parsename(target, &tgtdrive, &tgtpath, &tgtname, &tgtext);
  if(srcname) {
    templen = tgtdrive + tgtpath + srcname + tgtext;
    if(templen > 80) {fputs("ERROR:  File name too long", stderr); exit(100);}
    strncpy(ptr, target, tgtdrive + tgtpath);
    ptr += tgtdrive + tgtpath;
    source += srcdrive + srcpath;
    strncpy(ptr, source, srcname);
    ptr += srcname;
    target += tgtdrive + tgtpath + tgtname;
    strncpy(ptr, target, tgtext);
    ptr += tgtext;
    ptr[0] = 0;
    strcpy(savetgt, tempstr);
    }
  }
 
/*
** parse file name into [d:] [path\] [name] [.ext]
** return length of each part
*/
parsename(string, drivelen, pathlen, namelen, extlen)
    char *string;  int *drivelen, *pathlen, *namelen, *extlen; {
  char *temp;
  *drivelen = *pathlen = *namelen = *extlen = 0;
  if(alpha(string[0]) && (string[1] == ':')) *drivelen = 2;
  string += *drivelen;
  temp = rindex(string, '\\');
  if(temp) *pathlen = temp - string + 1;
  string += *pathlen;
  temp = index(string, '.');
  if(temp) *namelen = temp - string;
  else *namelen = strlen(string);
  string += *namelen;
  if(string[0] == '.') *extlen = strlen(string);
  }
 
prompt(msg, ans, anslen) char *msg, *ans; int anslen; {
  char *ptr;
  sout(msg, stdout);
  fgets(ans, anslen, stdin);
  if (*ans=='\n') return 0;
  if(ptr = index(ans, '\n')) *ptr = 0; /* zap trailing end-of-line */
  return 1; /* result is true unless line is null */
  }
 
/*
** print an error summary
*/
summary() {
  sout("\n", stdout);
  if(alarm) sout("\7", stdout);
  sout("There were ", stdout);
  errdec(errcnt);
  lout(" errors in this compilation", stdout);
  }
 
setops() {
  op2[0] =       op[0] =  or;          /* heir5  */
  op2[1] =       op[1] = xor;          /* heir6  */
  op2[2] =       op[2] = and;          /* heir7  */
  op2[3] =       op[3] =  eq;          /* heir8  */
  op2[4] =       op[4] =  ne;
  op2[5] =ule;   op[5] =  le;          /* heir9  */
  op2[6] =uge;   op[6] =  ge;
  op2[7] =ult;   op[7] =  lt;
  op2[8] =ugt;   op[8] =  gt;
  op2[9] =       op[9] = asr;          /* heir10 */
  op2[10]=       op[10]= asl;
  op2[11]=       op[11]= add;          /* heir11 */
  op2[12]=       op[12]= sub;
  op2[13]=       op[13]=mult;          /* heir12 */
  op2[14]=       op[14]= div;
  op2[15]=       op[15]= mod;
  }
 

