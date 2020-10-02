
/* MSO --> MOREd lists w/ Selection & Output
 *
 * J.Ekwall 27 November 89
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 */

#include <ctype.h>
#include <gadgets.h>
#include <keys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdek.h>

char *Documentation[] = {
    "",
    "Usage:",
    "       MSO [options] [file] ---> Present a List & Output Selection(s).",
    "",
    "  Note: $Pipes are Legal in \"file\" Name.",
    "",
    "Options:",
    "         /Cn ---> Specify # of Columns Displayed [n = 1-4;D:4]",
    "         /N ----> Prefix Selections w/ Item Line Numbers.",
    "         /R ----> Multiple Selections Permitted.",
    "",
    "   MSO may be Piped In/Out w/o Disrupting List Display/Selection.",
    "",
    "Last Update: 20 September 91/EK",
    "",
    NULL};

/* Declare Golbals */
int CleanUp = FALSE;
int Offset = 1, Bins = 4, Last_Item =  2500;
char Text[32500], *Item[2502], Name[81], FootPrint[4000];
FILE *fp = stdin;

/* Declare ProtoTypes */
int  Choose(int);
void Tload(FILE *);
void Usage(void);

main (int argc, char *argv[])
{
    int c, i, Repeats = FALSE, Number = FALSE;
    int xx, yy;
    char *tp1;

 /* Set Option Flags */
    while (*argv[1] IS SLASH) {
       for (tp1 = argv[1] + 1; *tp1 != NULL; ) {
          switch (toupper(*tp1++)) {
          case 'C':
             c = *tp1++; if ((c < '1') || (c > '4')) Usage();
             Bins = c - 48; break;
          case 'R':
             Repeats = TRUE; break;
          case 'N':
             Number = TRUE; break;
          }
       }

    /* SHIFT */
       for (i = 1, --argc; i < argc + 1; i++) argv[i] = argv[i+1];
    }
    if (argc > 2) Usage();
    if (argc IS 2)
       if (*argv[1] IS '$') {
          strcpy(Name,"\\STD "); strcat(Name,++argv[1]);
          if ((fp = fopen(Name,"r")) IS NULL) { perror(Name); exit(1); }
          CleanUp = TRUE;
       } else {
          strcpy(Name,argv[1]);
          if ((fp = fopen(Name,"r")) IS NULL) { perror(Name); Usage(); }
          CleanUp = FALSE;
       }


 /* Check for Pipe on Stdin */
    if (!INFLOW_EXISTS && fp IS stdin) Usage();

 /* Load Text Buffer */
    Getxy(&xx, &yy); SaveBox(1, 1, 80, 25, FootPrint);
    fprintf(stderr,"\nLoading List Buffer.  Please Wait.");
    Tload(fp);

 /* Display Lists & Capture User Selection */
    for(i = 0; (i = Choose(i)) != 0; ) {
       if (i < 0) continue;
       if (Number IS TRUE) printf("%d  ", i);  /* Prefix Line # */
       for (tp1 = Item[i] + 2; ; ) { putchar(*tp1); if (*tp1++ IS NL) break; }
       if (!Repeats) break;
    }
    RestoreBox(1, 1, 80, 25, FootPrint); Gotoxy(xx, yy); ShowCursor(0);
    if (CleanUp IS TRUE) unlink(Name);    /* DEL $Pipe */
    exit(OK);
}

int Choose(int Flag)
{
    int c, j, Dx = 0, xx, yy;
    char *tp1, Text[81];
    static int i, Offset = 0, X = 1, Y = 1;

 /* Set Initial Conditions */
    switch (Bins) {
    case 1: Dx = 40;
    case 2: Dx += 14;
    case 3: Dx += 6;
    case 4: Dx += 20;
    }

 /* Don't RePaint on Multi-Selections */
    if (Flag > 0) goto ReRun;

 /* Clear & Goto Bottom & Set White on Blue & Do Banner */
    HideCursor(); Clr(0x0A); RcolorSet(1, 25, 0x1F, 80);
    Dwrite(33, 25, 0x1F, "Pg Up/Dwn        ESC to Quit.");
    Gotoxy(1, 1);

 /* Display/ReDisplay a Page of Text w/ Banner */
    for (X = 1, i = 1; X < 75; X += Dx) {
       for (Y = 1; Y < 25; Y++) {
          if ((Offset + i) > Last_Item) break;
          tp1 = Item[Offset + i]; Gotoxy(X, Y);
          tp1 = strchr(tp1, NL); *tp1 = NULL;
          Dwrite(X, Y, 0x0A, Item[Offset + i++]); *tp1 = NL;
       }
    }

 /* Do MORE & Set Yellow ">" on First Item */
    if ((Offset + i) < Last_Item) Dwrite(1, 25, 0x1F, "  --- More --- ");
    Dwrite(2, 1, 0x0E, ">");

 /* Get User's Input(s) */
     X = Y = 1;
ReRun:
    for ( j = Offset; j IS Offset; ) {
       switch (toupper(Kbq_read())) {
       case UP:     if ( Y > 1) Y--; break;
       case DN:     if (Y < 24) Y++; break;
       case TAB:
       case FWD:    if (X + Dx < 75) X += Dx; break;
       case BS:
       case BWD:    if (X > 1) X -= Dx; break;
       case HOME:   Offset = 0;
       case PGUP:   Offset -= 24 * Bins; if (Offset < 0) Offset = 0; break;
       case END:    Offset = 2000;
       case SPACE:
       case PGDN:  
          Offset += 24 * Bins;
          if (Offset > Last_Item - 24 * Bins) Offset = Last_Item - 24 * Bins;
          if (Offset < 0) Offset = 0; break;
       case 'Q':
       case ESC:  return(0);
       case CR:   /* Select */
          i = Offset + (X / Dx) * 24 + Y;
          if (i > Last_Item) break;
          Dwrite(X, Y, 0x0E, "-"); *Item[i] = '-';
          return(i);
       }
       Getxy(&xx, &yy); Dwrite(xx-1, yy, 0x0A, " ");
       Dwrite(X+1, Y, 0x0E, ">");
    }
    return(-1);
}

void Tload(FILE *File)
{
    int c, i, x, Mx, Skip_Flag = FALSE;
    char *tp1, *tp2;

 /* Set Max Item Length */
    switch (Bins) {
    case 1: { Mx = 77; break; }
    case 2: { Mx = 37; break; }
    case 3: { Mx = 23; break; }
    case 4: { Mx = 17; break; }
    }

 /* Load Text Buffer */
    tp1 = Text, tp2 = Text + 32495, Item[(i = 1)] = tp1, x = 0;
    for (*tp1++ = SPACE, *tp1++ = SPACE; (c = getc(File)) != EOF; ) {
       if (c != NL) {
          if (Skip_Flag IS TRUE) continue;
          *tp1++ = c;
          if (tp1 > tp2) {
             *tp1++ = NL; *tp1 = NULL;
             fprintf(stderr,"Buffer Overflow."); Last_Item = i - 1; return; }
          if (x++ IS Mx) Skip_Flag = TRUE;
          continue;
       }
       *tp1++ = NL; *tp1 = NULL; Skip_Flag = FALSE; x = 0; Item[++i] = tp1;
       *tp1++ = SPACE, *tp1++ = SPACE;
       if (i > 2500) { fprintf(stderr,"Items 2500+ Skipped."); break; }
    }
    if ((Last_Item = i - 1) < 1) exit(1);
}

void Usage(void)
{
    char   **dp = Documentation;

    for ( ; *dp; dp++) fprintf(stderr,"%s\n", *dp);
    exit(1);
}

