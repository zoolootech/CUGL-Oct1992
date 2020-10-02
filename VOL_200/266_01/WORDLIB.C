/*    Keyword-value Text Handling Routines       File: WORDLIB.C
        Robert L. Patton, Jr.
        1713 Parkcrest Terrace
        Arlington, TX 76012
           21 April 1990
*/
#define EQUAL(S,T) (!strcmp(S,T))
#define YES 1
#define NO  0

void Blank (Word, Size)
/*   =====
            Blank out a word */
char *Word;
int  Size;
{
  int I;
  Word[0]='\0';
  for (I=0; I<Size; I++)
    strcat (Word," ");
}

void Capitalize (Word)
/*   ==========       Make word all upper case */
char *Word;
{
  char *s;
  for (s=Word; *s; s++) *s = toupper(*s);
}

void Clean (Sentence)
/*   =====           replace commas and equals with blanks */
char *Sentence;
{
  while (*Sentence++ !='\0')
    if (*Sentence==',' || *Sentence=='=') *Sentence=' ';
}

void Ftoa (Value, Digits)
/*   ====
          Converts a double to a string in a very limited way.
         (Developed for axis notations.)  It expects the fractional
          part to be not over 3 digits and it removes any trailing
          zeros and a trailing decimal point.  The user must supply
          a Digits string long enough to hold the value written to
          3 decimal places.
*/
double Value;
char *Digits;
{
  int N;
  sprintf (Digits,"%.3f",Value);
  N = strlen (Digits);
  while (Digits[--N] == '0') {
    Digits[N] = '\0';
  }
  if (Digits[N] == '.') Digits[N] = '\0';
}

int Filler (Word)
/*  ======       detect a nonsignificant word */
char *Word;
{
  if (EQUAL (Word,"by") ||
      EQUAL (Word,"is") ||
      EQUAL (Word,"of") ||
      EQUAL (Word,"are")  )
    return YES;
  else
    return NO;
}
/*@@*/
void GetInt (Sentence, Number)
/*   ======    Retrieve the next word of a phrase as an integer */
char *Sentence;
int  *Number;
{
  #define IMAX 6
  char Word[IMAX+1];
  int  N, N1;

  *Number = 0;
  GetWord (Sentence,Word,IMAX);
  N1 = 0;
  if (Word[0]=='+' || Word[0]=='-') N1 = 1;
  for (N=N1; N<strlen(Word); N++)
    if (! isdigit(Word[N])) return;
  *Number = atoi(Word);
}

void GetVal (Sentence, Value)
/*   ======    Retrieve the next word of a phrase as a double */
char *Sentence;
double *Value;
{
  #define VMAX 15
  char Word[VMAX+1], *W;
  int  D, E, N, P, S;
  double atof();

  *Value = 0;
  strcpy (Word,"              ");
  GetWord (Sentence,Word,VMAX);
  D = 0; E = 0; P = 0, S = 0;
  for (W=Word; *W; W++)
    if (*W == '.') P++;
    else if (isdigit(*W)) D++;
    else if (*W == 'e' ||
             *W == 'E'   ) E++;
    else if (*W == '-' ||
             *W == '+'   ) S++;
  if (D>0 && E<=1 && P<=1 && S<=2) *Value = atof(Word);
}

void GetWord (Sentence, Word, Size)
/*   =======     Strips a word from the front of a string */
char *Sentence,*Word;
int  Size;
{
  int I,N;
  Blank (Word,Size);
  for (N=0;N<strlen(Sentence);N++)
    if (Sentence[N]!=' ')
    { for (I=0; I<Size; I++)
      { Word[I]=Sentence[N+I];
        if (Word[I]==' '||Word[I]=='\n') Word[I]='\0';
        if (Word[I]=='\0') break;
        Sentence[N+I]=' ';
      }
      if (Filler (Word)) GetWord (Sentence, Word, Size);
      return;
    }
}
int iKeyFind (psList, iListLen, sWord)
/*  =========
      Finds if a given word matches any entry or unique abbreviation
      (consisting of the first n letters) thereof in a given word list.
      The word list may be in any order for user convenience.

      returns: the position (1 to n) of the word in the list
              (not to be confused with the subscript (0 to n-1))
              or  0 if not found
              or -1 if abbreviation is not unique
      ps. This was an experiment with the Hungarian naming convention.
*/
char **psList; /* the word list, array of strings */
int  iListLen; /* the number of items in the list */
char *sWord;   /* the word to try and match */
{
int iMatch, iTest, n;

    iTest = strlen(sWord);
    iMatch = 0;
    for (n=0; n<iListLen; n++) {                 /* search list */
        if (!strncmp(psList[n],sWord,iTest)) {   /* partial match ? */
            if (strlen(psList[n])==iTest) {      /* if exact,     */
                iMatch = n+1;                    /*    it's found */
                break;
            }
            if (!iMatch)                   /* if first time */
                iMatch = n+1;              /* mark the spot */
            else {
                iMatch = (-1);             /* if not first time */
                break;                     /* it's not unique   */
            }
        }
    }
    return iMatch;
}

void PauseMsg (Text)
/*   ======        print message and pause */
char *Text;
{
  printf("%s",Text);
  printf("\n  ...\n");
  getch();
}
