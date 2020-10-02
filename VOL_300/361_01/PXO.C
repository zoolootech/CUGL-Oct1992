
/* PXO.C  --> A Pipe Extension OutFlow Tool w/ Named Pipes.
 *
 * Jack Ekwall 2 May 89
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update:  29 September 89/EK
 */

#include <stdio.h>
#include <stdek.h>
#include <ctype.h>
#include <string.h>

/* Delare Globals */
char Pipe[16] = "\\STD OUT";

/* Declare Prototypes */
void Usage(void);

main (int argc, char *argv[])
{
    int c, i;
    FILE *fp = stdout;

 /* Check for a Pipe Name */
    if (argc > 2) Usage();
    if (argc IS 2) {
       if (*argv[1]++ != '$') Usage();
       if (strlen(argv[1]) > 4 || *argv[1] IS NULL) Usage();
       strcpy(Pipe + 5,argv[1]);
    }

 /* Check for Pipes */
    if (!INFLOW_EXISTS) Usage();
    if (OUTFLOW_EXISTS)
       if ((fp = fopen(Pipe,"a")) IS NULL) { perror(Pipe); exit(1); }

 /* Stream Text to Specified Target */
    while ((c = getchar()) != EOF) putc(c, fp);
    exit(0);
}

void Usage()
{
    fprintf(stderr,"\n\nUsage:\n");
    fprintf(stderr,"      PXO [$Pipe] ---> BATch File PX/CRT Pipe Cap.\n\n");
    fprintf(stderr,
"      IF Called By PX & Outflow is Piped, PXO Squirts Text into \"\\STD OUT\" or\n");
    fprintf(stderr,
"      into Specified Named Pipe.  (Name = 1-4 Alpha-numeric Characters.)\n\n");
    exit(1);
}
