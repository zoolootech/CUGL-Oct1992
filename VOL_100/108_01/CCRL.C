 
#define DIRSTART        0x0     /*CRL directory*/
#define DIREND          0x1ff   /*dir. stops here*/
#define FNCSTART        0x205   /*fnc.'s can start*/
#define TPAHBRLC        0x1     /*hi byte of tpa add*/
 
#define JUMPINSTR       0xc3    /*jump instr. op code*/
#define MASK7           0x7f    /*seven bit mask*/
#define NULENTRY        0x80    /*null function*/
 
#define CR              0xd     /*carriage return*/
#define LF              0xa     /*line feed*/
 
#define MXFNSIZE        4096    /*max function size*/
#define MXRELOCS        512     /*mx # reloc. symbls*/
#define MXRLCJTS        128     /*max # j.t. relocs*/
 
 
char    fncbuf[134], extbuf[134], outbuf[134];
char    filestr[20];            /*where file names are kept*/
 
main(argc, argv)
  int argc;
  char **argv;
{
  char  function[MXFNSIZE];
  int   fncfd, extfd, outfd;
  int   addr, fsize, size, fend, outbytes;
  int   rlc[MXRELOCS], rlcjt[MXRLCJTS];
  int   fip, rip, ripjt, rlcoffset;
  char  *s, *fn;
  int   i, j, k, ip, bust, c, cp;
 
  ip = 0;
 
  printf("Ccrl, 11/19/79\n");
  if(argc < 2)
    { printf("File required\n");
      exit();
    }
 
  s = *++argv;
  fn = filename(s, "COM");
  fncfd = fopen(fn, fncbuf);
  if(fncfd <= 0)
    { printf("Cannot open %s\n",fn);
      exit();
    }
 
  fn = filename(s, "EXT");
  extfd = fopen(fn, extbuf);
  if(extfd <= 0)
    { printf("Cannot open %s\n",fn);
      exit();
    }
 
  fn = filename(s, "CRL");
  outfd = fcreat(fn, outbuf);
  if(outfd <= 0)
    { printf("Cannot create %s\n",fn);
      exit();
    }
  else
    printf("Creating %s\n",fn);
 
  outbytes = -1;
  addr = FNCSTART;
  fn = funcname(extbuf);
  if(fn == '\0')
    { printf("Error: no function name on first line of .ext file\n");
      exit();
    }
  putstr(fn, outbuf, &outbytes);
  putw(addr, outbuf);
  outbytes += 2;
  putc(NULENTRY, outbuf);
  ++outbytes;
  fsize = rdhexlf(extbuf);      /*read function length*/
  size = rdhexlf(extbuf);       /*size incl. all other stuff*/
  fend = addr + size;
  putw(fend, outbuf);
  outbytes += 2;
  printf("Total size is %0d bytes\n", size);
  if(outbytes > DIREND)
    { printf("Error: new directory overflows directory area\n");
      exit();
    }
  while(outbytes < (addr - 1))
    { putc(0,outbuf);
      ++outbytes;
    }
  ip = 0;                       /*count of external functions*/
  do
    { fn = funcname(extbuf);
      if(fn != '\0')
        { while(*fn != '\0')
            { putc(*fn++, outbuf);
              ++outbytes;
            }
          ++ip;
        }
    }
  while(fn != '\0');
  putc(0x0, outbuf);
  ++outbytes;             /*null to end*/
  if(ip > 0)
    { if(3*ip >= 0xff)
        { printf("Error: to many external functions\n");
          exit();
        }
      function[0] = JUMPINSTR;
      function[1] = 3*(ip + 1);
      function[2] = TPAHBRLC;
      fip = 3;
      for(j = 1; j <= ip; j++)
        { function[fip++] = JUMPINSTR;
          function[fip++] = 0x0;
          function[fip++] = 0x0;
        }
      rlcoffset = 3*(ip + 1);
      rlcjt[0] = 1;             /*re-locate jump over jump table*/
      ripjt = 1;                /*this is first relocation parameter*/
    }
  else
    { fip = 0;
      rlcoffset = 0;
      ripjt = 0;
    }
  for(j = 1; j <= fsize; j++)
    { c = getc(fncbuf);
      function[fip++] = c;
      if(fip > MXFNSIZE)
        { printf("Error: function buffer size exceeded\n");
          exit();
        }
    }
  putw(fip, outbuf);
  outbytes += 2;                /*fnc size*/
  if(ripjt)                     /*no JT RLC's unless one already*/
    { while((c = readhex(extbuf))  >=  0)
        { rlcjt[ripjt++] = rlcoffset + c;
          if(ripjt > MXRLCJTS)
            { printf("Error: To many jump table relocation parameters\n");
              exit();
            }
        }
    }
  rip = 0;
  do
    { if((c = readhex(extbuf)) >= 0)
        rlc[rip++] = rlcoffset + c;
      if(rip > MXRELOCS)
        { prinxf("
			else{
				tabs--;
				puts();
				tabs++;
			}
			if((peek = getchr()) == ';'){
				printf(";");
				peek = -1;
			}
			getnl();
			puts();
			printf("\n");
			sflg = 1;
			break;
		case '/':
			string[j++] = c;
			if((peek = getchr()) != '*')break;
			string[j++] = peek;
			peek = -1;
			comment();
			break;
		case ')':
			paren--;
			string[j++] = c;
			puts();
			if(getnl() == 1){
				peek = '\n';
				if(paren != 0)aflg = 1;
				else if(tabs > 0){
					pflg[level]++;
					tabs++;
					ind[level] = 0;
				}
			}
			break;
		case '#':
			string[j++] = c;
			while((cc = getchr()) != '\n')string[j++] = cc;
			string[j++] = cc;
			sflg = 0;
			puts();
			sflg = 1;
			break;
		case '(':
			string[j++] = c;
			paren++;
			if(lookup(wfor) == 1){
				while((c = gets()) != ';');
				ct=0;
cont:
				while((c = gets()) != ')'){
					if(c == '(') ct++;
				}
				if(ct != 0){
					ct--;
					goto cont;
				}
				paren--;
				puts();
				if(getnl() == 1){
					peek = '\n';
					pflg[level]++;
					tabs++;
					ind[level] = 0;
				}
				break;
			}
			if(lookup(wif) == 1){
				puts();
				stabs[clevel][iflev] = tabs;
				spflg[clevel][iflev] = pflg[level];
				sind[clevel][iflev] = ind[level];
				iflev++;
				ifflg = 1;
			}
		}
	}
	if (outvect != 1){
		printf("\032");
		fflush(outbuf);
	}
	exit();
}
ptabs(){
	int i;
	for(i=0; i < tabs; i++)printf("\t");
}
getchr(){
	if(peek < 0 && lastchar != ' ' && lastchar != '\t')pchar = lastchar;
	lastchar = (peek<0) ? getc(inpbuf):peek;
	peek = -1;
	return(lastchar == '\r' ? getchr():lastchar);
}
puts(){
	if(j > 0){
		if(sflg != 0){
			ptabs();
			sflg = 0;
			if(aflg == 1){
				aflg = 0;
				if(tabs > 0)printf("    ");
			}
		}
		string[j] = '\0';
		printf("%s",string);
		j = 0;
	}
	else{
		if(sflg != 0){
			sflg = 0;
			aflg = 0;
		}
	}
}
lookup(tab)
char *tab[];
{
	char r;
	int l,kk,k,i;
	if(j < 1)return(0);
	kk=0;
	while(string[kk] == ' ')kk++;
	for(i=0; tab[i] != 0; i++){
		l=0;
		for(k=kk;(r = tab[i][l++]) == string[k] && r != '\0';k++);
		if(r == '\0' && (string[k] < 'a' || string[k] > 'z'))return(1);
	}
	return(0);
}
gets(){
	char ch;
beg:
	if((ch = string[j++] = getchr()) == '\\'){
		string[j++] = getchr();
		goto beg;
	}
	if(ch == '\'' || ch == '"'){
		while((cc = string[j++] = getchr()) != ch)if(cc == '\\')string[j++] = getchr();
		goto beg;
	}
	if(ch == '\n'){
		puts();
		aflg = 1;
		goto beg;
	}
	else return(ch);
}
gotelse(){
	tabs = stabs[clevel][iflev];
	pflg[level] = spflg[clevel][iflev];
	ind[level] = sind[clevel][iflev];
	ifflg = 1;
}
getnl(){
	while((peek = getchr()) == '\t' || peek == ' '){
		string[j++] = peek;
		peek = -1;
	}
	if((peek = getchr()) == '/'){
		peek = -1;
		if((peek = getchr()) == '*'){
			string[j++] = '/';
			string[j++] = '*';
			peek = -1;
			comment();
		}
		else string[j++] = '/';
	}
	if((peek = getchr()) == '\n'){
		peek = -1;
		return(1);
	}
	return(0);
}
comment(){
rep:
	while((c = string[j++] = getchr()) != '*')
		if(c == '\n'){
			puts();
			sflg = 1;
		}
gotstar:
	if((c = string[j++] = getchr()) != '/'){
		if(c == '*')goto gotstar;
		goto rep;
	}
}
printf(format,arg)
char *format;
unsigned arg;
{
	if (fprintf(outvect,format,arg) == -1){
		fprintf(1,"Disk write error\n");
		exit();
	}
}
öööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööööööööööööööööööö ööööööööööööööö 
 
 
 
 
 
 
 
 
 
 
