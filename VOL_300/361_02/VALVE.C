
/* (#) VALVE --> Pass or WOM stdin => stdout.  Trip on NL or Specified CHR$.
 *
 * Author: J.Ekwall 23 November 1987.
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Autorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update:  5 December 89/EK
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <io.h>
#include <string.h>
#include <stdek.h>

char *Documentation[] = {
    "",
    "Usage:",
    "       Valve [options] [filename] --> Control Text Flow.",
    "",
    "Options:",
    "          /An ---> Trip on the \"n\"th Trigger Character. [D:1]",
    "          /B ----> Block Text Until Triggered.",
    "          /Cc ---> Trigger on 'c' vice NL.  'c' is any Printable.",
    "                   or $$ or $### (3 Decimal Digits) for any ASCII.",
    "          /F ----> FIFO for Lines.  Pops Top Line Each Call.",
    "          /Hn ---> Pass Head (Top) n Lines.  [D:10]",
    "          /I ----> Initial Character Trips.  (Ignore Everything Else).",
    "                   /C option Required.",
    "          /J ----> Jog.  (Trip Twice & Hold Initial State).",
    "          /L-----> LIFO for Lines.  Pops Last Line Each Call.",
    "          /Tn ---> Pass Tail (Bottom) n Lines.  [D:10]",
    "          /V ----> Verbose.  Echo Text to CRT Until Triggered.",
    "",
    "   Valve defaults to a Filter.  $Pipes are Legal in filename.",
    "",
    "   After Loading, Repeated /F or /L Calls Pop Lines until Stack/Queue ",
    "is Empty.  Text is Stored in the Named Pipe, $VALV.",
    "",
    NULL };

/* Define Constants */
#define PIPE    "\\STD VALV"
#define PUKA    "\\STD VPUK"            /* "Puka" Hawaiia for Pigeon Hole */

/* Declare Globale */
int CleanUp, Flow_Mode;;
char Name[80];
FILE *fp = stdin;

/* Declare ProtoTypes */
void Usage(void);

main (int argc, char *argv[])
{
    int c, i, n, Count = 0, NL_Flag, Trip_Chr = NL;
    int Echo = FALSE, Flow = TRUE, Save = FALSE, Tail = FALSE, Ignore = FALSE;
    int Jog = FALSE, Fifo = FALSE, Lifo = FALSE, Lines_Only = FALSE;
    char *tp1;
    FILE *wfp;

 /* Set Option Flags */
    while (*argv[1] IS SLASH) {
       for (tp1 = argv[1] + 1; *tp1 != NULL; ) {
          switch (toupper(*tp1++)) {
          case 'A':                             /* Action Skip Count */
             if (Count > 0) Usage();
             for (Count = 0; (*tp1 >= '0') && (*tp1 <= '9'); tp1++)
                Count = 10 * Count + *tp1 - 48;
             if (Count > 0) --Count;
             break;
          case 'B': Flow = FALSE; break;        /* Block to Start */
          case 'C':                             /* Trigger Character Change */
             if ((c = (BYTE) *tp1++) IS '$') {
                for (c = 0; (*tp1 >= '0') && (*tp1 <= '9'); tp1++)
                   c = 10 * c + *tp1 - 48;
                if ((c IS 0) && (*tp1 IS '$')) { c = '$';  tp1++; }
             }
             Trip_Chr = c; if (Trip_Chr IS 0) Usage();
             break;
          case 'F': Fifo = Save = Lines_Only = TRUE; break;
          case 'H':                             /* Head */
             Lines_Only = TRUE;
             for (Count = -1; (*tp1 >= '0') && (*tp1 <= '9'); tp1++)
                Count = 10 * Count + *tp1 - 48;
             if (Count < 0) Count = 9;
             break;
          case 'I': Ignore = TRUE; break;
          case 'J': Jog = TRUE; break;
          case 'L': Lifo = Save = Lines_Only = Tail = TRUE; break;
          case 'T':                             /* Tail */
             Tail = Lines_Only = TRUE;
             for (Count = 0; (*tp1 >= '0') && (*tp1 <= '9'); tp1++)
                Count = 10 * Count + *tp1 - 48;
             if (Count IS 0) Count = 10;
             break;
          case 'V':     /* Verbose */
             Echo = TRUE; break;
          default:
             fprintf(stderr,"\nInvalid Option [%s].\n\n",argv[1]);
             Usage();
          }
       }

    /* SHIFT */
       for (i = 1, --argc; i < argc + 1; i++) argv[i] = argv[i+1];
    }

 /* Validate Options & Flags */
    if (Fifo && Lifo) Usage();
    if (Jog && (Lifo || Fifo)) Usage();
    if ((Trip_Chr != NL) && Lines_Only) Usage();
    if ((Trip_Chr IS NL) && Ignore) Usage();
    if (argc > 2) Usage();

 /* Open Specified File (If Any) */
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

 /* Check for Piping */
    if (!Save && !INFLOW_EXISTS && (fp IS stdin)) Usage();

 /* Set Up Streams & Pigeon Holes */
    if ((Save) && !(INFLOW_EXISTS) && (fp IS stdin))
       if ((fp = fopen(PIPE,"r")) IS NULL) exit(0);
    if (Tail || Save) {                         /* Stuff Pigeon Hole File */
       if (Tail) {
          Count = -(++Count); Flow = !Flow; if (Fifo || Lifo) Count--; }
       if ((wfp = fopen(PUKA,"w")) IS NULL) { perror(PUKA); exit(1); }
       while ((c = getc(fp)) != EOF) {
          if ((c IS Trip_Chr) && (Tail)) Count++; /* Figure Offset fm SOF */
          putc(c,wfp);
       }
       fclose(fp); fclose(wfp);
       if ((fp = fopen(PUKA,"r")) IS NULL) { perror(PUKA); exit(1); }
    }
    if (Save) if ((wfp = fopen(PIPE,"w")) IS NULL) { perror(PIPE); exit(1); }

 /* Perform Pass/Block Action */
    for (NL_Flag = TRUE; (c = getc(fp)) != EOF ; NL_Flag = (c IS NL)) {
       if (c IS Trip_Chr) {
          if (!NL_Flag && Ignore) Count++;
          if (Count-- IS 0) {
             if (Trip_Chr IS NL) {      /* Terminate Hanging Line */
                if (Flow IS TRUE) putchar(c);
                if ((!Flow) && (Save)) putc(c,wfp);
                if (Echo) putc(c,stderr);
             }
             Echo = FALSE;
             if ((Flow = !Flow) IS FALSE)
                if (!Save && !Jog) break;
             if (Jog) {Jog = FALSE; Count = 0; }
             continue;
          }
       }
       if (Flow IS TRUE) putchar(c);
       if ((!Flow) && (Save)) putc(c,wfp);
       if (Echo) putc(c,stderr);
    }

 /* CleanUp & Split */
    if (CleanUp IS TRUE) { fclose(fp); unlink(Name); }
    if (fp != NULL) { fclose(fp); unlink(PUKA); }
    if (wfp != NULL) {  /* Zip MT Pipes */
       fclose(wfp);
       if ((fp = fopen(PIPE,"r")) != NULL) {
          c = getc(fp); fclose(fp); if (c IS EOF) unlink(PIPE); }
    }
    exit(0);
}


void Usage(void)
{
    char   **dp = Documentation;

    for ( ; *dp; dp++) fprintf(stderr,"%s\n", *dp);
    exit(1);
}

