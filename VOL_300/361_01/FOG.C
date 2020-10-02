
/* FOG---> A Text Covering Scheme
 *
 * J.Ekwall 29 August 89
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Autorized.
 *
 * User Assumes All Risks/Liabilities.
 */

#include <stdio.h>
#include <ctype.h>
#include <io.h>
#include <time.h>
#include <stdlib.h>
#include <stdek.h>
#include <string.h>

/* Declare Constants  */
#define HIBIT3  243

/* Declare ProtoTypes */
void Usage(void);

char *Documentation[] = {
    "",
    "Usage:",
    "      Fog [options] [file]  --> a Full Ascii Cypher.",
    "",
    "Options:",
    "       /n ---> Use Key n where n is 0-9.",
    "       /D ---> Decode.",
    "       /H ---> An Historical Note.",
    "       /N ---> Neaten up the output w/ Spaces and LineFeeds.",
    "       /S ---> Silent.  Verbose is Default when Redirected.",
    "",
    " --> NOT APPROVED for Classified Information. <---",
    "",
    "Last Update: 17 July 91/EK",
    NULL};

char *History[] = {
    "",
"    FOG is based on a KGB trick that totally baffled the CIA in the early",
"60's.  On defecting, the US Station Chief revealed how the KGB had stumpted",
"the experts: Morse Code.",
    "",
"    Morse Code is a Trinary system which, if analyzed using binary",
"techniques, can't be cracked.  The KGB divided the alphabet into three",
"parts, assigning each Trinary bit a random letter drawn from the",
"appropriate group.  This introduces a large \"Noise\" factor.",
    "",
"   FOG uses a similar method except that normal (eight bit) Ascii code",
"is used (6 trinary digits).  Since I had 20 scrambled alphabets handy,",
"I added a touch of Bazeries Cylinders to the show, shifting among them",
"in a round robin.",
    "",
"   The amount of bulk expansion could be as high as 6:1, but generally",
"averages about 3.5:1 because only changed bits are retained and leading",
"zeros are dropped.",
    "",
    NULL };

char *Cylinder[] = {
    "ABDMPFHKCEQLWGOXYSINTRUZJV",
    "WXIKSVRYPZJCMNGDFEHLOBQTAU",
    "SPUVIXBHKEDCLMNFGOQTAJWYZR",
    "KVQLYABCDPZIMJNOSWREHXUFGT",
    "HVWFQBACDJRXYZEGLOMNPKSTIU",
    "UVYLFMZACHJNEWGXDKBPIQROST",
    "ALRUXVDYWQZCFKEIGPHNJMSTBO",
    "QTBUVXOHJKPRGSWAYEDILZCNFM",
    "QUBFNEDAJVWXZGOKSMTRYPHILC",
    "GIJCPNEKBDMOQFLRSATUVWXZYH",
    "BLEFGHIJNAKPQXDWYCTMUZORSV",
    "VHRLIFNUDMQTPJBEACSWXYKZOG",
    "LRTCSMKWXIYJBZAPDFNOUVEGQH",
    "QBNRUXYIDZEPSHTFOMVWJCGKAL",
    "UDILMNQSVAPWXRCFTOJYHBEKGZ",
    "JQDRUWXAPGHMSKFVCOYZBTLINE",
    "IUDLXYQFTNZESHKJMPBAORWGVC",
    "YBZXIDFNOGRPAKJMLTEUSVCHQW",
    "IQDPREBTCKONLSUVWXYMZAHFJG",
    "RXYZCLIASPGNUVDJBEKQMTWFOH",
    NULL };

/* Declare Globals */
int Decode = 0, NeatFlag = 0, Silent = 0;
char **Disk;
FILE *fp = stdin;

/* Declare ProtoTypes */
void DeCode(void);
void EnCode(void);
void Help(char **dp);
void Post(char ch);
void Usage(void);

main (int argc, char *argv[])
{
   int c, i, j, N,  Decode = FALSE, Flag, Nybble;
   char *tp1;

 /* Check Passed Parameters */
    for (Disk = Cylinder; *argv[1] IS SLASH; argc--, argv++) {
       for (tp1 = argv[1] + 1;(c = toupper(*tp1++)) != NULL; ){
          switch (c) {
          case '0': Disk++;
          case '9': Disk++;
          case '8': Disk++;
          case '7': Disk++;
          case '6': Disk++;
          case '5': Disk++;
          case '4': Disk++;
          case '3': Disk++;
          case '2': Disk++;
          case '1': Disk++;     break;
          case 'D': Decode++;   break;
          case  'H': Help(History);
          case 'N': NeatFlag++; break;
          case 'S': Silent++;   break;
          default: Usage();
          }
       }
    }

    /* Check Specified File or Stdin Pipe */
    if ((argc IS 1) && !INFLOW_EXISTS) Usage();
    if (argc IS 2)
       if ((fp = fopen(argv[1], "r")) IS NULL) { perror(argv[1]); exit(3); }

 /* Do Business */
    if (!OUTFLOW_EXISTS) Silent++;
    if (!Silent) fprintf(stderr, "\n");
    if (Decode) { NeatFlag = 0; DeCode(); } else EnCode();
    if (!Silent) fprintf(stderr, "\n"); if (NeatFlag) putchar(NL);
}

void DeCode(void)
{
    int c, ch = 0, i, Puka = 0;
    char *tp1;

    while ((c = fgetc(fp)) != EOF) {
       if (c IS SPACE || c IS NL) continue;
       if ((tp1 = strchr(*Disk, c)) IS NULL) exit(1);
       i = tp1 - (*Disk); ch = ch * 3 + i % 3;
       if (i > 20) {
          Puka  = (ch ^= Puka); Disk++;
          putchar(ch); if (!Silent) fprintf(stderr, "%c", ch); ch = 0;
          if (*Disk IS NULL) Disk = Cylinder;
       }
    }
}

void EnCode(void)
{
    int c, ch, i, j, Flag, Puka = 0;

    randomize();
    for ( ; (ch = getc(fp)) != EOF; Disk++) {
       if (*Disk IS NULL) Disk = Cylinder;
       c = Puka ^ ch; Puka = ch;
       for (i = HIBIT3, Flag = 0; i > 1; i /= 3) {
          if ((j = c / i) IS 0 && !Flag) continue;
          c %=  i; Flag++; Post((*Disk)[j + 3 * random(7)]);
       }
       Post((*Disk)[c + 21]);
    }
}

void Help(char **dp)
{
    for ( ; *dp; dp++) fprintf(stderr,"%s\n", *dp);
    exit(0);
}

void Post(char ch)
{
    static int N = 0;

    putchar(ch); if (!Silent) putch(ch); if (++N % 5) return;
    if (!Silent)  if (N IS 60) fprintf(stderr, "\n"); else putch(SPACE);
    if (NeatFlag) if (N IS 60) putchar(NL); else putchar(SPACE);
    if (N IS 60) N = 0;
}


void Usage(void)
{
    char   **dp = Documentation;

    for ( ; *dp; dp++) fprintf(stderr,"%s\n", *dp);
    if (OUTFLOW_EXISTS) { rewind(stdout); putchar(DOS_EOF); }
    exit(1);
}

