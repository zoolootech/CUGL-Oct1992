/*  >  DEFINE.C
 *
 *  Define -- Construct C definitions from
 *  descriptions given in plain English
 *  (C)  October 17  1989  by Asaf Arkin
 *  All rights reserved
 */



/*  Include files:
 */

#include  <ctype.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>


/*  Macro constants:- True/False; Token types.
 */

#define  FALSE  0
#define  TRUE   1

#define  POINTER   010
#define  FUNCTION  020
#define  FUNCACPT  021
#define  ARRAY     030
#define  ARRAYSZ   031
#define  OTHER     000


/*  Static variable definitions:
 *    Token is the last token read by GetToken().
 *    DefinBase points to the first char of the definition, DefinLen is the
 *    definition's length, and InsertAt is the offset to DefinBase at which
 *    to Insert() new characters.
 */

static char  Token;
static char  *DefinBase;
static int    DefinLen,  InsertAt;


/*  Structure containing illegal combinations: array of func, func returning
 *  array, and func returning func. Define will report, if you attempt any of
 *  these in a definition.
 */

struct
{
  char  Prev,  Cur,  *Msg;
}  Error[3] =
{
  ARRAY, FUNCTION, "array of function returning",
  FUNCTION, ARRAY, "function returning array of",
  FUNCTION, FUNCTION, "function returning function"
};


/*  Function declarations:
 */

extern void  Prepare(char *);
extern char  *Define(char *);
extern char  *FuncAccept(char *);
extern void  Insert(char);
extern char  *GetToken(char *);
extern char  *Compare(char *, char *, int);



/*  PROGRAM: Define.
 */

void  main(void)
{
  char  *Text,  *Ptr,  Buffer[256];

  while (TRUE)
  {
    printf("English: ");
    if (!( Text=gets(Buffer) ))
      break;
    Prepare(Text);
    if (Define(Text))
    {
      printf("C Defin: %s;\n",DefinBase);
      free(DefinBase);
    }
  }
}


/*  void  Prepare(char *)
 *
 *  Prepare English description: collapse multiple spaces and tabs into a
 *  single space, and replace line delimiters with a null.
 */

void  Prepare(char *Text)
{
  int  Ofst = 0;

  while (*Text)
  {
    if (Text[Ofst]==' ' || Text[Ofst]=='\t')
    {
      *Text++=' ';
      while (Text[Ofst]==' ' || Text[Ofst]=='\t')
        ++Ofst;
    }
    if (Text[Ofst]=='\n' || Text[Ofst]=='\r' || Text[Ofst]=='\0')
    {
      *Text='\0';
      return;
    }
    *Text=Text[Ofst];
    ++Text;
  }
}


/*  char  *Define(char *)
 *
 *  Construct C definition from description given in plain English.
 *  Define() returns a pointer to the end of the English text.
 */

char  *Define(char *Text)
{
  char  *Ptr;
  char  Prev,  Cur,  i;

  DefinBase=NULL;
  DefinLen= InsertAt=0;
  Prev= Cur=OTHER;

  /*  Copy identifier name (if present) to start of newly created memory
   *  block -- around this name the definition will be built.
   */
  if (*Text==' ')
    ++Text;
  GetToken(Text);
  if (Token==OTHER)
    while (isalnum(*Text) || *Text=='_')
      Insert(*Text++);
  if (Ptr=Compare(Text,"is",2))
    Text=Ptr;
  /*  Construct rest of definition: prefix and postfix identifier name with C
   *  tokens in accordance with the description.
   *  Also, report any illegal combinations found in the definition.
   */
  while (*Text)
  {
    if (*Text==' ')
      ++Text;
    Text=GetToken(Text);
    Prev=Cur;
    Cur=Token & 070;
    for (i=0; i<3; ++i)
      if (Prev==Error[i].Prev && Cur==Error[i].Cur)
        printf("Error: %s ... Illegal!\n",Error[i].Msg);

    InsertAt=DefinLen;
    switch (Token)
    {
      case POINTER:
        /*  Pointer:- if pointer to function or array, enclose definition
         *  with (..). In any case, prepend * to definition.
         */
        if (Compare(Text,"function",4) || Compare(Text,"array",5))
        {
          Insert(')');
          InsertAt=0;
          Insert('(');
        }
        else
          InsertAt=0;
        Insert('*');
        break;
      case FUNCTION:
        /*  Function:- append () to definition.
         */
        Insert('(');
        Insert(')');
        break;
      case FUNCACPT:
        Text=FuncAccept(Text);
        if (Ptr=Compare(Text,"returning",6))
          Text=Ptr;
        break;
      case ARRAY:
        /*  Array:- append [] to definition.
         */
        Insert('[');
        Insert(']');
        break;
      case ARRAYSZ:
        /*  Sized array:- append [size] to definition.
         */
        Insert('[');
        if (*Text==' ')
          ++Text;
        while (isalnum(*Text) || *Text=='_')
          Insert(*Text++);
        Insert(']');
        if (Ptr=Compare(Text,"of",2))
          Text=Ptr;
        break;
      default:
        /*  Data type:- insert data type at start of definition and quit.
         */
        InsertAt=0;
        while (isalnum(*Text) || *Text=='_' || *Text==' ')
          Insert(*Text++);
        Insert(' ');
        InsertAt=DefinLen;
        Insert('\0');
        return  Text;
    }
  }
  InsertAt=DefinLen;
  Insert('\0');
  return  Text;
}


/*  char  *FuncAccept(char *)
 *
 *  Function accepting parameters:- read multiple parameters, if enclosed
 *  with parentheses, else read one parameter; use Define() to read each
 *  parameter.
 *  FuncAccept() returns a pointer to text to after the parameters.
 */

char  *FuncAccept(char *Text)
{
  char  Paren;
  char  *SaveBase,  *Base;
  int    SaveOfst,   Ofst;

  if (*Text==' ')
    ++Text;
  if (*Text=='(')
    Paren=TRUE,  ++Text;
  else
    Paren=FALSE;
  InsertAt=DefinLen;
  Insert('(');
  while (TRUE)
  {
    SaveBase=DefinBase,  SaveOfst=DefinLen;
    Text=Define(Text);
    Base=DefinBase;
    DefinBase=SaveBase,  DefinLen=SaveOfst;
    InsertAt=DefinLen;
    for (Ofst=0; Base[Ofst]; ++Ofst)
      Insert(Base[Ofst]);
    free(Base);
    if (!Paren)
      break;
    if (*Text==' ')
      ++Text;
    if (*Text==',')
    {
      ++Text;
      Insert(',');
      Insert(' ');
      continue;
    }
    if (*Text==')')
      ++Text;
    else
      printf("Error: missed ')' on function definition.\n\n");
    break;
  }
  Insert(')');
  return  Text;
}


/*  void  Insert(char)
 *
 *  Insert one character into C definition at DefinBase+InsertAt; InsertAt
 *  increments by one.
 */

void  Insert(char Char)
{
  char  *Ptr;

  if ( Ptr=realloc(DefinBase,DefinLen+1) )
  {
    DefinBase=Ptr;
    memmove(DefinBase+InsertAt+1,DefinBase+InsertAt,DefinLen++ -InsertAt);
    DefinBase[InsertAt++]=Char;
  }
}


/*  char  *GetToken(char *)
 *
 *  Read and return English token from description: OTHER is returned if
 *  token not recognized. The token is held in static variable Token;
 *  GetToken() returns a pointer to Text to after the token.
 */

char  *GetToken(char *Text)
{
  char  *Ptr;

  if ( (Ptr=Compare(Text,"pointer",4)) || (Ptr=Compare(Text,"ptr",3)) )
  {
    Text=Ptr;
    Token=POINTER;
    if (Ptr=Compare(Text,"to",2))
      return  Ptr;
    return  Text;
  }
  if (Ptr=Compare(Text,"function",4))
  {
    Text=Ptr;
    if (Ptr=Compare(Text,"accepting",6))
    {
      Token=FUNCACPT;
      return  Ptr;
    }
    Token=FUNCTION;
    if (Ptr=Compare(Text,"returning",6))
      return  Ptr;
    return  Text;
  }
  if (Ptr=Compare(Text,"array",5))
  {
    Text=Ptr;
    if (Ptr=Compare(Text,"size",4))
    {
      Token=ARRAYSZ;
      return  Ptr;
    }
    Token=ARRAY;
    if (Ptr=Compare(Text,"of",2))
      return  Ptr;
    return  Text;
  }
  Token=OTHER;
  return  Text;
}


/*  char  *Compare(char *, char *, int)
 *
 *  Compare description (First) with token name (Second). Token in First must
 *  contain at least Minimum characters from Second (case insensitive).
 *  Compare() returns pointer to First to after token, or null if no match
 *  found.
 */

char  *Compare(char *First, char *Second, int Minimum)
{
  int  Count = 0;

  if (*First==' ')
    ++First;
  while (tolower(*First)==*Second)
  {
    ++First;
    ++Second;
    ++Count;
  }
  if (Count<Minimum || (*First!=' ' && *First))
    return  NULL;
  return  First;
}


