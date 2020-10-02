/* #include "bdscio.h"  */

/* in CI C86 need to compile this with the -u switch so that "char" is
   treated as "unsigned char"
*/

#include "stdio.h"      /* CI C86 */
#include "ctype.h"      /* CI C86 */
#include "limits.h"     /* CI C86 */
#include "setjmp.h"     /* CI C86 */
#include "string.h"     /* CI C86 */
#include "errno.h"      /* CI C86 */
#include "stdlib.h"     /* CI C86 */
#include "fileio2.h"    /* CI C86 */
#undef  isspace         /* CI C86 */
#define ERROR      0    /* CI C86 */
#define OK        -1    /* CI C86 */
#define FALSE      0    /* CI C86 */
#define TRUE       1    /* CI C86 */
char *ljust();          /* CI C86 */
#define errmsg(num,str) printf("\n        Error %d: %s",(num),(str))/* CI C86 */



#define OPENCMNT   1
#define HEADER     2
#define ATTRIBUTE  3
#define ENDCMNT    4
#define TITLE      5
#define VERSION    6
#define DATE       7
#define DESC       8
#define KEYWD      9
#define SYSTEM    10
#define FNAME     11
#define WARNING   12
#define CRC       13
#define ALSO      14
#define AUTHOR    15
#define COMP      16
#define REF       17
#define COMMA     18
#define ENDREF    19
#define REFAUTH   20
#define REFTTL    21
#define CITATION  22
#define REFCIT    23
#define COLON     24
#define TOOLONG   25
#define SEMI      26
#define QUOTE     27
#define FAULT     28

#define LEXSIZE 250

#define ON   1
#define OFF  0

/* FILE source, list; */
FILE *source, *list;     /* CI C86 */

/* char restart[6]; */
jmp_buf *restart;        /* CI C86 */
char catno[20];
char lexval[LEXSIZE];
int stoptoken;
int debug, testonly;
int parsing;
int number;

main(argc,argv)
int argc;
char **argv;

{
int fnumb, diskno;
int i;
char *lname;
char *first;         /* CI C86 */
char *next;          /* CI C86 */
char filespec[255];  /* CI C86 */
char *namestrt;      /* CI C86 */
extern char *filedir(); /* CI C86 */
extern char *upper();   /* CI C86 */
extern unsigned char *strchr();  /* CI C86 */
extern unsigned char *strrchr(); /* CI C86 */

/* if (wildexp(&argc,&argv) == ERROR)  / * CI C86 handles this differently * /
   exit(puts("Wildexp Overflow\n"));    */

number = debug = testonly = OFF;
fnumb = diskno = 0;

if (argc < 2) {
    printf("USAGE:  header [-o<filespec>] [-t] [-d] [-nd] {<inpathspec>} \n\n"); /* CI C86 */
    printf("   function:    Normally used to extract header information\n");
    printf("                from the files in inpathspec, reformatting the\n");  /* CI C86 */
    printf("                information in a fixed field format suitable\n");
    printf("                for input to a relational database. The formatted\n");
    printf("                output defaults to 'hdrout' on the current drive.\n\n");
    printf("   options:     -o  use filespec for the list file name.\n");
    printf("                -t  test only...checks syntax with no listing\n");
    printf("                -n  number mode. Generates sequential file numbers\n");
    printf("                    for all files on the disk. d is used as the\n");
    printf("                    volume number. Headers consisting of only a \n");
    printf("                    filename are generated for all files which don't\n");
    printf("                    have a header. These are written to the list\n");
    printf("                    file. Automatically sets -t, that is, since\n");
    printf("                    the list file is being used to capture headers\n");
    printf("                    no attempt is made to generate normal output.\n");
    printf("                -d  debug...prints trace info to stdout.\n");
    exit();
    }
lname="hdrout";
for (i=1; i<argc; i++){
   if (*argv[i] == '-'){
/*    switch (*(argv[i]+1)){ */
      switch (*upper(argv[i]+1)){                           /* CI C86 */
         case 'D': debug = ON; break;
         case 'T': testonly = ON; break;
         case 'N': sscanf(argv[i]+2,"%d",&diskno);
                   number = ON;
                   testonly = ON;
                   if (diskno < 100){
                      printf("\nCUG numbers start at 100!\n");
                      exit();
                      }
                   break;
         case 'O': lname= argv[i]+2;
                   break;
         default:  printf("\nUnrecognized option %s",argv[i]);
                   break;
         }
      }
   if (debug) printf("\nargv[%d] = %s",i,argv[i]);   /* CI C86 */
   }

if ((!testonly) || number) openlist(lname);
if (debug) printf("\n open list passed");

for (i=1; i<argc; i++){
   if (*argv[i] != '-'){

     strcpy(filespec,upper(argv[i]));                           /* CI C86 */
     if (debug) printf("\nExpanding filespec %s",filespec);     /* CI C86 */
     if ((namestrt = strrchr(filespec,'\\')) == NULL)           /* CI C86 */
       if ((namestrt = strchr(filespec,':')) == NULL)           /* CI C86 */
         namestrt = filespec - 1;                               /* CI C86 */
     namestrt += 1;                                             /* CI C86 */
     if ((first = filedir(filespec,0)) == NULL)                 /* CI C86 */
       { printf("\nNo files match %s",filespec);                /* CI C86 */
         continue; /* with next argv */                         /* CI C86 */
       };                                                       /* CI C86 */
     for (next = first; *next != NULL; next +=(strlen(next)+1)) /* CI C86 */
      {                                                         /* CI C86 */
       strcpy(namestrt,next); /* add name to drive:\path spec *//* CI C86 */
/*     printf("\n%-15s",argv[i]);  */
       printf("\n%s",filespec);                                 /* CI C86 */
/*     if (fopen(argv[i],source) == ERROR) printf(" ...Can't open ");*/
       if ((source=fopen(filespec,"r")) == NULL)                /* CI C86 */
                                     printf(" ...Can't open "); /* CI C86 */

       else {
          if (debug) {                                          /* CI C86 */
             putchar('\n');                                     /* CI C86 */
             for (debug=1;debug++ < 160;)                       /* CI C86 */
                printf("%c",getc(source));                      /* CI C86 */
             rewind(source); debug=ON;                          /* CI C86 */
             };                                                 /* CI C86 */
          if (number) {
             fnumb++;
/*           if (dofile() == OK) numfile(argv[i], diskno, fnumb);*/
             if (dofile() == OK) numfile(namestrt,diskno, \
                                         fnumb,filespec);    /* CI C86 */
             else {
                fclose(source);
                fprintf(list,"/* HEADER: CUG%03d.%02d; FILENAME: %s; */\n",
/*                           diskno,fnumb,argv[i]); */
                             diskno,fnumb,namestrt);         /* CI C86 */
                }
             }
          else {
             dofile();
             fclose(source);
             }
          }         /* of opening one source file */         /* CI C86 */
       }            /* of expanding one filespec  */         /* CI C86 */
       free(first);                                          /* CI C86 */
     }              /* of *argv[i] != '-' */                 /* CI C86 */
    }               /* of loop thru all argv[] values */     /* CI C86 */

if ((!testonly) || number){
   fprintf(list,"%c",0x1a);
   fflush(list);
   fclose(list);
   }
}

/*
   process a single file, adding results to output file
*/
int dofile()
{
int att;
int laterpass;

parsing = FAULT;
laterpass = setjmp(restart);
if (laterpass) return OK;

if (scantoken() != OPENCMNT) {
   printf(" ...File doesn't begin with comment");
   return !OK;
   }
if (scantoken() != HEADER) {
   printf(" ...First comment isn't header ");
   return !OK;
   }
printf(" ...Processing header");
if (scantoken() != COLON) {
   printf(" ...No colon after header keywork");
   return !OK;
   }
scancatno(); /* sets catno as sideeffect */
printf(" %9s",catno);                                          /* CI C86 */
if (scantoken() != SEMI) faterr(4,"Expecting semi after cat. no.");
while (tokentype(att=scantoken())==ATTRIBUTE) scanatt(att);
if (stoptoken != ENDCMNT) faterr(5,"Header must end with comment delimiter");
while(dohdr()==OK);  /*do following headers */
return OK;
}

int dohdr()
{
int att;
int laterpass;

parsing = FAULT;
laterpass = setjmp(restart);
if (laterpass) return !OK;

if (scantoken() != OPENCMNT) {
   return !OK;
   }
if (scantoken() != HEADER) {
   return !OK;
   }
printf("\n                ...and another");
if (scantoken() != COLON) {
   printf(" ...No colon after header keywork");
   return !OK;
   }
scancatno(); /* sets catno as sideeffect */
printf(" %9s",catno);                                          /* CI C86 */
if (scantoken() != SEMI) faterr(4,"Expecting semi after cat. no.");
while (tokentype(att=scantoken())==ATTRIBUTE) scanatt(att);
if (stoptoken != ENDCMNT) faterr(5,"Header must end with comment delimiter");
/* always scans one token beyond end of attribute, leaves */
return OK;
}

int scanatt(att)
int att;

{
parsing = att;
if (scantoken() != COLON)
    faterr(10, "Attribute name must be followed by colon");

switch(att){
   case TITLE:
          if (scanitem() != OK) faterr(10,"ITEM must follow TITLE");
          generate(catno,TITLE,lexval);
          scantoken();
               break;
   case VERSION:
          if (scannumber() != OK) faterr(11,"version number needed");
          generate(catno,VERSION,lexval);
          scantoken();
          break;
   case DATE:
          if (scandate() != OK) faterr(12,"date needed");
          generate(catno,DATE,lexval);
          scantoken();
          break;
   case DESC:
          if (scanstring(DESC)==TOOLONG)
              faterr(13,"string too long in description");
               /*scanstring breaks its output into lines and outputs it
                 as a side effect. More than 30 lines is treated as an
                 error (TOOLONG) */
          scantoken();
          break;
   case KEYWD:
          if (scanlist(KEYWD)!= OK) faterr(14,"Bad keyword list ");
          break;
   case SYSTEM:
          if (scanlist(SYSTEM)!= OK) faterr(15,"Bad System list");
          break;
   case FNAME:
          if (scanfname() != OK) faterr(16,"Poorly formed filename");
          generate(catno,FNAME,lexval);
          scantoken();
          break;
   case WARNING:
          if (scanstring(WARNING)==TOOLONG)
              faterr(17,"string too long in warning field");
          scantoken();
          break;
   case CRC:
          if (scanhex() != OK) faterr(18,"poorly formed CRC number");
          generate(catno,CRC,lexval);
          scantoken();
          break;
   case ALSO:
          if (scanlist(ALSO)!= OK) faterr(19,"Bad See-Also list"); break;
   case AUTHOR:
          if (scanlist(AUTHOR)!= OK) faterr(20,"Bad Author list");
          break;
   case COMP:
          if(scanlist(COMP)!=OK) faterr(21,"Bad Compiler List"); break;
   case REF:
          scanreflist(); /*generates output as side-effect*/
          break;
   default: faterr(22,"Unrecognized attribute");
   }
if (stoptoken != SEMI) faterr(23,"semi-colons must terminate attributes");
}

int scanlist(type)
int type;

{
do {
    if (scanitem()!= OK) {
        errmsg(30,"Item poorly formed");
        return(!OK);
        }
    generate(catno,type,lexval);
    } while (scantoken()==COMMA);
return(OK);
}

scanreflist()

{
do{
   if (scanref() != OK)
      faterr(40,"Reference poorly formed");
   /* generates output as side effect */
   } while (scantoken()== SEMI);
if (stoptoken!=ENDREF)
    faterr(41,"List of references must end with ENDREF keyword");
scantoken(); /*to load semicolon into stoptoken */
}

int scanref()

{
int stoken;                                                    /* CI C86 */
stoken = scantoken();                                          /* CI C86 */
if (stoken==ENDREF) {                                          /* CI C86 */
    errmsg(50,"ENDREF may not follow a : or ;");               /* CI C86 */
    return !OK;                                                /* CI C86 */
    }                                                          /* CI C86 */
/* if (scantoken()!=AUTHOR) { */
if (stoken!=AUTHOR) {                                          /* CI C86 */
    errmsg(50,"reference must begin with AUTHORS:");
    return !OK;
    }
if (scantoken()!=COLON) {
    errmsg(54,"AUTHOR keyword must be followed by colon");
    return !OK;
    }
if (scanlist(REFAUTH)!=OK) {
    errmsg(51,"Bad reference list");
    return !OK;
    }
if (stoptoken!= SEMI){
    errmsg(52,"Authors in reference must end with semicolon");
    return !OK;
    }
if (scantoken() != TITLE) {
    errmsg(52,"Title must follow authors in reference");
    return !OK;
    }
if (scantoken() != COLON) {
    errmsg(53,"TITLE keyword must be followed by colon");
    return !OK;
    }
if (scanstring(REFTTL)==TOOLONG){
    errmsg(55,"title in reference too long--missing end quote?");
    return !OK;
    }
if (scantoken()!= SEMI){
    errmsg(56,"title must end with semicolon in reference");
    return !OK;
    }
if (scantoken()!=CITATION){
    errmsg(57,"CITATION must follow title in reference");
    return !OK;
    }
if (scantoken() != COLON){
    errmsg(58,"Colon must follow keyword CITATION in reference");
    return !OK;
    }
if (scanstring(REFCIT)==TOOLONG){
    errmsg(59,"Citation too long -- missing end quote?");
    return !OK;
    }
return OK;
}

generate(fileid,fldtype,str)
char *fileid, *str;
int fldtype;

{
if (!testonly)
   fprintf(list,"%3d %9s %s\n",fldtype,fileid,ljust(str));
}

int scanstring(boss)
int boss;

{
int i, lines;
int tmp;
char buff[120];
char *tbuf;

lines = 0;
tmp = 0;

if (scantoken() != QUOTE) errmsg(51,"missing opening quote");
do {
   i = 0;
   lines += 1;
   tbuf=buff;
   while ((i++ < 80) && ((tmp = getc(source))!=0x0a)
         && (tmp != '"'))
         if (tmp != 0x0d)  *tbuf++ = tmp;
   *tbuf = 0;
   if (i >= 80) {
      errmsg(52,"string line over 80 chars -- missing end quote??");
      }
   generate(catno,boss,buff);
   } while ((tmp != '"') && (lines < 15));
return (lines > 15)? TOOLONG: OK;
}

int scandate()
{
int i,n,mo,day,year;
char buff[256];
char *tbuf;

n=mo=day=year=i=0;
tbuf=buff;
while (((*tbuf++ = getc(source)) != ';') && (i++ < 250));
if (i > 250) {
   errmsg(55,"unable to find date terminator");
   return !OK;
   }
ungetc(';',source);
*tbuf = 0;
if (debug) printf("scanning /%s/ in date\n",buff);
/* if ((n=sscanf(buff,"%d/%d/%d",&mo,&day,&year))!=3){ */
if ((n=sscanf(buff,"%d/%d/%d",&mo,&day,&year))!=5){         /* CI C86 */
   if (debug) printf("fscanf returns %d with %d %d %d\n",n,mo,day,year);
   return !OK;
   }
if (debug) printf("fscanf returns %d with %d %d %d\n",n,mo,day,year);
if ((mo < 1) || (mo > 12) || (day < 1) || (day > 31)){
   return !OK;
   }
if ((year < 100) && (year >50)) year = 1900 + year;
sprintf(lexval,"%02d/%02d/%04d",mo,day,year);
return OK;
}

int scantoken()

{
int rval;

fillex();
/* if (debug) printf("at scantoken start, lexval=/%s/\n",lexval); */
if (debug) printf("\nat scantoken start, lexval=/%s/",lexval);/* CI C86 */
switch (*lexval){
   case '/': if (getc(source) == '*') rval =  OPENCMNT;
             else rval = FAULT;
             break;
   case '*': if (getc(source) == '/') rval =  ENDCMNT;
             else rval = FAULT;
             break;
   case ',': rval =  COMMA; break;
   case '"': rval =  QUOTE; break;
   case ':': rval =  COLON; break;
   case ';': rval =  SEMI; break;
   default:
      if (!strcmp(lexval,"HEADER")) rval =  HEADER;
      else if (!strcmp(lexval,"TITLE")) rval =  TITLE;
      else if (!strcmp(lexval,"VERSION")) rval =  VERSION;
      else if (!strcmp(lexval,"DATE")) rval =  DATE;
      else if (!strcmp(lexval,"DESCRIPTION")) rval =  DESC;
      else if (!strcmp(lexval,"KEYWORDS")) rval =  KEYWD;
      else if (!strcmp(lexval,"SYSTEM")) rval =  SYSTEM;
      else if (!strcmp(lexval,"FILENAME")) rval =  FNAME;
      else if (!strcmp(lexval,"WARNINGS")) rval =  WARNING;
      else if (!strcmp(lexval,"CRC")) rval =  CRC;
      else if (!strcmp(lexval,"SEE-ALSO")) rval =  ALSO;
      else if (!strcmp(lexval,"AUTHORS")) rval =  AUTHOR;
      else if (!strcmp(lexval,"COMPILERS")) rval =  COMP;
      else if (!strcmp(lexval,"REFERENCES")) rval =  REF;
      else if (!strcmp(lexval,"ENDREF")) rval =  ENDREF;
      else if (!strcmp(lexval,"CITATION")) rval =  CITATION;
      else rval =  ERROR;
      }
stoptoken = rval;
if (debug) printf("\nscantoken returns %d /%s/",rval,lexval);
return rval;
}

scancatno()

{
int n,volume,file;
char *tmp;
char locbuf[512];
char waste[512];
char dot;

tmp=locbuf;
n=510;
do {
    *tmp=getc(source);
    if (!n--) faterr(73,"no semicolon following cat no.");
    }while ( *tmp++ != ';');
ungetc(';',source);
*(--tmp) = 0;
/* if ((n=sscanf(locbuf,"%sCUG%d%c%d",waste,&volume,&dot,&file))!=4){ */
if ((n=sscanf(locbuf,"CUG%d%c%d",&volume,&dot,&file))!=6){     /* CI C86 */
    if (debug) printf("\nfscanf returns %d",n);
/*  if (debug) printf("\nfscanf returns /%s/ %d %d %d",waste,volume,dot,file);*/
    if (debug) printf("\nfscanf returns %d %d %d",volume,dot,file);/* CI C86 */
    faterr(71,"Bad catalog number");
    }
if (volume < 100) errmsg(72,"CUG disks begin at 100!");
if (file > 99) errmsg(74,"File number greater than 100.");
if (dot != '.') errmsg(75,"Period separates disk and file no.");
sprintf(catno,"CUG%03d.%02d",volume,file);
}

int tokentype(tkn)
int tkn;

{
switch (tkn){
   case TITLE:
   case VERSION:
   case DATE:
   case DESC:
   case KEYWD:
   case SYSTEM:
   case FNAME:
   case WARNING:
   case CRC:
   case ALSO:
   case AUTHOR:
   case COMP:
   case REF:
        return ATTRIBUTE;
   default:
        return !ATTRIBUTE;
   }
}

int scanitem()
{
int tmp;
char *tpt;

tpt=lexval;
while (isspace(*tpt =     /* leading white space should */ /* CI C86 */
          getc(source))); /* not be part of the item    */ /* CI C86 */
ungetc(*tpt,source);                                       /* CI C86 */
do {
   stoptoken = *tpt++ = getc(source);
   } while ((stoptoken != ',') && (stoptoken != ';'));
*(--tpt) = 0;
ungetc(stoptoken,source);
tmp=strlen(lexval);
if (debug) printf("\nscanitem /%s/ %d",lexval,tmp);        /* CI C86 */
if ((tmp>40)||(tmp<1)) return !OK;
return OK;
}

fillex()
{
int i;
char *pnt;

if (debug) printf("\nfillex entered");
pnt=lexval;
i=0;
do{
   *pnt=getc(source);
/* if (debug) printf("\n  getc=%02.2x",*pnt); */
   if (debug) printf("\n  getc=%02x",*pnt);              /* CI C86 */
   } while (isspace(*pnt));
if (debug) printf("\nwhitespace loop passed");
stoptoken = *pnt;
if (!isterm(*pnt)){
    do{
        *(++pnt) = getc(source);
       }while (!isterm(*pnt) && (i++ < (LEXSIZE-3)));
    stoptoken = *pnt--;
    ungetc(stoptoken,source);
    }
*(++pnt)='\0';
if (debug) printf("\nfillex exited lexval = /%s/",lexval);
}

faterr(id,str)
int id;
char *str;

{
char *parstask;
switch (parsing){
   case OPENCMNT:
   case HEADER:
   case ATTRIBUTE: parstask = "header beginning"; break;
   case TITLE:     parstask = "title";break;
   case VERSION:   parstask = "version"; break;
   case DATE:      parstask = "date"; break;
   case DESC:      parstask = "description"; break;
   case KEYWD:     parstask = "keywords"; break;
   case SYSTEM:    parstask = "operating systems"; break;
   case FNAME:     parstask = "filename"; break;
   case WARNING:   parstask = "warnings"; break;
   case CRC:       parstask = "crck"; break;
   case ALSO:      parstask = "see-also"; break;
   case AUTHOR:    parstask = "authors"; break;
   case COMP:      parstask = "compilers"; break;
   case REF:       parstask = "references"; break;
   default:        parstask = 0; break;
   }

if (parstask) printf("\n               %3d: %s in %s field",id,str,parstask);
else          printf("\n               %3d: %s",id,str);
longjmp(restart,1);
}

int readwd(pnt)
char *pnt;

{
int i;

i=0;
while (isspace(*pnt = getc(source))) pnt++;
while ((*pnt != '"') && (!isspace(*pnt))){
	pnt++;i++;
	*pnt = getc(source);
	}
ungetc(*pnt, source);
*pnt ='\0';
return i;
}

int scanhex()
{
int val;

if (fscanf(source,"%x",&val)!= 1) return !OK;
sprintf(lexval,"%04x",val);
return OK;
}

int scannumb()

{
int fract;

fillex();

if (!isdigit(lexval[0])) {
   errmsg(95,"version number must begin with digit");
   return !OK;
   }
if (lexval[1] == '.'){
   if (!isdigit(lexval[2])){
      errmsg(90,"digit must follow '.' in version number");
      return !OK;
      }
   if (isdigit(lexval[3]) && isdigit(lexval[4])){
      errmsg(91,"only two digits may follow '.' in version number");
      return !OK;
      }
   return OK;
   }
else if (lexval[2] == '.'){
   if (!isdigit(lexval[3])){
      errmsg(92,"digit must follow '.' in version number");
      return !OK;
      }
   if (isdigit(lexval[4]) && isdigit(lexval[5])){
      errmsg(93,"only two digits may follow '.' in version number");
      return !OK;
      }
   return OK;
   }
else {
   errmsg(94,"version number must contain period");
   return !OK;
   }
}

int scanfname()
{
fillex();
return OK;
}

opensource(str)
char *str;

{
/* fopen(str,source);*/
if ((source = fopen(str,"a")) == NULL)                /* CI C86 */
   abort ("fopen of source file %s failed",str);      /* CI C86 */
}

openlist(str)
char *str;
{
/* fcreat(str,list);*/
if ((list = fopen(str,"w")) == NULL)                 /* CI C86 */
   abort ("fopen of list file %s failed",str);       /* CI C86 */
}

int isspace(ch)
char ch;

{
return (ch <= ' ');
}

int isterm(ch)
char ch;

{
switch (ch) {
   case 0x0d:
   case 0x0a:
   case ' ':
   case ':':
   case ',':
   case '"':
   case ';':
   case '*':
   case '/':
   case 0xff:                                           /* CI C86 */
             return TRUE;
             break;
   default: return FALSE;
   }
}

char *ljust(str)
char *str;

{
while ((*str) && (isspace(*str))) str++;
return str;
}

/* numfile(name,disk,file) */
numfile(name,disk,file,spec)                                  /* CI C86 */
char *name;
char *spec;                                                   /* CI C86 */
int disk, file;

{
char linebuf[512];
/* FILE newfile; */
FILE *newfile;                                                /* CI C86 */
int ch,i;
int st;

i=0;
st=fclose(source);
if (debug) printf("\nnum: close source returns %d",st);
/* st=fopen(name,source);
   if (debug) printf("\nnum: fopen source returns %d",st); */
source = fopen(spec,"r");                                     /* CI C86 */
if (source == NULL) abort("Could not open %s",spec);          /* CI C86 */
if (debug) printf("\nnum: fopen source returns %d",source);   /* CI C86 */
/* st=fcreat("HDR.$$$",newfile);
   if (debug) printf("\nnum: fcreat newfile returns %d",st); */
newfile = fopen("HDR.$$$","w");                               /* CI C86 */
if (newfile == NULL) abort("Could not open HDR.$$$");         /* CI C86 */
if (debug) printf("\nnum: fcreat newfile returns %d",newfile);/* CI C86 */
while((ch = getc(source)) != ':') putc(ch,newfile);
putc(ch,newfile);
fprintf(newfile,"  CUG%03d.%02d;",disk,file);
while ((getc(source) != ';') && (i++ < 200));
if (i >= 200) {
   printf(" ...volume number must end in semicolon");
   st= fclose(newfile);
   if(debug) printf("\nnum: fclose(newfile) returns %d",st);
   st= unlink("HDR.$$$");
   if (debug) printf("\nnum: unlink(temp) returns %d",st);
   st=fclose(source);
   if (debug) printf("\nnum: fclose source returns %d",st);
   return;
   }
while(fgets(linebuf,source)) fputs(linebuf,newfile);
fprintf(newfile,"\n%c",0x1a);
st=fflush(newfile);
if (debug) printf("\nnum: fflush newfile returns %d",st);
st=fclose(newfile);
if (debug) printf("\nnum: fclose newfile returns %d",st);
st=fclose(source);
if (debug) printf("\nnum: last fclose source returns %d",st);
st=rename(name,"NUM.BAK");
if (debug) printf("\nnum: rename source returns %d",st);
st=rename("HDR.$$$",name);
if (debug) printf("\nnum: rename temp returns %d",st);
st=unlink("NUM.BAK");
if (debug) printf("\nnum: unlink bak returns %d",st);
}
                                         