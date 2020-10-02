
/* Printer.c ---> A Cutdown of printer.c on CUG-273 */

#include <stdek.h>
#include <gadgets.h>
#include <conio.h>
#include <bios.h>

int LPrintChr(char ch, int lptnum) { return biosprint(0, ch, --lptnum) & 0x29; }

int LPreset(int lptnum) { return !(biosprint(1, 0, --lptnum) & 0x29); }

int LPready(int lptnum) { return !(biosprint(2, 0, --lptnum) & 0x40); }

int LPrintStr(char *str, int lptnum)
{
    for (lptnum--; *str; str++)
       if (biosprint(0, *str, lptnum) & 0x29) return 0;
    return 1;
}

