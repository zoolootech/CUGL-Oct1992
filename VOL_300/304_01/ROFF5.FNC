/********************************************************/
/*		 ROFF5, Version 2.00			*/
/*    (c) 1983,4,8,9 by Ernest E. Bergmann		*/
/*	730 Seneca Street, Bethlehem, Pa. 18015		*/
/*							*/
/* Permission is hereby granted for all commercial and	*/
/* non-commercial reproduction and distribution of this	*/
/* material provided this notice is included.		*/
/********************************************************/
#if (VERSION!=2) || (subVERSION!=00)
#error ***************This is version 2.00******************
#endif

/*  found in ROFF5.C : ****/
void main(int argc, char **argv);
void init_defaults(); /*initializes global variables*/
void range(char *s);/*sets FIRSTPAGE,LASTPAGE*/
void redoRR();/*initialize RULER string*/
int verifyRR(char *rr);/*checks legality of string as RULER and updates*/
/*********end of ROFF5.C*********/

/*found in ROFF51.C : */
void skip_blanks(); /*advances LIN in LINE over white space*/
CMD comtyp1(char *line);/*returns command value of command,IGNORE,UNKNOWN*/
void set(int *param,int val,char arg_typ,int defval,int minval,int maxval);
void Brk(); /*end current filled line*/
void initxu();/*init underline,overstrike variables*/
void need(int n); /*tests for space before footer*/
int min(int i, int j); /*returns min of 2 args*/
int max(int i, int j); /*returns max of 2 args*/
int process(ARGs *a); /*parses args from LIN in LINE; returns # args*/
void pbmac(char *m, ARGs a); /*generalized pbstr() for macro with args*/
int get_val2(char *l,char *typ,char **end);/*generalized get_val with arith*/
void setnxtval(char **line,int *param,int deflt,int minparam, int maxparam);
int expr(); /* returns value of arithmentic expression; error handling */
int expr0();/* returns value of arithmetic expresion; handles logicals*/
int expr1();/* handles combination of arithmetic terms */
int expr2();/* handles combination of arithmetic factors*/
int expr3();/* handles numbers, tokens, and (expresions) */
void amat();/* diagnostic showing where arithmetic parsing has reached */
/************end of ROFF51.C**********/

/*found in ROFF52.C : */
void insert(); /* from .DS LINE into an ENTRY of SLINK */
void showit(); /*string substitution table*/
void putback(char c); /*cf K & P, p256*/
int ngetc(FILE *iobuf);/*cf K & P, p256;filters \rfollowed by \n,msb*/
int kgetc();/*like getc(),from keyboard,line buffered*/
void pbstr(char s[LSZ]); /*putback string on input*/
void apatch(char *s); /*appends PatchName to s*/
void showm(int mode); /*lists macro definitions*/
char *macq1(char *line); /*looks up line in macro table;returns replacement
                                       or FALSE*/
char *find2(char *s, ENTRY *LINK);/*finds s in table; returns replacement
				       or FALSE*/
ENTRY *find(char *s, ENTRY *LINK);
ENTRY *mpatch(char *name,ENTRY *LINK); /*returns FALSE or &PatchName*/
void mappend(CMD cmd); /*does .de, .am */
ENTRY *findany(char *name);
void rndef(); /*does .rn */
ENTRY *find0(char *s,ENTRY *LINK); /*returns NULL or link->entry */
ENTRY *find0any(char *s);
int rm1(char *id); /*if entry id found, removes it and continuations*/
void rmdef(); /* does .rm */
void dappend(CMD cmd); /*does start of .di and .da*/
void divert(char *line); /*places line into diversion*/
void enddiv(); /*finishes (closes) diversion entry*/
void dtset(); /*similar to itset()*/
/*********** end of ROFF52.C ******/

/* found in ROFF53.C : */
void exitm(char *msg);/*prints msg to stderr; exit(1);*/
int *regist(char *s);/*checks RLIST makes new entry if necssary;
                       returns pointer to integer variable*/
void dovar(); /*process .RG request*/
void source();/*process .SO request*/
void showr();/*lists register names and contents*/
void assignfmt(); /*process .AF request*/
FORMAT *picfmt(char *string, FORMAT *pf); /*for assignfmt, sets <- pf*/
void endso(); /*used by source to return to previous input upon EOF*/
void dashes();/*prints -------- to stderr*/
void ignore(); /*does .ig*/
void detab(); /*expands tabs and soh in OUTBUF2*/
int readonly(char *name); /*handles predefined regs for fgets2()*/
/********** end of ROFF53.C ***/

/* found in ROFF54.C : */
void doTI(char *line); /*removes leading space; updates TIVAL*/
void leadbl(char *line); /*handles leading blanks, tabs, empty lines*/
void phead(); /*puts out page header*/
void pfoot(); /*puts out page footer*/
void pbcmd(char *xx,int num); /* pbstr(".xx <num>\n") */
void retract();/*backs out of stuff in progress during page trap*/
void space(int n); /*puts out n lines or to bottom of page*/
void text();
void put(char *line); /*output text line with correct indent, underline*/
void putwrd();/*concatenates GPWBUF to OUTBUF for filling*/
void offset(char *s,int i); /*prepends to s i spaces*/
void putline(char *line); /*sends line to OUTPUT2*/
void OUTcat(char *str); /*appends string to OUTBUF*/
void OUTSP(); /*appends blank to OUTBUF*/
void gloss(); /*prints on STDOUT glossary of .tr characters*/
void chgenv(int old,int new); /*change environments for headrs,foots*/
void putlinout(int txtline);	/* redirects output printer/diversion */
/*********** end of ROFF54.C ******/

/* found in ROFF55.C : */
void puttl3(char *s1,char *s2, char *s3, int pageno, FORMAT fmt);
           /* put out three part title */
void blankb(int i); /*sends i blanks to OUTBUF2; handles negative i*/
void gettl3(char *sourceline,char ttl1[MAXLINE],char **ttl2,char **ttl3);
      /* gets three part title from source line; no strings have \n */
void title3(char *sl,int pageno,FORMAT fmt); /*for .tl*/
void transfer(char **s,char **d,char c); /*specialied to use terminal char*/
void center(char *line); /*sets variables such as TIVAL*/
int getwrd(char *in,char *out);/*returns WE_HAVE_A_WORD*/
int gwLIN(char *out); /*equivalent to getwrd(LIN,out) */
void trunc_bl(char *string); /*truncates white space characters at EOS*/
void spread(char *line, int nextra,int no_words); /*"adjusts" line*/
void adjust();	/*handles .ad request*/
void puttl(char *str, int num, FORMAT fmt);
		/* place portion of title with opt page # in OUTBUF2 */
void putnum(int num, FORMAT fmt); /*put out num to OUTBUF2 (conversion) */
int itoC(int num,char buff[],FORMAT fmt);/*converts num to representation*/
char *itoR(int val,char *string);/*Roman numeral format*/
char *itoA(int val,char *string);/*conversion ->letters of alphabet*/
void putout(char c);/*places c in OUTBUF2[]*/
/***************** end of ROFF55.C *******/

/* found in ROFF56.C : */
int value(int base, char *string);/*unsigned conv; -1 error return*/
int digit(char d);/*for any radix; error return -1*/
int strln4(char *s, int word, int num,FORMAT fmt);
	/*checks legality, size of glob; use negative num to disable # */
void setS(int param[STKSIZ+1],int val,char arg_typ,int defval,int minval,
          int maxval);/*similar to set(), but for stacked variable*/
void initsk(int *stack,int v);/*init stacked var with v*/
void setchar(int *param,int stack,int defval,int minval,int maxval);
void getot();/*process .ot */
char getcode(char buff[128]);/*used by gettr(),getpc()...*/
void ocode(); /*process .ou */
void outstr(char *p); /*print string whose byte count is *p*/
void getfr(); /*process .FR; cf. ocode()*/
void when(); /*places a page trap for .wh*/
void changetrap(); /* changes page trap for .ch*/
void itset(); /*used to process .it */
void EMset(); /*used to process .em */
int dotT(int ln); /*returns distance to next trap from ln ".t" */
void setTRAP(); /*recalculates FOOTLOC,TRAPLOC*/
/********* end of ROFF56.C ****/

/* found in ROFF57.C : */
void printout(int txtlin);	/*outputs contents of OUTBUF2,
	including super and subs; txtlin=FALSE for titles*/
void padv();/*moves printer vertically speced by VLINENO,FVLINENO*/
void backup(int i);/*not yet implemented*/
void excurs(char *str, int *t, int *b);/*finds topmost and bottom pos of str*/
void flp(int level,int updat);/*fancy line print
                           at given vertical level of OUTBUF2[]*/
int retype();/*returns boolean;restores OUTBUF2 from DBUF*/
int lbc(int lev, char *str);/*counts printable chars at level and above*/
void prpass(int lev,int updat);/*printer pass initial cr; np lf anywhere*/
int class(char c);/*recognize which type char is*/
void fraction(); /*put printer in fractional spacing mode*/
void whole(); /*place printer in whole line spacing*/
void trch(char c);/*output string translation of c*/
void pcont(char c);/*output printer control string for c*/
void cputc(char c, FILE *fp); /* putc if !SUPPRESS */
/********* end of ROFF57.C **/

/* found in ROFF58.C */
char *fgets3(); /*fills LINE[] from ifp; returns &LINE[0] or NULL on EOF*/
void comand(); /*hugh switch; performs formatting commands*/
void dolns(); /*do processing of lines*/
void action(); /*processes true conditional grammar of dolns()*/
void inaction(); /*processed false conditional part of dolns()*/
int act2(int act); /*handles .el part; act T/F uses action/inaction*/
void conderr(char *err); /*non-fatal error reporting for cond grammar*/
/*******end of ROFF58.C ******/
