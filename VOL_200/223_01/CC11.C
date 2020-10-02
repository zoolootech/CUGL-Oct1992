/*
** execution begins here
*/

main(argc, argv) int argc; char *argv[]; {

  if (argc > MAXARGS) {
    lout("max command args exceeded", stderr);
    abort(ERRCODE);
    }
  argcs = argc;
  argvs = argv;

  sout("Small-C Version 2.7   3/26/86   (fas)", stderr);
  lout(" ", stderr);

#ifdef DYNAMIC
  swnext=calloc(SWTABSZ, 1);
  swend=swnext+((SWTABSZ-SWSIZ)>>1);
  stage=calloc(STAGESIZE, 1);
  stagelast=stage+STAGELIMIT;
  wq=calloc(WQTABSZ, BPW);
  litq=calloc(LITABSZ, 1);
  macn=calloc(MACNSIZE, 1);
                           /*10*/
  macq=calloc(MACQSIZE, 1);
  pline=calloc(LINESIZE, 1);
  mline=calloc(LINESIZE, 1);
  arrptr=calloc(MDASIZE, 2);				/* fas 2.6 */
  sarrptr=arrptr;					/* fas 2.6 */
  arrcount = 1;						/* fas 2.6 */
#else
  swend=(swnext=swq)+SWTABSZ-SWSIZ;
  stagelast=stage+STAGELIMIT;
#endif
  swactive=       /* not in switch */
  stagenext=      /* direct output mode */
  iflevel=        /* #if... nesting level = 0 */
  skiplevel=      /* #if... not encountered */
  macptr=         /* clear the macro pool */
  csp =           /* stack ptr (relative) */
  errflag=        /* not skipping errors till ";" */
  eof=            /* not eof yet */
  ncmp=           /* not in compound statement */
  files=
  filearg=
  inclevel=	  /* include nesting level	   fas 2.7 */
  quote[1]=0;
  func1=          /* first function */
  ccode=1;        /* enable preprocessing */
  wqptr=wq;       /* clear while queue */
  quote[0]='"';   /* fake a quote literal */
  input=input2[0]=input2[1]=input2[2]=input2[3]=
  input2[4]=input2[5]=EOF;			/* fas 2.7 */

/*
** this is the real start
*/

  ask();         /* get user options */
  openin();      /* and initial input file */
  preprocess();  /* fetch first line */

#ifdef DYNAMIC
  symtab=calloc((NUMLOCS*SYMAVG + NUMGLBS*SYMMAX), 1);
#endif
                         /*10*/
  locptr=STARTLOC;       /*52*/
  glbptr=STARTGLB;
  glbflag=1;
  ctext=0;
  header();          /* intro code */
  setops();          /* set values in op arrays */
  parse();           /* process ALL input */
  outside();         /* verify outside any function */
  trailer();         /* follow-up code */
  fclose(output);
  }

/*
** process all input text
**
** At this level, only static declarations,
**      defines, includes and function
**      definitions are legal...
*/
parse() {
  while (eof==0) {
    if(amatch("extern", 6))   dodeclare(EXTERNAL);
    else if(match("static")) {			/* fas 2.2 */
      m80flg = NO;
      dodeclare(STATIC);
      m80flg = YES;
    }
    else if(dodeclare(STATIC));
    else if(match("#asm"))    doasm();
    else if(match("#include"))doinclude();
    else if(match("#define")) addmac();
    else                      newfunc();
    blanks();       /* force eof if pending */
    }
  }

/*
** dump the literal pool
*/
dumplits(size) int size; {
  int j, k;
  k=0;
  while (k<litptr) {
    poll(1); /* allow program interruption */
    defstorage(size);
    j=10;
    while(j--) {
      outdec(getint(litq+k, size));
      k=k+size;
      if ((j==0)|(k>=litptr)) {
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

  if(!iflag){					/* fas 2.2 */
    if(count > 0){
    ot("DS ");
    outdec(size * count);
    nl();
    }
  }else{
    while (count > 0) {
      poll(1); /* allow program interruption */
      defstorage(size);
      j=30;
      while(j--) {
        outdec(0);
        if ((--count <= 0)|(j==0)) {
          nl();
          break;
          }
        outbyte(',');
        }
      }
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


ask()  {
  int i;
  i=listfp=nxtlab=0;
  output=stdout;

#ifdef OPTIMIZE
  optimize=
#endif /* OPTIMIZE */

  nbflg=iflag=alarm=monitor=pause=NO;
  m80flg=YES;
  line=mline;

  while(++i < argcs) {
    line = argvs[i];
    if (line[0] == '-') {
      switch (toupper(line[1])) {
        case 'L':  if (isdigit(line[2]) & (line[3] <= ' ')) {
                     listfp = line[2] - '0';
                     break;
                     }
                   else
                     goto errcase;
        case 'A':  alarm = YES;
                   break;
        case 'M':  monitor = YES;
                   break;
        case 'P':  pause = YES;
                   break;
	case 'I':  iflag = YES;
		   break;
	case 'N':  nbflg = YES;
		   break;
#ifdef OPTIMIZE
        case 'O':  optimize = YES;
                   break;
#endif /* OPTIMIZE */

#ifndef LINK
        case 'B': if (numeric(line[2]) & (line[3] <= ' ')) {
                    bump(0); bump(2);
                    if(number(&nxtlab)) break;
                    }
                    /* fall through to error case */
#endif /* LINK */

        errcase:
   default:  sout("usage: cc [file]...[-a] [-i] [-l#] [-m] [-n] [-p]", stderr);

#ifdef OPTIMIZE
                  sout(" [-o]", stderr);
#endif /* OPTIMIZE */

#ifndef LINK
                  sout(" [-b#]", stderr);
#endif /* LINK */

                  sout("\n", stderr);
                  abort(ERRCODE);
        }
      }
    }
  }


/*
** get next input file
*/

openin() {
  input = EOF;
  line = pline;

  while(++filearg < argcs) {
    line = argvs[filearg];
    if(line[0]=='-') continue;

    if((input=fopen(line,"r"))==NULL) {
      sout("open error: ", stderr);
      lout(line, stderr);
      abort(ERRCODE);
      }
    files=YES;
    kill();
    return;
    }
  if(files++) eof=YES;

  else input=stdin;  /* V7 convention */

  kill();
  }

setops() {
  op2[00]=     op[00]=  ffor;  /* heir5 */
  op2[01]=     op[01]= ffxor;  /* heir6 */
  op2[02]=     op[02]= ffand;  /* heir7 */
  op2[03]=     op[03]=  ffeq;  /* heir8 */
  op2[04]=     op[04]=  ffne;
  op2[05]=ule; op[05]=  ffle;  /* heir9 */
  op2[06]=uge; op[06]=  ffge;
  op2[07]=ult; op[07]=  fflt;
  op2[08]=ugt; op[08]=  ffgt;
  op2[09]=     op[09]= ffasr;  /* heir10 */
  op2[10]=     op[10]= ffasl;
  op2[11]=     op[11]= ffadd;  /* heir11 */
  op2[12]=     op[12]= ffsub;
  op2[13]=     op[13]=ffmult;  /* heir12 */
  op2[14]=     op[14]= ffdiv;
  op2[15]=     op[15]= ffmod;
  }

