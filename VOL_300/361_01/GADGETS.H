

/*
---> Gadgets.h  A "gadget" is an ingenious contivance (per Webster's).

   Every Programmer evolves a "Bag of Tricks" over time.  This is mine.  Some 
items are lifted from Publications, others are modifications of existing 
Public Domain or Shareware code, and some are original developments.

   Two library files (Gadgets.lib & Gadgetl.lib) are compiled on Turbo-C v2.0 
for the Small and Large models, respectively.  The Manual, Gadgets.Txt, is in 
TIS format (understood by VU).  It contains all source code (less headers) 
lest I forget how these critters really work.

============================================================================

  Copyrighted to the Public Domain.  Unlimited Distribution Authorized.

  User assumes All Risks/Responcibilities.

============================================================================
*/

#ifndef GADGETS
#define GADGETS

/* Declare Macros */
#define Clr(x)			(ClrBox(1, 1, 80, 25, x))
#define ClrBox(a,b,c,d,x)	(Scroll(a, b, c, d, x, 0, 0))
#define Cursor_OFF              (HideCursor())
#define Cursor_ON               (ShowCursor(0))
#define Cursor_FAT		(ShowCursor(1))
#define Cursor_LIT              (HideCursor())
#define DreadChr(x,y)		(Vpeek(Vadr(X, Y)) & 0xFF)
#define GetAttr(x,y)		(Vpeek(Vadr(X, Y)) >> 8)
#define InsChr(x,y)		(PadLeft(x, y, strlen(x) + 1))
#define IsTextMode()            ((peekb(0x40, 0x49) < 4) ? 1: 0)
#define OpenStr(x,y)		(PadLeft(x, SPACE, strlen(x) + y))
#define Trim(x)			(TrimStr(x, 0))
#define LTrim(x)		(TrimStr(x, 1))
#define RTrim(x)		(TrimStr(x, 2))
#define Vaddr(x,y)		((y-1) * 160 + (x-1) * 2)

/* Function prototypes for xStdEkx.lib */
int  AnyCharacter(void);
char *BaseName(char *FileName);
void Beep(unsigned int pitch, unsigned int Duration);
void Box(int Left, int Top, int Right, int Bottom, int Fill, int Color);
void CcolorSet(int X, int Y, int Color, int N);
void CgaSnowFence(void);
void ClrTo(int X, int Color);
void CursorBwd(int N);
void CursorDn(int N);
void CursorFwd(int N);
void CursorNL(int N);
void CursorUP(int N);
void Dread(int X, int Y, char *Text, int N);
void DputChr(char ch, int Color);
void Dwrite(int X, int Y, int Color, char *Text);
void DwriteEnd(int X, int Y, int Color, char *Text, int N);
void ExpandTabs(char *Text);
int  FileExists(char *FileName);
int  GetaKey(unsigned char *List, int CaseLess);
long GetFileSize(char *FileName);
unsigned int GetxKey(void);
int  GetLineN(char *Line, int Which1);
long GetLongDate(void);
void Gotoxy(int X, int Y);
int  Getxy(int *X, int *Y);
int  HideCursor(void);
void Kbq_flush(void);
int  Kbq_poll(void);
int  Kbq_snoop(int Which1);
int  Kbq_read(void);
int  Kbq_stuff(unsigned char ch);
int  Kbq_tally(void);
int  LPready(int lptnum);
int  LPreset(int lptnum);
int  LPrintChr(char ch, int lptnum);
int  LPrintStr(char *str, int lptnum);
void MkAskBox(char *FootPrint, int X, int Y, int Width, int Tall, char Shadow,
        char *TopTitle, char *FootTitle, char *Prompt, unsigned char Color);
void MkShadow(int Left, int Top, int Right, int Bottom, char Which1);
void NewExt(char *Old, char *New, char *Ext);
void PadEnds(char *Text, int ch, int NewLen);
void PadLeft(char *Text, int ch, int NewLen);
void PadRight(char *Text, int ch, int NewLen);
int  Query(char *Text, char *TopTitle, int CaseLess, unsigned char Color,
        char **Help2Pop);
void PopHelp(char **PopHelpPtr);
void RcolorSet(int X, int Y, int Color, int N);
void RepeatChr(char *Text, int ch, int N);
void Remove(char *Text, int ch);
void RestoreBox(int Left, int Top, int Right, int Bottom, char *FootPrint);
int  Sar(char *Text, char *This, char *That, int StopAfter);
void SaveBox(int Left, int Top, int Right, int Bottom, char *FootPrint);
void Scroll(int Left,int Top,int Right,int Bottom,int Color,int N,int Flag);
void SetAttr(int X, int Y, int Color);
void SetVideoMode(int Mode);
void ShowCursor(int Fat);
void Strrcpy(char *Target, char *Source);
int  Tally(char *Text, int ch);
int  TBarMu(char **TBar[], char *Title, long BarColor, long MuColors);
void TisHelp(void);
int  TMu(char **Mu, int X, int Y, long Colors, int Border);
char *TrimStr(char *Str, int WhichSide);
int  Vpeek(unsigned adr);
void Vpoke(unsigned adr, unsigned chr);
void ZapAskBox(char *FootPrint,int X,int Y,int Width,int Tall,char Shadow);

#endif

