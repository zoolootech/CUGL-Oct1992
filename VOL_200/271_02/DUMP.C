/*
**                HEX FILE DUMP UTILITY
**   Displays any file in hex and character representation
**   in color or monochrome, depending upon type of video
**   card installed.  Displays 20 lines per screen and waits
**   for a keypress.
**
**   Rev 1.00   8-09-87 for Turbo-C  S.E. Margison
**
**   As distributed, this program requires (for compilation):
**     "Steve's Turbo-C Library" version 1.30 or later
**   which may be obtained without registration from many Bulletin
**   Board Systems including:
**      Compuserve IBMSW
**      Cul-De-Sac (Holliston, MA.)
**      GEnie
**   and software library houses including:
**      Public (Software) Library (Houston, TX.)
**
**   or by registration:
**      $10 for Docs, Small Model Library
**      $25 for Docs, C, S, M, L, H libraries, and complete library source
**              in C and Assembler
**     Steven E. Margison
**     124 Sixth Street
**     Downers Grove, IL, 60515
**
**
** Program requires at least one parameter, the name of the file to dump.
** A second parameter is taken to be an offset value to start dumping,
** default is the start of the file.  If a third value is present it is
** taken to be a hex byte value to highlite whenever encountered in the file.
**
** More than a utility, this is a good demonstration of the direct
** video subroutines and video page switching mechanism (for CGA cards).
** Look upon this as a tutorial as well as a handy utility.
*/

#include <stdio.h>
#include <ctype.h>
#include <smdefs.h>
#include <screen.h>
#include <keys.h>

int adrclr,    /* address field color */
    hexclr,    /* hex data color */
    highclr,   /* highlite color */
    prompt,    /* prompt line color */
    ascclr;    /* ascii color */

char tbuf[82];
int buffer[16];
unsigned long offset = 0;
FILE *f;

int mono, cpage, eoflag;

main(argc,argv)
int argc;
char *argv[];
{
   int i, lines, view, vflag;

   eoflag = vflag = NO;      /* default highlite off */

   if ((argc < 2) or (argc > 4))
      error("Use: dump file [starting offset in hex] [highlite byte]");

   if((f = fopen(argv[1], "rb")) is NULL) cant(argv[1]);

   if(argc > 2) {
      sscanf(argv[2], "%lx", &offset);
      fseek(f, offset, 0);
      }

   if (argc is 4) {  /* get the value to highlite */
      sscanf(argv[3], "%02x", &view);
      vflag = YES;
      }

   i = stuff(0);   /* check video type */
   if(i is MONO) {
      mono = TRUE;
      prompt = BLINKING;
      adrclr = WHITE;
      hexclr = HIWHITE;
      ascclr = WHITE;
      highclr = HIGHBLINK;
      }
   else {         /* for CGA cards */
      vmode(CLR80);
      mono = FALSE;
      cpage = 1;       /* we'll write to page 1 first */
      prompt = BROWN;
      adrclr = LTGREEN;
      hexclr = YELLOW;
      ascclr = LTCYAN;
      highclr = HIWHITE;
      }

   dvid_init();    /* use direct video access routines */
   dvid_cls();
   dvid_move(0,0);
   dvid_flush();
   if(!mono) {
      dvid_setpage(0, 1);       /* set page 0 as display page */
      dvid_setpage(1, 0);       /* but set page 1 as writing page */
      }
   cursor_style(5, 0, 0);    /* kill the cursor */
   lines = 20;
   for ever {
        /* if this were a level 1 file using read() and open()
        ** the program would be faster.  Just a hint! */
      for (i = 0; i < 16; i++) buffer[i] = fgetc(f);

      if(buffer[0] is -1) {
         if(lines is 20) break;    /* we must've ended on even page */
         eoflag = TRUE;
         goto alldone;
         }
      dvid_attrib(adrclr);   /* set address color */
      sprintf(tbuf, "%04lx: ",offset);  /* display address */
      dvid_puts(tbuf);
      dvid_attrib(hexclr);   /* set hex value color */
      for (i = 0; i < 16; i++) {
         if (buffer[i] isnot -1) {
            if(vflag and (buffer[i] is view)) {  /* highlite this byte? */
               dvid_attrib(highclr);
               sprintf(tbuf, "%02x ", buffer[i]);
               dvid_puts(tbuf);
               dvid_attrib(hexclr);
               }
            else {
               sprintf(tbuf, "%02x ", buffer[i]);
               dvid_puts(tbuf);
               }
            }
         else dvid_puts("   ");
         }
      dvid_attrib(ascclr);  /* display ascii data color */
      dvid_puts("   ");
      for (i = 0; i < 16; i++) {
         if (buffer[i] isnot -1) {
            if (!isprint(buffer[i])) buffer[i] = '.';
            dvid_putchr(buffer[i]);
            }
         else dvid_putchr(' ');
         }
      dvid_putchr('\n');
      if(lines-- is 0) {
         alldone:
         dvid_attrib(prompt);
         dvid_say(22, 10, "Press any key to continue, ESCape to quit...");
         if(!mono) dvid_setpage(cpage, 1);   /* display this page */
         if(getkey() is ESC) {
            dvid_setpage(0, 1);        /* return to page 0 */
            if(!mono) cursor_style(1, 0, 0);  /* restore cursor */
            else cursor_style(2, 0, 0);
            aabort(1);
            }
         if(eoflag) break;
         cpage++;     /* bump page #, maintain 0-3 */
         cpage &= 3;

         if(!mono) dvid_setpage(cpage, 0);  /* set the next page for write */
         dvid_cls();
         dvid_flush();
         lines = 20;
         }
      offset += 16;
      }  /* end of forever loop */

   dvid_setpage(0, 1);   /* restore page 0 */
   dvid_cls();
   dvid_attrib(prompt);
   dvid_say(22, 10, "End of File Encountered");
   if(!mono) cursor_style(1, 0, 0);  /* restore cursor */
   else cursor_style(2, 0, 0);
   dvid_flush();
   dvid_done();
}


dvid_puts(buf)
char *buf;
{
   while(*buf isnot NULL) dvid_putchr(*buf++);
}

