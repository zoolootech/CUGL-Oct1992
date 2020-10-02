
/* TBarMu ---> A Six-Pac of Drop Menu "Hot Zones" w/ Context Sensitive Help.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 17 September 91/EK
 */

#include <stdek.h>
#include <stdio.h>
#include <gadgets.h>
#include <process.h>
#include <keys.h>

int TBarMu(char **TBar[], char *Title, long BarColor, long MuColor)
{
 /* Create & display a Menu Bar (w/ Drop-Down Menues) & Query User
  * Returns: (Slot# << 8) + Tag Byte or ESC for hit ESC on Bar Level.
  */
    int i, xx, yy;
    int HiLite, GreyOut, Flag, Lit = 0, Rtn, Slot = 0, Z;
    char FootPrint[321];
    extern char HelpTag[20];

 /* Build Menu Bar */
    Getxy(&xx, &yy); Flag = HideCursor(); GreyOut = BarColor & 0xFF;
    HiLite = (BarColor >>= 8) & 0xFF; BarColor >>= 8;
    SaveBox(1, 1, 80, 1, FootPrint); Gotoxy(1,1); ClrTo(80, BarColor);
    for (Z = 0; TBar[Z] != NULL; Z++) {
       strncpy(HelpTag, TBar[Z][0], 10); HelpTag[10] = NULL;
       if (TBar[Z][0][10] IS '+') Dwrite(Z*10, 1, BarColor, HelpTag);
       else Dwrite(Z*10, 1, GreyOut, HelpTag);
       if (Z IS 5) break;
    }
    Dwrite(61, 1, BarColor, Title); RcolorSet(1, 1, HiLite, 10);

 /* Scroll Right/Left & Pop Drop Menue(s) */
    for (Rtn = 0; !Rtn; ) {
       switch (Kbq_read()) {
       case F1:
          strncpy(HelpTag, TBar[Lit][0], 10); HelpTag[10] = NULL; TisHelp();
          break;
       case FWD: if (Slot++ IS Z) Slot = 0; break;
       case BWD: if (Slot-- IS 0) Slot = Z; break;
       case ESC: Rtn = ESC; break;
       case DN:
       case CR:
          if (TBar[Lit][0][10]  IS '+') Rtn =
             TMu(TBar[Slot], 1+Slot*10, 2, MuColor, (BarColor << 8)+GreyOut);
          break;
       }
       if (Slot != Lit) {
          if (TBar[Lit][0][10]  IS '+') RcolorSet(1+Lit*10, 1, BarColor, 10);
          else RcolorSet(1+Lit*10, 1, GreyOut, 10);
          Lit = Slot; RcolorSet(1+Lit*10, 1, HiLite, 10);
       }
    }

 /* Clean Up & Split */
    RestoreBox(1, 1, 80, 1, FootPrint); Gotoxy(xx, yy);
    if (Flag) ShowCursor(0); if (Rtn IS ESC) return ESC;
    return Rtn + (++Slot << 8);
}

