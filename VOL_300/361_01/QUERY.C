
/* Query.c --> A Gernal Purpose "Ask Box" w/ Context Sensitive Help
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 13 September 91/EK
 */

#include <stdek.h>
#include <gadgets.h>
#include <stdio.h>
#include <keys.h>

int AnyCharacter(void)
{ /* Create Any Character */
    int c, ch = 0, n = 3, xx, yy;
    char FootPrint[216];

 /* Ask 'em */
    Getxy(&xx, &yy);
    MkAskBox(FootPrint, 28, 11, 26, 3, 'L', "[ ASCII Value ]",
       "[ <ESC> to Cancel ]", "Enter Three Digits: ", 0x4E);

 /* Catch Response, Build Character & Split */
    while (n && (c = Kbq_read()) != ESC && c != CR)
       if (isdigit(c)) { ch = 10 * ch + c - '0'; n--; DputChr(c, 0x4E); }
    ZapAskBox(FootPrint, 28, 11, 26, 3, 'L'); Gotoxy(xx, yy); ShowCursor(1);
    if (c IS ESC) return 0; else return ch & 0xFF;
}

int Query(char *Text, char *TopTitle, int CaseLess, BYTE Color,
   char **Help2Pop)
{
    int c, xx, yy;
    char *tp1, FootPrint[488];

 /* Establish Window */
    tp1 = Text; Getxy(&xx, &yy);
    MkAskBox(FootPrint, 5, 10, 60, 3, 'L', TopTitle,
          "[ <Esc> to Cancel ]", Text,  Color);
    while ((c = Kbq_read()) != CR) {
       if (CaseLess) c = toupper(c);
       if (c IS ESC) { *Text = NULL; Text[1] = ESC; break; }
       else if (c IS BS) {
          if (tp1 IS Text) {
             if (!strlen(Text)) continue; tp1 += strlen(Text);  }
          tp1--; c = NULL; }
       else if (c IS F1 && Help2Pop != NULL) { PopHelp(Help2Pop); continue; }
       else if (c IS F2) c = AnyCharacter();
       else if (CaseLess IS -1) { if (!isdigit(c)) continue; }
       else if (CaseLess IS -2 && c IS SPACE) continue;
       else if (!iscntrl(c) && isascii(c));
       else continue;
       if (c && strlen(Text) < 57) *tp1++ = c; *tp1 = NULL;
       DwriteEnd(6, 11, Color, Text, 57);
    }
    ZapAskBox(FootPrint, 5, 10, 60, 3, 'L'); Gotoxy(xx, yy); ShowCursor(1);
    return !(!*Text && Text[1] IS ESC);
}

void PopHelp(char **PopHelpPtr)
{
    int i, j, xx, yy, Flag;
    char   **dp, FootPrint[2810];

    Getxy(&xx, &yy);
    for (i = 2, dp = PopHelpPtr; *dp; dp++) i++; if (i > 22) i = 22;
    Flag = HideCursor();
    MkAskBox(FootPrint, 5, 2, 60, i, 'L', "", "[ Hit Any Key ]", "", 0x0A);
    HideCursor();
    for (dp = PopHelpPtr, j = 3; *dp; dp++) {
       Dwrite(6, j, 0x0A, *dp);
       if (++j IS 22 || dp[1] IS NULL) {
          if (dp[1] !=  NULL) Dwrite(6, j, 0x0A, "--- More --- ");
          switch (Kbq_read()) {
          case  ESC:  j = EOF; break;
          case PGUP:
             if ((dp -= 18) < PopHelpPtr + 10) dp = PopHelpPtr + 9;
          default:   if (dp[1]  != NULL) dp -= 10;
          }
          if (j IS EOF) break;  j = 3; ClrBox(6, 3, 63, 22, 0x0A);
       }
    }
    ZapAskBox(FootPrint, 5, 2, 60, i, 'L'); Gotoxy(xx, yy);
    if (Flag) ShowCursor(0);
}

