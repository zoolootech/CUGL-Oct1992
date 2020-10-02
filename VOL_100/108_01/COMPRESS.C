/*
**	compress.c	Version 2.2
**
**	Version 2.2	15-Aug-80
**
**	this program compresses/expands any valid ASCII text
**	file. the CP/M CRLF sequence is replaced by NL, and
**	strings of identical characters of greater than 3 bytes
**	are reduced to 3 bytes in the following format:
**
**		byte 1:	repeat code	RCODE	000Q
**		byte 2: repeat count	cntr	---Q
**		byte 3:	character	c	---Q
**
**	in addition, the most common key-words of various
**	languages (BASIC, ASSEMBLER, FORTRAN, etc.), as
**	well as a number of miscellaneous strings, are
**	tokenized to 1 byte. the average total disk space
**	saved is approximately 20%. processing time is
**	approximately 30" for a 10K file with read-after-
**	write (RAW) in a 40K system.
**
**	considerable protection has been built in to avoid
**	such disasters as compressing a compressed file or
**	expanding an ordinary file - see COMPRESS.DOC for
**	complete details.
**
**	all files processed with earlier versions will be 
**	processed coorectly by this version.
**
**	A>compress {dev:}filename.ext /switch
**
**	/c compress file
**	/e expand file
**
**	NOTE: the system size (example 32K, 40K etc.) should
**	      be set as the defined value for SYSIZE. the
**	      greater the size of the working buffer, the
**	      faster the process will execute.
*/

#include <stdef.c>

#define	STRCOUNT 157		/* size of string array	*/
#define	STRSIZE	 9

#define	SYSIZE	40		/* system size		*/
#define	MAXFILE	SYSIZE-30
#define	BUFSIZE	(MAXFILE+1)*K
#define	TOPBUF	BUFSIZE-256

#define	TRUE	-1
#define	FALSE	0
#define	RCODE	0
#define	NULL	'\0'
#define	SYN	22
#define	FS	28
#define	LLIMIT	' '
#define	INT_EOS	'|'
#define	HLIMIT	'~'

#define	OFFSET1	99
#define	OFFSET2	1
#define	OFFSET3	3
#define	OFFSET4	4

#define ARGCOUNT 3
#define	MAXEXT	13
#define	COMPRESS 'C'
#define EXPAND   'E'

#define	ABRTMSG "aborting..."
#define	ARGCERR	"too many/few arguments"
#define	BINMSG  "  warning: possible binary file, continue (y/n) <abort>: "
#define	CLOSERR	"cannot close output file"
#define	IEXTERR	"illegal .EXT for this operation"
#define	INPFERR	"cannot open input file"
#define	NIMPERR	".EXT not implemented"
#define	OUTFERR	"cannot open output file"
#define	SWITERR	"illegal switch"

char wrdbuf[STRCOUNT][STRSIZE];
char rambuf[BUFSIZE], *bufpntr;
char inbuf[134], outbuf[134];
char expbuf[MAXEXT][4], cmpbuf[MAXEXT][4];

char ofname[12], ext[4];
unsigned charcntr;

main(argc,argv)
char **argv;
int argc;
{
	char *mode ; mode  = &argv[2][1];
	char *ifname; ifname = *++argv;
	int filedescr;

	if (argc != ARGCOUNT) error(ARGCERR);

	initbuf(); filedescr = openout(ifname,mode);
	getfile(ifname,mode); wrbuf();
	closeout(filedescr); unlink(ifname);
}

/*
**	read the contents of the file into the input
**	buffer and process.
*/
getfile(ifname,mode)
char *ifname, *mode;
{
	int cntr, filedescr, i, inword;
	char c, t, temp;

	if ((filedescr = fopen(ifname,inbuf)) == ERROR) {
	   error(INPFERR);
	}

	bufpntr = rambuf; charcntr = RESET;
	switch (*mode) {

	case COMPRESS:

	   inword = FALSE; i = RESET;

	   do {
		   c = getciobuf();
		   while ((c == SPACE || c == LF || c == TAB) &&
		          (inword == TRUE)) {
		      strbuf[i] = NULL; rword(); inword = FALSE;
		   }

		   if (inword == FALSE) {
		      strbuf[0] = wspace(c);
		      i = 1; inword = TRUE;
		   }
		   else {
		      i += 1; strbuf[i-1] = c; inword = TRUE;
		   }

	   } while (c != CPM_EOF);
	   *bufpntr = c; charcntr += 1; break;

	case EXPAND:

	   do {
		   i = RESET;

		   if ((c = getc(inbuf)) >= LLIMIT && c <= HLIMIT) {
		      *bufpntr++ = c; charcntr += 1;
		   }
		   else if (c > HLIMIT) { t = c - OFFSET1;
		      while ((c = wrdbuf[t][i++]) != NULL && c != INT_EOS) {
		         *bufpntr++ = c; charcntr += 1;
		      }
		   }
		   else switch (c) {

		   case RCODE:
		      cntr = getc(inbuf); c = getc(inbuf);
		      for (i = 0; i < cntr; ++i) {
			 if (c == LF) {
			    *bufpntr++ = CR; charcntr += 1;
			 }
			 *bufpntr++ = c; charcntr += 1;
		      }
		      break;
		   case TAB:
		      *bufpntr++ = c; charcntr += 1; break;
		   case LF:
		      *bufpntr++ = CR; *bufpntr++ = c;
		      charcntr += 2;
		      if ((c == LF) && (charcntr >= TOPBUF)) {
	   		 wrbuf(); bufpntr = rambuf; charcntr = RESET;
		      }
		      break;
		   case CPM_EOF:
		      *bufpntr = c; charcntr += 1; break;
		   default:
		      if (c < TAB) t = c - OFFSET2;
		      else t = (c < CPM_EOF) ? c - OFFSET3 : c - OFFSET4;
		      while ((c = wrdbuf[t][i++]) != NULL && c != INT_EOS) {
			 *bufpntr++ = c; charcntr += 1;
		      }
		      break;
		   }

	   } while (c != CPM_EOF);
	   break;

	default: error(SWITERR); break;
	}

	if (close(filedescr) == ERROR) error(CLOSERR);
}

/*
**	process white space
*/
wspace(c)
char c;
{
	int cntr, i, rflag;
	char temp;

	temp = c; rflag = RESET; cntr = 1;

	   do {
		   while ((c = getciobuf()) == temp) {
		      rflag = SET; cntr += 1;
		   }

		   switch (cntr) {

		   case 1:
		      *bufpntr++ = temp; temp = c; charcntr += 1; break;
		   case 2:
		   case 3:
		      for (i = 0; i < cntr; i++) {
			 *bufpntr++ = temp; charcntr += 1;
		      }
		      rflag = RESET; cntr = 1; temp = c; break;
		   default:
		      *bufpntr++ = RCODE; *bufpntr++ = cntr;
		      *bufpntr++ = temp; charcntr += 3;
		      rflag = RESET; cntr = 1; temp = c; break;
		   }

	   } while (c == SPACE || c == LF || c == TAB);
	   return (c);
}

/*
**	process words
*/
rword()
{
	int cntr, i, j, rflag; i = RESET;
	char c, temp; temp = strbuf[0];

	if (strbuf[1] == NULL) {
	   *bufpntr++ = strbuf[0]; charcntr += 1; return;
	}

   switch (strbuf[0]) {		/* optimize search time	*/

   case 'A': i = 0  ; j = 3  ; break; case 'B': i = 4  ; j = 5  ; break;
   case 'C': i = 6  ; j = 15 ; break; case 'D': i = 16 ; j = 27 ; break;
   case 'E': i = 28 ; j = 31 ; break; case 'F': i = 32 ; j = 34 ; break;
   case 'G': i = 35 ; j = 38 ; break; case 'I': i = 39 ; j = 44 ; break;
   case 'J': i = 45 ; j = 49 ; break; case 'L': i = 50 ; j = 56 ; break;
   case 'M': i = 57 ; j = 59 ; break; case 'N': i = 60 ; j = 60 ; break;
   case 'O': i = 61 ; j = 67 ; break; case 'P': i = 68 ; j = 71 ; break;
   case 'R': i = 72 ; j = 84 ; break; case 'S': i = 85 ; j = 92 ; break;
   case 'T': i = 93 ; j = 95 ; break; case 'U': i = 96 ; j = 97 ; break;
   case 'V': i = 98 ; j = 98 ; break; case 'W': i = 99 ; j = 101; break;
   case 'X': i = 102; j = 103; break; case 'a': i = 104; j = 107; break;
   case 'b': i = 108; j = 108; break; case 'c': i = 109; j = 110; break;
   case 'd': i = 111; j = 112; break; case 'e': i = 113; j = 114; break;
   case 'f': i = 115; j = 117; break; case 'g': i = 118; j = 119; break;
   case 'i': i = 120; j = 125; break; case 'n': i = 126; j = 126; break;
   case 'o': i = 127; j = 130; break; case 'p': i = 131; j = 132; break;
   case 'r': i = 133; j = 134; break; case 's': i = 135; j = 136; break;
   case 't': i = 137; j = 140; break; case 'u': i = 141; j = 142; break;
   case 'w': i = 143; j = 145; break;

   		default: i = 146; j = STRCOUNT; break;
   }

	do {
	   if ((compare(strbuf,wrdbuf[i])) == TRUE) {
	   if (i <= BELL) i += OFFSET2;
	   else if (i >= FS) i += OFFSET1;
	        else if (i <= SYN) i += OFFSET3;
	             else i += OFFSET4;
	   *bufpntr++ = i; charcntr += 1; return;
	   }
	} while (i++ < j);

/*
**	process non-reserved words
*/
	i = RESET; temp = c; rflag = RESET; cntr = 1;

	do {
		while ((c = strbuf[++i]) == temp) {
		   rflag = SET; cntr += 1;
		}

		switch (cntr) {

		case 1:
		   *bufpntr++ = temp; temp = c; charcntr += 1; break;
		case 2:
		case 3:
		   for (j = 0; j < cntr; j++) {
		      *bufpntr++ = temp; charcntr += 1;
		   }
		   rflag = RESET; cntr = 1; temp = c; break;
		default:
		   *bufpntr++ = RCODE; *bufpntr++ = cntr;
		   *bufpntr++ = temp; charcntr += 3;
		   rflag = RESET; cntr = 1; temp = c; break;
		}

	} while (c != NULL);
}

/*
**	compare word with reserved keywords
*/
compare(s,t)
char s[],t[];
{
	int i; i =RESET;

	while ((s[i] == t[i]) && (t[i] != INT_EOS)) i++;
	if ((s[i] == NULL) && (t[i] == INT_EOS)) return TRUE;
	else return FALSE;
}

/*
**	get character from inbuf (filter CR's).
*/
getciobuf()
{
	char c;
	while ((c = getc(inbuf)) == CR)
	   ;
	if ((c == LF) && (charcntr >= TOPBUF)) {
	   wrbuf(); bufpntr = rambuf; charcntr = RESET;
	}
	return (c);
}

/*
**	open the output file.
*/
openout(ifname,mode)
char *ifname, *mode;
{
	int filedescr;

	chgext(ifname,mode);
	if ((filedescr = fcreat(ofname,outbuf)) == ERROR) {
	   error(OUTFERR);
	}
	return filedescr;
}

/*
**	change extension, check for legal operation
*/
chgext(ifname,mode)
char *ifname, *mode;
{
	char c;
	int i,j;

	i = j = RESET;
	do {
	   ofname[i] = ifname[i];
	} while (ifname[i++] != '.');
	ofname[i++] = NULL; --i;
	while (ext[j++] = ifname[i++])
	   ;

	i = RESET;
	switch (*mode) {

	   case COMPRESS:	if (strcmp(ext,"TXT") == EQUAL ||
				   strcmp(ext,"MBA") == EQUAL) {
				   printf(BINMSG);
				   if (toupper((c = getchar() != 'Y')))
				      error(ABRTMSG);
				   putchar('\n');
				}
				if (strcmp(ext,"COM") == EQUAL ||
				   ext[0] == '$') error(IEXTERR);
				do {
				   if (strcmp(ext,expbuf[i]) == EQUAL) {
				      strcat(ofname,cmpbuf[i]); return;
				   }
				} while (i++ < MAXEXT);
				error(NIMPERR); break;

	   case EXPAND:		if (ext[0] != '$') error(IEXTERR);
				do {
				   if (strcmp(ext,cmpbuf[i]) == EQUAL) {
				      strcat(ofname,expbuf[i]); return;
				   }
				} while (i++ < MAXEXT);
				error(NIMPERR); break;

	   default:		break;
	}
}

/*
**	write out work buffer
*/
wrbuf()
{
	char c;

	bufpntr = rambuf;
	do {
	   putc((c = *bufpntr++),outbuf);
	} while (--charcntr);
}

/*
**	close output file
*/
closeout(filedescr)
int filedescr;
{
	while (outbuf[2]) putc(CPM_EOF,outbuf); fflush(outbuf);
	if (close(filedescr) == ERROR) error(CLOSERR);
}

initbuf()
{
	strcpy(wrdbuf[0]  ,"ADD|     ANA|     AND|     ANI|    ") ;
	strcpy(wrdbuf[4]  ,"BDOS|    BEGIN|   CALL|    CASE|   ") ;
	strcpy(wrdbuf[8]  ,"CLOSE|   CMC|     CMP|     COMMON| ") ;
	strcpy(wrdbuf[12] ,"CONSOLE| CONST|   CONTINUE|CPI|    ") ;
	strcpy(wrdbuf[16] ,"DAD|     DATA|    DB|      DCR|    ") ;
	strcpy(wrdbuf[20] ,"DCX|     DEF|     DI|      DIM|    ") ;
	strcpy(wrdbuf[24] ,"DIV|     DO|      DOUBLE|  DW|     ") ;
	strcpy(wrdbuf[28] ,"EI|      ELSE|    END|     EQU|    ") ;
	strcpy(wrdbuf[32] ,"FILE|    FOR|     FUNCTION|GET|    ") ;
	strcpy(wrdbuf[36] ,"GO|      GOSUB|   GOTO|    IF|     ") ;
	strcpy(wrdbuf[40] ,"IN|      INPUT|   INR|     INTEGER|") ;
	strcpy(wrdbuf[44] ,"INX|     JC|      JMP|     JNC|    ") ;
	strcpy(wrdbuf[48] ,"JNZ|     JZ|      LABEL|   LDA|    ") ;
	strcpy(wrdbuf[52] ,"LDAX|    LHLD|    LINE|    LOGICAL|") ;
	strcpy(wrdbuf[56] ,"LXI|     MOV|     MOVE|    MVI|    ") ;
	strcpy(wrdbuf[60] ,"NEXT|    OF|      ON|      OPEN|   ") ;
	strcpy(wrdbuf[64] ,"OR|      ORA|     ORG|     OUT|    ") ;
	strcpy(wrdbuf[68] ,"POP|     PRINT|   PROGRAM| PUSH|   ") ;
	strcpy(wrdbuf[72] ,"RAL|     RAR|     READ|    REAL|   ") ;
	strcpy(wrdbuf[76] ,"RECORD|  REM:|    REPEAT|  RET|    ") ;
	strcpy(wrdbuf[80] ,"RETURN|  RLC|     RNC|     RNZ|    ") ;
	strcpy(wrdbuf[84] ,"RRC|     SAVE|    SBI|     SET|    ") ;
	strcpy(wrdbuf[88] ,"SHLD|    STA|     STAX|    STC|    ") ;
	strcpy(wrdbuf[92] ,"SUI|     THEN|    TO|      TYPE|   ") ;
	strcpy(wrdbuf[96] ,"UNTIL|   USING|   VAR|     WEND|   ") ;
	strcpy(wrdbuf[100],"WHILE|   WITH|    XCHG|    XRA|    ") ;
	strcpy(wrdbuf[104],"abs|     and|     are|     as|     ") ;
	strcpy(wrdbuf[108],"break;|  case|    char|    default:|");
	strcpy(wrdbuf[112],"do|      else|    end|     false|  ") ;
	strcpy(wrdbuf[116],"float|   for|     get|     goto|   ") ;
	strcpy(wrdbuf[120],"if|      in|      input|   int|    ") ;
	strcpy(wrdbuf[124],"integer| is|      not|     of|     ") ;
	strcpy(wrdbuf[128],"on|      or|      output|  program|") ;
	strcpy(wrdbuf[132],"put|     real|    return|  struct| ") ;
	strcpy(wrdbuf[136],"switch|  that|    the|     to|     ") ;
	strcpy(wrdbuf[140],"true|    union|   unsigned|which|  ") ;
	strcpy(wrdbuf[144],"while|   with|    !=|      %=|     ") ;
	strcpy(wrdbuf[148],"&&|      *=|      **|      +=|     ") ;
	strcpy(wrdbuf[152],"-=|      <=|      ==|      >=|     ") ;
	strcpy(wrdbuf[156],"/=|     ");

	strcpy(expbuf[0],"ASM");  strcpy(cmpbuf[0],"$AS");
	strcpy(expbuf[1],"BAS");  strcpy(cmpbuf[1],"$BA");
	strcpy(expbuf[2],"C");    strcpy(cmpbuf[2],"$C");
	strcpy(expbuf[3],"COB");  strcpy(cmpbuf[3],"$CO");
	strcpy(expbuf[4],"DAT");  strcpy(cmpbuf[4],"$DA");
	strcpy(expbuf[5],"DOC");  strcpy(cmpbuf[5],"$DO");
	strcpy(expbuf[6],"FOR");  strcpy(cmpbuf[6],"$FO");
	strcpy(expbuf[7],"LIB");  strcpy(cmpbuf[7],"$LI");
	strcpy(expbuf[8],"MAC");  strcpy(cmpbuf[8],"$MA");
	strcpy(expbuf[9],"MBA");  strcpy(cmpbuf[9],"$MB");
	strcpy(expbuf[10],"RAT"); strcpy(cmpbuf[10],"$RA");
	strcpy(expbuf[11],"SRC"); strcpy(cmpbuf[11],"$SR");
	strcpy(expbuf[12],"TXT"); strcpy(cmpbuf[12],"$TX");
}

error(msg)
char *msg;
{
	printf("  error: %s\n",msg); exit();
}
