/* CLINK       6-9-85 ported to Mix C  5-31-86       */
/* Linkage of C source code subroutines              */
/* Phillip L. Emerson    Cleveland State University  */

      /* some modifiable parameters */
#define SAME 0
#define LESS -1
#define MORE 1
#define ONE 1
#define VOID -1
#define UNDRSC '\137'   /* ASCII special characters */
#define BELL '\007'
#define APOSTR '\047'
#define QUOTE '\042'
#define BACKSL '\134'
#define LF '\n'
#define SP '\40'
#define BUFSZ 5000       /*must hold any lib. func.  */
#define NAMSIZ 20        /*max length of func. names */
#define HASHSIZ 512      /*size of hash table        */
#define LINSIZ 140       /*source-code max line  ln  */
#define FNAMSZ 20        /*max len. of file names    */
#define OFFSET 20        /*buff offset for backup    */
#define MARGA 10         /*error margin for buff ovfl */
#define MARGB 15         /*error margin for buff ovfl */
#include "stdio"
/*$SIGNEXT*/
/*$ZERO*/
/* Version for Mix C.--  Main changes from C/80 version:
 1. added #include "stdio"
 2. added above two $ comments as Mix compiler parameters
 3. removed #define NULL, which is done in Mix's stdio
 4. changed BUFSIZ to BUFSZ ; BUFSIZ used by Mix's stdio
 5. changed IO type from int to FILE *
 6. changed erroneous instance of linebuff to linebuf
 7. removed #define alloc sbrk & used calloc() instead
*/
char progfil[FNAMSZ],libfil[FNAMSZ],keyfil[FNAMSZ],outfil[FNAMSZ];
char *fubuff, *linebuf ;
FILE *prchan,*lichan,*keychan,*ouchan,*fopen(),*foptst();
int quotes,apostrs,combal;
char *calloc() ;
/* some functs. before main */

/* next f name into r, from string starting
   at p -- return 0 if end reached */
char *funam(p,r) char *p,*r; {char *t,*q,*s,*u; int n;
  u = r ;
loop: s = p; r = u ;
   while ((*p) && (*p != '(' )) p++ ;
   if (*p == NULL) return(NULL) ;
   t = p-- ;
   while ((*p == ' ' ) && (p > s)) p-- ;
   q = p ;
   if( !lglname(*q)) { p = ++t ; goto loop ; }
   while ( lglname(*q) && (q >= s)) q-- ;
   q++ ;
   for(n=1; (n < (NAMSIZ-1)) && (q <= p); *r++ = *q++,n++) ;
   *r = NULL;
   if ((q < s) || (n >= (NAMSIZ-1)) || (n <= 0))
      {putstr("error in processing name\n") ;
       putstr(u); putchar(LF); exit(); }
   if (systn(u)) { p = ++t; goto loop; }
   return(++t) ; }

/* list of commen system function names to ignore */
#define SNAMES 19  /* nr of names*/
char *sysnam(n) int n;
{static char *name[] = {
"if","for","while","fclose","fopen","getc",
"putc","write","read","getchar","putchar",
"alloc","return","main","seek","ftell",
"ftellr","exec","switch" } ; return(name[n]) ; }

/* Mod2 counting of apostrophes and quotes */
char quotap(n) char n; {static char last;
  if(( n == QUOTE) && (last != BACKSL ) && !apostrs)
     quotes = ((quotes+1) % 2) ;
  if(( n == APOSTR) && (last != BACKSL) && !quotes)
     apostrs=((apostrs+1) % 2) ;
  last = n ;
  return(n) ; }

/* allocate space for s, store s, and return ptr to it */
char *strsave(s) char *s; {char *p ;
   if (((p=calloc(1,strlen(s)+1)) == VOID) || (p == NULL))
          return(NULL) ;
   strcpy(p,s); return(p) ; }

/* search for string q on p. rtrn ptr to it if so, null if not*/
char *strison(p,q) char *p,*q; {
   for( ; *p ; p++) {if(strisat(p,q)) return(p) ; }
   return(NULL) ; }

/* standard getch & ungetch to be used in next routine*/
int getctr ;
char getcbuf[5] ;
char getch()
  { if(getctr) return (getcbuf[--getctr]); return(getc(lichan)) ; }
ungetch(a) char a;
  {getcbuf[getctr++] = a; return; }

/* get a char from libchan, ignoring literals & comments */
char gignor()
{ char n,m;
loop1: if ((n=getch()) == VOID) return(VOID) ;
   if((n != '/') || quotes || apostrs) return(quotap(n)) ;
   if((m=getch()) != '*') {ungetch(m) ; return(quotap(n)) ; }
loop2: while (((m=getch()) != '*') && (m != VOID))
        {n=getch(); ungetch(n);
         if((m == '/') && (n == '*')) combal++ ; }
   if (m == VOID) return(VOID) ;
   if((m=getch()) != '/') {ungetch(m) ; goto loop2; }
   goto loop1; }

oulab() {putl("/* file ",ouchan);putl(outfil,ouchan);putl(" */\n",ouchan);}

struct list {
char *hashkey ;
char *defntn ;
struct list *next ; } ;
struct list **table1 ;
struct list **table2 ;

/*  BEGINNING OF MAIN PROGRAM */
main(argc,argv) int argc; char *argv[];
{
  putstr("CLINK  v. 6-9-85 ported to Mix C 5-31-86\n") ;
  linebuf = calloc(1,LINSIZ) ;
  table1 = (struct list **) calloc(1,HASHSIZ*(sizeof(struct list *))) ;
  table2 = (struct list **) calloc(1,HASHSIZ*(sizeof(struct list *))) ;
  tabinit() ;
  fubuff = calloc(1,BUFSZ) ;
  if( (fubuff == NULL) || (fubuff == VOID))
    {putstr("not enough room for buffer\n"); exit(); }
  strcpyn(progfil,argv[1],FNAMSZ-1) ;
  strcpyn(libfil,argv[2],FNAMSZ-1) ;
  strcpyn(keyfil,argv[3],FNAMSZ-1) ;
  strcpyn(outfil,argv[4],FNAMSZ-1) ;
  if (argc >= 2)
      { if ( !strison(progfil,"."))
            strcpy(progfil+strlen(progfil),".B") ;
        if (argc < 5) strcpyc(outfil,progfil,'.') ;
        if (argc < 4) strcpy(keyfil,"LIBKEY.B") ;
        if (argc < 3) strcpy(libfil,"USRLIB.B") ;
        defalt(progfil,keyfil,".B") ;
        defalt(progfil,libfil,".B") ;
        defalt(progfil,outfil,".C") ;
        putchar(LF) ;
        putstr(progfil) ; putstr(" program file\n") ;
        putstr(libfil) ; putstr(" library file\n") ;
        putstr(keyfil) ; putstr(" library key file\n") ;
        putstr(outfil) ; putstr(" output file \n") ;
        putstr("press RETURN (or ENTER) to proceed, Ctrl-C to abort\n") ;
        getchar() ;
        hashup() ;
        prchan = foptst(progfil,"r") ;
        ouchan = foptst(outfil,"w") ;
        oulab() ;
        buildup(1) ;
     }
 else
     {  putstr("\nlibrary file name? ") ;  getstr(libfil) ;
        putstr("\nlibrary keys file name? "); getstr(keyfil) ;
        putstr("wait -- getting library keys\n") ;
        hashup() ;
        putstr("output file name? ") ;  getstr(outfil) ;
        ouchan = foptst(outfil,"w") ; oulab() ;
        for (*progfil='1'; (*progfil) && (*progfil != LF) ; )
          { putstr("input file name? -- ") ;
            putstr("just press RETURN (or ENTER) if no more ") ;
            *progfil = NULL; getstr(progfil) ;
            if ( (*progfil) && (*progfil != LF ) )
              {prchan = foptst(progfil,"r") ;
               putstr("include in output file? (type 0 for no, 1 for yes)\n");
               buildup(getint()) ;
              }
          }
     }
 tidyup() ;
 if (quotes || apostrs || combal)
     {  putchar(LF); putstr("quotes mod2: ") ; printd(quotes) ;
        putchar(LF); putstr("apostrophes mod2: "); printd(apostrs) ;
        putchar(LF) ;
        putstr("occurrences of comment delimiters within comments: ") ;
        printd(combal) ; putchar(LF) ;
     }
 putstr("Linked output file is "); putstr(outfil); putstr("\n\n") ;
}
/*  END OF MAIN PROGRAM */

/*  MANY SUBROUTINES FOLLOW -- some of the more general purpose
    ones would ordinarily reside in a library, but this program
    is intended to be fairly self-contained
*/

/* Manipulation for defaults on file names */
defalt(fpr,fothr,ext) char *fpr,*fothr,*ext;
  {  char *a,temp[FNAMSZ]; int n;
     if( !strison(fothr,":") && (a = strison(fpr,":") ) )
         {  if ( n = a - fpr)
             {  strcpyn(temp,fpr,n+1) ;
                strcpy(temp+n+1,fothr) ;
                strcpy(fothr,temp) ;
             }
         }
     if( !strison(fothr,".") ) strcpy(fothr+strlen(fothr),ext) ;
  }

/* copy t to s up to but not including 1st instance of char c */
strcpyc(s,t,c) char *s,*t,c;
  { while( (*s++ = *t++) != c)  ; *(--s) = NULL ; }

/* tabinit,hash,look,insert are FUNCTIONS FOR HASHING FUNCTION CALLS UNDER
     FUNCTION NAMES
*/

tabinit()
  {int n; for (n=0; n < HASHSIZ ; n++) table1[n] = table2[n]=NULL ;}

hash(s) char *s; {int val;
  for (val=0; *s ;) val += *s++; return(val % HASHSIZ) ; }
struct list *look(s,table) char *s; struct list **table;
  {struct list *p ;
   for (p=table[hash(s)] ; p ; p = p->next )
     { if (strcmp(s,p->hashkey) == SAME) return(p) ; }
   return(NULL) ;
  }
struct list *insert(name,def,table) char *name,*def ;  struct list **table;
  {struct list *n; int val;
   if (n=look(name,table)) return(n) ;
              /* no redefinition of an old one */
   n = (struct list *) calloc(1,sizeof(struct list)) ;
   if ((n == VOID) || (n == NULL)) putstr("hash error#1\n") ;
   if ((n->hashkey = strsave(name)) == NULL) putstr("hash error#2\n") ;
   val = hash(n->hashkey) ;
   n->next = table[val] ;
   table[val] = n ;
   if ((n->defntn = strsave(def)) == NULL) putstr("hash error#3\n") ;
   return(n) ;
  }

/* get byte, starting at staddr, return ptr one past byte position
   in fubuff.  Return VOID if EOF.  Insert NULL at next position
   after byte */
char *getfrst(byte,staddr) char byte,*staddr;
  { char *q;
    for (q=staddr; ( (*q=gignor()) != VOID) && ((*q != byte)
         || quotes || apostrs) && (q < (fubuff+BUFSZ-MARGA)) ;
                 q++)  ;
    if (*q == VOID) return(VOID) ;
    if (q > (fubuff+BUFSZ-MARGB)) {putstr("buf ovrfl\n"); exit() ; }
    *++q = NULL ; return(q) ;
       /* q points one past byte position */
  }
/* Return 1 if c is legal in function name, 0 if not */
/* assumes ASCII */
lglname(c) char c ;
  { if( (c >= 'a') && (c <= 'z') ) return(ONE) ;
    if( (c >= 'A') && (c <= 'Z') ) return(ONE) ;
    if( (c >= '0') && (c <= '9') ) return(ONE) ;
    if( c == UNDRSC) return(ONE) ;
    return(NULL) ;
  }
/* compress func. defs. to func. calls */
char *cmprss(brace1) char *brace1;
  { char *p,*q ;
    *fubuff = '{' ;
    q = fubuff; q++;
    for(p=brace1; p=funam(p,q); q += strlen(q), *q++ = '(')  ;
    *q++ = '}' ; *q = NULL ; redcstr(fubuff+1) ;
  }
/* eliminate duplications on compressed definition string */
redcstr(origin) char *origin ;
  { char *p,*q,*r ;
    for (p=origin; *p != '}' ; p++)
      { if (*p == '(' ) *p = NULL ; }
    for (p=origin; *p != '}' ; )
      {q=p;
       while( *q != '}')
         {while( *q != NULL) q++ ;
          while( *q == NULL) q++ ;
          if( *q != '}')
             { if( strcmp(p,q) == SAME)
                 for(r=q; *r != NULL; r++ ) *r = NULL ;
             }
         }
       while( (*p != NULL) && (*p != '}') ) p++ ;
       while( *p == NULL ) p++ ;
      }
    for (p=origin; *p != '}' ; p++)  {if (*p == NULL) *p = '(' ; }
    deruns('(',origin) ;
  }
/* get next lib fn into fubuff, ret name, and pointer to
   opening brace as value
*/
char *getfu(name) char *name ;
  {char *brace1,*p,*q,*r; int n,braces ;
   for(q=fubuff+OFFSET,r=q; ((q=getfrst('(',q)) != VOID)
       && (funam(r,name) == NULL) ; r=q)  ;
   if(q == VOID) return(VOID) ;
         /* q will point one past the '(' */
   braces = NULL ;
   if( (q=getfrst('{',q)) == VOID) return(VOID) ;
   brace1 = --q; braces = 1 ;
   while (braces > NULL)
     { *++q = gignor() ;
       if (*q == VOID) return(VOID) ;
       if( (*q == '{') && !quotes && !apostrs) braces += 1;
       if( (*q == '}') && !quotes && !apostrs) braces -= 1;
     }
   *++q = NULL ;
   return(brace1) ;
  }
/* return -1 if string is among those listed as sys. funcs,
   0 of not  */
systn(s) char *s ;
  { int k ;
    for(k=0; k < SNAMES; k++)
      if(strcmp(s,sysnam(k)) == SAME) return(VOID);
    return(NULL) ;
  }
/* new hashup() 2-20-85 -- attempts to read keyfile.  If successful
   ok; if not, hashes up from lib file, and puts out new keyfile */
hashup()
  {char *q,name[NAMSIZ]; struct list *j; int flag;
   getctr=0; quotes=0; apostrs=0;
   if( (flag=keyup()) == 1) return(VOID) ;
   if( flag == VOID) {putstr("insert error on keys\n"); exit(); }
   keychan = foptst(keyfil,"w") ;
   lichan = foptst(libfil,"r") ;
   while( (q=getfu(name)) != VOID)
    {cmprss(q) ;
     if( ( (j=insert(name,fubuff,table1)) == NULL) || (j == VOID) )
       {putstr("insert error from fibfil\n") ; exit() ; }
     putl(name,keychan); putc(LF,keychan) ;
     putl(fubuff,keychan); putc(LF,keychan) ;
    }
   fclose(lichan) ;
   fclose(keychan) ; return(VOID) ;
  }
/* tries to open keyfil for input.  If successful, reads in hash table
   and returns 1.  If not, returns 0, but returns -1 if open to read
   is successful and something goes wrong.  */
keyup()
  {struct list *j ;
   keychan = fopen(keyfil,"r") ;
   if( (keychan == NULL) || (keychan == VOID) )
      {fclose(keychan); return(NULL) ; }
   while( (getl0(linebuf,keychan) != VOID) )
     {getl0(fubuff,keychan) ;
      if( (j=insert(linebuf,fubuff,table1)) == NULL ) return(VOID) ;
      if (j == VOID) return(VOID) ;
     }
   fclose(keychan); return(ONE) ;
  }
/* goes thru prog input file recording names of functions called; arg
   =1 for input into output file, n=0 if not */
buildup(n) int n;
  {while(progli(n) != VOID) ; fclose(prchan) ; }

/* processes a line */
progli(n) int n;
  { char *p; static char r[NAMSIZ] ;
    if (getl(linebuf,prchan) == VOID) return(VOID) ;
    if (n) putl(linebuf,ouchan) ;
    for (p=linebuf; p=funam(p,r); netup(r) ) ; return(ONE) ;
  }
/* gets all ramifications of a function call */
/* erasing tracks to prevent endless cycling */
netup(name) char *name ;
  {char *p,*q,r[NAMSIZ] ;  struct list *n;
   insert(name,"1",table2) ;
   if( (n=look(name,table1)) == NULL ) return ;
   p = q = n->defntn ;
   while( p=funam(q,r) )
     {*q = NULL; q = p; if( strcmp(name,r) != SAME ) netup(r) ; }
   return ;
  }
/* goes thru lib file putting out those whose names are recorded */
tidyup()
  {char *p,name[NAMSIZ] ;
   lichan = foptst(libfil,"r") ;
   putl("\n/* routines from library file, ",ouchan) ;
   putl(libfil,ouchan) ; putl("*/\n",ouchan) ;
   putl("/****************************************/\n",ouchan) ;
   getctr = 0; quotes = 0; apostrs = 0; combal = 0;
   while (getfu(name) != VOID)
     { if ( !look(name,table1) )
         {putstr("Warning: lib routine, ");
          putstr(name); putstr(", not indexed.\n") ;
          putchar(BELL) ;
         }
       for ( p=fubuff+OFFSET; *p ; p++)  ;
       *p++ = LF ; *p = NULL ;
       if( look(name,table2) )
         {for (p=fubuff+OFFSET ; *p == LF ; p++ )  ;
          deruns(SP,p); pairdec(SP,LF,p) ; deruns(LF,p) ;
          for ( ; *p ; putc(*p++,ouchan) )  ;
          putl("/**/\n",ouchan) ;
         }
     }
  fclose(ouchan) ;
 }
/* open file if possible, error msg & exit if not */
FILE *foptst(file,rw) char *file,*rw ;
 {FILE *chan;
  chan = fopen(file,rw) ;
  if( (chan == NULL) || (chan == VOID) ) ferror(file) ;
  return(chan) ;
 }
/* error msg & exit on errant fopen */
ferror(s) char *s;
 {int i; for (i=1; i < 100; putchar(BELL),i++) ;
  putstr("can't open "); putstr(s); putchar(LF); exit();
 }
/* copy str t to str s */
strcpy(s,t) char *s,*t;
 {while(*s++ = *t++)  ;
 }
/* length of str s, incl. LF if present */
strlen(s) char *s;
 {char *p; p = s; while(*p) p++; return(p-s);
 }
/* get str fm KB, replace LF with NULL at end*/
getstr(a) char *a;
 {int n; n=0;
  if( (*a=getchar()) == VOID) return(VOID) ;
  if( *a == LF ) {*a=NULL; return(NULL);}
  while( (*++a=getchar()) != LF) ++n ;
  *a = NULL; return(n);
 }
/*put str to screen, no auto LF at end*/
putstr(s) char *s;
 {while(*s) putchar(*s++) ;
 }
/* same as putstr, but to chan */
putl(a,chan) char *a; FILE *chan;
 {while(*a) putc(*a++,chan) ;
 }
/* similar to getstr, but from chan, and with LF followed by NULL */
getl(a,chan) char *a; FILE * chan;
 {int n; char c; n=0;
  if( (c = *a++ = getc(chan)) == VOID) return(VOID) ;
  if( c == LF) {*a = NULL; return(ONE) ; }
  while( (*a=getc(chan)) != LF ) ++a,++n ;
  *++a = NULL; return(++n) ;
 }
/* same as getl but with LF replaced by null */
getl0(a,chan) char *a; FILE * chan;
 {int n; n=getl(a,chan) ; if (n == VOID) return(VOID);
  *(a+n) = NULL ; return(n);
 }
/* cmp strs p and q; return -1,0,1, for p<q,p=q,p>q */
strcmp(p,q) char *p,*q;
 {while(*p == *q) {if (*p++ == NULL) return(SAME) ; else ++q; }
  return(*p < *q ? LESS:MORE) ;
 }
/* change runs of c to single c thruout s */
deruns(c,s) char c,*s;
 {pairdec(c,c,s) ;
 }
/* change ab to b thruout s */
pairdec(a,b,s) char a,b,*s;
 {char *t;
  for (t=s; *s = *t; )
    if( (*++t != b) || (*s != a) ) s++ ;
 }
/* print signed dec int -- assumes ASCII */
printd(n) int n;
 {int i;
  if(n < 0) {putchar('-'); n = -n ; }
  if( (i=n/10) != 0) printd(i) ;
  putchar(n % 10 + '0') ;
 }
/*copy 1st n bytes of t to s, insert NULL at end*/
strcpyn(s,t,n) char *s,*t; int n;
 {for ( ; n ; n--) *s++ = *t++ ; *s = NULL ;
 }
/*test if str q is at p. rtrn 1 is so, 0 if not*/
strisat(p,q) char *p,*q ;
 {while( *p && *q && (*q == *p) ) p++,q++ ;
  return(*q ? NULL:ONE) ;
 }
/* get a pos. int from kb */
getint()
 {char c; int n;
  for(n=0; isadig(c=getchar()); ) n=10*n + c - '0' ;
  return(n) ;
 }
/* test if byte is a digit */
isadig(c) char c;
 {return( ((c >= '0') && (c <= '9')) ? ONE:NULL ) ;
 }

