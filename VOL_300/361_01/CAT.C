
/* CAT ---> Like the Unix "cat" w/ more options
 *
 * J.Ekwall   2 May 89
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update:  25 September 89/EK
 */

#include <stdio.h>
#include <ctype.h>
#include <io.h>
#include <stdek.h>

/* Declare ProtoTypes */
void Bag(int);
void Usage(void);

/* Declare Globals */
char *tp1, *tp2, *tpf = 0, Queue[128];
char Stdname[12] = "\\STD ";

main (int argc, char *argv[])
{
    int i, c, x, Flag = FALSE;
    int Part = 1, Step = 100, Bump = 100;
    FILE *fp;

 /* Check for Arguements */
    if (argc IS 1) Usage();


 /* Check for File List */
    if (*argv[1] != '/') {
       for (i = 1; i < argc; i++) {
          tp1 = argv[i]; if (*tp1 IS '/') Usage();
          if (*tp1 IS '-' && *(tp1 +1) IS NULL) {
             if (isatty(0) != FALSE) Usage();
             while ((c = getchar()) != EOF) putchar(c);
             continue;
          }
          if (*tp1 IS '$'){
             tp1++; tp2 = Stdname + 5;
             while (*tp1 != NULL) *tp2++ = *tp1++;
             *tp1 = NULL; tp1 = Stdname;
          }
          if ((fp = fopen(tp1, "rt")) IS NULL) {
             fprintf(stderr,"Cannot Open %s.\n",tp1);
             continue;
          }
          while ((c = getc(fp)) != EOF) putchar(c);
          fclose(fp);
       }
       exit(0);
    }

 /* Must be a Filter Action */
    if (argc != 2) Usage();
    tp1 = argv[1]; if (*tp1++ != '/') Usage();
    x = toupper(*tp1++); if (*tp1 != NULL) Usage();

 /* Flow Thru & Do Indicated Action */
    while ((c = getchar()) != EOF) {
       if (x IS 'Q') { Bag(c); continue; }
       if (Flag IS FALSE) {
          if ((x IS 'U') && (c != '.')) continue;
          Flag = TRUE;
          if (x IS 'N') printf("%d. ",Part++);
          if (x IS 'T') {
             printf("%04d.%04d ",Part, Step);
             Step += Bump;
             if (Step > 9999) { Part++; Step = 0; }
          }
       }
       if ((Flag IS TRUE) && (x IS 'U')) {
          if (c != SPACE) continue;
          Flag++; continue;
       }
       putchar(c);
       if (c IS NL) Flag = FALSE;
    }
    if (x IS 'Q') Bag(EOF);
    exit (0);

}

void Bag(int c)
{
   static int i, j, s;

   if (tpf IS 0) {
      tpf = Queue + 127;
      tp1 = tp2 = Queue;
      i = j = s = 0;
   }

   if (c IS EOF) {
      while(tp1 != tp2) { putchar(*tp2++); if (tp2 IS tpf) tp2 = Queue; }
      putchar(NL);
      exit(0);
   }

 /* Do Business */
    if ((c IS CR) || (c IS SUB)) return;
    i++; if (c IS NL) { s = i; c = SPACE; }

 /* Push a CHR$ onto the Circular Queue */
    *tp1++ = c; if (tp1 IS tpf) tp1 = Queue;

 /* Check for Enough to Stream Out */
    if (i < 75) return;
    for (j = s; j > 0; j--) {
       i--; putchar(*tp2++); if (tp2 IS tpf) tp2 = Queue; }
    putchar(NL);

}

void Usage()
{
    fprintf(stderr, "\nUsage:\n");
    fprintf(stderr,
"       CAT [/Option or file list] ---> Stream Text to Stdout.\n");
    fprintf(stderr, "\n/Options:\n");
    fprintf(stderr,
"    /Q ---> Stack Stdin Lines into 75 CHR$ max lines.\n");
    fprintf(stderr,
"    /N ---> Add Line Numbers to Each Stdin Line.\n");
    fprintf(stderr,
"    /T ---> Add TEKTEST Line Numbers to Each Stdin Line.\n");
    fprintf(stderr,
"    /U ---> Remove Line Numbers from Each Stdin Line.\n\n");
    fprintf(stderr, "File List:\n");
    fprintf(stderr,
"    File Names (Wild Cards OK), $pipe or \"-\" for Stdin.\n\n");
    exit(1);
}
