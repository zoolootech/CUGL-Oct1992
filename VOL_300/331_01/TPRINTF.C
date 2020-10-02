/*
HEADER:     CUG999.14;
 */

/* This function is used only for code development.  It need not be
 * included in the link proceedure if not called.
 *
 * The text will appear on the second line in reverse field.
 * The second line is reserved for the diagnostic data on the first call.
 * Execution is suspended until the next keystroke so that the output
 * can be read in case of back-to-back outputs.
 *
 * The variables a1-a8 are not simply related to the actual parameters.
 * The code is a trick which copies the first 8 words of the argument
 * list to sprintf.  The technique is moderately portable, but is outside
 * the language defintion and is not used in the application code to
 * insure portability.  tprintf is used exactly like printf, except that
 * no newlines are needed and the number of arguments can't exceed 8 tyoe
 * integer or 4 type long.
 *
 * far pointers can be printed with tprintf as %lx integers.  They appear
 * in the form SEG:OFFSET.  The absolute memory address of a near pointer
 * can be displayed by casting the pointer as type far.
 *
 * Another method of printing diagnostic output is to set topline to a value
 * greater than 1 to reserve lines at the top, then use gotoxy() to position
 * the cursor, (gotoxy(0,1) for the second line), then use printf in the
 * conventional manner.  The last printf must be followed by a call to
 * resetpcursor.
 */

#include "ged.h"

/* print and hold a short time */
tprintf(str,a1,a2,a3,a4,a5,a6,a7,a8)
char *str, *a1, *a2, *a3, *a4, *a5, *a6,*a7,*a8;
{
    char buf[81];
    int j;
    long i;

/* reserve an additional line at the top.  help display assumed off */
    if(topline == 1) {
        topline = 2;
        calp();
/* a call to putpage here would be proper but might compiicate troubleshooting
 * too much.
 */
    }

    for (j=0; j<80; j++)
        buf[j]=' ';
    buf[79]=0;
    scr_aputs(0,1,buf,ATTR0);
    sprintf(buf,str,a1,a2,a3,a4,a5,a6,a7,a8);
    scr_aputs(0,1,buf,ATTR2);
    for (i = 0; i < 25000L; i++);
    return;
}

/* print and hold */

hprintf(str,a1,a2,a3,a4,a5,a6,a7,a8)
char *str, *a1, *a2, *a3, *a4, *a5;
{
    int i;
i = chkbuf();
    tprintf(str,a1,a2,a3,a4,a5,a6,a7,a8);
/* Wait for any key. */
    while (chkbuf() == i)
        ;
    inbufp =0;
    scr_aputs(78,1," ",ATTR2);  /* acknowledge the key. it is used normally. */
    return;
}





