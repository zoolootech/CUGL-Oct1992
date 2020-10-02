

/* APO  --> A "Boxed" Ask w/ Prompts & Output
 *
 * J.Ekwall     18 December 1989
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
#include <stdek.h>
#include <string.h>


char *Documentation[] = {
    "",
    "Usage:",
    "        APO [/option] text [title]",
    "",
    "Options:",
    "       /I ---> Ignore Case.  (Everything is UPPERS).",
    "       /N ---> Numbers Only.",
    "",
    "   Produces an \"Ask\" Box w/ Prompts & Seed Text.  Squirts to Stdout.",
    "",
    "   APO may be Piped w/o Disrupting User Input.",
    "",
    "Last Updated: 20 Sept 91/EK.",
    "",
    NULL};

char *Pop_Help[] = {
    "",
    " Esc ---> Cancel Input.",
    "",
    " F1 ----> Cheat Sheet Help.",
    " F2 ----> Build Any ASCII Character.",
    "",
    " BS ----> Rubout Text.",
    "",
    " Enter -> Accept Entry & Go On.",
    "",
    NULL };

/* Declare ProtoTypes */
void Usage(void);

main (int argc, char *argv[])
{
    int c, i, Flag = 0;
    char *tp1, Text[80], Title[80];

 /* Set Option Flags */
    while (*argv[1] IS SLASH) {
       for (tp1 = argv[1] + 1; *tp1 != NULL; ) {
          switch (toupper(*tp1++)) {
          case  'I': Flag =  1; break;          /* CaseLess */
          case  'N': Flag = -1; break;          /* Numerics */
          default:
             fprintf(stderr,"\nInvalid  Option [/%c].\n\n",*tp1); Usage();
          }
       }

    /* SHIFT <Preserve argv[0]> */
       for (i = 1, --argc; i < argc + 1; i++) argv[i] = argv[i+1];
    }
    /* Check for OutFlow Pipe */
    if (!OUTFLOW_EXISTS) Usage();

 /* Capture Prompts & Seed Text (IF Any) */
    if (argc IS 1) Usage();             /* Blank Box is Illegal */
    strcpy(Text,argv[1]);
    if (argc > 2) {
       strcpy(Title, "[ "); strcat(Title, argv[2]); strcat(Title, " ]");
    } else *Title = NULL;

 /* Mainline */
    if (Query(Text, Title, Flag, 0x4F, Pop_Help)) printf("%s\n", Text);
    printf("%c", DOS_EOF);
}

void Usage(void)
{
    char   **dp = Documentation;

    for ( ; *dp; dp++) fprintf(stderr,"%s\n", *dp);
    exit(1);
}

