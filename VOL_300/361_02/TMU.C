
#include <stdek.h>
#include <stdio.h>
#include <gadgets.h>
#include <process.h>
#include <keys.h>



int TMu(char **Mu, int X, int Y, long Color, int Border)
{
    int ch, i, xx, yy, Flag, GreyOut, HiLite, Lit, Selected, Z;
    char *FootPrint;
    extern char HelpTag[20];

 /* Save Cursor, Calculate Menu Size & Get Colors */
    Getxy(&xx, &yy); Flag = HideCursor(); if (Mu[0][10] IS '+') Border >>= 8;
    GreyOut = Color & 0xFF; Color >>= 8; HiLite  = Color & 0xFF; Color >>= 8;
    if (Mu[0][10] IS '-') Color = GreyOut;
    for (Z = 1; Mu[Z+1] != NULL ; Z++);
    if ((FootPrint = (char *) malloc(strlen((Mu[1]) + Z+3) * 2)) IS NULL)
       return 0;

 /* Build a Drop Box Menu */
    MkAskBox(FootPrint, X, Y, strlen(Mu[1])-1, Z+2, 'R',
       (char *) strchr(*Mu, '['), "[<Esc> to Cancel]", "", Border);
    for (i = 0; i++ < Z; )
        if (Mu[i][2] IS '-') Dwrite(X+1, Y+i, GreyOut,  Mu[i]+3);
        else Dwrite(X+1, Y+i, Color, Mu[i]+3);
    Lit = Selected = 1; Dwrite(X+1, Y+1, HiLite, Mu[1]+3); HideCursor();

 /* Querry User */
    do {
       if (Selected != Lit) {
          if (Mu[Lit][2] IS '-') Dwrite(X+1, Y+Lit, GreyOut, Mu[Lit]+3);
          else Dwrite(X+1, Y+Lit, Color, Mu[Lit]+3);
          Lit = Selected; Dwrite(X+1, Y+Lit, HiLite, Mu[Lit]+3);
       }
       switch (ch = Kbq_read()) {
       case F1:
          strncpy(HelpTag, Mu[0], 10); HelpTag[10] = NULL; TisHelp();
          break;
       case DN:   if (Selected++ IS Z) Selected = 1; break;
       case UP:   if (Selected-- IS 1) Selected = Z; break;
       case FWD:
       case BWD:
       case ESC:  Selected = ch = 0; break;
       case CR:   if (Mu[Selected][2] IS '+') ch = 0; break;
       default:
          for (i = 0; ch != SPACE && i++ < Z; )
             if (*Mu[i] IS ch && Mu[i][2] IS '+') {
                ch = 0; Selected = i; break; }
       }
    } while (ch);

 /* Clean Up & Split */
    ZapAskBox(FootPrint, X, Y, strlen(Mu[1])-1, Z+2, 'R'); free(FootPrint);
    Gotoxy(xx, yy); if (Flag) ShowCursor(0);
    return (!Selected || Mu[0][2] IS '-') ? 0 : Mu[Selected][1];
}

