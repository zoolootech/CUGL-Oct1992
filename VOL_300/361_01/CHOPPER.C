
/* CHOPPER ---> Crops a Byte Stream into 76 CHR$ Lines
 *
 * J.Ekwall 14 August 89
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * Last Update: 12 December 89/EK
 */

#include <stdio.h>
#include <io.h>
#include <stdek.h>

/* Declare Globals */
FILE *fp = stdin;

/* Declare ProtoTypes */
void Usage(void);

main (int argc, char *argv[])
{
    int c, CleanUp = FALSE, Tally;
    char Name[80], Text[80], *tp1;

 /* Open Specified File (If Any) */
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

 /* Check for Pipe on Stdin or Specified File */
    if (!INFLOW_EXISTS && (fp IS stdin)) Usage();

 /* Do Business */
    for (tp1 = Text, Tally = 0; (c = getc(fp)) != EOF;) {
       if (c IS NL) {
          *tp1 = NULL; printf("%s\n",Text); tp1 = Text; Tally = 0; continue; }
       *tp1++ = c;
       if (Tally++ IS 77) {
          c = getc(fp); ungetc(c,fp); if (c IS NL) continue;
          for (*tp1 = NULL; *tp1 != SPACE; tp1--) if (--Tally IS 65) break;
          if (*tp1 IS SPACE) {
             *tp1++ = NULL; printf("%s \n",Text); strcpy(Text,tp1);
          } else { printf("%s\n",Text); *Text = NULL; }
          tp1 = Text + strlen(Text); Tally = strlen(Text) + 1;
       }
    }
    *tp1 = NULL; if (*Text != NULL) printf("%s\n",Text);

 /* CleanUp & Split */
    if (CleanUp IS TRUE) { fclose(fp); unlink(Name); }
}

void Usage(void)
{
    fprintf(stderr, "\nUsage:\n");
    fprintf(stderr,
    "       CHOPPER [[drive:][path]filename]--> Hack Up Text into Lines.\n\n");
    fprintf(stderr, "   $Pipes are Legal.\n\n");
    exit(1);
}
