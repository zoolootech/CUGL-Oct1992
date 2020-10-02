/*  PLOX keyword handling modules  04-21-90       FILE: CONLIB.C
         Robert L. Patton, Jr.
         1713 Parkcrest Terrace
         Arlington, TX 76012
*/
#define HOLD 10
#define FNLEN 15

#include <stdio.h>
#include <string.h>
#include "PLOX.H"
#include "CONLIB.H"
#include "DATLIB.H"
#include "DRAWLIB.H"
#include "PXLIB.H"
#include "WORDLIB.H"
                           /* Global scaling parameters */
static SCALING Xfit = {0.0,20.0,4.32},
               Yfit = {0.0,25.0,2.88};
static int     Xlen = 6*INCH-1, Ylen = 4*INCH-1;

/*   ======= */
void Unknown (Word)
/*   ======= */
char *Word;
{
char Phrase[31] = "  unknown keyword: ";
  strncat (Phrase,Word,12);
  PauseMsg (Phrase);
}
/*   ===== */
void ARcon (Sentence)
/*   =====           Handles an AREA specification */
char *Sentence;
{
  #define INSET INCH/2
  char       Word[HOLD+1];
  double     Xarea, Yarea, Xsize, Ysize;
  static int Xa, Ya;
  int        Border;

  #ifdef DEBUG
  printf("ARcon:%s\n",Sentence);
  #endif

  GetWord (Sentence, Word, HOLD);
  Xa = INSET; Xlen = XMAX - 2 * INSET;
  Ya = INSET; Ylen = YMAX - 2 * INSET;
  Border = NO;
  while (Word[0] != ' ') {

    if (EQUAL (Word,"at")) {
      GetVal (Sentence, &Xarea);
      GetVal (Sentence, &Yarea);
      if (Xarea<=0 || Yarea<=0)
        PauseMsg("  invalid AREA location, default used ");
      else {
        Xa = PIXELS (Xarea);
        Ya = PIXELS (Yarea);
      }
    }
    else if (EQUAL (Word,"size")) {
      GetVal (Sentence, &Xsize);
      GetVal (Sentence, &Ysize);
      if (Xsize==0 || Ysize==0)
        PauseMsg("  invalid AREA size, default used");
      else {
        Xlen = PIXELS (Xsize);
        Ylen = PIXELS (Ysize);
      }
    }
    else if (EQUAL (Word,"outline")) Border = YES;

    else Unknown (Word);

    GetWord (Sentence, Word, HOLD);
  }
  PX_Origin (Xa, Ya);
  if (Border) Box (0, 0, Xlen, Ylen);
}
/*@@                                     CONLIB-2 */
/*   ===== */
void AXcon (Sentence)
/*   =====           Handles an AXIS specification */
char *Sentence;
{
  #define EXACT 1
  char       Dir, Locus, Word[HOLD+1];
  double     From, To;
  int        Axlen, Boxes, Side, Visible, Xa, Ya;
  SCALING    AxFit;

  #ifdef DEBUG
  printf("AXcon:%s\n",Sentence);
  #endif

  GetWord (Sentence, Word, HOLD);

                   /* Set all defaults */

  From = 0.0;  To = 0.0;  Boxes = 0;
  Locus = 'L'; Dir = 'V', Side = -1;
  Visible = YES;
                          /* Interpret the specification */
  while (Word[0] != ' ') {

      if      (EQUAL (Word,"invisible")) Visible = NO;
      else if (EQUAL (Word,"above")) {Locus='A' ;Dir='H'; Side=-1;}
      else if (EQUAL (Word,"below")) {Locus='B' ;Dir='H'; Side= 1;}
      else if (EQUAL (Word,"right")) {Locus='R' ;Dir='V'; Side= 1;}
      else if (EQUAL (Word,"left"))  {Locus='L' ;Dir='V'; Side=-1;}
      else if (EQUAL (Word,"boxes")) GetInt (Sentence, &Boxes);
      else if (EQUAL (Word,"from")) GetVal (Sentence, &From);
      else if (EQUAL (Word,"to"))  GetVal (Sentence, &To);
      else Unknown (Word);

      GetWord (Sentence, Word, HOLD);
  }
  Axlen = (Dir=='V'? Ylen: Xlen);
  Xa = (Locus=='R'? Xlen: 0);
  Ya = (Locus=='A'? Ylen: 0);
  if (From==0.0 && To==0.0) {
    PauseMsg("  axis range not given, using 0 to 100");
    To = 100.0;
  }
  Scale (From, To, Axlen, EXACT, &Boxes, &AxFit);
  if (Dir == 'H') CopyScale (&AxFit, &Xfit);
  else            CopyScale (&AxFit, &Yfit);
  if (Visible) AxNum (Xa, Ya, Dir, Side, Axlen, &AxFit, Boxes);
}
/*@@                                       CONLIB-3 */
void BRcon (Sentence)
/*   =====           Handles a BAR specification */
#define BRBUF  12
char *Sentence;
{
  double  Value;           /* local storage */
  double  Width;           /* user bar spec */
  float   Base;
  float   *Xptr, *Yptr;    /* pointers to plot data */
  char    Word[BRBUF+1];   /* local string storage */
  char    Fname[FNLEN+1];  /* file name for data */
  char    Dtype[MAXITEMS]; /* data type for any item */
  char    Dir;             /* bar direction (H,V) */
  int     Ix, Iy;          /* item no. in data line */
  int     Foot, Thick;     /* pixel bar specs */
  int     Rank, Of, Offset;/* abutted bar specs */
  int     Xfoot, Yfoot;    /* local storage */
  int     Xbar, Ybar;
  int     BaseSet;         /* flag for user set base */
  int     Points;          /* no. of data points */
  int     Hatch, Gap;      /* bar hatch pattern code */
  int     k;               /* local storage */

  #ifdef DEBUG
  printf("BRcon:%s\n",Sentence);
  #endif

  GetWord (Sentence, Word, BRBUF);

                 /* set defaults */
  Fname[0] = ' ';
  for (k=0; k<MAXITEMS; k++) Dtype[k] = 'N';
  Ix = 1;         Iy = 2;
  Width = .125;   Dir = 'V';  BaseSet = NO;
  Rank = 1;       Of = 1;
  Hatch = 0;      Gap = 4;
                        /* Interpret specification */
  while (Word[0]!=' ') {
      if      (EQUAL (Word,"file"))  GetWord (Sentence, Fname, FNLEN);
      else if (EQUAL (Word,"items")) SetItems (Sentence, &Ix, &Iy);
      else if (EQUAL (Word,"item"))  SetDtype (Sentence, Dtype);
      else if (EQUAL (Word,"base")) {GetVal (Sentence, &Value);
                                     Base = (float) Value;
                                     BaseSet = YES; }
      else if (EQUAL (Word,"width")) GetVal (Sentence, &Width);
      else if (EQUAL (Word,"hor") ||
               EQUAL (Word,"H")          ) Dir = 'H';
      else if (EQUAL (Word,"ver") ||
               EQUAL (Word,"V")          ) Dir = 'V';
      else if (EQUAL (Word,"hatch")) {
                GetWord (Sentence, Word, BRBUF);
                if      (EQUAL (Word,"H") ||
                         EQUAL (Word,"hor"))    Hatch = 1;
                else if (EQUAL (Word,"V") ||
                         EQUAL (Word,"ver"))    Hatch = 2;
                else if (EQUAL (Word,"right"))  Hatch = 3;
                else if (EQUAL (Word,"left"))   Hatch = 4;
                else if (EQUAL (Word,"square")) Hatch = 5;
                else if (EQUAL (Word,"blank"))  Hatch = 6;
                else    Unknown (Word);
      }
      else if (EQUAL (Word,"gap")) {
                GetInt (Sentence, &Gap);
                if (Gap <= 0) {
                  PauseMsg ("  positive number must follow 'gap'");
                  Gap = 4;
                }
      }
/*@@                                    CONLIB-4 */
      else if (EQUAL (Word,"abut")) {
                GetInt (Sentence, &Rank);
                GetInt (Sentence, &Of);
                if (Rank > Of || Rank <= 0) {
                  PauseMsg ("  error in abut specification");
                  Rank = 1; Of = 1;
                }
      }
      else    Unknown (Word);
      GetWord (Sentence, Word, BRBUF);
  }
                           /* Draw bars */
  if (Fname[0]==' ')
    PauseMsg("  no data file given - no bars drawn");
  else {
    GetTwo (Fname, &Xptr, Dtype[Ix-1], Ix, &Yptr, Dtype[Iy-1], Iy);
    Points = (int) *Xptr;
    if (Points == 0) {printf("  no data found for: "); PauseMsg(Fname);}
    else {
      PX_Style (Hatch, Gap);
      Thick = PIXELS (Width);
      Offset = (Thick * ((Rank<<1) - Of - 1)) / 2;
      if (BaseSet) Foot = Scaled (&Base, Dir=='V'? &Yfit: &Xfit);
      else         Foot = 0;
      printf("  drawing %d bars\n",Points);
      for (k=1; k<=Points; k++) {
        Xbar = Scaled (++Xptr, &Xfit);
        Ybar = Scaled (++Yptr, &Yfit);
        if (Dir == 'V') {Xbar += Offset; Xfoot = Xbar; Yfoot = Foot;}
        else            {Ybar += Offset; Xfoot = Foot; Yfoot = Ybar;}
        Bar (Xfoot, Yfoot, Xbar, Ybar, Dir, Thick, Hatch, Gap);
      }
    }
  }
}
/*@@                                      CONLIB-5 */
void IScon (Sentence)
/*   =====           Handles an ISO (gram) specification */
#define ISBUF  12
char *Sentence;
{

  float   Xdata[3];        /* isogram line x points */
  float   Ydata[3];        /* isogram line y points */
  char    Vword[ISBUF+1];  /* local string storage */
  char    Word[ISBUF+1];   /* local string storage */
  char    Dir;             /* direction code (X/Y) */
  char    Form;            /* location format code */
  int     Trace;           /* line type code */

  #ifdef DEBUG
  printf("IScon:%s\n",Sentence);
  #endif
  GetWord (Sentence, Word, ISBUF);

                 /* set defaults */
  Dir  = 'H';
  Form = 'N';
  strcpy(Vword,"0");
  Xdata[0] = Ydata[0] = 2.0;
  Xdata[1] = Xdata[2] = Ydata[1] = Ydata[2] = 0.0;
  Trace = SOLID;
                           /* Interpret specification */
  while (Word[0] != ' ') {

      if      (EQUAL (Word,"at")) GetWord (Sentence,Vword,ISBUF);
      else if (EQUAL(Word,"H") || EQUAL(Word,"hor"))   Dir = 'X';
      else if (EQUAL(Word,"V") || EQUAL(Word,"ver"))   Dir = 'Y';

      else if (EQUAL(Word,"form")) {
        Sentence[1] = '1';
        SetDtype (Sentence,&Form);
      }
      else if (LineType (Word,&Trace));
      else Unknown (Word);

      GetWord (Sentence, Word, ISBUF);
  }
  Evaluate (Vword,Form,&Xdata[1]);
  if (Dir=='Y') {
    Xdata[2] = Xdata[1];
    Ydata[1] = Yfit.Start;
    Ydata[2] = Ydata[1] + (double) Ylen / Yfit.DeltaP;
  }
  else {
    Ydata[2] = Ydata[1] = Xdata[1];
    Xdata[1] = Xfit.Start;
    Xdata[2] = Xdata[1] + (double) Xlen / Xfit.DeltaP;
  }
  #ifdef ISDEBUG
  ShowScale ('X',1,&Xfit);
  ShowScale ('Y',1,&Yfit);
  printf("Xlen = %d, Ylen = %d\n",Xlen,Ylen);
  printf("Line at %s %c\n",Vword,Form);
  printf("Trace = %o, Dir = %c\n",Trace,Dir);
  printf("Xdata = %.2f, %.2f, %.2f\n",Xdata[0],Xdata[1],Xdata[2]);
  printf("Ydata = %.2f, %.2f, %.2f\n",Ydata[0],Ydata[1],Ydata[2]);
  #endif
  Liner (Xdata,&Xfit,Ydata,&Yfit,Trace,0);
}
/*@@                                       CONLIB-6 */
void LAcon (Sentence)
/*   =====           Handles a LABEL specification */
#define LABUF  12
char *Sentence;
{
  float   Xdata, Ydata;    /* label-locus, data units */
  char    Dir, Justify;    /* text placement codes */
  char    Word[LABUF+1];   /* local string storage */
  char    Xword[LABUF+1];  /* local string storage */
  char    Yword[LABUF+1];  /* local string storage */
  char   *Text;            /* text manip. ptr  */
  char    Xform, Yform;    /* label-locus format codes */
  int     Font, Xt, Yt;    /* text size & location */
  int     TexLen;

  #ifdef DEBUG
  printf("LAcon:%s\n",Sentence);
  #endif

  GetWord (Sentence, Word, LABUF);

                 /* set defaults */
  Font = 1;
  Justify = 'C'; Dir   = 'H';
  Xform   = 'N'; Yform = 'N';
                           /* Interpret specification */
  while (Word[0] != ' ') {

      if (EQUAL (Word,"at")) {
        GetWord (Sentence,Xword,LABUF);
        GetWord (Sentence,Yword,LABUF);
      }
      else if (EQUAL(Word,"L") || EQUAL(Word,"left"))   Justify = 'L';
      else if (EQUAL(Word,"R") || EQUAL(Word,"right"))  Justify = 'R';
      else if (EQUAL(Word,"C") || EQUAL(Word,"center")) Justify = 'C';
      else if (EQUAL(Word,"V") || EQUAL(Word,"ver"))    Dir = 'V';
      else if (EQUAL(Word,"H") || EQUAL(Word,"hor"))    Dir = 'H';
      else if (EQUAL(Word,"font")) GetInt (Sentence,&Font);

      else if (EQUAL(Word,"xform")) {
        Sentence[1] = '1';
        SetDtype (Sentence,&Xform);
      }
      else if (EQUAL(Word,"yform")) {
        Sentence[1] = '1';
        SetDtype (Sentence,&Yform);
      }
      else if (EQUAL (Word,"text")) {
        Text = Sentence;
        while (*Text == ' ') Text++;
        Evaluate (Xword,Xform,&Xdata);
        Evaluate (Yword,Yform,&Ydata);
        Xt = Scaled (&Xdata,&Xfit);
        Yt = Scaled (&Ydata,&Yfit);
        if (Dir == 'H') {
          TexLen = TxWide (strlen (Text), Font);
          if      (Justify == 'C') Xt -= (TexLen>>1);
          else if (Justify == 'R') Xt -= TexLen;
        }
        PX_Text (Xt, Yt, Font, Dir, Text);
        break;
      }
      else Unknown (Word);

      GetWord (Sentence, Word, LABUF);
  }
}
/*@@                                  CONLIB-7 */
/*   ===== */
void LIcon (Sentence)
/*   =====            Handles a LINE specification */
#define LIBUF 20
char * Sentence;
{
  float    *Xptr, *Yptr;    /* pointers to plot data */
  char     Word[LIBUF+1];   /* local string storage */
  char     Fname[FNLEN+1];  /* file name for data */
  char     Dtype[MAXITEMS]; /* data type for any item */
  int      Ix, Iy;          /* item no. in data line */
  int      k, Points;       /* local storage */
  int      Node, Trace;     /* point and line type */

  #ifdef DEBUG
  printf("LIcon:%s\n",Sentence);
  #endif

  GetWord (Sentence, Word, LIBUF);

                       /* set defaults */
  Fname[0] = ' ';
  for (k=0; k<MAXITEMS; k++) Dtype[k] = 'N';
  Ix = 1; Trace = SOLID;
  Iy = 2;  Node = 0;
                         /* interpret specification */
  while (Word[0]!=' ') {
      if      (LineType (Word,&Trace));
      else if (EQUAL (Word,"file"))  GetWord (Sentence, Fname, FNLEN);
      else if (EQUAL (Word,"items")) SetItems (Sentence, &Ix, &Iy);
      else if (EQUAL (Word,"item"))  SetDtype (Sentence, Dtype);
      else if (EQUAL (Word,"points")) {
          GetWord (Sentence, Word, LIBUF);
          if      (EQUAL (Word,"square"))   Node = 0071;
          else if (EQUAL (Word,"circle"))   Node = 0061;
          else if (EQUAL (Word,"diamond"))  Node = 0042;
          else if (EQUAL (Word,"plus"))     Node = 0144;
          else if (EQUAL (Word,"pinwheel")) Node = 0113;
          else if (EQUAL (Word,"x"))        Node = 0112;
          else if (EQUAL (Word,"flower"))   Node = 0133;
          else if (EQUAL (Word,"blob"))     Node = 0077;
          else if (EQUAL (Word,"blot"))     Node = 0106;
          else    Unknown (Word);
      }
      else    Unknown (Word);
      GetWord (Sentence, Word, LIBUF);
    }
    if (Fname[0]==' ')
      PauseMsg("  no data file given - no line drawn");
    else {
      GetTwo (Fname, &Xptr, Dtype[Ix-1], Ix, &Yptr, Dtype[Iy-1], Iy);
      Points = (int) *Xptr;
      if (Points) {
        printf("  tracing %d points\n",Points);
        Liner (Xptr, &Xfit, Yptr, &Yfit, Trace, Node);
      }
      else
        {printf("  no data found for: "); PauseMsg(Fname);}
  }
}
/*@@                                      CONLIB-8 */
int ItemOut (Item)
/*  =======       check for data item out of range */
int Item;
{
  if (Item < 1 || Item > MAXITEMS) {
    printf("  item number %d",Item); PauseMsg(" out of range");
    return YES;
  }
  else return NO;
}
/*  ======== */
int LineType (Word,Trace)
/*  ========          Sets the Trace value if Word is a line type */
char *Word;
int  *Trace;
{
  int Result;

  Result = YES;
  if      (EQUAL (Word,"invisible")) *Trace = 000;
  else if (EQUAL (Word,"solid"))     *Trace = SOLID;
  else if (EQUAL (Word,"dotted"))    *Trace = 011;
  else if (EQUAL (Word,"spotted"))   *Trace = 025;
  else if (EQUAL (Word,"dash"))      *Trace = 007;
  else if (EQUAL (Word,"longdash"))  *Trace = 067;
  else if (EQUAL (Word,"dashdot"))   *Trace = 027;
  else if (EQUAL (Word,"sparse"))    *Trace = 001;
  else Result = NO;
  return Result;
}
/*   ======== */
void SetDtype (Sentence, Dtype)
/*   ========   handles an "item is" spec */
char *Sentence, *Dtype;
{
  #define    DTYPLEN 8
  char  Word[DTYPLEN+1];
  int   Item;

  GetInt (Sentence, &Item);
  GetWord (Sentence, Word, DTYPLEN);
  if (!ItemOut (Item)) {
    if      (EQUAL (Word, "yymmdd"))  Dtype[Item-1] = 'D';
    else   {printf("  unknown data type: "); PauseMsg(Word);}
  }
}
/*   ======== */
void SetItems (Sentence, Ix, Iy)
/*   ========    handles an "items" spec */
char *Sentence;
int  *Ix, *Iy;
{
  GetInt (Sentence, Ix);
  if (ItemOut (*Ix)) *Ix = 1;
  GetInt (Sentence, Iy);
  if (ItemOut (*Iy)) *Iy = 2;
}
/*@@                                 CONLIB-9 */
/*   ===== */
void PCcon (Sentence)
/*   =====           Handles a PIC specification */
char *Sentence;
{
  char       Word[HOLD+1];
  double     Xpic, Ypic;
  static int Xmax, Ymax;
  int        Border, Margin = 5;

  #ifdef DEBUG
  printf("PCcon:%s\n",Sentence);
  #endif

  GetWord (Sentence, Word, HOLD);
  if (EQUAL (Word,"CLOSE"))
    PX_Close (Xmax, Ymax);
  else {
    PX_Open ();
    Xmax = XMAX;
    Ymax = YMAX;
    Border = NO;
    while (Word[0] != ' ') {

      if (EQUAL (Word,"size")) {
        GetVal (Sentence, &Xpic);
        GetVal (Sentence, &Ypic);
        if (Xpic==0 || PIXELS(Xpic) > XMAX ||
            Ypic==0 || PIXELS(Ypic) > YMAX   )
          PauseMsg("  invalid PIC size, max used");
        else {
          Xmax = PIXELS (Xpic);
          Ymax = PIXELS (Ypic);
        }
      }
      else if (EQUAL (Word,"outline")) Border = YES;

      else if (EQUAL (Word,"margin"))  GetInt (Sentence, &Margin);

      else Unknown (Word);

      GetWord (Sentence, Word, HOLD);
    }
    PX_Margin (Margin);
    if (Border) Box (0, 0, Xmax, Ymax);
  }
}
/*@@                                  CONLIB-10 */
/*   ===== */
void TLcon (Sentence)
/*   =====           Handles a TITLE specification */
char *Sentence;
{
  static char Dir, Justify;
  static int  Font, Xt, Yt;
  double      Xloc, Yloc;
  char       *Text, Word[HOLD+1];
  int         TexLen;

  #ifdef DEBUG
  printf("TLcon:%s\n",Sentence);
  #endif

  GetWord (Sentence, Word, HOLD);
  if (!EQUAL (Word,"text")) {
    Justify = 'C';
    Dir = 'H';
    Font = 1;
  }
  while (Word[0] != ' ') {

    if (EQUAL (Word,"at")) {
      GetVal (Sentence,&Xloc);
      GetVal (Sentence,&Yloc);
      Xt = PIXELS (Xloc);
      Yt = PIXELS (Yloc);
      if (Xloc==0 || Yloc==0)
        PauseMsg("  invalid title location");
    }
    else if (EQUAL(Word,"L") || EQUAL(Word,"left"))   Justify = 'L';
    else if (EQUAL(Word,"R") || EQUAL(Word,"right"))  Justify = 'R';
    else if (EQUAL(Word,"C") || EQUAL(Word,"center")) Justify = 'C';
    else if (EQUAL(Word,"V") || EQUAL(Word,"ver"))    Dir = 'V';
    else if (EQUAL(Word,"H") || EQUAL(Word,"hor"))    Dir = 'H';
    else if (EQUAL(Word,"font")) GetInt (Sentence,&Font);

    else if (EQUAL (Word,"text")) {
      Text = Sentence;
      while (*Text == ' ') Text++;
      if (Dir == 'H') {
        TexLen = TxWide (strlen (Text), Font);
        if      (Justify == 'C') Xt -= (TexLen>>1);
        else if (Justify == 'R') Xt -= TexLen;
      }
      PX_Text (Xt, Yt, Font, Dir, Text);
      break;
    }
    else Unknown (Word);

    GetWord (Sentence, Word, HOLD);
  }
}
void HUcon (char *Sentence) {
/*   =====                    Handles a HUE specification */
char Word[HOLD+1];
int  Color;
  GetWord (Sentence, Word, HOLD);
  if ((Color=IsHue(Word)) > 0) PX_Hue (Color);
  else                         PauseMsg ("  unknown hue");
}
