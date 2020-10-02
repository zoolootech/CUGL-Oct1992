/*
  Program Name : spell.c
  Author       : Kenji Hino
  Descriptiton : It finds spelling errors and outputs error on the display.
  Compiler     : Microsoft C compiler ver 4.0
                 in small model, compact model, and huge model
*/

#include <stdio.h>
#include <ctype.h>
#include <malloc.h>

#define MAX_DIR_ENTRIES  10000       /* The maximum number of keys  in index */
#define MAXWORD          30         /* The maximum number of chars per word */
#define MAXLINE          100        /* The maximum number of chars per line */
#define DICTIONARY_FILE  "dict"
#define DICTIONARY_INDEX "dictdx"
#define DEFAULT_DIF      5         /* pick up a key every 10 words in dictionary */
#define OK               1
#define YES              1
#define NO               -1
#define ERROR            -1

FILE *dunit,                         /* file descriptor for dictionary file */
     *dx;                            /* file descriptor for index file */
int nlines = 0,                      /* number of index entries from index file */
    freep = 0;                       /* next available index entry */
struct free_list
         {
         char *key;                  /* address of key in main mem */
         long addr;                  /* file pointer where a word is stored */
         } index[MAX_DIR_ENTRIES];

main(argc, argv)
int argc;
char **argv;
{
FILE *unit;
void lodidx(),cant(),dospel();

lodidx();                             /* load dictionary index */
while ( argc-- != 1 )
  {
  argv++;
  if ((unit = fopen(*argv,"r")) == NULL)
    cant(*argv);
  else
    {
    dospel(unit);                     /* do spell checking */
    fclose(unit);
    }
  }
} /* end of main */

void lodidx()
{
void errormes(),cant(),dodx();
int inject();
long addr;
char key[MAXWORD];

if ((dunit = fopen(DICTIONARY_FILE, "r")) == NULL)
  cant(DICTIONARY_FILE);

if ((dx = fopen(DICTIONARY_INDEX, "r")) == NULL)
  dodx(DICTIONARY_INDEX);                       /* create index file */
else
  while ( fscanf(dx,"%s%lx",key,&addr) != EOF )
    {
    if (inject(key,addr) == ERROR)              /* create index in main mem */
      {
      errormes("dictinary index too long or run out of memory !!");
      break;
      }
    };
fclose(dx);
} /* end of lodidx */

int inject(key,addr)      /* injext a key into mian mem */
char *key;
long addr;
{
char *str,*malloc();

if ((freep >= MAX_DIR_ENTRIES) || ((str = malloc(strlen(key) + 1)) == NULL))
  return(ERROR);

strcpy(str,key);
index[freep].key = str;
index[freep].addr = addr;
freep++;
nlines++;
return(OK);
} /* end of inject */

void dodx(name)     /* create index file */
char *name;
{
void cant(),errormes(), outlin();
int doline(), inject();
char key[MAXWORD];
long addr = 0;
long n = 0;

if ((dx = fopen(name,"w")) == NULL)
  cant(name);

addr = ftell(dunit);
while (fscanf(dunit, "%s",key) != EOF)
  {
  n++;
  if (doline(n,DEFAULT_DIF) == YES)
    {
    outlin(key,addr,dx);
    if (inject(key,addr) == ERROR)
      errormes("dictionary index too large or run out of memeory !!");
    }
  addr = ftell(dunit);
  }
} /* end of dodx */

int doline(n,dif)        /* decide whether pick  up a key or not */
long n;
int dif;
{

if (dif == 1)
  return(YES);
else if ((n%dif) == 1)
       return(YES);
     else
       return(NO);
} /* end of doline */

void outlin(word, addr,out)    /* write index into a index file */
char *word;
long addr;
FILE *out;
{
fprintf(out,"%s %lx\n",word,addr);
} /* end of outlin */

void dospel(unit)             /* do spell checking */
FILE *unit;
{
char errbuf[MAXLINE];       /* to show error on stdout */
char word[MAXWORD];
char buf[MAXLINE];
int  gtword(), findwd();
int i,j,start,iferr;

while (fgets(buf,MAXLINE,unit) != NULL)       /* get a line from a target file */
  {
  fprintf(stdout,"%s",buf);
  for(j=0; buf[j] != '\0'; j++)              /* set up error buf */
    if (buf[j] == '\t')
      errbuf[j] = '\t';
    else if (buf[j] == '\n')
           errbuf[j] = '\n';
         else errbuf[j] =' ';
  errbuf[j] = '\0';

  iferr = NO;
  start = i = 0;
  while (gtword(buf, &i, word, &start) > 0)     /* get a word */
    if (findwd(word) == NO)                     /* search the word */
      {
      iferr = YES;
      for(j=start; j < i; j++)               /* make errbuf */
        errbuf[j] = '*';
      };
  if (iferr == YES)
    fprintf(stdout,"%s",errbuf);
  } /* while end */
} /* end of dospel */

int gtword(buf,i,word,start)     /* get a word from a line */
char *buf,*word;
int *i,*start;
{
int j,alphan();

while (alphan(buf[*i]) == NO)             /* skip non-alphan */
  if (buf[*i] == '\0')
    return(0);
  else
    (*i)++;
*start = *i;
for(j=0; alphan(buf[*i]) == YES; j++)    /* get a word */
  {
  word[j] = buf[*i];
  (*i)++;
  }
word[j] = '\0';
return(strlen(word));
} /* end of gtword */

int alphan(ch)
char ch;
{
int c;

c = ch;
if ((isalpha(c) != 0) || (isdigit(c) != 0))
  return(YES);
else
  return(NO);
} /* end of alphan */

int findwd(word)
char *word;
{
int i,cmp;
void wdstal();
int wdlook(),binsrc();
char buf[MAXLINE];

if (wdlook(word) == YES)      /* seen this mis-spelled word before */
  return(NO);
i = binsrc(word);             /* do binary search */
fseek(dunit,index[i].addr,SEEK_SET); /* move file pointer in dictionary file */
while (fscanf(dunit, "%s",buf) != EOF)
  if ((cmp =strcmpi(word,buf)) == 0)  /* strcmpi is case-insensitive */
    return(YES);
  else if (cmp < 0)
         {
         wdstal(word);        /* install mis-spelled word */
         return(NO);
         };
wdstal(word);
return(NO);
} /* end of fdword */

int binsrc(word)       /* do binary search */
char *word;
{
int i,last,first,cmp;

if (strcmpi(word,index[nlines-1].key) > 0)   /* if a word is greater than the greatest word in index */
  return(nlines-1);
if (strcmpi(word,index[0].key) < 0)       /* if a word is less than smallest word */
  return(0);
first = 0;
last = nlines;
while ((last - first) > 1)
  {
  i = (first+last)/2;
  if ((cmp = strcmpi(word,index[i].key)) < 0)
    last = i;
  else if (cmp == 0)
         {
         last = i;
         first = i;
         }
       else if (cmp > 0)
              first =i;
  } /* while end */
return(first);
} /* end of bisrc */

int wdlook(word)
char *word;
{
int i;

for(i=nlines; i < freep; i++)
  if (strcmpi(word,index[i].key) == 0)
    return(YES);
return(NO);
} /* end of wdlook */

void wdstal(word)
char *word;
{
char *str;

if ((freep <= MAX_DIR_ENTRIES) && ((str = malloc(strlen(word) + 1)) != NULL))
  {
  strcpy(str,word);
  index[freep++].key = str;
  }
} /* end of wdstal */

void cant(name)
char *name;
{
fprintf(stderr,"Can't open %s!\n",name);
exit(1);
}

void errormes(message)
char *message;
{
fprintf(stdout,"%s\n",message);
}
