

/* Keys --> Report ASCII Value, ScanCode & KeyMask on Key Hit.
 *
 * J.Ekwall
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update:  6 June 91/EK
 */

#include <stdio.h>
#include <io.h>
#include <bios.h>
#include <stdek.h>

/* Set Up Keyboard ScanCode/Character Union */
union Keypunch {
   int C;
   BYTE Chr[2];
} KeyStroke;

/* Set Up Control Chr Lookup Table */
char *CtrlCode[] = {
   "NULL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT", "NL",
   "VT", "FF", "CR", "SO", "SI", "DLE", "XON", "DC2", "XOFF", "DC4", "NAK",
   "SYN", "ETB", "CAN", "EOM", "SUB", "ESC", "FS", "GS", "RS", "US", NULL };

/* Set Up ScanCode Lookup Table */
char *ScanCode[] = {
   "ALT_BS ",   "SHIFT_HT",     "ALT_Q",        "ALT_W",        "ALT_E",
   "ALT_R",     "ALT_T",        "ALT_Y",                "ALT_U",        "ALT_I",
   "ALT_O",     "ALT_P",        "ALT_LBRACE ",  "ALT_RBRACE ",  "ALT_RTN        ",
   "EK157",     "ALT_A",        "ALT_S",                "ALT_D",        "ALT_F",
   "ALT_G",     "ALT_H",        "ALT_J",                "ALT_K",        "ALT_L",
   "ALT_SEMI ", "ALT_QUOTE ",   "ALT_TILDE ",   "EK170",        "ALT_BKSL ",
   "ALT_Z",     "ALT_X",        "ALT_C",                "ALT_V",        "ALT_B",
   "ALT_N",     "ALT_M",        "ALT_COMMA ",   "ALT_DOT ",     "ALT_QUERY ",
   "EK182",     "ALT_STAR ",    "EK184",        "EK185",        "EK186",
   "F1",        "F2",                   "F3",                   "F4",                   "F5",
   "F6",        "F7",                   "F8",                   "F9",                   "F10",
   "EK197",     "EK198",        "HOME",         "UP",           "PGUP",
   "ALT_MINUS ","BWD",          "EK204 ",       "FWD",          "ALT_PLUS ",
   "END",       "DN",           "PGDN", "INS",          "DEL_KEY",
   "SHIFT_F1",  "SHIFT_F2",     "SHIFT_F3",     "SHIFT_F4",     "SHIFT_F5",
   "SHIFT_F6",  "SHIFT_F7",     "SHIFT_F8",     "SHIFT_F9",     "SHIFT_F10",
   "CTL_F1",    "CTL_F2",               "CTL_F3",       "CTL_F4",       "CTL_F5",
   "CTL_F6",    "CTL_F7",               "CTL_F8",       "CTL_F9",       "CTL_F10",
   "ALT_F1",    "ALT_F2",               "ALT_F3",        "ALT_F4",      "ALT_F5",
   "ALT_F6",    "ALT_F7",               "ALT_F8",       "ALT_F9",       "ALT_F10",
   "EK242",     "CTL_BWD",              "CTL_FWD",      "CTL_END",              "CTL_PGDN",
   "CTL_HOME",  "ALT_1",        "ALT_2",        "ALT_3",        "ALT_4",
   "ALT_5",     "ALT_6",        "ALT_7",                "ALT_8",        "ALT_9",
   "ALT_0",     "ALT_DASH",     "ALT_EQUALS",           "CTL_PGUP",     "F11 ",
   "F12 ",      "SHIFT_F11 ",   "SHIFT_F12 ",   "CTL_F11 ",     "CTL_F12        ",
   "ALT_F11 ",  "ALT_F12 ",     "CTL_UP ",              "CTL_DASH ",    "CTL_5 ",
   "CTL_PLUS ", "CTL_DN ",      "CTL_INS ",             "CTL_DEL_KEY ", "CTL_HT ",
   "CTL_SL ",   "CTL_STAR ",    "ALT_HOME ",    "ALT_UP ",      "ALT_PGUP ",
   "EK282 ",    "ALT_BWD ",     "EK284 ",       "ALT_FWD ",     "EK286 ",
   "ALT_END ",  "ALT_DN ",      "ALT_PGDN ",    "ALT_INS ",     "ALT_DEL_KEY ",
   "ALT_SL ",   "ALT_HT ",      "ALT_ENTER ",   NULL };

main (int argc, char *argv[])
{
    int c, KeyCode, Silent = FALSE;
    char *tp1;

    if (argc IS 2 && *argv[1] IS SLASH && toupper(argv[1][1]) IS 'S')
       Silent = TRUE;
    else {
       printf("\nKey Stroke Test.  Hit [Esc] to Exit.  Hit Any Key.\n\n");
       printf("\tAscii\tScanCode\tKeyMask\t\tKeys.h\t\tSymbol\n");
    }

    do {
       KeyStroke.C = bioskey(0);
       if ((c = KeyStroke.Chr[0]) != 0) KeyCode = c;
       else KeyCode = KeyStroke.Chr[1] | 128;
       if (Silent) exit(KeyCode);
       printf("\t%u\t%u\t\t%d\t\t%d\t",
       KeyStroke.Chr[0], KeyStroke.Chr[1], bioskey(2), KeyCode);
       if (c) {
          if(c  < SPACE)
             printf("\t%s\n", CtrlCode[c]);
          else
             printf("\t\"%c\"\n", c);
       } else if ((c = KeyStroke.Chr[1]) >= 14 && c < 167) {
          tp1 = ScanCode[c-14];
          if (tp1[strlen(tp1)-1] IS SPACE) printf("NewKey");
          printf("\t%s\n", ScanCode[c-14]);
       } else if (c IS 1) printf("NewKey\tALT_ESC\n");
    } while (KeyStroke.Chr[0] != ESC);
}

