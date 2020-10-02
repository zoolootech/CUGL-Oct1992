

#include <stdio.h>
#include <ctype.h>
#include <io.h>
#include <stdlib.h>
#include <stdek.h>

/* GARBLE ---> A Text Scrambling Utility.   5 October 89
 *
 * Author: J.Ekwall
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Responcibilities.
 *
 * Last Update: 5 October 89/EK
 */

/* Declare ProtoTypes */
void Blip(void);
void Decode(void);
void Encode(void);
int Password(char *);
void Usage(void);

/* Declare Globals */
int K1, K2, K3, U_Flag = FALSE;
char Text[345];
FILE *fp = stdin;

/* Roman Centurians guarded their field orders by writing them on a ribbon
 * wrapped around their Staff (a symbol of their rank).  Since the message
 * was written along the axis, the unwrapped ribbon was a ramble of letters.
 *
 * Decoding was done by unrolling the ribbon and wrapping it around a Staff
 * of similar diameter.  Quick, simple and effective.
 *
 * GARBLE uses a buffer and a "Jump-Read" size based on a Password, in effect
 * varying the "Diameter" of the Staff.
 */

main (int argc, char *argv[])
{
    int c, i;
    char *tp1;

 /* Check Passed Parameters */
    if (argc IS 1) Usage();
    while (*argv[1] IS '/') {
       for (tp1 = argv[1] + 1;(c = toupper(*tp1++)) != NULL; ){
          switch (c) {
          case 'U': U_Flag = TRUE; break;
          default: Usage();
          }
       }
       for (i = 1; i < argc; i++) argv[i] = argv[i + 1]; argc--;
    }

 /* Verify & Recover Password */
    if ((argc IS 1) || (argc > 3)) Usage();
    if (!Password(argv[1])) Usage();

 /* Open Specified File (If Any) */
    if (argc IS 3)
       if ((fp = fopen(argv[2],"r")) IS NULL) {
          perror(argv[2]); exit(1); }
    if ((argc IS 2) && !INFLOW_EXISTS) Usage();


 /* Do Business */
    if (U_Flag) Decode(); else Encode();
}

void Blip(void)
{
    static int i = 0;

    putc(DOT,stderr); if (i++ IS 78) { putc(NL,stderr); i = 0; }
}

void Decode(void)
{
    int c, i, j;
    char *tp1;

    while ((c = getc(fp)) != EOF) {
       ungetc(c,fp); if (OUTFLOW_EXISTS) Blip();

    /* Jump-Load Text Buffer */
       for (i = 0, j = K2; i < K3; i++) {
          if ((c = getc(fp)) IS EOF) Usage();
          if (c IS 255) c = NULL; Text[j] = c;
          if ((j += K1) >= K3) j %= K3;
       }

    /* Print ReCovered Text */
       Text[K3] = NULL; printf("%s",Text);
    }
}

void Encode(void)
{
    int c, i, j;
    char *tp1;

 /* PreLoad Text$ w/ Junk */
    for (i = 0, tp1 = Text; i < K3; i++) {
       while ((c = random(127)) < SPACE); *tp1++ = c; }

 /* Fill Text$ & Jump-Read into Stdout */
    while ((c = getc(fp)) != EOF) {
       ungetc(c,fp); Blip();
       for (i = 0, tp1 = Text; i < K3; i++) {
          if ((c = getc(fp)) IS EOF) { *tp1 = 255; break; }
          *tp1++ = c;
       }

    /* Jump-Read Text Buffer */
       for (i = 0, j = K2; i < K3; i++) {
          putchar(Text[j]);
          if ((j += K1) >= K3) j %= K3;
       }
    }
}

int Password(char *Passwd)
{
    int c, i;
    int A_Flag = FALSE, HC_Flag = FALSE, LC_Flag = FALSE, N_Flag = FALSE;
    char *tp1;

 /* Capture & Validate Password */
    for (tp1 = Passwd, K1 = K2 = i = 0; (c = *tp1++) != NULL; i++) {
       if (isalnum(c)) A_Flag = TRUE; else N_Flag = TRUE;
       if (isupper(c)) HC_Flag = TRUE;
       if (islower(c)) LC_Flag = TRUE;
       if (i < 3) K1 += c; else K2 += c;
    }

 /* Fold Keys */
    K1 %= 16; K1 += 3; K3 = K1 * K1 + K1; K2 %= K3--;
    fprintf(stderr,"\nKey1 = %d, Key2 = %d, Key3 = %d.\n",K1,K2,K3);


    if ((i > 5) && (i < 17) && A_Flag && N_Flag && HC_Flag && LC_Flag)
       return(TRUE);

    fprintf(stderr,"\nGARBLE: Invalid Password.\n");
    fprintf(stderr,"     6-16 CHR$ & Mixed Case w/ Non-Alphanumeric.\n\n");
    return(FALSE);
}

void Usage(void)
{
    fprintf(stderr, "\nUsage:\n");
    fprintf(stderr,
     "       GARBLE [/U] Password [File] ---> Scramble Text.\n\n");
    exit(1);
}

