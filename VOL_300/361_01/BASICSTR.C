
/* BasicStr.c --> A Collection of String Tools
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 18 September 91/EK
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdek.h>
#include <gadgets.h>

void ExpandTabs(char *Text)
{
    int i;
    
    for (i = 0; Text[i]; i++)
       if (Text[i] IS HT) { 
          Text[i++] = SPACE; OpenStr(Text + i, (8 - (i % 8)) % 8); --i; }
}

void PadLeft(char *Text, int ch, int N)
{
    if (N > strlen(Text)) {
       strrev(Text); PadRight(Text, ch, N); strrev(Text); }
}

void PadRight(char *Text, int ch, int N)
{
    if ((N -= strlen(Text)) > 0) {
       for (Text += strlen(Text); N--; ) *Text++ = ch; *Text = NULL; }
}

void PadEnds(char *Text, int ch, int N)
{
   if (*Text) PadLeft(Text, ch, (N + strlen(Text)) / 2);
   PadRight(Text, ch, N);
}

void RepeatChr(char *Target, int ch, int N)
{ *Target = NULL; PadRight(Target, ch, N); }

void Remove(char *Text, int ch)
{ while ((Text = strchr(Text, ch)) != NULL) strcpy(Text, Text + 1); }

int Sar(char *Text, char *This, char *That, int StopAt)
{
    int i, j, k, n = 0;

    if (!StopAt || (i = strlen(This)) IS 0) return 0; j = strlen(That);
    if (strstr(That, This) != NULL) return 0;
    for (Text = strstr(Text, This); n < StopAt && Text != NULL; n++) {
       strcpy(Text, Text + i);
       if (j) { Strrcpy(Text+j, Text); memcpy(Text, That, j); }
       Text = strstr(Text, This);
    }
    return n;
}

void Strrcpy(char *Target, char *Source)
/* Target may be Inside Source */
{
    int N;

    for (N = strlen(Source), Source += N, Target += N++; N--; )
       *Target-- = *Source--;
}

int Tally(char *Text, int ch)
{
    int n;

    for(n = 0; (Text = strchr(Text, ch)) != NULL; n++) Text++;
    return n;
}

char *TrimStr(char *Text, int Flag)
{
    int i;

    if (Flag > 2 || Flag < 0 || !*Text) Flag = 0;
    if (!Flag || Flag > 1)
       while ((i = strlen(Text) - 1) >= 0 && isspace(Text[i])) Text[i] = NULL;
    if (Flag < 2 && (i = strspn(Text, " \t\n\f")) > 0) strcpy(Text, Text + i);
    return Text;
}

