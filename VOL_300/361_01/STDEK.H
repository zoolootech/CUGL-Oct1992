

/* STDEK.H ---> EK's List of Magic Numbers & Macros */

#ifndef STDEK

#define STDEK

#include <ctype.h>
#define IS      ==
#define EVER    ;;              /* for(EVER) is an Infinite Loop */
#define FALSE   0
#define TRUE    1
#define OK      0               /* For exit(OK) or exit(!OK) */
#define PIPE    "\\STD "
typedef unsigned char   BYTE;

/* Define ASCII CTL CHR$ */
#define SOH     1       /* ^A is RAM-Page NL */
#define STX     2       /* ^B is RAM-Page UndLn Marker */
#define ETX     3       /* ^C is RAM-Page Cmd Esc & Unix/DOS INTR */
#define EOT     4       /* ^D is Unix EOF/Kill Shell Marker */
#define ENQ     5       /* ^E is ENQuire */
#define ACK     6       /* ^F is ACKnowledge */
#define BEL     7       /* ^G is Bell or Beep */
#define BELL    7       /* ^G is Bell or Beep */
#define BS      8       /* ^H is BackSpace */
#define RUBOUT  8
#define HT      9       /* ^I is TAB right (normal TAB action) */
#define TAB     9       /* ^I is TAB right (normal TAB action) */
#define LF      10      /* ^J is LineFeed */
#define NL      10      /* Unix New Line */
#define VT      11      /* ^K is TAB Down (rare) */
#define FF      12      /* ^L is FormFeed */
#define CR      13      /* ^M is Carriage Return */
#define SO      14      /* ^N is Shift Out (to Alt CHR Set) */
#define SI      15      /* ^O is Shift In  (to Nml CHR Set) */
#define DLE     16      /* ^P is Data Link Escape (BiSync) */
#define DC1     17      /* ^Q is Device Control One */
#define XON     17      /* Switch ON (Paper Tape Reader) */
#define DC2     18      /* ^R is Device Control 2 */
#define DC3     19      /* ^S is Device Control 3 */
#define XOFF    19      /* Switch OFF (Paper Tape Reader) */
#define DC4     20      /* ^T is Device Control 4 */
#define NAK     21      /* ^U is Negative ACK */
#define SYN     22      /* ^V is Sync BiSync Link Clock */
#define ETB     23      /* ^W is End Text Block (BiSync) */
#define CAN     24      /* ^X is Cancel */
#define EOM     25      /* ^Y is End Of Message */
#define SUB     26
#define DOS_EOF 26      /* ^Z is CP/M and DOS EOF Marker */
#define ESC     27      /* ^[ is ESCape */
#define FS      28      /* ^\ is File Separator & C-Kermit Cmd Escape */
#define GS      29      /* ^] is Group Separator & MS-Kermit Cmd Escape */
#define RS      30      /* ^^ is Record Separator */
#define US      31      /* ^_ is Unit Separator */

/* Define ASCII Symbols */
#define SPACE   32
#define BANG    33      /* ! */
#define QUOTE   34
#define SHARP	35
#define DOLLAR  36
#define DIT     39      /* ' */
#define STAR    42      /* '*' */
#define COMMA   44
#define DASH    45
#define DOT     46
#define SLASH   47      /* '/' */
#define COLON   58
#define SEMI    59
#define QUERY   63      /* '?' */
#define BKSL    92      /* '\' */
#define UNDER   95      /* '_' */
#define ACCENT  96
#define LBRACE 123
#define BAR    124
#define RBRACE 125
#define DEL    127
#define BOX    219      /* Û */

/* Define Macros */
#define INFLOW_EXISTS           (isatty(0) IS FALSE)
#define OUTFLOW_EXISTS          (isatty(1) IS FALSE)
#define Pause2Read              (ungetch(getch()))

#endif
