
/* RSX.C command  --> Read in a Single line & eXecute "command"
 *                    w/ subtitutions.
 *
 * Author: Jack Ekwall 1 May 89
 *
 * Works Better if You Compile w/o WildCard Expansion.
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * Last Update: 7 August 90/EK
 */

#include <stdio.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <stdek.h>
#include <stdlib.h>

char *Documentation[] = {
    "",
"Usage:",
"       RSX [/Fc] Command [Arguements] --> Do Command w/ Parameter Passing.",
    "",
"Options:",
"         /Fc ---> Use 'c' as the Field Delimited vice Spaces.",
"                  The $### format for Decimal ASCII is Legal.",
    "",
"Action:",
"    1. RSX reads a Single Line of Text from Stdin.",
"    2. Using the Field Delimiter, Slots $1-$9 are stuffed.  ($0 is the ",
"       whole Line.)",
"    3. RSX Spawns a Sub-Shell, passing to it a \"Command Line\" made up ",
"       from Passed Parameters w/ these modifications:",
"          A. Contents of Field Slots get substitued for $Field Tags.",
"          B. Bangs (!) and Dits (') become (|) & (\"), respecitively.",
"          C. $$ becomes $.",
"          D. DOS WildCards are Passed Unchanged.",
"    4. RSX Repeats the Above for Until Stdin Runs Dry.",
    "",
    NULL};

/* Declare Prototypes */
void Usage(void);

main (int argc, char *argv[])
{
   int c, i, Flag = FALSE, Dlim = SPACE , Ok = TRUE;
   BYTE Command[128], CmdLine[128];             /* Cmd B4 & After Stuffing */
   char *tp1, *tp2, *tp3;                       /* Text Pointers */
   BYTE *Text, Slot[11][128];                   /* Line & Words fm Stdin */
   FILE *fp1, *fp2;                             /* Read, Write & Batch */

   Text = Slot[0];

 /* Check for /Option */
    if (open("\\STD ERR", 1) >= 0) exit(1);
    if (argc IS 1) Usage();
    tp1 = argv[1];
    if (*tp1++ IS '/') {
       if (toupper(*tp1++) != 'F') Usage();
       if (argc IS 3) Usage(); }
       if ((c = (BYTE) *tp1++) IS '$') {
          for (c = 0; (*tp1 >= '0') && (*tp1 <= '9'); tp1++)
             c = 10 * c + *tp1 - 48;
          if ((c IS 0) && (*tp1 IS '$')) { c = '$';  tp1++; }
       }
       if (c IS 0) Usage();
       if (*tp1 IS NULL) { Dlim = c;

    /* SHIFT */
       for (i = 1, --argc; i < argc + 1; i++) argv[i] = argv[i+1];
    }

 /* Build Command$ */
    for (i = 1, tp1 = Command; i < argc; i++) {
       tp2 = argv[i];
       while (*tp2 != NULL) {
          c = *tp2++;
          if (c IS BANG) c = BAR;
          if (c IS DIT) c = QUOTE;
          *tp1++ = c;
       }
       *tp1++ = SPACE;
    }
    tp1--; *tp1 = NULL;

 /* Check for Pipe on Stdin */
    if (!INFLOW_EXISTS) Usage();

 /* Do Business */
    while (gets(Text) != NULL) {

    /* Parse Text$ into Slot$[] by CHR$(Dlim) */
       if (*Text IS NULL) continue;     /* Skip MT Line */
       for (i = 1; i < 10; i++) *Slot[i] = NULL;
       i = 1; Flag = FALSE; tp1 = Text; tp2 = Slot[i++];
       while ((c = *tp1++) != NULL) {
          if (c != Dlim) { Flag = TRUE; *tp2++ = c; continue; }
          if (Flag IS TRUE) {
             *tp2 = NULL; Flag = FALSE;
             if (i < 9) { tp2 = Slot[i++]; continue; }
             *tp2++ = c; Flag = TRUE;   /* Spill XS Text into $9 */
          }
       }
       *tp2 = NULL;

    /* Build Command Line & Swap In Slot$[] */
       for ( tp1 = Command, tp2 = CmdLine; (c = *tp1++) != NULL; ) {
          *tp2++ = c;
          if (c IS '$') {
             if (*tp1 IS '$') { tp1++; continue; }      /* $$ ==> $ */
             if ((*tp1 < '0') || (*tp1 > '9')) continue;
             strcpy(--tp2,Slot[*tp1++ - '0']);
             tp2 = CmdLine + strlen(CmdLine);
          }
       }
       *tp2 = NULL;

    /* Shell Out & Do Command Line */
       if (system(CmdLine) != 0) Ok = FALSE;
       if (open("\\STD ERR", 1) >= 0) exit(1);
    }

    exit(Ok);
}

void Usage(void)
{
    char   **dp = Documentation;

    for ( ; *dp; dp++) fprintf(stderr,"%s\n", *dp);
    exit(1);
}

