
/* SAR ---> Search & Replace.  A Streaming SAR ala Unix sed.
 *
 * J.Ekwall  17 May 89
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 05 October 90/EK
 */

#include <stdio.h>
#include <ctype.h>
#include <io.h>
#include <string.h>
#include <stdek.h>

char *Documentation[] = {
    "",
    "Usage:",
    "      SAR \"This\" \"That\" ---> Find <This> & Substitute <That>.",
    "      SAR \"This\" ----------> Find <This> & Remove It.",
    "",
    "Wild Cards:",
    "   ? --> Place Holder in <This>.  Any Character Matches.",
    "   ? Cannot Be First Character In <This>.",
    "   ? --> Place Holder in <That>.  Original Character Remains Unchanged.",
    "   $### --> An ASCII Character w/ Decimal Value <3 Digits>.",
    "   $$ ----> A Single Dollar Sign <$>.",
    "",
    "   SAR is a Filter.",
    "",
    NULL};

/* Declare ProtoTypes */
void Expand(char *);
void Usage(void);

main (int argc, char *argv[])
{
    int i, c, Flag = FALSE;
    unsigned char This[128], That[128], Tbuff[512];

 /* Check for Pipe & Passed Parameters */
    if (INFLOW_EXISTS IS FALSE) Usage();
    if ((argc IS 1) || (argc > 3)) Usage();
    strcpy(This,argv[1]);
    if (argc IS 3) strcpy(That,argv[2]); else *That = NULL;

 /* Expand $### Entries (If Any) */
    Expand(This); Expand(That);

 /* Do Business */
    i = 0; *Tbuff = NULL;
    while ((c = getchar()) != EOF) {

    /* Find <This> */
       if ((c IS This[i]) || (This[i] IS '?')) {
           Flag = TRUE; Tbuff[i++] = c; Tbuff[i] = NULL; continue; }

    /* No Match.  Now What? */
       if (Flag IS FALSE) { putchar(c); continue; }  /* Nothing in Tbuff */
       if (This[i] IS NULL) {                        /* Hit! */
          for (i = 0; That[i] != NULL; i++) {       /* Do Replacement */
             if (That[i] != '?') { Tbuff[i] = That[i]; continue; }
             if (i < strlen(This)) continue;
             Tbuff[i] = '?';                       /* User Goofed */
          }
          Tbuff[i] = NULL;
       }

    /* Stream Out Text & Re-Check Mis-Matched CHR$ */
       printf("%s",Tbuff); Flag = FALSE; i = 0; *Tbuff = NULL;
       if ((c IS This[i]) || (This[i] IS '?')) {
           Flag = TRUE; Tbuff[i++] = c; Tbuff[i] = NULL; continue;
       } else  putchar(c);
    }

 /* Send Anything Left-over in Tbuff$ */
    if (Flag IS TRUE) printf("%s",Tbuff);
}

void Expand(char *tp1)
{
   int i;

   while ((tp1 = strchr(tp1,'$')) != NULL) {    /* Find '$' */
      if (tp1[1] IS '$') {
          strcpy(tp1,&tp1[1]); continue; }      /* Found $$ => Zip 2nd $ */
      for (*tp1 = NULL, i = 1; i < 4 && isdigit(tp1[i]); i++) {    
          *tp1 = 10 * ((BYTE)(*tp1)) + tp1[i] - '0';
      }
      if (*tp1 IS NULL) Usage();
      strcpy(&tp1[1],&tp1[i]); tp1++;           /* Zip '###' Text */
   }
}


void Usage(void)
{
    char   **dp = Documentation;

    for ( ; *dp; dp++) fprintf(stderr,"%s\n", *dp);
    exit(1);
}
