

/* VU --> A File Viewer ala MORE w/ Search & Multi-File/Pipe Lists
 *
 * J.Ekwall 13 March, 1990
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 */

#include <ctype.h>
#include <dos.h>
#include <gadgets.h>
#include <io.h>
#include <keys.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdek.h>
#include <string.h>

char *Documentation[] = {
    "",
    "Usage:",
    "       VU [options][drive:][path]filename [, ...] --> View Text Files.",
    "     | VU [options] ---> View a Text Stream ala MORE.",
    "",
    "Options:",
    "       /A -------------> Query for Initial Search.",
    "       /B -------------> Build/ReBuild Select List.",
    "       /C0xhhhhhh -----> Specify Colors.   [D:/C0x1F301E]",
    "       /F \"FindMe\" ----> Jump to Text Match.",
    "       /K -------------> Pop w/ Select List.",
    "       /R -------------> Respect RamPage UnderLines.",
    "       /S -------------> CGA Snow Protection.",
    "",
    "  Named Pipes are Legal.  Pipes are NOT Errased by Viewing.",
    "",
    "Last Updated: 18 September 91/EK",
    "",
    NULL};

char *Pop_Help[] = {
    "  <ESC> ---------> Exit & View Next File <If Any>.",
    " Alt-X ----------> Exit. (Don't View Next File).",
    "  <Enter> -------> Pop Item Select List.  <If Any>",
    " ^<Enter> -------> Build/ReBuild a Select List.",
    "   F2 -----------> Find a Text Phrase.",
    "Shift-F2 --------> Find a Text Phrase.  Ignore Case.",
    "   F3 -----------> \"Fuzzy Logic\" Word Finder.",
    "Shift-F3 --------> \"Fuzzy Logic\" Word Finder. Ignore Case.",
    "   F4 -----------> Repeat Search.",
    "   F5 -----------> Strip Hi-Bit Toggle.        [D:don't]",
    "   F6 -----------> Respect RamPage UnderLines. [D:don't]",
    "   F8 -----------> Set F3 \"Fuzz\" Level [D: 5]",
    "   F9 -----------> Select Another File to View",
    "   F10 ----------> Set/Report Color Settings.",
    "",
    "  <Home> / <Page Up>   / <Up Arrow> ----> Roll Upwards.",
    "  <End>  / <Page Down> / <Down Arrow> --> Roll Downward.",
    "  <Right/Left Arrow> --> Shift Text 10 Spaces.",
    "",
    "  Any Other Key -> Roll Down 11 Lines (Half a Screen).",
    NULL};


char *Pop_Help2[] = {
    "   \"Fuzzy Find\" use the Levenstein Distance Algorithm to",
    "compute a \"FuzzFactor\" (how well what you got matches",
    "what you wanted):",
    "",
    "        Wanted       Have     FuzzFactor",
    "        ÄÄÄÄÄÄ       ÄÄÄÄ     ÄÄÄÄÄÄÄÄÄÄ",
    "        FooBar  -->  FooBar ----> 0",
    "        FooBar  -->  fooBar ----> 1",
    "        FooBar  -->  FooxBar ---> 1",
    "        Foo     -->  FooBar ----> 3",
    "        Bar     -->  FooBar ----> 3",
    "        FooBar  -->  FooBat ----> 3",
    "        FooBar  -->  FooBa -----> 7",
    "        FooBar  -->  FoBar -----> 9",
    "        FooBar  -->  Foo -------> 20",
    "",
    "   The Value You Set Determines What will be a \"Match\".",
    NULL};

/* Declare Constants */
#define LINESIZE	513

/* Declare Globale */
BYTE BarColor = 0x30, NmlColor = 0x1F, HiLite = 0x1E;
int CrashStop = FALSE, WordStar = FALSE, InitAsk = FALSE, InitLst = FALSE;
int Offset = 0, ReBuild = FALSE, Fuzz = 5, RamPage = FALSE, Cleanup = FALSE;
BYTE Find[81], Line[LINESIZE], Name[81], Text[LINESIZE], LastFind[81];
FILE *fp = stdin;

/* Declare Local ProtoTypes */
int  AtoHexColors(char **ptr);
int  FindLine(int Start, int CaseLess);
int  FuzzyFind(char *Have, char *Want);
void GrabStdin(void);
int  ItemList(void);
void PaintCRT(int TopLine);
void Show(int CaseLess);
void Usage(void);
void UserSetsColors(void);

main (int argc, char *argv[])
{
    int i, xx, yy, Flag = FALSE;
    char *tp1, FootPrint[4000];

 /* Set Option Flags */
    *Find = *LastFind = NULL;
    while (*argv[1] IS SLASH) {
       for (tp1 = argv[1] + 1; *tp1 != NULL; ) {
	  switch (toupper(*tp1++)) {
	  case 'A': InitAsk++; break;
	  case 'B': ReBuild++; break;
          case 'C':             /* Colors */
	     if (!AtoHexColors(&tp1)) Usage(); break;
          case 'F':
             if (argc IS 2) Usage();
             for (i = 1, --argc; i < argc + 1; i++) argv[i] = argv[i+1];
             strcpy(Find,argv[1]); strupr(Find); Flag = TRUE;
             break;
	  case 'K': InitLst++; break;
	  case 'S': CgaSnowFence(); break;
          default:
             fprintf(stderr,"\nInvalid Option [/%c].\n\n",*tp1);
             Usage();
          }
       }

    /* SHIFT */
       for (i = 1, --argc; i < argc + 1; i++) argv[i] = argv[i+1];
    }

 /* Save FootPrint Area */
    Getxy(&xx, &yy); SaveBox(1, 1, 80, 25, FootPrint); HideCursor();

 /* Determine Ops Mode */
    if (argc IS 1)  {                    /* Barefoot Mode */
       if (INFLOW_EXISTS) GrabStdin();
       if (FileExists("\\STD OUT")) strcpy(Name,"\\STD OUT");
       else if (FileExists("\\STD IN"))  strcpy(Name,"\\STD IN");
       else Usage();
       Show(Flag); argc--;
    }

 /* Show the Specified Files */
    for (i = 1; i < argc; i++) {
       if (*argv[i] IS '$') {
          strcpy(Name,"\\STD "); strcat(Name,++argv[i]);
       } else strcpy(Name,argv[i]);
       if (FileExists(Name)) Show(Flag);
       Flag = FALSE; GetLineN(NULL, 0); if (CrashStop) break;
    }

 /* Restore FootPrint & Split */
    RestoreBox(1, 1, 80, 25, FootPrint); Gotoxy(xx, yy); ShowCursor(0);
    if (Cleanup) unlink("\\STD IN"); exit(0);
}

int AtoHexColors(char **ptr)
{
    int c, i = 0;
    unsigned int n = 0;

    if (strncmp(*ptr, "0x", 2) && strncmp(*ptr, "0X", 2)) return FALSE;
    for (*ptr += 2; isxdigit(c = toupper(**ptr)) && i++ < 6; *ptr += 1) {
	n = (isdigit(c)) ? 16 * n + c - 48 : 16 * n + c - 55;
	if (i IS 2) { NmlColor = n; n = 0; }
	if (i IS 4) { BarColor = n; n = 0; }
	if (i IS 6) HiLite = n;
    }
    return (i >= 6);
}

int FindLine(int Start, int CaseLess)
{
    int i, Dits = 18;
    char *tp1, DitLine[81];

    if (!*Find) return(Start); strcpy(DitLine, "---<Scanning>--- ");
    strcpy(LastFind, Find);
    for (i = Start; GetLineN(Line, (++i) + 1); ) {
       if (WordStar) for (tp1 = Line; *tp1 != NULL; ) *tp1++ &= 127;
       if (CaseLess % 2) strupr(Line);
       if (CaseLess < 2) { if(strstr(Line, Find) != NULL) return(i); }
       else for (tp1 = strtok(Line, " \t\n"); *tp1;
	  tp1 = strtok(NULL, " \t\n")) if (FuzzyFind(tp1, Find) < Fuzz)
             return(i);
       if (i % 23) continue;
       if (++Dits IS 78) {
          strcpy(DitLine,"."); Dits = 1;
       } else strcat(DitLine, ".");
       DwriteEnd(1, 25, BarColor, DitLine, 80);
       if (Kbq_poll() IS SPACE) break;
    }
    putchar(BEL); return(Start);
}

int FuzzyFind(char *Have, char *Want)
{
    int i, j, Delta, Prev, Distance[21];
    char Text[21], String[21], *tp1, *tp2;

    strncpy(Text, Want, 20); strncpy(String, Have, 20);
    Text[21] = String[21] = NULL; if (!*Want) return 0;
    if (!*Have) return 5 * strlen(Want);
    for (i = 0; i < 21; i++) Distance[i] = i;
    tp1 = Text;
    do {
       Prev = Distance[0] + 5; tp2 = String; i = 0;
       do {
          Delta = Prev + 1;
          if (*tp1 == *tp2) j = 0;
	  else if (toupper(*tp1) == toupper(*tp2)) j = 1;
          else j = 3;
	  if ((j += Distance[i]) < Delta) Delta = j; Distance[i] = Prev;
	  if ((j = Distance[++i] + 5) < Delta) Delta = j; Prev = Delta;
       } while (*tp2++);
       Distance[strlen(String)] = Delta;
    } while (*tp1++);
    if ((j = strlen(Text) - strlen(String)) > 0) Delta += 4 * j;
    return --Delta;
}

void GrabStdin(void)
{
    int c;

    if ((fp = fopen("\\STD IN", "w")) IS NULL) Usage();
    while((c = getchar()) != EOF) fputc(c, fp);
    fclose(fp); Cleanup++;
}

int ItemList(void)
{
 /* Find/Read .LST File, Present List(s) & Post User Selection */
    int i, Flag, TopItem, NxtLine, X, Y, xx, yy;
    char ListName[81], *tp1;

 /* Find .LST File */
    strupr(Name); if (strstr(Name, ".TXT") IS NULL) return FALSE;
    NewExt(Name, ListName, "LST");
    if (ReBuild) {
       if ((fp = fopen(ListName, "w")) IS NULL) return FALSE;
       Clr(0); printf("Rebuilding Select List.  Please Wait.. ");
       for (i = 1; GetLineN(Text, i++); ) {
          if (*Text != '>') continue;
	  if ((tp1 = strstr(Text, ".  ")) != NULL) tp1[1] = NULL;
          fprintf(fp, "%s\n", Text + 1);
       }
       fclose(fp); ReBuild = FALSE;
    }
    if (!FileExists(ListName)) return FALSE; GetLineN(ListName, 0);

 /* Construct Window */
    strcpy(Text, "[ <Enter> to Select Item or <ESC> to Cancel ]");
    PadEnds(Text, 'Ä', 80); Dwrite(1, 1, BarColor, Text);

 /* Do Business */
    TopItem = NxtLine = 0; ShowCursor(1); X = Y = 2;
RePaint:
    ClrBox(1, 2, 80, 24, 0x0E);
    for (NxtLine = TopItem, xx = 3, Flag = TRUE; xx < 75 && Flag; xx += 20) {
       for (yy = 2; Flag && yy < 25; yy++) {
	  Flag = GetLineN(Text, ++NxtLine); Text[17] = NULL;
	  Dwrite(xx, yy, 0x0A, Text);
       }
    }

 /* Update Footer */
    if (Flag) sprintf(Text,"ÄÄÄÄÄÄÄ[ -MORE-  %c  %c ]",ESC,SUB);
    else sprintf(Text,"ÄÄÄÄÄÄÄ[         %c  %c ]",ESC,SUB);
    PadRight(Text, 'Ä', 80); Dwrite(1, 25, BarColor, Text);

 /* Set Cursor & Get User Input */
    for (Flag = FALSE; !Flag; ) {
       Gotoxy(X, Y);
       switch (Kbq_read()) {
       case UP:   if (Y > 2) Y--; break;
       case DN:   if (Y < 24) Y++; break;
       case FWD:
	  if ((X += 20) < 75) break; else X -= 20; TopItem += 23; goto RePaint;
       case BWD:
	  if (X > 2) { X -= 20; break; }
          TopItem -= 23; if (TopItem < 0) TopItem = 0; goto RePaint;
       case HOME:  X = Y = 2; break;
       case END:   Y = 24; X = 62; break;
       case PGUP:
          X = Y = 2; TopItem -= 92; if (TopItem < 0) TopItem = 0;
          goto RePaint;
       case PGDN:
          X = Y = 2; TopItem += 92; goto RePaint;
       case ESC:  *Find = NULL; goto Tilt;
       case CR:   /* Select */
	  *Find = '>';
	  Flag = GetLineN(Find+1, TopItem + (X / 20) * 23 + Y - 1);
	  if (Flag) Dwrite(X-1, Y, 0x0E, "-");
       }
    }

 /* Post Selection (If Any), Delete Window & Split */
Tilt:
    GetLineN(Name, 0); HideCursor(); return Flag;
}

void PaintCRT(int TopLine)
{
    int c, i, Flag, Toggle, X;
    char *tp1;

 /* Do Header Bar */
    sprintf(Text, "[ VU File: %s ]", Name); PadEnds(Text, 'Í', 80);
    Dwrite(1, 1, BarColor, Text);

 /* Paint In the Text Window */
    for (i = 2; i < 25; i++) {
       Flag = !GetLineN(Line, TopLine+i-1);
       if (WordStar IS 1) for (tp1 = Line; *tp1 != NULL; ) *tp1++ &= 127;
       if (strchr(Line, TAB) != NULL) ExpandTabs(Line);
       if (RamPage && Tally(Line, UNDER)) {
          Gotoxy(1, i);
          for (X = -Offset, Toggle = 0, tp1 = Line; X < 80; ) {
             if (*tp1) c = *tp1++; else c = SPACE;
             if (c IS UNDER) { Toggle = !Toggle; continue; } 
             if (++X < 1) continue; if (c IS BAR) c = '³';
             if (!Toggle) DputChr(c, NmlColor);
             else { if (c IS SPACE) c = UNDER; DputChr(c, HiLite); }
          }
       } else {
          if (Offset >= strlen(Line)) *Text = NULL; 
          else strncpy(Text, Line+Offset, 80); Text[80] = NULL;
          if (strlen(Text) > 79 && Text[79]) Text[79] = BEL;
          else PadRight(Text, SPACE, 80);
          Dwrite(1, i, NmlColor, Text);
       }
    }

 /* Do Footer Bar */
    if (!Flag)
       sprintf(Text,"ÄÄÄ[ -More- ]ÄÄÄÄÄ[ Lines %d : %d ]", TopLine+1, 
          TopLine+23);
    else sprintf(Text,"ÄÄÄ[ *EOF* ]ÄÄÄÄÄ[ Lines %d : EOF ]", TopLine+1);
    PadRight(Text, 'Ä', 80); Dwrite(1, 25, BarColor, Text);
}

void Show(int FindFlag)
{
 /* Show an ASCII File */
    int TopLine = 0;
    char CurrentName[81];

 /* Initialize & Do Initial Search */
    Clr(NmlColor); GetLineN(Name, 0);
    if (FindFlag) TopLine = FindLine(0, FindFlag); else *Find = NULL;
    if (InitLst) {
       InitLst = FALSE; if (ItemList()) TopLine = FindLine(0, FALSE); }
    if (InitAsk) {
       InitAsk = FALSE;
       if (Query(Find, "[ Enter Text to Find.  F2 = Any Character. ]", 
          FindFlag, 0x4F, Pop_Help)) TopLine = FindLine(0, FALSE);
    }
    HideCursor(); PaintCRT(TopLine);

 /* Show Text ala more */
    for (EVER) {

    /* Query User */

       switch (Kbq_read()) {
       case END:   while(GetLineN(Line, 24 + TopLine++)); break;
       case PGDN:  TopLine += 22; break;
       case LF:    ReBuild++;
       case CR:    if (ItemList()) TopLine = FindLine(0, FALSE); break;
       case DN:    TopLine += 1; break;
       case HOME:  TopLine = 0; break;
       case PGUP:  TopLine -= 21;
       case UP:    TopLine -= 1; if (TopLine < 0) TopLine = 0; break;
       case ALT_X: CrashStop = TRUE;
       case ESC:   fclose(fp); return;      /* Clean Up & Exit */
       case BWD:   Offset += 20;
       case FWD:   Offset = (Offset > 10) ? Offset-10 : 0; break;
       case F1:    PopHelp(Pop_Help); continue;
       case F2:       FindFlag = 9;
       case SHIFT_F2: if (FindFlag < 5) FindFlag = 10;
       case F3:       if (FindFlag < 5) FindFlag = 20;
       case SHIFT_F3: if (FindFlag < 5) FindFlag = 30;
          if (FindFlag > 5) FindFlag /= 10;
	  if (FindFlag < 2 && !Query(LastFind, 
	     "[ Enter Text to Find.  F2 = Any Character. ]", FindFlag, 0x4F, 
	     Pop_Help)) continue;
	  if (FindFlag > 1 && !Query(LastFind, 
	     "[ Enter Word to Find.  F2 = Any Character. ]", FindFlag%2, 0x4F, 
	        Pop_Help)) continue;
       case F4:
          strcpy(Find, LastFind); TopLine = FindLine(TopLine, FindFlag);
          break;
       case F5:  WordStar = !WordStar;  break;
       case F6:  RamPage = !RamPage;  break;
       case F8:
          sprintf(Text, "%d", Fuzz);
          if (!Query(Text, "[ Enter FuzzFactor Threshold: ]", 2, 0x4F,
             Pop_Help2)) continue;
	  if ((Fuzz = atoi(Text)) < 2) Fuzz = 5; break;
       case F9:
          strcpy(CurrentName, Name);
	  if (Query(Name, "[ Enter File Name: ]", -2, 0x4E, Pop_Help)) {
	     if (FileExists(Name)) {
                GetLineN(Name, 0); Show(0); if (CrashStop) return; }
	     strcpy(Name, CurrentName); GetLineN(Name, 0);
          }
          break;
       case F10:       UserSetsColors(); break;
       default: TopLine += 11;
       }
       HideCursor(); PaintCRT(TopLine);
    }
}

void Usage(void)
{
    char   **dp = Documentation;

    for ( ; *dp; dp++) fprintf(stderr,"%s\n", *dp);
    exit(1);
}

void UserSetsColors(void)
{
    int i;

    strcpy(Text, "[ Set/Report_Current_Screen_Colors. ]");
    PadEnds(Text, UNDER, 80); Dwrite(1, 1, HiLite, Text);
    Dwrite(1, 25, BarColor,
"Text: Left/Right.  Bars:Up/Down. UnderLined_Text: ^Left/Right.  <Esc> Exits."
);
    for (EVER) {

    /* Query User */
       switch (Kbq_read()) {
       case ESC:
	  sprintf(Text, "[ Current Color Setting: /C0x%02X%02X%02X ]",
	     NmlColor, BarColor, HiLite);
	  PadEnds(Text, 196, 80); Dwrite(1, 1, 0x4E,Text);
	  strcpy(Text, "[ Hit Any Key]");
	  PadEnds(Text, 196, 80); Dwrite(1, 25, 0x4E, Text);
	  Kbq_read(); return;
       case FWD: NmlColor = (NmlColor & 0x70) + ((NmlColor+1) % 16); break;
       case UP:  BarColor = (BarColor & 0x70) + ((BarColor+1) % 16); break;
       case DN:  BarColor = (BarColor + 0x10) % 0x80; break;
       case BWD: NmlColor = (NmlColor + 0x10) % 0x80; break;
       case CTL_FWD:
	   HiLite = (HiLite & 0x70) + ((HiLite+1) % 16); break;
       case CTL_BWD: HiLite = (HiLite + 0x10) % 0x80; break;
       }
       for (i = 2; i < 25; i++) RcolorSet(1, i, NmlColor, 80);
       RcolorSet(1, 1, HiLite, 80); RcolorSet(1, 25, BarColor, 80);
    }
}

