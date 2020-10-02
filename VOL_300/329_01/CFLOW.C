/*  >  cFlow.c
 *
 *  cFlow -- Print function dependency tree
 *           from multiple C source files
 *  (C) October 18  1989  Asaf Arkin
 *  All rights reserved
 */



/*  Include files:
 */

#include  <ctype.h>
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>


/*  Macro constants:
 *    True/False; function state mask bits; constant values.
 */

#define  TRUE  1
#define  FALSE  0

#define  DEFINED  01    /*  State: function defined          */
#define  PRINTED  02    /*  State: function already printed  */

#define  ID_MAX  512    /*  Maximum identifier length  */
#define  IND_ADD  2     /*  Indentation increment      */


/*  Function declarations:
 */

int   Error(char *, ...);
void  Syntax(void);
void  Process(char *);
void  SkipQuote(FILE *, int *, int);
void   DoFunc(int, FILE *, int *, int);
void  *ScanFunc(char *);
int   GetChar(FILE *, int *);
void  DumpFlow(char *);
void  DumpFunc(void *, int, int);


/*  Structure strFunc: holds a function's state, line number, filename
 *  offset, pointer to definition text, and offsets to invoked functions.
 */

typedef struct
{
  char      State;
  int       LineNumber;
  unsigned  FileName;
  char     *Definition;
  unsigned  CallCnt;
  unsigned  Calls;
}  strFunc;


/*  Structure strList: holds an offset to function's name and a pointer to
 *  function's strFunc structure.
 */

typedef struct
{
  int       Name;
  strFunc  *Struct;
}  strList;


/*  Static variables:
 *    Memory blocks: ID, File, Name, List.
 *    Pointer to Func block of currently defined function.
 *    Undefined (options -u) and include (option -i) flags.
 *    cFlow output stream.
 */

char     *IDName;
int       IDLen;
char     *FileBase = NULL;
int       FileLen = 0;
char     *NameBase = NULL;
int       NameLen = 0;
strList  *ListBase = NULL;
int       ListLen = 0;
strFunc  *CurFunc = NULL;
char      UnDefined = 0,  Include = FALSE;
FILE     *OutFile = stdout;



/*  int  main(int, char **)
 *
 *  Parse command line off options. That done, consume filenames from command
 *  line, processing them with Process(). Finally, print dependency tree.
 */

int  main(int Argc, char **Argv)
{
  int  Arg,  Offset = 0;
  char  *Ptr;
  char  *MainFunc = "main";

  /*  Print cFlow title. Allocate memory for ID (constant size) and quit if
   *  failed to. In case of no command-line arguments, print syntax of cFlow
   *  and quit.
   */
  printf("cFlow   (C) Oct 17 '89  by Asaf Arkin\n\n");
  if (( IDName=malloc(ID_MAX) )==NULL)
    return  Error("Cannot allocate memory to begin with");
  if (Argc<2)
  {
    Syntax();
    return  0;
  }
  for (Arg=1; Arg<Argc; ++Arg)
  {
    Argv[Arg]=Argv[Arg+Offset];
    if (Argv[Arg][0]=='-')
    {
      for (Ptr=Argv[Arg]+1; *Ptr; ++Ptr)
        switch (tolower(*Ptr))
        {
          case 'i':
            Include=TRUE;
            break;
          case 'm':
            if (!*++Ptr)
            {
              if (Arg+1<Argc)
              {
                --Argc;
                Ptr=Argv[Arg+ ++Offset];
              }
              else
              {
                Error("-m<main>  First function in report is <main>\n");
                break;
              }
            }
            MainFunc=Ptr;
            while (*++Ptr)  ;
            --Ptr;
            break;
          case 'o':
            if (!*++Ptr)
            {
              if (Arg+1<Argc)
              {
                --Argc;
                Ptr=Argv[Arg+ ++Offset];
              }
              else
              {
                Error("-o<file>  Send output to <file>");
                break;
              }
            }
            if (( OutFile=freopen(Ptr,"w",OutFile) )==NULL)
              return  Error("Cannot open file '%s' for output",Ptr);
            while (*++Ptr)  ;
            --Ptr;
            break;
          case 'u':
            ++UnDefined;
            break;
          case '-':
            break;
          default:
            Error("Option -%c unknown",*Ptr);
        }
      --Argc;
      --Arg;
      ++Offset;
    }
  }
  for (Arg=1; Arg<Argc; ++Arg)
    Process(Argv[Arg]);
  DumpFlow(MainFunc);
  return  0;
}


/*  int  Error(char *, ...)
 *
 *  Issue an error (printf-style arguments) and return zero.
 */

int  Error(char *Message, ...)
{
  va_list  Args;

  va_start(Args,Message);
  printf("cFlow: ");
  vprintf(Message,Args);
  printf(".\n");
  va_end(Args);
  return  0;
}


/*  void  Syntax(void)
 *
 *  Print cFlow's syntax.
 */

void  Syntax(void)
{
  char  *Message =
    "Syntax:   cFlow  [<file>|-i|-m<main>|-o<file>|-u|-v]...\n"
    "Options:  -i        Read include files\n"
    "          -m<main>  First function in report is <main>\n"
    "          -o<file>  Send output to <file>\n"
    "          -u        Show undefined functions (2 levels)\n\n";

  printf("%s",Message);
}



/*  void  Process(char *)
 *
 *  Parse function definitions and invokations from the given C source file
 *  and build from it the dependency tree.
 */

void  Process(char *FileName)
{
  int  c,  LineCnt = 0,  Level = 0;
  char  *Ptr,  *EndPtr;
  FILE  *File;

  if (( File=fopen(FileName,"r") )==NULL)
  {
    Error("Cannot open source file '%s'",FileName);
    return;
  }
  /*  If file has not been openned before, append its name to File block,
   *  else quit (no point in reading a file twice.) Set FileName to the
   *  filename's offset within File.
   */
  Ptr=FileBase;
  EndPtr=Ptr+FileLen;
  for (; Ptr<EndPtr; Ptr+=strlen(Ptr)+1)
    if (!strcmp(FileName,Ptr))
      return;
  if (( Ptr=realloc(FileBase, FileLen+strlen(FileName)+1) )!=NULL)
  {
    FileName=strcpy(Ptr+FileLen,FileName);
    FileBase=Ptr;
    FileLen+=strlen(FileName)+1;
  }
  /*  Flush ID and start parsing the source file.
   */
  IDLen=0;
  c=GetChar(File,&LineCnt);
  while (!feof(File))
  {
    if (c==' ')
    {
      /*  Skip spaces, tabs and line delimiters.
       */
      c=GetChar(File,&LineCnt);
      continue;
    }
    if (isalpha(c) || c=='_' || c=='*')
    {
      /*  Identifier: consume as many characters as possible into ID. *'s
       *  are read because they are part of definitions (pointer to..).
       */
      while (c=='*')
      {
        IDName[IDLen++]=c;
        c=GetChar(File,&LineCnt);
      }
      if (isalpha(c) || c=='_')
      {
        do
        {
          IDName[IDLen++]=c;
          c=GetChar(File,&LineCnt);
        }
        while (isalnum(c) || c=='_');
        IDName[IDLen++]=' ';
      }
      continue;
    }
    if (c=='(' && IDLen)
    {
      /*  Openning parentheses: could be a function call/definition.
       */
      DoFunc(FileName-FileBase,File,&LineCnt,Level);
      IDLen=0;
      c=GetChar(File,&LineCnt);
      continue;
    }
    /*  Skip literal strings and character constants.
     *  Count left and right braces to determine nesting level.
     */
    if (c=='\'' || c=='\"')
      SkipQuote(File,&LineCnt,c);
    else
    if (c=='{')
      ++Level;
    else
    if (c=='}')
      if (--Level<0)
        Error("Source error: too many }'s do not balance");
    IDLen=0;
    c=GetChar(File,&LineCnt);
  }
  /*  File parsed through. Close it an return.
   */
  if (fclose(File))
    Error("Source file '%s' not closed",FileName);
}


/*  void  SkipQuote(FILE *, int *, int)
 *
 *  Skip literal strings ("...") and character constants ('.').
 */

void  SkipQuote(FILE *File, int *LineCnt, int c)
{
  int  Quote;

  Quote=c;
  do
  {
    c=getc(File);
    if (c=='\n' || c=='\r' || c=='\v' || c=='\f')
      ++*LineCnt;
    else
    if (c=='\\')
    {
      c=getc(File);
      if (c=='\n')
        ++*LineCnt;
      else
        c=getc(File);
    }
  }
  while (c!=Quote && !feof(File));
}


/*  int  GetChar(FILE *, int *)
 *
 *  Read a character from the source file: whitespaces, line delimiters and
 *  comments read as spaces; \NL sequences are ignored; preprocessor
 *  directives are ignored, except for #include.
 */

int  GetChar(FILE *File, int *LineCnt)
{
  int  c,  Next;
  char  FileName[21];

GetCharNext:
  switch (c=fgetc(File))
  {
    case ' ':
    case '\t':
      return  ' ';
    case '\n':
    case '\r':
    case '\v':
    case '\f':
      ++*LineCnt;
      while (c=fgetc(File), c==' ' || c=='\t')  ;
      if (c=='#')
      {
        /*  If #include directive and -i option specified, open include file,
         *  and read its contents with Process().
         *  A preprocessor line is skipped in whole.
         */
        while (c=fgetc(File), c==' ' || c=='\t')  ;
        if (Include && c=='i')
          if (fscanf(File, "nclude %*1[<\"] %20[^>\" ]",FileName )==1)
            Process(FileName);
        do
        {
          if (c=='\\' || c=='/')
          {
            ungetc(c,File);
            c=GetChar(File,LineCnt);
          }
          c=fgetc(File);
        }
        while (!feof(File) && c!='\n' && c!='\r' && c!='\v' && c!='\f');
        ungetc(c,File);
        return  ' ';
      }
      ungetc(c,File);
      return ' ';
    case '\\':
      c=fgetc(File);
      if (c=='\n')
      {
        ++*LineCnt;
        goto  GetCharNext;
      }
      ungetc(c,File);
      return  '\\';
    case '/':
      c=fgetc(File);
      if (c=='*')
      {
        Next='*';
        do
        {
          c=fgetc(File);
          if (c=='\n' || c=='\r' || c=='\v' || c=='\f')
            ++*LineCnt;
          if (c=='/' && Next=='/')
            return  ' ';
          if (c=='*')
            Next='/';
          else
            Next='*';
        }
        while (!feof(File));
        return  EOF;
      }
      ungetc(c,File);
      return  '/';
    default:
      return  c;
  }
  return  c;
}



/*  List of C keywords that may be mistaken as function names.
 */

char  *KeyWords[] =
{
  "typedef",  "switch",  "sizeof",  "case",  "do",
  "while",    "return",  "else",    "for",   "if"
};

/*  void  DoFunc(int, FILE *, int *, int)
 *
 *  Create a function definition/declaration, or add its invokation to
 *  another function's definition.
 */

void  DoFunc(int FileName, FILE *File, int *LineCnt, int Level)
{
  int  c,  Prev,  Cnt;
  int  LineNumber = *LineCnt,  Paren = 0;
  char  *Ptr,  *Name;
  char  *DefBase = NULL;
  int    DefLen = 0;
  strFunc  *sPtr;
  unsigned  Temp;

  /*  Seperate function name from rest of identifiers contained in ID.
   *  Check that function is not a keyword and return if so.
   */
  Name=IDLen+IDName;
  *Name='\0';
  if (Name[-1]==' ')
    *--Name='\0';
  while (Name-->IDName && *Name!=' ' && *Name!='*')  ;
  if (*++Name=='\0')
    return;
  Cnt=sizeof(KeyWords)/sizeof(char *);
  while (Cnt-->0)
    if (! strcmp(Name,KeyWords[Cnt]) )
      return;
  /*  Call ScanFunc to receive a pointer to the function's Func block.
   *  If nesting level is zero, function is either defined or declared;
   *  If level is more than zero, function is invoked.
   */
  if (( sPtr=ScanFunc(Name) )==NULL)
    return;
  if (Level==0)
  {
    /*  Read formal parameters and make them part of definition, along with
     *  return value contained in ID.
     */
    if (( Ptr=malloc(Name-IDName) )!=NULL)
      memmove(DefBase=Ptr, IDName, DefLen=Name-IDName);
    Prev= c='(';
    IDLen=0;
    do
    {
      if (c=='(')
        ++Paren;
      if (c==')')
        --Paren;
      if (!(c==' ' && Prev==' '))
        IDName[IDLen++]=c;
      Prev=c;
      c=GetChar(File,LineCnt);
    }
    while (Paren && !feof(File));
    if (c==' ')
      while (( c=GetChar(File,LineCnt) )==' ')  ;
    IDName[IDLen++]='\0';
    if (( Ptr=realloc(DefBase,DefLen+IDLen) )!=NULL)
      memmove(DefLen+( DefBase=Ptr ),IDName,IDLen);
    if (c==';' || c==',')
    {
      /*  Definition ends with ; or ,: it is a mere declaration.
       */
      if (sPtr->Definition==NULL)
        sPtr->Definition=DefBase;
      else
        free(DefBase);
      while (c!=';' && !feof(File))
        c=GetChar(File,LineCnt);
    }
    else
    {
      /*  Set information relevant to definition. Then skip everything up to
       *  first { (or: skip the parameters of an unprototyped function.)
       */
      CurFunc=sPtr;
      sPtr->State|=DEFINED;
      sPtr->LineNumber=LineNumber;
      sPtr->FileName=FileName;
      if (sPtr->Definition!=NULL)
        free(sPtr->Definition);
      sPtr->Definition=DefBase;
      if (c!='{')
        while (c!='{' && !feof(File))
          c=GetChar(File,LineCnt);
      ungetc(c,File);
    }
    return;
  }
  /*  Function invokation: add offset-to-List-entry to Calls list of
   *  currently defined function (the definition this invokation is part of);
   *  no offset is added more than once.
   *  Note: The parameters list is not consumed, for it may contain
   *  additional function invokations.
   */
  if (CurFunc==NULL)
    return;
  Temp=sPtr->Calls;
  for (Cnt=0; Cnt<CurFunc->CallCnt; ++Cnt)
    if ( ( (unsigned *) (CurFunc+1) )[Cnt]==Temp)
      return;
  sPtr=realloc(CurFunc, sizeof(strFunc)+(Cnt+1)*sizeof(unsigned) );
  if (sPtr)
  {
    (ListBase+sPtr->Calls)->Struct= CurFunc=sPtr;
    ( (unsigned *) (sPtr+1) )[CurFunc->CallCnt++]=Temp;
  }
  return;
}


/*  void  *ScanFunc (char *)
 *
 *  Return pointer to function's Func block: create one if inexistent.
 */

void  *ScanFunc(char *Name)
{
  int  Length = strlen(Name)+1;
  char  *Ptr,  *Ptr2,  *EndPtr;
  strList  *lPtr;
  strFunc  *sPtr;

  /*  Search Name for function's name: if exists, use matching List entry.
   */
  Ptr=NameBase;
  EndPtr=Ptr+NameLen;
  for (lPtr=ListBase; Ptr<EndPtr; ++lPtr)
  {
    if (*Name==*Ptr++)
    {
      Ptr2=Name+1;
      while (*Ptr==*Ptr2 && *Ptr)
        ++Ptr,  ++Ptr2;
      if (*Ptr=='\0' && *Ptr2=='\0')
        break;
    }
    while (*Ptr++)  ;
  }
  /*  If function is encountered for first time, append its name to Name, and
   *  append an entry to List. Otherwise, return pointer to Func block.
   */
  if (Ptr==EndPtr)
  {
    if (( Ptr=realloc(NameBase,NameLen+Length) )==NULL)
      return  NULL;
    strcpy(Ptr+NameLen,Name);
    NameBase=Ptr;
    NameLen+=Length;

    if (( lPtr=realloc(ListBase, (ListLen+1)*sizeof(strList) ) )==NULL)
      return  NULL;
    ListBase=lPtr;
    lPtr+=ListLen;
    ++ListLen;
    lPtr->Name=NameLen-Length;
    lPtr->Struct=NULL;
  }
  else
  if (lPtr->Struct)
    return  lPtr->Struct;
  /*  Allocate Func block for function, setting its status to undefined.
   */
  if (( sPtr=malloc(sizeof(strFunc)) )==NULL)
    return  NULL;
  sPtr->State=0;
  sPtr->Definition=NULL;
  sPtr->CallCnt=0;
  sPtr->Calls=lPtr-ListBase;
  lPtr->Struct=sPtr;
  return  sPtr;
}



/*  void  DumpFlow(char *)
 *
 *  Print definition of main() (or any other particular function), followed
 *  by definitions of all other defined functions. If the -u option appeared
 *  twice, also list all undefined functions.
 */

void  DumpFlow(char *MainFunc)
{
  strList  *lPtr,  *lEndPtr;

  putc('\n',OutFile);
  lPtr=ListBase;
  lEndPtr=lPtr+ListLen;
  for (; lPtr<lEndPtr; ++lPtr)
    if (! strcmp(MainFunc,NameBase+lPtr->Name) )
    {
        DumpFunc(lPtr,0,FALSE);
        break;
    }

  for (lPtr=ListBase; lPtr<lEndPtr; ++lPtr)
    if (!( lPtr->Struct->State&PRINTED ))
      DumpFunc(lPtr,0,FALSE);
  putc('\n',OutFile);

  if (UnDefined<2)
    return;
  fprintf(OutFile,"\n**  Undefined functions appearing in source:  **\n");
  for (lPtr=ListBase; lPtr<lEndPtr; ++lPtr)
    if (!( lPtr->Struct->State&PRINTED ))
      DumpFunc(lPtr,0,TRUE);
  putc('\n',OutFile);
}


/*  void  DumpFunc(void *, int, int)
 *
 *  Print function's name and definition. If function is defined, also print
 *  line number, filename, and list all the functions it calls: call DumpFunc
 *  itself for each one.
 */

void  DumpFunc(void *Ptr, int  Indent, int PrintUnDefined)
{
  int  Cnt,  i;
  strList  *lPtr = Ptr;
  strFunc  *sPtr = lPtr->Struct;

  if (sPtr->State&DEFINED || PrintUnDefined)
  {
    for (i=0; i<Indent; ++i)
      putc(' ',OutFile);
    fprintf(OutFile,"%s",NameBase+lPtr->Name);
    if (sPtr->Definition)
      fprintf(OutFile,": %s",sPtr->Definition);
    if (sPtr->State&DEFINED)
      fprintf(OutFile,",  <%s %u>\n",FileBase+sPtr->FileName,
                                     sPtr->LineNumber+1);
    else
      putc('\n',OutFile);
    /*  If function has been printed before, do not list the functions it
     *  calls once more. Prior to listing, set function's state to printed,
     *  so recursion in source code will not result in an infinite loop.
     */
    if (sPtr->State==DEFINED)
    {
      sPtr->State|=PRINTED;
      for (Cnt=0; Cnt<sPtr->CallCnt; ++Cnt)
      {
        lPtr=ListBase+( (unsigned *) (sPtr+1) )[Cnt];
        if (lPtr->Struct->State&DEFINED || UnDefined)
          DumpFunc(lPtr,Indent+IND_ADD,TRUE);
      }
    }
  }
}


